/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_zynqmp_device.c
 *
 *  This file contains hardware related definitions
 *  specific for AYXX platform
 *
 */

#include <linux/delay.h>
#include <linux/types.h>
#include "aim_zynqmp_device.h"
#include "aim_pci_mem.h"
#include "Ai_ringbuffer.h"




// -- ZynqMP memory bar description ---
//
//   BAR  |  TYPE        | WINDOW    | REAL SIZE            |   DETECTION                        |  COMMENT
//   ---------------------------------------------------------------------------------------------------------------------------
//    0   | DMA          | 1  MB     |                      |                                    | Required for interrupt handling and DMA, 4KB size does not work!
//    2   | Global RAM   | 32 MB     | 16MB                 | GLOBAL RAM Mark @0x007C (MIL)      |
//        |              |           |                      | GLOBAL RAM Mark @0x003C (CIV,AFDX) |
//        | Host-IO RAM  |           |  1MB                 |                                    |
//        | Shared RAM   |           | 15MB                 | SHARED RAM Mark @0x0000            |
//    4   | IO           | 1  MB     |                      |                                    |
//   ----------------------------------------------------------------------------------------------------------------------------
//
//  APXX3910,           DMA 1 MB, global 16MB, Host-IO 1MB, shared 15MB, IO 1 MB  // 64 bit memory bars

/*! \brief Maps PCI memory bar registers on AYXX boards
 *
 * @param aimDevice the device to map bar registers of
 * @return 0 on success, errno code otherwise
 */
static int aim_zynqmp_map_memory(struct aim_pci_device* aimDevice)
{
    struct pci_dev* pciDevice = NULL;
    resource_size_t barSize = 0;
    int ret;

    BUG_ON(!aimDevice || !aimDevice->pciDevice);

    pciDevice = aimDevice->pciDevice;


    ret = pci_request_regions(pciDevice, AIM_DRV_NAME);

    if (ret) {
        aim_dev_error(aimDevice, "request regions failed\n");
        pci_disable_device(pciDevice);
        return ret;
    }


    barSize = pci_resource_len(pciDevice, ZYNQMP_GLOBAL_SHARED_MEM_BAR);

    if( barSize != (ZYNQMP_GLOBAL_MEM_SIZE + ZYNQMP_HOSTIO_MEM_SIZE + ZYNQMP_SHARED_MEM_SIZE) )
    {
        aim_dev_error(aimDevice, "Invalid size for global/shared memory 0x%llx != 0x%llx\n",
                                        barSize,
                                        (ZYNQMP_GLOBAL_MEM_SIZE + ZYNQMP_SHARED_MEM_SIZE));
        return -ENOMEM;
    }

    aimDevice->globalMemoryPhysical = pci_resource_start(pciDevice,ZYNQMP_GLOBAL_SHARED_MEM_BAR);
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, ZYNQMP_GLOBAL_MEM_SIZE);

    if(!aimDevice->globalMemory) {
        aim_dev_error(aimDevice, "Mapping of global memory 0x%llx-0x%llx failed\n",
                                  aimDevice->globalMemoryPhysical,
                                  aimDevice->globalMemoryPhysical + ZYNQMP_GLOBAL_MEM_SIZE);
        return -ENOMEM;
    }

    aim_dev_info(aimDevice, "Global memory 0x%llx-0x%llx mapped to 0x%p\n",
                                aimDevice->globalMemoryPhysical,
                                aimDevice->globalMemoryPhysical + ZYNQMP_GLOBAL_MEM_SIZE,
                                aimDevice->globalMemory);

    aimDevice->globalMemorySize = ZYNQMP_GLOBAL_MEM_SIZE;


    aimDevice->hostIoMemoryPhysical  = pci_resource_start(pciDevice,ZYNQMP_GLOBAL_SHARED_MEM_BAR);
    aimDevice->hostIoMemoryPhysical += ZYNQMP_GLOBAL_MEM_SIZE;
    aimDevice->hostIoMemory = ioremap(aimDevice->hostIoMemoryPhysical, ZYNQMP_HOSTIO_MEM_SIZE);

    if(!aimDevice->hostIoMemory) {
        aim_dev_error(aimDevice, "Mapping of Host-IO memory 0x%llx-0x%llx failed\n",
                                  aimDevice->sharedMemoryPhysical,
                                  aimDevice->sharedMemoryPhysical + ZYNQMP_HOSTIO_MEM_SIZE);
        return -ENOMEM;
    }

    aim_dev_info(aimDevice, "Shared memory 0x%llx-0x%llx mapped to 0x%p\n",
                                aimDevice->hostIoMemoryPhysical,
                                aimDevice->hostIoMemoryPhysical + ZYNQMP_HOSTIO_MEM_SIZE,
                                aimDevice->hostIoMemory);

    aimDevice->hostIoMemorySize = ZYNQMP_HOSTIO_MEM_SIZE;




    aimDevice->sharedMemoryPhysical  = pci_resource_start(pciDevice,ZYNQMP_GLOBAL_SHARED_MEM_BAR);
    aimDevice->sharedMemoryPhysical += ZYNQMP_GLOBAL_MEM_SIZE + ZYNQMP_HOSTIO_MEM_SIZE;
    aimDevice->sharedMemory = ioremap(aimDevice->sharedMemoryPhysical, ZYNQMP_SHARED_MEM_SIZE);

    if(!aimDevice->sharedMemory) {
        aim_dev_error(aimDevice, "Mapping of shared memory 0x%llx-0x%llx failed\n",
                                  aimDevice->sharedMemoryPhysical,
                                  aimDevice->sharedMemoryPhysical + ZYNQMP_SHARED_MEM_SIZE);
        return -ENOMEM;
    }

    aim_dev_info(aimDevice, "Shared memory 0x%llx-0x%llx mapped to 0x%p\n",
                                aimDevice->sharedMemoryPhysical,
                                aimDevice->sharedMemoryPhysical + ZYNQMP_SHARED_MEM_SIZE,
                                aimDevice->sharedMemory);

    aimDevice->sharedMemorySize = ZYNQMP_SHARED_MEM_SIZE;

    barSize = pci_resource_len(pciDevice, ZYNQMP_IO_MEM_BAR);
    aimDevice->ioMemoryPhysical = pci_resource_start(pciDevice,ZYNQMP_IO_MEM_BAR);
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);

    if(!aimDevice->ioMemory) {
        aim_dev_error(aimDevice, "Mapping of IO memory 0x%llx-0x%llx failed\n",
                                  aimDevice->ioMemoryPhysical,
                                  aimDevice->ioMemoryPhysical + barSize);
        return -ENOMEM;
    }

    aim_dev_info(aimDevice, "IO memory 0x%llx-0x%llx mapped to 0x%p\n",
                                aimDevice->ioMemoryPhysical,
                                aimDevice->ioMemoryPhysical + barSize,
                                aimDevice->ioMemory);

    aimDevice->ioMemorySize = barSize;



    barSize = pci_resource_len(pciDevice, ZYNQMP_DMA_MEM_BAR);
    aimDevice->dmaMemoryPhysical = pci_resource_start(pciDevice,ZYNQMP_DMA_MEM_BAR);
    aimDevice->dmaMemory = ioremap(aimDevice->dmaMemoryPhysical, barSize);

    if(!aimDevice->dmaMemory) {
        aim_dev_error(aimDevice, "Mapping of DMA memory 0x%llx-0x%llx failed\n",
                                  aimDevice->dmaMemoryPhysical,
                                  aimDevice->dmaMemoryPhysical + barSize);
        return -ENOMEM;
    }

    aim_dev_info(aimDevice, "DMA memory bar %d 0x%llx-0x%llx mapped to 0x%p\n",
                                ZYNQMP_DMA_MEM_BAR,
                                aimDevice->dmaMemoryPhysical,
                                aimDevice->dmaMemoryPhysical + barSize,
                                aimDevice->dmaMemory);

    aimDevice->dmaMemorySize = barSize;

    return 0;
}


/*! \brief Unmaps PCI memory bars of a device
 *
 * @param aimDevice the device to unmap memory bars of
 */
static void aim_zynqmp_unmap_memory(struct aim_pci_device* aimDevice)
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

    if(aimDevice->dmaMemory)
    {
        aim_dev_debug(aimDevice, "Unmapping DMA memory\n");
        iounmap(aimDevice->dmaMemory);
        aimDevice->dmaMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}

/*! \brief Maps PCI memory bar registers on AYXX boards
 *
 * @param aimDevice the device to map bar registers of
 * @return 0 on success, errno code otherwise
 */
static int aim_zynqmp_wait_for_bridge_init(struct aim_pci_device* aimDevice)
{
    int i = 0;
    L_WORD dma_scratch0;

    iowrite32(AI_HW_ZYNQMP_DMAREG_SWINT, aimDevice->dmaMemory + AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_AXI_EVENT_REG );


    for(i=0; i<100; i++)
    {
        i++;
        dma_scratch0 = ioread32( aimDevice->dmaMemory + AI_HW_ZYNQMP_DMAREG_SCRATCH0 );

        aim_dev_info(aimDevice, "DMA SCRATCH0 = 0x%08X\n", dma_scratch0 );
        
        if( dma_scratch0 == AI_HW_ZYNQMP_AZMPA2P_BRIDGE_READY)
            return 0;
        
        msleep(100);
    }
    
    return -ETIME;
}


/*! \brief IRQ Bottom Half handler
 *
 * This function is scheduled via a tasklet and will handle the bottom half
 * of a device's interrupts
 * @param data pointer to \ref struct aim_pci_device of the device to handle interrupts on
 */
static void aim_zynqmp_irq_task(unsigned long data)
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
static irqreturn_t aim_zynqmp_irq_handler(int irq, void* arg)
{
    bool int_handled = false;
    int i, event;
    u32 put, get, available_data;
    struct aim_pci_device* device = (struct aim_pci_device*) arg;
    struct aim_pci_com_channel* channel = NULL;

    /* -- check if interrupt is from our device --- */

    event = ioread32(device->dmaMemory + AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_EVENT_REG );

    if (  (event & AI_HW_ZYNQMP_DMAREG_SGLINT)
       || (event & AI_HW_ZYNQMP_DMAREG_ERRINT))
    {
        int_handled = true;
        iowrite32(AI_HW_ZYNQMP_DMAREG_SGLINT | AI_HW_ZYNQMP_DMAREG_ERRINT, device->dmaMemory + AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_EVENT_REG );

        aim_pci_device_dma_complete( device );
    }


    if( (event & AI_HW_ZYNQMP_DMAREG_SWINT) != 0 )
    {
        int_handled = true;
        iowrite32(AI_HW_ZYNQMP_DMAREG_SWINT, device->dmaMemory + AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_EVENT_REG );

        /* -- get event from loglist --- */
        get = ioread32(device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_GET);
        put = ioread32(device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_PUT);

        available_data = ai_ringbuffer_consumer_available_bytes(AI_HW_ZYNQMP_HOSTIO_LOG_BYTE_SIZE, put, get);

        aim_dev_debug(device, "Found get=%d put=%d data=%d\n", get, put, available_data/HOSTIO_LOG_ENTRY_SIZE );

        for( i=0; i< (available_data/AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE); i++ )
        {
            event = ioread32(device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + get);

            switch(event)
            {
            case AIM_ZYNQMP_HOSTIO_CHN0:
                /* Maintenance com channel event */
                channel = radix_tree_lookup(&device->pciComChannels, 0);
                BUG_ON(!channel);
                complete(&channel->commandCompletion);
                break;
            case AIM_ZYNQMP_HOSTIO_CHN1:
                /* ASP com channel event */
                channel = radix_tree_lookup(&device->pciComChannels, 1);
                BUG_ON(!channel);
                complete(&channel->commandCompletion);
                break;
            case AIM_ZYNQMP_HOSTIO_ASP2HOST_INT:
                /* ASP to Host interrupt event */
                atomic_inc(&device->pendingIrqs[AIM_PCI_ASP_IRQ]);
                tasklet_schedule(&device->irqTasklet);
                break;
            default:
                aim_dev_error(device, "Unknown event get=%d put=%d event=%d\n", get, put, event );
                break;
            }

            ai_ringbuffer_increment_offset(&get, AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE, AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_BASE, AI_HW_ZYNQMP_HOSTIO_LOG_BYTE_SIZE);
            iowrite32(get, device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_GET);
        }
    }

    if(int_handled)
    {
        return IRQ_HANDLED;
    }
    else
    {
        return IRQ_NONE;
    }
}



/*! \brief Maps PCI memory bar registers on AYXX boards
 *
 * @param aimDevice the device to map bar registers of
 * @return 0 on success, errno code otherwise
 */
int aim_zynqmp_create_interrupt_on_device(struct aim_pci_device* device, enum ai_hw_zynqmp_hostio_event event )
{
    int ret = 0;
    int put, get, free_bytes;

    mutex_lock(&device->zynqmp_hostio_lock);

    /* -- get event from loglist --- */
    get = ioread32(device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_AXI_GET);
    put = ioread32(device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_AXI_PUT);

    free_bytes = ai_ringbuffer_producer_free_bytes(AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE, AI_HW_ZYNQMP_HOSTIO_LOG_BYTE_SIZE, put, get);

    if (AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE > free_bytes)
    {
        iowrite32(AI_HW_ZYNQMP_HOSTIO_LOG_OVERFLOW, device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_ERROR );
        aim_dev_error(device, "No space left in Host-IO event loglist\n");
        ret = -EXFULL;
    }
    else
    {
        /* -- Insert entry --- */
        iowrite32(event, device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + put );

        /* -- Update put --- */
        ai_ringbuffer_increment_offset(&put, AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE, AI_HW_ZYNQMP_HOSTIO_LOG_AXI_BASE, AI_HW_ZYNQMP_HOSTIO_LOG_BYTE_SIZE);

        iowrite32(put, device->hostIoMemory + AI_HW_ZYNQMP_HOSTIO_IRQ_LOG + AI_HW_ZYNQMP_HOSTIO_LOG_AXI_PUT );

    }

    wmb();
    /* -- create interrupt --- */
    iowrite32(AI_HW_ZYNQMP_DMAREG_SWINT, device->dmaMemory + AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_AXI_EVENT_REG );

    mutex_unlock(&device->zynqmp_hostio_lock);

    return 0;
}





int aim_zynqmp_enable_interrupts(struct aim_pci_device* device)
{
    int ret = 0;
    u32 pcie_int_mask;

    BUG_ON(!device || !device->ioMemory);

    tasklet_init(&device->irqTasklet, aim_zynqmp_irq_task, (unsigned long) device);

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

    ret = request_irq(device->pciDevice->irq, aim_zynqmp_irq_handler, IRQF_SHARED, dev_name(&device->pciDevice->dev), device);
    if( ret )
    {
        aim_dev_error(device, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(device, "Using IRQ %d\n", device->pciDevice->irq);


    pcie_int_mask = ioread32(device->dmaMemory + AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK_OFFSET);

    pcie_int_mask |= (AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK | AI_HW_ZYNQMP_DMAREG_SGLINT | AI_HW_ZYNQMP_DMAREG_ERRINT);

    iowrite32(pcie_int_mask, device->dmaMemory + AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK_OFFSET );

    return 0;
}


void aim_zynqmp_disable_interrupts(struct aim_pci_device* device)
{
    u32 pcie_int_mask;

    aim_dev_debug(device, "Disabling interrupts\n");

    pcie_int_mask = ioread32(device->dmaMemory + AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK_OFFSET);

    pcie_int_mask &= ~(AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK | AI_HW_ZYNQMP_DMAREG_SGLINT | AI_HW_ZYNQMP_DMAREG_ERRINT);

    iowrite32(pcie_int_mask, device->dmaMemory + AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK_OFFSET );

    free_irq(device->pciDevice->irq, device);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    if(device->hasMsiEnabled)
        pci_free_irq_vectors(device->pciDevice);
#endif       
}


static int aim_zynqmp_enable_dma_channel(struct aim_pci_device* aimDevice)
{
    int ret = 0;
    AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel;

    aim_dev_debug(device, "Using 64-Bit DMA width for ZynqMP platform\n");
    ret = dma_set_mask(&aimDevice->pciDevice->dev, DMA_BIT_MASK(64));

    if(ret)
    {
        aim_dev_error(aimDevice, "Failed to set DMA mask to 64 bit");
        return ret;
    }

    channel = kmalloc(sizeof(AIMPCI_HW_ZYNQMP_DMA_CHANNEL), GFP_KERNEL);

    if(!channel)
    {
        aim_dev_error(aimDevice, "Failed to allocate dma channel struct");
        return -ENOMEM;
    }

    channel->id = AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA;

    ret = aim_zynqmp_dma_channel_allocate(aimDevice, channel);

    if(ret)
    {
        kfree(channel);
        aim_dev_error(aimDevice, "Failed to allocate dma channel members (%d)", ret);
        return ret;
    }

    radix_tree_insert(&aimDevice->pciDmaChannels, channel->id, channel);


    return 0;
}


int aim_zynqmp_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_zynqmp_map_memory(aimDevice);
        if( ret )
        {
            break;
        }

        ret = aim_zynqmp_wait_for_bridge_init(aimDevice);

        if( ret )
        {
            break;
        }

        ret = aim_zynqmp_enable_interrupts(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_zynqmp_enable_dma_channel(aimDevice);

        if(ret)
        {
            break;
        }


    } while( 0 );

    if( ret )
    {
        aim_zynqmp_device_hw_remove(aimDevice);
    }

    return ret;
}

static int aim_zynqmp_free_dma_channel(struct aim_pci_device* aimDevice)
{
    AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel;

    channel = radix_tree_lookup(&aimDevice->pciDmaChannels, AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA);

    if(!channel)
        /* Channel 0 not in tree */
        return 0;

    radix_tree_delete(&aimDevice->pciDmaChannels, AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA);

    aim_zynqmp_dma_channel_stop(channel);

    aim_zynqmp_dma_free(channel);

    kfree(channel);

    return 0;
}

void aim_zynqmp_device_hw_remove(struct aim_pci_device* aimDevice)
{

    aim_zynqmp_free_dma_channel(aimDevice);

    aim_zynqmp_disable_interrupts(aimDevice);

    aim_zynqmp_unmap_memory(aimDevice);
}

