/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_irq_mil.h
 *
 *  This header file contains 1553 specific declarations
 *  used for interrupt handling
 *
 */

#ifndef AIM_IRQ_MIL_H_
#define AIM_IRQ_MIL_H_


#include "aim_pci_device.h"




/*! \brief 1553 specific handler for BIU interrupts
 *
 * @param[in] device device to handle BIU interrupts for
 * @param[in] biuID ID of BIU to handle interrupt of (zero-based)
 */
void aim_pci_mil_biu_irq(struct aim_pci_device* device, int biuID);


/*! \brief Handles ASP interrupts on AYX boards
 *
 * This function will just forward the interrupt info to user space via netlink
 * @param device device to handle ASP interrupts of
 */
void aim_pci_mil_asp_irq(struct aim_pci_device* device);




#endif /* AIM_IRQ_MIL_H_ */
