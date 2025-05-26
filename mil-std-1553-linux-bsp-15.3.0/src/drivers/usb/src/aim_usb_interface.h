/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_interface.h
 *
 */

#ifndef AIM_USB_INTERFACE_H_
#define AIM_USB_INTERFACE_H_


#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include "aim_usb_debug.h"
#include "aim_usb_com_channel.h"
#include "aim_usb_interrupt.h"
#include "aim_usb_nl.h"
#include "aim_usb.h"
#include "aim_ref.h"
#include "apu/aim_usb_nc.h"
#include "Ai_def.h"

/*! \def AIM_USB_MAX_COM_CHANNELS
 * Maximum number of USB communication channels supported per interface
 */
#define AIM_USB_MAX_COM_CHANNELS 3


/* Forward declaration */
struct aim_target_command;


/*! \struct aim_usb_memory_space
 * This structure describes a memory space accessible by AIM USB devices
 */
struct aim_usb_memory_space
{
    phys_addr_t bus_address;    /*!< Physical bus address of the memory. E.g. PCI address on APU global memory */
    size_t size;                /*!< Size of memory space in bytes */
    void* base;                 /*!< pointer to memory space base if mapped to kernel memory */
};


/*! \enum aim_usb_protocols
 * Enumeration of all protocols the generic USB driver part supports
 */
enum aim_usb_protocols
{
    PROTOCOL_429 = 0,  /*!< 429 Protocol */
    PROTOCOL_1553,     /*!< 1553 Protocol */
    PROTOCOL_FDX       /*!< AFDX Protocol */
};

/*! \struct aim_usb_interface
 *
 * This structure defines a protocol independent abstraction layer
 * for AIM USB interfaces.
 */
struct aim_usb_interface
{
    struct usb_interface* interface;        /*!< The generic USB interface the interface is bound to */
    struct device* dev;                     /*!< kernel generic device */
    TY_E_DEVICE_GROUP platform;             /*!< The platform the AIM USB device belongs to */
    enum aim_usb_protocols protocol;        /*!< The data bus protocol the device implements */
    AiUInt32 serial;                        /*!< The serial number of the device */
    AiUInt32 board_config;                  /*!< Board configuration value from device NOVRAM */
    AiUInt32 board_type;                    /*!< Board type value from device NOVRAM */
    AiUInt32 board_sub_type;                /*!< Board sub type value from device NOVRAM */
    AiUInt32 board_part_no;                 /*!< Board part no value from device NOVRAM */
    AiUInt32 hw_variant;                    /*!< Board hw variant value from device NOVRAM */
    struct mutex io_mutex;                  /*!< Lock board for IOCTL and Memory Access */

    AiBoolean has_asp;                      /*!< Indicates if board has Application Support Processor (ASP) */

    struct aim_usb_interface_ops* intf_ops; /*!< Protocol specific interface operations */
    struct aim_usb_ioctl_ops* ioctl_ops;    /*!< Protocol specific ioctl handlers */

    struct aim_usb_com_channel* com_channels[AIM_USB_MAX_COM_CHANNELS];

    struct aim_usb_int_ep* interrupt_ep;    /*!< Interrupt endpoint of the interface */

    struct aim_usb_memory_space global_memory;  /*!< Global memory space of a device */

    struct aim_usb_memory_space io_memory;      /*!< IO memory space of a device */

    struct aim_usb_memory_space shared_memory;  /*!< Shared memory space of a device */

    struct aim_usb_nl_setup nl_setup;       /*!< Protocol specific Netlink setup that can be used as interface to user space */

    struct aim_usb_ncc ncc;                 /*!< Controller for Netchip based devices */

    struct aim_usb_ncdmac ncdmac;          /*!< DMA Controller for Netchip based devices */

    atomic_t ref_count;                    /*!< Reference count */
};

/*! \def aim_usb_interface_active_users
 * Get number of active users of the interface.
 * The reference count is decremented by 1 to get actual number of open calls
 */
#define aim_usb_interface_active_users(aim_intf) \
    (aim_ref_read(&(aim_intf)->ref_count) - 1)


/*! \struct aim_usb_interface_ops
 *
 * This structure comprises protocol specific handler
 * functions for an interface
 */
struct aim_usb_interface_ops
{
    struct aim_usb_interface* (*create) (struct usb_interface*, const struct usb_device_id*);   /*!< Create USB interface device */

    void (*free) (struct aim_usb_interface*);     /*!< Function is called if reference count of interface drops to 0. Must release all resources allocated in create handler */
    int (*init)(struct aim_usb_interface*);       /*!< Protocol specific initialization of an interface can be performed here */
    int (*start)(struct aim_usb_interface*);      /*!< Protocol specific start-up function for an interface */
    void (*stop)(struct aim_usb_interface*);      /*!< Protocol specific stop function for an interface */
};


/*! \struct aim_usb_ioctl_ops
 *
 * This structure encapsulates protocol specific IOCTL handlers
 */
struct aim_usb_ioctl_ops
{
    ssize_t (*target_command)(struct aim_usb_interface*, void*, size_t, void*, size_t); /*!< Handler function for target command IOCTL        */
    int (*sync_active_memory)(struct aim_usb_interface*, size_t offset, size_t size);   /*!< Handler function for synchronizing memory mirror */
};


/*! \struct aim_usb_interface_data
 *
 * This structure comprises all information for a specific interface
 * that must be provided by protocol specific driver implementation
 */
struct aim_usb_interface_data
{
    TY_E_DEVICE_GROUP platform;              /*!< The device group the interface belongs to */
    enum aim_usb_protocols protocol;         /*!< The data bus protocol the device implements */
    char* name;                              /*!< Product name of the device */
    struct aim_usb_interface_ops* intf_ops;  /*!< Protocol specific handler functions for the interface creation/initialization */
    struct aim_usb_ioctl_ops* ioctl_ops;     /*!< Protocol specific IOCTL handler functions */
};


/*! \brief Protocol independent interface initialization
 * 
 * @param aim_interface AIM USB interface to initialize
 * @param data interface data
 * @param interface generic USB interface
 */
extern int aim_usb_interface_init(struct aim_usb_interface* aim_interface, struct aim_usb_interface_data* data,
                                  struct usb_interface* interface);


/*! \brief Free AIM USB interface
 *
 * Will call the protocol specific handler to free the interface
 * @param ref reference counter included in AIM USB interface to free
 */
static __inline__ void aim_usb_interface_free(atomic_t* ref)
{
    struct aim_usb_interface* aim_intf;
    int i;

    aim_intf = container_of(ref, struct aim_usb_interface, ref_count);

    aim_usb_intf_dbg(aim_intf, "Freeing interface");

    aim_usb_nl_release(aim_intf);

    for(i = 0; i < AIM_USB_MAX_COM_CHANNELS; i++)
    {
        if(aim_intf->com_channels[i])
        {
            kfree(aim_intf->com_channels[i]);

            aim_intf->com_channels[i] = NULL;
        }
    }

    if(aim_intf->interrupt_ep)
    {
        aim_usb_int_ep_free(aim_intf->interrupt_ep);
        kfree(aim_intf->interrupt_ep);
        aim_intf->interrupt_ep = NULL;
    }

    aim_usb_ncdmac_free(&aim_intf->ncdmac);
    aim_usb_ncc_free(&aim_intf->ncc);

    usb_put_intf(aim_intf->interface);
    aim_intf->interface = NULL;

    BUG_ON(!aim_intf->intf_ops || !aim_intf->intf_ops->free);

    aim_intf->intf_ops->free(aim_intf);
}


/*! \brief Interface start-up function
 *
 * This is a protocol independent function for starting an USB interface.
 * It will be called on first open syscall to the interface.
 * @param aim_intf the interface to start
 * @return 0 on success, errno on failure.
 */
extern int aim_usb_interface_start(struct aim_usb_interface* aim_intf);


/*! \brief Interface stop function
 *
 * This is a protocol independent function for stopping an USB interface.
 * It will be called on last close syscall to the interface.
 * @param aim_intf the interface to stop
 * @return None.
 */
extern void aim_usb_interface_stop(struct aim_usb_interface* aim_intf);


/*! \brief Allocates and assigns an USB communication channel to an interface
 *
 * @param intf the interface to creaet communication channel for
 * @param id ID to assign for the communication channel
 * @param cmd_ep command endpoint of the communication channel
 * @param response_ep response endpoint of the communication channel
 * @return 0 on success, errno code on failure
 */
extern struct aim_usb_com_channel* aim_usb_interface_create_com_channel(struct aim_usb_interface* intf, int id,
                                                                        struct usb_host_endpoint* cmd_ep,
                                                                        struct usb_host_endpoint* response_ep);


/*! \brief Get USB communication channel with specific ID
 *
 * @param intf The interface to get communication channel og
 * @param id ID of channel to get
 * @return pointer to communication channel on success, ERR_PTR on failure
 */
extern struct aim_usb_com_channel* aim_usb_interface_get_com_channel(struct aim_usb_interface* intf, int id);


/*! \brief Create interrupt endpoint for an AIM USB interface
 *
 * This function will set up the interrupt endpoint of an AIM USB interface.
 * It will allocate corresponding URB and transfer buffer.
 * @param intf the interface to create interrupt endpoint for
 * @param ep the actual USB endpoint to use for receiving interrupts
 * @return 0 on success, errno on failure
 */
extern struct aim_usb_int_ep* aim_usb_interface_create_int_ep(struct aim_usb_interface* intf,
                                                              struct usb_host_endpoint* ep);




#endif /* AIM_USB_INTERFACE_H_ */
