/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_CmdFrame.h
 *
 * This file contains the definition of an ANS command frame.
 *
 *
 ******************************************************************************/
#ifndef ANS_CMDFRAME_H
#define ANS_CMDFRAME_H

/* Includes *******************************************************************/

#include "ANS_Types.h"
#include "ANS_Header.h"
#include "ANS_MemChunk.h"
#include "ANS_Connection.h"
#include "Ai_cdef.h"


/* Constants ******************************************************************/

/* Macros ********************************************************************/

/**
 * Calculate the payload size for the used command
 */
#define ANS_CMD_PAYLOAD(cmdType)   (sizeof(AnsCmdFrameHeader)-sizeof(ANS_Header)+sizeof(cmdType))

/**
 * Calculate the payload size for the specified command response.
 * The payload size is the frame size minus the generic frame header.
 */
#define ANS_RSP_PAYLOAD(cmdType)   (sizeof(AnsCmdRspHeader)-sizeof(ANS_Header)+sizeof(cmdType))


/*! \def ANS_CMD_RESP_TIMEOUT_MS
 * Recommended timeout in milliseconds for board commands
 */
#define ANS_CMD_RESP_TIMEOUT_MS 60000L   /*!< ANS response timeout for commands    */


/* Type definitions ***********************************************************/



/**
 * ANS Command Frame header type definition
 */
#pragma pack(1)
typedef struct tagAnsCmdFrameHeader_t
{
    ANS_Header ansHeader;     /*!< ANS Frame header      */
    AiUInt32 commandtype;     /*!< Command type. See enumeration \ref CommandType */
    AiUInt32 functionId;     /*!< Function ID            */

} AnsCmdFrameHeader;
#pragma pack()


/**
 * ANS Command Frame type definition
 */
#pragma pack(1)
typedef struct tagAnsCmdFrame_t
{
    AnsCmdFrameHeader  header;

    char payload[1];              /*!< command type specified payload */

} AnsCmdFrame;
#pragma pack()


/**
 * ANS Command Response Frame header type definition.
 * The response header always carries the original function ID and
 * the ANS processing status.
 */
#pragma pack(1)
typedef struct tagAnsCmdRspHeader_t
{
    ANS_Header ansHeader;     /*!< ANS Frame header          */
    AiUInt32    functionId;     /*!< Function ID                */
    AiUInt32    status;         /*!< see AnsStatus             */

} AnsCmdRspHeader;
#pragma pack()

/**
 * ANS Command Response Frame type definition
 */
#pragma pack(1)
typedef struct
{
    AnsCmdRspHeader   header;     /*!< fix header part            */

    char payload[1];              /*!< command type specified payload */

} AnsCmdRspFrame;
#pragma pack()


typedef enum eCommandType
{
    AdminCommand = 0,
    BoardCommand
}CommandType;




/* Prototypes *****************************************************************/

 extern	AnsStatus      ANS_CmdFrame_read (
        struct AnsConnection* connection,
        unsigned long const timeOutMs,
        MemChunk            *pRxBuffer);

 extern AnsStatus ANS_CmdFrame_send (
    struct AnsConnection* connection,
    AnsCmdFrame    *pCmdFrame);


 extern AnsStatus ANS_CmdRspFrame_read (
     struct AnsConnection* connection,
     unsigned long const timeOutMs,
     AiUInt32            transactionNo,
     MemChunk            *pRxBuffer);

  extern AnsStatus ANS_CmdRspFrame_send (
     struct AnsConnection* connection,
     AnsCmdRspFrame  *pResponseFrame);


#endif  /* ANS_CMDFRAME_H */
