/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_interrupt.c
 *
 */


#include <linux/usb.h>
#include "aim_usb.h"
#include "aim_usb_interrupt.h"
#include "aim_usb_debug.h"
#include "aim_usb_interface.h"




/*! \brief URB completion handler for interrupt endpoints
 *
 *  Will call the interrupt endpoints completion handler
 *  and re-submit the URB.
 * @param urb The URB that was completed.
 */
static void aim_usb_int_ep_complete(struct urb* urb)
{
    struct aim_usb_int_ep* int_ep;
    int err;

    int_ep = urb->context;

    aim_usb_intf_dbg(int_ep->intf, "Interrupt URB on Endpoint 0x%x completed", int_ep->ep->desc.bEndpointAddress);
    if(urb->status == 0)
    {
        /* Check if handler function is installed for the endpoint and call it if this is the case */
        if(int_ep->completion_handler)
        {
            int_ep->completion_handler(int_ep, urb->transfer_buffer, urb->actual_length);
        }

        aim_usb_intf_dbg(int_ep->intf, "Interrupt URB successfully completed. Re-submitting it");

        /* Re-submit URB for next interrupt event */
        err = usb_submit_urb(urb, GFP_ATOMIC);
        if(err)
        {
            aim_usb_intf_dbg(int_ep->intf, "Failed to resubmit interrupt URB with err %d", err);
        }
    }
    else
    {
        aim_usb_intf_dbg(int_ep->intf, "Interrupt URB failed with status %d", urb->status);
    }
}




int aim_usb_int_ep_init(struct aim_usb_int_ep* int_ep, struct aim_usb_interface* intf, struct usb_host_endpoint* ep)
{
    int err;

    BUG_ON(!int_ep || !intf || !ep);

    aim_usb_intf_dbg(intf, "Setting up Interrupt Endpoint at address 0x%x", ep->desc.bEndpointAddress);

    int_ep->intf = intf;
    int_ep->ep = ep;
    int_ep->urb = NULL;
    int_ep->urb_buffer = NULL;
    int_ep->completion_handler = NULL;

    if(!usb_endpoint_is_int_in(&ep->desc))
    {
        aim_usb_intf_err(int_ep->intf, "Invalid endpoint type for interrupt endpoint");
        return -EINVAL;
    }

    err = 0;
    do
    {
        int_ep->urb = usb_alloc_urb(0, GFP_KERNEL);
        if(!int_ep->urb)
        {
            aim_usb_intf_err(int_ep->intf, "Failed to allocate URB for interrupt endpoint");
            err = -ENOMEM;
            break;
        }

        int_ep->buffer_len = usb_endpoint_maxp(&ep->desc);

        aim_usb_intf_dbg(intf, "Interrupt Endpoint buffer size is %zu", int_ep->buffer_len);

        int_ep->urb_buffer = kmalloc(int_ep->buffer_len, GFP_KERNEL);

        if(!int_ep->urb_buffer)
        {
            aim_usb_intf_err(int_ep->intf, "Failed to allocate URB buffer for interrupt endpoint");
            err = -ENOMEM;
            break;
        }
    }while(0);

    if(err)
    {
        if(int_ep->urb)
        {
            usb_free_urb(int_ep->urb);
        }

        if(int_ep->urb_buffer)
        {
            kfree(int_ep->urb_buffer);
        }
    }

    return err;
}


void aim_usb_int_ep_free(struct aim_usb_int_ep* int_ep)
{
    BUG_ON(!int_ep);

    aim_usb_intf_dbg(int_ep->intf, "Freeing interrupt endpoint");

    if(int_ep->urb)
    {
        usb_free_urb(int_ep->urb);
    }

    if(int_ep->urb_buffer)
    {
        kfree(int_ep->urb_buffer);
    }
}


int aim_usb_int_ep_start(struct aim_usb_int_ep* int_ep)
{
    struct usb_device* usb_dev;

    BUG_ON(!int_ep || !int_ep->urb || !int_ep->urb_buffer || !int_ep->intf);

    aim_usb_intf_dbg(int_ep->intf, "Starting interrupt endpoint at address: 0x%x", int_ep->ep->desc.bEndpointAddress);

    usb_dev = interface_to_usbdev(int_ep->intf->interface);

    usb_fill_int_urb(int_ep->urb, usb_dev, usb_rcvintpipe(usb_dev, int_ep->ep->desc.bEndpointAddress), int_ep->urb_buffer,
                     int_ep->buffer_len, aim_usb_int_ep_complete, int_ep, int_ep->ep->desc.bInterval);

    return usb_submit_urb(int_ep->urb, GFP_KERNEL);
}


void aim_usb_int_ep_stop(struct aim_usb_int_ep* int_ep)
{
    BUG_ON(!int_ep || !int_ep->urb || !int_ep->intf);

    aim_usb_intf_dbg(int_ep->intf, "Stopping interrupt endpoint at address: 0x%x", int_ep->ep->desc.bEndpointAddress);

    usb_kill_urb(int_ep->urb);
}


void aim_usb_int_ep_set_completion(struct aim_usb_int_ep* int_ep, aim_usb_int_ep_completion completion)
{
    aim_usb_intf_dbg(int_ep->intf, "Installed interrupt completion handler");
    int_ep->completion_handler = completion;
}


