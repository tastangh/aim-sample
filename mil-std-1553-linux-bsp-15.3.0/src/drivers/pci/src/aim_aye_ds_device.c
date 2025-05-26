/* SPDX-FileCopyrightText: 2020-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_aye_ds_device.c
 *
 *  This header file contains hardware related implementations
 *  specific for APE1553-1/2-DS (Digital Scope / MIL-Scope)
 *
 */

#include "aim_pci_device.h"
#include "aim_pci_mem.h"
#include "hw/AiHwAye.h"
#include "aim_aye_ds_device.h"


/*****************************************************************************/
/*                                                                           */
/*                         Defines and Data Types                            */
/*                                                                           */
/*****************************************************************************/



#define MAX_SCOPE_BUFFER_SIZE (64 * 1024)
#define MAX_ENTRIES_PER_SCOPE_BUFFER ((MAX_SCOPE_BUFFER_SIZE / PAGE_SIZE) + 1)




/*****************************************************************************/
/*                                                                           */
/*                           Internal  Functions                             */
/*                                                                           */
/*****************************************************************************/




/*! \brief This function checks if free entries are available in the DMA fifo
 *
 * @param aimDevice the device the ioctl was issued on
 */
static int aim_aye_ds_device_fifo_full(struct aim_pci_device *aimDevice)
{
    unsigned long ulFreeDmaEntries = 0;

    ulFreeDmaEntries = aim_pci_io_read32( aimDevice, AYE_REG_DMA_MS_HEADER_COUNTER * 4) / 4;

    aim_dev_debug(aimDevice, "aim_aye_ds_device_fifo_full: fifo count %ld\n", ulFreeDmaEntries);

    if (ulFreeDmaEntries < MAX_ENTRIES_PER_SCOPE_BUFFER)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}




/*! \brief This function writes the DMA scatter gather elements into the DMA fifo
 *
 * @param aimDevice the device the ioctl was issued on
 * @param pxMilscopeBuffer the digital scope settings provided by the API call
 * @param dmaBuffer the DMA memory scatter gather list
 */
static int aim_aye_ds_device_dma_start(struct aim_pci_device *aimDevice, TY_MILSCOPE_BUFFER *pxMilscopeBuffer, struct aim_dma_buffer* dmaBuffer)
{
    int i = 0;
    dma_addr_t DMAAddress;
    struct scatterlist* scatterlistEntry = NULL;
    TY_AYE_DMA_DMA_SGL_SETUP xDmaSetup;

    memset(&xDmaSetup, 0, sizeof(xDmaSetup));

    if (pxMilscopeBuffer == NULL)
        return -EINVAL;

    if (dmaBuffer == NULL)
        return -EINVAL;

    /* -- Write SGL to the hardware --- */

    xDmaSetup.xSglEntry.ulControl.part.MilScopeBlock = pxMilscopeBuffer->eType;
    xDmaSetup.xSglEntry.ulControl.part.DmaDest = AyEDmaDestHost;
    xDmaSetup.xSglEntry.ulControl.part.DmaSource = AyEDmaSourceMilScope;


    for_each_sg(dmaBuffer->scatterTable.sgl, scatterlistEntry, dmaBuffer->scatterTable.nents, i)
    {

        xDmaSetup.xSglEntry.ulControl.part.Size = sg_dma_len(scatterlistEntry);

        if (i == (dmaBuffer->scatterTable.nents - 1))
        {
            /* B1 Generate interrupt for the last SGL item */
            xDmaSetup.xSglEntry.ulControl.part.Interrupt = AyEDmaEnableInt;
        }

        DMAAddress = sg_dma_address(scatterlistEntry);
        xDmaSetup.xSglEntry.ulPciAddressHigh = (DMAAddress >> 32) & 0xFFFFFFFFUL;
        xDmaSetup.xSglEntry.ulPciAddressLow  = (DMAAddress)       & 0xFFFFFFFFUL;

        aim_dev_debug(aimDevice, "Parameters for MilScope scatter/gather entry %d:\n", i);
        aim_dev_debug(aimDevice, "\tControl:          0x%x\n", xDmaSetup.xSglEntry.ulControl.all);
        aim_dev_debug(aimDevice, "\tPCI High Address: 0x%x\n", xDmaSetup.xSglEntry.ulPciAddressHigh);
        aim_dev_debug(aimDevice, "\tPCI Low Address:  0x%x\n", xDmaSetup.xSglEntry.ulPciAddressLow);
        aim_dev_debug(aimDevice, "\tPCI Length:       0x%x\n", xDmaSetup.xSglEntry.ulControl.part.Size);
        aim_dev_debug(aimDevice, "\tDeviceAddress:    0x%x\n", xDmaSetup.xSglEntry.ulDeviceAddress);

        aim_pci_io_write32(aimDevice, AYE_REG_DMA_MS_SGL_CONTROL * 4, xDmaSetup.xSglEntry.ulControl.all );
        wmb();
        aim_pci_io_write32(aimDevice, AYE_REG_DMA_MS_SGL_PCI_LOW * 4, xDmaSetup.xSglEntry.ulPciAddressLow );
        wmb();
        aim_pci_io_write32(aimDevice, AYE_REG_DMA_MS_SGL_PCI_HIGH * 4, xDmaSetup.xSglEntry.ulPciAddressHigh );
        wmb();
        aim_pci_io_write32(aimDevice, AYE_REG_DMA_MS_SGL_DEVICE_LOW * 4, xDmaSetup.xSglEntry.ulDeviceAddress );
        wmb();
    }


    return 0;
}




/*! \brief This function resets the DMA engine
 *
 * @param aimDevice the device the ioctl was issued on
 */
static int aim_aye_ds_device_dma_reset(struct aim_pci_device *aimDevice)
{
    TY_AYE_MS_DMA_CONTROL xScopeDmaControl;

    xScopeDmaControl.all = aim_pci_io_read32( aimDevice, AYE_REG_DMA_MS_CONTROL * 4);

    rmb();

    xScopeDmaControl.settings.reset = 1;
    aim_pci_io_write32(aimDevice, AYE_REG_DMA_MS_CONTROL * 4, xScopeDmaControl.all );

    wmb();

    do
    {
        xScopeDmaControl.all = aim_pci_io_read32( aimDevice, AYE_REG_DMA_MS_CONTROL * 4);
        rmb();
    } while (xScopeDmaControl.settings.reset);

    return 0;
}




/*! \brief This function writes all provided buffers into the dma fifo
 *
 * @param aimDevice the device the ioctl was issued on
 */
static int aim_aye_ds_device_bufferqueue_drain(struct aim_pci_device *aimDevice)
{
    int retval = 0;
    struct aim_ays_ds_device_dma_buffer *my_obj;
    int queue_empty;
    int fifo_full;

    spin_lock_bh(&aimDevice->ScopeBufferQueueLock);

    queue_empty = list_empty(&aimDevice->ScopeBufferQueue);
    fifo_full = aim_aye_ds_device_fifo_full(aimDevice);

    while (!queue_empty && !fifo_full)
    {

        /*
     * All request that are written to the fifo are moved to the pending queue
     * */

        my_obj = list_first_entry(&aimDevice->ScopeBufferQueue, struct aim_ays_ds_device_dma_buffer, list);
        list_move_tail(&my_obj->list, &aimDevice->ScopeBufferPendingQueue);

        retval = aim_aye_ds_device_dma_start(aimDevice, &my_obj->xScopeBuffer, &my_obj->dmaBuffer);

        if (retval != 0)
        {
            aim_dev_error(aimDevice, "aim_aye_ds_device_bufferqueue_drain failed to start scope DMA (%d)\n", retval);
            break;
        }

        /* update status */
        queue_empty = list_empty(&aimDevice->ScopeBufferQueue);
        fifo_full = aim_aye_ds_device_fifo_full(aimDevice);
    }

    spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);

    return retval;
}




/*! \brief This function maps a new DMA buffer and adds it to the bufferqueue
 *
 * @param aimDevice the device the ioctl was issued on
 * @param pxMilscopeBuffer the digital scope settings provided by the API call
 */
static int aim_aye_ds_device_bufferqueue_add(struct aim_pci_device *aimDevice, TY_MILSCOPE_BUFFER *pxMilscopeBuffer)
{
    int ret;
    struct aim_ays_ds_device_dma_buffer *pCurrent = NULL;

    /* clear prev. canceled flag */
    atomic_set(&aimDevice->ScopeBufferCanceled, 0);

    aim_dev_debug(aimDevice, "aim_aye_ds_device_bufferqueue_add type=0x%x, ulSize=0x%x, ullVAddress = 0x%016llX\n", pxMilscopeBuffer->eType, pxMilscopeBuffer->ulSize, pxMilscopeBuffer->ullVirtualAddress);

    /* Allocate list item to store MS DMA specific information */

    pCurrent = (struct aim_ays_ds_device_dma_buffer *)kmalloc(sizeof(struct aim_ays_ds_device_dma_buffer), GFP_KERNEL);

    if (pCurrent == NULL)
    {
        aim_dev_error(aimDevice, "aim_aye_ds_device_bufferqueue_add out of memory\n");
        return -ENOMEM;
    }

    do
    {
        /* -- Prepare SGL --- */
        ret = aim_dma_buffer_init( &pCurrent->dmaBuffer, (void*)pxMilscopeBuffer->ullVirtualAddress, pxMilscopeBuffer->ulSize);

        if (ret != 0)
        {
            aim_dev_error(aimDevice, "Allocation of page pointers failed\n");
            break;
        }


        ret = aim_dma_buffer_lock_pages( &pCurrent->dmaBuffer );

        if (ret != 0)
        {
            aim_dev_error(aimDevice, "Locking of user-space buffer failed\n");
            break;
        }

        aim_dev_debug(aimDevice, "Locked %d user pages with start 0x%p length 0x%lx\n", pCurrent->dmaBuffer.numPages, pCurrent->dmaBuffer.bufferBase, pCurrent->dmaBuffer.length);

        /* Create the scatterlist and assign the pages to the corresponding entries */
        ret = aim_dma_buffer_create_sg( &pCurrent->dmaBuffer );

        if (ret != 0)
        {
            aim_dev_error(aimDevice, "Creation of scatterlist failed\n");
            break;
        }

        ret = aim_dma_buffer_map( &pCurrent->dmaBuffer, &aimDevice->pciDevice->dev );

        if (ret != 0)
        {
            aim_dev_error(aimDevice, "Mapping of scatterlist failed\n");
            break;
        }

        /* -- Insert the new scope buffer into the available buffers list. --- */
        pCurrent->xScopeBuffer.eType = pxMilscopeBuffer->eType;
        pCurrent->xScopeBuffer.ulSize = pxMilscopeBuffer->ulSize;
        pCurrent->xScopeBuffer.ullVirtualAddress = pxMilscopeBuffer->ullVirtualAddress;

    } while (0); /* Try Catch Block*/
    

    if(ret == 0)
    {
        /* Buffers are prepared add current to list */
        spin_lock_bh(&aimDevice->ScopeBufferQueueLock);
        list_add_tail(&(pCurrent->list), &(aimDevice->ScopeBufferQueue));
        spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);

        /* -- Try to write the new buffers into the HW fifo --- */
        ret = aim_aye_ds_device_bufferqueue_drain(aimDevice);
    }
    else
    {
        /* Clean up in case of error */
        aim_dma_buffer_free(&pCurrent->dmaBuffer, &aimDevice->pciDevice->dev, false);
        kfree(pCurrent);
    }

    return ret;
}




/*! \brief This function stops the DMA engine and releases all queue buffers
 *
 * @param aimDevice the device the ioctl was issued on
 */
static int aim_aye_ds_device_bufferqueue_release(struct aim_pci_device *aimDevice)
{
    int retval = 0;
    struct list_head *pos, *q;
    struct aim_ays_ds_device_dma_buffer *my_obj;

    retval = aim_aye_ds_device_dma_reset(aimDevice);

    if (retval != 0)
    {
        aim_dev_error(aimDevice, "aim_aye_ds_device_dma_reset failed\n");
    }

    spin_lock_bh(&aimDevice->ScopeBufferQueueLock);

    /*
   * Remove all requests from list and pending list
   */

    list_for_each_safe(pos, q, &aimDevice->ScopeBufferQueue)
    {
        my_obj = list_entry(pos, struct aim_ays_ds_device_dma_buffer, list);

        aim_dma_buffer_free( &my_obj->dmaBuffer, &aimDevice->pciDevice->dev, false );

        list_del(pos);
        kfree(my_obj);
    }

    list_for_each_safe(pos, q, &aimDevice->ScopeBufferPendingQueue)
    {
        my_obj = list_entry(pos, struct aim_ays_ds_device_dma_buffer, list);

        aim_dma_buffer_free( &my_obj->dmaBuffer, &aimDevice->pciDevice->dev, false );

        list_del(pos);
        kfree(my_obj);
    }

    list_for_each_safe(pos, q, &aimDevice->ScopeBufferDoneQueue)
    {
        my_obj = list_entry(pos, struct aim_ays_ds_device_dma_buffer, list);

        aim_dma_buffer_free( &my_obj->dmaBuffer, &aimDevice->pciDevice->dev, false );

        list_del(pos);
        kfree(my_obj);
    }

    atomic_set(&aimDevice->ScopeBufferCanceled, 1);

    spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);

    /* complete waiting sync requests */
    wake_up_interruptible(&aimDevice->ScopeBufferDoneWaitQueue);

    return retval;
}




/*! \brief This function waits until all queued buffers are written into the fifo
 *
 * @param aimDevice the device the ioctl was issued on
 */
static int aim_aye_ds_device_bufferqueue_wait(struct aim_pci_device *aimDevice)
{
    int empty;
    int fifo_full;

    do
    {
        /* wait until all request are written to the fifo or the fifo is full */
        empty = list_empty(&aimDevice->ScopeBufferQueue);
        fifo_full = aim_aye_ds_device_fifo_full(aimDevice);

    } while (!empty && !fifo_full);

    return 0;
}




/*! \brief This function waits until a queued buffer is read to be read by the host
 *
 * @param aimDevice the device the ioctl was issued on
 * @param pFflags the synchronisation flags
 */
static int aim_aye_ds_device_bufferqueue_sync(struct aim_pci_device *aimDevice, unsigned int *pFflags)
{
    long wait_result = 0;
    struct aim_ays_ds_device_dma_buffer *my_obj = NULL;

    /* -- Check if a complete buffer is available --- */

    *pFflags = 0;

    do
    {
        spin_lock_bh(&aimDevice->ScopeBufferQueueLock);

        if (!list_empty(&aimDevice->ScopeBufferDoneQueue))
        {
            my_obj = list_first_entry(&aimDevice->ScopeBufferDoneQueue, struct aim_ays_ds_device_dma_buffer, list);

            list_del_init(&my_obj->list);
        }

        spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);

        /* -- if no buffer is available - wait --- */
        if (my_obj == NULL)
        {
            aim_dev_debug(aimDevice, "Waiting for next MS buffer completion\n");

            wait_result = wait_event_interruptible(aimDevice->ScopeBufferDoneWaitQueue,
                                                   !list_empty(&aimDevice->ScopeBufferDoneQueue) || atomic_read(&aimDevice->ScopeBufferCanceled));

            if (atomic_read(&aimDevice->ScopeBufferCanceled))
            {
                *pFflags = (1 << 1); /* SCOPE_BUFFER_FLAG_CANCELED */
                return 0;
            }

            if (wait_result < 0)
            {
                aim_dev_error(aimDevice, "aim_aye_ds_device_bufferqueue_sync Transfer interrupted\n");
                return -ERESTARTSYS;
            }
        }
    } while (my_obj == 0);

    /* We got a buffer to complete */

    aim_dev_debug(aimDevice, "aim_aye_ds_device_bufferqueue_sync Transfer successful\n");

    *pFflags = (1 << 0); /* SCOPE_BUFFER_FLAG_FILLED */


    aim_dma_buffer_free( &my_obj->dmaBuffer, &aimDevice->pciDevice->dev, false );
    kfree(my_obj); /* must be moved to finished list later */

    return 0;
}







/*****************************************************************************/
/*                                                                           */
/*                        IOCTL Handling Functions                           */
/*                                                                           */
/*****************************************************************************/





/*! \brief This function processes the provide scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_provide(struct aim_pci_device* device, struct aim_scope_buffer __user* argument)
{
    TY_MILSCOPE_BUFFER xMilscopeBuffer;

    aim_dev_debug(device, "Provide scope buffer\n");

    if(copy_from_user(&xMilscopeBuffer, (TY_MILSCOPE_BUFFER*) argument, sizeof(TY_MILSCOPE_BUFFER)))
    {
        aim_dev_error(device, "Couldn't transfer buffer parameters from user space\n");
        return -EFAULT;
    }

    return aim_aye_ds_device_bufferqueue_add( device, &xMilscopeBuffer );
}




/*! \brief This function processes the release scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 */
long aim_aye_ds_device_ioctl_scope_buffers_release(struct aim_pci_device* device)
{
    aim_dev_debug(device, "Release scope buffer\n");
    return aim_aye_ds_device_bufferqueue_release( device );
}




/*! \brief This function processes the provide scope buffers wait IOCTL
 *
 * @param device the device the ioctl was issued on
 */
long aim_aye_ds_device_ioctl_scope_buffers_wait(struct aim_pci_device* device)
{
    aim_dev_debug(device, "Wait until buffers are written to the hardware\n");
    return aim_aye_ds_device_bufferqueue_wait( device );
}




/*! \brief This function processes the sync scope buffers IOCTL
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
long aim_aye_ds_device_ioctl_scope_buffers_sync(struct aim_pci_device* device, unsigned int __user* argument)
{
    int ret;
    unsigned int flags;
    aim_dev_debug(device, "Wait until provided buffer is read\n");

    ret = aim_aye_ds_device_bufferqueue_sync( device, &flags );

    if( copy_to_user( argument, &flags, sizeof(unsigned int) ) )
    {
        aim_dev_error(device, "%s:%d buffer flags to user space failed  ERROR\n", __FILE__, __LINE__);
        ret = -EFAULT;
    }

    return ret;
}




/*****************************************************************************/
/*                                                                           */
/*                         IRQ Handling Functions                            */
/*                                                                           */
/*****************************************************************************/




/*! \brief This function processes the digital scope buffer done interrupt
 *
 * @param device the device the ioctl was issued on
 */
int aim_aye_ds_device_irq_buffer_finished(struct aim_pci_device *aimDevice)
{
    struct aim_ays_ds_device_dma_buffer *my_obj = NULL;

    spin_lock_bh(&aimDevice->ScopeBufferQueueLock);

    if (list_empty(&aimDevice->ScopeBufferPendingQueue))
    {
        aim_dev_error(aimDevice, "aim_aye_ds_device_irq_buffer_finished: No buffer in queue\n");
        spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);
        return -1;
    }

    my_obj = list_first_entry(&aimDevice->ScopeBufferPendingQueue, struct aim_ays_ds_device_dma_buffer, list);

    list_move_tail(&my_obj->list, &aimDevice->ScopeBufferDoneQueue);

    spin_unlock_bh(&aimDevice->ScopeBufferQueueLock);

    wake_up_interruptible(&aimDevice->ScopeBufferDoneWaitQueue);

    return aim_aye_ds_device_bufferqueue_drain(aimDevice);
}




/*! \brief This function processes the digital scope buffer error interrupt
 *
 * @param device the device the ioctl was issued on
 */
int aim_aye_ds_device_irq_buffer_error(struct aim_pci_device *aimDevice)
{
    aim_aye_ds_device_dma_reset(aimDevice);

    atomic_set(&aimDevice->ScopeBufferCanceled, 1);

    wake_up_interruptible(&aimDevice->ScopeBufferDoneWaitQueue);
    return 0;
}
