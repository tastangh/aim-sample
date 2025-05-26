/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb.h
 *
 */

#ifndef AIM_USB_H_
#define AIM_USB_H_


#include <linux/usb.h>
#include <linux/version.h>


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)


#include <linux/err.h>


static inline long IS_ERR_OR_NULL(const void *ptr)
{
    return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}


#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)

#include <linux/string.h>
#include <linux/vmalloc.h>

    static inline void* vzalloc(unsigned long size)
    {
        void* mem = vmalloc(size);
        if(!mem)
        {
            return NULL;
        }

        memset(mem,0, size);

        return mem;
    }

#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(3,2,0)


static inline int usb_endpoint_maxp(const struct usb_endpoint_descriptor *epd)
{
    return __le16_to_cpu(epd->wMaxPacketSize);
}


#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)

#include <linux/types.h>

typedef u32 phys_addr_t;


#endif




/*! aim_usb_device_table
 * Table with USB devices the driver module supports.
 * Must be defined by protocol specific driver implementation
 */
extern const struct usb_device_id aim_usb_device_table[];



/*!< global variable that holds basic information of the driver */
extern struct usb_driver aim_usb_driver;



#endif /* AIM_USB_H_ */
