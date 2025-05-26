/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_com_channel.c
 *
 */


#include "aim_usb_com_channel.h"




/*! \def AIM_USB_COM_CHANNEL_DEFAULT_TIMEOUT_MS
 *
 * Default timeout in milliseconds for send/receive operations on USB communication channels
 */
#define AIM_USB_COM_CHANNEL_DEFAULT_TIMEOUT_MS  5000




int aim_usb_com_channel_init(struct aim_usb_com_channel* channel, struct aim_usb_interface* intf,
                             struct usb_host_endpoint* cmd_ep, struct usb_host_endpoint* response_ep)
{
    if(!usb_endpoint_is_bulk_out(&cmd_ep->desc))
    {
        aim_usb_intf_err(channel->intf, "Invalid command endpoint");
        return -EINVAL;
    }

    if(!usb_endpoint_is_bulk_in(&response_ep->desc))
    {
        aim_usb_intf_err(channel->intf, "Invalid response endpoint");
        return -EINVAL;
    }

    channel->intf = intf;
    channel->cmd_ep = cmd_ep;
    channel->response_ep = response_ep;
    channel->timeout_ms = AIM_USB_COM_CHANNEL_DEFAULT_TIMEOUT_MS;

    mutex_init(&channel->lock);

    return 0;
}

int aim_usb_com_channel_send(struct aim_usb_com_channel* channel, void* buffer, size_t len)
{
    struct usb_device* usb_dev;
    int actual_len;
    int err;

    BUG_ON(!channel || !channel->cmd_ep);

    aim_usb_intf_dbg(channel->intf, "Sending %zu bytes on com channel with Endpoint %d", len,
                     usb_endpoint_num(&channel->cmd_ep->desc));


    usb_dev = interface_to_usbdev(channel->intf->interface);

    err = usb_bulk_msg(usb_dev, usb_sndbulkpipe(usb_dev, channel->cmd_ep->desc.bEndpointAddress), buffer, len,
                       &actual_len, channel->timeout_ms);
    if(err)
    {
        aim_usb_intf_err(channel->intf, "Failed to send on com channel and send %d bytes", actual_len);
        return err;
    }
    aim_usb_intf_dbg(channel->intf, "Send a cmd with %d", actual_len);
    /* Send a zero length package when transfer length is an integral multiple
     * of the Endpoint's maximum packet size. If the transfer length is equal to the
     * maximum transfer size however no ZLP is required.
     */
    if((len % channel->cmd_ep->desc.wMaxPacketSize == 0) && (len != channel->max_transfer_length))
    {
        aim_usb_intf_dbg(channel->intf, "Sending ZLP on com channel with Endpoint %d",
                         usb_endpoint_num(&channel->cmd_ep->desc));
        err = usb_bulk_msg(usb_dev, usb_sndbulkpipe(usb_dev, channel->cmd_ep->desc.bEndpointAddress), buffer, 0,
                           NULL, channel->timeout_ms);
        if(err)
        {
            aim_usb_intf_err(channel->intf, "Failed to send ZLP on com channel");
            return err;
        }
    }

    return actual_len == len ? 0 : -EAGAIN;
}


ssize_t aim_usb_com_channel_receive(struct aim_usb_com_channel* channel, void* buffer, size_t len)
{
    struct usb_device* usb_dev;
    int actual_len;
    int err;

    BUG_ON(!channel || !channel->response_ep);

    usb_dev = interface_to_usbdev(channel->intf->interface);

    aim_usb_intf_dbg(channel->intf, "Receiving %zu bytes on com channel with Endpoint %d", len,
                     usb_endpoint_num(&channel->response_ep->desc));

    usb_dev = interface_to_usbdev(channel->intf->interface);

    err = usb_bulk_msg(usb_dev, usb_rcvbulkpipe(usb_dev, channel->response_ep->desc.bEndpointAddress), buffer, len,
                       &actual_len, channel->timeout_ms);
    if(err)
    {
        aim_usb_intf_err(channel->intf, "Failed to receive on com channel");
        return err;
    }

    return actual_len;
}

ssize_t aim_usb_com_channel_issue_cmd(struct aim_usb_com_channel* channel, void* cmd, size_t cmd_len,
                                      void* response, size_t resp_len)
{
    int err;
    ssize_t bytes_received;
    BUG_ON(!channel || !cmd);

    aim_usb_com_channel_lock(channel);

    err = 0;
    bytes_received = 0;
    do
    {
        err = aim_usb_com_channel_send(channel, cmd, cmd_len);
        if(err)
        {
            aim_usb_intf_err(channel->intf, "Failed to send command");
            break;
        }

        if(!resp_len)
        {
            break;
        }

        bytes_received = aim_usb_com_channel_receive(channel, response, resp_len);
        if(bytes_received < 0)
        {
            aim_usb_intf_err(channel->intf, "Failed to receive response");
            err = bytes_received;
            break;
        }
    }while(0);

    aim_usb_com_channel_unlock(channel);

    return err ? err : bytes_received;
}

