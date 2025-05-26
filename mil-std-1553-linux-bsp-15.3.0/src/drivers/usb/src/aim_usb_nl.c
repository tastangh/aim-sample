/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_nl.c
 *
 *  This file contains definitions used for creating
 *  generic netlink interfaces to user space
 *
 */


#include <linux/version.h>
#include "aim_usb_nl.h"
#include "aim_usb_interface.h"
#include "aim_usb_debug.h"




int aim_usb_nl_setup(struct aim_usb_interface* intf)
{
    struct aim_usb_nl_setup* nl_setup = NULL;
    int ret = 0;

    BUG_ON(!intf);

    nl_setup = &intf->nl_setup;

    /* Set-up default netlink family for the device.
     * It uses the name of the device and will generate its own ID
     */
    snprintf(nl_setup->family.name, sizeof(nl_setup->family.name), "%s%d", KBUILD_MODNAME, intf->interface->minor );

    aim_usb_intf_info(intf, "Registering generic netlink family: %s", nl_setup->family.name);
    aim_usb_intf_dbg(intf, "Family has %zu operations", nl_setup->num_ops);
    aim_usb_intf_dbg(intf, "Family has %zu multicast groups", nl_setup->num_groups);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0))
    nl_setup->family.module   = THIS_MODULE;
    nl_setup->family.ops      = nl_setup->ops;
    nl_setup->family.n_ops    = nl_setup->num_ops;
    nl_setup->family.mcgrps   = nl_setup->groups;
    nl_setup->family.n_mcgrps = nl_setup->num_groups;

    ret = genl_register_family(&nl_setup->family);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
    ret = _genl_register_family_with_ops_grps(&nl_setup->family, nl_setup->ops, nl_setup->num_ops,
                                              nl_setup->groups, nl_setup->num_groups);
#else
    {
        size_t i = 0;
        struct genl_multicast_group* dev_private_groups = NULL;

        ret = genl_register_family(&nl_setup->family);
        if(ret)
        {
            return ret;
        }

        do
        {
            for(i = 0; i < nl_setup->num_ops; i++)
            {
                ret = genl_register_ops(&nl_setup->family, &nl_setup->ops[i]);
                if(ret)
                {
                    break;
                }
            }

            if(ret)
            {
                break;
            }

            /* Before Linux 3.13, we need to dynamically allocate the genl_multicast_group structures for each device.
             * Otherwise the repeated usage of these structures by multiple devices will lead to problems
             */
            dev_private_groups = kzalloc(sizeof(struct genl_multicast_group) * nl_setup->num_groups,
                                         GFP_KERNEL);

            for(i = 0; i < nl_setup->num_groups; i++)
            {
                /* Copy the group name to the dynamically allocated group specification */
                snprintf(dev_private_groups[i].name, sizeof(dev_private_groups[i].name), nl_setup->groups[i].name);
                ret = genl_register_mc_group(&nl_setup->family, &dev_private_groups[i]);
                if(ret)
                {
                    break;
                }
            }

            /* Now replace static group specifications with the dynamically alocated ones */
            nl_setup->groups = dev_private_groups;

        }while(0);

        if(ret)
        {
            genl_unregister_family(&nl_setup->family);
        }
    }
#endif

    if(ret)
    {
        aim_usb_intf_err(intf, "Failed to register netlink interface");
    }

    return ret;
}


void aim_usb_nl_release(struct aim_usb_interface* intf)
{
    struct aim_usb_nl_setup* nl_setup = NULL;

    BUG_ON(!intf);

    nl_setup = &intf->nl_setup;

    if(nl_setup->family.id != 0)
    {
        aim_usb_intf_info(intf, "Unregistering generic netlink family %s (ID: %d)", nl_setup->family.name,
                          nl_setup->family.id);

        genl_unregister_family(&nl_setup->family);

        #if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)

        /* Before kernel 3.13, the genl_multicast_group structure have been dynamically allocated
         * and need to be freed.
         */
        if(nl_setup->groups)
        {
            kfree(nl_setup->groups);
            nl_setup->groups = NULL;
        }

        #endif

    }
}




static int __inline__ aim_usb_nl_get_genl_group_id(struct aim_usb_interface* intf, int family_id)
{
    BUG_ON(!intf || family_id >= intf->nl_setup.num_groups);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
    return family_id;
#else
    return intf->nl_setup.groups[family_id].id;
#endif
}


int aim_usb_nl_mc_send_sba(struct aim_usb_interface* intf, int family_group, int command, int attr,
                           size_t attrSize, void* attrData)
{
    struct sk_buff* skb = NULL;
    void* header = NULL;
    int ret = 0;
    struct aim_usb_nl_setup* nl_setup = NULL;
    int genl_group = -1;

    BUG_ON(!intf || !attrData);

    nl_setup = &intf->nl_setup;

    genl_group = aim_usb_nl_get_genl_group_id(intf, family_group);

    aim_usb_intf_dbg(intf, "Sending binary attribute to mc group %d. (cmd: %d attr: %d size: %zu)", genl_group, command,
                     attr, attrSize);

    /* Allocate generic netlink socket buffer that can hold one attribute
     * with the given attribute size
     */
    skb = genlmsg_new(nla_total_size(attrSize), GFP_ATOMIC);
    if(!skb)
    {
        aim_usb_intf_err(intf, "Failed to allocate socket buffer");
        return -ENOMEM;
    }

    do
    {
        header = genlmsg_put(skb, 0, 0, &nl_setup->family, 0, command);
        if(!header)
        {
            aim_usb_intf_err(intf, "Failed to create nl message header");
            ret = -EFAULT;
            break;
        }

        ret = nla_put(skb, attr, attrSize, attrData);
        if(ret)
        {
            aim_usb_intf_err(intf,  "Failed to append nl attribute data");
            break;
        }

        genlmsg_end(skb, header);

    }while(0);

    if(ret)
    {
        kfree_skb(skb);
        return ret;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
    ret = genlmsg_multicast(&nl_setup->family, skb, 0, genl_group, GFP_ATOMIC);
#else
    ret = genlmsg_multicast(skb, 0, genl_group, 0);
#endif

    if(ret)
    {
        aim_usb_intf_dbg(intf, "Failed to multicast nl message with error %d", ret);
    }

    /* -ESRCH is not treated as an error, as it means no one is listening */
    return ret == -ESRCH ? 0 : ret;
}
