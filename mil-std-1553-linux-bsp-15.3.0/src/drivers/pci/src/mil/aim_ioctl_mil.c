/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ioctl_mil.c
 *
 *  This file contains 1553 specific definitions
 *  for implementing the driver's IOCTLs
 *
 */


#include "aim_ioctl_mil.h"
#include "aim_pci_device_mil.h"
#include "aim_tsw_mil.h"
#include "api_prot.h"
#include "aim_pci_mem.h"



/*! \brief Determine acknowledge data size
 *
 * Analyzes the acknowledge stream in the given acknowledge stream and
 * determines the total size of the stream.
 * @param ack_buffer acknowledge data to analyze
 * @return number of acknowledge size in bytes on success, negative error otherwise
 */
static ssize_t get_ack_size(__u8* ack_buffer)
{
    TY_MIL_COM_ACK* new_ack = (TY_MIL_COM_ACK*) ack_buffer;
    ssize_t ack_size = -1;

    if(new_ack->ulMagic == MIL_COM_MAGIC)
    {
        return new_ack->ulSize;
    }

    switch( ack_buffer[1])
    {
    case API_TG_ACK:
        ack_size = 8;
        break;

    case API_TG_ACK2:
        ack_size = (ack_buffer[2] << 8) + ack_buffer[3];
        break;

    case API_TG_NAK:
        ack_size = 8;
        break;

    default:
        ack_size = -1;
        break;
    }

    return ack_size;
}


/*! \brief Process target command in driver
 *
 * @param device the device to process command on
 * @param commandParams command parameters
 */
static long process_target_cmd(struct aim_pci_mil_device* device, const struct aim_target_command* commandParams)
{
    long ret = 0;
    __u8* cmd_buffer = NULL;
    __u8* ack_buffer = NULL;
    ssize_t ack_size = 0;

    do
    {
        cmd_buffer = aim_mem_cache_alloc();
        if(!cmd_buffer)
        {
            ret = -ENOMEM;
            break;
        }

        ack_buffer = aim_mem_cache_alloc();
        if(!ack_buffer)
        {
            ret = -ENOMEM;
            break;
        }

        if(copy_from_user(cmd_buffer, commandParams->command, commandParams->cmdSize))
        {
            aim_dev_error(mil_to_aim(device), "Failed to copy command to command buffer");
            ret = -EFAULT;
            break;
        }

        /* Call target software library command processing function */
        api_opr(device->tswData, cmd_buffer, ack_buffer);

        ack_size = get_ack_size(ack_buffer);

        if(ack_size < 0)
        {
            aim_dev_error(mil_to_aim(device), "Couldn't determine size of acknowledge data");
            ret = -EIO;
            break;
        }

        if(ack_size > commandParams->expectedAckSize)
        {
            aim_dev_error(mil_to_aim(device), "Ack size exceeds acknowledge buffer");
            ret = -ENOMEM;
            break;
        }

        if(copy_to_user(commandParams->acknowledge, ack_buffer, ack_size))
        {
            aim_dev_error(mil_to_aim(device), "Failed to copy command to command buffer");
            ret = -EFAULT;
            break;
        }

    }while(0);

    if(cmd_buffer)
    {
    	aim_mem_cache_free(cmd_buffer);
    }

    if(ack_buffer)
    {
    	aim_mem_cache_free(ack_buffer);
    }

    return ret;
}



/*! \brief Re-read the board type from shared memory
 * This is necessary to handle the EFA-EFEx switch case.
 * The board gets a new device type without notifying us.
 *
 * @param device the device to process command on
 */
static int aim_pci_device_init_mil_patch_board_type(struct aim_pci_device* device)
{
    int ret = 0;
    TY_HW_INFORMATION __iomem *board_info = device->sharedMemory;

    device->properties.type = ioread32(&board_info->ul_BoardType);

    return ret;
}


/*! \brief Process target command on ASP of device
 *
 * @param device the device to process command on
 * @param commandParams command parameters
 */
static long process_target_cmd_asp(struct aim_pci_mil_device* device, const struct aim_target_command* commandParams)
{
    long ret = 0;
    __u8* cmd_buffer = NULL;
    __u8* ack_buffer = NULL;
    ssize_t ack_size = 0;

    do
    {
        cmd_buffer = kmalloc(commandParams->cmdSize, GFP_KERNEL);
        if(!cmd_buffer)
        {
            ret = -ENOMEM;
            break;
        }

        ack_buffer = kmalloc(commandParams->expectedAckSize, GFP_KERNEL);
        if(!ack_buffer)
        {
            ret = -ENOMEM;
            break;
        }

        if(copy_from_user(cmd_buffer, commandParams->command, commandParams->cmdSize))
        {
            aim_dev_error(mil_to_aim(device), "Failed to copy command to command buffer");
            ret = -EFAULT;
            break;
        }

        if( device->aimDevice.deviceData->platform == AI_DEVICE_ZYNQMP_ASP )
            ack_size = aim_exec_asp_command_mil_irq(device, cmd_buffer,  commandParams->cmdSize, ack_buffer, commandParams->expectedAckSize);
        else
            ack_size = aim_exec_asp_command_mil(device, cmd_buffer,  commandParams->cmdSize, ack_buffer, commandParams->expectedAckSize);

        if(ack_size < 0)
        {
            ret = ack_size;
            break;
        }

        if(copy_to_user(commandParams->acknowledge, ack_buffer, ack_size))
        {
            aim_dev_error(mil_to_aim(device), "Failed to copy command to command buffer");
            ret = -EFAULT;
            break;
        }

    }while(0);

    if(cmd_buffer)
    {
        kfree(cmd_buffer);
    }

    if(ack_buffer)
    {
        kfree(ack_buffer);
    }

    return ret;
}


long aim_handle_target_cmd_mil(struct aim_pci_device* aimDevice, struct aim_target_command __user* parameters)
{
    struct aim_target_command commandParams = {};
    long ret = 0;

    BUG_ON(!aimDevice || !parameters);

    /* Copy the IO parameters into kernel space. We need */
    /*all information herein directly accessible.       */
    if(copy_from_user(&commandParams, parameters, sizeof(commandParams)))
    {
        aim_dev_error(aimDevice, "Reading of IOCTL parameters from userspace failed\n");
        return -EFAULT;
    }

    mutex_lock(&aimDevice->targetCmdLock);
    do
    {
        if(aimDevice->hasAsp)
        {
            ret = process_target_cmd_asp(aim_dev_to_mil(aimDevice), &commandParams);

            aim_pci_device_init_mil_patch_board_type(aimDevice);

            break;
        }
        else
        {
            ret = process_target_cmd(aim_dev_to_mil(aimDevice), &commandParams);
            break;
        }
    }while(0);

    mutex_unlock(&aimDevice->targetCmdLock);

    return ret;
}

long aim_pci_read_ack_size(const struct aim_pci_com_channel * channel, void * response)
{
    TY_MIL_COM_ACK * milAckHeader;
    milAckHeader = (TY_MIL_COM_ACK *)response;

    memcpy_fromio(response, channel->responseBase, sizeof(TY_MIL_COM_ACK));

    if( milAckHeader->ulMagic != MIL_COM_MAGIC)
        /* Handle commands with old host to target communication */
        return AIM_PCI_UNKNOWN_ACK_SIZE;
    else
        /* handle commands with new host to target communication */
        return milAckHeader->ulSize;
}

