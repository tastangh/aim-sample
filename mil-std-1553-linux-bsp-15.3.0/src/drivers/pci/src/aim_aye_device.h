/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aye_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for AYE platform
 *
 */

#ifndef AIM_AYE_DEVICE_H_
#define AIM_AYE_DEVICE_H_


#ifdef AIM_AYE



#include "aim_pci_device.h"
#include "aim_rw_interface.h"
#include "aim_dma.h"




/*! \brief Initializes AYE hardware of a device
 *
 * This function initializes the hardware specific parts of an AYE device. \n
 * It will map its bar registers, enable interrupts and DMA availability. \
 * After this function returned, the device must be fully operational and accessible.
 * @param aimDevice The device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_aye_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down AYE hardware of a device
 *
 * This function will completely shut down hardware specific parts of an AYE device. \n
 * It will disable interrupts and un-map the device's bar registers.
 * After this function returned, the device hardware must no more being accessed.
 * @param aimDevice device to shut down hardware of
 */
void aim_aye_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an AYE device
 *
 * This function will set the device's top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_aye_enable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an AYE device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_aye_disable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief AYE DMA read function
 *
 * This function tries to start a DMA transaction on the FPGA memory DMA channel. \n
 * No target software interaction must be necessary here
 * @param device device to start DMA read on
 * @param buffer user space buffer prepared for DMA to read into
 * @param memType board memory type to read from
 * @param offset offset in bytes to read from
 * @return
 */
int aim_aye_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType,
                                   loff_t offset);


#endif /* AIM_AYE */


#endif /* AIM_AYE_DEVICE_H_ */
