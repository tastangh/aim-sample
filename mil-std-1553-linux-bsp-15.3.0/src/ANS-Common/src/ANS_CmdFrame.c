/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_CmdFrame.c
 *
 * This file contains implementations of functions for reading and wrting
 * ANS Command Frames
 *
 *
 ******************************************************************************/
#define MODULENAME "ANS_CmdFrame"

/* Includes *******************************************************************/

#include <stdlib.h>

#include "ANS_CmdFrame.h"
#include "Ai_cdef.h"
#include "Ai_Validate.h"
#include "ANS_Log.h"
#include "ANS_Frame.h"

#ifdef _DO_HEXDUMP
#include "hexdump.h"
#endif

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

/* Variables ******************************************************************/

/* Functions ******************************************************************/

/*************************************************************************//**
 * Read the next ANS command frame from the open socket.
 *****************************************************************************/
extern	AnsStatus	ANS_CmdFrame_read (
    struct AnsConnection* connection,   /*!< [in] connection to read on        */
    unsigned long const timeOutMs,  /*!< [in] read timeout [ms] or      */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE      */
    MemChunk			*pRxBuffer)	/*!< [in] pointer to allocated Memory */
{
    AnsStatus		status;
    AnsCmdFrame    *pRxFrame;

    VALIDATE_PTR(connection, AnsStatus_Error);    /** \return AnsStatus_Error */
    VALIDATE_PTR(pRxBuffer, AnsStatus_Error);

    /* Read the next ANS Frame */
    status = ANS_Frame_read( connection, timeOutMs, pRxBuffer);
    if ( AnsStatus_OK != status )
    {
        if ( AnsStatus_SocketDisconnected != status )
        {
            ANSLogError(MODULENAME ": ERROR: Frame read error!");
        }
        return status;
    }

    /*
     * Ensure that there was a whole command frame received.
     */
    pRxFrame = (AnsCmdFrame *) pRxBuffer->pMemory;
    if ( NULL == pRxFrame 
    || pRxFrame->header.ansHeader.transactionSize
        < (sizeof(AnsCmdFrameHeader) - sizeof(ANS_Header)) )
    {
        ANSLogError(MODULENAME ": ERROR: ANS Cmd Frame too small or NULL!");
        if ( NULL != pRxFrame )
        {
            ANSLogError(MODULENAME ": ERROR: -> cmd=%d payload=%d",
                (int) pRxFrame->header.functionId,
                (int) pRxFrame->header.ansHeader.fragmentPayloadSize);
        }
        status = AnsStatus_InvalidHeader;  /** \return AnsStatus_InvalidHeader */
    }

    /* TODO check if the received frame is a Command Frame */
    /* ????? */

    return AnsStatus_OK;					/** \return AnsStatus_OK   */
}

/**************************************************************************//**
 * This function sends an ANS Command Frame
 *****************************************************************************/
extern AnsStatus ANS_CmdFrame_send (
    struct AnsConnection* connection, /*!< [in] connection to send on */
    AnsCmdFrame    *pCmdFrame)     /*!< [in] frame to send                 */
{
    AnsStatus ret;
    int numBytesToSend;

    VALIDATE_PTR(connection, AnsStatus_Error);  /** \return AnsStatus_Error */
    VALIDATE_PTR(pCmdFrame, AnsStatus_Error);

    /* Get number of Bytes to send: numBytes of Header + NumBytes payload */
    numBytesToSend = sizeof(ANS_Header) + pCmdFrame->header.ansHeader.transactionSize;

    /* send ANS-Response-Frame via socket */
    ret = AnsConnection_send(connection, (const char*) pCmdFrame, numBytesToSend);

    if ( ret != AnsStatus_OK)
    {
        ANSLogError(MODULENAME ": ERROR : Could not send Frame (%d)!", (int) ret);
        return AnsStatus_SendFrameError;   /** \return AnsStatus_SendFrameError   */
    }

    return AnsStatus_OK;	/** \return AnsStatus_OK - success                */
}


/*************************************************************************//**
 * Read the next ANS response frame from the open socket.
 *****************************************************************************/
extern  AnsStatus   ANS_CmdRspFrame_read (
    struct AnsConnection* connection,   /*!< [in] connection to read from*/
    unsigned long const timeOutMs,  /*!< [in] read timeout [ms] or          */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
    AiUInt32            transactionNo,
                                    /*!< [in] expected transaction number   */
    MemChunk            *pRxBuffer) /*!< [in] pointer to allocated Memory */
{
    AnsCmdRspFrame *pRxFrame;
    AnsStatus       status;

    VALIDATE_PTR(connection, AnsStatus_Error);    /** \return AnsStatus_Error */
    VALIDATE_PTR(pRxBuffer, AnsStatus_Error);

    /*
     * Read the next ANS Frame
     */
    status = ANS_Frame_read(connection, timeOutMs, pRxBuffer);
    if ( AnsStatus_OK != status )
    {
        if ( AnsStatus_SocketDisconnected != status )
        {
            ANSLogError(MODULENAME ": ERROR : Frame read error %d!", (int) status);
        }

        return status;                  /** \return see ANS_Frame_read */
    }

    /*
     * Ensure that there was a whole response frame received.
     */
    pRxFrame = (AnsCmdRspFrame *) pRxBuffer->pMemory;
    if ( NULL == pRxFrame
    || pRxFrame->header.ansHeader.transactionSize
        < (sizeof(AnsCmdRspHeader) - sizeof(ANS_Header)) )
    {
        ANSLogError(MODULENAME ": ERROR : CmdRsp Frame too small or NULL!");
        return AnsStatus_InvalidHeader;
    }


    /*
    * Ensure that the transaction number matches the expected one.
    */
    if ( pRxFrame->header.ansHeader.transactionId != transactionNo )
    {
        ANSLogError(MODULENAME
            ": read: transaction ID mismatch: expected: %lu got: %lu !",
            (unsigned long) transactionNo,
            (unsigned long) pRxFrame->header.ansHeader.transactionId);

        return AnsStatus_InvalidTransactionNo;
    }


    /*
    * Check server error state
    */
    if (pRxFrame->header.status != AnsStatus_OK)
    {
        ANSLogError(MODULENAME
            ": read: header status signals error %lu on server!",
            (unsigned long)pRxFrame->header.status);

        return pRxFrame->header.status;
    }


    return AnsStatus_OK;                  /** \return AnsStatus_OK   */
}

/**************************************************************************//**
 * This function sends an ANS Response Frame.
 *****************************************************************************/
extern AnsStatus ANS_CmdRspFrame_send (
    struct AnsConnection* connection,   /*!< [in] connection to send on */
    AnsCmdRspFrame *pResponseFrame)  /*!< [in] pointer to response frame to send */
{
    AnsStatus ret;
    int numBytesToSend;

    VALIDATE_PTR(connection, AnsStatus_Error);  /** \return AnsStatus_Error */
    VALIDATE_PTR(pResponseFrame, AnsStatus_Error);

    /* Get number of Bytes to send: numBytes of Header + NumBytes payload */
    numBytesToSend = sizeof(ANS_Header) + pResponseFrame->header.ansHeader.transactionSize;

    /* send ANS-Response-Frame via socket */
    ret = AnsConnection_send(connection, (const char *) pResponseFrame, numBytesToSend);

    if ( ret != AnsStatus_OK)
    {
        ANSLogError(MODULENAME ": ERROR : Could not send frame (%d)!", ret);
        return AnsStatus_SocketWriteError;
                                /** \return AnsStatus_SocketWriteError */
    }

    return AnsStatus_OK;        /** \return AnsStatus_OK - success     */
}
