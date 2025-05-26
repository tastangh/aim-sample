/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_dma.h
 *
 *  This header file contains protocol independent
 *  declarations for DMA transactions
 *
 */

#ifndef AIM_DMA_H_
#define AIM_DMA_H_


#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/scatterlist.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <asm/page.h>
#include "aim_pci_device.h"
#include "aim_rw_interface.h"




#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)

struct sg_table {
    struct scatterlist *sgl;    /* the list */
    unsigned int nents;     /* number of mapped entries */
    unsigned int orig_nents;    /* original size of list */
};

#endif




/*! \def AIM_DMA_LOCKED
 * DMA buffer flag that shows if buffer's pages are locked
 */
#define AIM_DMA_LOCKED  (1 << 0)


/*! \def AIM_DMA_MAPPED
 * DMA buffer flag that shows if buffer is mapped for DMA
 */
#define AIM_DMA_MAPPED  (1 << 1)




/*! \struct aim_dma_buffer
 *
 * This structure holds a user-space buffer
 * that shall be written or read via DMA
 */
struct aim_dma_buffer
{
    void __user *bufferBase;        /*!< virtual base address of the buffer (as obtained by e.g. malloc) */
    size_t length;                  /*!< size of buffer in bytes */
    unsigned int numPages;          /*!< number of physical pages the buffer consists of */
    struct page** bufferPages;      /*!< array of pointers to the physical pages of the buffer */
    struct sg_table scatterTable;   /*!< scatter/gather table of the buffer used for DMA */
    unsigned int flags;             /*!< flags that signify a specific state of the buffer */
};


/*! \struct aim_board_dma_token
 * DMA token that can be used to wait for DMA transactions being finished
 */
struct aim_board_dma_token
{
    atomic_t finished;              /*!< Signifies if DMA transaction has finished */
    int status;                     /*!< Status of the DMA transaction after it has finished */
    size_t bytesTransferred;        /*!< total number of bytes transferred with the DMA transaction */
    struct list_head list;          /*!< list to insert token into a queue */
    struct completion completion;   /*!< wait completion to wait for DMA transaction being finished */
};


/*! \struct aim_pci_dma_ops
 * DMA specific handler functions
 */
struct aim_pci_dma_ops
{
    int (*start_dma_read)(struct aim_pci_device* aimDevice, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset);
    int (*get_dma_device_offset)(struct aim_pci_device* aimDevice, enum aim_memory_type memType, u64 * offset);
};


/*! \brief Computes the number of pages for a user space buffer
 *
 * @param bufferBase virtual base address of the buffer (as obtained by e.g. malloc)
 * @param length size of the buffer in bytes
 * @return returns number of pages the buffer utilizes
 */
static inline unsigned int aim_dma_num_pages(void __user* bufferBase, size_t length)
{
    return DIV_ROUND_UP(((uintptr_t) bufferBase) + length, PAGE_SIZE) - (uintptr_t) bufferBase / PAGE_SIZE;
}


/*! \brief Initializes a DMA buffer
 *
 * The functions computes the number of pages the buffer uses
 * by the given base address and size. \n
 * Then it allocates an array for the pages needed by the buffer. \n
 * This means the function could fail with -ENOMEM
 * @param dmaBuffer the DMA buffer to initialize
 * @param bufferBase the virtual base address to initialize the buffer with
 * @param length the size in bytes to initialize the buffer with
 * @return returns 0 on success, a negative error code otherwise
 */
static inline int aim_dma_buffer_init(struct aim_dma_buffer* dmaBuffer, void __user *bufferBase, __kernel_size_t length)
{
    if(!dmaBuffer)
    {
        return -EINVAL;
    }

    dmaBuffer->bufferBase = bufferBase;
    dmaBuffer->length = length;
    dmaBuffer->numPages = aim_dma_num_pages(bufferBase, length);
    dmaBuffer->flags = 0;
    dmaBuffer->bufferPages = kmalloc(sizeof(struct page*) * dmaBuffer->numPages, GFP_KERNEL);
    if(!dmaBuffer->bufferPages)
    {
        return -ENOMEM;
    }

    return 0;
}


/*! \brief Locks the physical pages of buffer
 *
 * This function locks the physical pages of a buffer in the page cache. \n
 * This is necessary so the pages can't be swapped out and given to other
 * processes during DMA transfer.
 * @param dmaBuffer DMA buffer to lock the pages of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_dma_buffer_lock_pages(struct aim_dma_buffer* dmaBuffer);


/*! \brief Unlocks the physical pages of buffer
 *
 * Unlocks the pages so they can be removed from the page cache. \n
 * The caller can specify if pages shall be marked dirty, which is necessary
 * if a device wrote into the buffer. \n
 * @param dmaBuffer DMA buffer to unlock the pages of
 * @param dirty specifies if pages are to be marked as dirty.
 */
void aim_dma_buffer_unlock_pages(struct aim_dma_buffer* dmaBuffer, bool dirty);


/*! \brief Creates the scatter/gather list for a DMA buffer
 *
 * The function will allocate a scather/gather list with the needed size. \n
 * Also, the scatter/gather list entries will be initialized with the corresponding
 * pages of the buffer, their offsets and sizes.
 * @param dmaBuffer DMA buffer to create scatter/gather list for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_dma_buffer_create_sg(struct aim_dma_buffer* dmaBuffer);


/*! \brief Frees the scatter/gather list of a DMA buffer
 *
 * @param dmaBuffer DMA buffer to free
 */
void aim_dma_buffer_free_sg(struct aim_dma_buffer* dmaBuffer);


/*! \brief Maps a DMA buffer for actual DMA transaction
 *
 * The scatter/gather list of the DMA buffer will be mapped
 * according to the DMA capabilities of the given device.
 * @param dmaBuffer DMA buffer to map
 * @param device device to map the buffer for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_dma_buffer_map(struct aim_dma_buffer* dmaBuffer, struct device* device);


/*! \brief Unmaps a DMA buffer
 *
 * @param dmaBuffer DMA buffer to unmap
 * @param device device to unmap the buffer from
 */
void aim_dma_buffer_unmap(struct aim_dma_buffer* dmaBuffer, struct device* device);


/*! \brief Free a DMA buffer
 *
 * This function unmaps the DMA buffer, frees its scatter/gather list
 * and unlocks its pages from the page cache.
 * @param dmaBuffer DMA buffer to destroy
 * @param device device this buffer could be mapped for
 * @param dirty signifies if pages shall be marked dirty
 */
void aim_dma_buffer_free(struct aim_dma_buffer* dmaBuffer, struct device* device, bool dirty);


/*! \brief Performs a DMA read on a specific PCI device
 *
 * @param device The device to read from
 * @param buffer user space buffer prepared for DMA to read into
 * @param memType board memory type to read from
 * @param offset offset in bytes to read from
 * @param size number of bytes to read
 * @return 0 on success, errno code otherwise
 */
int aim_pci_device_dma_read(struct aim_pci_device* device, struct aim_dma_buffer* buffer,
                            enum aim_memory_type memType, loff_t offset, size_t size);


/*! \brief Completes the first waiting DMA from the list
 *
 * @param device The device to read from
 * @return 0 on success, errno code otherwise
 */
int aim_pci_device_dma_complete(struct aim_pci_device* device );


/*! \brief Generic DMA function
 *
 * This function tries to start a DMA transaction according to the platform device. \n
 * No target software interaction must be necessary here
 * @param device device to start DMA read on
 * @param buffer user space buffer prepared for DMA to read into
 * @param memType board memory type to read from
 * @param offset offset in bytes to read from
 * @return
 */
int aim_pci_device_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType,
                                          loff_t offset);


#endif /* AIM_DMA_H_ */
