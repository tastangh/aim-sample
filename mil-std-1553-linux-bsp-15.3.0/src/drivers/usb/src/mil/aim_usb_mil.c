/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_mil.c
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include "aim_usb.h"
#include "aim_usb_interface.h"
#include "aim_usb_ays.h"
#include "aim_usb_apu.h"
#include "mil/aim_usb_nl_interface_mil.h"
#include "generated/mil_tsw_version.h"
#include "mil_tsw_includes.h"
#include "api_int_loglist.h"


/*! \def SHARED_MEM_SIZE
 * Shared memory size to allocate for devices without ASP
 */
#define SHARED_MEM_SIZE 0x1000000




/*! \struct aim_usb_mil_interface
 *
 * This structure represents an AIM 1553 USB interface
 */
struct aim_usb_mil_interface
{
    struct aim_usb_interface intf;  /*!< Generic AIM USB interface */
    TY_API_DEV tsw; /*!< Properties of internal target software library */
    struct ty_api_intr_event_list * irq_loglist; /*!< Interrupt loglist in shared memory */

    struct work_struct irq_work_item;   /*!< work item for handling interrupts */
};


/*! \def to_aim_mil_interface
 *
 * Converts generic AIM USB interface to its enclosing 1553 interface
 * @param interface the interface to convert
 */
#define to_aim_mil_interface(aim_usb_intf) \
    container_of(aim_usb_intf, struct aim_usb_mil_interface, intf)


/*! \enum aim_mil_nl_mc_group_ids
 * Enumeration of all available multicast group IDs
 */
enum aim_mil_nl_mc_group_ids
{
    AIM_MIL_NL_GRP_ID_IRQ_EVENT = 0,    /*!< Group for interrupt events */
    __AIM_MIL_NL_GRP_MAX,
};


/* Netlink operations for the 1553 driver.
 * Empty as it does not receive any messages
 */
static struct genl_ops aim_mil_nl_ops[] = {};


/*! \enum aim_mil_nl_mc_groups
 *
 * Enumeration of all 1553 netlink multicast groups
 */
static struct genl_multicast_group aim_mil_nl_mc_groups[__AIM_MIL_NL_GRP_MAX] =
{
    [AIM_MIL_NL_GRP_ID_IRQ_EVENT] = {.name = AIM_MIL_NL_IRQ_EVENT_GROUP}, /*!< Group for sending interrupt event notifications */
};


/*! \brief Creates an 1553 USB interface
 *
 * @param interface Linux kernel USB interface that will be used by the created interface
 * @param id the device id of the interface
 * @return the generic AIM USB interface part of the created interface on success. ERR_PTR on failure
 */
static struct aim_usb_interface* aim_usb_mil_intf_create(struct usb_interface* interface, const struct usb_device_id* id)
{
    struct aim_usb_mil_interface* mil_intf;

    mil_intf = kzalloc(sizeof(struct aim_usb_mil_interface), GFP_KERNEL);
    if(!mil_intf)
    {
        dev_err(&interface->dev, "Failed to allocate device");
        return ERR_PTR(-ENOMEM);
    }

    return &mil_intf->intf;
}


/*! \brief Deletes an 1553 USB interface
 *
 * @param intf Generic interface part of the 1553 interface to free
 */
static void aim_usb_interface_mil_free(struct aim_usb_interface* intf)
{
    struct aim_usb_mil_interface* mil_intf;

    mil_intf = to_aim_mil_interface(intf);

    kfree(mil_intf);
}


/*! \brief Interrupt endpoint completion handler for ASC1553
 *
 * @param ep endpoint that issued interrupt
 * @param buffer buffer that contains interrupt data
 * @param len length of interrupt data in bytes
 */
static __inline__ void mil_ays_irq_completion(struct aim_usb_int_ep* ep, void* buffer, size_t len)
{
    BUG_ON(!ep || !buffer);

    if(aim_usb_nl_mc_send_sba(ep->intf, AIM_MIL_NL_GRP_ID_IRQ_EVENT, AIM_MIL_NL_OP_LOGLIST_EVENT,
                              AIM_MIL_NL_A_LOGLIST_ENTRY, len, buffer))
    {
        aim_usb_intf_err(ep->intf, "Failed to send interrupt loglist entry");
    }
}


/*! \brief Handles interrupts for APU devices
 *
 * @param ep the interrupt endpoint the interrupt was received on
 * @param buffer the URB buffer holding interrupt data
 * @param len length of interrupt data in buffer
 */
static __inline__ void mil_apu_irq_completion(struct aim_usb_int_ep* ep, void* buffer, size_t len)
{
    union nc_irqstat1_reg irq_status;
    struct aim_usb_mil_interface* mil_intf;

    BUG_ON(!ep || !buffer);

    /* We expect to get the IRQSTAT1 Netchip register with 4 bytes here */
    if(len != ep->buffer_len)
    {
        aim_usb_intf_err(ep->intf, "Received interrupt with wrong data length");
        return;
    }

    irq_status.value = le32_to_cpu(*(__u32*) buffer);

    aim_usb_intf_dbg(ep->intf, "Netchip interrupt with status 0x%x", irq_status.value);

    /* When PCI INT A interrupt was raised, we expect it to be a BIU interrupt
     * and schedule a work item to process it */
    if(irq_status.fields.pci_int_a_irq)
    {
        aim_usb_intf_dbg(ep->intf, "Scheduling work item");
        mil_intf = to_aim_mil_interface(ep->intf);
        schedule_work(&mil_intf->irq_work_item);
    }
}


/*! \brief Interrupt endpoint completion handler for ASC1553
 *
 * @param ep endpoint that issued interrupt
 * @param buffer buffer that contains interrupt data
 * @param len length of interrupt data in bytes
 */
static void mil_irq_completion(struct aim_usb_int_ep* ep, void* buffer, size_t len)
{
    BUG_ON(!ep || !buffer);

    aim_usb_intf_dbg(ep->intf, "IRQ with len %zu", len);

    switch(ep->intf->platform)
    {
        case AI_DEVICE_AYS_ASP:
        case AI_DEVICE_ZYNQMP_ASP:
            mil_ays_irq_completion(ep, buffer, len);
            break;

        case AI_DEVICE_USB:
            mil_apu_irq_completion(ep, buffer, len);
            break;

        default:
            aim_usb_intf_err(ep->intf, "No interrupt endpoint completion available");
            break;
    }
}


/*! \brief Removes one entry from a device's interrupt loglist
 *
 * The function expects the interrupt loglist to be located in mapped kernel memory,
 * no on-board
 * @param mil_intf the 1553 USB interface to get loglist entry of
 * @param entry if entry available, it will be stored here
 * @return true, if entry was retrieved, false if loglist was empty
 */
static __inline__ bool get_loglist_entry(struct aim_usb_mil_interface* mil_intf, struct ty_api_intr_loglist_entry * entry)
{
    AiUInt32 get_count;
    AiUInt32 put_count;

    BUG_ON(!mil_intf || !mil_intf->irq_loglist);

    get_count = mil_intf->irq_loglist->get_cnt;
    put_count = mil_intf->irq_loglist->put_cnt;

    aim_usb_intf_dbg(&mil_intf->intf, "Interrupt log list put %d, get %d\n", put_count, get_count);

    if(get_count != put_count)
    {
        memcpy(entry, &mil_intf->irq_loglist->entry[get_count], sizeof(struct ty_api_intr_loglist_entry));
        API_INC_LOLI_CNT(get_count);
        mil_intf->irq_loglist->get_cnt = get_count;
        return true;
    }

    return false;
}


/*! \brief Forwards all interrupt currently in device's loglist to user space
 *
 * @param mil_intf the 1553 USB interface to forward interrupts of
 */
static void forward_interrupts(struct aim_usb_mil_interface* mil_intf)
{
    struct ty_api_intr_loglist_entry entry;
    bool entries_left;

    BUG_ON(!mil_intf);

    aim_usb_intf_dbg(&mil_intf->intf, "Forwarding interrupts to user space");

    do
    {
        entries_left = get_loglist_entry(mil_intf, &entry);

        if(entries_left)
        {
            if(aim_usb_nl_mc_send_sba(&mil_intf->intf, AIM_MIL_NL_GRP_ID_IRQ_EVENT, AIM_MIL_NL_OP_LOGLIST_EVENT,
                                      AIM_MIL_NL_A_LOGLIST_ENTRY, sizeof(entry), &entry))
            {
                aim_usb_intf_err(&mil_intf->intf, "Failed to send interrupt loglist entry");
            }
        }
    }while(entries_left);

}


/*! \brief Interrupt handler function for APU devices, called in work item context
 *
 * @param mil_intf The 1553 USB interface to handle interrupts for
 */
static void mil_apu_irq_work(struct aim_usb_mil_interface* mil_intf)
{
    union nc_irqstat1_reg irq_status;
    int event = 0;
    int err;

    BUG_ON(!mil_intf);

    aim_usb_intf_dbg(&mil_intf->intf, "Processing APU IRQ work");

    do
    {
        aim_usb_apu_pci_int_ack(&mil_intf->intf, &event);

        /* We assume all PCI INT A interrupts raised by Netchip */

        if( event & 0x1 )
            api_ir(&mil_intf->tsw, BIU_1 - 1);

        if( event & 0x2 )
            api_ir(&mil_intf->tsw, BIU_2 - 1);

        err = aim_usb_ncc_reg_read(&mil_intf->intf.ncc, NC_IRQSTAT1_REG_ADDR, &irq_status.value);
        if(err)
        {
            aim_usb_intf_err(&mil_intf->intf, "Failed to read Netchip IRQ status");
            return;
        }

    }while(irq_status.fields.pci_int_a_irq);

    forward_interrupts(mil_intf);
}


/*! \brief Work item handler function for interrupts
 *
 * @param work the work item scheduled for handling interrupts
 */
static void mil_irq_work(struct work_struct* work)
{
    struct aim_usb_mil_interface* mil_intf;

    BUG_ON(!work);

    mil_intf = container_of(work, struct aim_usb_mil_interface, irq_work_item);

    aim_usb_intf_dbg(&mil_intf->intf, "Processing IRQ work");

    switch(mil_intf->intf.platform)
    {
        case AI_DEVICE_USB:
            return mil_apu_irq_work(mil_intf);

        default:
            aim_usb_intf_err(&mil_intf->intf, "No IRQ work for this platform");
            return;
    }
}


/*! \brief Initialization handler for 1553 USB devices
 *
 * @param intf the interface to initialize
 * @return 0 on success, negative errno code on failure
 */
static int aim_usb_interface_mil_init(struct aim_usb_interface* intf)
{
    struct aim_usb_nl_setup* nl_setup;
    struct aim_usb_mil_interface* mil_intf;

    BUG_ON(!intf);

    mil_intf = to_aim_mil_interface(intf);

    aim_usb_intf_dbg(intf, "Initializing 1553 USB interface");

    nl_setup = &intf->nl_setup;

    nl_setup->family.version = AIM_MIL_NL_VERSION;
    nl_setup->family.maxattr = AIM_MIL_NL_A_MAX;

    nl_setup->ops = aim_mil_nl_ops;
    nl_setup->num_ops = ARRAY_SIZE(aim_mil_nl_ops);

    nl_setup->groups = aim_mil_nl_mc_groups;
    nl_setup->num_groups = ARRAY_SIZE(aim_mil_nl_mc_groups);

    INIT_WORK(&mil_intf->irq_work_item, mil_irq_work);

    if(intf->interrupt_ep)
    {
        aim_usb_int_ep_set_completion(intf->interrupt_ep, mil_irq_completion);
    }

    return 0;
}


/*! \brief Handles target commands on ASC1553 boards
 *
 * @param intf the generic USB interface the target command is issued for
 * @param cmd the target command
 * @param len length of target command in bytes
 * @param response buffer where response will be stored
 * @param resp_len maximum length of response in bytes
 * @return length of response on success, negative errno code on failure
 */
static ssize_t ays_mil_target_command(struct aim_usb_interface* intf, void* cmd, size_t len,
                                  void* response, size_t resp_len)
{
    BUG_ON(!intf || !cmd);

    aim_usb_intf_dbg(intf, "ASC1553 target command");

    return aim_usb_ays_target_command(intf, cmd, len, response, resp_len);
}


/*! \brief Initializes internal target software library for devices without ASP
 *
 * @param mil_intf the 1553 interface to initialize target software of
 * @return 0 on success, errno code on failure
 */
static int initialize_target_software(struct aim_usb_mil_interface* mil_intf)
{
    TY_API_HW_INIT_PARAMS tsw_settings;

    memset( &tsw_settings, 0, sizeof(tsw_settings) );

    BUG_ON(!mil_intf);

    aim_usb_intf_info(&mil_intf->intf, "Initializing 1553 target software");
    aim_usb_intf_info(&mil_intf->intf, MIL_TSW_VERSION_STRING_FULL);
    aim_usb_intf_info(&mil_intf->intf, MIL_TSW_BUILD_DATE);

    BUG_ON(mil_intf->intf.shared_memory.base);

    mil_intf->intf.shared_memory.base = vzalloc(SHARED_MEM_SIZE);

    if(!mil_intf->intf.shared_memory.base)
    {
        aim_usb_intf_err(&mil_intf->intf, "Failed to allocate shared memory");
        return -ENOMEM;
    }

    mil_intf->intf.shared_memory.size = SHARED_MEM_SIZE;

    mil_intf->irq_loglist = mil_intf->intf.shared_memory.base + 0x00008000L;


    tsw_settings.mon_data        = NULL;
    tsw_settings.pxMyMonContext  = NULL;
    tsw_settings.pDevice         = &mil_intf->intf;
    tsw_settings.pDeviceContext  = &mil_intf->intf;

    tsw_settings.SharedRAMBase   = mil_intf->intf.shared_memory.base;
    tsw_settings.GlobalRAMBase   = mil_intf->intf.global_memory.base;
    tsw_settings.GlobalRAMBase2  = NULL;
    tsw_settings.IORAMBase       = mil_intf->intf.io_memory.base;
    tsw_settings.SharedRAMSize   = mil_intf->intf.shared_memory.size;
    tsw_settings.GlobalRAMSize   = mil_intf->intf.global_memory.size;
    tsw_settings.GlobalRAMSize2  = 0;

    tsw_settings.PlatformGroup  = AI_DEVICE_USB;

    tsw_settings.BoardSerial    = mil_intf->intf.serial;
    tsw_settings.BoardConfig    = mil_intf->intf.board_config;
    tsw_settings.BoardType      = mil_intf->intf.board_type;
    tsw_settings.BoardSubType   = mil_intf->intf.board_sub_type;
    tsw_settings.BoardPartNo    = mil_intf->intf.board_part_no;
    tsw_settings.CPUClk         = 1; /* N/A for APU */;
    tsw_settings.SYSClk         = 1; /* N/A for APU */;
    tsw_settings.BoardAppType   = 0; /* N/A for APU */;
    tsw_settings.HwVariant      = mil_intf->intf.hw_variant;

    mil_hw_initialize_tsw(&mil_intf->tsw, &tsw_settings );

    return 0;
}


/*! \brief 1553 specific interface start function
 *
 * @param intf the 1553 interface to start
 * @return 0 on success, errno code on failure
 */
static int aim_usb_interface_mil_start(struct aim_usb_interface* intf)
{
    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Starting 1553 interface");

    if(!intf->has_asp)
    {
        return initialize_target_software(to_aim_mil_interface(intf));
    }

    return 0;
}


/*! \brief 1553 specific device stop function
 *
 * @param intf the 1553 interface to stop
 */
static void aim_usb_interface_mil_stop(struct aim_usb_interface* intf)
{
    struct aim_usb_mil_interface * mil_intf = NULL;

    BUG_ON(!intf);

    mil_intf = to_aim_mil_interface(intf);

    aim_usb_intf_dbg(intf, "Stopping 1553 device");

    if(!intf->has_asp && intf->shared_memory.base)
    {
        mil_hw_shutdown_tsw(&mil_intf->tsw);

        aim_usb_intf_dbg(intf, "Freeing shared memory");
        vfree(intf->shared_memory.base);
        intf->shared_memory .base = NULL;
        intf->shared_memory.size = 0;
    }
}





static struct aim_usb_interface_ops aim_mil_usb_device_ops = {
        .create = aim_usb_mil_intf_create,
        .init = aim_usb_interface_mil_init,
        .free = aim_usb_interface_mil_free,
        .start = aim_usb_interface_mil_start,
        .stop = aim_usb_interface_mil_stop,
};


static struct aim_usb_ioctl_ops asc1553_ioctl_ops = {
        .target_command = ays_mil_target_command,
};




/*! ASC1553 specific device data */
static const struct aim_usb_interface_data asc1553_device_data = {
        .name = "ASC1553",
        .platform = AI_DEVICE_AYS_ASP,
        .intf_ops = &aim_mil_usb_device_ops,
        .ioctl_ops = &asc1553_ioctl_ops,
        .protocol = PROTOCOL_1553,
};




/*! ASC1553-Gen2 specific device data */
static const struct aim_usb_interface_data asc1553_gen2_device_data = {
        .name = "ASC1553-Gen2",
        .platform = AI_DEVICE_ZYNQMP_ASP,
        .intf_ops = &aim_mil_usb_device_ops,
        .ioctl_ops = &asc1553_ioctl_ops,
        .protocol = PROTOCOL_1553,
};



/** USB device ID's that are handled by the 1553 USB driver */
const struct usb_device_id aim_usb_device_table[] = {
        {USB_DEVICE(0x1633, 0x4510), .driver_info = (kernel_ulong_t) &asc1553_device_data},
        {USB_DEVICE(0x1633, 0x5710), .driver_info = (kernel_ulong_t) &asc1553_gen2_device_data},
        {},
};


MODULE_DEVICE_TABLE(usb, aim_usb_device_table);
