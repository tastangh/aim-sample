/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file aim_rw_interface.h
 *
 *  This header file contains the user space interface declarations
 *  for the read/write system call on AIM PCI drivers
 *
 */

#ifndef AIM_RW_INTERFACE_H_
#define AIM_RW_INTERFACE_H_


#include <linux/types.h>

#ifndef __KERNEL__

#define __user

#endif


/*! \enum aim_memory_type
 * Supported memory types the AIM PCI driver can read from or write to
 */
enum aim_memory_type
{
    AIM_MEMTYPE_GLOBAL = 0,         /*!< Global Memory */
    AIM_MEMTYPE_SHARED,             /*!< Shared Memory */
    AIM_MEMTYPE_LOCAL,              /*!< Local Memory */
    AIM_MEMTYPE_IO,                 /*!< IO Memory */
    AIM_MEMTYPE_GLOBAL_UNCACHED,    /*!< Global Memory not using any caches */
    AIM_MEMTYPE_GLOBAL_EXTENSION,   /*!< Global Extension Memory */
    AIM_MEMTYPE_MAX                 /*!< Signifies last entry */
};




/*! \struct aim_data_transaction_user
 * This structure holds the parameters for a read or write \n
 * system call to the PCI driver
 */
struct aim_data_transaction_user
{
    enum aim_memory_type memType;  /*!< memory type to read from / write to */
    loff_t offset;                 /*!< offset in bytes to read from write/to */
    size_t size;                   /*!< size of objects to read/write in bytes */
    size_t numObjects;             /*!< number of objects to read/write */
    bool useDMA;                   /*!< signifies if DMA engine of hardware shall be used for transaction */
    void __user *dataBuffer;       /*!< buffer in user space to read to / write from */
};




#endif /* AIM_RW_INTERFACE_H_ */
