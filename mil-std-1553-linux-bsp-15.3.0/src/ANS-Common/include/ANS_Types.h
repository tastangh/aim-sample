/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Types.h
 *
 * This file contains common data ANS data types.
 *
 *
 ******************************************************************************/
#ifndef ANS_TYPES_H
#define ANS_TYPES_H

/* Includes *******************************************************************/

#include "Ai_cdef.h"

/* Constants ******************************************************************/


#if defined(WIN32) && !defined(__attribute__)
# define __attribute__(p)   /* empty */
#endif

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/**
 * Definition of the ANS status word. This status word is part of any
 * response frame sent back to the client.
 */
typedef  enum tagAnsStatus_t
{
    AnsStatus_OK = 0,              /*!< everything was okay                  */
    AnsStatus_InvalidPeerId,     /*!< the specified peer ID is not valid */
    AnsStatus_InvalidLinkType,     /*!< invalid link type specified          */
    AnsStatus_ClientRegistrationFailure,
                                    /*!< the client registration failed     */
    AnsStatus_InvalidCmdFrame,     /*!< invalid cmd frame received         */
    AnsStatus_InvalidHeader,       /*!< invalid header received            */
    AnsStatus_InvalidTransactionNo,/*!< the transaction number does not    */
                                    /*!< match the expected one             */
    AnsStatus_InvalidFunctionId,   /*!< unknown function ID                */
    AnsStatus_OutOfMemory,         /*!< out of Memory                      */
    AnsStatus_ReadFrameError,      /*!< reading an frame failed            */
    AnsStatus_SendFrameError,      /*!< sending a frame failed             */
    AnsStatus_InvalidModuleIndex,  /*!< invalid module index specified     */
    AnsStatus_Error,               /*!< a major error occurred             */

    AnsStatus_SocketDisconnected,  /*!< socket disconnected by peer        */
    AnsStatus_SocketCreateError,   /*!< socket write failed                */
    AnsStatus_SocketConnectError,  /*!< socket connection failed           */
    AnsStatus_SocketReadError,     /*!< socket read failed                 */
    AnsStatus_SocketWriteError,    /*!< socket write failed                */

    AnsStatus_IncompatibleProtVer,  /*!< Connection with incompatible ANS protocol version detected */

    AnsStatus_Timeout,              /*!< Generic timeout error */

} AnsStatus;



/**
 * Definition of the various supported ANS link types.
 */
typedef enum tagAnsLinkType_t
{
    AnsLinkType_AdminChannel = 0, /*!< (admin.) command/configuration Channel */
    AnsLinkType_BoardChannel      /*!< Board Command Channel, Communication with  */
                                   /*!< Board uses this type of Link               */
} AnsLinkType;



/**
 * Type-save definition of a boolean for ANS functions.
 */
typedef enum tagAnsBool_t
{
    AnsFalse = 0,              /*!< FALSE                          */
    AnsTrue                    /*!< TRUE                           */

} AnsBool;

/* Prototypes *****************************************************************/

#endif  /* ANS_TYPES_H */
