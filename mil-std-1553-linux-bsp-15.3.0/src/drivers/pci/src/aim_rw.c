/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_rw.c
 *
 *  This file contains the protocol independent definitions
 *  for the read/write system call on AIM PCI drivers
 *
 */


#include <linux/uaccess.h>
#include "aim_rw.h"
#include "aim_rw_interface.h"
#include "aim_debug.h"
#include "aim_pci_device.h"
#include "aim_pci_mem.h"
#include "aim_dma.h"



/*! \brief Handles the read system call when DMA read is requested
 *
 * This function will create a DMA buffer for the specified read buffer \n
 * and will forward the read request to teh DMA module
 * @param device the device to read from
 * @param readParameters the read call parameters
 * @return The number of bytes read on success, errno code otherwise
 */
static ssize_t aim_read_dma(struct aim_pci_device* device, const struct aim_data_transaction_user* readParameters)
{
    struct aim_dma_buffer dmaBuffer;
    int ret = 0;
    size_t readSize = 0;
    bool dirty = false;

    BUG_ON(!device || !readParameters);

    /* Calculate actual read size in bytes */
    readSize = readParameters->size * readParameters->numObjects;

    aim_dev_debug(device, "Reading %zu bytes from memory %d at offset %lld using DMA", readSize, readParameters->memType, readParameters->offset);

    do
    {
        ret = aim_dma_buffer_init(&dmaBuffer, readParameters->dataBuffer, readSize);
        if(ret)
        {
            aim_dev_error(device, "Failed to init DMA buffer\n");
            break;
        }

        ret = aim_dma_buffer_lock_pages(&dmaBuffer);
        if(ret)
        {
            aim_dev_error(device, "Locking of pages failed %d\n", ret);
            break;
        }

        ret = aim_dma_buffer_create_sg(&dmaBuffer);
        if(ret)
        {
            aim_dev_error(device, "Creation of scatter/gather table failed\n");
            break;
        }

        ret = aim_dma_buffer_map(&dmaBuffer, &device->pciDevice->dev);
        if(ret)
        {
            aim_dev_error(device, "Mapping of scatter/gather table failed\n");
            break;
        }

        ret = aim_pci_device_dma_read(device, &dmaBuffer, readParameters->memType, readParameters->offset, readSize);
        if(ret)
        {
            aim_dev_error(device, "PCI device read DMA failed\n");
            break;
        }

        dirty = true;

    }while(0);


    aim_dma_buffer_free(&dmaBuffer, &device->pciDevice->dev, dirty);


    return ret == 0 ? readSize : ret;

}




ssize_t aim_read(struct file* file, char __user* parameters, size_t parameterSize, loff_t* offset)
{
    struct aim_pci_device* aimDevice = NULL;
    struct aim_data_transaction_user readParameters = {0};

    BUG_ON(!file || !parameters);

    aimDevice = (struct aim_pci_device*) file->private_data;
    if(!aimDevice)
    {
        aim_error("No valid device for read request\n");
        return -ENODEV;
    }

    /* Copy the read parameters from user space */
    if(parameterSize != sizeof(struct aim_data_transaction_user))
    {
        aim_dev_error(aimDevice, "Invalid parameter size for request\n");
        return -EINVAL;
    }

    if(copy_from_user(&readParameters, parameters, sizeof(struct aim_data_transaction_user)))
    {
        aim_dev_error(aimDevice, "Failed to copy parameters from user space\n");
        return -EFAULT;
    }

    aim_dev_debug(aimDevice, "Reading memory type %d with offset %lld. %zu Objects with size %zu are read. (DMA: %d)\n",
                  readParameters.memType, readParameters.offset, readParameters.numObjects, readParameters.size, readParameters.useDMA);

    if(readParameters.useDMA)
    {
        return aim_read_dma(aimDevice, &readParameters);
    }

    return aim_mem_transfer_user(aimDevice, &readParameters, READ);
}


ssize_t aim_write(struct file* file, const char __user* parameters, size_t parameterSize, loff_t* offset)
{
    struct aim_pci_device* aimDevice = NULL;
    struct aim_data_transaction_user writeParameters = {0};

    BUG_ON(!file || !parameters);

    aimDevice = (struct aim_pci_device*) file->private_data;
    if (!aimDevice)
    {
        aim_error("No valid device for write request\n");
        return -ENODEV;
    }

    if(parameterSize != sizeof(struct aim_data_transaction_user))
    {
        aim_dev_error(aimDevice, "Invalid parameter size for request\n");
        return -EINVAL;
    }

    /* Copy the write parameters from user space */
    if(copy_from_user(&writeParameters, parameters, sizeof(struct aim_data_transaction_user)))
    {
        aim_dev_error(aimDevice, "Failed to copy parameters from user space\n");
        return -EFAULT;
    }

    aim_dev_debug(aimDevice, "Writing memory type %d with offset %lld. %zu Objects with size %zu are written. (DMA: %d)\n",
                  writeParameters.memType, writeParameters.offset, writeParameters.numObjects, writeParameters.size, writeParameters.useDMA);

    if(writeParameters.useDMA)
    {
        aim_dev_error(aimDevice, "Driver does not support DMA writes\n");
        return -EINVAL;
    }

    return aim_mem_transfer_user(aimDevice, &writeParameters, WRITE);
}

int aim_mmap(struct file *file, struct vm_area_struct *vma )
{
    /* This is not used for typical access to the driver	*/
    /* This code will enable the user to get a direct pointer to the
       memory specified 	*/

    struct aim_pci_device* aimDevice;
    int ret;
    unsigned int base = 0;
    size_t       size = 0;

    aimDevice = (struct aim_pci_device*)file->private_data;

    if(!aimDevice)
    {
        aim_dev_error(aimDevice, "No valid device info\n");
        return -ENODEV;
    }

    switch(vma->vm_pgoff<<PAGE_SHIFT)
    {
       case AI_MMAP_OFFSET(AI_MEMTYPE_GLOBAL):
          base = aimDevice->globalMemoryPhysical;
          size = (size_t)aimDevice->globalMemorySize;
          aim_dev_debug(aimDevice, "Mapping global memory size=0x%zx\n", size);
          break;
       case AI_MMAP_OFFSET(AI_MEMTYPE_SHARED):
          base = aimDevice->sharedMemoryPhysical;
          size = (size_t)aimDevice->sharedMemorySize;
          aim_dev_debug(aimDevice, "Mapping shared memory size=0x%zx\n", size);
          break;
       case AI_MMAP_OFFSET(AI_MEMTYPE_IO):
          base = aimDevice->ioMemoryPhysical;
          size = (size_t)aimDevice->ioMemorySize;
          aim_dev_debug(aimDevice, "Mapping io memory size=0x%zx\n", size);
          break;
       default:
          aim_dev_error(aimDevice, "Invalid offset. Must be AI_MMAP_OFFSET(n) with n in [AI_MEMTYPE_GLOBAL,AI_MEMTYPE_IO,AI_MEMTYPE_IO]\n");
          return -EINVAL;
    }

    if( base == 0 )
    {
        aim_dev_error(aimDevice, "Physical address for memtype not available\n");
        return -ENODEV;
    }

    /* test size validity */
    if ( (vma->vm_end - vma->vm_start) > size )
    {
        aim_dev_error(aimDevice, "Size 0x%lx is too big, max authorize is 0x%zx", vma->vm_end - vma->vm_start, size );
        return -EINVAL;
    }

    /* mapping */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    ret = remap_pfn_range(
            vma,                            /* user vma to map to */
            vma->vm_start,                  /* target user address to start at */
            base>>PAGE_SHIFT,               /* physical address of kernel memory */
            vma->vm_end - vma->vm_start,    /* size of map area */
            vma->vm_page_prot
          );

    if (0 != ret)
    {
        aim_dev_error(aimDevice, "remap_pfn_range returned an error (%d)", ret);
    }

    return ret;
}



