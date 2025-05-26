/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_nl.h
 *
 *  This file contains declarations used for creating
 *  netlink interfaces to user space
 *
 */

#ifndef AIM_USB_NL_H_
#define AIM_USB_NL_H_


#include <net/genetlink.h>




/* Forward declaration */
struct aim_usb_interface;




/*! \struct aim_usb_nl_setup
 *
 * This structure holds the netlink user space interface
 * for an AIM USB interface.
 */
struct aim_usb_nl_setup
{
    struct genl_family family;              /*!< The generic netlink family of the device */
    struct genl_ops* ops;                   /*!< Pointer to array of generic netlink operations of the device */
    size_t num_ops;                         /*!< number of entries \ref ops holds */
    struct genl_multicast_group* groups;    /*!< Pointer to array of generic netlink multicast groups of the device */
    size_t num_groups;                      /*!< number of entries \ref groups holds */
};




/*! \brief Sets up netlink interface for a USB interface
 *
 * This function registers the netlink family, operations
 * and multicast groups for an AIM USB interface according to
 * the netlink set-up stored with the interface.
 * @param intf the AIM USB interface to set up netlink interface for
 * @return returns 0 on success, a negative errno code otherwise
 */
extern int aim_usb_nl_setup(struct aim_usb_interface* intf);


/*! \brief Releases netlink interface of an USB interface
 *
 * This function unregisters the netlink interface of an
 * AIM USB interface.
 * @param intf the device to unregister netlink interface of
 */
extern void aim_usb_nl_release(struct aim_usb_interface* intf);


/*! \brief Sends single binary attribute over NL multicast group
 *
 *  This function creates a generic netlink message that consists of
 *  one single binary attribute.
 *  The created message is sent to the specified multicast group.
 * @param intf the USB interface to send the netlink message for
 * @param family_group the multicast group ID to send message to
 * @param command the command ID to use for the message
 * @param attr the attribute ID to use
 * @param attrSize the size of the binary attribute in bytes
 * @param attrData pointer to the binary attribute to include in the message.
 * @return
 */
extern int aim_usb_nl_mc_send_sba(struct aim_usb_interface* intf, int family_group, int command, int attr,
                                  size_t attrSize, void* attrData);




#endif /* AIM_USB_NL_H_ */
