/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_apxe_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for APXE (AYX-GNET) platform
 *
 */

#ifndef AIM_APXE_DEVICE_H_
#define AIM_APXE_DEVICE_H_


#include "aim_pci_device.h"




/*! \brief Initializes APXE hardware of a device
 *
 * This function initializes the hardware specific parts of an APXE device. \n
 * It will map its bar registers, enable interrupts and DMA availability. \
 * After this function returned, the device must be fully operational and accessible.
 * @param aimDevice The device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_apxe_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down APXE hardware of a device
 *
 * This function will completely shut down hardware specific parts of an APXE device. \n
 * It will disable interrupts and un-map the device's bar registers.
 * After this function returned, the device hardware must no more being accessed.
 * @param aimDevice device to shut down hardware of
 */
void aim_apxe_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an APXE device
 *
 * This function will initialize the device's IRQ tasklet and set the top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_apxe_enable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an APXE device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_apxe_disable_interrupts(struct aim_pci_device* aimDevice);


#endif /* AIM_APXE_DEVICE_H_ */
