/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_LinkInitFrame.c
 *
 * This module provides functions related to the ANS Link
 * Initialization frames.
 *
 ******************************************************************************/
#define MODULENAME "ANS_LinkInitFrame"

/* Includes *******************************************************************/

#include <stdlib.h>
#include <string.h>

#include "Ai_Defines.h"
#include "Ai_Socket.h"
#include "Ai_Validate.h"
#include "ANS_Types.h"
#include "ANS_Client.h"
#include "ANS_Log.h"
#include "ANS_LinkInitFrame.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

/* Variables ******************************************************************/

/* Functions ******************************************************************/

/*************************************************************************//**
 * Read the incoming link initialization frame from the open channel.
*****************************************************************************/
extern AiBoolean ANS_LinkInitFrame_read (
    struct AnsConnection* connection,   /*!< [in] connection to read from */
    unsigned long const timeOutMs,  /*!< [in] read timeout [ms] or      */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE      */
    AnsLinkInitFrame   *pFrame)    /*!< [out] frame read on success    */
{
    AnsStatus ret;

    VALIDATE_PTR(connection, AiFalse);
    VALIDATE_PTR(pFrame, AiFalse);

    ret = AnsConnection_receive(connection, (char*) pFrame, sizeof(*pFrame), timeOutMs);
    if (ret != AnsStatus_OK)
    {
       ANSLogError("AnsLinkInitFrame read error (%d)!", (int) ret);

       return AiFalse;         /** \return AiFalse - read error            */
    }

    /* Ensure that the frame type is the expected one.                      */
    if ( pFrame->magicValue != AnsProtocol_getMagic(&g_AnsProtocol) )
    {
        ANSLogError("AnsLinkInitFrame read: Unexpected magic %lX !", (unsigned long) pFrame->magicValue);
        return AiFalse;
    }

    return AiTrue;              /** \return AiTrue - success                */
}

/*************************************************************************//**
 * Write a link initialization frame to the open channel.
*****************************************************************************/
extern AiBoolean ANS_LinkInitFrame_write (
    struct AnsConnection* connection,       /*!< [in] connection to write on */
    unsigned long const timeOutMs,      /*!< [in] write time-out [ms]       */
    AnsLinkType const  linkType,       /*!< [in] link type                 */
    struct AnsProtocolVersion const protocolVersion,
                                        /*!< [in] protocol version          */
    AnsServerPeerId  const  peerId)       /*!< [in] Peer ID                 */
{
    AnsStatus ret;
    AnsLinkInitFrame   frame;

    VALIDATE_PTR(connection, AiFalse);

    (void) memset(&frame, 0, sizeof(frame));
    frame.peerId = peerId;
    frame.linkType = linkType;
    frame.magicValue = AnsProtocol_getMagic(&g_AnsProtocol);
    frame.protocolVersion = protocolVersion;

    ret = AnsConnection_send(connection, (char *) &frame, sizeof(frame));
    if ( ret != AnsStatus_OK )
    {
        ANSLogError(MODULENAME " write: socket write error %d!", (int) ret);
        return AiFalse;
    }

    return AiTrue;
}


/*************************************************************************//**
 * Send a link initialization response frame with the specified data.
*****************************************************************************/
extern AiBoolean ANS_LinkResponseFrame_send (
    AnsLinkType const  linkType,       /*!< [in] ANS link type        */
    AnsServerPeerId const  clientId,       /*!< [in] Client ID             */
    AnsStatus const    ansStatus,      /*!< [in] ANS status word      */
    AiUInt32 const       numBoards,     /*!< [in] number of boards installed */
    struct AnsConnection* connection)              /*!< [in] open socket handle    */
{
    AnsLinkResponseFrame frame;
    AnsStatus ret;

    (void) memset(&frame, 0, sizeof(frame));
    frame.magicValue = AnsProtocol_getMagic(&g_AnsProtocol);
    frame.protocolVersion = AnsProtocol_getVersion(&g_AnsProtocol);
    frame.linkType = linkType;
    frame.peerId = (AiUInt32) clientId;
    frame.statusWord = ansStatus;

    ret = AnsConnection_send(connection, (char *) &frame, sizeof(frame));
    if (ret != AnsStatus_OK)
    {
        ANSLogError("AnsLinkResponseFrame send error (%d)!", (int) ret);
        return AiFalse;         /** \return AiFalse - send error            */
    }

    return AiTrue;              /** \return AiTrue - success                */
}

/*************************************************************************//**
 * Read the incoming link initialization frame response from the open channel.
*****************************************************************************/
extern AiBoolean ANS_LinkResponseFrame_read (
    struct AnsConnection* connection,   /*!< [in] connection to read from  */
    unsigned long const     timeOutMs,  /*!< [in] read timeout [ms] or      */
                                        /*!< NTSOCKET_TIMEOUT_INFINITE      */
    AnsLinkResponseFrame   *pFrame)     /*!< [out] frame read on success    */
{
    AnsStatus ret;

    VALIDATE_PTR(connection, AiFalse);
    VALIDATE_PTR(pFrame, AiFalse);

    ret = AnsConnection_receive(connection, (char *) pFrame, sizeof(*pFrame), timeOutMs);
    if (ret != AnsStatus_OK)
    {
        ANSLogError(MODULENAME " read error (%d)!\n", (int) ret);

        return AiFalse;         /** \return AiFalse - read error            */
    }

    /* Ensure that the frame type is the expected one.                      */
    if ( pFrame->magicValue != AnsProtocol_getMagic(&g_AnsProtocol) )
    {
        ANSLogError(MODULENAME " read: Unexpected magic %lX !", (unsigned long) pFrame->magicValue);
        return AiFalse;
    }

    return AiTrue;              /** \return AiTrue - success                */
}


