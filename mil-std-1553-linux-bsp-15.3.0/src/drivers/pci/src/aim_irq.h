/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_irq.h
 *
 *  This header file contains platform and protocol
 *  independent declarations for handling device interrupts.
 *
 */

#ifndef AIM_IRQ_H_
#define AIM_IRQ_H_


#include "aim_pci_device.h"


/*! Forward declaration of AIM PCI device structure */
struct aim_pci_device;


/*! \enum aim_pci_irq_sources
 * This enumeration holds all possible interrupt sources
 * of AIM PCI devices
 */
enum aim_pci_irq_sources{
    AIM_PCI_BIU1_IRQ = 0,  /*!< BIU1 was the source of the interrupt */
    AIM_PCI_BIU2_IRQ,      /*!< BIU2 was the source of the interrupt */
    AIM_PCI_BIU3_IRQ,      /*!< BIU3 was the source of the interrupt */
    AIM_PCI_BIU4_IRQ,      /*!< BIU4 was the source of the interrupt */
    AIM_PCI_ASP_IRQ,       /*!< ASP was the source of the interrupt */
    AIM_PCI_DS_IRQ,        /*!< Digital Scope was the source of the interrupt */
    AIM_PCI_DS_ERR_IRQ,    /*!< Digital Scope Error was the source of the interrupt */
    AIM_PCI_MAX_IRQ,       /*!< Just signifies end of interrupt source list */
};




/*! \struct aim_pci_irq_ops
 *
 * This structure shall be used to insert protocol specific
 * interrupt handlers
 */
struct aim_pci_irq_ops
{
    void (*handle_biu_irq)(struct aim_pci_device* aimDevice, int biuID);  /*!< handler for BIU related interrupts */
    void (*handle_asp_irq)(struct aim_pci_device* aimDevice);             /*!< handler for ASP related interrupts */
};


/*! \def aim_pci_device_handle_asp_irqs
 * Handles all pending ASP interrupts of a device
 * by calling the protocol specific handler
 */
#define aim_pci_device_handle_asp_irqs(aimDevice) \
    do \
    { \
        while(atomic_read(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]) > 0) \
        { \
            atomic_dec(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]); \
            if(!aimDevice->deviceData->irqOps) \
               	break; \
            if(!aimDevice->deviceData->irqOps->handle_asp_irq) \
                break; \
            aimDevice->deviceData->irqOps->handle_asp_irq(aimDevice); \
        } \
    }while(0) \


/*! \def aim_pci_device_handle_biu_irqs
 * Handles all pending BIU interrupts of a device
 * by calling the protocol specific handler */
#define aim_pci_device_handle_biu_irqs(aimDevice, biuID) \
    do \
    { \
        while(atomic_read(&aimDevice->pendingIrqs[biuID]) > 0) \
        { \
            atomic_dec(&aimDevice->pendingIrqs[biuID]); \
            if(!aimDevice->deviceData->irqOps) \
               	break; \
            if(!aimDevice->deviceData->irqOps->handle_biu_irq) \
                break; \
            aimDevice->deviceData->irqOps->handle_biu_irq(aimDevice, biuID); \
        } \
    }while(0) \



#endif /* AIM_IRQ_H_ */
