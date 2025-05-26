/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_mymon.c
 *
 *  This file contains definitions used for
 *  driver specific MyMon handling
 *
 */


#include <linux/delay.h>
#include "aim_pci_mymon.h"
#include "aim_pci_device.h"
#include "aim_pci_mem.h"




AiBoolean aim_pci_mymon_io_read( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords)
{
    struct aim_pci_device* device = NULL;

    BUG_ON(!context || !context->pxUserData || !context->pxUserData->device);

    device = context->pxUserData->device;

    while(num_lwords-- > 0)
    {
        *data = aim_pci_io_read32(device, byteOffset);
        byteOffset += sizeof(AiUInt32);
        data++;
    }

    return AiTrue;
}


AiBoolean aim_pci_mymon_io_write( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords)
{
    struct aim_pci_device* device = NULL;

    BUG_ON(!context || !context->pxUserData || !context->pxUserData->device);

    device = context->pxUserData->device;

    while(num_lwords-- > 0)
    {
        aim_pci_io_write32(device, byteOffset, *data);
        byteOffset += sizeof(AiUInt32);
        data++;
    }

    return AiTrue;
}


AiBoolean aim_pci_mymon_glob_ext_read( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords )
{
    struct aim_pci_device* device = NULL;

    BUG_ON(!context || !context->pxUserData || !context->pxUserData->device);

    device = context->pxUserData->device;

    BUG_ON(!device->globalExtMemory);

    while(num_lwords-- > 0)
    {
        *data = ioread32(device->globalExtMemory + byteOffset);
        byteOffset += sizeof(AiUInt32);
        data++;
    }

    return AiTrue;
}


AiBoolean aim_pci_mymon_glob_ext_write( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords)
{
    struct aim_pci_device* device = NULL;

    BUG_ON(!context || !context->pxUserData || !context->pxUserData->device);

    device = context->pxUserData->device;

    BUG_ON(!device->globalExtMemory);

    while(num_lwords-- > 0)
    {
        iowrite32(*data, device->globalExtMemory + byteOffset);
        byteOffset += sizeof(AiUInt32);
        data++;
    }

    return AiTrue;
}


AiBoolean aim_pci_mymon_wait( struct TyMyMonContext * context, AiUInt32 microseconds )
{
    udelay(microseconds);

    return AiTrue;
}


