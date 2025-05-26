/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_AdminCommands.h
 *
 *  This file contains frame definitions for
 *  all generic administrative ANS commands
 *  and their responses
 *
 */

#ifndef ANS_ADMINCOMMANDS_H_
#define ANS_ADMINCOMMANDS_H_

#include "ANS_Protocol.h"
#include "ANS_Server.h"




/*! \enum AdminCommandID
 *  Enumeration for all possible administrative commands \n
 *  Values equal or greater than AdminCommandProtID can be used
 *  for protocol specific administrative commands.
 */
typedef enum eAdminCommandID
{
    GetNumBoardsID = 0,     //!< GetNumBoardsID
    GetServerInfoID,        //!< GetServerInfoID
    AdminCommandProtID = 256//!< AdminCommandProtID
}AdminCommandID;




#pragma pack(1)
/**
 * GetServerInfo response payload
 */
typedef struct
{
    struct AnsServerInfo serverInfo;
    struct AnsServerVersion serverVersion;
    struct AnsProtocolVersion protocolVersion;
    AiChar hostName[128];
    AiChar osInfo[128];
} AnsGetServerInfoResponsePayload;
#pragma pack()


/**
 * GetNumBoards response payload
 */
#pragma pack(1)
typedef struct
{
    AiUInt32    numBoards;

} AnsGetNumBoardsResponsePayload;
#pragma pack()






/* Internal UDP commands for server discovery. Do not change this */

#pragma pack(1)
typedef struct
{
    AiInt32 magic;
    AiInt32 port;
    char    info[32];
} AnsDiscoverRequest;
#pragma pack()

#pragma pack(1)
typedef struct
{
    AiUInt32 magic;
    AiUInt32 protocol;
    AiUInt32 port;
    AiUInt32 board_count;
    AiUInt32 protocol_major;
    AiUInt32 protocol_minor;
    char     host_name[128];
} AnsDiscoverRespons;
#pragma pack()

#define ANS_DISCOVER_REQUST_MAGIC   (('B'<<24)|('I'<<16)|('N'<<8)|'G')
#define ANS_DISCOVER_RESPONSE_MAGIC (('B'<<24)|('O'<<16)|('N'<<8)|'G')

typedef void(*ANS_DISCOVER_CALLBACK_FUNC_PTR)(const char * ip, AnsDiscoverRespons * response);

#pragma pack(1)
typedef struct
{
    AiUInt32 port;
    ANS_DISCOVER_CALLBACK_FUNC_PTR callback;
    AiInt32 threadTableIndex;
    AiBoolean active;
} AnsDiscoverThreadConfig;
#pragma pack()

#endif /* ANS_ADMINCOMMANDS_H_ */
