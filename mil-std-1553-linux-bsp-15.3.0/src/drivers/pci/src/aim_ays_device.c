/* SPDX-FileCopyrightText: 2014-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ays_device.c
 *
 *  This file contains hardware related function definitions
 *  specific for AYS (Zynq) platform
 *
 */


#include <linux/types.h>
#include "aim_ays_device.h"
#include "aim_debug.h"
#include "aim_pci_mem.h"
#include "aim_kernel_mem.h"
#include "esmart.h"

/*! \def AYE_MAX_BIU
 *
 * Maximum number of logical BIUs on AYE boards
 */
#define AYS_MAX_BIUS 4



#define AYS_COMMAND_CHANNEL_ID 0

/*! \def AYS_HOST_IRQ_EVENT_REG
 *
 * Byte offset of interrupt event register for host in IO memory
 */
#define AYS_HOST_IRQ_EVENT_REG  0x48


/*! \def AYS_HOST_IRQ_MASK_REG
 *
 * Byte offset of interrupt mask register for host in IO memory
 */
#define AYS_HOST_IRQ_MASK_REG   0x4C


/*! \def AYS_PCI_COM_ID_TO_EVENT
 *
 * This define maps the ID of an interrupt based PCI communication
 * channel to the corresponding interrupt event bit
 * @param id ID of the channel to get event bit of
 */
#define AYS_PCI_COM_ID_TO_EVENT(id)     (1 << id)


/*! \def AYS_PCI_COM_0_EVENT
 * Interrupt event of PCI communication channel 0
 */
#define AYS_PCI_COM_0_EVENT     AYS_PCI_COM_ID_TO_EVENT(0)


/*! \def AYS_PCI_COM_1_EVENT
 * Interrupt event of PCI communication channel 1
 */
#define AYS_PCI_COM_1_EVENT     AYS_PCI_COM_ID_TO_EVENT(1)


/*! \def AYS_PCI_COM_2_EVENT
 * Interrupt event of PCI communication channel 2
 */
#define AYS_PCI_COM_2_EVENT     AYS_PCI_COM_ID_TO_EVENT(2)


/*! \def AYS_PCI_COM_3_EVENT
 * Interrupt event of PCI communication channel 3
 */
#define AYS_PCI_COM_3_EVENT     AYS_PCI_COM_ID_TO_EVENT(3)


/*! \def AYS_ASP_IRQ_LOGLIST_EVENT
 * Interrupt event bit that signals availability of new irq log list entry
 */
#define AYS_ASP_IRQ_LOGLIST_EVENT   (1 << 4)

/*! \def AYS_BIU1_IRQ_LOGLIST_EVENT
* Interrupt event bit that signals availability of new biu1 irq log list entry
*/
#define AYS_BIU1_IRQ_LOGLIST_EVENT   (1 << 8)

/*! \def AYS_BIU2_IRQ_LOGLIST_EVENT
* Interrupt event bit that signals availability of new biu2 irq log list entry
*/
#define AYS_BIU2_IRQ_LOGLIST_EVENT   (1 << 9)

/*! \def AYS_BIU3_IRQ_LOGLIST_EVENT
* Interrupt event bit that signals availability of new biu3 irq log list entry
*/
#define AYS_BIU3_IRQ_LOGLIST_EVENT   (1 << 10)

/*! \def AYS_BIU4_IRQ_LOGLIST_EVENT
* Interrupt event bit that signals availability of new biu4 irq log list entry
*/
#define AYS_BIU4_IRQ_LOGLIST_EVENT   (1 << 11)


/*! \def MAX_PCI_COM_ID
 * greatest ID of interrupt based PCI communication channels the driver cares for
 */
#define MAX_PCI_COM_ID 3


/*! \def AYS_ASP_IRQ_LOGLIST_EVENT
 * Interrupt event bit that signals availability of new irq log list entry
 */
#define AYS_ASP_IRQ_LOGLIST_EVENT   (1 << 4)


/*! \def AYS_HOST_IRQ_EVENTS
 *
 * All interrupt events the driver is responsible for
 */

/*! \def AYS_DMA_WIDTH
 * Number of bits the AYS DMA Engine is able to access
 */
#define AYS_DMA_WIDTH 64

#define AYS_HOST_IRQ_EVENTS (AYS_PCI_COM_0_EVENT | AYS_PCI_COM_1_EVENT | AYS_PCI_COM_2_EVENT | AYS_PCI_COM_3_EVENT | \
                             AYS_ASP_IRQ_LOGLIST_EVENT | \
                             AYS_BIU1_IRQ_LOGLIST_EVENT | AYS_BIU2_IRQ_LOGLIST_EVENT | AYS_BIU3_IRQ_LOGLIST_EVENT | AYS_BIU4_IRQ_LOGLIST_EVENT)

#define AYS_IRQ_ALL        AYS_HOST_IRQ_EVENTS

 



/*! \brief Bottom-Half handler for AYS boards
 *
 * This handler function is called in the IRQ tasklet context of AYS boards. \n
 * It will call the protocol specific ASP interrupt handler function for each pending \n
 * ASP interrupt.
 * @param data pointer to \ref struct aim_pci_device of the corresponding device.
 */
void aim_ays_irq_task(unsigned long data);




/*! \brief Maps the bar registers of an AYS device
 *
 * This function will map the shared RAM bar and the IO memory
 * bar of AYS devices.
 * @param aimDevice the device to map the PCI Bar registers of
 * @return returns 0 on success, a negative error otherwise
 */
static int aim_ays_map_bars(struct aim_pci_device* aimDevice)
{
	int ret;
    struct pci_dev* pciDevice = NULL;
    resource_size_t barSize = 0;

    BUG_ON(!aimDevice || !aimDevice->pciDevice);

    pciDevice = aimDevice->pciDevice;

    ret = pci_request_regions(pciDevice, AIM_DRV_NAME);
    if (ret) {
	    aim_dev_error(aimDevice, "request regions failed\n");
	    pci_disable_device(pciDevice);
	    return ret;
    }

	if( pci_resource_start(pciDevice, 4) == 0 )
    {
    	// 2 Memory bars => FDX or FC board
		barSize = pci_resource_len(pciDevice, AYS_IO_MEM_BAR);
		aimDevice->ioMemoryPhysical = pci_resource_start(pciDevice, AYS_IO_MEM_BAR);
		aimDevice->ioMemory = ioremap(aimDevice->ioMemoryPhysical, barSize);

		if(!aimDevice->ioMemory)
		{
			aim_dev_error(aimDevice, "Allocation of IO memory failed\n");
			return -ENOMEM;
		}

		aimDevice->ioMemorySize = barSize;

		aim_dev_info(aimDevice, "Mapped IO memory bar to 0x%p with size %llu KB\n", aimDevice->ioMemory,
					  (unsigned long long) aimDevice->ioMemorySize / 1024);


		aim_pci_mem_io_dump(aimDevice, 120);
    }
    else
    {
		// 3 Memory bars => AME1553
		barSize                         = pci_resource_len(pciDevice, 4);
		aimDevice->ioMemoryPhysical     = pci_resource_start(pciDevice, 4);
		aimDevice->ioMemory             = ioremap(aimDevice->ioMemoryPhysical, barSize);
		if(aimDevice->ioMemory)
		{
			aimDevice->ioMemorySize         = barSize;
			aim_dev_info(aimDevice, "Mapped IO memory bar to 0x%p with size %llu KB\n", aimDevice->ioMemory, (unsigned long long) aimDevice->ioMemorySize / 1024);
		}
		else
		{
			aim_dev_error(aimDevice, "Allocation of IO memory failed\n");
			return -ENOMEM;
		}


		barSize                         = pci_resource_len(pciDevice, 2);
		aimDevice->globalMemoryPhysical = pci_resource_start(pciDevice, 2);
		aimDevice->globalMemory         = ioremap(aimDevice->globalMemoryPhysical, barSize);
		if(aimDevice->globalMemory)
		{
			aimDevice->globalMemorySize = barSize;
			aim_dev_info(aimDevice, "Mapped GLOBAL memory bar to 0x%p with size %llu KB\n", aimDevice->globalMemory, (unsigned long long) aimDevice->globalMemorySize / 1024);
		}
		else
		{
			aim_dev_error(aimDevice, "Allocation of GLOBAL memory failed\n");
			return -ENOMEM;
		}


	    if(!aimDevice->hasAsp)
	    {
	        /* Allocate a virtual shared memory for devices without ASP */
	        aimDevice->sharedMemory = vzalloc(AYS_SHARED_MEMORY_SIZE);

	        if(aimDevice->sharedMemory)
			{
		        aimDevice->sharedMemorySize = AYS_SHARED_MEMORY_SIZE;
		        aim_dev_info(aimDevice, "Allocated shared memory to %p with size %llu KB\n", aimDevice->sharedMemory,
		                     (unsigned long long) aimDevice->sharedMemorySize / 1024);
			}
	        else
	        {
	            aim_dev_error(aimDevice, "Allocation of shared memory failed\n");
	            return -ENOMEM;
	        }
	    }
    }

    return 0;
;
}


/*! \brief Unmaps the BAR registers of AYS devices
 *
 * @param aimDevice device to unmap the bars of
 */
static void aim_ays_unmap_bars(struct aim_pci_device* aimDevice)
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


/*! \brief Top-Half IRQ handler for AYS boards
 *
 * This handler is called when an interrupt on an IRQ line, that is used
 * by an AYS device, is detected. \n
 * It will check if the corresponding device actually issued the interrupt. \n
 * It checks the event register for interrupt events that were issued due
 * to a complete command/response sequence on any interrupt based PCI communication channel. \n
 * The corresponding PCI communication sequence will then be directly completed, so issuer
 * will continue to run. \n
 * No tasklet is used so far.
 * @param irq the IRQ line that raised the interrupt
 * @param arg pointer to \ref struct aim_pci_device of the corresponding device
 * @return returns IRQ_HANDLED if the device actually issued an interrupt, IRQ_NONE otherwise
 */
static irqreturn_t aim_ays_irq_handler(int irq, void* arg)
{
    __u32 irqEvent = 0;
    int i = 0;
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) arg;
    struct aim_pci_com_channel* channel = NULL;
    loff_t irq_reg_evt_offset = 0;
    int run_tasklet = 0;

    /* Check the IRQ event register and return in case
     * no event, we're responsible for, was raised
     */

    if(aimDevice->deviceData->platform==AI_DEVICE_AYS)
        irq_reg_evt_offset = AYS_HOST_IRQ_EVENT_REG+AYS_HOST_IRQ_REG_OFFSET;
    else
        irq_reg_evt_offset = AYS_HOST_IRQ_EVENT_REG;
        
        
    irqEvent = aim_pci_io_read32(aimDevice, irq_reg_evt_offset);

    if(!(irqEvent & AYS_HOST_IRQ_EVENTS))
    {
        return IRQ_NONE;
    }

    aim_dev_debug(aimDevice, "Irq Event: 0x%08x\n", irqEvent);

    /* Acknowledge all events we will provess further on */
    aim_pci_io_write32(aimDevice, irq_reg_evt_offset, irqEvent & AYS_HOST_IRQ_EVENTS);

    /* Check all PCI communication channels we support for a complete command/response sequence */
    for(i = 0; i <= MAX_PCI_COM_ID; i++)
    {
        if(irqEvent & (AYS_PCI_COM_ID_TO_EVENT(i)))
        {
            /* Get the corresponding PCI communication channel of the device and complete it */
            channel = radix_tree_lookup(&aimDevice->pciComChannels, i);
            if(!channel)
            {
                aim_dev_error(aimDevice, "Got interrupt for PCI Com channel %d, which is not available\n", i);
                continue;
            }

            complete(&channel->commandCompletion);
        }
    }
 
    if(irqEvent & AYS_BIU1_IRQ_LOGLIST_EVENT)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU1_IRQ]);
        aim_dev_debug(aimDevice, "Received BIU1 interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU1_IRQ]));
         run_tasklet = 1;
    }

    if(irqEvent & AYS_BIU2_IRQ_LOGLIST_EVENT)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU2_IRQ]);
        aim_dev_debug(aimDevice, "Received BIU2 interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU2_IRQ]));
         run_tasklet = 1;

    }

    if(irqEvent & AYS_BIU3_IRQ_LOGLIST_EVENT)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU3_IRQ]);
        aim_dev_debug(aimDevice, "Received BIU3 interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU3_IRQ]));
         run_tasklet = 1;

    }

    if(irqEvent & AYS_BIU4_IRQ_LOGLIST_EVENT)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_BIU4_IRQ]);
        aim_dev_debug(aimDevice, "Received BIU4 interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_BIU4_IRQ]));
         run_tasklet = 1;

    }
	
     if(irqEvent & AYS_ASP_IRQ_LOGLIST_EVENT)
    {
        atomic_inc(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]);
        aim_dev_debug(aimDevice, "Received ASP interrupt. %d still pending\n", atomic_read(&aimDevice->pendingIrqs[AIM_PCI_ASP_IRQ]));
         run_tasklet = 1;

    }

    if(run_tasklet)
        tasklet_schedule(&aimDevice->irqTasklet);


    return IRQ_HANDLED;
}


int aim_ays_enable_interrupts(struct aim_pci_device* aimDevice, unsigned long offset)
{
    int ret = 0;
    __u32 irqMask = 0;

    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    /* Clear all pending interrupts before enabling them */
    aim_dev_debug(aimDevice, "Clearing all events....(0x%08x)\n",AYS_HOST_IRQ_EVENTS);
    aim_pci_io_write32(aimDevice, AYS_HOST_IRQ_EVENT_REG+offset, AYS_HOST_IRQ_EVENTS);

    tasklet_init(&aimDevice->irqTasklet, aim_ays_irq_task, (unsigned long) aimDevice);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    /* Try to enable MSI interrupts for kernel versions that support pci_alloc_irq_vectors and request_irq. */
    ret = pci_alloc_irq_vectors(aimDevice->pciDevice, 1, 1, PCI_IRQ_MSI);
    if (ret > 0)
    {
        aimDevice->hasMsiEnabled = TRUE;
        /* Override PCI irq number with MSI vector for request_irq and free_irq */
        aimDevice->pciDevice->irq = pci_irq_vector(aimDevice->pciDevice, 0);
        aim_dev_info(aimDevice, "Allocated MSI vector %d.\n", aimDevice->pciDevice->irq);
    }
    else
    {
        aimDevice->hasMsiEnabled = FALSE;
        aim_dev_warning(aimDevice, "Failed to allocate a MSI vector. Fallback to legacy interrupts.\n");
    }
#endif   

    ret = request_irq(aimDevice->pciDevice->irq, aim_ays_irq_handler, IRQF_SHARED, dev_name(&aimDevice->pciDevice->dev), aimDevice);
    if(ret)
    {
        aim_dev_error(aimDevice, "Failed to set up IRQ handler\n");
        return ret;
    }

    aim_dev_info(aimDevice, "Using IRQ %d\n", aimDevice->pciDevice->irq);

    irqMask = aim_pci_io_read32(aimDevice, AYS_HOST_IRQ_MASK_REG+offset);
    irqMask |= AYS_IRQ_ALL;

    aim_dev_debug(aimDevice, "IRQ Mask is 0x%x\n", irqMask);

    aim_pci_io_write32(aimDevice, AYS_HOST_IRQ_MASK_REG+offset, irqMask);

    return ret;
}


void aim_ays_disable_interrupts(struct aim_pci_device* aimDevice, unsigned long offset)
{
    u32 irqMask = 0;
    BUG_ON(!aimDevice || !aimDevice->ioMemory);

    irqMask = aim_pci_io_read32(aimDevice, AYS_HOST_IRQ_MASK_REG+offset);
    irqMask &= ~AYS_IRQ_ALL;

    aim_dev_debug(aimDevice, "IRQ Mask after disabling interrupts is 0x%x\n", irqMask);

    aim_pci_io_write32(aimDevice, AYS_HOST_IRQ_MASK_REG+offset, irqMask);

    /* Clear outstanding interrupts */
    aim_pci_io_write32(aimDevice, AYS_HOST_IRQ_EVENT_REG+offset, AYS_HOST_IRQ_EVENTS);

    free_irq(aimDevice->pciDevice->irq, aimDevice);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
    if(aimDevice->hasMsiEnabled)
        pci_free_irq_vectors(aimDevice->pciDevice);
#endif    
}


/*! \brief Initializes DMA transactions for an AYS device
 *
 * This function will just set the DMA mask for the device.
 * @param aimDevice the device to initialize DMA for
 * @return returns 0 on success, a negative error code otherwise
 */
static int __inline__ aim_ays_dma_init(struct aim_pci_device* aimDevice)
{
    aim_dev_debug(aimDevice, "Using %d-Bit DMA width for AYS platform\n", AYS_DMA_WIDTH);
    return dma_set_mask(&aimDevice->pciDevice->dev, DMA_BIT_MASK(AYS_DMA_WIDTH));
}




int aim_ays_device_hw_init(struct aim_pci_device* aimDevice)
{
    int ret = 0;

    do
    {
        ret = aim_ays_map_bars(aimDevice);
        if(ret)
        {
            break;
        }

        if(aimDevice->deviceData->platform==AI_DEVICE_AYS)
            ret = aim_ays_enable_interrupts(aimDevice, AYS_HOST_IRQ_REG_OFFSET);
        else
            ret = aim_ays_enable_interrupts(aimDevice, 0);
        
        if(ret)
        {
            break;
        }

        ret = aim_ays_dma_init(aimDevice);
        if(ret)
        {
            break;
        }
    }while(0);

    if(ret)
    {
        aim_ays_device_hw_remove(aimDevice);
    }

    return ret;
}


void aim_ays_device_hw_remove(struct aim_pci_device* aimDevice)
{
    if(aimDevice->deviceData->platform==AI_DEVICE_AYS)
        aim_ays_disable_interrupts(aimDevice, AYS_HOST_IRQ_REG_OFFSET);
    else
        aim_ays_disable_interrupts(aimDevice, 0);
        
    aim_ays_unmap_bars(aimDevice);
}


void aim_ays_irq_task(unsigned long data)
{
    struct aim_pci_device* aimDevice = (struct aim_pci_device*) data;
    int i = 0;

    BUG_ON(!aimDevice);
	if(aimDevice->deviceData->platform==AI_DEVICE_AYS)
	{
		for(i = 0; i < AYS_MAX_BIUS; i++)
		{
			aim_dev_debug(aimDevice, "Inside Tasklet: Calling handle_biu %d\n", i);
			aim_pci_device_handle_biu_irqs(aimDevice, i);
		}
	}
	else
	{
		aim_pci_device_handle_asp_irqs(aimDevice);	
	}
}


int aim_gen_ays_novram_get(void* Device, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesRead, void* pv_Novram)
{
	struct aim_pci_device* aimDevice = (struct aim_pci_device*)Device;
    struct esmart_novram_get_req esmart_req;
    struct esmart_novram_get_resp* esmart_response = NULL;
    struct aim_pci_com_channel* esmart_com_channel;
    int err;
	size_t dataOffset = 0;

    BUG_ON(!aimDevice || !pv_Novram);

    err = 0;
    do
    {
		/* Allocate Response */
        esmart_response = kmalloc(esmart_novram_get_resp_size(max_size / 4), GFP_KERNEL);
        if(!esmart_response)
        {
            err = -ENOMEM;
            break;
        }
        
		/* Find Channel (0) */
        esmart_com_channel = radix_tree_lookup(&aimDevice->pciComChannels, AYS_COMMAND_CHANNEL_ID); /*aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);*/
        if(!esmart_com_channel)
        {
            err = PTR_ERR(esmart_com_channel);
            break;
        }

		/* Setup Request Packet */
        esmart_req.header.magic = ESMART_MAGIC;
        esmart_req.header.request_id = ESMART_REQ_NOVRAM_GET;
        esmart_req.header.request_size = sizeof(struct esmart_novram_get_req);

        esmart_req.start_offset = address;
        esmart_req.num_entries = (max_size / 4);

		/* Send the command */
        err = aim_pci_com_channel_issue_cmd(esmart_com_channel, &esmart_req, sizeof(esmart_req), esmart_response, esmart_novram_get_resp_size(max_size  / 4));
        if(err < 0)
        {
            break;
        }

		/* Evaluate Status */
		if(esmart_response->header.magic != ESMART_MAGIC || esmart_response->status != ESMART_OK)
		{
            err = -EIO;
            break;			
		}
		else
		{
			    dataOffset = offsetof(struct esmart_novram_get_resp, novram_data);
				memcpy(pv_Novram, (AiUInt8*)esmart_response + dataOffset, (esmart_response->num_entries * 4));
				break;
		}
    }while(0);

    if(esmart_response)
    {
        kfree(esmart_response);
    }

    return err;
}





int aim_gen_ays_novram_set(void* Device, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesWritten, void* pv_Novram)
{
	struct aim_pci_device* aimDevice = (struct aim_pci_device*)Device;
    struct esmart_novram_set_req * esmart_req;
    struct esmart_novram_set_resp  esmart_response;
    struct aim_pci_com_channel* esmart_com_channel;
    int err;
	size_t dataOffset = 0;

    BUG_ON(!aimDevice || !pv_Novram);

    err = 0;
    do
    {
		/* Allocate Response */
    	esmart_req = kmalloc(esmart_novram_set_req_size(max_size / 4), GFP_KERNEL);
        if(!esmart_req)
        {
            err = -ENOMEM;
            break;
        }

		/* Find Channel (0) */
        esmart_com_channel = radix_tree_lookup(&aimDevice->pciComChannels, AYS_COMMAND_CHANNEL_ID); /*aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);*/
        if(!esmart_com_channel)
        {
            err = PTR_ERR(esmart_com_channel);
            break;
        }

		/* Setup Request Packet */
        esmart_req->header.magic = ESMART_MAGIC;
        esmart_req->header.request_id = ESMART_REQ_NOVRAM_SET;
        esmart_req->header.request_size = esmart_novram_set_req_size(max_size / 4);

        esmart_req->start_offset = address;
        esmart_req->num_entries = (max_size / 4);

	    dataOffset = offsetof(struct esmart_novram_set_req, novram_data);
		memcpy((AiUInt8*)esmart_req + dataOffset, pv_Novram, (esmart_req->num_entries * 4));

		/* Send the command */
        err = aim_pci_com_channel_issue_cmd(esmart_com_channel, esmart_req, esmart_novram_set_req_size(max_size / 4), &esmart_response, sizeof(esmart_response));
        if(err < 0)
        {
            break;
        }

		/* Evaluate Status */
		if(esmart_response.header.magic != ESMART_MAGIC || esmart_response.status != ESMART_OK)
		{
            err = -EIO;
            break;
		}
    }while(0);

    if(esmart_req)
    {
        kfree(esmart_req);
    }

    return err;
}






int aim_gen_ays_novram_finalize(void* Device)
{
	struct aim_pci_device* aimDevice = (struct aim_pci_device*)Device;
    struct esmart_novram_prog_req   esmart_req;
    struct esmart_novram_prog_resp  esmart_response;
    struct aim_pci_com_channel* esmart_com_channel;
    int err;

    BUG_ON(!aimDevice);

    err = 0;
    do
    {
		/* Find Channel (0) */
        esmart_com_channel = radix_tree_lookup(&aimDevice->pciComChannels, AYS_COMMAND_CHANNEL_ID); /*aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);*/
        if(!esmart_com_channel)
        {
            err = PTR_ERR(esmart_com_channel);
            break;
        }

		/* Setup Request Packet */
        esmart_req.header.magic        = ESMART_MAGIC;
        esmart_req.header.request_id   = ESMART_REQ_NOVRAM_PROG;
        esmart_req.header.request_size = sizeof(esmart_req);


		/* Send the command */
        err = aim_pci_com_channel_issue_cmd(esmart_com_channel, &esmart_req, sizeof(esmart_req), &esmart_response, sizeof(esmart_response));
        if(err < 0)
        {
            break;
        }

		/* Evaluate Status */
		if(esmart_response.header.magic != ESMART_MAGIC || esmart_response.status != ESMART_OK)
		{
            err = -EIO;
            break;
		}
    }while(0);


    return err;
}


int aim_gen_ays_version_read(void* Device, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersionValue)
{
	struct aim_pci_device* aimDevice = (struct aim_pci_device*)Device;
    struct esmart_version_get_req esmart_req;
    struct esmart_version_get_resp* esmart_response = NULL;
    struct aim_pci_com_channel* esmart_com_channel;
    int err;

    BUG_ON(!aimDevice);
    BUG_ON(!pxVersionValue);
    err = 0;
    do
    {
		/* Allocate Response */
        esmart_response = kmalloc(sizeof(struct esmart_version_get_resp), GFP_KERNEL);
        if(!esmart_response)
        {
            err = -ENOMEM;
            break;
        }
        
		/* Find Channel (0) */
        esmart_com_channel = radix_tree_lookup(&aimDevice->pciComChannels, AYS_COMMAND_CHANNEL_ID); /*aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);*/
        if(!esmart_com_channel)
        {
            err = PTR_ERR(esmart_com_channel);
            break;
        }

		/* Setup Request Packet */
	    esmart_req.header.magic = ESMART_MAGIC;
        esmart_req.header.request_id = ESMART_REQ_VERSION_GET;
        esmart_req.header.request_size = sizeof(struct esmart_version_get_req);
		esmart_req.versionId = eId;

		/* Send the command */
        err = aim_pci_com_channel_issue_cmd(esmart_com_channel, &esmart_req, sizeof(esmart_req), esmart_response, sizeof(struct esmart_version_get_resp));
        if(err < 0)
        {
            break;
        }

		/* Evaluate Status */
		if(esmart_response->header.magic != ESMART_MAGIC || esmart_response->status != ESMART_OK || esmart_response->header.request_id != ESMART_REQ_VERSION_GET)
		{
            err = -EIO;
            break;			
		}
		else
		{
				pxVersionValue->ul_VersionType = esmart_response->version.ul_VersionType;
				pxVersionValue->ul_MajorVer = esmart_response->version.ul_MajorVer;
				pxVersionValue->ul_MinorVer = esmart_response->version.ul_MinorVer;
				pxVersionValue->ul_PatchVersion = esmart_response->version.ul_PatchVersion;
				pxVersionValue->ul_BuildNr = esmart_response->version.ul_BuildNr;

				strncpy(pxVersionValue->ac_Description, esmart_response->version.ac_Description, AI_DESCRIPTION_STRINGL);
				strncpy(pxVersionValue->ac_FullVersion, esmart_response->version.ac_FullVersion, AI_DESCRIPTION_STRINGL);
				break;
		}
    }while(0);

    if(esmart_response)
    {
        kfree(esmart_response);
    }

    return err;
}

