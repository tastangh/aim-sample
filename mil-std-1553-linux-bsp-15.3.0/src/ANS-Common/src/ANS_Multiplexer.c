/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Multiplexer.c
 *
 * This module provides the ANS connection multiplexer thread.
 *
 ******************************************************************************/
#define MODULENAME "ANS_Multiplexer"

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
# include <pthread.h>
#endif

#include "Ai_cdef.h"
#include "Ai_Socket.h"
#include "Ai_Socktl.h"
#include "Ai_Validate.h"
#include "Ai_Defines.h"
#include "ANS_Server.h"
#include "ANS_Types.h"
#include "ANS_AdminWorker.h"
#include "ANS_BoardWorker.h"
#include "ANS_Config.h"
#include "ANS_LinkInitFrame.h"
#include "ANS_Thread.h"
#include "ANS_Log.h"
#include "ANS_Multiplexer.h"


/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/


/* Variables ******************************************************************/

/* Functions ******************************************************************/

/**************************************************************************//**
 * \brief ANS Connection Multiplexer
 *
 * This function realizes the ANS connection multiplexer thread.
 * It processes the initial link initialization frame sent by the ANS client.
 * In case of a board connection requested by the client, this thread
 * forwards the open connection to the board worker thread.
 *
 ******************************************************************************/
extern void *ANS_MultiplexerThread (
    void    *thisarg)   /*!< [in] ThreadArgCommParams argument          */
{
    AiBoolean           boolRc;
    AiBoolean           haveError;
    long int            functionRc = EXIT_FAILURE;
    ANSThreadType       threadType = ANSThreadTypeMultiplexer;
    AnsServerPeerId     peerId = ANS_SERVER_PEER_ID_UNKNOWN;
    ANSThread_MxCallback *pCallback = NULL;
    AnsLinkType         linkType;
    AnsLinkInitFrame    initFrame;
    AnsThreadId         threadId;
    struct AnsServerPeer* peer = NULL;
    struct AnsConnection* connection = (struct AnsConnection*) thisarg;

    VALIDATE_PTR(thisarg, (void *) ((long) EXIT_FAILURE));

    ANSLogDebug(MODULENAME ": Info: Connection multiplexer thread started.");


#if PTHREADS
    threadId = pthread_self();
#elif WIN32
    threadId = GetCurrentThreadId();
#else
    #error Threads not supported
#endif

    for (;;)
    {
        /*
         * Wait for the link initialization frame.
         * Terminate the connection in case the frame is not received in time
         * or invalid data was sent.
         */
        boolRc = ANS_LinkInitFrame_read(connection, ANS_CONFIG_LINK_INIT_READ_TIMEOUT_MS, &initFrame);
        if ( AiFalse == boolRc )
        {
            ANSLogError(MODULENAME ": ERR: Link initialization frame read error - aborting");
            break;
        }
        linkType  = (AnsLinkType) initFrame.linkType;
        peerId  = (AnsServerPeerId) initFrame.peerId;  /* temporary only, if -1 */
        ANSLogDebug(MODULENAME ": INF: New init frame from client=%d, type=%d", (int) peerId, (int) linkType);

        /* Deny any unsupported (future) protocol versions.                     */
        if ( initFrame.protocolVersion.majorVersion != AnsProtocol_getMajorVersion(&g_AnsProtocol))
        {
            ANSLogError(MODULENAME "Incompatible protocol version %d.%d !", initFrame.protocolVersion.majorVersion,
                        initFrame.protocolVersion.minorVersion);
            ANS_LinkResponseFrame_send(linkType, peerId, AnsStatus_IncompatibleProtVer, 0, connection);
            break;
        }

        if(initFrame.protocolVersion.minorVersion > AnsProtocol_getMinorVersion(&g_AnsProtocol))
        {
            ANSLogWarn("Server does not support full client functionality. Update necessary\n");
        }

        /* Ensure that the link type is supported. */
        haveError = AiFalse;
        switch ( linkType )
        {
        case AnsLinkType_AdminChannel:
            pCallback = ANS_AdminWorkerCallback;
            threadType = ANSThreadTypeAdminWorker;
            break;
        case AnsLinkType_BoardChannel:
            pCallback = ANS_BoardWorkerCallback;
            threadType = ANSThreadTypeBoardWorker;
            break;
        default:
            haveError = AiTrue;
            ANSLogError(MODULENAME ": ERROR: Unknown link type %d", (int) linkType);
            ANS_LinkResponseFrame_send(linkType, peerId, AnsStatus_InvalidLinkType,
                                       AnsServer_getHostedBoardCount(&g_AnsServer), connection);
            break;
        }

        if ( haveError )
        {
            break;
        }

        if(peerId != ANS_SERVER_PEER_ID_UNKNOWN)
        {
            peer = AnsServer_requestPeer(&g_AnsServer, peerId);
            if(!peer)
            {
                ANSLogError(MODULENAME ": ERROR: Unknown client id %d", peerId);
                ANS_LinkResponseFrame_send(linkType, peerId, AnsStatus_InvalidPeerId,
                                           AnsServer_getHostedBoardCount(&g_AnsServer), connection);
                break;
            }
        }
        else
        {
            peer = AnsServer_createPeer(&g_AnsServer);
            if(!peer)
            {
                ANSLogError(MODULENAME ": ERROR: Failed to create peer");
                ANS_LinkResponseFrame_send(linkType, peerId, AnsStatus_ClientRegistrationFailure,
                                           AnsServer_getHostedBoardCount(&g_AnsServer), connection);
                break;
            }

            ANSLogDebug("Created new client with id %d", peer->id);
        }

        AnsServerPeer_registerConnection(peer, connection);

        /*
         * Acknowledge the link initialization request now.
         * This completes the link initialization process here.
         */
        boolRc = ANS_LinkResponseFrame_send(linkType, peer->id, AnsStatus_OK, AnsServer_getHostedBoardCount(&g_AnsServer),
                                            connection);
        if ( AiFalse == boolRc )
        {
            ANSLogError(MODULENAME ": ERR: Link initialization response write error - aborting");
            break;
        }

        /*
         * Change the multiplexer thread identity to match the selected one.
         */
        boolRc = ANSThread_update(threadType, ANSThreadStateRunning, threadId, connection);
        if ( AiFalse == boolRc )
        {
            ANSLogError(MODULENAME ": ERR: ANSSThread_update failed - aborting");
            break;
        }

        /*
         * Run the selected worker callback.
         */
        if ( NULL != pCallback )
        {
            functionRc = pCallback(threadId, peer, connection);
        }

        break;
    } /* for */

    if(peer)
    {
        AnsConnection_close(connection);

        AnsServerPeer_removeConnection(peer, connection);

        AnsServer_releasePeer(&g_AnsServer, peer);
    }

    /* Close the connection in case of any errors. */
    if ( EXIT_SUCCESS != functionRc )
    {
        ANSLogError(MODULENAME ": closing connection");
    }

    ANSLogDebug(MODULENAME ": terminating thread, rc=%ld", functionRc);

    return (void *) functionRc; /** \return EXIT_SUCCESS, EXIT_FAILURE */
}


