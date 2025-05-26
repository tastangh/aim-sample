/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_debug.h
 *
 *  This header file contains generic declarations
 *  used for debugging the AIM USB driver
 *
 */

#ifndef AIM_USB_DEBUG_H_
#define AIM_USB_DEBUG_H_




#define aim_info(fmt, ...) \
    printk(KERN_INFO KBUILD_MODNAME " [INFO]: " fmt, ##__VA_ARGS__)

#define aim_error(fmt, ...) \
    printk(KERN_ERR KBUILD_MODNAME " [ERROR]: %s: " fmt, __func__ , ##__VA_ARGS__)

#define aim_warning(fmt, ...) \
    printk(KERN_WARNING KBUILD_MODNAME " [WARNING]: %s: " fmt, __func__ , ##__VA_ARGS__)


#ifdef DEBUG
    #define aim_debug(fmt, ...) \
        printk(KERN_DEBUG KBUILD_MODNAME " [DEBUG]: %s: " fmt, __func__ , ##__VA_ARGS__)
#else
    #define aim_debug(fmt, ...)
#endif


#define aim_usb_intf_info(aim_usb_intf, fmt, ...) \
    dev_info((aim_usb_intf)->dev, fmt, ##__VA_ARGS__)

#define aim_usb_intf_dbg(aim_usb_intf, fmt, ...) \
    dev_dbg((aim_usb_intf)->dev, fmt, ##__VA_ARGS__)

#define aim_usb_intf_err(aim_usb_intf, fmt, ...) \
    dev_err((aim_usb_intf)->dev, fmt, ##__VA_ARGS__)

#define aim_usb_intf_warn(aim_usb_intf, fmt, ...) \
    dev_warn((aim_usb_intf)->dev, fmt, ##__VA_ARGS__)




#endif /* AIM_USB_DEBUG_H_ */
