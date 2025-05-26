/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Frame.c
 *
 * This file contains the implementations of the generic ANS Frame, as well
 * as read and write functions
 *
 *
 ******************************************************************************/
#define MODULENAME "ANS_Frame"

/* Includes *******************************************************************/

#include <stdlib.h>

#include "ANS_Frame.h"
#include "ANS_Header.h"
#include "ANS_Log.h"
#include "Ai_Validate.h"

#include "Ai_cdef.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

 static AnsStatus	readPayload (
        struct AnsConnection* connection,
        unsigned long const timeOutMs,
        AiUInt32            payloadSize,
        MemChunk            *pRxBuffer);

/* Variables ******************************************************************/

/* Functions ******************************************************************/

/**************************************************************************//**
 * This function reads an ANS frame from the open channel
 *****************************************************************************/
extern AnsStatus ANS_Frame_read (
    struct AnsConnection* connection,   /*!< [in] connection to read from          */
    unsigned long const timeOutMs,  /*!< [in] read timeout [ms] or        */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE        */
    MemChunk            *pRxBuffer)	/*!< [in] pointer to allocated Memory */
{
    ANS_Header		*pHeader;
    AnsStatus		status = AnsStatus_Error;

    VALIDATE_PTR(connection, AnsStatus_Error);
    VALIDATE_PTR(pRxBuffer, AnsStatus_Error);

    /*
     * Reading is done with a 2-Step read process
     * first step: read the header of a frame
     * second step: read the payload of a frame,
     * 				payload size depends on transaction size (=payload size)
     * 				specified in the header
     */

    /* Step 1: read header */
    status = ANS_Header_read2Buffer(connection, timeOutMs, pRxBuffer);
    if ( AnsStatus_OK != status )
    {
        return status;         /** \return read error, see AnsStatus            */
    }

    /* get payload size */
    pHeader = (ANS_Header *) pRxBuffer->pMemory;

    /* Step 2: read payload, read NumBytes specified in Header */
    if ( pHeader->transactionSize > 0 )
    {
        status = readPayload(connection, timeOutMs, (size_t) pHeader->transactionSize, pRxBuffer);
    }
    else
    {
        ANSLogDebug("ANSFrameRead: No payload");
    }
    if ( AnsStatus_OK != status )
    {
        return status;
    }
    
    return AnsStatus_OK;       /** \return AnsStatus_OK               */
}

/**************************************************************************//**
 * This function reads the payload from an ANS frame and stores the payload
 * in the RX-Buffer (described by MemChunk)
 *****************************************************************************/
static AnsStatus readPayload (
    struct AnsConnection* connection,  		/*!< [in] connection to read on */
    unsigned long const timeOutMs,  	/*!< [in] read timeout [ms] or      */
                                        /*!< NTSOCKET_TIMEOUT_INFINITE      */
    AiUInt32 payloadSize,	/*!< [in] Size of payload in bytes   */
    MemChunk *pRxBuffer)		/*!< [in,out] Receive buffer        */
{
    AnsStatus  ret;
    AiUInt32 nTotalBytes;
    AiBoolean status = AiFalse;
    char* pToPayload;

    VALIDATE_PTR(connection, AnsStatus_Error);
    VALIDATE_PTR(pRxBuffer, AnsStatus_Error);

    nTotalBytes = sizeof(ANS_Header) + payloadSize;
    status = MemChunk_reallocate(pRxBuffer, nTotalBytes);

    if ( AiFalse == status )
    {
        return AnsStatus_Error;
    }

    /* Skip the header */
    pToPayload = (char *) pRxBuffer->pMemory;
    pToPayload += sizeof(ANS_Header);

    ret = AnsConnection_receive(connection, pToPayload, payloadSize, timeOutMs);
    if(ret != AnsStatus_OK)
    {
        ANSLogError("Error: Failed to recieve payload on connection %p", connection);
        return ret;
    }

    return AnsStatus_OK;           /** \return AnsStatus_OK - success     */
}
