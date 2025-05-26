/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Client.cpp
 *
 * This file contains ANS client management functions.
 *
 *
 ******************************************************************************/


#define MODULENAME "ANS_Client"

/* Includes *******************************************************************/

#include <stdlib.h>


#include "Ai_Socket.h"
#include "Ai_Socktl.h"
#include "Ai_Sockll.h"
#include "Ai_Sem.h"
#include "ANS_CmdFrame.h"
#include "ANS_LinkInitFrame.h"
#include "Ai_cdef.h"
#include "Ai_Validate.h"
#include "ANS_MemChunk.h"
#include "Ai_NotUsed.h"
#include "ANS_Log.h"
#include "ANS_Frame.h"
#include "Ai_cdef.h"
#include "Ai_Strncpy.h"
#include "ANS_Client.h"
#include "ANS_Thread.h"
#include "Ai_Defines.h"
#include "ANS_AdminCommands.h"
#include "Ai_mutex.h"



#ifdef WIN32
    #pragma warning(disable:4127)   /* conditional expression is constant */
    #pragma warning( disable : 4996 ) /* sscanf warning */
#endif

/* Constants ******************************************************************/

#define TIMEOUT_WAIT          ANS_CMD_RESP_TIMEOUT_MS   //!< Semaphore wait time-out [ms]


#define ANS_URL_PORT_DELIM ":"


/* Macros *********************************************************************/



/* Type definitions ***********************************************************/


/* Prototypes *****************************************************************/



struct AnsClient g_AnsClient;


/* Variables ******************************************************************/





/* Functions ******************************************************************/


AnsStatus AnsClient_init(struct AnsClient* client)
{
    /* Ensure that the underlying socket layer is initialized. */
    if ( ntsocket_startup() != 0)
    {
        ANSLogError("ntsocket_startup failed (WSA version problem?)!\n");
        return AnsStatus_Error;
    }

    AI_LIST_INIT(client->peers);
    client->peer_lock = ai_mutex_create();
    if(!client->peer_lock)
    {
        return AnsStatus_Error;
    }


    return AnsStatus_OK;
}


struct AnsClientPeer* AnsClient_requestPeer(struct AnsClient* client, const char* peer_address, unsigned short port)
{
    struct AnsClientPeer* current;

    ai_mutex_lock(client->peer_lock);

    ai_list_for_each_entry(current, &client->peers, struct AnsClientPeer, list)
    {
        if(!strcmp(current->address, peer_address) && current->port == port)
        {
            current->reference_count++;
            ai_mutex_release(client->peer_lock);
            return current;
        }
    }

    ai_mutex_release(client->peer_lock);

    return NULL;
}


struct AnsClientPeer* AnsClient_createPeer(struct AnsClient* client, const char* peer_address, unsigned short port)
{
    struct AnsClientPeer* peer;

    peer = malloc(sizeof(struct AnsClientPeer));
    if(!peer)
    {
        return NULL;
    }

    AnsClientPeer_init(peer, peer_address, port);

    peer->reference_count++;

    ai_mutex_lock(client->peer_lock);

    ai_list_add_tail(&peer->list, &client->peers);

    ai_mutex_release(client->peer_lock);

    return peer;
}


extern void AnsClient_releasePeer(struct AnsClient* client, struct AnsClientPeer* peer)
{
    int ret;

    ret = ai_mutex_lock(client->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("peer list locking failed with %d\n", ret);
        return;
    }

    peer->reference_count--;

    /* Check if we are the only one that holds a reference to the peer and destroy it in this case */
    if(peer->reference_count == 1)
    {
        ai_list_del(&peer->list);
        AnsClientPeer_destroy(peer);
    }

    ret = ai_mutex_release(client->peer_lock);
    if(ret != AI_MUTEX_OK)
    {
        ANSLogError("peer list unlocking failed with %d\n", ret);
    }
}


AnsStatus AnsClient_translateServerURL(const char* url, char* peer_address, size_t addr_len,
                                       unsigned int* port)
{
    char temp_url[ANS_CLIENT_PEER_MAX_ADDR_LEN];
    char* save_ptr;
    char* token;

    STRNCPY(temp_url, url, sizeof(temp_url));

    if (!(token = STRTOK(temp_url, ANS_URL_PORT_DELIM, &save_ptr)))
    {
        return AnsStatus_Error;
    }

    STRNCPY(peer_address, token, addr_len);

    if (!(token = STRTOK(NULL, ANS_URL_PORT_DELIM, &save_ptr)))
    {
        *port = 0;
    }
    else
    {
        if(sscanf(token, "%u", port) != 1)
        {
            return AnsStatus_Error;
        }
    }

    return AnsStatus_OK;
}


AnsStatus AnsClient_discover( int port )
{
    int                     ret;
    NTSOCKET                bcast_socket;
    AnsDiscoverRequest      bcast_buffer;
    int                     written;

    /* -- create transmit port --- */
    bcast_buffer.magic = ANS_DISCOVER_REQUST_MAGIC;
    bcast_buffer.port  = port;

    if (ntsocket_create_udp(&bcast_socket, port) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to create rcv UDP socket");
        return AnsStatus_Error;
    }

    ret = ntsocket_sendto(&bcast_socket, "255.255.255.255", 1553, &bcast_buffer, sizeof(bcast_buffer), &written);

    if (ret != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to send UDP packet");
        return AnsStatus_Error;
    }


    if (ntsocket_destroy(&bcast_socket) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to destroy xmt UDP socket");
        return AnsStatus_Error;
    }

    return AnsStatus_OK;
}




/*! \brief ANS Server Thread
*
* This function implements the ANS Server Thread.
* Its task is to accept any incoming connection on the TCP server
* and delegate further processing to the multiplexer thread.
*/
extern void *AnsClient_discover_thread(void *thisarg) /*!< [in] unused default parameter */
{
    long int                  ret = NTSOCKET_RET_OK;
    NTSOCKET                  bcast_socket;
    AnsDiscoverRespons        bcast_buffer;
    int                       bcast_received;
    char                      sender[65];
    AnsDiscoverThreadConfig * config = (AnsDiscoverThreadConfig*)thisarg;

    memset(&bcast_buffer, 0, sizeof(bcast_buffer));

    ANSLogInfo(MODULENAME ": UDP Broadcast request receiver started");

    if (ntsocket_create_udp(&bcast_socket, 1) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to create UDP socket");
        return (void *)EXIT_FAILURE;
    }

    if (ntsocket_bind_udp_bcast(&bcast_socket, config->port) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to bind UDP socket");
        return (void *)EXIT_FAILURE;
    }

    for (;;)
    {
        ret = ntsockll_peek(bcast_socket.socket, 500, PEEK_MODE_READ);

        if (NTSOCKET_RET_OK != ret)
            if (config->active)
                continue;
            else
                break;

        memset(&bcast_buffer, 0, sizeof(bcast_buffer));

        ret = ntsocket_recvfrom(&bcast_socket, &bcast_buffer, sizeof(bcast_buffer), &bcast_received, sender);

        if (ret != NTSOCKET_RET_OK)
        {
            ANSLogError(MODULENAME ": Failed receive UDP packet");
            break;
        }

        if (bcast_buffer.magic == ANS_DISCOVER_RESPONSE_MAGIC)
            config->callback( sender, &bcast_buffer );
    }

    if (ntsocket_destroy(&bcast_socket) != NTSOCKET_RET_OK)
    {
        ANSLogError(MODULENAME ": Failed to destroy UDP socket");
        return (void *)EXIT_FAILURE;
    }

    return (void *)ret;
}

AnsStatus AnsClient_discover_thread_start(AnsDiscoverThreadConfig * config)
{

    int rc;
    ANSThreadStatus threadStatus;

    /* Initialize the thread table. */
    threadStatus = ANSThread_initializeModule();

    if (ANSThreadStatusOK != threadStatus)
    {
        ANSLogError("ANSThread_initalizeModule error (%d)!", threadStatus);
        return AnsStatus_Error;
    }

    rc = ntsocket_startup();
    if (0 != rc)
    {
        ANSLogError("ERROR: ntsocket_startup failed with error %d!", rc);
        return AnsStatus_Error;
    }

    config->active = AiTrue;
    threadStatus = ANSThread_createThread(AnsClient_discover_thread, (void*)config, ANSThreadTypeNone, AiFalse, &config->threadTableIndex);

    if (ANSThreadStatusOK != threadStatus)
    {
        ANSLogError("ERROR: ANSThread_createThread error (%d) for UDP packets!", (int)threadStatus);
    }

    return AnsStatus_OK;
}

AnsStatus AnsClient_discover_thread_stop(AnsDiscoverThreadConfig * config)
{

    int rc;
    void* threadRc = (void *)EXIT_SUCCESS;
    ANSThreadStatus threadStatus;

    /* Initialize the thread table. */
    threadStatus = ANSThread_initializeModule();

    if (ANSThreadStatusOK != threadStatus)
    {
        ANSLogError("ANSThread_initalizeModule error (%d)!", threadStatus);
        return AnsStatus_Error;
    }

    /* Signal thread termination. */
    config->active = AiFalse;

    /* Wait for the server thread to terminate. */
    rc = ANSThread_join(config->threadTableIndex, &threadRc);

    if (0 != rc)
    {
        ANSLogError("ANSThread_join(server) failed (%d) for UDP packets thread!", rc);
        return EXIT_FAILURE;
    }


    return AnsStatus_OK;
}