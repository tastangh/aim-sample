/* SPDX-FileCopyrightText: 2015-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_nl.h
 *
 *  This file contains declarations used for creating
 *  netlink interfaces to user space
 *
 */

#ifndef AIM_NL_H_
#define AIM_NL_H_


#include <linux/types.h>
#include <net/genetlink.h>




/* Forward declaration */
struct aim_pci_device;




/*! \struct aim_nl_setup
 *
 * This structure holds the netlink user space interface
 * for an AIM PCI device.
 */
struct aim_nl_setup
{
    struct genl_family family;              /*!< The generic netlink family of the device */
    struct genl_ops* ops;                   /*!< Pointer to array of generic netlink operations of the device */
    size_t num_ops;                         /*!< number of entries \ref ops holds */
    struct genl_multicast_group* groups;    /*!< Pointer to array of generic netlink multicast groups of the device */
    size_t num_groups;                      /*!< number of entries \ref groups holds */
};




/*! \brief Sets up netlink interface for a device
 *
 * This function registers the netlink family, operations
 * and multicast groups for an AIM PCI device according to
 * the netlink set-up stored with the device.
 * @param aimDevice the device to set up netlink interface for
 * @return returns 0 on success, a negative errno code otherwise
 */
int aim_nl_dev_setup(struct aim_pci_device* aimDevice);


/*! \brief Releases netlink interface for a device
 *
 * This function unregisters the netlink interface of an
 * AIM PCI device.
 * @param aimDevice the device to unregister netlink interface of
 */
void aim_nl_dev_release(struct aim_pci_device* aimDevice);


/*! \brief Sends single binary attribute over NL multicast group
 *
 *  This function creates a generic netlink message that consists of
 *  one single binary attribute.
 *  The created message is sent to the specified multicast group.
 * @param aimDevice the device to send the netlink message for
 * @param family_group the multicast group ID (relative to the device family) to send message to
 * @param command the command ID to use for the message
 * @param attr the attribute ID to use
 * @param attrSize the size of the binary attribute in bytes
 * @param attrData pointer to the binary attrbute to include in the message.
 * @return
 */
int aim_nl_mc_send_sba(struct aim_pci_device* aimDevice, int family_group, int command, int attr,
                       size_t attrSize, void* attrData);




#endif /* AIM_NL_H_ */
