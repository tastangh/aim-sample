/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_com_channel.h
 *
 */

#ifndef AIM_USB_COM_CHANNEL_H_
#define AIM_USB_COM_CHANNEL_H_


#include "aim_usb_interface.h"




/*! \struct aim_usb_com_channel
 *
 * This structure comprises one bulk out and one bulk in
 * endpoint to a generic USB communication channel
 */
struct aim_usb_com_channel
{
    struct aim_usb_interface* intf;         /*!< The AIM USB interface the channel belongs to */
    struct usb_host_endpoint* cmd_ep;       /*!< The bulk out endpoint for issuing commands to the interface */
    struct usb_host_endpoint* response_ep;  /*!< The bulk in endpoint for receiving responses from the interface */

    struct mutex lock;                      /*!< Mutex for synchronizing access to the channel */
    int timeout_ms;                         /*!< Time-out in milliseconds for send/receive operations */
    size_t max_transfer_length;             /*!< The maximum transfer length of the end points */
};


/*! \brief Locks access to an USB communication channel
 *
 * @param channel The channel to lock
 */
static __inline__ void aim_usb_com_channel_lock(struct aim_usb_com_channel* channel)
{
    mutex_lock(&channel->lock);
}


/*! \brief Unlocks an USB communication channel
 *
 * @param channel The channel to unlock
 */
static __inline__ void aim_usb_com_channel_unlock(struct aim_usb_com_channel* channel)
{
    mutex_unlock(&channel->lock);
}


/*! \brief Sets timeout for USB communication channel
 *
 * @param channel the channel to set timeout for
 * @param timeout time-out for all send/receive operations in milliseconds
 */
static __inline__ void aim_usb_com_channel_set_timeout(struct aim_usb_com_channel* channel, int timeout)
{
    channel->timeout_ms = timeout;
}


/*! \brief Initialization function for USB communication channel
 *
 * @param channel the channel to initialize
 * @param intf the AIM USB interface the channel is attached to
 * @param cmd_ep the endpoint to use for sending commands
 * @param response_ep the endpount to use for receiving commands
 * @return 0 on success, errno on failure
 */
extern int aim_usb_com_channel_init(struct aim_usb_com_channel* channel, struct aim_usb_interface* intf,
                                    struct usb_host_endpoint* cmd_ep, struct usb_host_endpoint* response_ep);


/*! \brief Send command over a USB communication channel
 *
 * @param channel the channel to send command on
 * @param buffer buffer that holds the command to send
 * @param len size of command in bytes
 * @return 0 on success, errno on failure
 */
extern int aim_usb_com_channel_send(struct aim_usb_com_channel* channel, void* buffer, size_t len);


/*! \brief Receive response over a USB communication channel
 *
 * @param channel the channel to receive response on
 * @param buffer the buffer t store response in
 * @param len number of bytes the response size is expected.
 * @return number of bytes received on success, negative errno on failure
 */
extern ssize_t aim_usb_com_channel_receive(struct aim_usb_com_channel* channel, void* buffer, size_t len);


/*! \brief Issues a command/response sequence on an USB communication channel
 *
 * @param channel The channel to issue command on
 * @param cmd the command to send
 * @param cmd_len length of command in bytes
 * @param response buffer where response will be stored
 * @param response_len size of response buffer in bytes
 * @return number of bytes received as response on success. Negative errno code on failure
 */
extern ssize_t aim_usb_com_channel_issue_cmd(struct aim_usb_com_channel* channel, void* cmd, size_t cmd_len,
                                             void* response, size_t response_len);






#endif /* AIM_USB_COM_CHANNEL_H_ */
