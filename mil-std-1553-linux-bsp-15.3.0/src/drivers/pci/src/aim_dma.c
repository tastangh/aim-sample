/* SPDX-FileCopyrightText: 2013-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_dma.c
 *
 *  This file contains protocol independent
 *  definitions for DMA transactions
 */


#include <linux/rwsem.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/dma-mapping.h>
#include <linux/pagemap.h>
#include "aim_dma.h"
#include "aim_debug.h"
#include "Ai_def.h"
#include "aim_aye_device.h"
#include "aim_zynqmp_device.h"
#include "aim_artixus_device.h"




/*! \def aim_dma_buffer_mapped
 * Checks if the 'mapped' flag of a buffer is set
 */
#define aim_dma_buffer_mapped(dmaBuffer)   (dmaBuffer->flags & AIM_DMA_MAPPED)


/*! \def aim_dma_buffer_locked
 * Checks if the 'locked' flag of a buffer is set
 */
#define aim_dma_buffer_locked(dmaBuffer)   (dmaBuffer->flags & AIM_DMA_LOCKED)


/*! \def AIM_GENERIC_DMA_TIMEOUT_MS
 *
 * Generic DMA transaction timeout in milliseconds
 */
#define AIM_GENERIC_DMA_TIMEOUT_MS  5000



/*! Allocates the scatter/gather list for a DMA buffer
 *
 * The number of pages of the buffer is used to determine the size of the scatter/gather list.
 * Allocation is performed via GFP_KERNEL and might therefore sleep
 * @param dmaBuffer DMA buffer to create the scatter/gather table for
 * @return returns 0 on success, a negative error code otherwise
 */
static inline int aim_dma_alloc_sg(struct aim_dma_buffer* dmaBuffer)
{
    int ret = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
    ret = sg_alloc_table(&dmaBuffer->scatterTable, dmaBuffer->numPages, GFP_KERNEL);
#else
    dmaBuffer->scatterTable.nents = dmaBuffer->numPages;
    dmaBuffer->scatterTable.orig_nents = dmaBuffer->numPages;
    dmaBuffer->scatterTable.sgl = kmalloc(sizeof(struct scatterlist) * dmaBuffer->numPages, GFP_KERNEL);
    if(dmaBuffer->scatterTable.sgl == NULL)
    {
        ret = -ENOMEM;
    }
    else
    {
        ret  = 0;
    }
#endif

    return ret;
}


static inline void lock_current_mmap_read(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,8,0)
    down_read(&current->mm->mmap_sem);
#else
    mmap_read_lock(current->mm);
#endif
}

static inline void unlock_current_mmap_read(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,8,0)
    up_read(&current->mm->mmap_sem);
#else
    mmap_read_unlock(current->mm);
#endif
}


int aim_dma_buffer_lock_pages(struct aim_dma_buffer* dmaBuffer)
{
    int ret = 0;

    if(!dmaBuffer)
    {
        return -EINVAL;
    }

    /* Now lock physical pages of user buffer in memory */
    lock_current_mmap_read();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,5,0)
    ret = get_user_pages((unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages, FOLL_WRITE,
                          dmaBuffer->bufferPages);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
    ret = get_user_pages((unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages, FOLL_WRITE,
                          dmaBuffer->bufferPages, NULL);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0)
    ret = get_user_pages((unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages,
                             1, 0, dmaBuffer->bufferPages, NULL);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
    ret = get_user_pages(current, current->mm, (unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages,
                         1, 0, dmaBuffer->bufferPages, NULL);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,168)
    ret = get_user_pages(current, current->mm, (unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages,
                         FOLL_WRITE, dmaBuffer->bufferPages, NULL);
#else
    ret = get_user_pages(current, current->mm, (unsigned long) dmaBuffer->bufferBase, dmaBuffer->numPages,
                         1, 0, dmaBuffer->bufferPages, NULL);
#endif
    if(ret < 0 || ret < dmaBuffer->numPages)
    {
        aim_error("Not all physical pages could be locked for DMA\n");
        unlock_current_mmap_read();
        return ret;
    }

    unlock_current_mmap_read();

    dmaBuffer->flags |= AIM_DMA_LOCKED;

    return 0;
}


int aim_dma_buffer_create_sg(struct aim_dma_buffer* dmaBuffer)
{
    int ret = 0;
    struct scatterlist* scatterEntry = NULL;
    int i = 0;
    size_t bytesLeft = 0;
    size_t scatterEntryLength = 0;
    void __user* scatterEntryBase = NULL;

    if(!dmaBuffer)
    {
        return -EINVAL;
    }

    ret = aim_dma_alloc_sg(dmaBuffer);
    if(ret)
    {
        aim_error("Allocation of Scatter Table failed\n");
        return ret;
    }

    /* Now set page, length and offset for every scatterlist entry */
    scatterEntry = dmaBuffer->scatterTable.sgl;
    bytesLeft = dmaBuffer->length;
    scatterEntryBase = dmaBuffer->bufferBase;

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,24)
    sg_init_table(scatterEntry, dmaBuffer->scatterTable.orig_nents);
#endif

    for (i = 0; i < dmaBuffer->numPages; i++)
    {
        /* Determine the offset and length of the current scatter/entry */
        if(bytesLeft > (PAGE_SIZE - offset_in_page(scatterEntryBase)))
        {
            scatterEntryLength = PAGE_SIZE - offset_in_page(scatterEntryBase);
        }
        else
        {
            scatterEntryLength = bytesLeft;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
        sg_set_page(scatterEntry, dmaBuffer->bufferPages[i], scatterEntryLength, offset_in_page(scatterEntryBase));
#else
        scatterEntry->page = dmaBuffer->bufferPages[i];
        scatterEntry->offset = offset_in_page(scatterEntryBase);
        scatterEntry->length = scatterEntryLength;
#endif


        /* Now go to the next scatter gather entry */
        bytesLeft -= scatterEntryLength;
        scatterEntryBase += scatterEntryLength;

#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
        scatterEntry++;
#else
        scatterEntry = sg_next(scatterEntry);
#endif

    }

    return ret;
}


int aim_dma_buffer_map(struct aim_dma_buffer* dmaBuffer, struct device* device)
{
    if(!dmaBuffer)
    {
        return -EINVAL;
    }

    if(!dmaBuffer->scatterTable.sgl)
    {
        return -EFAULT;
    }

    dmaBuffer->scatterTable.nents = dma_map_sg(device, dmaBuffer->scatterTable.sgl,
                                               dmaBuffer->scatterTable.orig_nents, DMA_FROM_DEVICE);

    dmaBuffer->flags |= AIM_DMA_MAPPED;

    return 0;
}


void aim_dma_buffer_free_sg(struct aim_dma_buffer* dmaBuffer)
{
    if(!dmaBuffer || !dmaBuffer->scatterTable.sgl)
    {
        return;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
    sg_free_table(&dmaBuffer->scatterTable);
#else
    kfree(dmaBuffer->scatterTable.sgl);
    dmaBuffer->scatterTable.nents = 0;
    dmaBuffer->scatterTable.orig_nents = 0;
#endif

}


void aim_dma_buffer_free(struct aim_dma_buffer* dmaBuffer, struct device* device, bool dirty)
{
    if(!dmaBuffer)
    {
        return;
    }

    aim_dma_buffer_unmap(dmaBuffer, device);

    aim_dma_buffer_free_sg(dmaBuffer);

    aim_dma_buffer_unlock_pages(dmaBuffer, dirty);

    if(dmaBuffer->bufferPages)
    {
        kfree(dmaBuffer->bufferPages);
        dmaBuffer->bufferPages = NULL;
    }
}


void aim_dma_buffer_unmap(struct aim_dma_buffer* dmaBuffer, struct device* device)
{
    if(!dmaBuffer || !device || !dmaBuffer->scatterTable.sgl || !aim_dma_buffer_mapped(dmaBuffer))
    {
        return;
    }

    dma_unmap_sg(device, dmaBuffer->scatterTable.sgl, dmaBuffer->scatterTable.orig_nents, DMA_FROM_DEVICE);

    dmaBuffer->scatterTable.nents = dmaBuffer->scatterTable.orig_nents;

    dmaBuffer->flags &= ~AIM_DMA_MAPPED;
}


void aim_dma_buffer_unlock_pages(struct aim_dma_buffer* dmaBuffer, bool dirty)
{
    int i = 0;

    if(!dmaBuffer || !dmaBuffer->bufferPages || !aim_dma_buffer_locked(dmaBuffer))
    {
        return;
    }

    for(i = 0; i < dmaBuffer->numPages; i++)
    {
        if(dirty)
        {
            if(!PageReserved(dmaBuffer->bufferPages[i]))
            {
                SetPageDirty(dmaBuffer->bufferPages[i]);
            }
        }

        put_page(dmaBuffer->bufferPages[i]);
    }

    dmaBuffer->flags &= ~AIM_DMA_LOCKED;
}


int aim_pci_device_dma_read(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType,
                            loff_t offset, size_t size)
{
    struct aim_board_dma_token* dmaToken = NULL;
    int ret = 0;
    unsigned long timeout = msecs_to_jiffies(AIM_GENERIC_DMA_TIMEOUT_MS);
    unsigned long flags;

    BUG_ON(!device || !buffer || !aim_dma_buffer_mapped(buffer));

    if(!device->deviceData || !device->deviceData->dmaOps || !device->deviceData->dmaOps->start_dma_read)
    {
        return -EIO;
    }

    /* Create and set-up token for this DMA transaction */
    dmaToken = kzalloc(sizeof(struct aim_board_dma_token), GFP_KERNEL);
    if(!dmaToken)
        return -ENOMEM;

    INIT_LIST_HEAD(&dmaToken->list);
    atomic_set(&dmaToken->finished, 0);
    init_completion(&dmaToken->completion);

    mutex_lock(&device->dmaRequestLock);

    spin_lock_irqsave(&device->dmaQueueLock, flags);
    list_add_tail(&dmaToken->list, &device->dmaQueue);
    spin_unlock_irqrestore(&device->dmaQueueLock, flags);

    do
    {
        ret = device->deviceData->dmaOps->start_dma_read(device, buffer, memType, offset);
        if(ret)
        {
            break;
        }

        ret = wait_for_completion_timeout(&dmaToken->completion, timeout);
        if(ret == 0)
        {
            aim_dev_error(device, "DMA timeout");
            ret = -ETIME;
            break;
        }

        ret = 0;

    }while(0);

    mutex_unlock(&device->dmaRequestLock);
    
    kfree(dmaToken);

    return ret;
}



int aim_pci_device_dma_complete(struct aim_pci_device* device )
{
    unsigned long flags = 0;
    struct aim_board_dma_token* dmaTransaction = NULL;

    spin_lock_irqsave(&device->dmaQueueLock, flags);

    if(list_empty(&device->dmaQueue))
    {
        /* @AIMFIXME: When using hostio and dma in parallel tasks we get to many interrupts. */
        aim_dev_error(device, "aim_pci_device_dma_complete: No DMA request available for completion.");
    }
    else
    {
        dmaTransaction = list_first_entry(&device->dmaQueue, struct aim_board_dma_token, list);
        list_del(&dmaTransaction->list);
    }

    spin_unlock_irqrestore(&device->dmaQueueLock, flags);

    if(dmaTransaction)
        complete(&dmaTransaction->completion);

    return 0;
}




int aim_pci_device_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType,
                                          loff_t offset)
{
    int ret = 0;

    BUG_ON(!device || !buffer || !aim_dma_buffer_mapped(buffer));

    switch(device->deviceData->platform)
    {

#ifdef AIM_AYE
    case AI_DEVICE_AYE:
    case AI_DEVICE_AYE_ASP:
        return aim_aye_start_dma_read_generic(device, buffer, memType, offset);
#endif

#ifdef AIM_ZYNQMP
    case AI_DEVICE_ZYNQMP_ASP:
        return aim_zynqmp_start_dma_read_generic(device, buffer, memType, offset);
#endif

#ifdef AIM_ARTIXUS
    case AI_DEVICE_ARTIXUS:
        return aim_artixus_start_dma_read_generic(device, buffer, memType, offset);
#endif

    default:
        aim_dev_error(device, "Platform does not support generic DMA reads");
        return -EIO;
    }

    return ret;
}

