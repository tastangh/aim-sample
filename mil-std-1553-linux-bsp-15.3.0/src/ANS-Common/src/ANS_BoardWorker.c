/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_BoardWorker.c
 *
 * This module provides the ANS board worker thread.
 *
 *
 ******************************************************************************/
#define MODULENAME "ANS_BoardWorker"

/* Includes *******************************************************************/

#include <stdlib.h>
#include <string.h>


#include "Ai_Defines.h"
#include "ANS_BoardWorker.h"
#include "ANS_Thread.h"
#include "ANS_Log.h"
#include "ANS_Types.h"
#include "ANS_CmdFrame.h"
#include "Ai_Socket.h"
#include "Ai_Validate.h"
#include "ANS_MemChunk.h"
#include "ANS_BoardCommands.h"
#include "ANS_Server.h"
#include "Ai_Socksv.h"

#if defined(__unix) || defined(TARGET_OS2) || defined(_VXBUS)
    # include <netinet/in.h>
#endif
#if defined(WIN32)
    # include <winsock2.h>
    typedef int socklen_t;
#endif


/*! \def ANS_BOARD_EVENT_STREAM_TIMEOUT_MS
 *
 * Timeout in milliseconds to wait for a board event stream is connected to by client
 */
#define ANS_BOARD_EVENT_STREAM_TIMEOUT_MS   10000



/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/


/* Variables ******************************************************************/


/* Functions ******************************************************************/


static AnsCmdRspFrame* processBoardCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* cmdFrame, MemChunk* responseMemory)
{
    AnsCmdRspFrame* (*handler)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*) = NULL;
    AnsCmdRspFrame* responseFrame = NULL;


    switch (cmdFrame->header.functionId)
    {
    case CloseBoardID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->close;
        break;

    case ReadMemoryID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->readMemory;
        break;

    case WriteMemoryID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->writeMemory;
        break;

    case IoCommandID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->ioCommand;
        break;

    case IoCommandStructID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->ioCommandStruct;
        break;

    case GetDriverVersionID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->getDriverVersion;
        break;

    case GetSoftwareVersionsID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->getSoftwareVersions;
        break;

    case GetDriverInfoID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->getDriverInfo;
        break;

    case GetBoardMemSizeID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->getBoardMemSize;
        break;

    case OpenBoardEventStreamID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->openBoardEventStream;
        break;

    case CloseBoardEventStreamID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->closeBoardEventStream;
        break;

    case RegisterCallbackID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->registerCallback;
        break;

    case UnregisterCallbackID:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->unregisterCallback;
        break;

    default:
        handler = AnsServer_getBoardCommandHandlers(&g_AnsServer)->protocolHandler;
        break;
    }

    if(!handler)
    {
        ANSLogError(MODULENAME ": ERROR: No handler for board command with ID %u provided.", cmdFrame->header.functionId);
        return NULL;
    }

    responseFrame = handler(board, connection, cmdFrame, responseMemory);

    return responseFrame;
}


static AnsCmdRspFrame* processBoardOpenCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* cmdFrame,
                                               MemChunk* responseMemory)
{
    if(!AnsServer_getBoardCommandHandlers(&g_AnsServer)->open)
    {
        ANSLogError(MODULENAME ": ERROR: No handler for board command with ID %u provided.", cmdFrame->header.functionId);
        return NULL;
    }

    return AnsServer_getBoardCommandHandlers(&g_AnsServer)->open(board, connection, cmdFrame, responseMemory);
}


static AnsCmdRspFrame* processBoardCloseCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* cmdFrame,
                                                MemChunk* responseMemory)
{
    AnsCmdRspFrame* response = NULL;

    if(!AnsServer_getBoardCommandHandlers(&g_AnsServer)->close)
    {
        ANSLogError(MODULENAME ": ERROR: No handler for board command with ID %u provided.", cmdFrame->header.functionId);
        return NULL;
    }

    response = AnsServer_getBoardCommandHandlers(&g_AnsServer)->close(board, connection, cmdFrame, responseMemory);
    if(response && board)
    {
        AnsServer_releaseBoard(&g_AnsServer, board);
    }

    return response;
}


static struct AnsBoard* attachBoard(struct AnsBoard* current_attached, struct AnsBoard* board_to_attach)
{
    struct AnsBoard* attached_board;

    if(current_attached && current_attached->handle != board_to_attach->handle)
    {
        ANSLogError(MODULENAME "ERROR: Tried to attach a second board");
        return NULL;
    }
    else if(current_attached)
    {
        return current_attached;
    }
    else
    {
        attached_board = AnsServer_requestBoard(&g_AnsServer, board_to_attach->handle);
        if(!attached_board)
        {
            attached_board = AnsServer_createBoard(&g_AnsServer, board_to_attach->handle);
            if(!attached_board)
            {
                ANSLogError(MODULENAME "ERROR: Failed to create new board with handle %p", board_to_attach->handle);
                return NULL;
            }
        }

        return attached_board;
    }
}



/**************************************************************************//**
 * This function realizes the worker thread callback for 
 * ANS board and module I/O commands.
 * It runs in the context of the multiplexer thread.
 *****************************************************************************/
extern long int ANS_BoardWorkerCallback (
    AnsThreadId         threadId,
    struct AnsServerPeer* peer,
    struct AnsConnection* connection)
{
    MemChunk			rxMemory;
    MemChunk			txMemory;
    AiBoolean			boolRc;
    AnsCmdFrame		*pCmdFrame;
    AnsCmdRspFrame		*pCmdResponse;
    AnsStatus			status;
    signed long         functionRc = EXIT_SUCCESS;
    struct AnsBoard board_to_open = {0};
    struct AnsBoard* attached_board = NULL;

    VALIDATE_PTR(peer, EXIT_FAILURE);
    VALIDATE_PTR(connection, EXIT_FAILURE);

    ANSLogDebug(MODULENAME ": Debug: BoardWorkerThread started.");

    NOTUSED_PARAM(threadId);

    ANSLogDebug( MODULENAME ": Debug: Servicing client %.16s", connection->peer_ip.ipAddress);

    MemChunk_init(&rxMemory);
    MemChunk_init(&txMemory);

    /*
     * Pre-allocate memory for the transmit and receive frames.
     * This "memory chunks" get reused for all I/O operations in this thread.
     */
    boolRc = MemChunk_allocate(&rxMemory, sizeof(ANS_Header));
    if ( boolRc == AiFalse )
    {
        ANSLogError(MODULENAME ": MemChunk_allocate error for rxMemory!");
        return EXIT_FAILURE;
    }

    boolRc = MemChunk_allocate(&txMemory, sizeof(ANS_Header));
    if ( AiFalse == boolRc )
    {
        MemChunk_free(&rxMemory);
        ANSLogError(MODULENAME ": MemChunk_allocate error for txMemory!");
        return EXIT_FAILURE;
    }

    for (;;)
    {
        /* Wait for the next command sent by the client */

        /* Read Command Frame */
        status = ANS_CmdFrame_read(connection, NTSOCKET_TIMEOUT_INFINITE, &rxMemory);
        if ( AnsStatus_SocketDisconnected == status )
        {
            ANSLogDebug(MODULENAME ": DBG: Client disconnected");
            break;
        }

        /* Check status */
        if ( AnsStatus_OK != status )
        {
            ANSLogError(MODULENAME ": ERROR: CmdFrame_read error(%d)!", (int) status);
            functionRc = EXIT_FAILURE;
            break;
        }

        pCmdFrame = (AnsCmdFrame *)rxMemory.pMemory;
        pCmdResponse = NULL;

        if(pCmdFrame->header.commandtype != BoardCommand)
        {
            ANSLogError(MODULENAME ": ERROR: Unexpected Command Type (%u)!", pCmdFrame->header.commandtype);
            functionRc = EXIT_FAILURE;
            break;
        }

        switch(pCmdFrame->header.functionId)
        {
            case OpenBoardID:
                pCmdResponse = processBoardOpenCommand(&board_to_open, connection, pCmdFrame, &txMemory);
                if(!(attached_board = attachBoard(attached_board, &board_to_open)))
                {
                    ANSLogError(MODULENAME "Error: Failed to attach board");

                    /* Just return the response header without actual payload to indicate error*/
                    pCmdResponse->header.status = AnsStatus_Error;
                    pCmdResponse->header.ansHeader.transactionSize = sizeof(AnsCmdRspHeader) - sizeof(ANS_Header);
                }
                break;

            case CloseBoardID:
                pCmdResponse = processBoardCloseCommand(attached_board, connection, pCmdFrame, &txMemory);
                if(pCmdResponse)
                {
                    attached_board = NULL;
                }
                break;

            default:
                pCmdResponse = processBoardCommand(attached_board, connection, pCmdFrame, &txMemory);
                break;
        }

        if ( NULL == pCmdResponse )
        {
            /* Assume handler function has already sent a response and continue with next command */
            continue;
        }

        /* send Response */
        ANSLogDebug(MODULENAME ": returning AnsStatus=%d", (int) pCmdResponse->header.status);
        status = ANS_CmdRspFrame_send(connection, pCmdResponse);
        if ( AnsStatus_OK != status )
        {
            ANSLogError(MODULENAME ": ERROR: response send error (%d)!", (int) status);
            functionRc = EXIT_FAILURE;
            break;
        }

    } 	/* for */

    /* Finally release the RX and TX buffers */
    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    if(attached_board)
    {
        AnsServer_releaseBoard(&g_AnsServer, attached_board);
    }

    ANSLogDebug( MODULENAME ": INF: Disconnecting from client %.16s", connection->peer_ip.ipAddress);

    return functionRc;      /** \return EXIT_SUCCESS, EXIT_FAILURE  */
}


extern AnsCmdRspFrame* ANS_OpenBoardEventStream(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* cmdFrame, MemChunk* responseMemory)
{
    AiUInt32 payloadSize;
    AnsCmdRspFrame* responseFrame = NULL;
    struct AnsOpenBoardEventStreamResponsePayload* responsePayload = NULL;
    NTSOCKET stream_socket = {0};
    struct AnsBoardEventObserver* event_stream = NULL;
    AnsStatus ret;
    struct sockaddr_in sockaddr;
    socklen_t socklen = sizeof(sockaddr);

    /* Prepare the response frame */
    payloadSize  = ANS_RSP_PAYLOAD(struct AnsOpenBoardEventStreamResponsePayload);

    if(ANS_Header_create(cmdFrame->header.ansHeader.transactionId, // copy Transaction ID
                         payloadSize, 0, payloadSize, // fragment payload=payload as not fragmentated
                         cmdFrame->header.ansHeader.clientId, // copy Client ID
                         responseMemory) != AnsStatus_OK)
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame*) responseMemory->pMemory;
    responsePayload = (struct AnsOpenBoardEventStreamResponsePayload*) responseFrame->payload;
    responseFrame->header.status = AnsStatus_OK;

    ret = AnsStatus_OK;
    do
    {
        if(!board || !connection)
        {
            ret = responseFrame->header.status = AnsStatus_Error;
            break;
        }

        /* Create a listening TCP socket that will be used as connection point for the client */
        if(ntsocket_create(&stream_socket, NTSOCKET_STYLE_TCP) != NTSOCKET_RET_OK)
        {
            ret = responseFrame->header.status = AnsStatus_SocketCreateError;
            break;
        }

        if(ntsocket_createserver(&stream_socket, 0, 1) != NTSOCKET_RET_OK)
        {
            ret = responseFrame->header.status = AnsStatus_SocketCreateError;
            break;
        }

        event_stream = malloc(sizeof(struct AnsBoardEventObserver));
        if(!event_stream)
        {
            ret = responseFrame->header.status = AnsStatus_Error;
            break;
        }

        if(getsockname(stream_socket.socket, (struct sockaddr*)&sockaddr, &socklen))
        {
            ret = responseFrame->header.status = AnsStatus_Error;
            break;
        }

        /* Use the port number of the listening socket as handle */
        responsePayload->handle = sockaddr.sin_port;

        /* Convert the port number to little endian */
        responsePayload->port = BSWAP16(ntohs(sockaddr.sin_port));

        ret = ANS_CmdRspFrame_send(connection, responseFrame);
        if(ret != AnsStatus_OK)
        {
            ret = responseFrame->header.status = AnsStatus_SendFrameError;
            break;
        }

        /* Response already sent. Clear response to be returned by caller */
        responseFrame = NULL;

        /*
         * Attention: This handle will be transferred over network and should NEVER be larger than 32-bit.
         * This is guaranteed when using the port number
         *
         */
        ret = AnsBoardEventObserver_init(event_stream, (AiHandle) (AiUIntPtr) responsePayload->handle);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        ret = AnsBoardEventObserver_waitConnect(event_stream, &stream_socket, ANS_BOARD_EVENT_STREAM_TIMEOUT_MS);
        if(ret != AnsStatus_OK)
        {
            break;
        }
    }while(0);

    ntsocket_destroy(&stream_socket);

    if(ret != AnsStatus_OK)
    {
        if(event_stream)
        {
            AnsBoardEventObserver_destroy(event_stream);
        }
    }
    else
    {
        AnsBoard_registerEventObserver(board, event_stream);
    }

    return responseFrame;
}


AnsCmdRspFrame* ANS_CloseBoardEventStream(struct AnsBoard* board, struct AnsConnection* connection,
                                          AnsCmdFrame* cmdFrame, MemChunk* responseMemory)
{
    struct AnsCloseBoardEventStreamPayload* command_payload = NULL;
    AiUInt32 payloadSize;
    AnsCmdRspFrame* response_frame = NULL;
    struct AnsBoardEventObserver* event_stream = NULL;

    command_payload = (struct AnsCloseBoardEventStreamPayload*) cmdFrame->payload;

    /* Prepare the response frame */
    payloadSize  = sizeof(AnsCmdRspFrame);

    if(ANS_Header_create(cmdFrame->header.ansHeader.transactionId, // copy Transaction ID
                         payloadSize, 0, payloadSize, // fragment payload=payload as not fragmentated
                         cmdFrame->header.ansHeader.clientId, // copy Client ID
                         responseMemory) != AnsStatus_OK)
    {
        return NULL;
    }

    response_frame = responseMemory->pMemory;
    response_frame->header.status = AnsStatus_OK;

    do
    {
        if(!board)
        {
            response_frame->header.status = AnsStatus_Error;
            break;
        }

        event_stream = AnsBoard_getEventObserver(board, (AiHandle) (AiUIntPtr) command_payload->handle);
        if(!event_stream)
        {
            response_frame->header.status = AnsStatus_Error;
            break;
        }

        AnsBoard_unregisterEventObserver(board, event_stream);

        AnsBoardEventObserver_destroy(event_stream);

    }while(0);

    return response_frame;
}


