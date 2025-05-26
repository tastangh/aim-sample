/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_mem.h
 *
 *  This header file contains declarations for
 *  protocol independent handling of PCI memory resources
 *
 */

#ifndef AIM_PCI_MEM_H_
#define AIM_PCI_MEM_H_




#include "aim_pci_device.h"
#include "aim_rw_interface.h"




/*! \enum aim_mem_transfer_dir
 * Transfer directions of memory transfers on AIM PCI devices
 */
enum aim_mem_transfer_dir
{
    AIM_MEM_READ = 0,   /*!< Transfer of data from hardware to kernel/user-space */
    AIM_MEM_WRITE       /*!< Transfer of data from kernel/user-space to hardware */
};


/*! \brief Initialisation function for memory management module
 *
 * Has to be called by each driver module before using any of the
 * interface functions.
 * @return  returns 0 on success, negative error code otherwise
 */
int aim_mem_init(void);


/*! \brief Exit function for memory management module
 *
 * Releases all resources held by the memory management module
 */
void aim_mem_exit(void);



/*! \brief Allocate a cache buffer from the aim memory cache for this module
 *
 * @return  returns The address of the cache buffer or zero on failue
 */
void * aim_mem_cache_alloc(void);


/*! \brief Free a chache buffer from the aim memory cache for this module
 *
 * @param buffer is the pointer to the cache buffer to be freed.
 */
void aim_mem_cache_free(void * buffer);


/*! \brief Reads a 32Bit value from IO memory
 *
 * This function reads a 32Bit value from a specific offset \n
 * of the IO memory area of an AIM PCI device.
 * @param aimDevice device to read the value from
 * @param offset byte offset in IO memory area to read from
 * @return returns the vaue that is read from hardware
 */
static inline u32 aim_pci_io_read32(struct aim_pci_device* aimDevice, loff_t offset)
{
    BUG_ON(!aimDevice || !aimDevice->ioMemory);
    return ioread32(aimDevice->ioMemory + offset);
}


/*! \brief Writes a 32Bit value to IO memory
 *
 * This function writes a 32Bit value to a specific offset \n
 * of the IO memory area of an AIM PCI device.
 * @param aimDevice device to write to
 * @param offset byte offset in IO memory area to write to
 * @param value value to write
 */
static inline void aim_pci_io_write32(struct aim_pci_device* aimDevice, loff_t offset, u32 value)
{
    BUG_ON(!aimDevice || !aimDevice->ioMemory);
    iowrite32(value, aimDevice->ioMemory + offset);
}




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
ssize_t aim_mem_transfer_user(struct aim_pci_device* aimDevice, struct aim_data_transaction_user* parameters, enum aim_mem_transfer_dir direction);


/*! \brief Transfers data between hardware and local space
 *
 *  This function will either read data from hardware memory and copy it to \n
 *  local memory or read data from local memory and copy it to hardware memory. \n
 *  32-Bit PCI bus accesses are used for for hardware data transfers, no DMA \n
 *  is used.
 * @param hw The HW memory access pointer
 * @param data The local memory pointer
 * @param bytes_to_copy The number of bytes to read/write
 * @param direction The direction of the memory transfer, either read or write
 * @return  returns number of bytes transferred on success, negative error code otherwise
 */
void aim_mem_copy( void __iomem * hw, void * data, __kernel_ssize_t bytes_to_copy, enum aim_mem_transfer_dir dir );



#ifdef AIM_DEBUG_MEM

/*! \brief Simple shared memory test
 *
 * This function just writes 32-bit values to shared memory,
 * reads them back and checks them
 * \param device device to check shared memory of.
 * \param size number of bytes to check
 */
static __inline__ void aim_pci_mem_shared_test(struct aim_pci_device* device, size_t size)
{
    unsigned int i;
    __u32 val = 0;

    BUG_ON(!device || !device->sharedMemory);


    for(i = 0;i < size; i+=4)
    {
        iowrite32(i, device->sharedMemory + i);
        val = ioread32(device->sharedMemory + i);

        if(val != i)
        {
            aim_dev_error(device, "Wrong shared mem value at offset 0x%x. Expected %u, got %u",i, i, val);
        }
    }
}


/*! \brief Dumps start of IO memory for a given device
 *
 * \param device device for IO memory is dumped
 * \param size number of bytes to dump
 */
static __inline__ void aim_pci_mem_io_dump(struct aim_pci_device* device, size_t size)
{
    unsigned int i;
    __u32 val = 0;

    BUG_ON(!device || !device->ioMemory);

    for(i = 0; i < size; i+=4)
    {
        val = ioread32(device->ioMemory + i);
        aim_dev_debug(device, "IO mem at %i : 0x%x\n", i, val);
    }

}

#else

static __inline__ void aim_pci_mem_shared_test(struct aim_pci_device* device, size_t size){}
static __inline__ void aim_pci_mem_io_dump(struct aim_pci_device* device, size_t size){}

#endif


/*! \brief Transfers data from hardware to kernel space
 *
 * This function will copy data from hardware memory to kernel space\n
 * At most, 64-Bit PCI bus accesses are used for transferring the data\n
 * @param dest pointer to kernel space buffer to copy data to
 * @param src pointer to I/O memory to copy data from
 * @param bytes_to_copy The number of bytes to copy
 * @return  none
 */
static inline void memcpy64_fromio(void* dest, const volatile void __iomem* src, size_t bytes_to_copy)
{
    volatile __u64* dest_64;
    const volatile __iomem __u64* src_64;
    volatile __u32* dest_32;
    const volatile __iomem __u32* src_32;
    volatile __u8*  dest_8;
    const volatile __iomem __u8*  src_8;

    dest_64 = (volatile __u64*)dest;
    src_64  = (const volatile __iomem __u64*)src;
    // -- copy 64bit chunks first ---
    while (bytes_to_copy >= sizeof(__u64))
    {
        *(dest_64++) = *(src_64++);
        bytes_to_copy = bytes_to_copy - sizeof(__u64);
    }

    dest_32 = (volatile __u32*)dest_64;
    src_32  = (const volatile __iomem __u32*)src_64;
    // -- copy 32bit chunks ---
    while (bytes_to_copy >= sizeof(__u32))
    {
        *(dest_32++) = *(src_32++);
        bytes_to_copy = bytes_to_copy - sizeof(__u32);
    }

    // -- copy rest in bytes ---
    dest_8 = (volatile __u8*)dest_32;
    src_8  = (const volatile __iomem __u8*)src_32;
    while (bytes_to_copy > 0)
    {
        *(dest_8++) = *(src_8++);
        bytes_to_copy--;
    }
}

/*! \brief Transfers data from kernel space to hardware
 *
 * This function will copy data from kernel space to hardware memory\n
 * At most, 64-Bit PCI bus accesses are used for transferring the data\n
 * @param dest pointer to I/O memory to copy the data to
 * @param src pointer to kernel space memory to copy data from
 * @param bytes_to_copy The number of bytes to copy
 * @return  none
 */
static inline void memcpy64_toio(volatile void __iomem* dest, const void* src, size_t bytes_to_copy)
{
    volatile __iomem __u64* dest_64;
    const  __u64* src_64;
    volatile __iomem __u32* dest_32;
    const __u32* src_32;
    volatile __iomem __u8*  dest_8;
    const __u8*  src_8;

    dest_64 = (volatile __iomem __u64*) dest;
    src_64  = (const  __u64*) src;
    // -- copy 64bit chunks first ---
    while (bytes_to_copy >= sizeof(__u64))
    {
        *(dest_64++) = *(src_64++);
        bytes_to_copy = bytes_to_copy - sizeof(__u64);
    }

    dest_32 = (volatile __iomem __u32*) dest_64;
    src_32  = (const __u32*) src_64;
    // -- copy 32bit chunks ---
    while (bytes_to_copy >= sizeof(__u32))
    {
        *(dest_32++) = *(src_32++);
        bytes_to_copy = bytes_to_copy - sizeof(__u32);
    }
    // -- copy rest in bytes ---
    dest_8 = (volatile __iomem __u8*) dest_32;
    src_8  = (const __u8*) src_32;
    while (bytes_to_copy > 0)
    {
        *(dest_8++) = *(src_8++);
        bytes_to_copy--;
    }
}



#endif /* AIM_PCI_MEM_H_ */
