/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_mem.c
 *
 *  This header file contains declarations for
 *  protocol independent handling of PCI memory resources
 *
 */

#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include "aim_pci_mem.h"
#include "aim_debug.h"


#define SWAP_BUFFER_SIZE (16 * 1024)
#define KMALLOC_LIMIT (2*1024*1024) /*Kmalloc can allocated a approx. 4MB (Depends on OS version, page size and MAX_ORDER)*/


/*! Cache for swapping data that has to be transferred between hardware and user space */
static struct kmem_cache* aim_swap_cache = NULL;


/*
    memcpy
*/
void aim_mem_copy( void __iomem * hw, void * data, __kernel_ssize_t bytes_to_copy, enum aim_mem_transfer_dir dir )
{
    volatile __u32 __iomem*  lhw      = (__u32  __iomem *)hw;
    volatile __u32*  ldata    = (__u32*)data;

    volatile __u8 __iomem*   uhw      = (__u8 __iomem*)hw;
    volatile __u8*   udata    = (__u8*)data;

    // -- copy long words first ---

    while( bytes_to_copy >= 4 )
    {
        if( dir == AIM_MEM_WRITE )
            __raw_writel(*ldata, lhw);
        else
            *ldata = __raw_readl(lhw);

        bytes_to_copy -= 4;
        ldata++;
        lhw++;
    }

    // -- copy rest in bytes ---

    uhw      = (__u8 __iomem*)lhw;
    udata    = (__u8*)ldata;

    while( bytes_to_copy > 0 )
    {
        if( dir == AIM_MEM_WRITE )
            __raw_writeb(*udata, uhw);
        else
            *udata = __raw_readb(uhw);

        bytes_to_copy--;

        udata++;
        uhw++;
    }
}

#ifdef __BIG_ENDIAN
void aim_mem_swap_buffer_32( void __user * data, __kernel_ssize_t count, enum aim_mem_transfer_dir dir )
{
    __kernel_ssize_t i;
    volatile __u32*  ldata      = (__u32*)data;

    for( i=0; i<count; i++ )
    {
        if( dir == AIM_MEM_WRITE )
            ldata[i] = __cpu_to_le32(ldata[i]);
        else
            ldata[i] = __le32_to_cpu(ldata[i]);
    }
}

void aim_mem_swap_buffer_16( void __user * data, __kernel_ssize_t count, enum aim_mem_transfer_dir dir )
{
    __kernel_ssize_t i;
    volatile __u16*  ldata      = (__u16*)data;

    for( i=0; i<count; i++ )
    {
        if( dir == AIM_MEM_WRITE )
            ldata[i] = __cpu_to_le16(ldata[i]);
        else
            ldata[i] = __le16_to_cpu(ldata[i]);
    }
}

#endif


static void __inline__ aim_mem_swap_buffer( void* data, __kernel_ssize_t width,
                                            __kernel_ssize_t count, enum aim_mem_transfer_dir dir )
{
#ifdef __BIG_ENDIAN
    switch(width)
    {
    case 4:
        aim_mem_swap_buffer_32( data, count, dir );
        break;
    case 2:
        aim_mem_swap_buffer_16( data, count, dir );
        break;
    default:
        break;
    }
#endif
}

static ssize_t aim_mem_copy_from_hw_to_user( void __iomem * hw, void __user * data, void * swap_buffer,
                                             __kernel_ssize_t width, __kernel_ssize_t count )
{
    size_t byte_size = width*count;
    int err;

    err = 0;

    do
    {
        aim_mem_copy(hw,swap_buffer,byte_size,AIM_MEM_READ);

        aim_mem_swap_buffer( swap_buffer, width, count, AIM_MEM_READ );

        if(copy_to_user(data,swap_buffer,byte_size))
        {
            err = EFAULT;
            break;
        }

    }while(0);

    return err ? err : byte_size;
}

static ssize_t aim_mem_copy_from_user_to_hw( void __user * data, void __iomem * hw, void * swap_buffer,
                                             __kernel_ssize_t width, __kernel_ssize_t count )
{
    size_t byte_size = width*count;
    int err;

    err = 0;

    do
    {
        if(copy_from_user(swap_buffer,data,byte_size))
        {
            err = -EFAULT;
            break;
        }

        aim_mem_swap_buffer( swap_buffer, width, count, AIM_MEM_WRITE );

        aim_mem_copy(hw,swap_buffer,byte_size,AIM_MEM_WRITE);

    }while(0);

    return err ? err : byte_size;
}

enum allocator_type { allocator_cache, allocator_kmalloc, allocator_vmalloc };

/*! \brief Transfers data between hardware and user space
 *
 *  This function will either read data from hardware memory and copy it to \n
 *  user space or read data from user space and copy it to hardware memory. \n
 *  32-Bit PCI bus accesses are used for for hardware data transfers, no DMA \n
 *  is used.
 * @param aimDevice The AIM PCI device to transfer memory for
 * @param parameters memory transaction parameters
 * @param direction The direction of the memory transfer, either read or write
 * @return  returns number of bytes transferred on success, negative error code otherwise
 */
ssize_t aim_mem_transfer_user(struct aim_pci_device* aimDevice, struct aim_data_transaction_user* parameters, enum aim_mem_transfer_dir direction)
{
    void __iomem *hwMemory = NULL;
    resource_size_t maxSize = 0;
    ssize_t bytesTransferred = 0;
    void * swap_buffer = NULL;
    enum allocator_type allocator;

    /* Determine the hw address and maximum size of the memory type requested */
    switch (parameters->memType)
    {
    case AIM_MEMTYPE_SHARED:
        hwMemory = aimDevice->sharedMemory;
        maxSize = aimDevice->sharedMemorySize;
        break;
    case AIM_MEMTYPE_GLOBAL:
    case AIM_MEMTYPE_GLOBAL_UNCACHED:
        hwMemory = aimDevice->globalMemory;
        maxSize = aimDevice->globalMemorySize;
        break;
    case AIM_MEMTYPE_IO:
        hwMemory = aimDevice->ioMemory;
        maxSize = aimDevice->ioMemorySize;
        break;
    case AIM_MEMTYPE_GLOBAL_EXTENSION:
        hwMemory = aimDevice->globalExtMemory;
        maxSize = aimDevice->globalExtMemorySize;
        break;
    default:
        aim_dev_error(aimDevice, "Memory type %d not supported by this driver\n", parameters->memType);
        return -EINVAL;
    }

    if (!hwMemory)
    {
        aim_dev_error(aimDevice, "Memory type %d not mapped for this board\n", parameters->memType);
        return -EINVAL;
    }

    /* Check if transfer is out of bounds on hardware memory.
     * If this is the case, return error, we do no partial transfers!
     */
    if((parameters->offset + (parameters->size * parameters->numObjects)) > maxSize)
    {
        aim_dev_error(aimDevice, "Accessing memory %d out of bounds (offset: %lld size %zu bytes)\n",
                      parameters->memType, parameters->offset, parameters->size * parameters->numObjects);
        return -EINVAL;
    }

    /* Now transfer data in width chunks to be able to swap properly */
    hwMemory += parameters->offset;


    if( (parameters->size * parameters->numObjects) <= SWAP_BUFFER_SIZE)
    {
        swap_buffer = kmem_cache_alloc(aim_swap_cache, GFP_KERNEL);
        allocator   = allocator_cache;
    }
    else if( (parameters->size * parameters->numObjects) <= KMALLOC_LIMIT)
    {
        swap_buffer = kmalloc(parameters->size * parameters->numObjects, GFP_KERNEL);
        allocator   = allocator_kmalloc;
    }

    if( swap_buffer == NULL )
    {
        /* The previous allocation might have failed. */
        /* - size was too big for kmalloc
         * - kmalloc could not get a buffer
         * */

        swap_buffer = vmalloc(parameters->size * parameters->numObjects);
        allocator   = allocator_vmalloc;
    }

    if( swap_buffer == NULL )
        return -ENOMEM;

    if( direction == AIM_MEM_READ )
        bytesTransferred = aim_mem_copy_from_hw_to_user( hwMemory, parameters->dataBuffer, swap_buffer, parameters->size, parameters->numObjects );
    else
        bytesTransferred = aim_mem_copy_from_user_to_hw( parameters->dataBuffer, hwMemory, swap_buffer, parameters->size, parameters->numObjects );


    switch(allocator)
    {
        case allocator_cache:   kmem_cache_free(aim_swap_cache, swap_buffer); break;
        case allocator_kmalloc: kfree(swap_buffer); break;
        case allocator_vmalloc: vfree(swap_buffer); break;
    }


    if( bytesTransferred == 0 )
        aim_dev_warning(aimDevice, "Copy of zero bytes\n" );

    if( bytesTransferred < 0 )
        aim_dev_error(aimDevice, "Copy data failed with %zd\n", bytesTransferred );

    return bytesTransferred;
}


int aim_mem_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,16,0)
    // Debian 10 Kernel 4.19.0-8-amd64 uses CONFIG_HARDENED_USERCOPY
    // This raises an error if kmem_cache_create is used in combination with copy_from_user
    aim_swap_cache = kmem_cache_create_usercopy(AIM_DRV_NAME, SWAP_BUFFER_SIZE, 4, 0, 0, SWAP_BUFFER_SIZE, NULL);
#else
    aim_swap_cache = kmem_cache_create(AIM_DRV_NAME, SWAP_BUFFER_SIZE, 4, 0, NULL);
#endif

    if(IS_ERR_OR_NULL(aim_swap_cache))
    {
        aim_error("Failed to create memory cache");
        return -ENOMEM;
    }

    return 0;
}


void aim_mem_exit(void)
{
    kmem_cache_destroy(aim_swap_cache);
}





/*! \brief Allocate a cache buffer from the aim memory cache for this module
 *
 * @return  returns The address of the cache buffer or zero on failue
 */
void * aim_mem_cache_alloc(void)
{
    return kmem_cache_alloc(aim_swap_cache, GFP_KERNEL);
}


/*! \brief Free a chache buffer from the aim memory cache for this module
 *
 */
void aim_mem_cache_free(void * buffer)
{
    kmem_cache_free(aim_swap_cache, buffer);
}
