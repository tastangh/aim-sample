/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Net_io.c
 *
 * NET io functions for libraries with ANS-Client implementations
 * 
 */

#include "net_io.h"
#include "Ai_container.h"
#include "ANS_Client.h"
#include "ANS_ClientPeer.h"
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "mil/ANS1553_CommonSettings.h"
#include "mil/ANS1553_BoardCommands.h"
#include "mil/ANS1553_ProtocolCommands.h"
#include "ANS_Protocol.h"
#include "ANS_Log.h"
#include "ANS_Thread.h"
#include "AiOs.h"
#include "Ai1553i_def.h"


/*! \def ANS_MAX_SERVERS
* Maximum number of server connections the Net layer supports
*/
#define ANS_MAX_SERVERS 8


/*! \def ANS_SERVER_INDEX_TO_ID
* Get the server ID from a server index in \ref g_mil_client_peers
*/
#define ANS_SERVER_INDEX_TO_ID(index) (index + 1)


/*! \def ANS_SERVER_ID_TO_INDEX
* Get server ID of a specific index in \ref g_mil_client_peers
*/
#define ANS_SERVER_ID_TO_INDEX(id) (id - 1)



/*! \def ANS_SERVER_ID_CLEAR
* Clear a specific server ID from \ref g_mil_client_peers
*/
#define ANS_SERVER_ID_CLEAR(server_id) (g_mil_client_peers[ANS_SERVER_ID_TO_INDEX(server_id)] = NULL)


/*! \def ANS_SERVER_ID_SET
* Attach an AnsClientPeer struct to a specific server ID using \ref g_mil_client_peers
*/
#define ANS_SERVER_ID_SET(server_id, peer) (g_mil_client_peers[ANS_SERVER_ID_TO_INDEX(server_id)] = peer)


/*! \def ANS_SERVER_ID_TO_PEER
* Get the AnsClientPeer struct from a specific server ID
*/
#define ANS_SERVER_ID_TO_PEER(server_id) (g_mil_client_peers[ANS_SERVER_ID_TO_INDEX(server_id)])


/*! \def ANS_BOARD_HANDLE
* Converts a tuple of server ID and board ID to a module handle
*/
#define ANS_BOARD_HANDLE(server_id, board_id) (AiHandle) (AiUIntPtr) ((server_id << API_SERVER_POS) | board_id)




static char buf[2000];


/*!< Holds AnsClientPeer objects for each connected server */
static struct AnsClientPeer* g_mil_client_peers[ANS_MAX_SERVERS];


/*!< Indicates if network layer has been initialized */
static AiBoolean g_mil_net_initialized = AiFalse;


/*! \struct net_layer_properties
*
* This structure contains properties that are specific for remote devices
* attached over ANS
*/
struct mil_net_layer_properties
{
    struct AnsBoardEventObserver* event_observer; /*!< Used to receive board events over a socket based connection */
    struct ai_mutex* lock;                        /*!< Used for synchronizing net layer access to the device */
};


/*! \brief Establish a new server connection

This function will try to resolve the given network address and then to connect
to the ANS server on the remote peer. \n
Once the connection is established an administration channel to the server will
be opened.\n
Then the number of MIL boards on the server is determined. \n
The function returns a new, unique ID for the server.
\param pNetworkAddress either IP string e.g. 192.168.0.1 or a resolvable host name
\param pNumServerBoards pointer that will receive number of 429 boards on the server.
\param pServerIdx pointer that will receive ID of the newly connected server
\return returns AI429_ERR_NONE on success, an appropriate error code otherwise */
static AiReturn milEstablishConnection(const char* pNetworkAddress, AiInt16* pNumServerBoards,
                                    int* server_id);


/*! \brief Initializes the 1553 specific ANS settings

    This function initializes settings needed for \n
    communication with the ANS1553, e.g. port number, \n
    magic number and protocol version 
    \return returns 0 on success */
static AiInt16 Ans1553SettingsInit(void);


/*! \brief Gets information about a remote server

    This function issues a command to the server in order to get some information about the remote system. \n
    For example, the version of the server and the host name are returned. \n
    \param server server pointer to \ref AnsClientPeer that references the server
    \param pServerInfo pointer to structure that receieves server information
    \return returns API_OK on success, an appropriate error code otherwise */
static AiInt16 Ans1553GetServerInfo(struct AnsClientPeer* server, TY_API_SERVERINFO *pServerInfo);


/*! \brief Initializes the ANS Network layer
* */
static AI_INLINE AiReturn mil_initialize_network_layer(void)
{
    AnsStatus ans_ret;

    /* Initialize 429 specific settings in ANS library */
    if (0 != Ans1553SettingsInit())
    {
        return API_ERR_SERVER;  //** \return API_ERR_SERVER
    }

    memset(g_mil_client_peers, 0, sizeof(g_mil_client_peers));

    /*
    * The ANS client layer gets initialized.
    */
    ans_ret = AnsClient_init(&g_AnsClient);
    if (ans_ret != AnsStatus_OK)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "ANS client initialization failed!\n");
        return API_ERR_SERVER;  //** \return API_ERR_SERVER
    }

    g_mil_net_initialized = AiTrue;

    return API_OK;
}


/*! \brief Get the AnsClientPeer structure for a specific URL and port
*
*/
static AI_INLINE struct AnsClientPeer* milGetClientPeer(const char* url, unsigned short port, int* server_id)
{
    int i;
    struct AnsClientPeer* current;

    for (i = 0; i < AI_ARRAY_COUNT(g_mil_client_peers); i++)
    {
        current = g_mil_client_peers[i];
        if (current)
        {
            if (!strcmp(current->address, url) && current->port == port)
            {
                if (server_id != NULL)
                {
                    *server_id = ANS_SERVER_INDEX_TO_ID(i);
                }
                return current;
            }

        }
    }

    /* in case no valid url / port combination was found*/
    if (server_id != NULL)
    {
        *server_id = 0;
    }
    return NULL;
}


/*! \brief get a free server ID
*
*/
static AI_INLINE int milGetFreeServerPeerId(void)
{
    int i;

    for (i = 0; i < AI_ARRAY_COUNT(g_mil_client_peers); i++)
    {
        if (g_mil_client_peers[i])
        {
            continue;
        }
        else
        {
            return ANS_SERVER_INDEX_TO_ID(i);
        }
    }

    return -1;

}

//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : pszNetworkAdress = Name of server
//
//    Outputs   : w_ServerBoards   = Number of boards found on server
//                ServerID         = Server Identifier
//
//    Description :
//    This function is used to establish connection to server
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiConnectToServer( AiUChar * pszNetworkAdress, AiInt16 *w_ServerBoards )
{
    int server_id;
    AiReturn                ret = API_OK;
    int                     ii;
    AiChar                  szMsg[256];

    if ( NULL == pszNetworkAdress )
    {
        return API_ERR_PARAM1_IS_NULL;
    }
    if ( NULL == w_ServerBoards )
    {
        return API_ERR_PARAM2_IS_NULL;
    }

    if (!g_mil_net_initialized)
    {
        ret = mil_initialize_network_layer();
        if (ret != API_OK)
        {
            return ret;
        }
    }

    /*
    * Establish the connection to the ANS server.
    */
    ret = milEstablishConnection((const char*)pszNetworkAdress, w_ServerBoards, &server_id);

    if ( API_OK == ret )
    {
        /*
        * Create the device list.
        */
        for ( ii = 0; ii < *w_ServerBoards; ii += 1 )
        {
            sprintf((char*) szMsg, "Remote(%d:%d)", ii, server_id);
            _ApiCreateMilDeviceListInfo( (char*)szMsg);
        }
    }


    v_ExamineRetVal( __FUNCTION__, ret );
    TRACE_BEGIN
    TRACE_FCTA( __FUNCTION__, ret); 
    TRACE_RPARA(pszNetworkAdress);
    TRACE_PARE(*w_ServerBoards);
    TRACE_FCTE;
    TRACE_END

    return ret;
}


//    Descriptions
//    ------------
//    Inputs    : ServerID         = ID of server to disconnect
//
//    Outputs   : -
//
//    Description :
//    This function is used to disconnect from server
//
//**************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDisconnectFromServer( AiUChar *pszNetworkAdress)
{
    int server_id;
    AiReturn ret = API_OK;
    int                 ii;
    char                szMsg[256];
    TY_DEVICE_INFO      *pDevice;
    char server_address[ANS_CLIENT_PEER_MAX_ADDR_LEN];
    unsigned int port;
    AnsStatus ans_status;
    struct AnsClientPeer* peer;

    if ( NULL == pszNetworkAdress )
    {
        return API_ERR_PARAM1_IS_NULL;
    }

    sprintf(buf, "Trying to disconnect from server <%.80s>... ", (const char*)pszNetworkAdress);
    DEBUGOUT(DBG_SERVER, __FUNCTION__, buf);

    ans_status = AnsClient_translateServerURL(pszNetworkAdress, server_address, sizeof(server_address), &port);
    if (ans_status != AnsStatus_OK)
    {
        return API_ERR_SERVER;
    }

    if (port == 0)
    {
        port = ANS1553_DEFAULT_PORT;
    }

    /*
    * Try to find the server identifier.
    */
    if (!(peer = milGetClientPeer(server_address, port, &server_id)))
    {
        sprintf(szMsg, "-> Server not registered!\n");
        DEBUGOUT(DBG_TRACE, __FUNCTION__, szMsg);
        return API_ERR_SERVER;   //** \return API_ERR_SERVER
    }

    ii = 0;
    do 
    {
        sprintf(szMsg, "Remote(%d:%d)", ii, server_id);
        pDevice = _ApiGetDeviceInfoPtrByName((char *)szMsg);
        if (NULL != pDevice)
        {
            _ApiDeleteMilDeviceListInfo(NULL, pDevice);
        }
        ii++;
    } while (pDevice != NULL);

    ANS_SERVER_ID_CLEAR(server_id);

    AnsClient_releasePeer(&g_AnsClient, peer);
  
    v_ExamineRetVal( __FUNCTION__, ret );
    TRACE_BEGIN
    TRACE_FCTA(__FUNCTION__, ret); 
    TRACE_RPARE(pszNetworkAdress);
    TRACE_FCTE;
    TRACE_END

    return ret;
}



//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : - 
//
//    Outputs   : ServerInfo *pServerInfo
//
//    Description :
//    This function is used to get server information structure
//
//**************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetServerInfo(AiUInt32 moduleHandle, TY_API_SERVERINFO *pServerInfo)
{
    TY_DEVICE_INFO      *device;
    int server_id;
    struct AnsClientPeer* remote_peer = NULL;

    device = _ApiGetDeviceInfoPtrByModule( moduleHandle );

    if( NULL == device )
        return API_ERR_NO_MODULE_EXTENSION;

    /* -- clear output in case we are not connected to a server --- */
    memset( pServerInfo, 0, sizeof(TY_API_SERVERINFO) );

    server_id = GET_SERVER_ID(moduleHandle);
    remote_peer = ANS_SERVER_ID_TO_PEER(server_id);
    if (!remote_peer)
    {
        return API_ERR_SERVER;
    }

    /* -- Read information from server --- */
    return Ans1553GetServerInfo(remote_peer, pServerInfo);
    
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServer(AiUInt32 port)
{
    AnsStatus ret;

    ret = AnsClient_discover(port);

    if (ret != AnsStatus_OK)
        ret = API_ERR_SERVER;

    return API_OK;
}



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServerCallback(TY_API_DISCOVER_CALLBACK * config)
{
    AnsStatus ret;
    
    if (!config)
        return API_ERR_PARAM1_IS_NULL;

    if (!config->callback)
        return API_ERR_SUBPARAM2_IS_NULL;

    if(config->port == 0)
        config->port = ANS1553_DEFAULT_CLIENT_PORT;

    ret = AnsClient_discover_thread_start((AnsDiscoverThreadConfig*)config);

    if (ret != AnsStatus_OK)
        ret = API_ERR_SERVER;
    else
        ret = API_OK;

    return ret;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServerUnregister(TY_API_DISCOVER_CALLBACK * config)
{
    AnsStatus ret;

    ret = AnsClient_discover_thread_stop((AnsDiscoverThreadConfig*)config);

    if (ret != AnsStatus_OK)
        ret = API_ERR_SERVER;
    else
        ret = API_OK;

    return ret;
}

AiReturn _MilNetGetModuleHandle(TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle)
{
    int server_id;
    AnsStatus             ansStatus = AnsStatus_Error;   // assume failure
    char serverAddress[ANS_CLIENT_PEER_MAX_ADDR_LEN];
    unsigned int port;
    struct AnsClientPeer* peer = NULL;

    ansStatus = AnsClient_translateServerURL(px_ApiOpen->ac_SrvName, serverAddress, sizeof(serverAddress), &port);
    if (ansStatus != AnsStatus_OK)
    {
        return API_ERR_SERVER;
    }

    port = port == 0 ? ANS1553_DEFAULT_PORT : port;

    if (!(peer = milGetClientPeer(serverAddress, port, &server_id)))
    {
        return API_ERR_SERVER;
    }

    *pulModuleHandle = CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, server_id);

    return API_OK;
}



//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : api module [ulModule]
//                stream     [ulStream]
//
//    Outputs   : none
//
//    Description :
//
//**************************************************************************

AiReturn _MilNetOpen( TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle )
{
    int server_id; 
    AiReturn            ret = API_OK;   // assume failure
    AnsStatus           ansStatus = AnsStatus_Error;   // assume failure
    MemChunk            txMemory;
    MemChunk            rxMemory;
    AiUInt32            cmdFrameSize = 0;
    AiUInt32            payloadSize  = 0;
    AnsCmdFrame        *pCmdFrame;
    AnsCmdRspFrame     *pCmdResponse;    // handled by ANS client layer
    Ans1553OpenCmdPayload* commandPayload = NULL;
    Ans1553OpenResponsePayload* responsePayload = NULL;
    char serverAddress[ANS_CLIENT_PEER_MAX_ADDR_LEN];
    unsigned int port;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;
    TY_DEVICE_INFO* device;
    
    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    ansStatus = AnsClient_translateServerURL(px_ApiOpen->ac_SrvName, serverAddress, sizeof(serverAddress), &port);
    if (ansStatus != AnsStatus_OK)
    {
        return API_ERR_SERVER;
    }

    port = port == 0 ? ANS1553_DEFAULT_PORT : port;

    if (!(peer = milGetClientPeer(serverAddress, port, &server_id)))
    {
        return API_ERR_SERVER;
    }

    device = _ApiGetDeviceInfoPtrByModule(CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, server_id));
    if (!device)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    /* Check if device has been opened before.
    * If this is the case, just increase the reference count and return.
    */
    if (device->ulOpenCount > 0)
    {
        device->ulOpenCount++;

        if (pulModuleHandle)
        {
            *pulModuleHandle = CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, server_id);
        }

        return API_OK;
    }


    board = AnsClientPeer_createBoard(peer, ANS_BOARD_HANDLE(server_id, px_ApiOpen->ul_Module));
    if (!board)
    {
        return API_ERR_MALLOC_FAILED;
    }

    ret = API_OK;
    do
    {
        ansStatus = AnsClientPeer_connectBoard(peer, board);
        if (ansStatus != AnsStatus_OK)
        {
            ret = API_ERR_SERVER;
            break;
        }

        // prepare CmdFrame
        payloadSize = ANS_CMD_PAYLOAD(Ans1553OpenCmdPayload);
        cmdFrameSize = sizeof(ANS_Header) + payloadSize;

        if (!MemChunk_allocate(&txMemory, cmdFrameSize))
        {
            ret = API_ERR_MALLOC_FAILED;
            break;
        }

        pCmdFrame = (AnsCmdFrame *)txMemory.pMemory;         // cast TxBuffer to ANS Command Frame
        pCmdFrame->header.commandtype = BoardCommand;
        pCmdFrame->header.functionId = OpenBoardID;
        commandPayload = (Ans1553OpenCmdPayload*)pCmdFrame->payload;
        commandPayload->Module = px_ApiOpen->ul_Module;
        commandPayload->Stream = px_ApiOpen->ul_Stream;

        ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, pCmdFrame, payloadSize, &rxMemory);
        if (ansStatus != AnsStatus_OK)
        {
            ret = API_ERR_SERVER;
        }
        else
        {
            // process response from Server
            pCmdResponse = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
            responsePayload = (Ans1553OpenResponsePayload*)pCmdResponse->payload;
            ret = responsePayload->ApiFunctionRc;
        }

        if (ret != API_OK)
        {
            break;
        }

        /* Board successfully opened. Increase reference count */
        device->ulOpenCount++;

    } while (0);

    /* free Rx and Tx buffers */
    MemChunk_free(&rxMemory);
    MemChunk_free(&txMemory);

    if (ret == API_OK && pulModuleHandle)
    {
        *pulModuleHandle = CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, server_id);
    }

    return ret;
}

//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : api module [bModule]
//
//    Outputs   : none
//
//    Description :
//    This function is used for finishing of NET communication
//
//**************************************************************************

AiInt16 _MilNetClose(AiUInt32 moduleHandle, AiBoolean force)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553CloseCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CloseResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;
    TY_DEVICE_INFO* device = NULL;


    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    device = _ApiGetDeviceInfoPtrByModule(moduleHandle);
    if (!device)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    /* Check if device is already closed */
    if (device->ulOpenCount == 0)
    {
        return API_OK;
    }

    /* Decrease reference count.
    * If there are still active users, just return
    * If forced close is requested set to 0
    */
    device->ulOpenCount = force ? 0 : device->ulOpenCount - 1;

    if (device->ulOpenCount > 0)
    {
        return API_OK;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    do
    {
        payloadSize = ANS_CMD_PAYLOAD(Ans1553CloseCmdPayload);

        cmdFrameSize = sizeof(ANS_Header) + payloadSize;
        if (!MemChunk_allocate(&txMemory, cmdFrameSize))
        {
            ANSLogError("ERR: MemChunk_allocate(TX) failed!\n");
            ret = API_ERR_MALLOC_FAILED;
            break;
        }

        commandFrame = (AnsCmdFrame*)txMemory.pMemory;
        commandFrame->header.commandtype = BoardCommand;
        commandFrame->header.functionId = CloseBoardID;

        commandPayload = (Ans1553CloseCmdPayload*)commandFrame->payload;
        commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);

        ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
        if (ansStatus != AnsStatus_OK)
        {
            ret = API_ERR_SERVER;
            break;
        }

        // process response from Server
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;

        responsePayload = (Ans1553CloseResponsePayload*)responseFrame->payload;

        ret = responsePayload->ApiFunctionRc;
        if (ret != API_OK)
        {
            break;
        }

        AnsClientPeer_releaseBoard(peer, board);

    } while (0);


    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : api module [bModule]
//                Pointer to command array [cmd]
//                Pointer to acknowledge array [ack]
//
//    Outputs   : none
//
//    Description :
//    This function is used for passing the command to the driver
//
//**************************************************************************

AiInt16 _MilNetIo(AiUInt32 moduleHandle, AiUInt8 biu, 
               AiUInt32 commandCode, AiInt16 expectedAckCode, 
               AiUInt8* byteOutData, AiInt16 byteOutDataUnits, 
               AiUInt16* wordOutData, AiInt16 wordOutDataUnits, 
               AiUInt8* byteInData, AiInt16* byteInDataUnits, 
               AiUInt16* wordInData, AiInt16* wordInDataUnits,
               AiUInt32* lwordInData, AiInt16* lwordInDataUnits)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553IoCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553IoResponsePayload* responsePayload = NULL;
    AiUInt8* data = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553IoCmdPayload);
    payloadSize -= 1; /* Dummy byte in Ans1553IoCmdPayload */
    payloadSize += byteOutDataUnits + wordOutDataUnits * sizeof(AiUInt16);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = IoCommandID;

    commandPayload = (Ans1553IoCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE_KEEP_HS_FLAG(moduleHandle);
    commandPayload->Biu = biu;
    commandPayload->Cmd = commandCode;
    commandPayload->ExpectAck = expectedAckCode;
    commandPayload->OutByteSize = byteOutDataUnits;
    commandPayload->OutWordSize = wordOutDataUnits;

    /* Now serialize command parameters
       We will begin with long words then with words and at last the bytes.
       This prevents a wrong alignment. E.g. on ARM systems 2-Byte values have to be aligned on
       2-.Byte addresses and 4-Byte values on 4-Byte addresses. */
    data = commandPayload->Data;
    memcpy(data, wordOutData, wordOutDataUnits * sizeof(AiUInt16));
    data += wordOutDataUnits * sizeof(AiUInt16);
    memcpy(data, byteOutData, byteOutDataUnits);


    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* Process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553IoResponsePayload*) responseFrame->payload;

        ret = (AiInt16)responsePayload->ApiFunctionRc;

        if( ret == API_OK)
        {
            *byteInDataUnits = responsePayload->InByteSize;
            *wordInDataUnits = responsePayload->InWordSize;
            *lwordInDataUnits = responsePayload->InLWordSize;

            /* Now de-serialize command parameters
               We will begin with long words then with words and at last the bytes.
               This prevents a wrong alignment. E.g. on ARM systems 2-Byte values have to be aligned on
               2-.Byte addresses and 4-Byte values on 4-Byte addresses. */
            data = responsePayload->Data;
            memcpy(lwordInData, data, *lwordInDataUnits * sizeof(AiUInt32));
            data += *lwordInDataUnits * sizeof(AiUInt32);
            memcpy(wordInData, data, *wordInDataUnits * sizeof(AiUInt16));
            data += *wordInDataUnits * sizeof(AiUInt16);
            memcpy(byteInData, data, *byteInDataUnits);
        }
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
} // end: _MilNetIo



//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : M. Riedinger             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 14.02.01     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : bModule - Module Number
//                offset  - Offset relativ to start of Global-RAM (BIU-memory)
//                width   - data width (1 for BYTE(AiUInt8), 2 for WORD(AiUInt16) and 4 LWORD(AiUInt32))
//                data_p  - Data pointer, where the data should be read to. 
//                size    - size of data to read with "width"
//
//    Outputs   : Data read to data_p
//
//    Description : Reads a Data location in the Global RAM in avoidance the Command
//                  and Acknowledge area. 
//
//**************************************************************************
AiInt16 _MilNetReadMemData(AiUInt32 moduleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                        void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553ReadMemCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553ReadMemResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553ReadMemCmdPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    /* Prepare the read command frame */
    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = ReadMemoryID;

    commandPayload = (Ans1553ReadMemCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Memtype = memtype;
    commandPayload->Offset = offset;
    commandPayload->Width = width;
    commandPayload->NumElements = size;

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553ReadMemResponsePayload*) responseFrame->payload;

        if(responsePayload->ApiFunctionRc == API_OK)
        {
            memcpy(data_p, responsePayload->Data, responsePayload->BytesRead);

            if(pul_BytesRead)
            {
                *pul_BytesRead = responsePayload->BytesRead;
            }
        }
     
        ret = (AiInt16) responsePayload->ApiFunctionRc;
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}

//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : bModule - Module Number
//                offset  - Offset relativ to start of Global-RAM (BIU-memory)
//                width   - data width (1 for BYTE(AiUInt8), 2 for WORD(AiUInt16) and 4 LWORD(AiUInt32))
//                data_p  - Data pointer, where the data should be read to. 
//                size    - size of data to write with "width"
//
//    Outputs   : Data read to data_p
//
//    Description : Reads a Data location in the Global RAM in avoidance the Command
//                  and Acknowledge area. 
//
//**************************************************************************
AiInt16 _MilNetWriteMemData(AiUInt32 moduleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                         void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553WriteMemCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553WriteMemResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553WriteMemCmdPayload);
    payloadSize += size * width;

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    /* Now Prepare the write command frame */
    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = WriteMemoryID;

    commandPayload = (Ans1553WriteMemCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Memtype = memtype;
    commandPayload->Offset = offset;
    commandPayload->Width = width;
    commandPayload->NumElements = size;
    memcpy(commandPayload->Data, data_p, commandPayload->NumElements * commandPayload->Width);

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        // process response from Server
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553WriteMemResponsePayload*) responseFrame->payload;

        if(responsePayload->ApiFunctionRc == API_OK)
        {
            if(pul_BytesWritten)
            {
                *pul_BytesWritten = responsePayload->BytesWritten;
            }
        }
     
        ret = (AiInt16) responsePayload->ApiFunctionRc;
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : M. Haag             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 21.11.11     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************
AiInt16 _MilNetGetDriverInfo( AiUInt32 moduleHandle, TY_API_DRIVER_INFO *px_DriverInfo )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553GetDriverInfoCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553GetDriverInfoResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553GetDriverVersionCmdPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = GetDriverInfoID;

    commandPayload = (Ans1553GetDriverInfoCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553GetDriverInfoResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;

        if(ret == API_OK)
        {
            if( px_DriverInfo != NULL )
            {
                px_DriverInfo->uc_DeviceGroup     = responsePayload->DeviceGroup;
                px_DriverInfo->uc_ReservedB2      = responsePayload->ReservedB2;
                px_DriverInfo->uc_ReservedB3      = responsePayload->ReservedB3;
                px_DriverInfo->uc_ReservedB4      = responsePayload->ReservedB4;
                px_DriverInfo->uw_ReservedW1      = responsePayload->ReservedW1;
                px_DriverInfo->uw_ReservedW2      = responsePayload->ReservedW2;
                px_DriverInfo->ul_DriverFlags     = responsePayload->DriverFlags;
                px_DriverInfo->ul_SN              = responsePayload->SN;
                px_DriverInfo->ul_BoardConfig     = responsePayload->BoardConfig;
                px_DriverInfo->ul_BoardType       = responsePayload->BoardType;
                px_DriverInfo->ul_OpenConnections = responsePayload->OpenConnections;
                px_DriverInfo->ul_ReservedLW6     = responsePayload->ReservedLW6;
                px_DriverInfo->ul_ReservedLW7     = responsePayload->ReservedLW7;
                px_DriverInfo->ul_ReservedLW8     = responsePayload->ReservedLW8;
            }
        }
     
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : M. Haag             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 22.08.14     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************
AiInt16 _MilNetReadVersion( AiUInt32 moduleHandle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553ReadVersionCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553ReadVersionResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553ReadVersionCmdPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = GetSoftwareVersionsID;

    commandPayload = (Ans1553ReadVersionCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->VersionId = eId;

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553ReadVersionResponsePayload*) responseFrame->payload;

        if(responsePayload->ApiFunctionRc == API_OK)
        {
            memcpy( pxVersion, &responsePayload->Version, sizeof(TY_VER_INFO) );
        }
     
        ret = (AiInt16) responsePayload->ApiFunctionRc;
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;

}

//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : M. Riedinger             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 14.02.01     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetGetSysDrvVersion(AiUInt32 moduleHandle, AiUInt32 *pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553GetDriverVersionCmdPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553GetDriverVersionResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553GetDriverVersionCmdPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = GetDriverVersionID;

    commandPayload = (Ans1553GetDriverVersionCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle = GET_LOCAL_MODULE_HANDLE(moduleHandle);

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553GetDriverVersionResponsePayload*) responseFrame->payload;

        if(responsePayload->ApiFunctionRc == API_OK)
        {
            if(pul_SysDrvVer)
            {
                *pul_SysDrvVer = responsePayload->SysDrvVer;
            }
        
            if(pul_SysDrvBuild)
            {
                *pul_SysDrvBuild = responsePayload->SysDrvBuild;
            }
        }
     
        ret = (AiInt16) responsePayload->ApiFunctionRc;
    }


    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;

}


//**************************************************************************
//
//   Module : NET_IO                   
//
//---------------------------------------------------------------------------
//   Create : 13.09.12     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************
AiInt16 _NetCmdScopeStart(AiUInt32 bModule)
{
    /* @AIMFIXME: _NetCmdScopeStart not yet implemented */
    return API_ERR_SERVER;
}



//**************************************************************************
//
//   Module : NET_IO                   
//
//---------------------------------------------------------------------------
//   Create : 13.09.12     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _NetCmdScopeStop(AiUInt32 bModule)
{
    /* @AIMFIXME: _NetCmdScopeStop not yet implemented */
    return API_ERR_SERVER;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//---------------------------------------------------------------------------
//   Create : 13.09.12     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _NetCmdScopeSetup( AiUInt32 bModule, TY_API_SCOPE_SETUP *px_ScopeSetup)
{
    /* @AIMFIXME: _NetCmdScopeSetup not yet implemented */
    return API_ERR_SERVER;
}

//**************************************************************************
//
//   Module : NET_IO                   
//
//---------------------------------------------------------------------------
//   Create : 21.09.12     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _NetCmdScopeStatus( AiUInt32 bModule, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft)
{
    /* @AIMFIXME: _NetCmdScopeStatus not yet implemented */
    return API_ERR_SERVER;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//---------------------------------------------------------------------------
//   Create : 13.09.12     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _NetProvideScopeBuffers(AiUInt32 bModule, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER *ax_ScopeBuffers[])
{
    /* @AIMFIXME: _NetProvideScopeBuffers not yet implemented */
    return API_ERR_SERVER;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : P. Giesel             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 21.01.03     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetDataQueueOpen(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 * queue_size)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553CmdDataQueueOpenPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CmdDataQueueOpenResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553CmdDataQueueOpenPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolDataQueueOpenID;

    commandPayload = (Ans1553CmdDataQueueOpenPayload*) commandFrame->payload;

    commandPayload->ModHandle        = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Id               = id;

    ansStatus =  AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553CmdDataQueueOpenResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;
        *queue_size = responsePayload->QueueSize;
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : P. Giesel             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 21.01.03     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetDataQueueClose(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553CmdDataQueueClosePayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CmdDataQueueCloseResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553CmdDataQueueClosePayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolDataQueueCloseID;

    commandPayload = (Ans1553CmdDataQueueClosePayload*) commandFrame->payload;

    commandPayload->ModHandle        = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Id               = id;

    ansStatus =  AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553CmdDataQueueCloseResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;

    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : P. Giesel             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 21.01.03     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetDataQueueControl(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 mode)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553CmdDataQueueControlPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CmdDataQueueControlResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553CmdDataQueueControlPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolDataQueueControlID;

    commandPayload = (Ans1553CmdDataQueueControlPayload*) commandFrame->payload;

    commandPayload->ModHandle        = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Id               = id;
    commandPayload->Mode             = mode;

    ansStatus =  AnsClientPeer_transmitBoardCommand( peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553CmdDataQueueControlResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;

    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


//**************************************************************************
//
//   Module : NET_IO                   
//
//   Author : P. Giesel             Project   : 
//
//---------------------------------------------------------------------------
//   Create : 21.01.03     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetDataQueueRead(AiUInt32 moduleHandle, AiUInt8 uc_Biu, TY_API_DATA_QUEUE_READ *px_Queue, TY_API_DATA_QUEUE_STATUS * info)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553CmdDataQueueReadPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CmdDataQueueReadResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553CmdDataQueueReadPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolDataQueueReadID;

    commandPayload = (Ans1553CmdDataQueueReadPayload*) commandFrame->payload;

    commandPayload->ModHandle        = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Id               = px_Queue->id;
    commandPayload->BytesToRead      = px_Queue->bytes_to_read;

    ansStatus =  AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553CmdDataQueueReadResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;

        info->status                  = responsePayload->Status;
        info->bytes_transfered        = responsePayload->BytesTransferred;
        info->bytes_in_queue          = responsePayload->BytesInQueue;
        info->total_bytes_transfered  = ((AiUInt64)responsePayload->TotalBytesTransferredHi) << 32;
        info->total_bytes_transfered += responsePayload->TotalBytesTransferredLo;

        if( responsePayload->BytesTransferred <= commandPayload->BytesToRead )
        {
            memcpy( px_Queue->buffer, responsePayload->Data, responsePayload->BytesTransferred );
        }
        else
        {
            ret = API_ERR_NO_SPACE_LEFT;
        }
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}




//**************************************************************************
//
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetGetDeviceConfig( AiUInt32 moduleHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553GetDeviceConfigPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553GetDeviceConfigResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553GetDeviceConfigPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolGetDeviceConfigID;

    commandPayload = (Ans1553GetDeviceConfigPayload*) commandFrame->payload;

    commandPayload->ModHandle        = GET_LOCAL_MODULE_HANDLE(moduleHandle);

    ansStatus = AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553GetDeviceConfigResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;

        pxConfig->uc_DmaEnabled          = responsePayload->DmaEnabled;
        pxConfig->uc_DataQueueMemoryType = responsePayload->DataQueueMemoryType;
        pxConfig->uc_DataQueueMode       = responsePayload->DataQueueMode;
        pxConfig->uc_ReservedB4          = responsePayload->ReservedB4;
        pxConfig->uw_ReservedW1          = responsePayload->ReservedW1;
        pxConfig->uw_ReservedW2          = responsePayload->ReservedW2;
        pxConfig->ul_DmaMinimumSize      = responsePayload->DmaMinimumSize;
        pxConfig->ul_IntRequestCount     = responsePayload->IntRequestCount;
        pxConfig->ul_DriverFlags         = responsePayload->DriverFlags;
        pxConfig->ul_ReservedLW4         = responsePayload->ReservedLW4;
        pxConfig->ul_ReservedLW5         = responsePayload->ReservedLW5;
        pxConfig->ul_ReservedLW6         = responsePayload->ReservedLW6;
        pxConfig->ul_ReservedLW7         = responsePayload->ReservedLW7;
        pxConfig->ul_ReservedLW8         = responsePayload->ReservedLW8;

    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}

//**************************************************************************
//
//    Descriptions
//    ------------
//
//**************************************************************************

AiInt16 _MilNetSetDeviceConfig( AiUInt32 moduleHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553SetDeviceConfigPayload* commandPayload = NULL;
    AiInt16 ret = API_OK;
    AnsStatus ansStatus = AnsStatus_Error;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553SetDeviceConfigResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553SetDeviceConfigPayload);

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;

    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId  = ProtocolSetDeviceConfigID;

    commandPayload = (Ans1553SetDeviceConfigPayload*) commandFrame->payload;

    commandPayload->ModHandle           = GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->DmaEnabled          = pxConfig->uc_DmaEnabled;
    commandPayload->DataQueueMemoryType = pxConfig->uc_DataQueueMemoryType;
    commandPayload->DataQueueMode       = pxConfig->uc_DataQueueMode;
    commandPayload->ReservedB4          = pxConfig->uc_ReservedB4;
    commandPayload->ReservedW1          = pxConfig->uw_ReservedW1;
    commandPayload->ReservedW2          = pxConfig->uw_ReservedW2;
    commandPayload->DmaMinimumSize      = pxConfig->ul_DmaMinimumSize;
    commandPayload->IntRequestCount     = pxConfig->ul_IntRequestCount;
    commandPayload->DriverFlags         = pxConfig->ul_DriverFlags;
    commandPayload->ReservedLW4         = pxConfig->ul_ReservedLW4;
    commandPayload->ReservedLW5         = pxConfig->ul_ReservedLW5;
    commandPayload->ReservedLW6         = pxConfig->ul_ReservedLW6;
    commandPayload->ReservedLW7         = pxConfig->ul_ReservedLW7;
    commandPayload->ReservedLW8         = pxConfig->ul_ReservedLW8;

    ansStatus =  AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);

    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553SetDeviceConfigResponsePayload*) responseFrame->payload;

        ret = (AiInt16) responsePayload->ApiFunctionRc;
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}



static AiReturn milEstablishConnection (
    const char  *pNetworkAddress,   /*!< [in] Server address                */
    AiInt16     *pNumServerBoards,  /*!< [out] Number of boards installed   */
                                    /*!< on server                          */
    int* server_id)                 /*!< [out] Server index on success      */
{
    char logBuf[ANS_CLIENT_PEER_MAX_ADDR_LEN * 2]; /* give some space for additional info*/
    char serverAddress[ANS_CLIENT_PEER_MAX_ADDR_LEN];
    unsigned int port = 0;
    AiReturn ret = API_OK;
    AnsStatus ans_status;
    struct AnsClientPeer* peer = NULL;
    unsigned int board_count;

    if (NULL == pNetworkAddress)
    {
        sprintf(logBuf, "No Network address specified!\n");
        DEBUGOUT(DBG_ERROR, __FUNCTION__, logBuf);
        return API_ERR_PARAM1_IS_NULL;
    }

    sprintf(logBuf, "Trying to connect to server <%.32s>... ", pNetworkAddress);
    DEBUGOUT(DBG_INIT, "milEstablishConnection", logBuf);

    /* 'local' is reserved for local connections.
    This would collide with the ApiOpen implementation */
    if (0 == strcmp("local", (const char *)pNetworkAddress))
    {
        sprintf(logBuf, "Unable to connect to \"local\", maybe you meant \"localhost\" ");
        DEBUGOUT(DBG_ERROR, "milEstablishConnection", logBuf);
        return API_ERR_SERVER;
    }

    ans_status = AnsClient_translateServerURL(pNetworkAddress, serverAddress, sizeof(serverAddress), &port);
    if (ans_status != AnsStatus_OK)
    {
        DEBUGOUT(DBG_ERROR, __FUNCTION__, "Invalid server URL");
        return API_ERR_SERVER;
    }

    if (port == 0)
    {
        port = ANS1553_DEFAULT_PORT; /* Use default server port */
    }

    ret = API_OK;
    do
    {
        peer = AnsClient_requestPeer(&g_AnsClient, serverAddress, port);
        if (!peer)
        {
            peer = AnsClient_createPeer(&g_AnsClient, serverAddress, port);
            if (!peer)
            {
                sprintf(logBuf, "Failed to create peer %s %d", serverAddress, port);
                DEBUGOUT(DBG_ERROR, __FUNCTION__, logBuf);
                ret = API_ERR_MALLOC_FAILED;
                break;
            }

            ans_status = AnsClientPeer_connect(peer);
            if (ans_status != AnsStatus_OK)
            {
                sprintf(logBuf, "_registerServer() failed\n");
                DEBUGOUT(DBG_ERROR, __FUNCTION__, logBuf);

                switch (ans_status)
                {
                case AnsStatus_IncompatibleProtVer:
                    ret = API_ERR_SERVER_INVALID_VERSION;
                    break;
                case AnsStatus_SocketConnectError:
                    ret = API_ERR_SERVER_CONNECT;
                    break;

                default:
                    ret = API_ERR_SERVER;
                    break;
                }
                AnsClient_releasePeer(&g_AnsClient, peer);

                break;
            }

            *server_id = milGetFreeServerPeerId();
            if (*server_id < 0)
            {
                sprintf(logBuf, "No free server id for %s at port %d", serverAddress, port);
                DEBUGOUT(DBG_ERROR, __FUNCTION__, logBuf);
                ret = API_ERR_SERVER;
                break;
            }

            ANS_SERVER_ID_SET(*server_id, peer);

        }
        else /* Server already connected */
        {
            struct AnsClientPeer* peer_tmp = NULL;

            peer_tmp = milGetClientPeer(serverAddress, port, server_id);
            if ((0 == server_id) || (peer != peer_tmp))
           {
                /* Server already connected, but we cannot find a valid server Address and corresponding port. */
                ret = API_ERR_SERVER;
                break;
            }
        }

        ans_status = AnsClientPeer_requestBoardCount(peer, &board_count);
        if (ans_status != AnsStatus_OK)
        {
            ret = API_ERR_SERVER;
            break;
        }

    } while (0);

    if (pNumServerBoards)
    {
        *pNumServerBoards = board_count;
    }

    return ret;
}


static AiInt16 Ans1553SettingsInit(void)
{
    AnsProtocol_setMagic(&g_AnsProtocol, ANS1553_MAGIC_VALUE);

    AnsProtocol_setVersion(&g_AnsProtocol, &g_Ans1553ProtocolVersion);

    return 0;
}


static AiInt16 Ans1553GetServerInfo(struct AnsClientPeer* server, TY_API_SERVERINFO *pServerInfo)
{
    AnsGetServerInfoResponsePayload serverInfo;
    AnsStatus status = AnsStatus_Error;
    char logBuf[128];

    status = AnsClientPeer_getServerInfo(server, &serverInfo);
    if(status != AnsStatus_OK)
    {
        sprintf(logBuf, "ANS Get Server Info command failed with status %d\n", status);
        DEBUGOUT( DBG_SERVER, __FUNCTION__, logBuf);
        return API_ERR_SERVER;
    }

    memset(pServerInfo, 0, sizeof(TY_API_SERVERINFO) );

    pServerInfo->server_version.ul_VersionType  = AI_ANS_VER;
    pServerInfo->server_version.ul_MajorVer     = serverInfo.serverVersion.majorVersion;
    pServerInfo->server_version.ul_MinorVer     = serverInfo.serverVersion.minorVersion;
    pServerInfo->server_version.ul_PatchVersion = serverInfo.serverVersion.patchVersion;
    AiOsStrncpy((char*)pServerInfo->server_version.ac_FullVersion, serverInfo.serverVersion.versionString, sizeof(pServerInfo->server_version.ac_FullVersion), AiFalse);
    AiOsStrncpy((char*)pServerInfo->server_version.ac_Description, "ANS",  sizeof(pServerInfo->server_version.ac_FullVersion), AiFalse);

    pServerInfo->protocol_major = serverInfo.protocolVersion.majorVersion;
    pServerInfo->protocol_minor = serverInfo.protocolVersion.minorVersion;

    AiOsStrncpy((char*)pServerInfo->application_name, serverInfo.serverInfo.name, sizeof(pServerInfo->application_name), AiFalse);
    AiOsStrncpy((char*)pServerInfo->description,      serverInfo.serverInfo.description,     sizeof(pServerInfo->description),      AiFalse);
    AiOsStrncpy((char*)pServerInfo->host_name,        serverInfo.hostName,                   sizeof(pServerInfo->host_name),        AiFalse);
    AiOsStrncpy((char*)pServerInfo->os_info,          serverInfo.osInfo,                     sizeof(pServerInfo->os_info),          AiFalse);

    return API_OK;
}


AiInt16 _MilNetIoStruct(AiUInt32 moduleHandle, TY_MIL_COM* pTargetCommand, TY_MIL_COM_ACK* pReturnData)
{
    MemChunk txMemory;
    MemChunk rxMemory;
    AiUInt32 moduleID = 0;
    AiUInt8 serverID = 0;
    AiUInt32 payloadSize = 0;
    AiUInt32 cmdFrameSize = 0;
    AnsCmdFrame* commandFrame = NULL;
    Ans1553IoCommandStructCmdPayload* commandPayload = NULL;
    AnsStatus ansStatus = AnsStatus_Error;
    AiInt16 ret = API_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553IoCommandStructResponsePayload* responsePayload = NULL;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;

    MemChunk_init(&txMemory);
    MemChunk_init(&rxMemory);

    moduleID = GET_MODULE_ID(moduleHandle);
    serverID = GET_SERVER_ID(moduleHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }

    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));
    if (!board)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    payloadSize = ANS_CMD_PAYLOAD(Ans1553IoCommandStructCmdPayload);
    payloadSize -= 1; /* Dummy command data member */
    payloadSize += pTargetCommand->ulSize;

    cmdFrameSize = sizeof(ANS_Header) + payloadSize;
    if(!MemChunk_allocate( &txMemory, cmdFrameSize ))
    {
        AnsClientPeer_releaseBoard(peer, board);
        return API_ERR_MALLOC_FAILED;
    }

    commandFrame = (AnsCmdFrame*) txMemory.pMemory;
    commandFrame->header.commandtype = BoardCommand;
    commandFrame->header.functionId = IoCommandStructID;

    commandPayload = (Ans1553IoCommandStructCmdPayload*) commandFrame->payload;
    commandPayload->ModHandle= GET_LOCAL_MODULE_HANDLE(moduleHandle);
    commandPayload->Size = pTargetCommand->ulSize;
    commandPayload->ExpectedAckSize = pTargetCommand->ulExpectedAckSize;
    memcpy(commandPayload->CommandData, pTargetCommand, commandPayload->Size);

    ansStatus =  AnsClientPeer_transmitBoardCommand(peer, board, commandFrame, payloadSize, &rxMemory);
    if ( ansStatus != AnsStatus_OK )
    {
        ret = API_ERR_SERVER;
    }
    else
    {
        /* process response from Server */
        responseFrame = (AnsCmdRspFrame *)rxMemory.pMemory;         // cast rxBuffer to ANS Command Response Frame
        
        responsePayload = (Ans1553IoCommandStructResponsePayload*) responseFrame->payload;

        ret = (AiInt16)responsePayload->ApiFunctionRc;

        if( ret == API_OK )
        {
            if (responsePayload->Size)
            {
                if (pTargetCommand->ulExpectedAckSize < responsePayload->Size)
                {
                    ret = API_ERR_NO_SPACE_LEFT;
                }
                else
                {
                    memcpy(pReturnData, responsePayload->ResponseData, responsePayload->Size);
                }
            }
        }
     
    }

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&txMemory);
    MemChunk_free(&rxMemory);

    return ret;
}


/*! \brief Publish remote board events
*
* This function will run in a separate thread.
* It will listen for events on a specific device and publish these to registered observers
* @param args pointer to \ref TY_DEVICE_INFO that contains the device to publish events for
*/
static ANS_THREAD_RETURN ANS_THREAD_CALL_CONV _MilNetEventPublisher(void* args)
{
    AiUInt32 stream, biu;
    AiUInt32 ulModHandle = 0;
    AnsStatus status;
    Ans1553Event event;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;
    TY_DEVICE_INFO* device = args;
    TY_INT_FUNC_PTR user_callback;
    struct mil_net_layer_properties* net_layer = device->net_layer;

    

    peer = ANS_SERVER_ID_TO_PEER(GET_SERVER_ID(device->ul_ModuleNo));

    if (!peer)
    {
        return (ANS_THREAD_RETURN)0;
    }

    board = AnsClientPeer_requestBoard(peer, (AiHandle)(AiUIntPtr)device->ul_ModuleNo);
    
    if (!board)
    {
        return (ANS_THREAD_RETURN)0;
    }

    while (1)
    {

        /* Use a timeout here as closing of the socket connection from main thread
        * might not wake up the wait call.
        */
        status = AnsBoardEventObserver_waitEvent(net_layer->event_observer, (char*)&event, sizeof(Ans1553Event),
            1000);
        if (status != AnsStatus_OK)
        {
            /* If wait timed out, just continue with waiting for next event
            * If error occurred, e.g. observer connection closed, terminate thread
            */
            if (status == AnsStatus_Timeout)
            {
                continue;
            }
            else
            {
                break;
            }
        }


        stream = GET_STREAM_ID(event.ModHandle);

        if (stream != 0)
        {
            /* open ex used */
            if ( event.biu == API_INT_LS )
            {
                uw_ApiGetBiuFromStream(event.ModHandle | device->ul_ModuleNo, 1, &biu);
            }
            else
            {
                uw_ApiGetBiuFromStream(event.ModHandle | device->ul_ModuleNo | API_HS_ACCESS, 1, &biu);
            }
        }
        else
        {
            /* open used */
            biu = event.biu - 1;
        }

        if ((user_callback = device->ax_IntFuncTable[biu-1].af_IntFunc[event.type]))
        {
            user_callback(event.ModHandle | device->ul_ModuleNo, event.biu, event.type, (TY_API_INTR_LOGLIST_ENTRY*)event.event_data);
        }
    }

    AnsClientPeer_releaseBoard(peer, board);

    return (ANS_THREAD_RETURN)0;
}


AiReturn _MilNetCallbackRegister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc)
{
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ulModHandle);
    AiReturn ret;
    AiUInt8 moduleID;
    AiUInt8 serverID;
    AiUInt32 ulStream = GET_STREAM_ID(ulModHandle);
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;
    AnsStatus ans_status;
    MemChunk command_memory;
    MemChunk response_memory;
    AiUInt32 command_frame_size;
    AnsCmdFrame* command_frame = NULL;
    Ans1553CallbackPayload* command_payload = NULL;
    struct AnsBoardEventObserver* event_observer = NULL;
    struct mil_net_layer_properties* net_layer;

    if (!device)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    net_layer = device->net_layer;

    MemChunk_init(&command_memory);
    MemChunk_init(&response_memory);

    moduleID = GET_MODULE_ID(ulModHandle);
    serverID = GET_SERVER_ID(ulModHandle);
    
    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }
    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));

    if (!board)
    {
        return API_ERR_WRONG_MODULE;
    }

    /* Lock access to net layer here,
    * so we can safely install the handler, create the event observer and start the
    * event publisher thread
    */
    ai_mutex_lock(net_layer->lock);

    ret = API_OK;
    do
    {
        device->ax_IntFuncTable[uc_Biu].ul_OpenExUsed = ulStream;
        device->ax_IntFuncTable[uc_Biu].ul_Module = ulModHandle;
        device->ax_IntFuncTable[uc_Biu].af_IntFunc[uc_Type] = pf_IntFunc;


        /* There's no event observer for the device yet, hence create
        * it and start the event publisher thread
        */
        if (!net_layer->event_observer)
        {
            ans_status = AnsClientPeer_openBoardEventStream(peer, board, &event_observer);
            if (ans_status != AnsStatus_OK)
            {
                ret = API_ERR_SERVER;
                break;
            }

            net_layer->event_observer = event_observer;

            if (ANS_CREATE_OS_THREAD(_MilNetEventPublisher, device, &device->_hInterruptThread))
            {
                ret = API_ERR_SERVER;
                break;
            }
        }

        /* Now register the event callback on remote peer */
        command_frame_size = sizeof(ANS_Header) + ANS_CMD_PAYLOAD(Ans1553CallbackPayload);

        if (!MemChunk_allocate(&command_memory, command_frame_size))
        {
            ret = API_ERR_MALLOC_FAILED;
            break;
        }

        command_frame = (AnsCmdFrame*)command_memory.pMemory;
        command_payload = (Ans1553CallbackPayload*)command_frame->payload;

        command_frame->header.commandtype = BoardCommand;
        command_frame->header.functionId = RegisterCallbackID;

        command_payload->ModHandle = GET_LOCAL_MODULE_HANDLE(ulModHandle);
        command_payload->biu       = API_INT_LS;

        if (device->ax_IntFuncTable[uc_Biu].ul_OpenExUsed)
        {
            if( IS_HS_ACCESS(ulModHandle)) {
                command_payload->biu = API_INT_HS;
            }
        }
        else
        {
            command_payload->biu = uc_Biu + 1;
        }
        command_payload->type      = uc_Type;

        ans_status = AnsClientPeer_transmitBoardCommand(peer, board, command_frame, command_frame_size, &response_memory);
        if (ans_status != AnsStatus_OK)
        {
            ret = API_ERR_SERVER;
            break;
        }
    } while (0);

    if (ret != API_OK)
    {
        device->ax_IntFuncTable[uc_Biu].af_IntFunc[uc_Type] = NULL;

        /* If this is a valid pointer, the observer has been created during this function call */
        if (event_observer)
        {
            AnsClientPeer_closeBoardEventStream(peer, board, event_observer->handle);

            if (device->_hInterruptThread != INVALID_HANDLE_VALUE)
            {
                ANS_JOIN_OS_THREAD(device->_hInterruptThread);
            }

            AnsBoardEventObserver_destroy(event_observer);
            net_layer->event_observer = NULL;
            device->_hInterruptThread = INVALID_HANDLE_VALUE;
        }
    }

    ai_mutex_release(net_layer->lock);

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&command_memory);
    MemChunk_free(&response_memory);

    return ret;
}


AiReturn _MilNetCallbackUnregister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type)
{
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ulModHandle);
    AiReturn ret;
    AiUInt8 moduleID;
    AiUInt8 serverID;
    struct AnsClientPeer* peer = NULL;
    struct AnsBoard* board = NULL;
    AnsStatus ans_status;
    MemChunk command_memory;
    MemChunk response_memory;
    AiUInt32 command_frame_size;
    AnsCmdFrame* command_frame = NULL;
    Ans1553CallbackPayload* command_payload = NULL;  // ?
    int i, j;
    AiBoolean handlers_left;
    struct mil_net_layer_properties* net_layer = NULL;

    if (!device)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    net_layer = device->net_layer;

    MemChunk_init(&command_memory);
    MemChunk_init(&response_memory);

    moduleID = GET_MODULE_ID(ulModHandle);
    serverID = GET_SERVER_ID(ulModHandle);

    peer = ANS_SERVER_ID_TO_PEER(serverID);
    if (!peer)
    {
        return API_ERR_SERVER;
    }
    board = AnsClientPeer_requestBoard(peer, ANS_BOARD_HANDLE(serverID, moduleID));

    if (!board)
    {
        return API_ERR_WRONG_MODULE;
    }

    /* Lock access to net layer here,
    * so we can safely uninstall the handler and close the event observer if no more necessary
    */
    ai_mutex_lock(net_layer->lock);

    ret = API_OK;
    do
    {
        /* First unregister event callback on remote peer */
        command_frame_size = sizeof(ANS_Header) + ANS_CMD_PAYLOAD(Ans1553CallbackPayload);

        if (!MemChunk_allocate(&command_memory, command_frame_size))
        {
            ret = API_ERR_MALLOC_FAILED;
            break;
        }

        command_frame = (AnsCmdFrame*)command_memory.pMemory;
        command_payload = (Ans1553CallbackPayload*)command_frame->payload;

        command_frame->header.commandtype = BoardCommand;
        command_frame->header.functionId = UnregisterCallbackID;

        command_payload->ModHandle = GET_LOCAL_MODULE_HANDLE(ulModHandle);
        command_payload->type      = uc_Type;
        command_payload->biu       = API_INT_LS;

        if( IS_HS_ACCESS(ulModHandle)) {
            command_payload->biu = API_INT_HS;
        }

        ans_status = AnsClientPeer_transmitBoardCommand(peer, board, command_frame, command_frame_size, &response_memory);
        if (ans_status != AnsStatus_OK)
        {
            ret = API_ERR_UNKNOWN_COMMAND;
            break;
        }
        device->ax_IntFuncTable[uc_Biu].af_IntFunc[uc_Type] = NULL;

        /* Check if any handler function is left.
        * If not, stop event publisher thread
        */
        for (i = 0, handlers_left = AiFalse; i < AI_ARRAY_COUNT(device->ax_IntFuncTable); i++)
        {
            for( j=0; j<AI_ARRAY_COUNT(device->ax_IntFuncTable[0].af_IntFunc); j++ )
            {
                if (device->ax_IntFuncTable[i].af_IntFunc[j])
                {
                    handlers_left = AiTrue;
                    break;
                }
            }
        }

        if (!handlers_left && net_layer->event_observer)
        {
            ans_status = AnsClientPeer_closeBoardEventStream(peer, board, net_layer->event_observer->handle);
            if (ans_status != AnsStatus_OK)
            {
                ret = API_ERR_SERVER;
                break;
            }

            ANS_JOIN_OS_THREAD(device->_hInterruptThread);
            AnsBoardEventObserver_destroy(net_layer->event_observer);
            net_layer->event_observer = NULL;
            device->_hInterruptThread = INVALID_HANDLE_VALUE;
        }

    } while (0);

    ai_mutex_release(net_layer->lock);

    AnsClientPeer_releaseBoard(peer, board);

    MemChunk_free(&command_memory);
    MemChunk_free(&response_memory);

    return ret;
}


AiReturn Net1553DeviceInit(AiUInt32 ulModHandle)
{
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ulModHandle);
    struct mil_net_layer_properties* net_layer;
    
    device->net_layer = malloc(sizeof(struct mil_net_layer_properties));
    if (!(net_layer = device->net_layer))
    {
        return API_ERR_MALLOC_FAILED;
    }

    net_layer->event_observer = NULL;
    device->net_layer->lock = ai_mutex_create();

    return API_OK;
}

void Net1553DeviceFree(AiUInt32 ulModHandle)
{
    AiUInt8 server_id = GET_SERVER_ID(ulModHandle);
    struct AnsClientPeer* peer = ANS_SERVER_ID_TO_PEER(server_id);
    struct AnsBoard* board = NULL;
    TY_DEVICE_INFO* device = _ApiGetDeviceInfoPtrByModule(ulModHandle);

    if (peer)
    {
        board = AnsClientPeer_requestBoard(peer, (AiHandle)(AiUIntPtr) ulModHandle);
        AnsClientPeer_releaseBoard(peer, board);

        if (board)
        {
           _MilNetClose(ulModHandle, AiTrue);
        }

    }

    if (device->net_layer)
    {
        ai_mutex_lock(device->net_layer->lock);
        if (device->net_layer->event_observer)
        {
            AnsClientPeer_closeBoardEventStream(peer, board, device->net_layer->event_observer->handle);

            if (device->_hInterruptThread != INVALID_HANDLE_VALUE)
            {
                ANS_JOIN_OS_THREAD(device->_hInterruptThread);
            }

            AnsBoardEventObserver_destroy(device->net_layer->event_observer);

        }
        ai_mutex_release(device->net_layer->lock);

        ai_mutex_free(device->net_layer->lock);
        free(device->net_layer);
    }
}
