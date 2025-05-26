/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_device_mil.h
 *
 *  This header file contains declarations for
 *  1553 channel specific PCI device properties
 *
 */

#ifndef AIM_PCI_DEVICE_MIL_H_
#define AIM_PCI_DEVICE_MIL_H_


#include <linux/spinlock.h>
#include "aim_pci_device.h"
#include "api_defv.h"




/*! \struct aim_pci_mil_device
 * This structure describes an AIM PCI 1553 device
 */
struct aim_pci_mil_device
{
    struct aim_pci_device aimDevice;  /*!< Generic PCI device */

    TY_API_DEV* tswData; /*!< target software data */

    spinlock_t irq_loglist_lock; /*!< spin lock for serializing access to interrupt log list */

    struct ty_api_intr_event_list __iomem* interruptLoglist; /*!< pointer to interrupt log list of the device */
};


#define aim_dev_to_mil(aimDev) container_of(aimDev,struct aim_pci_mil_device, aimDevice)
#define mil_to_aim(milDevice) (&milDevice->aimDevice)
#define mil_to_dev(milDevice) aim_to_dev(mil_to_aim(milDevice))




/*! \brief Function for initializing 1553 devices
 *
 * This function performs the basic operations to initialize a 1553 device. \n
 * It is called when an AIM PCI device is probed.
 * @param aimDevice pointer to generic AIM PCI device structure of the device to initialize
 * @return returns 0 on success, an appropriate error code otherwise
 */
int aim_pci_device_init_mil(struct aim_pci_device* aimDevice);


/*! \brief Creates a 1553 device structure
 *
 * This function creates and initializes a 1553 device structure
 * for a specific device. \n
 * The function returns a corresponding generic AIM PCI device structure, that is included in the 1553 device structure.
 * It is called when an AIM PCI device is probed.
 * @param dev The generic PCI device to create the 1553 device structure for
 * @param id PCI specific data for the device
 * @return returns a pointer to the corresponding generic AIM PCI device structure of the device, NULL on error
 */
struct aim_pci_device* aim_pci_device_create_mil(struct pci_dev *dev, const struct pci_device_id *id);


/*! \brief Frees a 1553 device structure
 *
 * This function is called when all references to a device
 * are dropped and deletes the 1553 device structure
 * @param aimDevice corresponding generic AIM device structure of the device to free
 */
void aim_pci_device_free_mil(struct aim_pci_device* aimDevice);



#endif /* AIM_PCI_DEVICE_MIL_H_ */
