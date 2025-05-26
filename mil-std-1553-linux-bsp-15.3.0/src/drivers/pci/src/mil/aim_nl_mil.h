/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_nl_mil.h
 *
 *  This header contains declarations for 1553
 *  specific netlink interface implementations
 *
 */

#ifndef AIM_NL_MIL_H_
#define AIM_NL_MIL_H_


#include "aim_nl.h"
#include "aim_pci_device_mil.h"




/*! \enum aim_mil_nl_mc_group_ids
 * Enumeration of all aim_mil generic netlink multicast group IDs
 */
enum aim_mil_nl_mc_group_ids
{
    AIM_MIL_NL_GRP_ID_IRQ_EVENT = 0,    /*!< Group for interrupt events */
    __AIM_MIL_NL_GRP_MAX,
};




/*! \brief Set up 1553 specific netlink interface
 *
 * This function sets up the 1553 specific netlink operations
 * and multicast groups in the AIM device generic netlink interface.
 * @param device The 1553 device to set up netlink interface for.
 */
void aim_nl_setup_mil(struct aim_pci_mil_device* device);




#endif /* AIM_NL_MIL_H_ */
