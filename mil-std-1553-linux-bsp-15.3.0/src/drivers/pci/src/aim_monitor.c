/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_monitor.c
 *
 *  This file contains definitions for monitor specific functions
 *
 */


#include "type_def.h"
#include "prototypes.h"
#include <linux/firmware.h>
#include "aim_pci_device.h"




/*! \brief Firmware blob retrieval code used by PMC monitor
 *
 * This function is called by the PMC Monitor when it requests to load
 * a firmware file in SRE format.
 * @param driverData pointer to \ref struct aim_pci_device of the device to load firmware for
 * @param fileName Name of the firmware SRE file to load
 * @param buffer the buffer to put the firmware blob into
 * @param actualSize actual size of the firmware blob in bytes
 * @param bufferSize size of the buffer for the firmware blob in bytes
 */
unsigned long ul_LoadCode( void* driverData, char* fileName,
                           unsigned char* buffer, unsigned long* actualSize,
                           unsigned long bufferSize )
{
    const struct firmware* firmwareBlob = NULL;
    struct aim_pci_device* aimDevice = NULL;
    int ret;

    BUG_ON(!driverData || !buffer || !actualSize );

    aimDevice = (struct aim_pci_device*) driverData;

    aim_dev_info(aimDevice, "Loading firmware image %s\n", fileName);

    ret = request_firmware( &firmwareBlob, fileName, &aimDevice->pciDevice->dev);
    if(ret)
    {
        aim_dev_error(aimDevice, "Couldn't load firmware image %s, error is %i\n", fileName, ret );
        return -1;
    }

    do
    {
        if( firmwareBlob->size > bufferSize )
        {
            aim_dev_error(aimDevice, "%s is too big with %zu bytes, allowed are %li bytes\n", fileName,
                          firmwareBlob->size, bufferSize);
            ret = -1;
            break;
        }

        memcpy( buffer, firmwareBlob->data, firmwareBlob->size );
        *actualSize = firmwareBlob->size;

    }while(0);

    release_firmware( firmwareBlob );

    return 0;
}


/*! \def Memory copy routine used by PMC-Monitor
 *
 * Attention! This routine does not distinguish
 * between kernel and IO memory and therefore
 * does not use any ioread/write kernel functions
 * @param dst pointer to destination memory
 * @param src pointer to source memory
 * @param count number of bytes to copy
 * @return returns pointer to destination. Why?
 */
void* Api_memcpy( void * dst, const void * src,size_t count )
{
    size_t i = 0;
    void * ret = dst;

    for(i = count; i >= sizeof(__u32); i -= sizeof(__u32))
    {
        *((__u32*) dst) = *((__u32*) src);
        dst += sizeof(__u32);
        src += sizeof(__u32);
    }

    while(i)
    {
        *((__u8*) dst) = *((__u8*) src);
        dst++;
        src++;
        i--;
    }

    return ret;
}

