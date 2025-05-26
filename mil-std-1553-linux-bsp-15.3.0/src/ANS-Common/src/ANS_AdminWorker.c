/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_AdminWorker.c
 *
 * This module provides the ANS administrative commands worker thread.
 *
 *
 ******************************************************************************/
#define MODULENAME "ANS_AdminWorker"

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>




#include "Ai_cdef.h"
#include "Ai_Socket.h"
#include "Ai_Socktl.h"
#include "Ai_Validate.h"
#include "Ai_Defines.h"
#include "Ai_Strncpy.h"
#include "ANS_Log.h"
#include "ANS_CmdFrame.h"
#include "ANS_Client.h"
#include "ANS_Config.h"
#include "ANS_AdminWorker.h"
#include "ANS_Server.h"
#include "ANS_AdminCommands.h"

#if defined(__linux) || defined(_VXBUS)
  #include <sys/utsname.h>
  #include <unistd.h>
#endif
/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/



/* Prototypes *****************************************************************/


static void fillServerInfoTo (/*@out@*/
        AnsGetServerInfoResponsePayload* pServerInfoResponse);





/* Variables ******************************************************************/




/* Functions ******************************************************************/

/*************************************************************************//**
 * \brief Administrative Commands Worker Callback
 *
 * This function realizes the worker thread callback for 
 * administrative ANS commands.
 * It runs in the context of the multiplexer thread.
 *****************************************************************************/
extern long int ANS_AdminWorkerCallback (
    AnsThreadId         threadId,       /*!< [in] Current thread ID         */
    struct AnsServerPeer* peer,           /*!< [in] peer the thread is attached to */
    struct AnsConnection* connection)   /*!< [in] connection used for servicing server commands */

{
    MemChunk			rxMemory;
    MemChunk			txMemory;
    AiBoolean			boolRc;
    AnsCmdFrame		*pCmdFrame;
    AnsCmdRspFrame		*pCmdResponse;
    AnsStatus			status;
    signed long         functionRc = -1;

    VALIDATE_PTR(peer, EXIT_FAILURE);   /** \return EXIT_FAILURE        */
    VALIDATE_PTR(connection, EXIT_FAILURE);   /** \return EXIT_FAILURE        */

    ANSLogDebug(MODULENAME ": DEB: AdminWorkerCallback active.");
    ANSLogDebug( MODULENAME ": DEB: Servicing client %.16s", connection->peer_ip.ipAddress);

    /*
     * Prepare the RX and TX memory chunks. These memory chunks are
     * buffers of variable size that are worked on through the entire
     * lifetime of this thread.
     */
    MemChunk_init(&rxMemory);
    MemChunk_init(&txMemory);

    boolRc = MemChunk_allocate( &rxMemory, 512 );
    if ( boolRc == AiFalse )
    {
        ANSLogError(MODULENAME ": ERR: MemChunk_allocate(RX) failed!");
        return EXIT_FAILURE;
    }

    boolRc = MemChunk_allocate( &txMemory, 512 );
    if ( boolRc == AiFalse )
    {
        MemChunk_free(&rxMemory);
        ANSLogError(MODULENAME ": ERR: MemChunk_allocate(TX) failed!");
        return EXIT_FAILURE;
    }

    for (;;)
    {
        /* Wait for the next command sent by the client */
        pCmdResponse = NULL;

        /* Read Command Frame */
        status = ANS_CmdFrame_read(connection, NTSOCKET_TIMEOUT_INFINITE, &rxMemory);

        /* Check status */
        if ( AnsStatus_SocketDisconnected == status )
        {
            ANSLogDebug(MODULENAME ": DBG: Client disconnected");
            functionRc = EXIT_SUCCESS;
            break;
        }

        if ( status != AnsStatus_OK )
        {
            ANSLogError(MODULENAME ": ERROR: CmdFrame_read error(%d)!", (int) status);
            functionRc = EXIT_FAILURE;
            break;
        }

        pCmdFrame = (AnsCmdFrame *)rxMemory.pMemory;

        if(pCmdFrame->header.commandtype != AdminCommand)
        {
            ANSLogError(MODULENAME ": ERROR: Unexpected Command Type (%u)!", pCmdFrame->header.commandtype);
            functionRc = EXIT_FAILURE;
            break;
        }

        switch(pCmdFrame->header.functionId)
        {
        case GetNumBoardsID:
            if(!AnsServer_getCommandHandlers(&g_AnsServer)->getNumBoards)
            {
                ANSLogError(MODULENAME ": ERROR: No handler for GetNumBoards command available\n");
            }
            else
            {
                pCmdResponse = AnsServer_getCommandHandlers(&g_AnsServer)->getNumBoards(pCmdFrame, &txMemory);
            }
            break;

        case GetServerInfoID:
            if(!AnsServer_getCommandHandlers(&g_AnsServer)->getServerInfo)
            {
                ANSLogError(MODULENAME ": ERROR: No handler for GetServerInfo command available");
            }
            else
            {
                pCmdResponse = AnsServer_getCommandHandlers(&g_AnsServer)->getServerInfo(pCmdFrame, &txMemory);
            }
            break;

        default:
            if(!AnsServer_getCommandHandlers(&g_AnsServer)->protocolLibHandler)
            {
                ANSLogError(MODULENAME ": ERROR: No protocol specific handler for administrative command available");
            }
            else
            {
                pCmdResponse = AnsServer_getCommandHandlers(&g_AnsServer)->protocolLibHandler(pCmdFrame, &txMemory);
            }
            break;
        }

        if ( NULL == pCmdResponse )
        {
            ANSLogError(MODULENAME ": ERROR: Command could not be processed!");
            functionRc = EXIT_FAILURE;
            break;
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
    } 	// End FOR-LOOP

    /* Release the RX and TX buffers */
    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    AnsServerPeer_closeConnections(peer);

    ANSLogInfo( MODULENAME ": INF: Disconnecting from client %.16s", connection->peer_ip.ipAddress);

    return functionRc;              /** \return EXIT_SUCCESS    */
}



/**************************************************************************//**
 * Fill the server information structure with all required data.
 *****************************************************************************/
static void fillServerInfoTo (
        AnsGetServerInfoResponsePayload* pServerInfoResponse) /*!< [out] Server information */
{
    char hostname[512/*HOST_NAME_MAX*/];
#ifndef WIN32
    struct utsname osInfo; 
#endif

    memset(pServerInfoResponse, 0, sizeof(*pServerInfoResponse));

    /* TODO: ANS-Common Lib should provide functions to get host name and OS info
     * instead of using values from global settings
     */

    if(gethostname(hostname, sizeof(hostname)))
    {
        ANSLogError("Failed to determine host name");
    }
    else
    {
        STRNCPY(pServerInfoResponse->hostName, hostname, sizeof(pServerInfoResponse->hostName));
    }

#ifndef WIN32
    if(uname(&osInfo))
    {
        ANSLogError("Failed to determine OS info");
    }
    else
    {
        int osInfoRemaining = sizeof(pServerInfoResponse->osInfo) - 1;
        STRNCPY(pServerInfoResponse->osInfo, osInfo.release, osInfoRemaining);
        osInfoRemaining = sizeof(pServerInfoResponse->osInfo) - strlen(pServerInfoResponse->osInfo) - 1;
        strncat(pServerInfoResponse->osInfo, " ", osInfoRemaining);
        osInfoRemaining = sizeof(pServerInfoResponse->osInfo) - strlen(pServerInfoResponse->osInfo) - 1;
        strncat(pServerInfoResponse->osInfo, osInfo.version, osInfoRemaining);
        osInfoRemaining = sizeof(pServerInfoResponse->osInfo) - strlen(pServerInfoResponse->osInfo) - 1;
        strncat(pServerInfoResponse->osInfo, " ", osInfoRemaining);
        osInfoRemaining = sizeof(pServerInfoResponse->osInfo) - strlen(pServerInfoResponse->osInfo) - 1;
        strncat(pServerInfoResponse->osInfo, osInfo.machine, osInfoRemaining);
    }
#else
    {
        OSVERSIONINFO osvi;

        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        GetVersionEx(&osvi);
        sprintf(pServerInfoResponse->osInfo, "Windows %d.%d.%d)", osvi.dwMajorVersion, 
                                                                  osvi.dwMinorVersion, 
                                                                  osvi.dwBuildNumber);

    }
#endif

    STRNCPY(pServerInfoResponse->serverInfo.name, AnsServer_getName(&g_AnsServer), sizeof(pServerInfoResponse->serverInfo.name));
    STRNCPY(pServerInfoResponse->serverInfo.description, AnsServer_getDescription(&g_AnsServer), sizeof(pServerInfoResponse->serverInfo.description));

    pServerInfoResponse->serverVersion.majorVersion = AnsServer_getMajorVersion(&g_AnsServer);
    pServerInfoResponse->serverVersion.minorVersion = AnsServer_getMinorVersion(&g_AnsServer);
    pServerInfoResponse->serverVersion.patchVersion = AnsServer_getPatchVersion(&g_AnsServer);
    STRNCPY(pServerInfoResponse->serverVersion.versionString, AnsServer_getVersion(&g_AnsServer), sizeof(pServerInfoResponse->serverVersion.versionString));

    pServerInfoResponse->protocolVersion.majorVersion = AnsProtocol_getMajorVersion(&g_AnsProtocol);
    pServerInfoResponse->protocolVersion.minorVersion = AnsProtocol_getMinorVersion(&g_AnsProtocol);
}


AnsCmdRspFrame* ANS_GetNumBoardsHandler(AnsCmdFrame* cmdFrame, MemChunk* responseMemory)
{
    AiUInt32 payloadSize = 0;
    AnsStatus ansStatus = AnsStatus_Error;
    AnsCmdRspFrame* responseFrame = NULL;
    AnsGetNumBoardsResponsePayload* numBoardsResponse = NULL;

    ANSLogDebug(MODULENAME ": command: GetNumBoards");

    payloadSize = ANS_RSP_PAYLOAD(AnsGetNumBoardsResponsePayload);

    ansStatus =  ANS_Header_create(cmdFrame->header.ansHeader.transactionId, payloadSize, 0, payloadSize,
                                   cmdFrame->header.ansHeader.clientId, responseMemory);
    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame*) responseMemory->pMemory;
    responseFrame->header.functionId = cmdFrame->header.functionId;
    responseFrame->header.status     = (AiUInt32) AnsStatus_OK;

    numBoardsResponse = (AnsGetNumBoardsResponsePayload*) responseFrame->payload;

    numBoardsResponse->numBoards = AnsServer_getHostedBoardCount(&g_AnsServer);

    return responseFrame;
}


AnsCmdRspFrame* ANS_GetServerInfoHandler(AnsCmdFrame* cmdFrame, MemChunk* responseMemory)
{
    AiUInt32 payloadSize = 0;
    AnsStatus ansStatus = AnsStatus_Error;
    AnsCmdRspFrame* responseFrame = NULL;
    AnsGetServerInfoResponsePayload* serverInfoResponse = NULL;

    ANSLogDebug(MODULENAME ": command: GetServerInfo");
    payloadSize = ANS_RSP_PAYLOAD(AnsGetServerInfoResponsePayload);

    ansStatus =  ANS_Header_create(cmdFrame->header.ansHeader.transactionId, payloadSize,
                                   0, payloadSize, cmdFrame->header.ansHeader.clientId, responseMemory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

             /* Add the response payload. */
    responseFrame = (AnsCmdRspFrame*) responseMemory->pMemory;

    responseFrame->header.functionId = GetServerInfoID;
    responseFrame->header.status     = (AiUInt32) AnsStatus_OK;

    serverInfoResponse = (AnsGetServerInfoResponsePayload*) responseFrame->payload;

    fillServerInfoTo(serverInfoResponse);

    return responseFrame;
}
