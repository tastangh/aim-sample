/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ioctl.h
 *
 * This header file contains protocol independent
 * declarations used for the IOCTL interface of the
 * AIM PCI driver
 *
 */

#ifndef AIM_IOCTL_H_
#define AIM_IOCTL_H_


#include "aim_ioctl_interface.h"
#include "aim_pci_device.h"
#include "aim_dma.h"



/*! \struct aim_pci_ioctl_ops
 *
 * This structure shall be used for defining protocol specific IOCTL handler functions
 */
struct aim_pci_ioctl_ops
{
    long (*target_command)(struct aim_pci_device* deviceInfo,                   /*!< handler for target commands */
                           struct aim_target_command __user* parameters);
    long (*target_command_direct)(struct aim_pci_device* deviceInfo,            /*!< handler for direct target commands using DMA */
                                  struct aim_target_command_direct* parameters,
                                  struct aim_dma_buffer* dmaBuffers);
};




/*! \brief Protocol independent IOCTL handler function
 *
 * This function is the entry point for all IOCTLs.
 * It will check the given IOCTL code and the parameter size and
 * the process the IOCTL
 *
 * @param file file pointer the IOCTL was issued on
 * @param code the IOCTL command code
 * @param arguments IOCTL specific parameters
 */
long aim_ioctl(struct file* file, unsigned int code, unsigned long arguments);




#endif /* AIM_IOCTL_H_ */
