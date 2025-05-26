/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file aim_usb_rw_interface.h
 *
 *  This header file contains the user space interface declarations
 *  for the read/write system call on AIM USB drivers
 *
 */

#ifndef AIM_USB_RW_INTERFACE_H_
#define AIM_USB_RW_INTERFACE_H_


#include <linux/types.h>
#include <Ai_def.h>


#ifndef __KERNEL__

#define __user

#endif







/*! \struct aim_data_transaction_user
 * This structure holds the parameters for a read or write \n
 * system call to the USB driver
 */
struct aim_data_transaction_user
{
    TY_E_MEM_TYPE memType;         /*!< memory type to read from / write to */
    loff_t offset;                 /*!< offset in bytes to read from write/to */
    size_t size;                   /*!< size of objects to read/write in bytes */
    size_t numObjects;             /*!< number of objects to read/write */
    bool useDMA;                   /*!< signifies if DMA engine of hardware shall be used for transaction */
    void __user *dataBuffer;       /*!< buffer in user space to read to / write from */
};




#endif /* AIM_USB_RW_INTERFACE_H_ */
