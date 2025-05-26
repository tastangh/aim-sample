/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_kernel_mem.h
 *
 *  This header file contains declarations
 *  for handling kernel memory allocation
 *
 */

#ifndef AIM_KERNEL_MEM_H_
#define AIM_KERNEL_MEM_H_


#include <linux/version.h>
#include <linux/vmalloc.h>


/* The vzalloc implementation was introduced in kernel 2.6.37 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)

#include <linux/string.h>

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


#endif /* AIM_KERNEL_MEM_H_ */
