/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Server.c
 *
 * This module provides the ANS server thread.
 *
 *
 ******************************************************************************/
#define MODULENAME "ANS_Server"

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
# include <pthread.h>
#endif
#include <string.h>

#include "Ai_Socket.h"
#include "ANS_Multiplexer.h"
#include "ANS_Log.h"
#include "ANS_Server.h"
#include "ANS_Thread.h"
#include "Ai_cdef.h"
#include "Ai_Sockll.h"
#include "Ai_Socksv.h"
#include "Ai_Validate.h"
#include "Ai_Defines.h"
#include "Ai_Socktl.h"
#include "Ai_Sem.h"
#include "Ai_Strncpy.h"
#include "ANS_Connection.h"
#include "ANS_BoardWorker.h"
#include "ANS_AdminWorker.h"

/* Constants ******************************************************************/

#define TIMEOUT_WAIT   ANS_CMD_RESP_TIMEOUT_MS   /*!< Semaphore wait time-out [ms]           */


  /*!< Max. number of ANS clients
       supported by the system        */
#define ANS_CONFIG_MAX_CLIENTS     (16)



/* Macros *********************************************************************/


/* Type definitions ***********************************************************/




/* Prototypes *****************************************************************/
/* Variables ******************************************************************/



struct AnsServer g_AnsServer;


/*!< Default board command handlers for a server */
struct AnsBoardCommandHandlers g_DefaultBoardCommandHandlers =
{
        NULL, /* open */
        NULL, /* close */
        NULL, /* readMemory */
        NULL, /* writeMemory */
        NULL, /* ioCommand = NULL */
        NULL, /* ioCommandStruct */
        NULL, /* getDriverVersion = NULL */
        NULL, /* getSoftwareVersions */
        NULL, /* getDriverInfo */
        NULL, /* getBoardMemSize */
        ANS_OpenBoardEventStream,   /* Open board event stream */
        ANS_CloseBoardEventStream,  /* Close board event stream */
        NULL, /* registerCallback */
        NULL, /* unregisterCallback */
        NULL, /* protocolHandler = NULL */
};


/*!< Default command handlers for a server */
struct AnsCommandHandlers g_DefaultCommandHandlers =
{
        ANS_GetNumBoardsHandler, /* getNumBoards */
        ANS_GetServerInfoHandler, /* getServerInfo */
        NULL, /* protocolLibHandler */
};




/* Functions ******************************************************************/

/*! \brief ANS Server Thread
*
* This function implements the ANS Server Thread.
* Its task is to accept any incoming connection on the TCP server
* and delegate further processing to the multiplexer thread.
*/
extern void *AnsServer_thread(void *thisarg) /*!< [in] unused default parameter */
{
    long int                functionRc = EXIT_SUCCESS;
    int                     threadIndex = -1;
    ANSThreadStatus         threadStatus;
    NTSOCKET                srv_socket;
    NTSOCKET_IPADDRESS      peerIpAddress;
    NTSOCKET_RET            ntSocketRc;
    struct AnsConnection* connection;

    (void)memset(&peerIpAddress, 0, sizeof(peerIpAddress));

    /* Create a TCP socket. */
    ntSocketRc = ntsocket_create(&srv_socket, NTSOCKET_STYLE_TCP);
    if (NTSOCKET_RET_OK != ntSocketRc)
    {
        ANSLogError(MODULENAME ": ERROR: ntsocket_create failed with error %d!", (int)ntSocketRc);
        return (void *)EXIT_FAILURE;
    }

    /*
    * Transform the TCP socket into a lisitening server socket that provides
    * the interface in direction to the ANS clients.
    */
    ntSocketRc = ntsocket_createserver(&srv_socket, g_AnsServer.port, 5);
    if (NTSOCKET_RET_OK != ntSocketRc)
    {
        ANSLogError(MODULENAME ": ERROR: ntsocket_createserver failed with error %d!", (int)ntSocketRc);
        return (void *)EXIT_FAILURE;
    }

    ANSLogInfo(MODULENAME ": Info: listening on port=%d", g_AnsServer.port);


    AnsServer_send_info(&g_AnsServer, 1, "255.255.255.255", g_AnsServer.client_port);

    for (;;)
    {
        /* Allocate Memory for the new connection descriptor.   */
        connection = (struct AnsConnection*) malloc(sizeof(struct AnsConnection));
        if (!connection)
        {
            /* If out of memory, shutdown the system.   */
            ANSLogError(MODULENAME ": ERROR: out of memory error! - exiting");
            functionRc = EXIT_FAILURE;
            break;
        }

        if (AnsConnection_init(connection) != AnsStatus_OK)
        {
            ANSLogError("Failed to initialize connection");
            functionRc = EXIT_FAILURE;
            break;
        }


        if (AnsConnection_accept(connection, &srv_socket, NTSOCKET_TIMEOUT_INFINITE) != AnsStatus_OK)
        {
            ANSLogError("Failed to accept connection");
            functionRc = EXIT_FAILURE;
            break;
        }

        /*
        * Create a new multiplexer thread instance that processes the incoming
        * connection request.
        */
        threadStatus = ANSThread_createThread(
            ANS_MultiplexerThread, connection,
            ANSThreadTypeMultiplexer, AiTrue, &threadIndex);

        if (ANSThreadStatusOK != threadStatus)
        {
            ANSLogError(MODULENAME ": ERROR: ANSThread_createThread error (%d)!", (int)threadStatus);
            functionRc = EXIT_FAILURE;
            break;
        }

    } /* for */

    /* Shutdown the server socket */
    (void)ntsocket_destroy(&srv_socket);

    return (void *)functionRc; /** \return EXIT_SUCCESS, EXIT_FAILURE */
}


/*! \brief ANS Server Thread
*
* This function implements the ANS Server Thread.
* Its task is to accept any incoming connection on the TCP server
* and delegate further processing to the multiplexer thread.
*/
extern void *AnsServer_receive_broadcast_request(void *thisarg) /*!< [in] unused default parameter */
{
    long int                ret = NTSOCKET_RET_OK;
    NTSOCKET                bcast_socket;
    AnsDiscoverRequest      bcast_buffer;
    int                     bcast_received;
    char                    sender[65];

    memset(&bcast_buffer, 0, sizeof(bcast_buffer));

    ANSLogInfo(MODULENAME ": UDP Broadcast request receiver started");

    if (ntsocket_create_udp(&bcast_socket, 1) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to create UDP socket");
        return (void *)EXIT_FAILURE;
    }

    if (ntsocket_bind_udp_bcast(&bcast_socket, g_AnsServer.port) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to bind UDP socket");
        return (void *)EXIT_FAILURE;
    }

    for(;;)
    {
        memset(&bcast_buffer, 0, sizeof(bcast_buffer) );

        ret = ntsocket_recvfrom(&bcast_socket, &bcast_buffer, sizeof(bcast_buffer), &bcast_received, sender );

        if (ret != NTSOCKET_RET_OK)
        {
            ANSLogError(MODULENAME ": Failed receive UDP packet");
        }


        if (bcast_received == sizeof(AnsDiscoverRequest))
        {
            if (bcast_buffer.magic == ANS_DISCOVER_REQUST_MAGIC)
            {
                AnsServer_send_info(&g_AnsServer, 0, sender, bcast_buffer.port);
            }
        }
    }

    if (ntsocket_destroy(&bcast_socket) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to destroy UDP socket");
        return (void *)EXIT_FAILURE;
    }

    return (void *)ret;
}


void AnsServer_init(struct AnsServer* server)
{
    AI_LIST_INIT(server->connected_peers);
    server->peer_lock = ai_mutex_create();

    AI_LIST_INIT(server->open_boards);
    server->board_lock = ai_mutex_create();

    AnsServer_setCommandHandlers(server, &g_DefaultCommandHandlers);
    AnsServer_setBoardCommandHandlers(server, &g_DefaultBoardCommandHandlers);
}


AnsStatus AnsServer_send_info(const struct AnsServer* server, int broadcast, const char * receiver, int port)
{
    int                     ret;
    NTSOCKET                bcast_socket;
    AnsDiscoverRespons      bcast_buffer;
    char                    host_name[32];
    int                     bcast_written;

    memset(host_name, 0, sizeof(host_name));
    memset(&bcast_buffer, 0, sizeof(bcast_buffer));

    gethostname(bcast_buffer.host_name, sizeof(host_name));

    bcast_buffer.magic          = ANS_DISCOVER_RESPONSE_MAGIC;
    bcast_buffer.protocol       = (g_AnsProtocol.magic>>16) & 0xFFFF; /* The protocol is encoded into the higer 16 bits of the ANS magic */
    bcast_buffer.port           = server->port;
    bcast_buffer.board_count    = server->hosted_board_count;
    bcast_buffer.protocol_major = g_AnsProtocol.version.majorVersion;
    bcast_buffer.protocol_minor = g_AnsProtocol.version.minorVersion;

    if (ntsocket_create_udp(&bcast_socket, broadcast) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to create UDP socket");
        return AnsStatus_Error;
    }

    ret = ntsocket_sendto(&bcast_socket, receiver, port, &bcast_buffer, sizeof(bcast_buffer), &bcast_written);

    if (ret != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to connect UDP socket");
        return AnsStatus_Error;
    }


    if (ntsocket_destroy(&bcast_socket) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to destroy UDP socket");
        return AnsStatus_Error;
    }

    return 0;
}


struct AnsServerPeer* AnsServer_requestPeer(const struct AnsServer* server, AnsServerPeerId id)
{
    int ret;
    struct AnsServerPeer* current;

    ret = ai_mutex_lock(server->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list locking failed with %d\n", ret);
        return NULL;
    }

    ai_list_for_each_entry(current, &server->connected_peers, struct AnsServerPeer, list)
    {
        if(current->id == id)
        {
            /* Peer found, increase its reference count */
            current->reference_count++;
            ret = ai_mutex_release(server->peer_lock);
            if(ret != AI_MUTEX_OK)
            {
                ANSLogError("Client list unlocking failed with %d\n", ret);
                return NULL;
            }
            return current;
        }
    }

    ret = ai_mutex_release(server->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list unlocking failed with %d\n", ret);
        return NULL;
    }

    return NULL;
}


struct AnsServerPeer* AnsServer_createPeer(struct AnsServer* server)
{
    struct AnsServerPeer* peer;
    static AnsServerPeerId client_ids = 0;
    int ret;

    peer = malloc(sizeof(struct AnsServerPeer));
    if(!peer)
    {
        return NULL;
    }

    AnsServerPeer_init(peer, ++client_ids);

    ret = ai_mutex_lock(server->peer_lock);
    if(ret)
    {
        ANSLogError("Client list locking failed with %d\n", ret);
        return NULL;
    }

    ai_list_add_tail(&peer->list, &server->connected_peers);
    peer->reference_count++;

    ret = ai_mutex_release(server->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list unlocking failed with %d\n", ret);
        return NULL;
    }

    return peer;
}


extern void AnsServer_releasePeer(struct AnsServer* server, struct AnsServerPeer* peer)
{
    int ret;

    ret = ai_mutex_lock(server->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list locking failed with %d\n", ret);
        return;
    }

    peer->reference_count--;

    /* Check if we are the only one that holds a reference to the peer and destroy it in this case */
    if(peer->reference_count == 1)
    {
        ai_list_del(&peer->list);
        AnsServerPeer_destroy(peer);
    }

    ret = ai_mutex_release(server->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list unlocking failed with %d\n", ret);
    }

}


struct AnsBoard* AnsServer_requestBoard(struct AnsServer* server, AiHandle handle)
{
    int ret;
    struct AnsBoard* current;

    ret = ai_mutex_lock(server->board_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Board list locking failed with %d\n", ret);
        return NULL;
    }

    ai_list_for_each_entry(current, &server->open_boards, struct AnsBoard, list)
    {
        if(current->handle == handle)
        {
            /* Board found, increase its reference count */
            current->reference_count++;
            ret = ai_mutex_release(server->board_lock);
            if(ret != AI_MUTEX_OK)
            {
                ANSLogError("Board list unlocking failed with %d\n", ret);
                return NULL;
            }
            return current;
        }
    }

    ret = ai_mutex_release(server->board_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Board list unlocking failed with %d\n", ret);
        return NULL;
    }

    return NULL;
}


struct AnsBoard* AnsServer_createBoard(struct AnsServer* server, AiHandle handle)
{
    struct AnsBoard* board;
    int ret;

    board = malloc(sizeof(struct AnsBoard));
    if(!board)
    {
        return NULL;
    }

    AnsBoard_init(board, handle);

    ret = ai_mutex_lock(server->board_lock);
    if(ret)
    {
        ANSLogError("Board list locking failed with %d\n", ret);
        return NULL;
    }

    ai_list_add_tail(&board->list, &server->open_boards);
    board->reference_count++;

    ret = ai_mutex_release(server->board_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Client list unlocking failed with %d\n", ret);
        return NULL;
    }

    return board;
}


void AnsServer_releaseBoard(struct AnsServer* server, struct AnsBoard* board)
{
    int ret;

    ret = ai_mutex_lock(server->board_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Board list locking failed with %d\n", ret);
        return;
    }

    board->reference_count--;

    /* Check if we are the only one that holds a reference to the board and destroy it in this case */
    if(board->reference_count == 1)
    {
        ai_list_del(&board->list);
        AnsBoard_destroy(board);
    }

    ret = ai_mutex_release(server->board_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("Board list unlocking failed with %d\n", ret);
    }
}


void AnsServer_setPort(struct AnsServer* server, AiUInt16 port)
{
    server->port = port;
}


void AnsServer_setClientPort(struct AnsServer* server, AiUInt16 port)
{
    server->client_port = port;
}


void AnsServer_setInfo(struct AnsServer* server, const struct AnsServerInfo* new_info)
{
    struct AnsServerInfo* current_info = &server->info;

    STRNCPY(current_info->name, new_info->name, sizeof(current_info->name));
    STRNCPY(current_info->description, new_info->description, sizeof(current_info->description));
}


void AnsServer_setVersion(struct AnsServer* server, const struct AnsServerVersion* new_version)
{
    struct AnsServerVersion* current_version = &server->version;

    current_version->majorVersion = new_version->majorVersion;
    current_version->minorVersion = new_version->minorVersion;
    current_version->patchVersion = new_version->patchVersion;
    STRNCPY(current_version->versionString, new_version->versionString, sizeof(current_version->versionString));
}


void AnsServer_setBoardCommandHandlers(struct AnsServer* server, const struct AnsBoardCommandHandlers* new_handlers)
{
    if( new_handlers->open )
    {
        ANSLogDebug("Setting new handler for open command");
        server->board_command_handlers.open = new_handlers->open;
    }

    if( new_handlers->close )
    {
        ANSLogDebug("Setting new handler for close command");
        server->board_command_handlers.close = new_handlers->close;
    }

    if( new_handlers->readMemory )
    {
        ANSLogDebug("Setting new handler for read Memory command");
        server->board_command_handlers.readMemory = new_handlers->readMemory;
    }

    if( new_handlers->writeMemory )
    {
        ANSLogDebug("Setting new handler for write memory command");
        server->board_command_handlers.writeMemory = new_handlers->writeMemory;
    }

    if( new_handlers->ioCommand )
    {
        ANSLogDebug("Setting new handler for io command");
        server->board_command_handlers.ioCommand = new_handlers->ioCommand;
    }

    if( new_handlers->ioCommandStruct )
    {
        ANSLogDebug("Setting new handler for structure based io command");
        server->board_command_handlers.ioCommandStruct = new_handlers->ioCommandStruct;
    }

    if( new_handlers->getDriverVersion )
    {
        ANSLogDebug("Setting new handler for 'get driver version' command");
        server->board_command_handlers.getDriverVersion = new_handlers->getDriverVersion;
    }

    if( new_handlers->getSoftwareVersions )
    {
        ANSLogDebug("Setting new handler for 'get software versions command' command");
        server->board_command_handlers.getSoftwareVersions = new_handlers->getSoftwareVersions;
    }

    if( new_handlers->getDriverInfo )
    {
        ANSLogDebug("Setting new handler for 'get driver info' command");
        server->board_command_handlers.getDriverInfo = new_handlers->getDriverInfo;
    }

    if( new_handlers->getBoardMemSize )
    {
        ANSLogDebug("Setting new handler for 'get board mem size' command");
        server->board_command_handlers.getBoardMemSize = new_handlers->getBoardMemSize;
    }

    if( new_handlers->openBoardEventStream )
    {
        ANSLogDebug("Setting new handler for 'open board event stream' command");
        server->board_command_handlers.openBoardEventStream = new_handlers->openBoardEventStream;
    }

    if( new_handlers->closeBoardEventStream )
    {
        ANSLogDebug("Setting new handler for 'close board event stream' command");
        server->board_command_handlers.closeBoardEventStream = new_handlers->closeBoardEventStream;
    }

    if( new_handlers->registerCallback )
    {
        ANSLogDebug("Setting new handler for 'register callback' command");
        server->board_command_handlers.registerCallback = new_handlers->registerCallback;
    }

    if( new_handlers->unregisterCallback )
    {
        ANSLogDebug("Setting new handler for 'unregister callback' command");
        server->board_command_handlers.unregisterCallback = new_handlers->unregisterCallback;
    }

    if( new_handlers->protocolHandler )
    {
        ANSLogDebug("Setting new handler for protocol specific commands");
        server->board_command_handlers.protocolHandler = new_handlers->protocolHandler;
    }
}


void AnsServer_setHostedBoardCount(struct AnsServer* server, AiUInt32 num_boards)
{
    server->hosted_board_count = num_boards;
}


void AnsServer_setCommandHandlers(struct AnsServer* server, const struct AnsCommandHandlers* handlers)
{
    if( handlers->getNumBoards )
    {
        ANSLogDebug("Setting new handler for GetNumBoards command");
        server->command_handlers.getNumBoards = handlers->getNumBoards;
    }

    if( handlers->getServerInfo )
    {
        ANSLogDebug("Setting new handler for GetServerInfo command");
        server->command_handlers.getServerInfo = handlers->getServerInfo;
    }

    if( handlers->protocolLibHandler )
    {
        ANSLogDebug("Setting new protocol specific handler for administrative commands");
        server->command_handlers.protocolLibHandler = handlers->protocolLibHandler;
    }
}




