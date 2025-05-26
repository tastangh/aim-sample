/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_amc_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for AIM AMC platform
 *
 */

#ifndef AIM_AMC_DEVICE_H_
#define AIM_AMC_DEVICE_H_


#include "aim_pci_device.h"




/*! \brief Initializes devices based on AMC platform
 *
 * This function maps the bar registers of AMC devices
 * and enables interrupt availability.
 *
 * @param aimDevice device to initialize hardware of
 * @return returns 0 on success, a negative errno code otherwise
 */
int aim_amc_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Removes an AMC platform device
 *
 * This function will release all resources that have been
 * allocated for this device.
 * E.g. Interrupts are disabled
 * @param aimDevice device to remove
 */
void aim_amc_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an AMC device
 *
 * This function will initialize the device's IRQ tasklet and set the top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_amc_enable_interrupts(struct aim_pci_device* aimDevice);




/*! \brief Disables interrupts of an AMC device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_amc_disable_interrupts(struct aim_pci_device* aimDevice);



#endif /* AIM_AMC_DEVICE_H_ */
