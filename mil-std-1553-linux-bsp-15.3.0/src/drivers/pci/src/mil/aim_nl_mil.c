/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_nl_mil.c
 *
 *  This header contains declarations for 1553
 *  specific netlink interface implementations
 *
 */


#include "aim_nl_mil.h"
#include "aim_nl_interface_mil.h"



/*! \struct aim_mil_nl_ops
 *
 * 1553 specific generic netlink operations.
 * Empty, as driver does not receive any netlink messages
 */
static struct genl_ops aim_mil_nl_ops[] = {};


/*! \struct aim_mil_nl_mc_groups
 * Generic netlink multicast groups of the driver
 */
static struct genl_multicast_group aim_mil_nl_mc_groups[__AIM_MIL_NL_GRP_MAX] =
{
    [AIM_MIL_NL_GRP_ID_IRQ_EVENT] = {.name = AIM_MIL_NL_IRQ_EVENT_GROUP}, /*!< Group for sending interrupt event notifications */
};




void aim_nl_setup_mil(struct aim_pci_mil_device* device)
{
    struct aim_pci_device* aimDevice = NULL;
    struct aim_nl_setup* nl_setup = NULL;

    BUG_ON(!device);

    aimDevice = &device->aimDevice;
    nl_setup = &aimDevice->nl_setup;

    nl_setup->family.version = AIM_MIL_NL_VERSION;
    nl_setup->family.maxattr = AIM_MIL_NL_A_MAX;

    nl_setup->ops = aim_mil_nl_ops;
    nl_setup->num_ops = ARRAY_SIZE(aim_mil_nl_ops);

    nl_setup->groups = aim_mil_nl_mc_groups;
    nl_setup->num_groups = ARRAY_SIZE(aim_mil_nl_mc_groups);

}
