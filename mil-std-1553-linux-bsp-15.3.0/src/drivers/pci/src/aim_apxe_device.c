/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_apxe_device.c
 *
 *  This file contains definitions for the
 *  APX-GNET platform devices
 *
 */


#include <linux/interrupt.h>
#include "aim_apxe_device.h"
#include "aim_pci_mem.h"
#include "aim_debug.h"



/*! \def APXE_SHARED_MEM_BAR
 * PCI Bar register that holds the shared memory
 */
#define APXE_SHARED_MEM_BAR     0


/*! \def APXE_IO_MEM_BAR
 * PCI Bar register that holds the IO memory
 */
#define APXE_IO_MEM_BAR     4


/*! \def GNET_DOORBELL_REG
 * Byte offset of door bell register in IO Memory. \n
 * This register signals interrupt events.
 */
#define GNET_DOORBELL_REG   0x2C


/*! \def GNET_IRQ_MASK_REG
 * Byte offset of interrupt mask register in IO Memory. \n
 * This register can be used to mask specific interrupt events.
 */
#define GNET_IRQ_MASK_REG   0x34


/*! \def GNET_IMASK_BITMASK
 * IRQ Mask register value that enables ASP interrupt
 */
#define GNET_IMASK_BITMASK  0xFFFFFFEB


/*! \def GNET_ICAUSE_BITMASK
 * IRQ Event mask that holds all IRQ events the driver cares for
 */
#define GNET_ICAUSE_BITMASK 0x1FFFFFFF




/*! \brief Top-Half IRQ handler for APXE boards
 *
 * This handler is called when an interrupt on an IRQ line, that is used
 * by an APXE device, is detected. \n
 * It will check if the corresponding device actually issued the interrupt \n
 * and will call the IRQ tasklet for performing the bottom-half tasks.
 * @param irq the IRQ line that raised the interrupt
 * @param arg pointer to \ref struct aim_pci_device of the corresponding device
 * @return returns IRQ_HANDLED if the device actually issued an interrupt, IRQ_NONE otherwise
 */
irqreturn_t aim_apxe_irq_handler(int irq, void* arg);


/*! \brief Bottom-Half handler for APXE boards
 *
 * This handler function is called in the IRQ tasklet context of APXE boards. \n
 * It will call the protocol specific ASP interrupt handler function for each pending \n
 * ASP interrupt.
 * @param data pointer to \ref struct aim_pci_device of the corresponding device.
 */
void aim_apxe_irq_task(unsigned long data);




/*! \brief Maps the PCI bar registers of an APXE device
 *
 * This function will map the shared RAM bar and the IO memory
 * bar of APXE devices.
 * @param aimDevice the device to map the bars of.
 * @return returns 0 on success, a negative error code otherwise.
 */
static int aim_apxe_map_bars(struct aim_pci_device* aimDevice)
{
    struct pci_dev* pciDevice = NULL;
    resource_size_t barSize = 0;
    int ret = 0;

    BUG_ON(!aimDevice || !aimDevice->pciDevice);

    pciDevice = aimDevice->pciDevice;

    ret = pci_request_regions(pciDevice, AIM_DRV_NAME);
    if (ret) {
	    aim_dev_error(aimDevice, "request regions failed\n");
	    pci_disable_device(pciDevice);
	    return ret;
    }

    barSize = pciDevice->resource[APXE_SHARED_MEM_BAR]. end - pciDevice->resource[APXE_SHARED_MEM_BAR].start + 1;

    aimDevice->sharedMemoryPhysical = pciDevice->resource[APXE_SHARED_MEM_BAR].start;
    aimDevice->sharedMemory = ioremap(aimDevice->sharedMemoryPhysical, barSize);

    if(!aimDevice->sharedMemory)
    {
        aim_dev_error(aimDevice, "Allocation of shared memory failed\n");
        return -ENOMEM;
    }

    aimDevice->sharedMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped shared memory bar to %p with size %llu KB\n", aimDevice->sharedMemory,
                 (unsigned long long) aimDevice->sharedMemorySize / 1024);

    barSize = pciDevice->resource[APXE_IO_MEM_BAR].end - pciDevice->resource[APXE_IO_MEM_BAR].start + 1;

    aimDevice->ioMemoryPhysical = pciDevice->resource[APXE_IO_MEM_BAR].start;
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);
    if(!aimDevice->ioMemory)
    {
        aim_dev_error(aimDevice, "Allocation of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n", aimDevice->ioMemory,
                 (unsigned long long) aimDevice->ioMemorySize / 1024);


    return 0;
}


/*! \brief Un-maps the PCI memory bar registers of APXE devices
 *
 * @param aimDevice the device to un-map the bar registers of.
 */
static void aim_apxe_unmap_bars(struct aim_pci_device* aimDevice)
{
    BUG_ON(!aimDevice);

    aim_dev_debug(aimDevice, "Un-mapping memory bars\n");

    if(aimDevice->ioMemory)
    {
        iounmap(aimDevice->ioMemory);
        aimDevice->ioMemory = NULL;
    }

    if(aimDevice->sharedMemory)
    {
        iounmap(aimDevice->sharedMemory);
        aimDevice->sharedMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}


int aim_apxe_enable_interrupts(struct aim_pci_device* aimDevice)
{
    int ret = 0;
    u32 irqMask = 0;

    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    tasklet_init(&aimDevice->irqTasklet, aim_apxe_irq_task, (unsigned long) aimDevice);

    ret = request_irq(aimDevice->pciDevice->irq, aim_apxe_irq_handler, IRQF_SHARED, dev_name(&aimDevice->pciDevice->dev), aimDevice);
    if(ret)
    {
        aim_dev_error(aimDevice, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(aimDevice, "Using IRQ %d\n", aimDevice->pciDevice->irq);

    irqMask = aim_pci_io_read32(aimDevice, GNET_IRQ_MASK_REG);
    irqMask &= GNET_IMASK_BITMASK;
    aim_pci_io_write32(aimDevice, GNET_IRQ_MASK_REG, irqMask);

    return ret;
}


void aim_apxe_disable_interrupts(struct aim_pci_device* aimDevice)
{
    u32 irqMask = 0;

    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    aim_dev_debug(aimDevice, "Disabling interrupts\n");

    irqMask = aim_pci_io_read32(aimDevice, GNET_IRQ_MASK_REG);
    irqMask |= ~GNET_IMASK_BITMASK;
    aim_pci_io_write32(aimDevice, GNET_IRQ_MASK_REG, irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(aimDevice, GNET_DOORBELL_REG, GNET_ICAUSE_BITMASK);

    free_irq(aimDevice->pciDevice->irq, aimDevice);
}


/*! \brief Initializes DMA transactions for an APXE device
 *
 * This function will just set the DMA mask for the device.
 * @param aimDevice the device to initialize DMA for
 * @return returns 0 on success, a negative error code otherwise
 */
static int aim_apxe_dma_init(struct aim_pci_device* aimDevice)
{
    aim_dev_debug(aimDevice, "Using 64-Bit DMA Mask for APXE platform\n");
    return dma_set_mask(&aimDevice->pciDevice->dev, DMA_BIT_MASK(64));
}


int aim_apxe_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    ret = aim_apxe_map_bars(aimDevice);
    if(ret)
    {
        return ret;
    }

    ret = aim_apxe_enable_interrupts(aimDevice);
    if(ret)
    {
        return ret;
    }

    ret = aim_apxe_dma_init(aimDevice);
    if(ret)
    {
        return ret;
    }

    return ret;
}


irqreturn_t aim_apxe_irq_handler(int irq, void* arg)
{
    u32 irqEvent = 0;
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) arg;

    aim_dev_debug(aimDevice, "IRQ on line %d\n", irq);

    irqEvent = aim_pci_io_read32(aimDevice, GNET_DOORBELL_REG);

    aim_dev_debug(aimDevice, "Irq Event: 0x%08x\n", irqEvent);

    if(irqEvent & GNET_ICAUSE_BITMASK)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]);
        aim_dev_debug(aimDevice, "Received ASP interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]));
        aim_pci_io_write32(aimDevice, GNET_DOORBELL_REG, irqEvent & GNET_ICAUSE_BITMASK);

        tasklet_schedule(&aimDevice->irqTasklet);

        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}


void aim_apxe_device_hw_remove(struct aim_pci_device* deviceInfo)
{
    aim_apxe_disable_interrupts(deviceInfo);

    aim_apxe_unmap_bars(deviceInfo);
}


void aim_apxe_irq_task(unsigned long data)
{
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) data;

    BUG_ON(!aimDevice);

    aim_pci_device_handle_asp_irqs(aimDevice);
}

