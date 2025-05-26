/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ayx_device.c
 *
 *  This file contains hardware related definitions
 *  specific for AYX platform
 *
 */


#include <linux/types.h>
#include "aim_ayx_device.h"
#include "aim_pci_mem.h"




/*! \def AYX_SHARED_MEM_BAR
 * ID of shared memory bar on AYX devices
 */
#define AYX_SHARED_MEM_BAR 0


/*! \def AYX_GLOBAL_MEM_BAR
 * ID of global memory bar on AYX devices
 */
#define AYX_GLOBAL_MEM_BAR 1


/*! \def AYX_IO_MEM_BAR
 * ID of IO memory bar on AYX devices
 */
#define AYX_IO_MEM_BAR 2


/*! \def AYX_IO_GLOB_MEM_BAR
 * ID of IO and GLOBAL memory bar on APE3910 AYX based devices
 */
#define AYX_IO_GLOB_MEM_BAR 4


/*! \def AYX_SHARED_MEM_OFFSET
 * AYX devices have mapped some internal data to the beginning of the bar register.
 * This is the byte offset were real shared memory starts
 */
#define AYX_SHARED_MEM_OFFSET 0x4000


/*! \def AYX_REG_IRQ_HOST_EVENT
 * Byte offset to interrupt event register in IO memory
 */
#define AYX_REG_IRQ_HOST_EVENT 0x180


/*! \def AYX_REG_IRQ_HOST_MASK
 * Byte offset to interrupt mask register in IO memory
 */
#define AYX_REG_IRQ_HOST_MASK 0x190


/*! \def AYX_IRQ_EVENT_ASP
 * ASP interrupt event bit
 */
#define AYX_IRQ_EVENT_ASP 0x00000004


/*! \def AYX_IRQ_EVENT_ALL
 * All interrupt event types on AYX devices that are handled by the driver
 */
#define AYX_IRQ_EVENT_ALL (AYX_IRQ_EVENT_ASP)





/*! \brief Maps PCI memory bar registers on AYX boards
 *
 * @param aimDevice the device to map bar registers of
 * @return 0 on success, errno code otherwise
 */
static int aim_ayx_map_memory_standard(struct aim_pci_device* aimDevice)
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

    barSize = pciDevice->resource[AYX_GLOBAL_MEM_BAR].end - pciDevice->resource[AYX_GLOBAL_MEM_BAR].start + 1;

    aimDevice->globalMemoryPhysical = pciDevice->resource[AYX_GLOBAL_MEM_BAR].start;
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, barSize);
    if( !aimDevice->globalMemory )
    {
        aim_dev_error(aimDevice, "Mapping of global memory failed\n");
        return -ENOMEM;
    }

    aimDevice->globalMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped global memory bar to %p with size %llu KB\n", aimDevice->globalMemory,
                  (unsigned long long) aimDevice->globalMemorySize / 1024);


    barSize = pciDevice->resource[AYX_IO_MEM_BAR].end - pciDevice->resource[AYX_IO_MEM_BAR].start + 1;

    aimDevice->ioMemoryPhysical = pciDevice->resource[AYX_IO_MEM_BAR].start;
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);

    if( !aimDevice->ioMemory )
    {
        aim_dev_error(aimDevice, "Mapping of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n", aimDevice->ioMemory,
                 (unsigned long long ) aimDevice->ioMemorySize / 1024);

    barSize = pciDevice->resource[AYX_SHARED_MEM_BAR].end - pciDevice->resource[AYX_SHARED_MEM_BAR].start + 1;

    aimDevice->sharedMemoryPhysical = pciDevice->resource[AYX_SHARED_MEM_BAR].start + AYX_SHARED_MEM_OFFSET;

    aimDevice->sharedMemory = ioremap(aimDevice->sharedMemoryPhysical, barSize - AYX_SHARED_MEM_OFFSET);

    if( !aimDevice->sharedMemory )
    {
        aim_dev_error(aimDevice, "Mapping of shared memory failed\n");
        return -ENOMEM;
    }

    aimDevice->sharedMemorySize = barSize - AYX_SHARED_MEM_OFFSET;

    aim_dev_info(aimDevice, "Mapped shared memory bar to %p with size %llu KB\n", aimDevice->sharedMemory,
                   (unsigned long long ) aimDevice->sharedMemorySize / 1024);

    return 0;
}


#define APE3910_AYX_BASED_GLOB_START  0x03000000
#define APE3910_AYX_BASED_GLOB_SIZE   0x01000000 /* 16 MB */
#define APE3910_AYX_BASED_IO_START    0x01000000
#define APE3910_AYX_BASED_IO_SIZE     0x00100000 /* 1 MB */


static int aim_ayx_map_memory_ape3910(struct aim_pci_device* aimDevice)
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

    aimDevice->globalMemoryPhysical = pciDevice->resource[AYX_IO_GLOB_MEM_BAR].start + APE3910_AYX_BASED_GLOB_START;
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, APE3910_AYX_BASED_GLOB_SIZE);

    if( !aimDevice->globalMemory )
    {
        aim_dev_error(aimDevice, "Mapping of global memory failed\n");
        return -ENOMEM;
    }

    aimDevice->globalMemorySize = APE3910_AYX_BASED_GLOB_SIZE;

    aim_dev_info(aimDevice, "Mapped global memory bar to %p with size %llu KB\n", aimDevice->globalMemory,
                  (unsigned long long) aimDevice->globalMemorySize / 1024);

    aimDevice->ioMemoryPhysical = pciDevice->resource[AYX_IO_GLOB_MEM_BAR].start + APE3910_AYX_BASED_IO_START;
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, APE3910_AYX_BASED_IO_SIZE);

    if( !aimDevice->ioMemory )
    {
        aim_dev_error(aimDevice, "Mapping of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = APE3910_AYX_BASED_IO_SIZE;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n", aimDevice->ioMemory,
                 (unsigned long long ) aimDevice->ioMemorySize / 1024);

    barSize = pciDevice->resource[AYX_SHARED_MEM_BAR].end - pciDevice->resource[AYX_SHARED_MEM_BAR].start + 1;

    aimDevice->sharedMemoryPhysical = pciDevice->resource[AYX_SHARED_MEM_BAR].start + AYX_SHARED_MEM_OFFSET;
    aimDevice->sharedMemory = ioremap(aimDevice->sharedMemoryPhysical, barSize - AYX_SHARED_MEM_OFFSET);

    if( !aimDevice->sharedMemory )
    {
        aim_dev_error(aimDevice, "Mapping of shared memory failed\n");
        return -ENOMEM;
    }

    aimDevice->sharedMemorySize = barSize - AYX_SHARED_MEM_OFFSET;

    aim_dev_info(aimDevice, "Mapped shared memory bar to %p with size %llu KB\n", aimDevice->sharedMemory,
                   (unsigned long long ) aimDevice->sharedMemorySize / 1024);

    return 0;
}


// -- APX / ACX-3U memory bar description ---
//
//   BAR  |  TYPE        | WINDOW    | REAL SIZE            |   DETECTION                        |  COMMENT
//   ---------------------------------------------------------------------------------------------------------------------------
//    0   | Shared RAM   | 32, 64 MB |                      | SHARED RAM Mark @0x4000            |
//    1   | Global RAM   | 16 MB     | 1, 2, 4, 8, 16, 32MB | GLOBAL RAM Mark @0x007C (MIL)      | Real size detection required
//        |              |           |                      | GLOBAL RAM Mark @0x003C (CIV,AFDX) | Real size detection required
//    2   | IO     RAM   | 1  MB     |                      | Fixed size / First 4 Words equal   | Required for interrupt handling
//   ----------------------------------------------------------------------------------------------------------------------------
//
//  APX1553-4 PBI2.0,           shared 64MB, global 16MB (16MB)
//  APX1553-1DS,                shared 64MB, global 16MB ( 4MB)
//  APX1553 (first) New-PBI,    shared 64MB, global 16MB, ( Windows freeze during deactivate/activate )
//  APX3910-EL (first) New-PBI, shared 64MB, global 16MB, ( Windows freeze during deactivate/activate )
//
//  APX3910 New-PBI (optical),  shared 32MB, global 16MB (16MB)
//
//  ACX1553-3U-1-DS,            shared 32MB, global 16MB
//  ACX3910-3U,                 shared 32MB, global 16MB
//
//  APX-GNET
//
// -- ACX-6U memory bar description ---
//
//   BAR       |  TYPE        | WINDOW    | REAL SIZE    |   DETECTION                      |  COMMENT
//   ------------------------------------------------------------------------------------------------------------------------
//    0        | Shared RAM   | 64 MB     |              | SHARED RAM Mark @0x4000          |
//    1        | Global RAM 1 | 64 MB     | 16, 32MB     | GLOBAL RAM Mark @0x007C          |
//    1 + 32MB | Global RAM 2 |           | 16, 32MB     | GLOBAL RAM Mark @0x007C          | Real size detection required
//    2        | IO     RAM   | 1  MB     |              | Fixed size / First 4 Words equal | Required for interrupt handling
//   ------------------------------------------------------------------------------------------------------------------------
//
//  ACX3910-6U-2,               shared 64MB, global 64MB (2*16MB),
//
// -- APE3910 (AyX based) memory bar description ---
//
//   BAR     |  TYPE        | WINDOW    | REAL SIZE            |   DETECTION                        |  COMMENT
//   -----------------------------------------------------------------------------------------------------------
//    0      | Shared RAM   | 32        |                      | SHARED RAM Mark @0x4000            |
//    4      | Global/IO    | 64 MB     |                      |
//    4+ 0MB | Spare        | 16 MB     | ignored              |                                    | Ignored
//    4+16MB | IO     RAM   | 16 MB     | 1MB                  |                                    | ASL
//    4+32MB | Mscope BRAM  | 16 MB     | ignored              |                                    | Ignored
//    4+48MB | Global RAM   | 16 MB     | 16MB                 | GLOBAL RAM Mark @0x007C (MIL)      |
//   -----------------------------------------------------------------------------------------------------------
//
//  APE3910,           shared 32MB, global 16MB (16MB), io 16 MB

static int aim_ayx_map_memory(struct aim_pci_device* aimDevice)
{
    if( aimDevice->pciDevice->resource[AYX_IO_GLOB_MEM_BAR].start != 0 )
        return aim_ayx_map_memory_ape3910(aimDevice);
    else
        return aim_ayx_map_memory_standard(aimDevice);

}


/*! \brief Unmaps PCI memory bars of a device
 *
 * @param aimDevice the device to unmap memory bars of
 */
static void aim_ayx_unmap_memory(struct aim_pci_device* aimDevice)
{
    BUG_ON(!aimDevice);

    aim_dev_debug(aimDevice, "Unmapping memory bars\n");

    if(aimDevice->globalMemory)
    {
        aim_dev_debug(aimDevice, "Unmapping Global Memory\n");
        iounmap(aimDevice->globalMemory);
        aimDevice->globalMemory = NULL;
    }

    if(aimDevice->ioMemory)
    {
        aim_dev_debug(aimDevice, "Unmapping IO memory\n");
        iounmap(aimDevice->ioMemory);
        aimDevice->ioMemory = NULL;
    }

    if(aimDevice->sharedMemory)
    {
        aim_dev_debug(aimDevice, "Unmapping shared memory\n");
        iounmap(aimDevice->sharedMemory);
        aimDevice->sharedMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}


/*! \brief IRQ Bottom Half handler
 *
 * This function is scheduled via a tasklet and will handle the bottom half
 * of a device's interrupts
 * @param data pointer to \ref struct aim_pci_device of the device to handle interrupts on
 */
static void aim_ayx_irq_task(unsigned long data)
{
    struct aim_pci_device* device = (struct aim_pci_device*) data;

    BUG_ON(!device);

    aim_pci_device_handle_asp_irqs(device);
}


/*! \brief Top-Half IRQ handler function
 *
 * @param irq IRQ to handle
 * @param arg pointer to \ref struct aim_pci_device the IRQ needs to be handled for
 * @return IRQ_HANDLED, if interrupt was issued by the device, IRQ_NONE otherwise
 */
static irqreturn_t aim_ayx_irq_handler(int irq, void* arg)
{
    __u32 irqEvent = 0;
    struct aim_pci_device* device = (struct aim_pci_device*) arg;

    /* Check the IRQ event register and return in case
     * no event, we're responsible for, was raised
     */
    irqEvent = aim_pci_io_read32(device, AYX_REG_IRQ_HOST_EVENT);

    if(!(irqEvent & AYX_IRQ_EVENT_ALL))
    {
        return IRQ_NONE;
    }

    aim_dev_debug(device, "Irq Event: 0x%08x\n", irqEvent);

    /* Acknowledge all events we will process further on */
    aim_pci_io_write32(device, AYX_REG_IRQ_HOST_EVENT, irqEvent & AYX_IRQ_EVENT_ALL);

    if(irqEvent & AYX_IRQ_EVENT_ASP)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_ASP_IRQ]);
        aim_dev_debug(device, "Received ASP interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_ASP_IRQ]));
    }

    tasklet_schedule(&device->irqTasklet);

    return IRQ_HANDLED;
}


int aim_ayx_enable_interrupts(struct aim_pci_device* device)
{
    __u32 mask = 0;
    int ret = 0;

    BUG_ON(!device || !device->ioMemory);

    /* Clear outstanding pending interrupts */
    aim_pci_io_write32(device, AYX_REG_IRQ_HOST_EVENT, AYX_IRQ_EVENT_ALL);

    tasklet_init(&device->irqTasklet, aim_ayx_irq_task, (unsigned long) device);

    ret = request_irq(device->pciDevice->irq, aim_ayx_irq_handler, IRQF_SHARED, dev_name(&device->pciDevice->dev), device);
    if( ret )
    {
        aim_dev_error(device, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(device, "Using IRQ %d\n", device->pciDevice->irq);

    /* Now unmask all interrupts we're interested in */
    mask = aim_pci_io_read32(device, AYX_REG_IRQ_HOST_MASK);

    mask |= AYX_IRQ_EVENT_ALL;

    aim_pci_io_write32(device, AYX_REG_IRQ_HOST_MASK, mask);

    aim_dev_debug(device, "IRQ mask: 0x%x", mask);

    return 0;
}


void aim_ayx_disable_interrupts(struct aim_pci_device* device)
{
    __u32 irqMask = 0;

    BUG_ON(!device || !device->ioMemory);

    aim_dev_debug(device, "Disabling interrupts\n");

    irqMask = aim_pci_io_read32(device, AYX_REG_IRQ_HOST_MASK);
    irqMask &= ~AYX_IRQ_EVENT_ALL;

    aim_dev_debug(device, "IRQ Mask after disabling interrupts is 0x%x\n", irqMask);

    aim_pci_io_write32(device, AYX_REG_IRQ_HOST_MASK, irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(device, AYX_REG_IRQ_HOST_EVENT, AYX_IRQ_EVENT_ALL);

    free_irq(device->pciDevice->irq, device);
}


int aim_ayx_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_ayx_map_memory(aimDevice);
        if( ret )
        {
            break;
        }

        ret = aim_ayx_enable_interrupts(aimDevice);
        if(ret)
        {
            break;
        }
        
        dma_set_mask(&aimDevice->pciDevice->dev, 0xffffffffULL );

    } while( 0 );

    if( ret )
    {
        aim_ayx_device_hw_remove(aimDevice);
    }

    return ret;
}


void aim_ayx_device_hw_remove(struct aim_pci_device* aimDevice)
{
    aim_ayx_disable_interrupts(aimDevice);

    aim_ayx_unmap_memory(aimDevice);
}

