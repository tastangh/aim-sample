/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_device_mil.c
 *
 *  This file contains definitions for
 *  1553 specific PCI device properties
 *
 */



#include "aim_pci_device_mil.h"
#include "aim_tsw_mil.h"
#include "aim_nl_mil.h"
#include "mil_hw.h"



struct aim_pci_device* aim_pci_device_create_mil(struct pci_dev *dev, const struct pci_device_id *id)
{
    struct aim_pci_mil_device* aimMilDevice = NULL;

    aimMilDevice = kzalloc(sizeof(struct aim_pci_mil_device), GFP_KERNEL);
    if(!aimMilDevice)
    {
        return NULL;
    }

    spin_lock_init(&aimMilDevice->irq_loglist_lock);

    /* Set up netlink interface */
    aim_nl_setup_mil(aimMilDevice);

    return &aimMilDevice->aimDevice;
}




void aim_pci_device_free_mil(struct aim_pci_device* aimDevice)
{
    struct aim_pci_mil_device* milDevice = NULL;

    BUG_ON(!aimDevice);

    aim_dev_debug(aimDevice, "Freeing device\n");

    milDevice = aim_dev_to_mil(aimDevice);

    if(milDevice->tswData)
    {
        mil_hw_shutdown_tsw(milDevice->tswData);
        kfree(milDevice->tswData);
    }

    kfree(milDevice);
}




int aim_pci_device_init_mil(struct aim_pci_device* device)
{
    int ret = 0;
    struct aim_pci_mil_device* milDevice = NULL;
    TY_HW_INFORMATION __iomem *board_info = NULL;
    __u32 intLoglistOffset = 0;

    BUG_ON(!device || !device->sharedMemory);

    milDevice = aim_dev_to_mil(device);

    if(!device->hasAsp)
    {
        ret = aim_pci_mil_init_tsw(milDevice);
        if(ret)
        {
            return ret;
        }
    }

    /* Now read necessary board configuration from shared memory
     * Must happen AFTER target software is initialized!!!
     */
    board_info = device->sharedMemory;

    device->properties.serial = ioread32(&board_info->ul_BoardSerialNo);
    aim_dev_debug(device, "Board Serial: %u", device->properties.serial);

    device->properties.config = ioread32(&board_info->ul_BoardConfig);
    aim_dev_debug(device, "Board Config: 0x%x", device->properties.config);

    device->properties.type = ioread32(&board_info->ul_BoardType);
    aim_dev_debug(device, "Board Type: 0x%x", device->properties.type);

    intLoglistOffset = 0x00008000L;
    milDevice->interruptLoglist = device->sharedMemory + intLoglistOffset;

    aim_dev_debug(device, "Interrupt Loglist is at %p (offset 0x%x)", milDevice->interruptLoglist, intLoglistOffset);

    return ret;
}


