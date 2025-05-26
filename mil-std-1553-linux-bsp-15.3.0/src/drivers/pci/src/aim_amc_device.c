/* SPDX-FileCopyrightText: 2015-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! aim_amc_device.c
 *
 *
 *  This file contains hardware related definitions
 *  specific for AIM AMC platform
 *
 */


#include <linux/errno.h>
#include "aim_amc_device.h"
#include "aim_debug.h"
#include "aim_pci_mem.h"
#include "type_def.h"
#include "prototypes.h"
#include "aim_kernel_mem.h"
#include "aim_pci_device_prot.h"




/*! \def AMC_GLOBAL_MEM_BAR
 * PCI Bar register that holds the global memory
 */
#define AMC_GLOBAL_MEM_BAR     0


/*! \def AMC_IO_MEM_BAR
 * PCI Bar register that holds the IO memory
 */
#define AMC_IO_MEM_BAR     1


/*! \def AMC_SHARED_MEM_BAR
 * PCI Bar register that holds the shared memory
 * on AMC devices with ASP
 */
#define AMC_SHARED_MEM_BAR     2


/*! \def AMC_VIRTUAL_SHARED_MEMORY_SIZE
 * Size in bytes of allocated shared memory for AMC devices without ASP
 */
#define AMC_VIRTUAL_SHARED_MEMORY_SIZE (16 << 20) /* 16 MB */


/*! \def MON_SREC_CONV_MEM_SIZE
 *
 * size of temporary srec conversion buffer in bytes
 */
#define MON_SREC_CONV_MEM_SIZE (4 << 20) /* 4 MB */


/*! \def AMC_MAX_BIUS
 * Maximum number of supported BIUs on AMC devices
 */
#define AMC_MAX_BIUS    4


/* These are the supported interrupt event types on AMC platform */
#define PMC_IRQ_EVENT_BIU1   (1 << 0)
#define PMC_IRQ_EVENT_BIU2   (1 << 1)
#define PMC_IRQ_EVENT_BIU3   (1 << 24)
#define PMC_IRQ_EVENT_BIU4   (1 << 25)
#define PMC_IRQ_EVENT_ASP    (1 << 6)


/*! \def PMC_IRQ_EVENTS
 * All interrupt events the driver will care for on AMC devices
 */
#define PMC_IRQ_EVENTS \
    (PMC_IRQ_EVENT_BIU1 | PMC_IRQ_EVENT_BIU2 | PMC_IRQ_EVENT_BIU3 | PMC_IRQ_EVENT_BIU4 | \
     PMC_IRQ_EVENT_ASP)


/*! \def PMC_PCI_IRQ_ENABLED
 * Bit in interrupt event register that will enable/disable PCI interrupts
 */
#define PMC_PCI_IRQ_ENABLED   (1 << 31)



int aim_amc_memory_detect_size( void __iomem * mem, resource_size_t mem_size, AiUInt32 ulMagic, resource_size_t ulMagicOffsetWord, resource_size_t * pulDetectedSize );



/*! \brief Maps the PCI bar registers of an AMC device
 *
 * This function will map the shared, global and the IO memory
 * bar of AMC devices.
 * @param aimDevice the device to map the bars of.
 * @return returns 0 on success, a negative error code otherwise.
 */
static int aim_amc_map_memory(struct aim_pci_device* aimDevice)
{
    struct pci_dev* pciDevice = NULL;
    resource_size_t barSize = 0;
    resource_size_t barSizeDetected = 0;
    int ret = 0;

    BUG_ON(!aimDevice || !aimDevice->pciDevice);

    pciDevice = aimDevice->pciDevice;

    ret = pci_request_regions(pciDevice, AIM_DRV_NAME);
    if (ret) {
	    aim_dev_error(aimDevice, "request regions failed\n");
	    pci_disable_device(pciDevice);
	    return ret;
    }

    barSize = resource_size(&pciDevice->resource[AMC_GLOBAL_MEM_BAR]);

    aimDevice->globalMemoryPhysical = pciDevice->resource[AMC_GLOBAL_MEM_BAR].start;
    aimDevice->globalMemory = ioremap(aimDevice->globalMemoryPhysical, barSize);

    if(!aimDevice->globalMemory)
    {
        aim_dev_error(aimDevice, "Mapping of global memory failed\n");
        return -ENOMEM;
    }


    aim_amc_memory_detect_size( aimDevice->globalMemory, barSize, AIM_PCI_DEVICE_GLOBAL_RAM_MARK, AIM_PCI_DEVICE_GLOBAL_RAM_MARK_OFFSET / 4, &barSizeDetected );

    if( barSizeDetected == 0 )
        aim_dev_error(aimDevice, "Global size detection failed\n");
    else
        barSize = barSizeDetected;

    aimDevice->globalMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped global memory bar to %p with size %llu KB\n", aimDevice->globalMemory,
                 (unsigned long long) aimDevice->globalMemorySize / 1024);

    barSize = resource_size(&pciDevice->resource[AMC_IO_MEM_BAR]);

    aimDevice->ioMemoryPhysical = pciDevice->resource[AMC_IO_MEM_BAR].start;

    aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);
    if(!aimDevice->ioMemory)
    {
        aim_dev_error(aimDevice, "Mapping of IO memory failed\n");
        return -ENOMEM;
    }

    aimDevice->ioMemorySize = barSize;

    aim_dev_info(aimDevice, "Mapped IO memory bar to %p with size %llu KB\n", aimDevice->ioMemory,
                 (unsigned long long) aimDevice->ioMemorySize / 1024);

    if(aimDevice->hasAsp)
    {
        barSize = resource_size(&pciDevice->resource[AMC_SHARED_MEM_BAR]);

        aimDevice->sharedMemoryPhysical = pciDevice->resource[AMC_SHARED_MEM_BAR].start;
        aimDevice->sharedMemory = ioremap(aimDevice->sharedMemoryPhysical, barSize);

        if(!aimDevice->sharedMemory)
        {
            aim_dev_error(aimDevice, "Mapping of shared memory failed\n");
            return -ENOMEM;
        }

        aimDevice->sharedMemorySize = barSize;

        aim_dev_info(aimDevice, "Mapped shared memory bar to %p with size %llu KB\n", aimDevice->sharedMemory,
                      (unsigned long long) aimDevice->sharedMemorySize / 1024);
    }
    else
    {
        /* Allocate a virtual shared memory for devices without ASP */
        aimDevice->sharedMemory = vzalloc(AMC_VIRTUAL_SHARED_MEMORY_SIZE);
        if(!aimDevice->sharedMemory)
        {
            aim_dev_error(aimDevice, "Allocation of shared memory failed\n");
            return -ENOMEM;
        }

        aimDevice->sharedMemorySize = AMC_VIRTUAL_SHARED_MEMORY_SIZE;

        aim_dev_info(aimDevice, "Allocated shared memory to %p with size %llu KB\n", aimDevice->sharedMemory,
                     (unsigned long long) aimDevice->sharedMemorySize / 1024);

    }

    return 0;
}


/*! \brief Unmaps the PCI bar registers of an AMC device
 *
 * This function will unmap the shared,global and the IO memory
 * bar of AMC devices.
 * @param aimDevice the device to map the bars of.
 * @return returns 0 on success, a negative error code otherwise.
 */
static void aim_amc_unmap_memory(struct aim_pci_device* aimDevice)
{
    BUG_ON(!aimDevice);

    aim_dev_debug(aimDevice, "Un-mapping memory bars\n");

    if( aimDevice->globalMemory )
    {
        aim_dev_debug(aimDevice, "Unmapping Global Memory\n");
        iounmap(aimDevice->globalMemory);
        aimDevice->globalMemory = NULL;
    }

    if( aimDevice->ioMemory )
    {
        aim_dev_debug(aimDevice, "Unmapping IO memory\n");
        iounmap(aimDevice->ioMemory);
        aimDevice->ioMemory = NULL;
    }

    if( aimDevice->sharedMemory )
    {
        aim_dev_debug(aimDevice, "Freeing shared memory\n");

        if(aimDevice->hasAsp)
        {
            iounmap(aimDevice->sharedMemory);
        }
        else
        {
            vfree(aimDevice->sharedMemory);
        }
        aimDevice->sharedMemory = NULL;
    }

    pci_release_regions(aimDevice->pciDevice);
}


/*! \brief Prepare some monitor data settings
 *
 * Initializes some settings in the device's monitor data.
 * This is necessary for hardware booting.
 * @param device the device to initialize monitor data of
 * @return 0 on success, errno code otherwise
 */
static int aim_amc_prepare_monitor(struct aim_pci_device* device)
{
    TY_MONITOR_DATA_STRUCT* monitorData = NULL;

    BUG_ON(!device);

    device->monitorData = kzalloc( sizeof(TY_MONITOR_DATA_STRUCT), GFP_KERNEL );

    if(!device->monitorData)
    {
        aim_dev_error(device, "Failed to allocate PMC monitor Monitor Data memory");
        return -ENOMEM;
    }

    monitorData = device->monitorData;

    monitorData->h_Drv = device;
    monitorData->PCI_FPGA_Reg_BAdr = device->ioMemory;
    monitorData->ASP_RAM_BAdr = device->sharedMemory;
    monitorData->GLOBAL_RAM_BAdr = device->globalMemory;

    monitorData->RAM_Size_Reg.Reg.GRam_Size = device->globalMemorySize / (1024*1024);
    monitorData->RAM_Size_Reg.Reg.Local_ASPRam = device->sharedMemorySize / (1024*1024);

    return 0;
}


/*! \brief Frees monitor data
 *
 * @param device device to free monitor data of
 */
static void aim_amc_free_monitor(struct aim_pci_device* device)
{
    BUG_ON(!device || !device->monitorData);

    kfree(device->monitorData);
    device->monitorData = NULL;
}


/*! \brief Loads firmware of PMC boards
 *
 * This function uses the PMC monitor in order to load
 * the IO-LCA, Firmware and target software on PMC boards.
 * @param aimDevice the device to load the firmware for
 * @return returns zero on success, a negative errno value otherwise
 */
static int aim_amc_load_firmware(struct aim_pci_device* aimDevice)
{
    TY_MONITOR_DATA_STRUCT* monitorData = NULL;
    unsigned int monitorRet = FAIL;
    int ret = 0;

    BUG_ON(!aimDevice || !aimDevice->monitorData);

    monitorData = aimDevice->monitorData;

    do
    {
        /* Allocate temporary buffer for firmware blobs */
        monitorData->GLOBAL_TEMP_MEM_SIZE = MON_SREC_CONV_MEM_SIZE;
        monitorData->GLOBAL_TEMP_MEM = vmalloc( MON_SREC_CONV_MEM_SIZE );

        if(!monitorData->GLOBAL_TEMP_MEM)
        {
            aim_dev_error(aimDevice, "Failed to allocate PMC monitor memory");
            ret = -ENOMEM;
            break;
        }

        aim_dev_info(aimDevice, "Loading firmware...");

        monitorRet = start_hardware(monitorData);
        if(monitorRet != PASS)
        {
            aim_dev_error(aimDevice, "Failed to load firmware");
            ret = -EIO;
            break;
        }

        aim_dev_info(aimDevice, "Firmware successfully loaded");

    }while(0);

    if(monitorData->GLOBAL_TEMP_MEM)
    {
        vfree(monitorData->GLOBAL_TEMP_MEM);
    }
        
    return ret;
}


/*! \brief Top-Half IRQ handler for AMC boards
 *
 * This handler is called when an interrupt on an IRQ line, that is used
 * by an AMC device, is detected. \n
 * It will check if the corresponding device actually issued the interrupt \n
 * and will call the IRQ tasklet for performing the bottom-half tasks.
 * @param irq the IRQ line that raised the interrupt
 * @param arg pointer to \ref struct aim_pci_device of the corresponding device
 * @return returns IRQ_HANDLED if the device actually issued an interrupt, IRQ_NONE otherwise
 */
static irqreturn_t aim_amc_irq_handler(int irq, void* arg)
{
    __u32 irqEvent = 0;
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) arg;

    aim_dev_debug(aimDevice, "IRQ on line %d\n", irq);

    irqEvent = aim_pci_io_read32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32));

    aim_dev_debug(aimDevice, "Irq Event: 0x%08x\n", irqEvent);

    if(irqEvent & PMC_IRQ_EVENTS)
    {
        if(irqEvent & PMC_IRQ_EVENT_ASP)
        {
            atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]);
            aim_dev_debug(aimDevice, "Received ASP interrupt. %d still pending\n",
                          atomic_read(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]));
        }

        if(irqEvent & PMC_IRQ_EVENT_BIU1)
        {
            atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU1_IRQ]);
            aim_dev_debug(aimDevice, "Received BIU 1 interrupt. %d still pending\n",
                          atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU1_IRQ]));
        }

        if(irqEvent & PMC_IRQ_EVENT_BIU2)
        {
            atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU2_IRQ]);
            aim_dev_debug(aimDevice, "Received BIU 2 interrupt. %d still pending\n",
                          atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU2_IRQ]));
        }

        if(irqEvent & PMC_IRQ_EVENT_BIU3)
        {
            atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU3_IRQ]);
            aim_dev_debug(aimDevice, "Received BIU 3 interrupt. %d still pending\n",
                          atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU3_IRQ]));
        }

        if(irqEvent & PMC_IRQ_EVENT_BIU4)
        {
            atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU4_IRQ]);
            aim_dev_debug(aimDevice, "Received BIU 4 interrupt. %d still pending\n",
                          atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU4_IRQ]));
        }

        /* Acknowledge all interrupts we handled, but leave the PCI interrupt enable bit set */
        aim_pci_io_write32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32),
                            (irqEvent & PMC_IRQ_EVENTS) | PMC_PCI_IRQ_ENABLED);

        tasklet_schedule(&aimDevice->irqTasklet);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}


/*! \brief Bottom-Half handler for AMC boards
 *
 * This handler function is called in the IRQ tasklet context of AMC boards. \n
 * It will call the protocol specific ASP interrupt handler function for each pending \n
 * ASP interrupt. \n
 * PMC modules without ASP are not supported at this time
 * @param data pointer to \ref struct aim_pci_device of the corresponding device.
 */
static void aim_amc_irq_task(unsigned long data)
{
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) data;
    int biu = 0;

    BUG_ON(!aimDevice);

    if(aimDevice->deviceData->platform == AI_DEVICE_AMC)
    {
        for(biu = 0; biu < AMC_MAX_BIUS; biu++)
        {
            aim_pci_device_handle_biu_irqs(aimDevice, biu);
        }
    }
    else if(aimDevice->deviceData->platform == AI_DEVICE_AMC_ASP)
    {
        aim_pci_device_handle_asp_irqs(aimDevice);
    }
}


/*! \brief Enables interrupts for an AMC device
 *
 * This function will initialize the device's IRQ tasklet and set the top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_amc_enable_interrupts(struct aim_pci_device* aimDevice)
{
    int ret = 0;
    __u32 irqMask = 0;
    __u32 irqEvent = 0;

    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    tasklet_init(&aimDevice->irqTasklet, aim_amc_irq_task, (unsigned long) aimDevice);

    ret = devm_request_irq(aim_to_dev(aimDevice), aimDevice->pciDevice->irq, aim_amc_irq_handler, IRQF_SHARED,
                           dev_name(&aimDevice->pciDevice->dev), aimDevice);
    if(ret)
    {
        aim_dev_error(aimDevice, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(aimDevice, "Using IRQ %d\n", aimDevice->pciDevice->irq);

    /* Disable all interrupts, but the ones we're interested in */
    irqMask = 0xFFFFFFFFU;
    irqMask &= ~PMC_IRQ_EVENTS;

    aim_dev_debug(aimDevice, "New IRQ Mask: 0x%x", irqMask);

    aim_pci_io_write32(aimDevice, PMC_IRQ_MASK_Reg_Adr * sizeof(__u32), irqMask);

    /* Clear all pending interrupts, even the one we're not handling, as mask register
     * is not initialized correctly
     */
    irqEvent = ~PMC_PCI_IRQ_ENABLED;
    aim_pci_io_write32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32), irqEvent);

    /* Now enable the PCI interrupts in event register */
    irqEvent = PMC_PCI_IRQ_ENABLED;
    aim_pci_io_write32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32), irqEvent);

    return ret;
}


/*! \brief Disables interrupts of an AMC device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_amc_disable_interrupts(struct aim_pci_device* aimDevice)
{
    __u32 irqMask = 0;

    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    aim_dev_debug(aimDevice, "Disabling interrupts\n");

    irqMask = aim_pci_io_read32(aimDevice, PMC_IRQ_MASK_Reg_Adr * sizeof(__u32));
    irqMask |= PMC_IRQ_EVENTS;

    aim_dev_debug(aimDevice, "New Mask: 0x%x", irqMask);

    aim_pci_io_write32(aimDevice, PMC_IRQ_MASK_Reg_Adr * sizeof(__u32), irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32), PMC_IRQ_EVENTS);

    /* Now disable the PCI interrupts in event register */
    aim_pci_io_write32(aimDevice, PMC_IRQ_EVENT_Reg_Adr * sizeof(__u32), 0);

    devm_free_irq(aim_to_dev(aimDevice), aimDevice->pciDevice->irq, aimDevice);
}


int aim_amc_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_amc_map_memory(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_amc_prepare_monitor(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_amc_load_firmware(aimDevice);
        if(ret)
        {
            break;
        }

        ret = aim_amc_enable_interrupts(aimDevice);
        if(ret)
        {
            break;
        }

        /* Now retrieve the basic device properties from the PMC monitor data structure */
        aimDevice->properties.serial = aimDevice->monitorData->NVR_BOARD_SERIAL.Reg.SERIAL;
        aimDevice->properties.config = aimDevice->monitorData->NVR_BOARD_CONFIG.all;
        aimDevice->properties.type = aimDevice->monitorData->NVR_BOARD_TYPE.all;
        aimDevice->properties.sub_type = aimDevice->monitorData->NVR_BOARD_ST.all;
        aimDevice->properties.hw_variant = aimDevice->monitorData->NVR_HW_VAR.all;
        aimDevice->properties.part_number = aimDevice->monitorData->NVR_BOARD_PARTNO.all;

        aim_dev_debug(aimDevice, "Serial: %u", aimDevice->properties.serial);
        aim_dev_debug(aimDevice, "Board Config: 0x%x", aimDevice->properties.config);
        aim_dev_debug(aimDevice, "Board Type: 0x%x", aimDevice->properties.type);
        aim_dev_debug(aimDevice, "Board Subtype: 0x%x", aimDevice->properties.sub_type);
        aim_dev_debug(aimDevice, "Hardware Variant: 0x%x", aimDevice->properties.hw_variant);
        aim_dev_debug(aimDevice, "Part Number: %u", aimDevice->properties.part_number);

    }while(0);

    if(ret)
    {
        aim_amc_device_hw_remove(aimDevice);
    }

    return ret;
}


void aim_amc_device_hw_remove(struct aim_pci_device* aimDevice)
{
    aim_amc_disable_interrupts(aimDevice);

    aim_amc_free_monitor(aimDevice);

    aim_amc_unmap_memory(aimDevice);
}


/*
    This function detects the real size of a mapped memory.

    The memory is detected by writing a Magic word to a specified
    position in the memory. The memory is then check where the magic
    word can be found again. Some plattforms do already contain a magic
    word (AyI, AyX) but AMC does not (not yet booted). Because of this
    we write the magic in this function to its usual location.

    IN :
    PAIMPCI_MEMORY pMemory    => The memory to analyse
    ULONG ulMagic             => The magic code
    ULONG ulMagicOffsetWord   => The word offset to write the magic word

    OUT :
    PULONG pulDetectedSize    <=  The detected real memory size

*/
int aim_amc_memory_detect_size( void __iomem * mem, resource_size_t mem_size, AiUInt32 ulMagic, resource_size_t ulMagicOffsetWord, resource_size_t * pulDetectedSize )
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
