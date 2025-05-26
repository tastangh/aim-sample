/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file AnsClientPeer.c
 *
 */


#include "ANS_ClientPeer.h"
#include "Ai_Socktl.h"
#include "ANS_Log.h"
#include "Ai_Strncpy.h"
#include "ANS_LinkInitFrame.h"
#include "Ai_Validate.h"
#include "ANS_BoardCommands.h"


#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <arpa/inet.h>
#endif



/*! \def ANS_LINK_INIT_TIMEOUT_MS
 * Initial link initialization time-out in milliseconds
 */
#define ANS_LINK_INIT_TIMEOUT_MS 3000L


/**
 * Current transaction number.
 */
static AiUInt32 g_transactionNumber = 0;



static AI_INLINE AiUInt32 getNextTransactionNumber(void)
{
    return ++g_transactionNumber;
}




void AnsClientPeer_init(struct AnsClientPeer* peer, const char* url, unsigned short port)
{
    STRNCPY(peer->address, url, sizeof(peer->address));
    peer->port = port;
    peer->connection = NULL;
    peer->connection_lock = ai_mutex_create();
    AI_LIST_INIT(peer->boards);
    peer->board_lock = ai_mutex_create();
    peer->reference_count = 1;
}


/**************************************************************************//**
 * This function tries to resolve the specified symbolic host name.
 *****************************************************************************/
static AnsBool resolveName (
    char const          *pName,     //!< [in] host name
    NTSOCKET_IPADDRESS  *pIp)       //!< [out] resolved IP address
{

    int             rc;
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct in_addr  addr;

    if ( NULL == pName )
        return AnsFalse;

    (void) memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    rc = getaddrinfo(pName, NULL, &hints, &res);
    if ( 0 != rc )
        return AnsFalse;

    if ( NULL == res )
        return AnsFalse;

    (void) memset(&addr, 0, sizeof(addr));
    addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;

    if ( 0 != ntsocket_setip(pIp, inet_ntoa(addr)) )
        return AnsFalse;

    return AnsTrue;
}



AnsStatus AnsClientPeer_connect(struct AnsClientPeer* peer)
{
    NTSOCKET_IPADDRESS ip;
    AnsStatus ret;
    AnsLinkResponseFrame linkRxFrame;
    struct AnsConnection* connection = NULL;

    ai_mutex_lock(peer->connection_lock);

    /* Peer already connected. Just return okay */
    if(peer->connection)
    {
        ai_mutex_release(peer->connection_lock);
        return AnsStatus_OK;
    }

    ret = AnsStatus_OK;
    do
    {
        if ( !ntsocket_isvalidip(peer->address) )
        {
            if(!resolveName(peer->address, &ip))
            {
                ANSLogError("Can't resolve host name!\n");
                ret = AnsStatus_Error;
                break;
            }
        }
        else
        {
            (void) STRNCPY(ip.ipAddress, peer->address,  sizeof(ip.ipAddress));
        }

        connection = malloc(sizeof(struct AnsConnection));
        if(!connection)
        {
            ret = AnsStatus_OutOfMemory;
            break;
        }

        ret = AnsConnection_init(connection);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        ret = AnsConnection_connect(connection, ip.ipAddress, peer->port);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        /*
         * Send the link initialization frame.
         */
        if(!ANS_LinkInitFrame_write(connection, ANS_LINK_INIT_TIMEOUT_MS, AnsLinkType_AdminChannel,
                                    AnsProtocol_getVersion(&g_AnsProtocol), ANS_SERVER_PEER_ID_UNKNOWN))
        {
            ANSLogError("Link init frame write failed!\n");
            ret = AnsStatus_SocketWriteError;
            break;
        }

        /*
         * Wait for the link initialization response frame.
         */
        if(!ANS_LinkResponseFrame_read(connection, ANS_LINK_INIT_TIMEOUT_MS, &linkRxFrame))
        {
            ANSLogError("Link init response read failed!\n");
            ret = AnsStatus_IncompatibleProtVer;
            break;
        }

        if ( AnsStatus_OK != linkRxFrame.statusWord )
        {
            ANSLogError("Link init returned ANS error(%d)!\n", (int) linkRxFrame.statusWord);
            ret = linkRxFrame.statusWord;
            break;
        }
    }while(0);

    if(ret)
    {
        if(connection)
        {
            AnsConnection_destroy(connection);
        }
    }
    else
    {
        peer->connection = connection;
        peer->peer_id = linkRxFrame.peerId;
    }

    ai_mutex_release(peer->connection_lock);

    return ret;
}


void AnsClientPeer_disconnect(struct AnsClientPeer* peer)
{
    struct AnsConnection* connection;
    struct AnsBoard* board;

    ai_mutex_lock(peer->connection_lock);
    ai_mutex_lock(peer->board_lock);

    ai_list_for_each_entry(board, &peer->boards, struct AnsBoard, list)
    {
        AnsClientPeer_disconnectBoard(peer, board);
    }

    ai_mutex_release(peer->board_lock);

    peer->peer_id = ANS_SERVER_PEER_ID_UNKNOWN;

    connection = peer->connection;

    if(connection)
    {
        peer->connection = NULL;
        AnsConnection_destroy(connection);
    }

    ai_mutex_release(peer->connection_lock);
}


AnsStatus AnsClientPeer_requestBoardCount(struct AnsClientPeer* peer, unsigned int* board_count)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AnsCmdFrame* commandFrame = NULL;
    AiUInt32 frameSize;
    AnsStatus ansStatus;
    AnsCmdRspFrame* responseFrame = NULL;
    AnsGetNumBoardsResponsePayload* responsePayload = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    frameSize = sizeof(AnsCmdFrameHeader);

    if(!MemChunk_allocate(&txMemory, frameSize))
    {
        ANSLogError("ERROR: MemChunk_allocate(TX) failed!\n");
        return AnsStatus_OutOfMemory;
    }

    do
    {
        commandFrame = txMemory.pMemory;
        commandFrame->header.commandtype = AdminCommand;
        commandFrame->header.functionId = GetNumBoardsID;

        ansStatus = AnsClientPeer_transmitCommand(peer, commandFrame, frameSize, &rxMemory);
        if (AnsStatus_OK != ansStatus )
        {
            break;
        }

        responseFrame = (AnsCmdRspFrame*) rxMemory.pMemory;
        if(responseFrame->header.status != AnsStatus_OK)
        {
            ansStatus = responseFrame->header.status;
            break;
        }

        responsePayload = (AnsGetNumBoardsResponsePayload*) responseFrame->payload;

        if(board_count)
        {
            *board_count = responsePayload->numBoards;
        }

    }while(0);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ansStatus;
}


extern AnsStatus AnsClientPeer_getServerInfo(struct AnsClientPeer* peer, AnsGetServerInfoResponsePayload* server_info)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AnsCmdFrame* commandFrame = NULL;
    AiUInt32 frameSize;
    AnsStatus ansStatus;
    AnsCmdRspFrame* responseFrame = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    frameSize = sizeof(AnsCmdFrameHeader);

    if( !MemChunk_allocate(&txMemory, frameSize) )
    {
        ANSLogError("ERROR: MemChunk_allocate(TX) failed!\n");
        return AnsStatus_OutOfMemory;
    }

    do
    {
        commandFrame = txMemory.pMemory;
        commandFrame->header.commandtype = AdminCommand;
        commandFrame->header.functionId = GetServerInfoID;

        ansStatus = AnsClientPeer_transmitCommand(peer, commandFrame, frameSize, &rxMemory);
        if( AnsStatus_OK != ansStatus )
        {
            break;
        }

        responseFrame = (AnsCmdRspFrame*) rxMemory.pMemory;
        if( responseFrame->header.status != AnsStatus_OK )
        {
            ansStatus = responseFrame->header.status;
            break;
        }

        memcpy(server_info, responseFrame->payload, sizeof(AnsGetServerInfoResponsePayload));

    } while( 0 );

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ansStatus;
}


void AnsClientPeer_destroy(struct AnsClientPeer* peer)
{
    AnsClientPeer_disconnect(peer);

    ai_mutex_free(peer->board_lock);

    free(peer);
}


AnsStatus AnsClientPeer_transmitCommand(struct AnsClientPeer* peer, AnsCmdFrame* command, AiSize command_size,
                                       MemChunk* response_memory)
{
    AnsStatus rc;
    AiUInt32 transactionNo;

    VALIDATE_PTR(command, AnsStatus_Error);
    VALIDATE_PTR(response_memory, AnsStatus_Error);

    ai_mutex_lock(peer->connection_lock);

    /*
     * Finalize the header
     */
    transactionNo = getNextTransactionNumber();
    command->header.ansHeader.clientId = peer->peer_id;
    command->header.ansHeader.fragmentIndex = 0;
    command->header.ansHeader.fragmentPayloadSize = command_size;
    command->header.ansHeader.transactionId = transactionNo;
    command->header.ansHeader.transactionSize = command_size;

    /*
     * Send the command frame.
     */
    rc = ANS_CmdFrame_send(peer->connection, command);
    if ( AnsStatus_OK != rc )
    {
        ANSLogError("%s: send error(%d)!\n", __FUNCTION__, (int) rc);
    }
    else
    {
        /*
         * Wait for the response from the server.
         */
        rc = ANS_CmdRspFrame_read(peer->connection, ANS_CMD_RESP_TIMEOUT_MS, transactionNo, response_memory);
    }

    ai_mutex_release(peer->connection_lock);

    return rc;
}


struct AnsBoard* AnsClientPeer_requestBoard(struct AnsClientPeer* peer, AiHandle board_handle)
{
    struct AnsBoard* current;

    ai_mutex_lock(peer->board_lock);

    ai_list_for_each_entry(current, &peer->boards, struct AnsBoard, list)
    {
        if(current->handle == board_handle)
        {
            current->reference_count++;
            ai_mutex_release(peer->board_lock);
            return current;
        }
    }

    ai_mutex_release(peer->board_lock);

    return NULL;
}


struct AnsBoard* AnsClientPeer_createBoard(struct AnsClientPeer* peer, AiHandle board_handle)
{
    struct AnsBoard* board;

    board = malloc(sizeof(struct AnsBoard));
    if( !board )
    {
        return NULL;
    }

    AnsBoard_init(board, board_handle);

    board->reference_count++;

    ai_mutex_lock(peer->board_lock);

    ai_list_add_tail(&board->list, &peer->boards);

    ai_mutex_release(peer->board_lock);

    return board;
}


void AnsClientPeer_releaseBoard(struct AnsClientPeer* peer, struct AnsBoard* board)
{
    ai_mutex_lock(peer->board_lock);

    board->reference_count--;

    /* Check if we are the only one that holds a reference to the board and destroy it in this case */
    if(board->reference_count == 1)
    {
        ai_list_del(&board->list);
        AnsClientPeer_disconnectBoard(peer, board);
        AnsBoard_destroy(board);
    }

    ai_mutex_release(peer->board_lock);
}


AnsStatus AnsClientPeer_connectBoard(struct AnsClientPeer* peer, struct AnsBoard* board)
{
    NTSOCKET_IPADDRESS ip;
    AnsStatus ret;
    struct AnsConnection* connection = NULL;
    AnsLinkResponseFrame linkRxFrame;

    AnsBoard_lock(board);

    ret = AnsStatus_OK;
    do
    {
        if(board->command_connection)
        {
            ret = AnsStatus_OK;
            break;
        }

        if ( !ntsocket_isvalidip(peer->address) )
        {
            if(!resolveName(peer->address, &ip))
            {
                ANSLogError("Can't resolve host name!\n");
                ret = AnsStatus_Error;
                break;
            }
        }
        else
        {
            (void) STRNCPY(ip.ipAddress, peer->address,  sizeof(ip.ipAddress));
        }

        connection = malloc(sizeof(struct AnsConnection));
        if(!connection)
        {
            ret = AnsStatus_OutOfMemory;
            break;
        }

        ret = AnsConnection_init(connection);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        ret = AnsConnection_connect(connection, ip.ipAddress, peer->port);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        /*
         * Send the link initialization frame. Reuse the client ID that
         * is registered here.
         */
        if(!ANS_LinkInitFrame_write(connection, ANS_LINK_INIT_TIMEOUT_MS, AnsLinkType_BoardChannel,
                                    AnsProtocol_getVersion(&g_AnsProtocol), peer->peer_id))
        {
            ANSLogError("_openModuleChannel: link init frame write failed!\n");
            ret = AnsStatus_SocketWriteError;
            break;
        }

        /*
         * Wait for the link initialization response frame.
         */
        if(!ANS_LinkResponseFrame_read(connection, ANS_CMD_RESP_TIMEOUT_MS, &linkRxFrame))
        {
            ret = AnsStatus_SocketReadError;
            break;
        }

        ret = (AnsStatus) linkRxFrame.statusWord;
        if ( AnsStatus_OK != ret )
        {
            break;
        }
    }while(0);

    if(ret != AnsStatus_OK)
    {
        if(connection)
        {
            AnsConnection_destroy(connection);
        }
    }
    else
    {
        AnsBoard_registerConnection(board, connection);
    }

    AnsBoard_unlock(board);

    return ret;
}


void AnsClientPeer_disconnectBoard(struct AnsClientPeer* peer, struct AnsBoard* board)
{
    AnsBoard_lock(board);

    do
    {
        if(board->command_connection)
        {
            AnsConnection_destroy(board->command_connection);
            board->command_connection = NULL;
        }
    }while(0);

    AnsBoard_unlock(board);
}


AnsStatus AnsClientPeer_transmitBoardCommand(struct AnsClientPeer* peer, struct AnsBoard* board,
                                                    AnsCmdFrame* command, AiSize command_size,
                                                    MemChunk* response_memory)
{
    AiUInt32 transactionNo;
    AnsStatus ret;

    VALIDATE_PTR(command, AnsStatus_Error);
    VALIDATE_PTR(response_memory, AnsStatus_Error);


    AnsBoard_lock(board);

    do
    {
        if(!board->command_connection)
        {
            ret = AnsStatus_SocketDisconnected;
            break;
        }

        transactionNo = getNextTransactionNumber();
        command->header.ansHeader.clientId = peer->peer_id;
        command->header.ansHeader.fragmentIndex = 0;
        command->header.ansHeader.fragmentPayloadSize = command_size;
        command->header.ansHeader.transactionId = transactionNo;
        command->header.ansHeader.transactionSize = command_size;

        /*
         * Send the command frame.
         */
        ret = ANS_CmdFrame_send(board->command_connection, command);
        if ( AnsStatus_OK == ret )
        {
            /*
             * Wait for the response from the server.
             */
             ret = ANS_CmdRspFrame_read(board->command_connection, ANS_CMD_RESP_TIMEOUT_MS, transactionNo, response_memory);
        }
    }while(0);

    AnsBoard_unlock(board);

    return ret;
}


AnsStatus AnsClientPeer_openBoardEventStream(struct AnsClientPeer* peer, struct AnsBoard* board,
                                             struct AnsBoardEventObserver** observer)
{
    AiUInt32 command_frame_size;
    MemChunk command_memory;
    MemChunk response_memory;
    AnsCmdFrame* command_frame = NULL;
    AnsStatus ret;
    AnsCmdRspFrame* response_frame = NULL;
    struct AnsOpenBoardEventStreamResponsePayload* response_payload = NULL;
    struct AnsBoardEventObserver* event_observer = NULL;

    MemChunk_init(&command_memory);
    MemChunk_init(&response_memory);

    command_frame_size = sizeof(AnsCmdFrame);

    if (!MemChunk_allocate(&command_memory, command_frame_size))
    {
        return AnsStatus_Error;
    }

    ret = AnsStatus_OK;
    do
    {
        /* Tell the peer to open an event stream for this board */
        command_frame = (AnsCmdFrame*) command_memory.pMemory;
        command_frame->header.commandtype = BoardCommand;
        command_frame->header.functionId = OpenBoardEventStreamID;

        ret = AnsClientPeer_transmitBoardCommand(peer, board, command_frame, command_frame_size, &response_memory);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        response_frame = (AnsCmdRspFrame*) response_memory.pMemory;
        response_payload = (struct AnsOpenBoardEventStreamResponsePayload*) response_frame->payload;

        event_observer = malloc(sizeof(struct AnsBoardEventObserver));
        if(!event_observer)
        {
            ret = AnsStatus_OutOfMemory;
            break;
        }

        ret = AnsBoardEventObserver_init(event_observer, (AiHandle) (AiUIntPtr) response_payload->handle);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        /* Connect the event observer. The returned port number is little endian, hence
         * we need to swap it on big endian systems before giving it to socket layer.
         */
        ret = AnsBoardEventObserver_connect(event_observer, peer->connection->peer_ip.ipAddress,
                                            BSWAP16(response_payload->port));
        if(ret != AnsStatus_OK)
        {
            break;
        }

        AnsBoard_registerEventObserver(board, event_observer);


    }while(0);

    if(ret == AnsStatus_OK && observer)
    {
        *observer = event_observer;
    }

    MemChunk_free(&command_memory);
    MemChunk_free(&response_memory);

    return ret;
}


AnsStatus AnsClientPeer_closeBoardEventStream(struct AnsClientPeer* peer, struct AnsBoard* board,
                                              AiHandle observer_handle)
{
    AiUInt32 command_frame_size;
    MemChunk command_memory;
    MemChunk response_memory;
    AnsCmdFrame* command_frame = NULL;
    AnsStatus ret;
    struct AnsBoardEventObserver* event_observer = NULL;
    struct AnsCloseBoardEventStreamPayload* command_payload = NULL;

    MemChunk_init(&command_memory);
    MemChunk_init(&response_memory);

    command_frame_size = sizeof(ANS_Header) + ANS_CMD_PAYLOAD(struct AnsCloseBoardEventStreamPayload);

    if( !MemChunk_allocate(&command_memory, command_frame_size) )
    {
        return AnsStatus_Error;
    }

    ret = AnsStatus_OK;
    do
    {
        command_frame = command_memory.pMemory;
        command_payload = (struct AnsCloseBoardEventStreamPayload*) command_frame->payload;

        command_frame->header.commandtype = BoardCommand;
        command_frame->header.functionId = CloseBoardEventStreamID;

        command_payload->handle = (AiUInt32) observer_handle;

        ret = AnsClientPeer_transmitBoardCommand(peer, board, command_frame, command_frame_size, &response_memory);
        if(ret != AnsStatus_OK)
        {
            break;
        }

        event_observer = AnsBoard_getEventObserver(board, observer_handle);
        if(!event_observer)
        {
            ret = AnsStatus_Error;
            break;
        }

        AnsBoard_unregisterEventObserver(board, event_observer);
        AnsBoardEventObserver_disconnect(event_observer);

    }while(0);

    MemChunk_free(&command_memory);
    MemChunk_free(&response_memory);

    return ret;
}
