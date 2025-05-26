/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_debug.h
 *
 *  This header file contains generic declarations
 *  used for debugging the AIM PCI driver
 *
 */

#ifndef AIM_DEBUG_H_
#define AIM_DEBUG_H_

#include <linux/device.h>




#define aim_info(fmt, ...) \
    printk(KERN_INFO KBUILD_MODNAME " [INFO]: " fmt, ##__VA_ARGS__)

#define aim_error(fmt, ...) \
    printk(KERN_ERR KBUILD_MODNAME " [ERROR]: %s: " fmt, __func__ , ##__VA_ARGS__)

#define aim_warning(fmt, ...) \
    printk(KERN_WARNING KBUILD_MODNAME " [WARNING]: %s: " fmt, __func__ , ##__VA_ARGS__)


#ifdef AIM_DEBUG
    #define aim_debug(fmt, ...) \
        printk(KERN_DEBUG KBUILD_MODNAME " [DEBUG]: %s: " fmt, __func__ , ##__VA_ARGS__)
#else
    #define aim_debug(fmt, ...)
#endif


#define aim_dev_info(aimDevice, fmt, ...) \
    dev_info(aim_to_dev(aimDevice), fmt, ##__VA_ARGS__)

#define aim_dev_error(aimDevice, fmt, ...) \
    dev_err(aim_to_dev(aimDevice), "[ERROR]: %s: " fmt , __func__, ##__VA_ARGS__)

#define aim_dev_warning(aimDevice, fmt, ...) \
    dev_warn(aim_to_dev(aimDevice), "[WARNING]: %s: " fmt , __func__, ##__VA_ARGS__)

#ifdef AIM_DEBUG
    #define aim_dev_debug(aimDevice, fmt, ...) \
        dev_printk(KERN_DEBUG, aim_to_dev(aimDevice), "[DEBUG]: %s: " fmt, __func__, ##__VA_ARGS__)
#else
    #define aim_dev_debug(aimDevice, fmt, ...)
#endif

#endif /* AIM_DEBUG_H_ */
