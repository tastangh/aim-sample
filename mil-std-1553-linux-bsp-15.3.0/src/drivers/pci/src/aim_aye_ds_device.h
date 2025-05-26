/* SPDX-FileCopyrightText: 2020-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aye_ds_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for APE1553-1/2-DS (Digital Scope / MIL-Scope)
 *
 */
#ifndef AIM_AYE_DS_H_
#define AIM_AYE_DS_H_

#ifdef AIM_AYE_DS

#include "AiMilCom.h"
#include "aim_dma.h"


/*****************************************************************************/
/*                                                                           */
/*                         Defines and Data Types                            */
/*                                                                           */
/*****************************************************************************/


/*! \def aim_ays_ds_device_dma_buffer
 *
 * Digital Scope DMA Buffer List Element.
 */
struct aim_ays_ds_device_dma_buffer
{
    struct list_head list;
    TY_MILSCOPE_BUFFER xScopeBuffer;
    struct aim_dma_buffer dmaBuffer;
};


/*****************************************************************************/
/*                                                                           */
/*                        IOCTL Handling Functions                           */
/*                                                                           */
/*****************************************************************************/


/*! \brief This function processes the provide scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_provide(struct aim_pci_device* device, struct aim_scope_buffer __user* argument);


/*! \brief This function processes the release scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_release(struct aim_pci_device* device);


/*! \brief This function processes the provide scope buffers wait IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_wait(struct aim_pci_device* device);


/*! \brief This function processes the sync scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_sync(struct aim_pci_device* device, unsigned int __user* argument);


/*****************************************************************************/
/*                                                                           */
/*                         IRQ Handling Functions                            */
/*                                                                           */
/*****************************************************************************/


/*! \brief This function processes the digital scope buffer finished interrupt
 *
 * @param device the device the ioctl was issued on
 */
int aim_aye_ds_device_irq_buffer_finished(struct aim_pci_device *device);


/*! \brief This function processes the digital scope buffer error interrupt
 *
 * @param device the device the ioctl was issued on
 */
int aim_aye_ds_device_irq_buffer_error(struct aim_pci_device *device);


#endif /* AIM_AYE_DS    */
#endif /* AIM_AYE_DS_H_ */
