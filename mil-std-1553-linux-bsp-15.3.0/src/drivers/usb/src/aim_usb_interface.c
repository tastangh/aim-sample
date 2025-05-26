/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_interface.c
 *
 */


#include <linux/slab.h>
#include "aim_usb_interface.h"
#include "aim_usb_hw.h"




/*! \def com_channel_id_to_index
 *
 * Converts an USB communication channel ID to array index for accessing it
 */
#define com_channel_id_to_index(channel_id) \
    (channel_id)




int aim_usb_interface_init(struct aim_usb_interface* aim_interface, struct aim_usb_interface_data* data,
                           struct usb_interface* interface)
{
    int err;

    aim_interface->dev = &interface->dev;
    aim_interface->intf_ops = data->intf_ops;
    aim_interface->ioctl_ops = data->ioctl_ops;
    aim_ref_init(&aim_interface->ref_count);
    aim_interface->platform = data->platform;
    aim_interface->interface = usb_get_intf(interface);
    aim_interface->protocol = data->protocol;

    mutex_init(&aim_interface->io_mutex);

    err = aim_usb_hw_init(aim_interface);
    if(err)
    {
        aim_usb_intf_err(aim_interface, "Failed to initialize hardware");
        return err;
    }

    return err;
}

int aim_usb_interface_start(struct aim_usb_interface* aim_intf)
{
    int err;

    BUG_ON(!aim_intf || !aim_intf->intf_ops);

    err = 0;
    do
    {
        err = aim_usb_hw_start(aim_intf);
        if(err)
        {
            aim_usb_intf_err(aim_intf, "Failed to start hardware");
            break;
        }

        /* Check if protocol specific start-up function is available */
        if(aim_intf->intf_ops->start)
        {
            err = aim_intf->intf_ops->start(aim_intf);
            if(err)
            {
                aim_usb_intf_err(aim_intf, "Protocol specific start-up failed");
                break;
            }
        }
    }while(0);

    if(err)
    {
        aim_usb_interface_stop(aim_intf);
    }

    return err;
}


void aim_usb_interface_stop(struct aim_usb_interface* aim_intf)
{
    BUG_ON(!aim_intf || !aim_intf->intf_ops);

    aim_usb_intf_dbg(aim_intf, "Stopping device");

    /* Check if protocol specific stop function is available */
    if(aim_intf->intf_ops->stop)
    {
        aim_intf->intf_ops->stop(aim_intf);
    }

    aim_usb_hw_stop(aim_intf);
}


struct aim_usb_com_channel* aim_usb_interface_create_com_channel(struct aim_usb_interface* intf, int id,
                                                                 struct usb_host_endpoint* cmd_ep,
                                                                 struct usb_host_endpoint* response_ep)
{
    struct aim_usb_com_channel* com_channel;
    int err;

    BUG_ON(!intf || !cmd_ep || !response_ep);

    aim_usb_intf_dbg(intf, "Creating USB com channel with ID %d. (CMD EP addr 0x%x, RESP EP addr: 0x%x)",
                     id, cmd_ep->desc.bEndpointAddress, response_ep->desc.bEndpointAddress);

    if(id < 0 || id >= AIM_USB_MAX_COM_CHANNELS)
    {
        return ERR_PTR(-EINVAL);
    }

    if(intf->com_channels[com_channel_id_to_index(id)])
    {
        return ERR_PTR(-EBUSY);
    }

    com_channel = kmalloc(sizeof(struct aim_usb_com_channel), GFP_KERNEL);
    if(!com_channel)
    {
        aim_usb_intf_err(intf, "Failed to allocate USB com channel");
        return ERR_PTR(-ENOMEM);
    }

    err = aim_usb_com_channel_init(com_channel, intf, cmd_ep, response_ep);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to initialize USB com channel");
        kfree(com_channel);
        return ERR_PTR(err);
    }

    intf->com_channels[com_channel_id_to_index(id)] = com_channel;

    return com_channel;
}


struct aim_usb_com_channel* aim_usb_interface_get_com_channel(struct aim_usb_interface* intf, int id)
{
    int index;

    BUG_ON(!intf);

    index = com_channel_id_to_index(id);

    if(index < 0 || index >= AIM_USB_MAX_COM_CHANNELS || !intf->com_channels[index])
    {
        aim_usb_intf_err(intf, "Can't find communication channel with ID %d", id);
        return ERR_PTR(-EINVAL);
    }

    return intf->com_channels[index];
}


struct aim_usb_int_ep* aim_usb_interface_create_int_ep(struct aim_usb_interface* intf, struct usb_host_endpoint* ep)
{
    int err;

    BUG_ON(!intf || !ep);

    aim_usb_intf_dbg(intf, "Creating USB interrupt endpoint.");

    if(intf->interrupt_ep)
    {
        return ERR_PTR(-EBUSY);
    }

    intf->interrupt_ep = kmalloc(sizeof(struct aim_usb_int_ep), GFP_KERNEL);
    if(!intf->interrupt_ep)
    {
        aim_usb_intf_err(intf, "Failed to allocate interrupt endpoint");
        return ERR_PTR(-ENOMEM);
    }

    err = aim_usb_int_ep_init(intf->interrupt_ep, intf, ep);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to allocate interrupt endpoint");
        return ERR_PTR(err);
    }

    return intf->interrupt_ep;
}

