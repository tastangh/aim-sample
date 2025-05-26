/* SPDX-FileCopyrightText: 2014-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_com_channel.c
 *
 *  This file contains definitions for a new, interrupt based
 *  host to target communication channels over PCI bus
 *
 */
#include <linux/types.h>
#include "aim_pci_com_channel.h"
#include "aim_debug.h"
#include "aim_zynqmp_device.h"
#include "aim_pci_mem.h"


/*! \def AIM_PCI_COM_CHANNEL_TIMEOUT_MS
 *
 * Timeout for PCI communication channels in milliseconds
 */
#define AIM_PCI_COM_CHANNEL_TIMEOUT_MS 10000


int aim_pci_com_channel_hw_setup(struct aim_pci_com_channel* channel, struct aim_pci_com_channel_data* channelData,
                                    struct aim_pci_device* aimDevice)
{
    __u8 resourceID = 0;
    resource_size_t resourceSize;
    int ret = 0;

    BUG_ON(!channel || !channelData || !aimDevice);

    if(!aimDevice->pciDevice)
    {
        aim_dev_error(aimDevice, "No PCI device available");
        return -EFAULT;
    }

    if(!aimDevice->ioMemory)
    {
        aim_dev_error(aimDevice, "No IO memory for device available");
        return -EFAULT;
    }

    aim_dev_debug(aimDevice, "HW setup of target com channel %u on bar %u", channel->id, channelData->resourceID);
    aim_dev_debug(aimDevice, "Command offset is %u size %zu", channelData->commandOffset, channelData->commandSize);
    aim_dev_debug(aimDevice, "Response offset is %u size %zu", channelData->responseOffset, channelData->responseSize);

    resourceID = channelData->resourceID;
    resourceSize = aimDevice->pciDevice->resource[resourceID].end - aimDevice->pciDevice->resource[resourceID].start + 1;

    if((channelData->commandOffset + channelData->commandSize) > resourceSize
          || channelData->responseOffset + channelData->responseSize > resourceSize)
    {
        aim_dev_error(aimDevice, "Target communication channel %u data areas exceed resource size", channel->id);
        return -EINVAL;
    }

    if(channelData->signalRegisterOffset + sizeof(__u32) > aimDevice->ioMemorySize)
    {
        aim_dev_error(aimDevice, "Event signal register of target com channel %u is out of range", channel->id);
        return -EINVAL;
    }

    mutex_lock(&channel->lock);
    do
    {
        channel->commandBase = ioremap(aimDevice->pciDevice->resource[resourceID].start + channelData->commandOffset, channelData->commandSize);
        if(!channel->commandBase)
        {
            aim_dev_error(aimDevice, "Failed to map command data area for target com channel %u", channel->id);
            ret = -EINVAL;
            break;
        }

        channel->commandSize = channelData->commandSize;

        channel->responseBase = ioremap(aimDevice->pciDevice->resource[resourceID].start + channelData->responseOffset, channelData->responseSize);
        if(!channel->responseBase)
        {
            aim_dev_error(aimDevice, "Failed to map response data area for target com channel %u", channel->id);
            ret = -EINVAL;
            break;
        }

        channel->responseSize = channelData->responseSize;

        if( channel->aimDevice->deviceData->platform ==  AI_DEVICE_ZYNQMP_ASP )
            channel->signalRegister = NULL;
        else
            channel->signalRegister = aimDevice->ioMemory + channelData->signalRegisterOffset;

        aim_dev_debug(aimDevice, "Target com channel %u set up with command 0x%p response 0x%p and signal register 0x%p", channel->id,
                channel->commandBase, channel->responseBase, channel->signalRegister);

    }while(0);

    /* Set up failed, so unmap all bars */
    if(ret)
    {
        if(channel->commandBase)
        {
            iounmap(channel->commandBase);
            channel->commandBase = NULL;
            channel->commandSize = 0;
        }

        if(channel->responseBase)
        {
            iounmap(channel->responseBase);
            channel->responseBase = NULL;
            channel->responseSize = 0;
        }

        channel->signalRegister = NULL;
    }

    mutex_unlock(&channel->lock);

    return ret;
}


void aim_pci_com_channel_hw_release(struct aim_pci_com_channel* channel)
{
    BUG_ON(!channel);

    mutex_lock(&channel->lock);

    if(channel->commandBase)
    {
        iounmap(channel->commandBase);
        channel->commandBase = NULL;
    }

    if(channel->responseBase)
    {
        iounmap(channel->responseBase);
        channel->responseBase = NULL;
    }

    mutex_unlock(&channel->lock);
}


int aim_pci_com_channel_issue_cmd( struct aim_pci_com_channel* channel, void* command, size_t commandSize,
                                   void* response, size_t responseSize)
{
    int ret = 0;
    unsigned long timeLeft = 0;
    size_t boardResponseSize;

    BUG_ON(!channel || (!command && commandSize));

    mutex_lock(&channel->lock);

    do
    {
        if(commandSize)
        {
            if(!channel->commandBase)
            {
                aim_dev_error(channel->aimDevice, "Communication channel has no valid command area");
                ret = -EIO;
                break;
            }

            if(commandSize > channel->commandSize)
            {
                aim_dev_error(channel->aimDevice, "command size exceeds command area size of channel %u", channel->id);
                ret = -EMSGSIZE;
                break;
            }

            memcpy_toio(channel->commandBase, command, commandSize);
        }

        reinit_completion(&channel->commandCompletion);

        if( channel->aimDevice->deviceData->platform ==  AI_DEVICE_ZYNQMP_ASP )
        {
#ifdef AIM_ZYNQMP
            aim_zynqmp_create_interrupt_on_device(channel->aimDevice, (1 << channel->id) );
#else
            aim_dev_error(channel->aimDevice, "ZynqMP platform support not enabled");
            ret = -EPROTONOSUPPORT;
            break;
#endif
        }
        else
        {
            iowrite32((1 << channel->id), channel->signalRegister);
        }

        timeLeft = wait_for_completion_timeout(&channel->commandCompletion,msecs_to_jiffies(AIM_PCI_COM_CHANNEL_TIMEOUT_MS));
        if(timeLeft == 0)
        {
            aim_dev_error(channel->aimDevice, "Command on com channel %u timed out", channel->id);
            ret = -ETIME;
            break;
        }


        if(!response || !channel->responseBase)
        {
            aim_dev_error(channel->aimDevice, "Communication channel has no valid command area");
            ret = -EIO;
            break;
        }

        //Board-type dependent read to get actual acknowledge response size, if possible
        if(AIM_PCI_UNKNOWN_ACK_SIZE == (boardResponseSize = aim_pci_read_ack_size(channel, response)) )
        {
            boardResponseSize = responseSize;
        }

        if(boardResponseSize > channel->responseSize)
        {
            aim_dev_error(channel->aimDevice, "Response size exceeds response area size of channel %u", channel->id);
            ret = -EMSGSIZE;
            break;
        }

        if((responseSize != AIM_PCI_UNKNOWN_ACK_SIZE) && (boardResponseSize > responseSize))
        {
            aim_dev_error(channel->aimDevice, "Response size from board exceeds expected response size");
            ret = -EMSGSIZE;
            break;
        }

        /* Re-read to get the remaining part of the ACK */
        memcpy_fromio(response, channel->responseBase, boardResponseSize);

    } while(0);

    mutex_unlock(&channel->lock);

    return ret;
}
