/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS1553_CommonSettings.h
 *
 *  This header file contains all settings for
 *  the MIL implementation of ANS that are used by
 *  the server, as well as the client application
 *
 */

#ifndef ANS1553_COMMONSETTINGS_H_
#define ANS1553_COMMONSETTINGS_H_


#include "ANS_Protocol.h"




/*!
 * This is the AIM1553 Magic Value. Actually 0x09B0h is the morse code of AIMANS
 * whereas a short mark ("dot" or "dit") is binary encoded as 0 and a long mark
 * ("dash" or "dah") is binary encoded as 1. To determine that this ANS Version
 * implements socket communication for MIL-STD-1553/STANAG3910 "0x1553" is prefixed in hex.
 */
#define ANS1553_MAGIC_VALUE 0x155309B0


/*! \def ANS1553_DEFAULT_PORT
* default port number for the ANSMIL Server. \n
* This port number is not registered with the IANA and may be
* in use on some systems.
*/
#define ANS1553_DEFAULT_PORT 1553


/*! \def ANS1553_DEFAULT_CLIENT_PORT
* default port number for the ANSMIL Client (UDP Broadcasts). \n
* This port number is not registered with the IANA and may be
* in use on some systems.
*/
#define ANS1553_DEFAULT_CLIENT_PORT 11553




/*! Protocol version the ANS1553 protocol stack uses */
static struct AnsProtocolVersion g_Ans1553ProtocolVersion = {
        2, /* majorVersion */
        2, /* minorVersion */
};





#endif /* ANS1553_COMMONSETTINGS_H_ */
