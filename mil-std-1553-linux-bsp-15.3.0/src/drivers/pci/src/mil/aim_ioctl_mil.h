/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ioctl_mil.h
 *
 *  This header file contains declarations for 1553
 *  specific IOCTLs.
 *
 */

#ifndef AIM_IOCTL_MIL_H_
#define AIM_IOCTL_MIL_H_


#include "aim_ioctl_interface.h"
#include "aim_pci_device.h"



/*! \brief Handles the target command IOCTL for 1553l boards
 *
 * This function processes a target command either in driver or on the device's ASP.
 * \param aimDevice the device the command is processed for
 * \param parameters the target command parameters
 * \return returns 0 on success, an errno code otherwise
 */
long aim_handle_target_cmd_mil(struct aim_pci_device* aimDevice, struct aim_target_command __user* parameters);






#endif /* AIM_IOCTL_FC_H_ */
