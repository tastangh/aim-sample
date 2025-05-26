/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_device.c
 *
 *  This file contains generic definitions
 *  for AIM PCI based devices
 *
 */


#include <linux/fs.h>
#include <linux/module.h>
#include <linux/bitops.h>
#include "aim_pci_device.h"
#include "aim_debug.h"
#include "aim_ioctl.h"
#include "aim_aio.h"
#include "aim_rw.h"
#include "aim_apxe_device.h"
#include "aim_ays_device.h"
#include "aim_amc_device.h"
#include "aim_aye_device.h"
#include "aim_ayx_device.h"
#include "aim_zynqmp_device.h"
#include "aim_artix7_device.h"
#include "aim_artixus_device.h"




/*! \def AIO_QUEUE_ID_TO_INDEX
 *
 * Converts ID of an asynchronous IO queue to an index to a static array holding the queues of a device
 */
#define AIO_QUEUE_ID_TO_INDEX(id) ((id) - 1)




/*! \brief Get asynchronous IO queue with specific ID from device
 *
 * @param device device to get queue from
 * @param id ID of queue to get
 * @return returns queue if available, NULL otherwise
 */
static __inline__ struct aim_aio_queue* get_aio_queue(struct aim_pci_device* device, int id)
{
    return device->aio_queues[AIO_QUEUE_ID_TO_INDEX(id)];
}


/*! \brief Set asynchronous IO queue for device
 *
 * @param device device to set queue for
 * @param queue the queue to set
 */
static __inline__ void set_aio_queue(struct aim_pci_device* device, struct aim_aio_queue* queue)
{
    device->aio_queues[AIO_QUEUE_ID_TO_INDEX(queue->id)] = queue;
}


/*! \brief Clears asynchronous IO queue from a device's queue array
 *
 * @param device the device to clear queue from
 * @param id ID of the queue to clear
 * @return the queue that has been clear. NULL if none was set before
 */
static __inline__ struct aim_aio_queue* clear_aio_queue(struct aim_pci_device* device, int id)
{
    struct aim_aio_queue* queue = device->aio_queues[AIO_QUEUE_ID_TO_INDEX(id)];

    device->aio_queues[AIO_QUEUE_ID_TO_INDEX(id)] = NULL;

    return queue;
}


/*! \brief Implements the 'open' system call
 *
 * This function is called when a user space application opens a device file of this driver. \n
 * It will just store a reference to the device for later IOCTL calls
 * @param inode the inode of the device file that is opened.
 * @param file file pointer to the device file that is opened
 * @return returns 0 on success, a negative error code otherwise
 */
static int aim_pci_open(struct inode* inode, struct file* file)
{
    struct cdev* characterDevice = NULL;
    struct aim_pci_device* deviceInfo = NULL;

    characterDevice = inode->i_cdev;
    if (!characterDevice)
    {
        aim_error("No valid character device\n");
        return -ENODEV;
    }

    deviceInfo = container_of(characterDevice, struct aim_pci_device, characterDev);
    if(!deviceInfo)
    {
        aim_error("No valid device\n");
        return -ENODEV;
    }

    /* Grab the reference count of the device info structure, so
     * it can't be deleted any more.
     */
    aim_ref_get(&deviceInfo->referenceCount);

    /* store the device info structure in the file pointer for use in read/write calls*/
    file->private_data = deviceInfo;

    return 0;
}


/*! \brief Implements the 'close' system call
 *
 * This function is called when a user space application closes a device file of this driver. \n
 * It will just decrement the reference count to the device for later IOCTL calls.
 * @param inode the inode of the device file that is closed.
 * @param file file pointer to the device file that is closed
 * @return returns 0 on success, a negative error code otherwise
 */
static int aim_pci_release(struct inode* inode, struct file* file)
{
    struct cdev* characterDevice = NULL;
    struct aim_pci_device* device = NULL;
    int id;
    struct aim_aio_queue* aio_queue;

    characterDevice = inode->i_cdev;
    if (!characterDevice)
    {
        aim_error("No valid character device\n");
        return -ENODEV;
    }

    device = container_of(characterDevice, struct aim_pci_device, characterDev);
    if (!device)
    {
        aim_error("No valid device\n");
        return -ENODEV;
    }

    /* Check if closed file was assigned to any of the
     * available asynchronous IO queues. If this is the case,
     * delete the corresponding queues, as the can't be accessed any more
     */
    for(id = 1; id <= AIM_DEVICE_MAX_AIO_QUEUES; id++)
    {
        aio_queue = aim_pci_device_get_aio_queue(device, id);
        if(!IS_ERR(aio_queue))
        {
            if(aim_aio_queue_get_fhandle(aio_queue) == file)
            {
                aim_pci_device_delete_aio_queue(device, id);
            }

            aim_aio_queue_put(aio_queue);
        }
    }

    aim_ref_put(&device->referenceCount, release_device);

    return 0;
}




/*! character device interface for AIM PCI drivers */
static struct file_operations aim_pci_fops = {
   .owner =          THIS_MODULE,
   .open  =          aim_pci_open,
   .release =        aim_pci_release,
   .unlocked_ioctl = aim_ioctl,
   .read =           aim_read,
   .write =          aim_write,
   .mmap =           aim_mmap,
};




void release_device(atomic_t* reference)
{
    struct aim_pci_device* aimDevice = NULL;
    struct aim_pci_device_ops* deviceOps = NULL;

    aimDevice = container_of(reference, struct aim_pci_device, referenceCount);

    BUG_ON(!aimDevice->deviceData || !aimDevice->deviceData->deviceOps);

    aim_pci_put_minor(aimDevice->minor);

    deviceOps = aimDevice->deviceData->deviceOps;

    if(!deviceOps->free_device)
    {
        aim_dev_error(aimDevice, "No handler for freeing the device available\n");
    }
    else
    {
        deviceOps->free_device(aimDevice);
    }
}


/*! \brief Releases all target communication channels of a device
 *
 * Iterates over all of the target communication channels of a device,
 * releases their hardware resources and frees them
 * @param aimDevice The device to release the target communication channels of
 */
static void aim_pci_device_target_com_release(struct aim_pci_device* aimDevice)
{
    enum{ maxChannels = 64 };
    struct aim_pci_com_channel* channels[maxChannels];
    unsigned int numChannels = 0;
    unsigned int i = 0;

    BUG_ON(!aimDevice);

    /* There's no radix tree iterator in older kernels, so assume we have at most 64
     * channels in the device
     */
    numChannels = radix_tree_gang_lookup(&aimDevice->pciComChannels, (void**) channels, 0, maxChannels);
    for(i = 0; i < numChannels; i++)
    {
        aim_dev_debug(aimDevice, "Releasing target com channel %u", channels[i]->id);

        if(!radix_tree_delete(&aimDevice->pciComChannels, channels[i]->id))
        {
            aim_dev_warning(aimDevice, "Failed to remove target com channel %u from radix tree", channels[i]->id);
        }

        aim_pci_com_channel_hw_release(channels[i]);

        kfree(channels[i]);
    }
}


/*! \brief Initializes all target communication channels of a device
 *
 * This function iterates over all target communication channels found in the
 * device specific data. \n
 * Each channel is set up, e.g. IO memory is mapped. \n
 * Channels are stored in a device specific radix tree.
 * @param aimDevice the device to initialize target communication channels for
 * @return returns 0 on success, a negative error code otherwise.
 */
static int aim_pci_device_target_com_init(struct aim_pci_device* aimDevice)
{
    unsigned int i;
    struct aim_pci_com_channel* channel = NULL;
    struct aim_pci_com_channel_data* channelData = NULL;
    struct aim_pci_device_data* deviceData = NULL;
    int ret = 0;

    BUG_ON(!aimDevice || !aimDevice->deviceData || !aimDevice->pciDevice);

    deviceData = aimDevice->deviceData;

    /* Iterate over all target communication channels of the device */
    for(i = 0; i < deviceData->numComChannels; i++)
    {
        channelData = &deviceData->comChannels[i];

        aim_dev_debug(aimDevice, "Device offers target communication channel with ID %d", channelData->id);

        channel = kmalloc(sizeof(struct aim_pci_com_channel), GFP_KERNEL);
        if(!channel)
        {
            aim_dev_error(aimDevice, "Failed to allocate target communication channel");
            ret = -ENOMEM;
            break;
        }

        aim_pci_com_channel_init(channel, aimDevice, channelData->id);

        ret = aim_pci_com_channel_hw_setup(channel, channelData, aimDevice);
        if(ret)
        {
            kfree(channel);
            break;
        }

        radix_tree_insert(&aimDevice->pciComChannels, channel->id, channel);
    }

    /* Set-up of communication channels failed, so
     * release all channels that were set up before and
     * return with probe failure
     */
    if(ret)
    {
        aim_pci_device_target_com_release(aimDevice);
    }

    return ret;
}


int aim_pci_device_hw_init(struct aim_pci_device* deviceInfo)
{
    int ret = 0;

    ret = pci_enable_device(deviceInfo->pciDevice);
    if(ret)
    {
        aim_dev_error(deviceInfo, "Failed to enable low-level PCI functionality");
        return ret;
    }

    pci_set_master(deviceInfo->pciDevice);

    /* First perform platform specific initialization, as we need mapped PCI Bars */
    switch(deviceInfo->deviceData->platform)
    {
    case AI_DEVICE_AYXGNET:
        aim_dev_info(deviceInfo, "Found APXE platform device\n");
        ret = aim_apxe_device_hw_init(deviceInfo);
        break;

    case AI_DEVICE_AYS_ASP:
    case AI_DEVICE_AYS:
        aim_dev_info(deviceInfo, "Found AYS platform");
        ret = aim_ays_device_hw_init(deviceInfo);
        break;

#ifdef AIM_AYE
    case AI_DEVICE_AYE:
        aim_dev_info(deviceInfo, "Found AYE platform device");
        ret = aim_aye_device_hw_init(deviceInfo);
        break;
#endif

#ifdef AIM_AMC
    case AI_DEVICE_AMC:
        aim_dev_info(deviceInfo, "Found AMC platform device without ASP");
        ret = aim_amc_device_hw_init(deviceInfo);
        break;
    case AI_DEVICE_AMC_ASP:
        aim_dev_info(deviceInfo, "Found AMC platform device with ASP");
        ret = aim_amc_device_hw_init(deviceInfo);
        break;
#endif

#ifdef AIM_AYX

    case AI_DEVICE_AYX:
        aim_dev_info(deviceInfo, "Found AYX platform device");
        ret = aim_ayx_device_hw_init(deviceInfo);
        break;
#endif

#ifdef AIM_ZYNQMP
    case AI_DEVICE_ZYNQMP_ASP:
        aim_dev_info(deviceInfo, "Found ZynqMP platform device");
        ret = aim_zynqmp_device_hw_init(deviceInfo);
        break;
#endif

#ifdef AIM_ARTIX7
    case AI_DEVICE_ARTIX7:
        aim_dev_info(deviceInfo, "Found Artix7 platform device");
        ret = aim_artix7_device_hw_init(deviceInfo);
        break;
#endif

#ifdef AIM_ARTIXUS
    case AI_DEVICE_ARTIXUS:
        aim_dev_info(deviceInfo, "Found ArtixUS platform device");
        ret = aim_artixus_device_hw_init(deviceInfo);
        break;
#endif


    default:
        return -ENOSYS;
    }

    if(ret)
    {
        pci_disable_device(deviceInfo->pciDevice);
        return ret;
    }

    /* Now initialize all target communication channels of the board */
    ret = aim_pci_device_target_com_init(deviceInfo);
    if(ret)
    {
        aim_dev_error(deviceInfo, "Failed to initialize target communication channels");
        aim_pci_device_hw_remove(deviceInfo); /* Release all previously set up resources */
        return ret;
    }

    return ret;
}


void aim_pci_device_hw_remove(struct aim_pci_device* deviceInfo)
{
    aim_pci_device_target_com_release(deviceInfo);

    switch(deviceInfo->deviceData->platform)
    {
    case AI_DEVICE_AYXGNET:
        aim_dev_info(deviceInfo, "Removing APXE platform device\n");
        aim_apxe_device_hw_remove(deviceInfo);
        break;

    case AI_DEVICE_AYS_ASP:
    case AI_DEVICE_AYS:
        aim_dev_info(deviceInfo, "Removing AYS platform");
        aim_ays_device_hw_remove(deviceInfo);
        break;

#ifdef AIM_AYE
    case AI_DEVICE_AYE:
        aim_dev_info(deviceInfo, "Removing AYE platform device");
        aim_aye_device_hw_remove(deviceInfo);
        break;
#endif

#ifdef AIM_AMC
    case AI_DEVICE_AMC:
    case AI_DEVICE_AMC_ASP:
        aim_dev_info(deviceInfo, "Removing AMC platform device with ASP");
        aim_amc_device_hw_remove(deviceInfo);
        break;

#endif

#ifdef AIM_AYX
    case AI_DEVICE_AYX:
        aim_dev_info(deviceInfo, "Removing AYX platform device");
        aim_ayx_device_hw_remove(deviceInfo);
        break;

#endif

#ifdef AIM_ZYNQMP
    case AI_DEVICE_ZYNQMP_ASP:
        aim_dev_info(deviceInfo, "Removing ZynqMP platform device");
        aim_zynqmp_device_hw_remove(deviceInfo);
        break;

#endif

#ifdef AIM_ARTIX7
    case AI_DEVICE_ARTIX7:
        aim_dev_info(deviceInfo, "Removing Artix7 platform device");
        aim_artix7_device_hw_remove(deviceInfo);
        break;
#endif

#ifdef AIM_ARTIXUS
    case AI_DEVICE_ARTIXUS:
        aim_dev_info(deviceInfo, "Removing ArtixUS platform device");
        aim_artixus_device_hw_remove(deviceInfo);
        break;
#endif


    default:
        aim_dev_error(deviceInfo, "Unknown platform. Device not correctly removed\n");
        return;
    }
}


int aim_pci_device_cdev_setup(struct aim_pci_device* aimDevice)
{
    int ret = 0;
    struct device* deviceFile = NULL;

    BUG_ON(!aimDevice);

    cdev_init(&aimDevice->characterDev, &aim_pci_fops);
    aimDevice->characterDev.owner = THIS_MODULE;

    ret = cdev_add(&aimDevice->characterDev, MKDEV(driver_data.major, aimDevice->minor), 1);
    if(ret)
    {
        aim_dev_error(aimDevice, "Adding of character device failed\n");
        return ret;
    }


#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
    deviceFile = device_create(driver_data.deviceClass, &aimDevice->pciDevice->dev, MKDEV(driver_data.major,aimDevice->minor),
                               "%s%d", AIM_DRV_NAME, aimDevice->minor);
#else
    deviceFile = device_create(driver_data.deviceClass, &aimDevice->pciDevice->dev, MKDEV(driver_data.major,aimDevice->minor),
                                NULL, "%s%d", AIM_DRV_NAME, aimDevice->minor);
#endif
    if(!deviceFile)
    {
        aim_dev_error(aimDevice, "Creation of device file failed\n");
        return -1;
    }

    return ret;
}


int aim_pci_device_enable_interrupts(struct aim_pci_device* device)
{
    BUG_ON(!device || !device->deviceData);

    aim_dev_info(device, "Enabling IRQ handling");

    switch(device->deviceData->platform )
    {
    case AI_DEVICE_AYXGNET:
        aim_apxe_enable_interrupts(device);
        break;

    case AI_DEVICE_AYS_ASP:
        aim_ays_enable_interrupts(device, 0);
        break;

    case AI_DEVICE_AYS:
        aim_ays_enable_interrupts(device, AYS_HOST_IRQ_REG_OFFSET / 4);
        break;

#ifdef AIM_AYE
    case AI_DEVICE_AYE:
        aim_aye_enable_interrupts(device);
        break;
#endif

#ifdef AIM_AMC
    case AI_DEVICE_AMC:
    case AI_DEVICE_AMC_ASP:
        aim_amc_enable_interrupts(device);
        break;
#endif

#ifdef AIM_AYX
    case AI_DEVICE_AYX:
        aim_ayx_enable_interrupts(device);
        break;

#endif

#ifdef AIM_ZYNQMP
    case AI_DEVICE_ZYNQMP_ASP:
        aim_zynqmp_enable_interrupts(device);
        break;

#endif

#ifdef AIM_ARTIX7
    case AI_DEVICE_ARTIX7:
        aim_artix7_enable_interrupts(device);
        break;
#endif

#ifdef AIM_ARTIXUS
    case AI_DEVICE_ARTIXUS:
        aim_artixus_enable_interrupts(device);
        break;
#endif


    default:
        aim_dev_error(device, "Unknown platform. IRQ handling can not be enabled\n");
        return -ENOSYS;
    }

    return 0;
}


int aim_pci_device_disable_interrupts(struct aim_pci_device* device)
{
    BUG_ON(!device || !device->deviceData);

    aim_dev_info(device, "Disabling IRQ handling");

    switch(device->deviceData->platform )
    {
    case AI_DEVICE_AYXGNET:
        aim_apxe_disable_interrupts(device);
        break;

    case AI_DEVICE_AYS_ASP:
        aim_ays_disable_interrupts(device,0);
        break;

    case AI_DEVICE_AYS:
        aim_ays_disable_interrupts(device, (AYS_HOST_IRQ_REG_OFFSET / 4));
        break;


#ifdef AIM_AYE
    case AI_DEVICE_AYE:
        aim_aye_disable_interrupts(device);
        break;
  #endif

#ifdef AIM_AMC
    case AI_DEVICE_AMC:
    case AI_DEVICE_AMC_ASP:
        aim_amc_disable_interrupts(device);
        break;
#endif

#ifdef AIM_AYX
    case AI_DEVICE_AYX:
        aim_ayx_disable_interrupts(device);
        break;
#endif

#ifdef AIM_ZYNQMP
    case AI_DEVICE_ZYNQMP_ASP:
        aim_zynqmp_disable_interrupts(device);
        break;
#endif

#ifdef AIM_ARTIX7
    case AI_DEVICE_ARTIX7:
        aim_artix7_disable_interrupts(device);
        break;
#endif

#ifdef AIM_ARTIXUS
    case AI_DEVICE_ARTIXUS:
        aim_artixus_disable_interrupts(device);
        break;
#endif


    default:
        aim_dev_error(device, "Unknown platform. IRQ handling can not be disabled\n");
        return -ENOSYS;
    }

    return 0;
}


struct aim_aio_queue* aim_pci_device_create_aio_queue(struct aim_pci_device* device, int id, struct file* fhandle)
{
    struct aim_aio_queue* queue;

    BUG_ON(!device);

    if(id < 1 || id > AIM_DEVICE_MAX_AIO_QUEUES)
    {
        aim_dev_error(device, "Invalid AIO queue ID: %d", id);

        return ERR_PTR(-EINVAL);
    }

    queue = kzalloc(sizeof(struct aim_aio_queue), GFP_KERNEL);
    if(!queue)
    {
        aim_dev_error(device, "Failed to allocate AIO queue");
        return ERR_PTR(-ENOMEM);
    }

    spin_lock_bh(&device->aio_queues_lock);

    do
    {
        if( get_aio_queue(device, id) != NULL)
        {
            aim_dev_error(device, "Queue with ID %d already created", id);
            kfree(queue);
            queue = ERR_PTR(-EEXIST);
            break;
        }

        aim_aio_queue_init(queue, id, device);

        aim_aio_queue_get(queue);

        aim_aio_queue_assign_fhandle(queue, fhandle);

        set_aio_queue(device, queue);

    }while(0);

    spin_unlock_bh(&device->aio_queues_lock);

    return queue;
}


int aim_pci_device_delete_aio_queue(struct aim_pci_device* device, int id)
{
    struct aim_aio_queue* queue;
    int ret;

    BUG_ON(!device);

    aim_dev_debug(device, "Deleting AIO queue with ID %d", id);

    if(id < 1 || id > AIM_DEVICE_MAX_AIO_QUEUES)
    {
        aim_dev_error(device, "Invalid AIO queue ID: %d", id);
        return -EINVAL;
    }

    spin_lock_bh(&device->aio_queues_lock);

    queue = clear_aio_queue(device, id);

    spin_unlock_bh(&device->aio_queues_lock);

    if(queue)
    {
        aim_aio_queue_put(queue);
        ret = 0;
    }
    else
    {
        aim_dev_error(device, "Tried to delete non-existent AIO queue %d", id);
        ret = -EINVAL;
    }

    return ret;
}


struct aim_aio_queue* aim_pci_device_get_aio_queue(struct aim_pci_device* device, int id)
{
    struct aim_aio_queue* queue;

    BUG_ON(!device);

    if(id < 1 || id > AIM_DEVICE_MAX_AIO_QUEUES)
    {
        aim_dev_error(device, "Invalid AIO queue ID: %d", id);
        return ERR_PTR(-EINVAL);
    }

    spin_lock_bh(&device->aio_queues_lock);

    queue = get_aio_queue(device, id);

    if(queue)
    {
        aim_aio_queue_get(queue);
    }
    else
    {
        queue = ERR_PTR(-EIDRM);
    }

    spin_unlock_bh(&device->aio_queues_lock);

    return queue;
}

