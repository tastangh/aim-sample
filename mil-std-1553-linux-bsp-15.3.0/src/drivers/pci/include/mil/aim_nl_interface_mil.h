/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file aim_nl_interface_mil.h
 *
 *  This header file contains the 1553 specific
 *  netlink interface definitions that can be used
 *  by user space
 *
 */

#ifndef AIM_NL_INTERFACE_MIL_H_
#define AIM_NL_INTERFACE_MIL_H_


/*! \def AIM_MIL_NL_VERSION
 * Netlink interface version of aim_mil
 */
#define AIM_MIL_NL_VERSION   1


/*! \def AIM_MIL_NL_FAMILY_NAME_FORMAT
 * Define for device specific netlink family \n
 * Consists of the driver name extended by the minor number
 * of the corresponding device.
 */
#define AIM_MIL_NL_FAMILY_NAME_FORMAT "aim_mil%d"


/*! \def AIM_MIL_NL_IRQ_EVENT_GROUP
 * Netlink multicast group for generic interrupt events
 */
#define AIM_MIL_NL_IRQ_EVENT_GROUP    "mil_irq_event"




/*! \enum aim_mil_nl_ops
 * Enumeration of all possible netlink commands
 */
enum aim_mil_nl_ops
{
    AIM_MIL_NL_OP_UNSPEC,         /*!< AIM_MIL_NL_OP_UNSPEC */
    AIM_MIL_NL_OP_LOGLIST_EVENT,  /*!< Command for notification about new interrupt log list entries */
    __AIM_MIL_NL_OP_MAX,          /*!< __AIM_MIL_NL_OP_MAX */
};


/*! \def AIM_MIL_NL_OP_MAX
 * Maximum number of supported netlink commands
 */
#define AIM_MIL_NL_OP_MAX (__AIM_MIL_NL_OP_MAX - 1)


/*! \enum aim_mil_nl_attributes
 * Enumeration of all aim_mil netlink attributes
 */
enum aim_mil_nl_attributes
{
    AIM_MIL_NL_A_UNSPEC,         /*!< AIM_MIL_NL_A_UNSPEC */
    AIM_MIL_NL_A_LOGLIST_ENTRY,  /*!< Attribute for receive events */
    __AIM_MIL_NL_A_MAX,          /*!< __AIM_MIL_NL_A_MAX */
};


/*! \def AIM_MIL_NL_A_MAX
 * Maximum number of supported Attributes
 */
#define AIM_MIL_NL_A_MAX (__AIM_MIL_NL_A_MAX - 1)



#endif /* AIM_NL_INTERFACE_MIL_H_ */
