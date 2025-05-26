/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * aim_pci_ioctl.c
 *
 */


#include "aim_ioctl.h"
#include "aim_pci_module.h"
#include "aim_pci_device.h"
#include "aim_ioctl_interface.h"
#include <linux/uaccess.h>
#include <linux/scatterlist.h>
#include "aim_debug.h"
#include "aim_dma.h"
#include "aim_pci_module.h"
#include "aim_aio.h"
#include "aim_aye_ds_device.h"


/*! \struct dmaBufferPage
 *
 * This structure defines a page of a user space DMA buffer
 * in serialized, raw format as used in IOCTLs.
 */
struct dmaBufferPage
{
    __u32 highAddr; /*!< Upper 32-bit of DMA buffer */
    __u32 lowAddr;  /*!< Lower 32-Bit of DMA buffer */
    __u32 size;     /*!< Size of DMA buffer in bytes */
};


/*! \struct dmaBufferSerialized
 *
 * This structure defines a user space DMA buffer
 * serialized in a number of buffer pages, so it can be
 * written to an on-board command array.
 */
struct dmaBufferSerialized
{
    __u32 numBufferPages;               /*!< number of \ref struct dmaBufferPage entries that follow. */
    struct dmaBufferPage pageArray[];   /*!< array of DMA buffer pages */
};





/*! \brief Processes a target command IOCTL
 *
 * This function will process a target command IOCTL
 * by calling the protocol specific handler function.
 * \param aimDevice the device the IOCTL will be processed for
 * \param aim_target_command the target command parameters.
 */
static long aim_pci_target_command(struct aim_pci_device* aimDevice, struct aim_target_command __user* userCommandParams)
{
    struct aim_pci_ioctl_ops* ioctlHandlers = NULL;

    BUG_ON(!aimDevice || !aimDevice->deviceData || !aimDevice->deviceData->ioctlOps || !userCommandParams);

    ioctlHandlers = aimDevice->deviceData->ioctlOps;

    if(!ioctlHandlers->target_command)
    {
        aim_dev_error(aimDevice, "No protocol specific target command handler installed\n");
        return -EINVAL;
    }

    return ioctlHandlers->target_command(aimDevice, userCommandParams);
}

static long do_aim_pci_target_command_direct(struct aim_pci_device* aimDevice, struct aim_target_command_direct* directTargetCommand)
{
	long ret = 0;
	unsigned int i = 0;
    struct aim_dma_buffer* dmaBuffers = NULL;
    bool buffersDirty = false;
    struct aim_pci_ioctl_ops* ioctlHandlers = NULL;
    struct aim_dma_buffer* currentDmaBuffer = NULL;

	BUG_ON(!aimDevice || !aimDevice->deviceData || !aimDevice->deviceData->ioctlOps || !directTargetCommand);
    ioctlHandlers = aimDevice->deviceData->ioctlOps;

    if(!ioctlHandlers->target_command_direct)
    {
        aim_dev_error(aimDevice, "No protocol specific direct target command handler installed\n");
        return -EINVAL;
    }

    /* Now create a list of scatter/gather DMA capable user space buffers which can be used by the target directly */
    dmaBuffers = kmalloc(sizeof(struct aim_dma_buffer) * directTargetCommand->numDmaBuffers, GFP_KERNEL);
    if(!dmaBuffers)
    {
    	kfree(directTargetCommand);
        aim_dev_error(aimDevice, "Failed to allocate DMA buffers\n");
        return -ENOMEM;
    }

    for(i = 0; i < directTargetCommand->numDmaBuffers; i++)
    {
        currentDmaBuffer = &dmaBuffers[i];
        ret = aim_dma_buffer_init(currentDmaBuffer, directTargetCommand->dmaBuffers[i].base, directTargetCommand->dmaBuffers[i].size);
        if(ret)
        {
            aim_dev_error(aimDevice, "Failed to init DMA buffer\n");
            kfree(dmaBuffers);
            return ret;
        }

        aim_dev_debug(aimDevice, "Buffer %d consists of %d pages\n", i, currentDmaBuffer->numPages);

        ret = aim_dma_buffer_lock_pages(currentDmaBuffer);
        if(ret)
        {
            aim_dev_error(aimDevice, "Locking of pages failed %ld\n", ret);
            goto error;
        }

        ret = aim_dma_buffer_create_sg(currentDmaBuffer);
        if(ret)
        {
            aim_dev_error(aimDevice, "Creation of scatter/gather table failed\n");
            goto error;
        }

        ret = aim_dma_buffer_map(currentDmaBuffer, &aimDevice->pciDevice->dev);
        if(ret)
        {
            aim_dev_error(aimDevice, "Mapping of scatter/gather table failed\n");
            goto error;
        }
    }

    /* Call the protocol specific handler function */
    ret = ioctlHandlers->target_command_direct(aimDevice, directTargetCommand, dmaBuffers);

    aim_dev_debug(aimDevice, "Direct target command processed. Freeing DMA buffers\n");

    /* Assume the DMA buffers are no more needed and free them */
    for(i = 0; i < directTargetCommand->numDmaBuffers; i++)
    {
        if(directTargetCommand->dmaBuffers[i].direction == AIM_DMA_FROM_DEVICE ||
           directTargetCommand->dmaBuffers[i].direction == AIM_DMA_BOTH)
        {
            buffersDirty = true;
        }
        else
        {
            buffersDirty = false;
        }

        aim_dma_buffer_free(&dmaBuffers[i], &aimDevice->pciDevice->dev, buffersDirty);
    }

    kfree(dmaBuffers);

    return ret;

error:
    /* Destroy all DMA buffers that have been set up so far */
    for(i = 0; i < directTargetCommand->numDmaBuffers; i++)
    {
        aim_dma_buffer_free(&dmaBuffers[i], &aimDevice->pciDevice->dev, false);
    }

    kfree(dmaBuffers);

    return ret;

}

static long aim_pci_target_command_direct_dynamic(struct aim_pci_device* aimDevice, struct aim_target_command_direct_dynamic __user* userCommandParams)
{
    struct aim_target_command_direct* directTargetCommand = NULL;
    size_t commandSize = 0;
    long ret = 0;

    BUG_ON(!aimDevice || !aimDevice->deviceData || !aimDevice->deviceData->ioctlOps || !userCommandParams);

    aim_dev_debug(aimDevice, "Processing direct target command\n");

    if(NULL == (directTargetCommand = kmalloc(sizeof(struct aim_target_command_direct_dynamic), GFP_KERNEL)))
    {
    	aim_dev_error(aimDevice, "Failed ot allocate memory for command\n");
    	return -ENOMEM;
    }

    /* Read only the command parameters and the number of buffers from user space */
    if(copy_from_user(directTargetCommand, userCommandParams, sizeof(struct aim_target_command_direct_dynamic)))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        ret = -EFAULT;
        goto error;
    }

    aim_dev_debug(aimDevice, "Command uses %zu user buffers for DMA\n", directTargetCommand->numDmaBuffers);
    commandSize = sizeof(struct aim_target_command_direct_dynamic) + (sizeof(struct aim_dma_buffer_user) * directTargetCommand->numDmaBuffers);

    //Reallocate buffer to read the entire command
    kfree(directTargetCommand);
    directTargetCommand = NULL;
    if(NULL == (directTargetCommand = kmalloc(commandSize, GFP_KERNEL)))
    {
    	aim_dev_error(aimDevice, "Failed ot allocate memory for command\n");
    	return -ENOMEM;
    }

    /* Get the entire command and buffers from userspace */
    if(copy_from_user(directTargetCommand, userCommandParams, commandSize))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        ret = -EFAULT;
        goto error;
    }

    ret = do_aim_pci_target_command_direct(aimDevice, directTargetCommand);

error:
	kfree(directTargetCommand);
	return ret;
}
/*! \brief Handles the direct target command IOCTL
 *
 * This function will map all user space buffers provided for the direct target command so they are ready for DMA. \n
 * Then the protocol specific handler function will be called, which must perform the actual target command. \n
 * After the handler function finished, the DMA buffers will be freed.
 * \param aimDevice device to process the direct target command on
 * \param userCommandParams user space pointer to the command parameters.
 */
static long aim_pci_target_command_direct(struct aim_pci_device* aimDevice, struct aim_target_command_direct __user* userCommandParams)
{
    struct aim_target_command_direct* directTargetCommand = NULL;
    long ret = 0;

    BUG_ON(!aimDevice || !aimDevice->deviceData || !aimDevice->deviceData->ioctlOps || !userCommandParams);

    aim_dev_debug(aimDevice, "Processing direct target command\n");

    if(NULL == (directTargetCommand = kmalloc(sizeof(struct aim_target_command_direct), GFP_KERNEL)))
    {
    	aim_dev_error(aimDevice, "Failed ot allocate memory for command\n");
    	return -ENOMEM;
    }

    if(copy_from_user(directTargetCommand, userCommandParams, sizeof(struct aim_target_command_direct)))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        kfree(directTargetCommand);
        return -EFAULT;
    }

    ret = do_aim_pci_target_command_direct(aimDevice, directTargetCommand);

    kfree(directTargetCommand);

    return ret;
}


/*! \brief This function handles the IOCTL for PCI communication channel command/response sequences
 *
 * @param aimDevice The device the IOCTL was issued on
 * @param userParameters user space pointer to command parameters
 * @return returns 0 on success, a negative error code otherwise
 */
static long aim_ioctl_pci_com_chn_cmd(struct aim_pci_device* aimDevice, struct aim_pci_com_chn_cmd __user *userParameters)
{
    struct aim_pci_com_chn_cmd params = {0};
    struct aim_pci_com_channel* channel = NULL;
    int ret = 0;
    void* commandBuffer = NULL;
    void* responseBuffer = NULL;

    BUG_ON(!aimDevice);

    /* Get the command parameters from user space */
    if(copy_from_user(&params, userParameters, sizeof(params)))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        return -EFAULT;
    }

    aim_dev_debug(aimDevice, "Communication command on channel %u with cmdsize %zu and acksize %zu",
            params.id, params.commandParams.cmdSize, params.commandParams.expectedAckSize);

    channel = radix_tree_lookup(&aimDevice->pciComChannels, params.id);
    if(!channel)
    {
        aim_dev_error(aimDevice, "Communication channel with ID %u was not found", params.id);
        return -EINVAL;
    }

    do
    {
        if(params.commandParams.cmdSize > 0)
        {
            /* Allocate kernel command buffers and copy the user space command data
             * into it
             */
            commandBuffer = kmalloc(params.commandParams.cmdSize, GFP_KERNEL);
            if(!commandBuffer)
            {
                aim_dev_error(aimDevice, "Allocation of command buffer failed");
                ret = -ENOMEM;
                break;
            }

            if(copy_from_user(commandBuffer, params.commandParams.command, params.commandParams.cmdSize))
            {
                aim_dev_error(aimDevice, "Failed to copy command from user buffer");
                ret = -EFAULT;
                break;
            }
        }

        if(params.commandParams.expectedAckSize > 0)
        {
            /* Allocate kernel buffer for response data */
            responseBuffer = kmalloc(params.commandParams.expectedAckSize, GFP_KERNEL);
            if(!responseBuffer)
            {
                aim_dev_error(aimDevice, "Allocation of response buffer failed");
                ret = -ENOMEM;
                break;
            }
        }

        /* Now issue command on specified communication channel */
        ret = aim_pci_com_channel_issue_cmd(channel, commandBuffer, params.commandParams.cmdSize, responseBuffer, params.commandParams.expectedAckSize);
        if(ret)
        {
            aim_dev_error(aimDevice, "PCI communication channel command failed with error %d", ret);
            break;
        }

        if(params.commandParams.expectedAckSize > 0)
        {
            /* Copy response data to user space */
            if(copy_to_user(params.commandParams.acknowledge, responseBuffer, params.commandParams.expectedAckSize))
            {
                aim_dev_error(aimDevice, "Failed to copy response to user space");
                ret = -EFAULT;
                break;
            }
        }
    }while(0);

    if(commandBuffer)
    {
        kfree(commandBuffer);
    }

    if(responseBuffer)
    {
        kfree(responseBuffer);
    }

    return ret;
}


/*! \brief This function handles the IOCTL for PCI communication channel command/response sequences with DMA
 *
 *  This IOCTL will create a DMA scatter/gather list of the user space DMA buffer given with the
 *  IOCTL. \n
 *  It will then attach the number of scatter/gather entries, followed by the list of scatter/gather
 *  entries to the original command data. \n
 *  Each scatter/gather entry consists of a 32-Bit high address, a 32-Bit low address and a 32-Bit data size
 *  value. \n
 *  The resulting command string is written to the corresponding PCI communication channel. \n
 * @param aimDevice The device the IOCTL was issued on
 * @param userParameters user space pointer to command parameters
 * @return returns 0 on success, a negative error code otherwise
 */
static long aim_ioctl_pci_com_chn_cmd_dma(struct aim_pci_device* aimDevice, struct aim_pci_com_chn_cmd_dma __user *userParameters)
{
    struct aim_pci_com_chn_cmd_dma params = {{0}};
    struct aim_pci_com_channel* channel = NULL;
    int ret = 0;
    void*commandBuffer = NULL;
    struct aim_dma_buffer dmaBuffer;
    size_t totalCmdSize = 0;
    int i = 0;
    struct scatterlist* scatterlistEntry = NULL;
    struct dmaBufferPage* currentPage = NULL;
    struct dmaBufferSerialized* serializedBuffer = NULL;
    void* responseBuffer = NULL;
    struct aim_pci_com_chn_cmd* channelCommand = NULL;
    bool bufferDirty = false;

    BUG_ON(!aimDevice);

    memset(&dmaBuffer, 0, sizeof(dmaBuffer));


    /* Get the command parameters from user space */
    if(copy_from_user(&params, userParameters, sizeof(params)))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        return -EFAULT;
    }

    channelCommand = &params.channelCommand;

    aim_dev_debug(aimDevice, "Communication command with DMA on channel %u with cmdsize %zu and acksize %zu",
                  channelCommand->id, channelCommand->commandParams.cmdSize, channelCommand->commandParams.expectedAckSize);

    channel = radix_tree_lookup(&aimDevice->pciComChannels, channelCommand->id);
    if(!channel)
    {
        aim_dev_error(aimDevice, "Communication channel with ID %u was not found", channelCommand->id);
        return -EINVAL;
    }

    do
    {
        /* Check if response is expected and prepare the user space response buffer for DMA */
        if( params.dmaBuffer.base)
        {
            ret = aim_dma_buffer_init(&dmaBuffer, params.dmaBuffer.base,
                                      params.dmaBuffer.size);
            if( ret )
            {
                aim_dev_error(aimDevice, "Failed to init DMA buffer\n");
                break;
            }

            aim_dev_debug(aimDevice, "Buffer consists of %d pages\n", dmaBuffer.numPages);

            ret = aim_dma_buffer_lock_pages(&dmaBuffer);
            if( ret )
            {
                aim_dev_error(aimDevice, "Locking of pages failed %d\n", ret);
                break;
            }

            ret = aim_dma_buffer_create_sg(&dmaBuffer);
            if( ret )
            {
                aim_dev_error(aimDevice, "Creation of scatter/gather table failed\n");
                break;
            }

            ret = aim_dma_buffer_map(&dmaBuffer, &aimDevice->pciDevice->dev);
            if( ret )
            {
                aim_dev_error(aimDevice, "Mapping of scatter/gather table failed\n");
                break;
            }

            aim_dev_debug(aimDevice, "Using %d entries for scatter/gather DMA\n", dmaBuffer.scatterTable.nents);
        }

        totalCmdSize = channelCommand->commandParams.cmdSize; /* size of original command */;
        totalCmdSize += sizeof(__u32) + (dmaBuffer.scatterTable.nents * sizeof(struct dmaBufferPage)); /* number of pages + array of DMA pages */

        aim_dev_debug(aimDevice, "Total size of PCI com DMA Response command is %zu bytes\n", totalCmdSize);

        commandBuffer = kmalloc(totalCmdSize, GFP_KERNEL);
        if( !commandBuffer )
        {
            aim_dev_error(aimDevice, "Allocation of command buffer failed");
            ret = -ENOMEM;
            break;
        }

        /* Copy the original command from user space to command buffer */
        if( copy_from_user(commandBuffer, channelCommand->commandParams.command, channelCommand->commandParams.cmdSize) )
        {
            aim_dev_error(aimDevice, "Failed to copy command from user buffer");
            ret = -EFAULT;
            break;
        }

        /* Now attach all pages of the user space DMA buffer to the command */
        serializedBuffer = (struct dmaBufferSerialized*) (commandBuffer + channelCommand->commandParams.cmdSize);
        serializedBuffer->numBufferPages = dmaBuffer.scatterTable.nents;

        currentPage = serializedBuffer->pageArray;

        for_each_sg(dmaBuffer.scatterTable.sgl, scatterlistEntry, dmaBuffer.scatterTable.nents, i)
        {
            currentPage->highAddr = (sg_dma_address(scatterlistEntry) >> 32) & 0xFFFFFFFFUL;
            currentPage->lowAddr = (sg_dma_address(scatterlistEntry)) & 0xFFFFFFFFUL;
            currentPage->size = sg_dma_len(scatterlistEntry);

            aim_dev_debug(aimDevice, "Adding scatter/gather entry with high addr: 0x%x, low addr 0x%x and size %u bytes\n",
                          currentPage->highAddr, currentPage->lowAddr, currentPage->size);

            currentPage++;
        }

        if(channelCommand->commandParams.expectedAckSize > 0)
        {
            /* Allocate kernel buffer for response data */
            responseBuffer = kmalloc(channelCommand->commandParams.expectedAckSize, GFP_KERNEL);
            if(!responseBuffer)
            {
                aim_dev_error(aimDevice, "Allocation of response buffer failed");
                ret = -ENOMEM;
                break;
            }
        }

        /* Issue command on the given PCI communication channel and neglect response */
        ret = aim_pci_com_channel_issue_cmd(channel, commandBuffer, totalCmdSize, responseBuffer, channelCommand->commandParams.expectedAckSize);
        if(ret)
        {
            aim_dev_error(aimDevice, "PCI communication channel command failed with error %d", ret);
            break;
        }

        if(channelCommand->commandParams.expectedAckSize > 0)
        {
            /* Copy response data to user space */
            if(copy_to_user(channelCommand->commandParams.acknowledge, responseBuffer, channelCommand->commandParams.expectedAckSize))
            {
                aim_dev_error(aimDevice, "Failed to copy response to user space");
                ret = -EFAULT;
                break;
            }
        }

    }while(0);

    if(params.dmaBuffer.direction == AIM_DMA_FROM_DEVICE || params.dmaBuffer.direction == AIM_DMA_BOTH)
    {
        bufferDirty = true;
    }

    /* Free the user space DMA buffer so it will be readable from user space */
    aim_dma_buffer_free(&dmaBuffer, &aimDevice->pciDevice->dev, bufferDirty);

    if(commandBuffer)
    {
        kfree(commandBuffer);
    }

    if(responseBuffer)
    {
        kfree(responseBuffer);
    }

    return ret;
}


/*! \brief This function handles the IRQ deactivation IOCTL
 *
 *
 * @param aimDevice The device the IOCTL was issued on
 * @param userParameters user space pointer to command parameters
 * @return returns 0 on success, a negative error code otherwise
 */
static long aim_ioctl_deactivate_irq(struct aim_pci_device* aimDevice, bool __user *userParameters)
{
    long ret;
    AiBoolean deactivate = false;

    /* Get the command parameters from user space */
    if(copy_from_user(&deactivate, userParameters, sizeof(deactivate)))
    {
        aim_dev_error(aimDevice, "Failed to copy data from user space\n");
        return -EFAULT;
    }

    if(deactivate)
    {
        ret = aim_pci_device_disable_interrupts(aimDevice);
    }
    else
    {
        ret = aim_pci_device_enable_interrupts(aimDevice);
    }

    return ret;
}


/*! \brief Handles IOCTL for getting generic board information
 *
 * @param device the device to get generic information for
 * @param[out] userParameters board information will be stored here
 */
static long aim_ioctl_generic_board_info(struct aim_pci_device* device, TY_DRIVER_INFO __user *userParameters)
{
    TY_DRIVER_INFO board_info = {0};

    BUG_ON(!device || !userParameters);

    board_info.uc_DeviceGroup = device->deviceData->platform;
    board_info.ul_SN = device->properties.serial;
    board_info.ul_BoardConfig = device->properties.config;
    board_info.ul_BoardType = device->properties.type;

    /* Reference count must be decremented by 1, as driver holds one reference without open called */
    board_info.ul_OpenConnections = aim_ref_read(&device->referenceCount) - 1;

    if(device->hasAsp)
    {
        board_info.ul_DriverFlags |= 0x0001; /* ASP */
    }

    if(copy_to_user(userParameters, &board_info, sizeof(board_info)))
    {
        aim_dev_error(device, "Failed to copy board info to user space");
        return -EFAULT;
    }

    return 0;
}


/*! \brief This function processes the get mem size IOCTL
 *
 * It will just return the size of requested memory type if it is available on this platform.
 * Will return -EINVAL if memory type is not available on the device
 * @param device device to request memory size of
 * @param user_params contains the requested memory type and the returned size
 */
static long aim_ioctl_get_mem_size( struct aim_pci_device* device, struct aim_get_mem_size __user*  user_params )
{
    struct aim_get_mem_size mem_size;

    if(copy_from_user(&mem_size, user_params, sizeof(mem_size)))
    {
        aim_dev_error(device, "Failed to copy ioctl get mem size params from user space");
        return -EFAULT;
    }

    switch(mem_size.type)
    {
        case AI_MEMTYPE_GLOBAL:
        case AI_MEMTYPE_GLOBAL_DIRECT:
            mem_size.size = device->globalMemorySize;
            break;

        case AI_MEMTYPE_GLOBAL_EXTENSION:
            mem_size.size = device->globalExtMemorySize;
            break;

        case AI_MEMTYPE_SHARED:
            mem_size.size = device->sharedMemorySize;
            break;

        case AI_MEMTYPE_IO:
            mem_size.size = device->ioMemorySize;
            break;

        default:
            return -EINVAL;
    }

    aim_dev_debug(device, "Size of memory type %d is %lld", mem_size.type, mem_size.size);

    if(copy_to_user(user_params, &mem_size, sizeof(mem_size)))
    {
        aim_dev_error(device, "Failed to copy ioctl get mem size response to user space");
        return -EFAULT;
    }

    return 0;
}


/*! \brief This function processes the asynchronous IO queue creation IOCTL
 *
 * Will get the requested ID from user space and create corresponding queue for the device
 * @param device the device to create asynchronous IO queue for
 * @param argument user space buffer that holds the ID of the queue to create
 * @param fhandle file handle that shall be assigned to queue
 * @return 0 on success, errno code otherwise
 */
static long aim_ioctl_aio_queue_create(struct aim_pci_device* device, int __user* argument, struct file* fhandle)
{
    int id;
    int error;
    struct aim_aio_queue* queue;

    BUG_ON(!device);

    error = get_user(id, argument);

    if (error)
    {
        aim_dev_error(device, "AIO queue delete IOCTL: get_user() returned %d", error);
        return error;
    }
    aim_dev_debug(device, "AIO queue create IOCTL with ID %d", id);

    queue = aim_pci_device_create_aio_queue(device, id, fhandle);

    if(IS_ERR(queue))
    {
        aim_dev_error(device, "AIO queue create IOCTL failed with %ld", PTR_ERR(queue));

        return PTR_ERR(queue);
    }

    aim_aio_queue_put(queue);

    return 0;
}


/*! \brief This function processes the asynchronous IO queue creation IOCTL
 *
 * @param device device to delete asynchronous IO queue of
 * @param argument user space buffer that holds the ID of the queue to delete
 * @return 0 on success, negative errno code on failure
 */
static long aim_ioctl_aio_queue_delete(struct aim_pci_device* device, int __user* argument)
{
    int id;
    int error;

    BUG_ON(!device);

    error= get_user(id, argument);

    if (error)
    {
        aim_dev_error(device, "AIO queue delete IOCTL: get_user() returned %d", error);
        return error;
    }

    aim_dev_debug(device, "AIO queue delete IOCTL with ID %d", id);

    return aim_pci_device_delete_aio_queue(device, id);
}


/*! \brief Copies an asynchronous IO attachment from user space to request
 *
 * @param req the request to copy attachment to
 * @param attachment the user space buffer that holds the attachment
 * @param size size of valid data in attachment buffer in bytes
 * @return 0 on success, negative errno code otherwise
 */
static int __inline__ copy_aio_attachment(struct aim_aio_request* req, void __user* attachment, size_t size)
{
    if( size > 0 )
    {
        if( !attachment )
        {
            aim_dev_error(req->device, "Missing attachment for AIO request");
            return -EINVAL;
        }

        if( size > AIO_REQ_MAX_ATTCHMNT_SIZE )
        {
            aim_dev_error(req->device, "Attachment for AIO request is too large");
            return -EINVAL;
        }

        if( copy_from_user(req->attachment, attachment, size) )
        {
            aim_dev_error(req->device, "Failed to copy attachment for AIO request");
            return -EFAULT;

        }
    }

    req->attachment_size = size;

    return 0;
}


/*! \brief starts a bunch of asynchronous IO operations
 *
 * @param device the device to start aio operations on
 * @param queue the queue to submit the operations to
 * @param aio_ops array of user space pointers to operation definitions
 * @param num_ops number of operations to start
 * @return Number of operations started on success, negative error code on failure
 */
static int __inline__ start_aio_ops(struct aim_pci_device* device, struct aim_aio_queue* queue,
                                    struct aim_aio_op __user** aio_ops, int num_ops)
{
    int i;
    int ret;
    struct aim_aio_op current_op;
    struct aim_aio_request* current_req = NULL;

    ret = 0;
    i = 0;
    while( i < num_ops)
    {
        if( copy_from_user(&current_op, aio_ops[i], sizeof(struct aim_aio_op)) )
        {
            aim_dev_error(device, "Failed to copy AIO op info from user space");
            ret = -EFAULT;
            break;
        }

        aim_dev_debug(device, "Create AIO %s OP with dma buffer %p and size %zu",
                      current_op.buffer.direction == AIM_DMA_TO_DEVICE ? "output" : "input",
                      current_op.buffer.base, current_op.buffer.size);

        current_req = kzalloc(sizeof(struct aim_aio_request), GFP_KERNEL);
        if( !current_req )
        {
            aim_dev_error(device, "Failed to allocate AIO request");
            ret = -ENOMEM;
            break;
        }

        ret = aim_aio_request_init(current_req, device, current_op.buffer.direction, current_op.buffer.base,
                                   current_op.buffer.size);
        if( ret )
        {
            kfree(current_req);
            current_req = NULL;
            break;
        }

        aim_dev_debug(device, "Created AIO request with ID %llu", current_req->id);

        ret = copy_aio_attachment(current_req, current_op.attachment, current_op.attachment_size);
        if(ret)
        {
            aim_dev_error(device, "Failed to append AIO attachment");
            break;
        }

        if( device->deviceData->aio_ops->start_func )
        {
            aim_aio_request_set_start_func(current_req, device->deviceData->aio_ops->start_func);
        }

        if( device->deviceData->aio_ops->cancel_func )
        {
            aim_aio_request_set_cancel_func(current_req, device->deviceData->aio_ops->cancel_func);
        }

        ret = aim_aio_queue_submit(queue, current_req);
        if( ret )
        {
            aim_dev_error(device, "Failed to submit AIO request");
            break;
        }

        /* Copy the ID of the request back to user space */
        if( copy_to_user(&(aio_ops[i]->id), &current_req->id, sizeof(aio_ops[i]->id)) )
        {
            aim_dev_error(device, "Failed to copy AIO request ID to user");
            ret = -EFAULT;
            break;
        }

        aim_aio_request_put(current_req);

        current_req = NULL;
        i++;
    }

    /* We got an error and the last created request has to be freed */
    if(current_req)
    {
        aim_aio_request_put(current_req);
    }

    /* Return number of requests started, if any. Last error code otherwise */
    return i > 0 ? i : ret;
}


/*! \brief This function processes the start asynchronous IO ioctl
 *
 * @param device the device the ioctl was issued to
 * @param argument user space buffer to arguments
 */
static long aim_ioctl_aio_start(struct aim_pci_device* device, struct aim_aio_start __user* argument)
{
    struct aim_aio_start aio_start;
    struct aim_aio_op __user** aio_ops = NULL;
    struct aim_aio_queue* queue = NULL;
    int ret;

    BUG_ON(!device || !device->deviceData || !device->deviceData->aio_ops);

    if(copy_from_user(&aio_start, argument, sizeof(aio_start)))
    {
        aim_dev_error(device, "Failed to copy data from user space");
        return -EFAULT;
    }

    if(aio_start.num_ops == 0)
    {
        return 0;
    }

    aim_dev_debug(device, "Starting %d AIO operations on queue %d", aio_start.num_ops, aio_start.queue_id);

    ret = 0;
    do
    {
        queue = aim_pci_device_get_aio_queue(device, aio_start.queue_id);
        if(IS_ERR(queue))
        {
            aim_dev_error(device, "No AIO queue with ID %d available", aio_start.queue_id);
            ret = PTR_ERR(queue);
            break;
        }

        aio_ops = kzalloc(aio_start.num_ops * sizeof(struct aim_aio_op*), GFP_KERNEL);
        if(!aio_ops)
        {
            aim_dev_error(device, "No memory for aio ops");
            ret = -ENOMEM;
            break;
        }

        if(copy_from_user(aio_ops, aio_start.ops, aio_start.num_ops * sizeof(struct aim_aio_op*)))
        {
            aim_dev_error(device, "Failed to copy AIO op info from user space");
            ret = -EFAULT;
            break;
        }

        ret = start_aio_ops(device, queue, aio_ops, aio_start.num_ops);

    }while(0);

    if(queue && !IS_ERR(queue))
    {
        aim_aio_queue_put(queue);
    }

    if(aio_ops)
    {
        kfree(aio_ops);
    }

    return ret;
}


/*! \brief This function processes the get asynchronous events ioctl
 *
 * @param device he device the ioctl was issued for
 * @param argument user space buffer with arguments
 * @return 0 on success. Negative errno code otherwise
 */
static long aim_ioctl_aio_get_events(struct aim_pci_device* device, struct aim_aio_get_events __user* argument)
{
    struct aim_aio_get_events get_events;
    struct aim_aio_queue* queue = NULL;
    struct aim_aio_request** requests = NULL;
    struct aim_aio_request* current_request;
    struct aim_aio_event current_event;
    int ret;
    int i;

    BUG_ON(!device || !argument);

    if(copy_from_user(&get_events, argument, sizeof(struct aim_aio_get_events)))
    {
        aim_dev_error(device, "Failed to copy data from user space");
        return -EFAULT;
    }

    aim_dev_debug(device, "Getting aio events from queue %d", get_events.queue_id);

    if(get_events.max_events == 0)
    {
        aim_dev_debug(device, "Actually no events requested");
        return 0;
    }

    ret = 0;
    do
    {
        queue = aim_pci_device_get_aio_queue(device, get_events.queue_id);
        if(IS_ERR_OR_NULL(queue))
        {
            ret= PTR_ERR(queue);
            break;
        }

        /* Allocate storage buffer for completed requests and retrieve completed requests
         * from the asynchronous IO queue
         */
        requests = kzalloc(sizeof(struct aim_aio_request*) * get_events.max_events, GFP_KERNEL);
        if(!requests)
        {
            aim_dev_error(device, "Failed to allocate aio request store");
            ret = -ENOMEM;
            break;
        }

        ret = aim_aio_queue_pop_completed(queue, requests, get_events.max_events, get_events.timeout_ms);
        if(ret < 0)
        {
            break;
        }

        /* Now iterate over all completed requests we got
         * and copy corresponding events to user space
         */
        for(i = 0; i < ret; i++)
        {
            current_request = requests[i];

            if(copy_from_user(&current_event, get_events.events + i, sizeof(struct aim_aio_event)))
            {
                aim_dev_error(device, "Failed to copy event from user space");
                ret = -EFAULT;
                break;
            }

            if(current_request->attachment_size > current_event.attachment_size)
            {
                aim_dev_error(device, "Attachment %zu too large for event buffer", current_request->attachment_size);
                ret = -EINVAL;
                break;
            }
            current_event.id = current_request->id;
            current_event.state = current_request->state;
            current_event.bytes_transferred = current_request->bytes_transferred;
            current_event.attachment_size = current_request->attachment_size;

            if(copy_to_user(get_events.events + i, &current_event, sizeof(struct aim_aio_event)))
            {
                aim_dev_error(device, "Failed to copy aio event to user space");
                ret = -EFAULT;
                break;
            }

            if(copy_to_user(current_event.attachment, current_request->attachment, current_request->attachment_size))
            {
                aim_dev_error(device, "Failed to copy event attachment to user space");
                ret = -EFAULT;
                break;
            }

            aim_aio_request_put(current_request);
        }
    }while(0);


    if(!IS_ERR_OR_NULL(queue))
    {
        aim_aio_queue_put(queue);
    }

    if(requests)
    {
        kfree(requests);
    }

    aim_dev_debug(device, "aim_ioctl_aio_get_events() returned %d", ret);
    return ret;
}


/*! \brief This function processes the cancel aio operation ioctl
 *
 * @param device the device the ioctl was issued on
 * @param argument pointer to user space buffer with arguments
 */
static long aim_ioctl_aio_cancel(struct aim_pci_device* device, struct aim_aio_cancel __user* argument)
{
    struct aim_aio_cancel cancel_request;
    struct aim_aio_queue* queue = NULL;
    int ret;

    BUG_ON(!device || !argument);

    if(copy_from_user(&cancel_request, argument, sizeof(struct aim_aio_cancel)))
    {
        aim_dev_error(device, "Failed to copy arguments fro aio cancel from user space");
        return -EFAULT;
    }

    ret = 0;
    do
    {
        queue = aim_pci_device_get_aio_queue(device, cancel_request.queue_id);
        if(IS_ERR_OR_NULL(queue))
        {
            ret = PTR_ERR(queue);
            break;
        }

        ret = aim_aio_queue_cancel_request(queue, cancel_request.aio_op_id);
        if(ret)
        {
            aim_dev_error(device, "Failed to cancel aio request %llu in queue %d", cancel_request.aio_op_id, queue->id);
            break;
        }

    }while(0);

    if(!IS_ERR_OR_NULL(queue))
    {
        aim_aio_queue_put(queue);
    }

    return ret;
}


long aim_ioctl(struct file* file, unsigned int code, unsigned long arguments)
{
    struct aim_pci_device* deviceInfo = NULL;
    long ret = 0;

    deviceInfo = file->private_data;
    if(!deviceInfo)
    {
        aim_error("No valid device info for IOCTL\n");
        return -ENODEV;
    }

    if (_IOC_TYPE(code) != AIM_IOCTL_BASE)
    {
        aim_dev_error(deviceInfo, "Invalid IOCTL base for device\n");
        return -EINVAL;
    }

    switch (code)
    {
    case AIM_IOCTL_GET_DRV_VERS:
        if(_IOC_SIZE(code) != sizeof(struct aim_driver_version))
        {
            aim_dev_error(deviceInfo, "Invalid size of get version parameters\n");
            return -EINVAL;
        }

        if (copy_to_user((struct aim_driver_version_user __user*) arguments, &driverVersion, sizeof(struct aim_driver_version_user)))
        {
           aim_dev_error(deviceInfo, "Failed to copy version to userspace\n");
           return -EFAULT;
        }
        break;

    case AIM_IOCTL_TARGET_COMMAND:
        if(_IOC_SIZE(code) != sizeof(struct aim_target_command))
        {
            aim_dev_error(deviceInfo, "Invalid size of target command parameters\n");
            return -EINVAL;
        }

        ret = aim_pci_target_command(deviceInfo, (struct aim_target_command __user*) arguments);
        break;

    case AIM_IOCTL_TARGET_COMMAND_DIRECT:
        if(_IOC_SIZE(code) != sizeof(struct aim_target_command_direct))
        {
            aim_dev_error(deviceInfo, "Invalid size of target command direct parameters\n");
            return -EINVAL;
        }

        ret = aim_pci_target_command_direct(deviceInfo, (struct aim_target_command_direct __user*) arguments);
        break;

    case AIM_IOCTL_TARGET_COMMAND_DIRECT_DYNAMIC:
    	if(_IOC_SIZE(code) != sizeof(struct aim_target_command_direct_dynamic))
    	{
            aim_dev_error(deviceInfo, "Invalid size of target command direct dynamic parameters\n");
            return -EINVAL;
    	}
        ret = aim_pci_target_command_direct_dynamic(deviceInfo, (struct aim_target_command_direct_dynamic __user*) arguments);
        break;

    case AIM_IOCTL_PCI_COM_CHN_CMD:
        if(_IOC_SIZE(code) != sizeof(struct aim_pci_com_chn_cmd))
        {
            aim_dev_error(deviceInfo, "Invalid size of parameters communication channel command\n");
            return -EINVAL;
        }

        ret = aim_ioctl_pci_com_chn_cmd(deviceInfo, (struct aim_pci_com_chn_cmd __user*) arguments);
        break;


    case AIM_IOCTL_PCI_COM_CHN_CMD_DMA:
        if(_IOC_SIZE(code) != sizeof(struct aim_pci_com_chn_cmd_dma))
        {
            aim_dev_error(deviceInfo, "Invalid size of parameters for communication channel command DMA\n");
            return -EINVAL;
        }

        ret = aim_ioctl_pci_com_chn_cmd_dma(deviceInfo, (struct aim_pci_com_chn_cmd_dma __user*) arguments);
        break;

    case AIM_IOCTL_DEACTIVATE_IRQ:
        if(_IOC_SIZE(code) != sizeof(AiBoolean))
        {
            aim_dev_error(deviceInfo, "Invalid size of parameters for IRQ deactivation IOCTL \n");
            return -EINVAL;
        }
        ret = aim_ioctl_deactivate_irq(deviceInfo, (bool __user*) arguments);
        break;

    case AIM_IOCTL_GENERIC_BOARD_INFO:
        if(_IOC_SIZE(code) != sizeof(TY_DRIVER_INFO))
        {
            aim_dev_error(deviceInfo, "Invalid size of parameters for generic board info ioctl\n");
            return -EINVAL;
        }

        ret = aim_ioctl_generic_board_info(deviceInfo, (TY_DRIVER_INFO __user*) arguments);
        break;

    case AIM_IOCTL_GET_MEM_SIZE:
        if(_IOC_SIZE(code) != sizeof(struct aim_get_mem_size))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for get mem size IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_get_mem_size(deviceInfo, (struct aim_get_mem_size __user*) arguments);
        break;

    case AIM_IOCTL_AIO_QUEUE_CREATE:
        if(_IOC_SIZE(code) != sizeof(int))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for aio queue create IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_aio_queue_create(deviceInfo, (int __user*) arguments, file);
        break;

    case AIM_IOCTL_AIO_QUEUE_DELETE:
        if( _IOC_SIZE(code) != sizeof(int) )
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for aio queue delete IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_aio_queue_delete(deviceInfo, (int __user*) arguments);
        break;

    case AIM_IOCTL_AIO_START:
        if( _IOC_SIZE(code) != sizeof(struct aim_aio_start))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for aio start IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_aio_start(deviceInfo, (struct aim_aio_start __user*) arguments);
        break;

    case AIM_IOCTL_AIO_GET_EVENTS:
        if( _IOC_SIZE(code) != sizeof(struct aim_aio_get_events))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for aio get events IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_aio_get_events(deviceInfo, (struct aim_aio_get_events __user*) arguments);
        break;

    case AIM_IOCTL_AIO_CANCEL:
        if( _IOC_SIZE(code) != sizeof(struct aim_aio_cancel))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for aio cancel IOCTL\n");
            return -EINVAL;
        }

        ret = aim_ioctl_aio_cancel(deviceInfo, (struct aim_aio_cancel __user*) arguments);
        break;

#ifdef AIM_AYE_DS
    case AIM_IOCTL_SCOPE_BUFFERS_PROVIDE:
        if( _IOC_SIZE(code) != sizeof(struct aim_scope_buffer))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for AIM_IOCTL_SCOPE_BUFFERS_PROVIDE\n");
            return -EINVAL;
        }

        ret = aim_aye_ds_device_ioctl_scope_buffers_provide(deviceInfo, (struct aim_scope_buffer __user*) arguments);
        break;
    case AIM_IOCTL_SCOPE_BUFFERS_RELEASE:
        ret = aim_aye_ds_device_ioctl_scope_buffers_release(deviceInfo);
        break;
    case AIM_IOCTL_SCOPE_BUFFERS_WAIT:
        ret = aim_aye_ds_device_ioctl_scope_buffers_wait(deviceInfo);
        break;
    case AIM_IOCTL_SCOPE_BUFFERS_SYNC:
        if( _IOC_SIZE(code) != sizeof(unsigned int))
        {
            aim_dev_error(deviceInfo, "Invalid parameter size for AIM_IOCTL_SCOPE_BUFFERS_SYNC\n");
            return -EINVAL;
        }

        ret = aim_aye_ds_device_ioctl_scope_buffers_sync(deviceInfo, (unsigned int __user*) arguments);
        break;
#endif /* AIM_AYE_DS */




    default:
        aim_dev_error(deviceInfo, "Unknown ioctl %d\n", code);
        ret = -EINVAL;
        break;
    }

    return ret;
}



