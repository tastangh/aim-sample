/* SPDX-FileCopyrightText: 2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_artix7_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for Artix7 platform
 *
 */

#ifndef AIM_ARTIX7_DEVICE_H_
#define AIM_ARTIX7_DEVICE_H_


#ifdef AIM_ARTIX7



#include "aim_pci_device.h"
#include "aim_rw_interface.h"
#include "aim_dma.h"




/*! \brief Initializes Artix7 hardware of a device
 *
 * This function initializes the hardware specific parts of an Artix7 device. \n
 * It will map its bar registers and enable interrupts. \
 * After this function returned, the device must be fully operational and accessible.
 * @param aimDevice The device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_artix7_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down Artix7 hardware of a device
 *
 * This function will completely shut down hardware specific parts of an Artix7 device. \n
 * It will disable interrupts and un-map the device's bar registers.
 * After this function returned, the device hardware must no more being accessed.
 * @param aimDevice device to shut down hardware of
 */
void aim_artix7_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an Artix7 device
 *
 * This function will set the device's top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_artix7_enable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an Artix7 device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_artix7_disable_interrupts(struct aim_pci_device* aimDevice);


#endif /* AIM_ARTIX7 */


#endif /* AIM_ARTIX7_DEVICE_H_ */
