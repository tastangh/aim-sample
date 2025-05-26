/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_LinkInitFrame.h
 *
 * This file contains the definition of the ANS link initialization frame.
 *
 *
 ******************************************************************************/
#ifndef ANS_LINKINITFRAME_H
#define ANS_LINKINITFRAME_H

/* Includes *******************************************************************/

#include "Ai_cdef.h"
#include "ANS_Types.h"
#include "Ai_Socket.h"
#include "ANS_ServerPeer.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/


/**
 * Definition of a link initialization frame. This frame is the first
 * frame sent from client to ANS. It specifies the details of the
 * freshly opened ANS channel.
 */
#pragma pack(1)
typedef struct tagLinkInitFrame_t
{
    AiUInt32 magicValue;                        /*!< Protocol specific magic value (see g_MagicValue) */
    struct AnsProtocolVersion protocolVersion;  /*!< Version of the ANS protocol used for link */
    AnsLinkType linkType;                       /*!< see ANSLinkType          		*/
    AnsServerPeerId peerId;                     /*!< peer ID */

}  AnsLinkInitFrame;
#pragma pack()


/**
 * Definition of a link initialization response frame.
 */
#pragma pack(1)
typedef struct tagAnsLinkResponseFrame_t
{
    AiUInt32 magicValue;                        /*!< see Protocol specific magic value (see g_MagicValue)       */
    struct AnsProtocolVersion protocolVersion;  /*!< Version of the ANS protocol used for link         */
    AnsLinkType linkType;                       /*!< see ANSLinkType            */
    AnsServerPeerId peerId;                     /*!< client ID (see ANSClientId)    */
    AiUInt32 statusWord;                        /*!< see ANSStatus              */
} AnsLinkResponseFrame;
#pragma pack()


/* Prototypes *****************************************************************/

extern AiBoolean ANS_LinkInitFrame_read (
    struct AnsConnection* connection,
    unsigned long const timeOutMs,
    AnsLinkInitFrame   *pFrame);

extern AiBoolean ANS_LinkInitFrame_write (
    struct AnsConnection* connection,
    unsigned long const timeOutMs,
    AnsLinkType const  linkType,
    struct AnsProtocolVersion const protocolVersion,
    AnsServerPeerId const  clientId);

extern AiBoolean ANS_LinkResponseFrame_send (
    AnsLinkType const  linkType,
    AnsServerPeerId const   clientId,
    AnsStatus const    ansStatus,
    AiUInt32 const      numBoards,
    struct AnsConnection* connection);

extern AiBoolean ANS_LinkResponseFrame_read (
    struct AnsConnection* connection,
    unsigned long const     timeOutMs,
    AnsLinkResponseFrame   *pFrame);





#endif  /* ANS_LINKINITFRAME_H */
