/* SPDX-FileCopyrightText: 2013-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_module.c
 *
 *  This file contains generic definitions
 *  for AIM PCI drivers
 *
 */


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/bitmap.h>
#include <linux/version.h>
#include "aim_pci_module.h"
#include "aim_pci_device.h"
#include "aim_debug.h"
#include "aim_nl.h"
#include "aim_pci_mem.h"
#include "generated/version.h"




/* no longer supported since 3.8 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
  #define __devinit
  #define __devexit
  #define __devexit_p(x) x
#endif


/* __refdata was introduced with kernel 2,6,25 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)

  #define __refdata

#endif




/*! This bitmap is used to administer the minor numbers
 * used for AIM PCI devices.
 */
DECLARE_BITMAP(deviceMinors, AIM_MAX_DEVICES);




/*! Global, built-in version of the driver
 */
struct aim_driver_version driverVersion = {
        .major = MAJOR_VERSION,
        .minor = MINOR_VERSION,
        .patch = PATCH_VERSION,
        .build = 0,
        .full_version = VERSION_STRING_FULL
};


/*! Global driver specific data */
struct aim_pci_driver_data driver_data = {
        .major = 0,             /*!< Stores the major number once it is allocated */
        .deviceClass = NULL,    /*!< Stores the class of the driver once allocated */
};


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)


#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITS_PER_LONG))



static void bitmap_set(unsigned long *map, int start, int nr)
{
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_set = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_set >= 0) {
        *p |= mask_to_set;
        nr -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        p++;
    }
    if (nr) {
        mask_to_set &= BITMAP_LAST_WORD_MASK(size);
        *p |= mask_to_set;
    }
}

static void bitmap_clear(unsigned long *map, int start, int nr)
{
    unsigned long *p = map + BIT_WORD(start);
    const int size = start + nr;
    int bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

    while (nr - bits_to_clear >= 0) {
        *p &= ~mask_to_clear;
        nr -= bits_to_clear;
        bits_to_clear = BITS_PER_LONG;
        mask_to_clear = ~0UL;
        p++;
    }
    if (nr)
    {
        mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
        *p &= ~mask_to_clear;
    }
}
#endif


long aim_pci_get_minor(void)
{
    unsigned long first = find_first_zero_bit(deviceMinors, AIM_MAX_DEVICES);

    if(first < AIM_MAX_DEVICES)
    {
        bitmap_set(deviceMinors, first, 1);
        return first;
    }
    else
    {
        return -EMFILE;
    }
}


void aim_pci_put_minor(unsigned long minor)
{
    bitmap_clear(deviceMinors, minor, 1);
}




/*! \brief Generic probe function of the AIM PCI driver
 *
 * This function is called once the driver detects a PCI device it is responsible for. \n
 * It will perform generic, platform specific and protocol specific initialization of the device. \n
 * After successful initialization, the character device interface for the device is set up.
 * \param dev PCI device to probe
 * \param pci_device_id device specific data from PCI device table
 * \return returns 0 on success, a negative error code otherwise.
 */
static int __devinit aim_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    struct aim_pci_device* aimDevice = NULL;
    struct aim_pci_device_ops* deviceOps = NULL;
    struct aim_pci_device_data* deviceData = NULL;
    int ret = 0;

    BUG_ON(!dev || !id);

    dev_info(&dev->dev, "Probing device\n");

    deviceData = (struct aim_pci_device_data*) id->driver_data;
    if(!deviceData)
    {
        dev_err(&dev->dev, "Device misses mandatory device data\n");
        return -ENODEV;
    }

    deviceOps = deviceData->deviceOps;
    if(!deviceOps)
    {
        dev_err(&dev->dev, "Device misses mandatory device operations\n");
        return -ENODEV;
    }

    if(!deviceOps->create_device)
    {
        dev_err(&dev->dev, "Device misses device creation handler\n");
        return -ENODEV;
    }

    /* Call the protocol specific driver part for generating a protocol specific
     * device structure
     */
    aimDevice = deviceOps->create_device(dev, id);
    if(!aimDevice)
    {
        dev_err(&dev->dev, "Failed to create AIM PCI device\n");
        return -ENODEV;
    }

    /* Perform protocol independent initialization of the device structure */
    aim_pci_device_init(aimDevice, (struct aim_pci_device_data*) id->driver_data, dev);

    ret = aim_pci_device_hw_init(aimDevice);
    if(ret)
    {
        aim_dev_error(aimDevice, "Initialization of device failed\n");
        aim_ref_put(&aimDevice->referenceCount,release_device);
        return ret;
    }

    /* Perform protocol specific initialization of device */
    if(!deviceOps->init_device)
    {
        aim_dev_warning(aimDevice, "No protocol specific device init handler available\n");
    }
    else
    {
        ret = deviceOps->init_device(aimDevice);
        if(ret)
        {
            aim_dev_error(aimDevice, "Protocol specific initialization of device failed\n");
            aim_ref_put(&aimDevice->referenceCount,release_device);
            return ret;
        }
    }

    /* Now set-up the character device based user space interface */
    ret = aim_pci_device_cdev_setup(aimDevice);
    if(ret)
    {
        aim_ref_put(&aimDevice->referenceCount,release_device);
        return ret;
    }

    /* Set-up the netlink user space interface */
    ret = aim_nl_dev_setup(aimDevice);
    if(ret)
    {
        aim_ref_put(&aimDevice->referenceCount,release_device);
        return ret;
    }

    return ret;
}


/*! \brief Generic remove function of the AIM PCI driver
 *
 * This function is called when a PCI device is removed from the system. \n
 * It will tear down the user space interface and shut down the hardware.
 * \param dev the PCI device to remove
 */
static void __devexit aim_pci_remove(struct pci_dev* dev)
{
    struct aim_pci_device* aimDevice = NULL;

    aimDevice = dev_get_drvdata(&dev->dev);
    if(!aimDevice)
    {
        aim_dev_error(aimDevice, "No valid device info available\n");
        return;
    }

    aim_nl_dev_release(aimDevice);

    device_destroy(driver_data.deviceClass, MKDEV(driver_data.major, aimDevice->minor));

    cdev_del(&aimDevice->characterDev);

    aim_pci_device_hw_remove(aimDevice);

    aim_ref_put(&aimDevice->referenceCount, release_device);

    return;
}




/*! Global PCI driver structure for the AIM device driver */
static struct pci_driver __refdata aim_pci_module = {
        .name       = AIM_DRV_NAME,
        .probe      = aim_pci_probe,
        .remove     = aim_pci_remove,
        .id_table   = aim_pci_table,
};




/*! \brief Init function of the AIM PCI driver
 *
 * This function is called when the AIM driver module is loaded. \n
 * It will allocate a major number for the driver, create a device class \n
 * and registers a PCI driver.
 * \return returns 0 on success, a negative error code otherwise
 */
static int __init init_pci_module(void)
{
    int ret = 0;
    dev_t deviceID = 0;

    aim_info("Loading driver module %s. Build on: %s\n", driverVersion.full_version, BUILD_DATE);

    bitmap_zero(deviceMinors, AIM_MAX_DEVICES);

    ret = aim_mem_init();
    if(ret)
    {
        aim_error("Initialization of memory subsystem failed\n");
        return ret;
    }

    /* Allocate a major number for the driver */
    ret = alloc_chrdev_region(&deviceID, 0, AIM_MAX_DEVICES, AIM_DRV_NAME);
    if(ret)
    {
        aim_error("Allocation of major number failed\n");
        return ret;
    }

    driver_data.major = MAJOR(deviceID);
    aim_info("Using major number %u\n", driver_data.major);

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,4,0)
    driver_data.deviceClass = class_create(THIS_MODULE, AIM_DRV_NAME);
#else
    driver_data.deviceClass = class_create(AIM_DRV_NAME);
#endif

    if(!driver_data.deviceClass || IS_ERR(driver_data.deviceClass))
    {
        aim_error("Creation of device class failed\n");
        unregister_chrdev_region(MKDEV(driver_data.major,0), AIM_MAX_DEVICES);
        return PTR_ERR(driver_data.deviceClass);
    }

    ret = pci_register_driver(&aim_pci_module);
    if(ret)
    {
        aim_error("Registering of driver failed\n");
        unregister_chrdev_region(MKDEV(driver_data.major,0), AIM_MAX_DEVICES);
        return ret;
    }

    return 0;
}


/*! \brief Exit function of the AIM PCI driver
 *
 * This function is called when the AIM PCI driver is unloaded. \n
 * It will free the reserved major number, the device class \n
 * and unregisters the PCI driver.
 */
static void __exit exit_pci_module(void)
{
    aim_info("Unloading module");

    pci_unregister_driver(&aim_pci_module);

    class_destroy(driver_data.deviceClass);

    unregister_chrdev_region(MKDEV(driver_data.major,0), AIM_MAX_DEVICES);

    aim_mem_exit();

    return;
}




MODULE_AUTHOR("AIM GmbH");
MODULE_LICENSE("GPL");


module_init(init_pci_module)
module_exit(exit_pci_module)

