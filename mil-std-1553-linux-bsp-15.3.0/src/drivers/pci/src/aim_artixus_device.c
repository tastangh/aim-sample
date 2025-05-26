/* SPDX-FileCopyrightText: 2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_artixus_device.c
 *
 *  This file contains definitions for the
 *  ArtixUS platform devices
 *
 */


#include <linux/types.h>
#include "aim_artixus_device.h"
#include "aim_debug.h"
#include "aim_pci_mem.h"
#include "aim_kernel_mem.h"
#include "aim_pci_device_prot.h"
#include "hw/AiHwArtixUS.h"



/*! \def ARTIXUS_IO_MEM_BAR
 *
 * Bar ID of IO memory (0,1) 64 BIT bar register
 */
#define ARTIXUS_IO_MEM_BAR 0

/*! \def ARTIXUS_RESERVED_MEM_BAR
 *
 * Bar ID of Reserved memory (2,3) 64 BIT bar register
 */
#define ARTIXUS_RESERVED_MEM_BAR 2

/*! \def ARTIXUS_GLOBAL_MEM_BAR
 *
 * Bar ID of global memory (4,5) 64 BIT bar register
 */
#define ARTIXUS_GLOBAL_MEM_BAR 4


/*! \def ARTIXUS_SHARED_MEMORY_SIZE
 *
 * Size of virtual shared memory in bytes
 */
#define ARTIXUS_SHARED_MEMORY_SIZE (16 << 20) /* 16MB */


/*! \def ARTIXUS_MAX_BIU
 *
 * Maximum number of logical BIUs on ARTIXUS boards
 */
#define ARTIXUS_MAX_BIU 4


/*! \brief Initializes DMA on ArtixUs boards
 *
 * Allocate the SGL memory for the DMA C2H channel 1
 * @param device device to initialize DMA for
 * @return returns 0 on success, an errno code otherwise
 */
static int __inline__ aim_artixus_dma_init(struct aim_pci_device* device)
{
    int ret = 0;
    AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel;

    aim_dev_debug(device, "Using 64-Bit DMA width for ArtixUS platform\n");
    ret = dma_set_mask(&device->pciDevice->dev, DMA_BIT_MASK(64));

    if(ret)
    {
        aim_dev_error(device, "Failed to set DMA mask to 64 bit");
        return ret;
    }

    channel = kmalloc(sizeof(AIMPCI_HW_ARTIXUS_DMA_CHANNEL), GFP_KERNEL);

    if(!channel)
    {
        aim_dev_error(device, "Failed to allocate dma channel struct");
        return -ENOMEM;
    }

    channel->id = 0;

    ret = aim_artixus_dma_channel_allocate(device, channel);

    if(ret)
    {
        kfree(channel);
        aim_dev_error(device, "Failed to allocate dma channel members (%d)", ret);
        return ret;
    }

    radix_tree_insert(&device->pciDmaChannels, channel->id, channel);

    return ret;
}

/*
    This function detects the real size of a mapped memory.

    The memory is detected by writing a Magic word to a specified
    position in the memory. The memory is then check where the magic
    word can be found again. We write the magic to its usual location
    to handle cases where the Firmware did not boot.

    IN :
    PAIMPCI_MEMORY pMemory    => The memory to analyse
    ULONG ulMagic             => The magic code
    ULONG ulMagicOffsetWord   => The word offset to write the magic word

    OUT :
    PULONG pulDetectedSize    <=  The detected real memory size

*/
static int aim_artixus_memory_detect_size( void __iomem * mem, resource_size_t mem_size, AiUInt32 ulMagic, resource_size_t ulMagicOffsetWord, resource_size_t * pulDetectedSize )
{
    __u32 __iomem*   pulData          = NULL;
    __u32             ulMegabyteOffset = 0x100000 / 4; // 1 MB in words
    __u32             ulCheckWord      = 0;
    __u32             bDetected        = 0;


    // -- check input parameters --

    if( pulDetectedSize == NULL )
        return -EINVAL;

    if( mem == NULL )
        return -EINVAL;

    if( (ulMagicOffsetWord*4) >= mem_size )
        return -EINVAL;

    // -- set magic and search the second offset --

    pulData          = (__u32 __iomem*)mem;
    *pulDetectedSize = 0;

    iowrite32(ulMagic, pulData + ulMagicOffsetWord);

    // -- check magic at offset + 1MB, +2MB, 4MB, +8MB, +16MB, +32MB --

    while( bDetected == 0 )
    {
        ulCheckWord = ulMegabyteOffset + ulMagicOffsetWord;

        if( (ulCheckWord*4) <= mem_size )
        {
            if( ioread32(pulData + ulCheckWord) != ulMagic )
            {
                // Magic not found check next megabyte offset
                ulMegabyteOffset  = ulMegabyteOffset * 2;
            }
            else
            {
                // Magic found store megabyte offset in byte
                bDetected = 1;
                *pulDetectedSize  = ulMegabyteOffset * 4;
            }
        }
        else
        {
            // outch : out of range index
            bDetected = 1;
            *pulDetectedSize = mem_size;
        }
    }


    return 0;
}

/*! \brief Maps memory of ARTIXUS boards
 *
 *  Maps global, IO and global extension memory PCI bars
 *  and allocates a virtual shared memory area in kernel space
 * @param aimDevice PCI device to map bars of
 * @return 0 on success, errno code otherwise
 */
static int aim_artixus_map_memory(struct aim_pci_device* aimDevice)
{
    struct pci_dev* pciDevice = NULL;
    resource_size_t barSize = 0;
    resource_size_t barSizeDetected = 0;
    int ret;

    BUG_ON(!aimDevice || !aimDevice->pciDevice);

    pciDevice = aimDevice->pciDevice;

    ret = pci_request_regions(pciDevice, AIM_DRV_NAME);
    if (ret) {
	    aim_dev_error(aimDevice, "request regions failed\n");
	    pci_disable_device(pciDevice);
	    return ret;
    }

    barSize = pci_resource_len(pciDevice, ARTIXUS_GLOBAL_MEM_BAR);
    aimDevice->globalMemoryPhysical = pci_resource_start(pciDevice,
							 ARTIXUS_GLOBAL_MEM_BAR);
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, barSize);
    if(!aimDevice->globalMemory) {
        aim_dev_error(aimDevice, "Mapping of global memory %llu-%llu failed\n",
		      aimDevice->globalMemoryPhysical,
		      aimDevice->globalMemoryPhysical + barSize);
        return -ENOMEM;
    }

    ret = aim_artixus_memory_detect_size( aimDevice->globalMemory, barSize, AIM_PCI_DEVICE_GLOBAL_RAM_MARK, AIM_PCI_DEVICE_GLOBAL_RAM_MARK_OFFSET / 4, &barSizeDetected );

    if( (ret != 0) || (barSizeDetected == 0) )
        aim_dev_error(aimDevice, "Global size detection failed\n");
    else
        barSize = barSizeDetected;

    aimDevice->globalMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped global memory bar to %p with size %llu KB\n",
		 aimDevice->globalMemory,
		 (unsigned long long)aimDevice->globalMemorySize / 1024);

    barSize = pci_resource_len(pciDevice, ARTIXUS_IO_MEM_BAR);
    aimDevice->ioMemoryPhysical = pci_resource_start(pciDevice, ARTIXUS_IO_MEM_BAR);
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);
    if(!aimDevice->ioMemory) {
        aim_dev_error(aimDevice, "Mapping of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n",
		 aimDevice->ioMemory,
                 (unsigned long long)aimDevice->ioMemorySize / 1024);

    /* As ARTIXUS boards do not have an ASP, we allocate a virtual shared memory */
    aimDevice->sharedMemory = vzalloc(ARTIXUS_SHARED_MEMORY_SIZE);
    if(!aimDevice->sharedMemory)
    {
        aim_dev_error(aimDevice, "Allocation of shared memory failed\n");
        return -ENOMEM;
    }

    aimDevice->sharedMemorySize = ARTIXUS_SHARED_MEMORY_SIZE;

    aim_dev_info(aimDevice, "Allocated shared memory to %p with size %llu KB\n", aimDevice->sharedMemory,
                 (unsigned long long) aimDevice->sharedMemorySize / 1024);

    return 0;
}


/*! brief Unmap memory of device
 *
 * @param aimDevice PCI device to unmap bar registers of
 */
static void aim_artixus_unmap_memory(struct aim_pci_device* aimDevice)
{
    BUG_ON(!aimDevice);

    aim_dev_debug(aimDevice, "Un-mapping memory bars\n");

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
        aim_dev_debug(aimDevice, "Freeing shared memory\n");
        vfree(aimDevice->sharedMemory);
        aimDevice->sharedMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}


/*! brief Wait until TCP is started
 *
 * @param aimDevice PCI device to wait for
 */
static int aim_artixus_d0_wait(struct aim_pci_device* device)
{
    u32 i            = 0;
    u32 ulBootStatus = 0;

    aim_dev_info(device, "aim_artixus_d0_wait: Wait for TCP ready signal.\n" );

    while( ++i != 0x00100000 )
    {
        // use i as hard timeout in case TCP does not start

        ulBootStatus = aim_pci_io_read32( device, ARTIXUS_REG_TCP_BOOT_STATUS* sizeof(__u32) );

        if (ulBootStatus == ARTIXUS_REG_TCP_BOOT_STATUS_READY)
            break;

        if( (i%0x10000) == 0 )
        {
            aim_dev_info(device, "aim_artixus_d0_wait: i = 0x%08X, BOOTSTATUS = 0x%08X\n", i, ulBootStatus );
        }
    }


    if( i < 0x00100000 )
    {
        // device ready
        aim_dev_info(device, "aim_artixus_d0_wait: i = 0x%08X, BOOTSTATUS = 0x%08X\n", i, ulBootStatus );
        aim_dev_info(device, "aim_artixus_d0_wait: Device ready\n" );
        return 0;
    }
    else
    {
        // time-out
        aim_dev_info(device, "aim_artixus_d0_wait: i = 0x%08X, BOOTSTATUS = 0x%08X\n", i, ulBootStatus );
        aim_dev_error(device, "aim_artixus_d0_wait: Device timeout\n" );
        return -EAGAIN;
    }

}






/*! \brief aim_artixus_irq_handler
 *
 * Top-Half IRQ handler function
 * @param irq IRQ to handle
 * @param arg pointer to \ref struct aim_pci_device the IRQ needs to be handled for
 * @return IRQ_HANDLED, if interrupt was issued by the device, IRQ_NONE otherwise
 */
static irqreturn_t aim_artixus_irq_handler(int irq, void* arg)
{
    __u32 irqEvent = 0;
    struct aim_pci_device* device = (struct aim_pci_device*) arg;
    bool needBottomHalf = false;

    /* Check the IRQ event register and return in case
     * no event, we're responsible for, was raised
     */
    irqEvent = aim_pci_io_read32(device, ARTIXUS_REG_IRQ_HOST_EVENT * sizeof(__u32));

    if(!(irqEvent & ARTIXUS_IRQ_ALL))
    {
        return IRQ_NONE;
    }

    aim_dev_debug(device, "Irq Event: 0x%08x\n", irqEvent);

    /* Acknowledge all events we will process further on */
    aim_pci_io_write32(device, ARTIXUS_REG_IRQ_HOST_EVENT * sizeof(__u32), irqEvent & ARTIXUS_IRQ_ALL);

    if(irqEvent & ARTIXUS_IRQ_BIU_1)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU1_IRQ]);
        aim_dev_debug(device, "Received BIU 1 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU1_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & ARTIXUS_IRQ_BIU_2)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU2_IRQ]);
        aim_dev_debug(device, "Received BIU 2 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU2_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & ARTIXUS_IRQ_BIU_3)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU3_IRQ]);
        aim_dev_debug(device, "Received BIU 3 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU3_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & ARTIXUS_IRQ_BIU_4)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU4_IRQ]);
        aim_dev_debug(device, "Received BIU 4 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU4_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & ARTIXUS_IRQ_DMA_C2H_CH1)
    {
        /* Got interrupt on DMA C2H channel 1
         * Get first DMA token from device global queue and complete
         * in order to wake up waiting processes
         */
        aim_dev_debug(device, "Received memory DMA C2H channel 1 interrupt.");

        aim_pci_device_dma_complete( device );
    }

    if(needBottomHalf)
    {
        tasklet_schedule(&device->irqTasklet);
    }

    return IRQ_HANDLED;
}


/*! \brief Bottom-Half IRQ handler function
 *
 * This function is scheduled via a tasklet and processes all pending interrupts
 * @param data pointer \ref struct aim_pci_device* of the device to process IRQs of.
 */
static void aim_artixus_irq_task(unsigned long data)
{
    int i = 0;
    struct aim_pci_device* device = (struct aim_pci_device*) data;

    BUG_ON(!device);

    for(i = 0; i < ARTIXUS_MAX_BIU; i++)
    {
        aim_pci_device_handle_biu_irqs(device, i);
    }
}


int aim_artixus_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_artixus_map_memory(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_artixus_d0_wait(aimDevice);

        if(ret)
        {
            break;
        }

        ret = aim_artixus_enable_interrupts(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_artixus_dma_init(aimDevice);
        if(ret)
        {
            break;
        }


        /* Now get the basic device properties via MyMon library */
        aimDevice->properties.serial      = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_SERIAL* sizeof(__u32));
        aimDevice->properties.config      = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_BOARDCFG* sizeof(__u32));
        aimDevice->properties.type        = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_BOARDTYPE* sizeof(__u32));
        aimDevice->properties.sub_type    = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_FRONTEND_CAP* sizeof(__u32));
        aimDevice->properties.hw_variant  = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_HWVARIANT* sizeof(__u32));
        aimDevice->properties.part_number = aim_pci_io_read32(aimDevice, ARTIXUS_REG_NOVRAM_PARTNO* sizeof(__u32));

        aim_dev_debug(aimDevice, "Serial: %u", aimDevice->properties.serial);
        aim_dev_debug(aimDevice, "Board Config: 0x%x", aimDevice->properties.config);
        aim_dev_debug(aimDevice, "Board Type: 0x%x", aimDevice->properties.type);
        aim_dev_debug(aimDevice, "Board Subtype: 0x%x", aimDevice->properties.sub_type);
        aim_dev_debug(aimDevice, "Hardware Variant: 0x%x", aimDevice->properties.hw_variant);
        aim_dev_debug(aimDevice, "Part Number: %u", aimDevice->properties.part_number);

    }while(0);


    if(ret)
    {
        aim_artixus_device_hw_remove(aimDevice);
    }

    return ret;
}


void aim_artixus_device_hw_remove(struct aim_pci_device* deviceInfo)
{
    aim_artixus_disable_interrupts(deviceInfo);

    aim_artixus_unmap_memory(deviceInfo);
}


int aim_artixus_enable_interrupts(struct aim_pci_device* device)
{
    __u32 mask = 0;
    int ret = 0;

    BUG_ON(!device || !device->ioMemory);

    /* Clear outstanding pending interrupts */
    aim_pci_io_write32(device, ARTIXUS_REG_IRQ_HOST_EVENT * sizeof(__u32), ARTIXUS_IRQ_ALL);


    tasklet_init(&device->irqTasklet, aim_artixus_irq_task, (unsigned long) device);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    /* Try to enable MSI interrupts for kernel versions that support pci_alloc_irq_vectors and request_irq. */
    ret = pci_alloc_irq_vectors(device->pciDevice, 1, 1, PCI_IRQ_MSI);
    if (ret > 0)
    {
        device->hasMsiEnabled = TRUE;
        /* Override PCI irq number with MSI vector for request_irq and free_irq */
        device->pciDevice->irq = pci_irq_vector(device->pciDevice, 0);
        aim_dev_info(device, "Allocated MSI vector %d.\n", device->pciDevice->irq);
    }
    else
    {
        device->hasMsiEnabled = FALSE;
        aim_dev_warning(device, "Failed to allocate a MSI vector. Fallback to legacy interrupts.\n");
    }
#endif   

    ret = request_irq(device->pciDevice->irq, aim_artixus_irq_handler, IRQF_SHARED, dev_name(&device->pciDevice->dev), device);
    if(ret)
    {
        aim_dev_error(device, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(device, "Using IRQ %d\n", device->pciDevice->irq);

    /* Now unmask all interrupts we're interested in */
    mask = aim_pci_io_read32(device, ARTIXUS_REG_IRQ_HOST_MASK * sizeof(__u32));

    mask |= ARTIXUS_IRQ_ALL;

    aim_pci_io_write32( device, ARTIXUS_REG_IRQ_HOST_MASK * sizeof(__u32), mask);

    aim_dev_debug(device, "IRQ mask: 0x%x", mask);

    return 0;
}


void aim_artixus_disable_interrupts(struct aim_pci_device* device)
{
    __u32 irqMask = 0;

    BUG_ON(!device || !device->ioMemory);

    aim_dev_debug(device, "Disabling interrupts\n");

    irqMask = aim_pci_io_read32(device, ARTIXUS_REG_IRQ_HOST_MASK * sizeof(__u32));
    irqMask &= ~ARTIXUS_IRQ_ALL;

    aim_dev_debug(device, "IRQ Mask after disabling interrupts is 0x%x\n", irqMask);

    aim_pci_io_write32(device, ARTIXUS_REG_IRQ_HOST_MASK * sizeof(__u32), irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(device, ARTIXUS_REG_IRQ_HOST_EVENT * sizeof(__u32), ARTIXUS_IRQ_ALL);

    free_irq(device->pciDevice->irq, device);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    if(device->hasMsiEnabled)
        pci_free_irq_vectors(device->pciDevice);
#endif    
}

