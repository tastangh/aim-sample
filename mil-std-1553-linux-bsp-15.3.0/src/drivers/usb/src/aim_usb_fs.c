/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_fs.c
 *
 */


#include <linux/kernel.h>
#include <linux/usb.h>
#include "aim_usb_fs.h"
#include "aim_usb.h"
#include "aim_usb_debug.h"
#include "aim_usb_interface.h"
#include "aim_usb_rw_interface.h"
#include "aim_usb_hw.h"
#include "aim_usb_ioctl_interface.h"
#include "generated/version.h"




/*! \brief This function handles the IOCTL for getting the driver version
 *
 * @param version_user user space before where driver version shall be copied to
 * @return 0 on success, errno code on failure
 */
static long ioctl_get_driver_version(struct aim_driver_version_user __user* version_user)
{
    struct aim_driver_version_user version;

    BUG_ON(!version_user);

    version.major = MAJOR_VERSION;
    version.minor = MINOR_VERSION;
    version.patch = PATCH_VERSION;
    version.build = 0;

    strncpy(version.full_version, VERSION_STRING_FULL, sizeof(version.full_version));

    if(copy_to_user(version_user, &version, sizeof(struct aim_driver_version_user)))
    {
        aim_error("Failed to copy version to user space");
        return -EFAULT;
    }

    return 0;
}


/*! \brief This function handles the IOCTL for querying board information
 *
 * @param intf the interface to query information of
 * @param info_user user space buffer where information will be copied to
 * @return 0 on success, errno code on failure
 */
static long ioctl_get_board_info(struct aim_usb_interface* intf,  TY_DRIVER_INFO __user* info_user)
{
    TY_DRIVER_INFO board_info = {0};

    BUG_ON(!intf || !info_user);

    board_info.uc_DeviceGroup = intf->platform;
    board_info.ul_SN = intf->serial;
    board_info.ul_BoardConfig = intf->board_config;
    board_info.ul_BoardType = intf->board_type;

    board_info.ul_OpenConnections = aim_usb_interface_active_users(intf);

    if(intf->has_asp)
    {
        board_info.ul_DriverFlags |= 0x0001;
    }

    if(copy_to_user(info_user, &board_info, sizeof(board_info)))
    {
        aim_usb_intf_err(intf, "Failed to copy board info to user space");
        return -EFAULT;
    }

    return 0;
}


/*! \brief This function handles the IOCTL for issuing target commands
 *
 * @param intf the interface to issue target command to
 * @param user_params user space buffer with command arguments
 */
static long ioctl_target_command(struct aim_usb_interface* intf, struct aim_target_command __user* user_arguments)
{
    struct aim_target_command arguments;
    long err;
    void* command_buffer;
    void* response_buffer;
    ssize_t response;

    BUG_ON(!intf || !user_arguments);

    aim_usb_intf_dbg(intf, "Target command ioctl");

    if(copy_from_user(&arguments, user_arguments, sizeof(arguments)))
    {
        aim_usb_intf_err(intf, "Failed to copy target command arguments from user space");
        return -EFAULT;
    }


    aim_usb_intf_dbg(intf, "Target command with command size: %zu and response size %zu", arguments.cmdSize,
                           arguments.expectedAckSize);

    err = 0;
    command_buffer = NULL;
    response_buffer = NULL;

    do
    {
        command_buffer = kmalloc(arguments.cmdSize, GFP_KERNEL);
        if(!command_buffer)
        {
            aim_usb_intf_err(intf, "Failed to allocate command buffer");
            err = -ENOMEM;
            break;
        }

        response_buffer = kmalloc(arguments.expectedAckSize, GFP_KERNEL);
        if(!response_buffer)
        {
            aim_usb_intf_err(intf, "Failed to allocate response buffer");
            err = -ENOMEM;
            break;
        }

        if(copy_from_user(command_buffer, arguments.command, arguments.cmdSize))
        {
            aim_usb_intf_err(intf, "Failed to copy target command from user space");
            err = -EFAULT;
            break;
        }

        if(!intf->ioctl_ops || !intf->ioctl_ops->target_command)
        {
            aim_usb_intf_err(intf, "No target command handler specified");
            err = -EOPNOTSUPP;
            break;
        }

        mutex_lock(&intf->io_mutex);

        response = intf->ioctl_ops->target_command(intf, command_buffer, arguments.cmdSize, response_buffer,
                                                   arguments.expectedAckSize);

        mutex_unlock(&intf->io_mutex);

        if(response < 0)
        {
            aim_usb_intf_err(intf, "Target command failure");
            err = response;
            break;
        }

        aim_usb_intf_dbg(intf, "Copying %zu bytes response to user space", response);

        if(copy_to_user(arguments.acknowledge, response_buffer, response))
        {
            aim_usb_intf_err(intf, "Failed to copy target response to user space");
            err = -EFAULT;
            break;
        }

    }while(0);

    if(command_buffer)
    {
        kfree(command_buffer);
    }

    if(response_buffer)
    {
        kfree(response_buffer);
    }

    return err;
}


/*! \brief Handles the IOCTL for generic USB communication channel commands
 *
 * @param intf the interface that holds the communication channel to issue commands on
 * @param user_arguments The ioctl arguments
 */
static long ioctl_usb_com_channel_cmd(struct aim_usb_interface* intf, struct aim_usb_com_chn_cmd __user* user_arguments)
{
    struct aim_usb_com_chn_cmd arguments;
    long err;
    void* command_buffer;
    void* response_buffer;
    struct aim_usb_com_channel* com_channel;
    ssize_t response_len;

    BUG_ON(!intf || !user_arguments);

    aim_usb_intf_dbg(intf, "USB channel command IOCTL");

    if(copy_from_user(&arguments, user_arguments, sizeof(struct aim_usb_com_chn_cmd)))
    {
        aim_usb_intf_err(intf, "Failed to copy USB channel command args from user space");
        return -EFAULT;
    }

    err = 0;
    command_buffer = NULL;
    response_buffer = NULL;
    do
    {
        com_channel = aim_usb_interface_get_com_channel(intf, arguments.id);
        if(IS_ERR(com_channel))
        {
            aim_usb_intf_err(intf, "Invalid communication channel");
            err = PTR_ERR(com_channel);
            break;
        }

        command_buffer = kmalloc(arguments.commandParams.cmdSize, GFP_KERNEL);
        if(!command_buffer)
        {
            aim_usb_intf_err(intf, "Failed to allocate USB command buffer");
            err = -ENOMEM;
            break;
        }

        if(copy_from_user(command_buffer, arguments.commandParams.command, arguments.commandParams.cmdSize))
        {
            aim_usb_intf_err(intf, "Failed to copy USB command from user space");
            err = -ENOMEM;
            break;
        }

        response_buffer = kmalloc(arguments.commandParams.expectedAckSize, GFP_KERNEL);
        if(!response_buffer)
        {
            aim_usb_intf_err(intf, "Failed to allocate USB command buffer");
            err = -ENOMEM;
            break;
        }

        response_len = aim_usb_hw_com_channel_cmd(com_channel, command_buffer, arguments.commandParams.cmdSize,
                                                  response_buffer, arguments.commandParams.expectedAckSize);
        if(response_len < 0)
        {
            aim_usb_intf_err(intf, "USB channel command failed");
            err = response_len;
            break;
        }

        if(copy_to_user(arguments.commandParams.acknowledge, response_buffer, response_len))
        {
            aim_usb_intf_err(intf, "Failed to copy USB channel command response to user space");
            err = -EFAULT;
            break;
        }

    }while(0);

    if(command_buffer)
    {
        kfree(command_buffer);
    }

    if(response_buffer)
    {
        kfree(response_buffer);
    }

    return err;
}


/*! \brief This function processes the get mem size IOCTL
 *
 * It will just return the size of requested memory type if it is available on this platform.
 * Will return -EINVAL if memory type is not available on the device
 * @param device device to request memory size of
 * @param user_params contains the requested memory type and the returned size
 */
static long ioctl_get_mem_size( struct aim_usb_interface* intf, struct aim_get_mem_size __user*  user_params )
{
    struct aim_get_mem_size mem_size;

    if(copy_from_user(&mem_size, user_params, sizeof(mem_size)))
    {
        aim_usb_intf_err(intf, "Failed to copy ioctl get mem size params from user space");
        return -EFAULT;
    }

    switch(mem_size.type)
    {
        case AI_MEMTYPE_GLOBAL:
        case AI_MEMTYPE_GLOBAL_DIRECT:
            mem_size.size = intf->global_memory.size;
            break;

        case AI_MEMTYPE_SHARED:
            mem_size.size = intf->shared_memory.size;
            break;

        case AI_MEMTYPE_IO:
            mem_size.size = intf->io_memory.size;
            break;

        default:
            return -EINVAL;
    }

    aim_usb_intf_dbg(intf, "Size of memory type %d is %lld", mem_size.type, mem_size.size);

    if(copy_to_user(user_params, &mem_size, sizeof(mem_size)))
    {
        aim_usb_intf_err(intf, "Failed to copy ioctl get mem size response to user space");
        return -EFAULT;
    }

    return 0;
}


int aim_usb_interface_open(struct inode* inode, struct file* file)
{
    struct usb_interface* usb_intf = NULL;
    struct aim_usb_interface* aim_intf = NULL;
    int err;

    usb_intf = usb_find_interface(&aim_usb_driver, iminor(inode));
    if (!usb_intf)
    {
        return -ENODEV;
    }

    aim_intf = usb_get_intfdata(usb_intf);
    if(!aim_intf)
    {
        return -ENODEV;
    }

    mutex_lock(&aim_intf->io_mutex);
    aim_usb_intf_dbg(aim_intf, "Interface opened");

    /* Grab the interface context, so it can't be deleted by another thread */
    aim_ref_get(&aim_intf->ref_count);

    err = 0;
    do
    {
        /* Prevent interface from being auto-suspended */
        err = usb_autopm_get_interface(usb_intf);
        if(err)
        {
            aim_usb_intf_err(aim_intf, "Failed to get interface from power management");
            mutex_unlock(&aim_intf->io_mutex);
            return err;
        }

        if(aim_usb_interface_active_users(aim_intf) == 1)
        {
            aim_usb_intf_dbg(aim_intf, "Starting interface");
            err = aim_usb_interface_start(aim_intf);
            if(err)
            {
                break;
            }
        }

        /* Store the interface context in the file pointer's private data field */
        file->private_data = aim_intf;

    }while(0);

    if(err)
    {
        usb_autopm_put_interface(usb_intf);
        aim_ref_put(&aim_intf->ref_count, aim_usb_interface_free);
    }

    mutex_unlock(&aim_intf->io_mutex);
    return err;
}


int aim_usb_interface_release(struct inode* inode, struct file* file)
{
    struct aim_usb_interface* aim_intf = file->private_data;

    BUG_ON(!aim_intf);

    mutex_lock(&aim_intf->io_mutex);
    aim_usb_intf_dbg(aim_intf, "Releasing interface");

    if(aim_usb_interface_active_users(aim_intf) == 1)
    {
        aim_usb_interface_stop(aim_intf);
    }

    usb_autopm_put_interface(aim_intf->interface);

    aim_ref_put(&aim_intf->ref_count, aim_usb_interface_free);

    mutex_unlock(&aim_intf->io_mutex);

    return 0;
}


ssize_t aim_usb_interface_read(struct file* file , char __user * buffer, size_t len, loff_t* offset)
{
    struct aim_usb_interface* intf;
    struct aim_data_transaction_user params;
    void* usb_buffer;
    int err;
    size_t read_size;

    intf = file->private_data;

    BUG_ON(!intf);

    if(len != sizeof(params))
    {
        aim_usb_intf_err(intf, "Invalid size of read params");
        return -EINVAL;
    }

    if(copy_from_user(&params, buffer, sizeof(struct aim_data_transaction_user)))
    {
        aim_usb_intf_err(intf, "Failed to copy read parameters from user space");
        return -EFAULT;
    }

    aim_usb_intf_dbg(intf, "Reading from memtype: %d, offset: %llu, size: %zu, num_objects: %zu DMA: %s",
                           params.memType, params.offset, params.size, params.numObjects, params.useDMA ? "Yes" : "No");

    read_size = params.size * params.numObjects;

    usb_buffer = kmalloc(read_size, GFP_KERNEL);
    if(!usb_buffer)
    {
        aim_usb_intf_err(intf, "Failed to allocate read buffer");
        return -ENOMEM;
    }

    err = 0;
    do
    {
        mutex_lock(&intf->io_mutex);

        err = aim_usb_hw_read(intf, usb_buffer, read_size, params.memType, params.offset);

        mutex_unlock(&intf->io_mutex);

        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read data from hardware");
            break;
        }

        if(copy_to_user(params.dataBuffer, usb_buffer, read_size))
        {
            aim_usb_intf_err(intf, "Failed to copy data to user space");
            err = -EFAULT;
            break;
        }
    }while(0);

    kfree(usb_buffer);

    return err ? err : read_size;
}


ssize_t aim_usb_interface_write(struct file* file, const char __user * buffer, size_t len, loff_t* offset)
{
    struct aim_usb_interface* intf;
    struct aim_data_transaction_user params;
    void* usb_buffer;
    int err;
    size_t write_size;

    intf = file->private_data;

    BUG_ON(!intf);

    if(len != sizeof(params))
    {
        aim_usb_intf_err(intf, "Invalid size of write params");
        return -EINVAL;
    }

    if(copy_from_user(&params, buffer, sizeof(struct aim_data_transaction_user)))
    {
        aim_usb_intf_err(intf, "Failed to copy write parameters from user space");
        return -EFAULT;
    }

    aim_usb_intf_dbg(intf, "Writing to memtype: %d, offset: %llu, size: %zu, num_objects: %zu DMA: %s",
                           params.memType, params.offset, params.size, params.numObjects, params.useDMA ? "Yes" : "No");

    write_size = params.size * params.numObjects;

    usb_buffer = kmalloc(write_size, GFP_KERNEL);
    if(!usb_buffer)
    {
        aim_usb_intf_err(intf, "Failed to allocate write buffer");
        return -ENOMEM;
    }

    err = 0;
    do
    {
        if(copy_from_user(usb_buffer, params.dataBuffer, write_size))
        {
            aim_usb_intf_err(intf, "Failed to copy data from user space");
            err = -EFAULT;
            break;
        }

        mutex_lock(&intf->io_mutex);

        err = aim_usb_hw_write(intf, usb_buffer, write_size, params.memType, params.offset);

        mutex_unlock(&intf->io_mutex);

        if(err)
        {
            aim_usb_intf_err(intf, "Failed to write data to hardware");
            break;
        }
    }while(0);

    kfree(usb_buffer);

    return err ? err : write_size;
}


long aim_usb_interface_ioctl(struct file* file, unsigned int command, unsigned long params)
{
    struct aim_usb_interface* intf;
    long err;

    intf = file->private_data;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "IOCTL with command: %u", command);

    if (_IOC_TYPE(command) != AIM_IOCTL_BASE)
    {
        aim_usb_intf_err(intf, "Invalid IOCTL base for device");
        return -EINVAL;
    }

    switch(command)
    {
        case AIM_IOCTL_GET_DRV_VERS:
            if(_IOC_SIZE(command) != sizeof(struct aim_driver_version_user))
            {
                aim_usb_intf_err(intf, "Invalid size of get version parameters\n");
                return -EINVAL;
            }

            err = ioctl_get_driver_version((struct aim_driver_version_user __user*) params);
            break;

        case AIM_IOCTL_GENERIC_BOARD_INFO:
            if(_IOC_SIZE(command) != sizeof(TY_DRIVER_INFO))
            {
                aim_usb_intf_err(intf, "Invalid size of board info parameters\n");
                return -EINVAL;
            }

            err = ioctl_get_board_info(intf, (TY_DRIVER_INFO __user *) params);
            break;

        case AIM_IOCTL_TARGET_COMMAND:
            if(_IOC_SIZE(command) != sizeof(struct aim_target_command))
            {
                aim_usb_intf_err(intf, "Invalid size of target command parameters\n");
                return -EINVAL;
            }

            err = ioctl_target_command(intf, (struct aim_target_command __user*) params);
            break;

        case AIM_IOCTL_USB_COM_CHN_CMD:
            if(_IOC_SIZE(command) != sizeof(struct aim_usb_com_chn_cmd))
            {
                aim_usb_intf_err(intf, "Invalid size of USB channel command parameters\n");
                return -EINVAL;
            }

            err = ioctl_usb_com_channel_cmd(intf, (struct aim_usb_com_chn_cmd __user*) params);
            break;

        case AIM_IOCTL_GET_MEM_SIZE:
            if(_IOC_SIZE(command) != sizeof(struct aim_get_mem_size))
            {
                aim_usb_intf_err(intf, "Invalid parameter size for get mem size IOCTL\n");
                return -EINVAL;
            }

            err = ioctl_get_mem_size(intf, (struct aim_get_mem_size __user*) params);
            break;

        default:
            aim_usb_intf_err(intf, "Unknown IOCTL command");
            err = -EINVAL;
            break;
    }

    return err;
}


