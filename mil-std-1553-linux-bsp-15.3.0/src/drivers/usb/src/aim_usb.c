/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb.c
 *
 */


#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <aim_usb.h>
#include "aim_usb_debug.h"
#include "aim_usb_interface.h"
#include "aim_usb_fs.h"
#include "generated/version.h"



/*! \def AIM_USB_MINOR_BASE
 * Minor number base for AIM USB devices.
 * We use 0 here to let the USB sub-system decide which minor to assign
 */
#define AIM_USB_MINOR_BASE 0




/*! aim_usb_fops
 * File operations used by the USB driver
 */
static const struct file_operations aim_usb_fops = {
    .owner = THIS_MODULE,
    .read = aim_usb_interface_read,
    .write = aim_usb_interface_write,
    .open = aim_usb_interface_open,
    .release = aim_usb_interface_release,
    .flush = NULL,
    .unlocked_ioctl = aim_usb_interface_ioctl,
};


/*! aim_usb_class
 * USB class driver that defines the driver's user interface
 */
static struct usb_class_driver aim_usb_class = {
    .name = KBUILD_MODNAME "%d",
    .fops = &aim_usb_fops,
    .minor_base = AIM_USB_MINOR_BASE,
};




/*! \brief Device probe function
 *
 * Will be called when any compatible USB interface is detected
 * @param intf the USB interface to probe
 * @param id USB IDs of the interface to probe
 * @return 0 if device was successfully probed, errno otherwise
 */
static int aim_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct aim_usb_interface* aim_intf;
    struct aim_usb_interface_data* intf_data;
    int err;

    BUG_ON(!intf || !id);

    intf_data = (struct aim_usb_interface_data*) id->driver_info;
    BUG_ON(!intf_data);

    dev_info(&intf->dev, "%s detected", intf_data->name);

    dev_dbg(&intf->dev, "\t.Number of settings: %u",intf->num_altsetting);
    dev_dbg(&intf->dev, "\t.Current setting: %p",intf->cur_altsetting);
    dev_dbg(&intf->dev, "\t.Minor: %d", intf->minor);
    dev_dbg(&intf->dev, "\t.Condition: %d", intf->condition);
    dev_dbg(&intf->dev, "\t.SysFS created: %s", intf->sysfs_files_created ? "Yes" : "No");
    dev_dbg(&intf->dev, "\t.USB device: %p", intf->usb_dev);

    BUG_ON(!intf_data->intf_ops || !intf_data->intf_ops->create);

    /* Let protocol specific part create the AIM USB device */
    aim_intf = intf_data->intf_ops->create(intf, id);
    if(IS_ERR_OR_NULL(aim_intf))
    {
        return -ENODEV;
    }

    /* Perform protocol independent initialization of the device */
    err = aim_usb_interface_init(aim_intf, (struct aim_usb_interface_data*) id->driver_info, intf);
    if(err)
    {
        dev_err(&intf->dev, "Failed to initialize AIM USB interface");
        BUG_ON(!aim_intf->intf_ops->free);
        aim_intf->intf_ops->free(aim_intf);
        return err;
    }

    err = 0;
    do{

        /* Now perform protocol specific device initialization */
        if(intf_data->intf_ops->init)
        {
            err = intf_data->intf_ops->init(aim_intf);
            if(err)
            {
                break;
            }
        }

        usb_set_intfdata(intf, aim_intf);

        err = usb_register_dev(intf, &aim_usb_class);
        if(err)
        {
            dev_err(&intf->dev, "Failed to register USB device");
            break;
        }

        aim_usb_intf_info(aim_intf, "%s attached to /dev/%s%d", intf_data->name, KBUILD_MODNAME, intf->minor);

        err = aim_usb_nl_setup(aim_intf);
        if(err)
        {
            aim_usb_intf_err(aim_intf, "Failed to set up netlink interface");
            break;
        }

    }while(0);

    if(err)
    {
        aim_ref_put(&aim_intf->ref_count, aim_usb_interface_free);
    }

    return err;
}


/*! \brief Disconnect function
 *
 * Will be called when any bound interface is disconnected
 * @param intf the USB interface that will be connected
 */
static void aim_usb_disconnect(struct usb_interface *intf)
{
    struct aim_usb_interface* aim_intf;

    aim_intf = usb_get_intfdata(intf);

    BUG_ON(!aim_intf);

    aim_usb_intf_info(aim_intf, "Disconnected");

    usb_set_intfdata(intf, NULL);

    /* give back our minor */
    usb_deregister_dev(intf, &aim_usb_class);

    aim_ref_put(&aim_intf->ref_count, aim_usb_interface_free);
}



/*! aim_usb_driver
 *
 * Defines basic functions of the driver
 */
struct usb_driver aim_usb_driver = {
    .name =  KBUILD_MODNAME,
    .probe = aim_usb_probe,
    .disconnect = aim_usb_disconnect,
    .suspend = NULL,
    .resume = NULL,
    .pre_reset = NULL,
    .post_reset = NULL,
    .id_table = aim_usb_device_table,
    .supports_autosuspend = 1,
};




/*! \brief Module initialization function
 *
 * Will be called when driver module is loaded
 * @return 0 on success, errno if module load fails
 */
static int __init init_usb_module(void)
{
    int err;

    aim_info("Loading version %s. Build on: %s\n", VERSION_STRING_FULL , BUILD_DATE);

    err = usb_register_driver(&aim_usb_driver, THIS_MODULE, KBUILD_MODNAME);
    if(err)
    {
        aim_error("Failed to register driver");
        return err;
    }

    return 0;
}


/*! \brief Module exit function
 *
 * Will be called when driver module is unloaded
 */
static void __exit exit_usb_module(void)
{
    aim_info("Unloading module");

    usb_deregister(&aim_usb_driver);

    return;
}


MODULE_AUTHOR("AIM GmbH");
MODULE_LICENSE("GPL");


module_init(init_usb_module)
module_exit(exit_usb_module)
