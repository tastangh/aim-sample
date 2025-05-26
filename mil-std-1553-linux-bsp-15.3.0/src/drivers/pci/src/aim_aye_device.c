/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aye_device.c
 *
 *  This file contains definitions for the
 *  AYE platform devices
 *
 */


#include <linux/types.h>
#include "aim_aye_device.h"
#include "aim_debug.h"
#include "aim_pci_mymon.h"
#include "aim_pci_mem.h"
#include "aim_kernel_mem.h"
#include "hw/AiHwAye.h"
#include "aim_dma.h"
#include "aim_aye_ds_device.h"



/*! \def AYE_GLOBAL_MEM_BAR
 *
 * Bar ID of global memory
 */
#define AYE_GLOBAL_MEM_BAR 0


/*! \def AYE_IO_MEM_BAR
 *
 * Bar ID of IO memory
 */
#define AYE_IO_MEM_BAR 1


/*! \def AYE_GLOBAL_EXT_MEM_BAR
 *
 * Bar ID of global extension memory
 */
#define AYE_GLOBAL_EXT_MEM_BAR 2


/*! \def AYE_SHARED_MEMORY_SIZE
 *
 * Size of virtual shared memory in bytes
 */
#define AYE_SHARED_MEMORY_SIZE (16 << 20) /* 16MB */


/*! \def AYE_MAX_BIU
 *
 * Maximum number of logical BIUs on AYE boards
 */
#define AYE_MAX_BIU 4


/*! \def AYE_DMA_WIDTH
 *
 * AYE boards can access 64-Bit addresses
 */
#define AYE_DMA_WIDTH 64




/*! \brief Initializes DMA on AYE boards
 *
 * Will set DMA mask of the device and start DMA memory channel 0
 * @param device device to initialize DMA for
 * @return returns 0 on success, an errno code otherwise
 */
static int __inline__ aim_aye_dma_init(struct aim_pci_device* device)
{
    int ret = 0;

    aim_dev_debug(device, "Using %d-Bit DMA width for AYE platform\n", AYE_DMA_WIDTH);
    ret = dma_set_mask(&device->pciDevice->dev, DMA_BIT_MASK(AYE_DMA_WIDTH));
    if(ret)
    {
        aim_dev_error(device, "Failed to set DMA mask to %d bit", AYE_DMA_WIDTH);
        return ret;
    }

    /* Start memory DMA channel 0 */
    aim_pci_io_write32(device, AYE_REG_DMA_MEM0_CONTROL * sizeof(__u32), AYE_DMA_CTRL_START);

    return ret;
}




/*! \brief Maps memory of AYE boards
 *
 *  Maps global, IO and global extension memory PCI bars
 *  and allocates a virtual shared memory area in kernel space
 * @param aimDevice PCI device to map bars of
 * @return 0 on success, errno code otherwise
 */
static int aim_aye_map_memory(struct aim_pci_device* aimDevice)
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

    barSize = pci_resource_len(pciDevice, AYE_GLOBAL_MEM_BAR);
    aimDevice->globalMemoryPhysical = pci_resource_start(pciDevice,
							 AYE_GLOBAL_MEM_BAR);
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, barSize);
    if(!aimDevice->globalMemory) {
        aim_dev_error(aimDevice, "Mapping of global memory %llu-%llu failed\n",
		      aimDevice->globalMemoryPhysical,
		      aimDevice->globalMemoryPhysical + barSize);
        return -ENOMEM;
    }

    aimDevice->globalMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped global memory bar to %p with size %llu KB\n",
		 aimDevice->globalMemory,
		 (unsigned long long)aimDevice->globalMemorySize / 1024);

    barSize = pci_resource_len(pciDevice, AYE_IO_MEM_BAR);
    aimDevice->ioMemoryPhysical = pci_resource_start(pciDevice, AYE_IO_MEM_BAR);
    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);
    if(!aimDevice->ioMemory) {
        aim_dev_error(aimDevice, "Mapping of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n",
		 aimDevice->ioMemory,
                 (unsigned long long)aimDevice->ioMemorySize / 1024);

    barSize = pci_resource_len(pciDevice, AYE_GLOBAL_EXT_MEM_BAR);
    aimDevice->globalExtMemoryPhysical =
	    pci_resource_start(pciDevice, AYE_GLOBAL_EXT_MEM_BAR);
    aimDevice->globalExtMemory =
	    ioremap(aimDevice->globalExtMemoryPhysical, barSize);
    if(!aimDevice->globalExtMemory) {
        aim_dev_error(aimDevice, "Mapping of global extension memory failed\n");
        return -ENOMEM;
    }

    aimDevice->globalExtMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped global extension memory bar to %p with size %llu KB\n", aimDevice->globalExtMemory,
		 (unsigned long long)aimDevice->globalExtMemorySize / 1024);

    /* As AYE boards do not have an ASP, we allocate a virtual shared memory */
    aimDevice->sharedMemory = vzalloc(AYE_SHARED_MEMORY_SIZE);
    if(!aimDevice->sharedMemory)
    {
        aim_dev_error(aimDevice, "Allocation of shared memory failed\n");
        return -ENOMEM;
    }

    aimDevice->sharedMemorySize = AYE_SHARED_MEMORY_SIZE;

    aim_dev_info(aimDevice, "Allocated shared memory to %p with size %llu KB\n", aimDevice->sharedMemory,
                 (unsigned long long) aimDevice->sharedMemorySize / 1024);

    return 0;
}


/*! brief Unmap memory of device
 *
 * @param aimDevice PCI device to unmap bar registers of
 */
static void aim_aye_unmap_memory(struct aim_pci_device* aimDevice)
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

    if(aimDevice->globalExtMemory)
    {
        aim_dev_debug(aimDevice, "Unmapping Global Extension Memory\n");
        iounmap(aimDevice->globalExtMemory);
        aimDevice->globalExtMemory = NULL;
    }

    if(aimDevice->sharedMemory)
    {
        aim_dev_debug(aimDevice, "Freeing shared memory\n");
        vfree(aimDevice->sharedMemory);
        aimDevice->sharedMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}


/*! brief Wait until FPGA/uMon is started
 *
 * @param aimDevice PCI device to wait for
 */
static int aim_aye_d0_wait(struct aim_pci_device* device)
{
    u32 i        = 0;
    u32 ulValue1 = 0;
    u32 ulValue2 = 0;
    u32 ulValue3 = 0;
    u32 ulValue4 = 0;

    aim_dev_info(device, "aim_aye_d0_wait: Wait for MyMON ready signal.\n" );

    while( ++i != 0x00100000 )
    {
        // use i as hard timeout in case MyMon does not start

        // OP-Reg 1: 0xcafe0001 => DDRAM is valid, NOVRAM is copied (Step 3)
        // OP-Reg 2: 0xcafe0002 => BIUs / ASP booted.(Step 5)
        // OP-Reg 3: 0xcafe0003 => IRQ controller is enabled, Device is ready

        ulValue1 = aim_pci_io_read32( device, MYMON_REG_OUTPUT_0 );
        ulValue2 = aim_pci_io_read32( device, MYMON_REG_OUTPUT_1 );
        ulValue3 = aim_pci_io_read32( device, MYMON_REG_OUTPUT_2 );
        ulValue4 = aim_pci_io_read32( device, MYMON_REG_BOOTSTATUS );

        if( ulValue3 == 0xCAFE0003 )
            // If the device is bootet the boot status is written in the output registers.
            // First LCA versions.
            break;

        if( ulValue4 == 0xCAFE0003 )
            // If the device is not rebootet the output registers
            // may contain an invalid status. In this case we have to check the
            // boot status register which stays the same until a reset is received.
            // Second LCA versions.
            break;

        if( (i%0x10000) == 0 )
        {
            aim_dev_info(device, "aim_aye_d0_wait: i = 0x%08X, O1 = 0x%08X, O2 = 0x%08X, O3 = 0x%08X, BS = 0x%08X\n", i, ulValue1, ulValue2, ulValue3, ulValue4 );
        }
    }


    if( i < 0x00100000 )
    {
        // device ready
        aim_dev_info(device, "aim_aye_d0_wait: i = 0x%08X, O1 = 0x%08X, O2 = 0x%08X, O3 = 0x%08X, BS = 0x%08X\n", i, ulValue1, ulValue2, ulValue3, ulValue4 );
        aim_dev_info(device, "aim_aye_d0_wait: Device ready\n" );
        return 0;
    }
    else
    {
        // time-out
        aim_dev_info(device, "aim_aye_d0_wait: i = 0x%08X, O1 = 0x%08X, O2 = 0x%08X, O3 = 0x%08X, BS = 0x%08X \n", i, ulValue1, ulValue2, ulValue3, ulValue4 );
        aim_dev_error(device, "aim_aye_d0_wait: Device timeout\n" );
        return -EAGAIN;
    }

}


/*! \brief Prepare MyMon access for device
 *
 * This function will initialize the device's MyMon data so it
 * is able to access MyMon specific memory.
 * @param device the device to initialize MyMon access for
 * @return 0 on success, errno code otherwise
 */
static int aim_aye_prepare_mymon(struct aim_pci_device* device)
{
    TyMyMonContext* mymonData = NULL;

    BUG_ON(!device);

    if(!device->mymonData)
    {
        device->mymonData = kzalloc(sizeof(TyMyMonContext), GFP_KERNEL);
        if(!device->mymonData)
        {
            aim_dev_error(device, "Failed to allocate mymon data");
            return -ENOMEM;
        }
    }

    mymonData = device->mymonData;

    mymonData->pxUserData = &device->mymonUserData;
    mymonData->pxUserData->device = device;

    mymonData->pfReadIo = aim_pci_mymon_io_read;
    mymonData->pfWriteIo = aim_pci_mymon_io_write;
    mymonData->pfReadGlobalExt = aim_pci_mymon_glob_ext_read;
    mymonData->pfWriteGlobalExt = aim_pci_mymon_glob_ext_write;

    mymonData->pfWait = aim_pci_mymon_wait;

    return 0;
}


/*! \brief Free MyMon data of device
 *
 * @param device the device to free MyMon data of
 */
static void aim_aye_free_mymon(struct aim_pci_device* device)
{
    BUG_ON(!device);

    if(device->mymonData)
    {
        kfree(device->mymonData);
        device->mymonData = NULL;
    }
}


/*! \brief aim_aye_irq_handler
 *
 * Top-Half IRQ handler function
 * @param irq IRQ to handle
 * @param arg pointer to \ref struct aim_pci_device the IRQ needs to be handled for
 * @return IRQ_HANDLED, if interrupt was issued by the device, IRQ_NONE otherwise
 */
static irqreturn_t aim_aye_irq_handler(int irq, void* arg)
{
    __u32 irqEvent = 0;
    struct aim_pci_device* device = (struct aim_pci_device*) arg;
    bool needBottomHalf = false;

    /* Check the IRQ event register and return in case
     * no event, we're responsible for, was raised
     */
    irqEvent = aim_pci_io_read32(device, AEC_REG_IRQ_HOST_EVENT * sizeof(__u32));

    if(!(irqEvent & AEC_IRQ_ALL))
    {
        return IRQ_NONE;
    }

    aim_dev_debug(device, "Irq Event: 0x%08x\n", irqEvent);

    /* Acknowledge all events we will process further on */
    aim_pci_io_write32(device, AEC_REG_IRQ_HOST_EVENT * sizeof(__u32), irqEvent & AEC_IRQ_ALL);

    if(irqEvent & AEC_IRQ_BIU_1)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU1_IRQ]);
        aim_dev_debug(device, "Received BIU 1 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU1_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & AEC_IRQ_BIU_2)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU2_IRQ]);
        aim_dev_debug(device, "Received BIU 2 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU2_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & AEC_IRQ_BIU_3)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU3_IRQ]);
        aim_dev_debug(device, "Received BIU 3 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU3_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & AEC_IRQ_BIU_4)
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_BIU4_IRQ]);
        aim_dev_debug(device, "Received BIU 4 interrupt. %d still pending\n", atomic_read(&device->pendingIrqs[AIM_PCI_BIU4_IRQ]));
        needBottomHalf = true;
    }

    if(irqEvent & AEC_IRQ_DMA_MEM0)
    {
        /* Got interrupt on DMA channel 0
         * Get first DMA token from device global queue and complete
         * in order to wake up waiting processes
         */
        aim_dev_debug(device, "Received memory DMA channel 0 interrupt.");

        aim_pci_device_dma_complete( device );
    }

#ifdef AIM_AYE_DS
    if((irqEvent & AEC_IRQ_DMA_MS) == AEC_IRQ_DMA_MS )
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_DS_IRQ]);
        needBottomHalf = true;
    }
    if((irqEvent & AEC_IRQ_DMA_MS_OVERFLOW) == AEC_IRQ_DMA_MS_OVERFLOW )
    {
        atomic_inc(&device->pendingIrqs[AIM_PCI_DS_ERR_IRQ]);
        needBottomHalf = true;
    }
#endif

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
static void aim_aye_irq_task(unsigned long data)
{
    int i = 0;
    struct aim_pci_device* device = (struct aim_pci_device*) data;

    BUG_ON(!device);

    for(i = 0; i < AYE_MAX_BIU; i++)
    {
        aim_pci_device_handle_biu_irqs(device, i);
    }

#ifdef AIM_AYE_DS
    while(atomic_read(&device->pendingIrqs[AIM_PCI_DS_IRQ]) > 0)
    {
        atomic_dec(&device->pendingIrqs[AIM_PCI_DS_IRQ]);
        aim_aye_ds_device_irq_buffer_finished( device );
    }

    while(atomic_read(&device->pendingIrqs[AIM_PCI_DS_ERR_IRQ]) > 0)
    {
        atomic_dec(&device->pendingIrqs[AIM_PCI_DS_ERR_IRQ]);
        aim_aye_ds_device_irq_buffer_error( device );
    }
#endif
}


int aim_aye_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_aye_map_memory(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_aye_d0_wait(aimDevice);

        if(ret)
        {
            break;
        }

        ret = aim_aye_prepare_mymon(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_aye_enable_interrupts(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_aye_dma_init(aimDevice);
        if(ret)
        {
            break;
        }

        /* Now get the basic device properties via MyMon library */
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_SERIAL, &aimDevice->properties.serial, 1);
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_BOARDCFG, &aimDevice->properties.config, 1);
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_BOARDTYPE, &aimDevice->properties.type, 1);
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_BOARDSUBTYP, &aimDevice->properties.sub_type, 1);
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_HWVARIANT, &aimDevice->properties.hw_variant, 1);
        aim_pci_mymon_glob_ext_read(aimDevice->mymonData, MYMON_GREG_NOVRAM_PARTNO, &aimDevice->properties.part_number, 1);

        aim_dev_debug(aimDevice, "Serial: %u", aimDevice->properties.serial);
        aim_dev_debug(aimDevice, "Board Config: 0x%x", aimDevice->properties.config);
        aim_dev_debug(aimDevice, "Board Type: 0x%x", aimDevice->properties.type);
        aim_dev_debug(aimDevice, "Board Subtype: 0x%x", aimDevice->properties.sub_type);
        aim_dev_debug(aimDevice, "Hardware Variant: 0x%x", aimDevice->properties.hw_variant);
        aim_dev_debug(aimDevice, "Part Number: %u", aimDevice->properties.part_number);

    }while(0);


    if(ret)
    {
        aim_aye_device_hw_remove(aimDevice);
    }

    return ret;
}


void aim_aye_device_hw_remove(struct aim_pci_device* deviceInfo)
{
    aim_aye_disable_interrupts(deviceInfo);

    aim_aye_free_mymon(deviceInfo);

    aim_aye_unmap_memory(deviceInfo);
}


int aim_aye_enable_interrupts(struct aim_pci_device* device)
{
    __u32 mask = 0;
    int ret = 0;

    BUG_ON(!device || !device->ioMemory);

    /* Clear outstanding pending interrupts */
    aim_pci_io_write32(device, AEC_REG_IRQ_HOST_EVENT * sizeof(__u32), AEC_IRQ_ALL);


    tasklet_init(&device->irqTasklet, aim_aye_irq_task, (unsigned long) device);

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

    /* Enable legacy interrupts for older kernel versions */
    ret = request_irq(device->pciDevice->irq, aim_aye_irq_handler, IRQF_SHARED, dev_name(&device->pciDevice->dev), device);
    if(ret)
    {
        aim_dev_error(device, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(device, "Using IRQ %d\n", device->pciDevice->irq);

    /* Now unmask all interrupts we're interested in */
    mask = aim_pci_io_read32(device, AEC_REG_IRQ_HOST_MASK * sizeof(__u32));

    mask |= AEC_IRQ_ALL;

    aim_pci_io_write32( device, AEC_REG_IRQ_HOST_MASK * sizeof(__u32), mask);

    aim_dev_debug(device, "IRQ mask: 0x%x", mask);

    return 0;
}


void aim_aye_disable_interrupts(struct aim_pci_device* device)
{
    __u32 irqMask = 0;

    BUG_ON(!device || !device->ioMemory);

    aim_dev_debug(device, "Disabling interrupts\n");

    irqMask = aim_pci_io_read32(device, AEC_REG_IRQ_HOST_MASK * sizeof(__u32));
    irqMask &= ~AEC_IRQ_ALL;

    aim_dev_debug(device, "IRQ Mask after disabling interrupts is 0x%x\n", irqMask);

    aim_pci_io_write32(device, AEC_REG_IRQ_HOST_MASK * sizeof(__u32), irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(device, AEC_REG_IRQ_HOST_EVENT * sizeof(__u32), AEC_IRQ_ALL);

    free_irq(device->pciDevice->irq, device);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    if(device->hasMsiEnabled)
        pci_free_irq_vectors(device->pciDevice);
#endif
}


int aim_aye_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType,
                                   loff_t offset)
{
    int ret = 0;
    TY_AYE_DMA_DMA_SGL_SETUP dmaSetup = {0};
    struct scatterlist* scatterlistEntry = NULL;
    unsigned int freeEntries = 0;
    bool sent = false;
    int i = 0;
    dma_addr_t dmaAddress = 0;

    BUG_ON(!device || !buffer);

    if( memType != AIM_MEMTYPE_GLOBAL )
    {
        aim_dev_error(device, "Invalid for AYE DMA: %d", memType );
        return -EINVAL;
    }

    if(buffer->scatterTable.nents == 0)
    {
        aim_dev_error(device, "Empty AYE DMA transaction:");
        return -EINVAL;
    }

    aim_dev_debug(device, "AYE DMA read with %u entries", buffer->scatterTable.nents);

    /* Initially read the number of free DMA FIFO entries */
    dmaSetup.ulHeaderCounter.all = aim_pci_io_read32(device, (AYE_REG_DMA_MEM0_HEADER_COUNTER*4));
    freeEntries = dmaSetup.ulHeaderCounter.part.count / 4;

    for_each_sg(buffer->scatterTable.sgl, scatterlistEntry, buffer->scatterTable.nents, i)
    {
       sent = false;

       do
       {
           if(freeEntries)
           {
               /* There's place for this scatter table entry in the FIFO.
                * So write it to board
                */
               dmaSetup.xSglEntry.ulControl.all            = 0;
               dmaSetup.xSglEntry.ulControl.part.DmaDest   = AyEDmaDestHost;
               dmaSetup.xSglEntry.ulControl.part.DmaSource = AyEDmaSourceGlobal;
               dmaSetup.xSglEntry.ulControl.part.Size      = sg_dma_len(scatterlistEntry);

               if( i == (buffer->scatterTable.nents - 1) )
               {
                   /* Generate interrupt for the last SGL item */
                   /* dmaSetup.xSglEntry.ulControl.part.Interrupt = AyEDmaEnableInt; */
                   dmaSetup.xSglEntry.ulControl.part.Interrupt = AyEDmaEnableInt;
               }

               dmaAddress = sg_dma_address(scatterlistEntry);
               dmaSetup.xSglEntry.ulPciAddressHigh = (dmaAddress & 0xFFFFFFFF00000000ULL) >> 32;
               dmaSetup.xSglEntry.ulPciAddressLow = dmaAddress & 0xFFFFFFFF;
               dmaSetup.xSglEntry.ulDeviceAddress = offset;

               /* DMA FIFO has be be accessed with 32-Bit writes
                * in correct order as FIFO internally consists of one single register only */
               aim_pci_io_write32( device, AYE_REG_DMA_MEM0_SGL_CONTROL*4, dmaSetup.xSglEntry.ulControl.all);
               wmb();
               aim_pci_io_write32( device, AYE_REG_DMA_MEM0_SGL_PCI_LOW*4, dmaSetup.xSglEntry.ulPciAddressLow  );
               wmb();
               aim_pci_io_write32( device, AYE_REG_DMA_MEM0_SGL_PCI_HIGH*4, dmaSetup.xSglEntry.ulPciAddressHigh );
               wmb();
               aim_pci_io_write32( device, AYE_REG_DMA_MEM0_SGL_DEVICE_LOW*4, dmaSetup.xSglEntry.ulDeviceAddress  );
               wmb();

               sent = true;
               freeEntries--;
               offset += dmaSetup.xSglEntry.ulControl.part.Size;
           }
           else
           {
               /* Re-read free DMA FIFO entry count */
               dmaSetup.ulHeaderCounter.all = aim_pci_io_read32(device, (AYE_REG_DMA_MEM0_HEADER_COUNTER*4));
               freeEntries = dmaSetup.ulHeaderCounter.part.count / 4;
           }

       }while(!sent);
    }

    return ret;
}


