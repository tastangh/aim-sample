/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */


/*! \file BoardCommandHandlers.c
 *
 *  This file contains definitions for the board
 *  command handler functions of the ANS1553
 *
 *  Created on: 28.11.2013
 *  Author: Martin Haag
 */


#include "BoardCommandHandlers.h"
#include "mil/ANS1553_BoardCommands.h"
#include "ANS_Server.h"
#include "ANS_Log.h"
#include "ANS_Board.h"
#include "Api1553.h"
#include "Ai1553i_def.h"
#include "AiOs.h"

#include <string.h>


/*! \def ANS1553_INVALID_BOARD_HANDLE
* As this is no valid local handle, we can use it
* for marking invalid handles
*/
#define ANS1553_INVALID_BOARD_HANDLE 0xFF



static AnsCmdRspFrame* processIoCommand (AiUInt8* byteAck, AiUInt16* wordAck, AiUInt32* lWordAck,
                                         AiUInt32 maxBytesPerBuffer, AnsCmdFrame* commandFrame, MemChunk* responseMemory);


static void Ans1553EventHandler(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type,
                                      struct ty_api_intr_loglist_entry* info )
{
    Ans1553Event event;
    struct AnsBoard* board = NULL;

    board = AnsServer_requestBoard(&g_AnsServer, (AiHandle)GET_MODULE_ID(bModule));
    if(!board)
    {
        ANSLogWarn("Received channel event for unknown board with handle %u", bModule);
        return;
    }

    event.ModHandle = bModule;
    event.biu = biu;
    event.type = uc_Type;

    memcpy(&event.event_data, info, sizeof(struct ty_api_intr_loglist_entry));

    AnsBoard_publishEvent(board, (const char*) &event, sizeof(Ans1553Event));
}


AnsCmdRspFrame* handleOpenCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553OpenCmdPayload*      payload         = (Ans1553OpenCmdPayload*) command->payload;
    TY_API_OPEN                 open;
    AiUInt32                    moduleHandle    = 0;
    AiReturn                    apiReturn       = API_OK;
    AiUInt32                    payloadSize     = 0;
    AnsStatus                   ansStatus       = AnsStatus_OK;
    AnsCmdRspFrame*             responseFrame   = NULL;
    Ans1553OpenResponsePayload* responsePayload = NULL;


    /* Extract the function payload */
    open.ul_Module = payload->Module;
    open.ul_Stream = payload->Stream;
    AiOsStrncpy( open.ac_SrvName, "local", sizeof("local"), AiFalse);

    ANSLogDebug("Handling open command for module %d", open.ul_Module );

    apiReturn = ApiOpenEx( &open, &moduleHandle );
    if (apiReturn == API_OK)
    {
        AnsBoard_init(board, (AiHandle)(AiUIntPtr) open.ul_Module);
    }
    else
    {
        AnsBoard_init(board, (AiHandle)(AiUIntPtr) ANS1553_INVALID_BOARD_HANDLE);
    }

    ANSLogDebug("ApiOpenEx returnCode=%d handle=0x%08X", apiReturn, (int) moduleHandle);

    /* Prepare the response frame */
    payloadSize = ANS_RSP_PAYLOAD(Ans1553OpenResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId, // copy transaction ID
                                   payloadSize,
                                   0,
                                   payloadSize,    // fragment payload=payload as not fragmented
                                   command->header.ansHeader.clientId,  // copy Client ID
                                   response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    /* Add the frame's payload  */
    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    responsePayload = (Ans1553OpenResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->ModHandle     = moduleHandle;

    return responseFrame;
}


AnsCmdRspFrame* handleCloseCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553CloseCmdPayload* payload = (Ans1553CloseCmdPayload*) command->payload;
    AiUInt32  module = 0;
    AiReturn  apiReturn = API_OK;
    AiUInt32  payloadSize = 0;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553CloseResponsePayload* responsePayload = NULL;

    module = payload->ModHandle;

    ANSLogDebug("Handling close command for module 0x%08X\n", module);

    apiReturn = ApiClose(module);

    ANSLogDebug("ApiClose returnCode=%d", apiReturn);

    /* Prepare the response frame */
    payloadSize = ANS_RSP_PAYLOAD(Ans1553CloseResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,  // copy Transaction ID
                                   payloadSize,
                                   0,
                                   payloadSize,   // fragment payload is the same as payload if no fragmentation is used
                                   command->header.ansHeader.clientId,  // copy Client ID
                                   response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    /* Add the frame's payload  */
    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    responsePayload = (Ans1553CloseResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;

    return responseFrame;
}


AnsCmdRspFrame* handleReadMemCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553ReadMemCmdPayload* payload = (Ans1553ReadMemCmdPayload*) command->payload;
    AiUInt32 payloadSize = 0;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    AiUInt32 bytesRead = 0;
    AiReturn apiReturn = API_OK;
    Ans1553ReadMemResponsePayload* responsePayload = NULL;


    ANSLogDebug("Handling read memory command on module 0x%d with memtype %d, offset 0x%x, width %d size %d", payload->ModHandle,
                                                                                                            payload->Memtype,
                                                                                                            payload->Offset,
                                                                                                            payload->Width,
                                                                                                            payload->NumElements);

    /* Prepare the response frame */
    /* calc payload size */

    payloadSize  = ANS_RSP_PAYLOAD(Ans1553ReadMemResponsePayload);
    payloadSize += payload->NumElements * payload->Width;
    payloadSize -= 1;   /* dummy data entry */

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,  // copy Transaction ID
                                   payloadSize,
                                   0,
                                   payloadSize,    // fragment payload is the same as payload if no fragmentation is used
                                   command->header.ansHeader.clientId,   // copy Client ID
                                   response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    /* Point to the memory where the data has to be read to */
    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responsePayload = (Ans1553ReadMemResponsePayload*) responseFrame->payload;

    apiReturn = ApiReadBlockMemData( payload->ModHandle,
                                       payload->Memtype,
                                       payload->Offset,
                                       payload->Width, /* width */
                                       responsePayload->Data,    // Data to read to in inTxBuffer
                                       payload->NumElements,
                                       &bytesRead);

    ANSLogDebug("ApiReadBlockMemData returnCode=%d", apiReturn);

    /* Add the frame's payload  */

    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status     = (AiUInt32) ansStatus;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->BytesRead     = bytesRead;

    return responseFrame;
}


AnsCmdRspFrame* handleWriteMemCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553WriteMemCmdPayload* payload = (Ans1553WriteMemCmdPayload*) command->payload;
    AiUInt32 bytesWritten = 0;
    AiReturn apiReturn = API_OK;
    AiUInt32 payloadSize = 0;
    Ans1553WriteMemResponsePayload* responsePayload = NULL;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;

    ANSLogDebug("Handling write memory command on module 0x%08X with memtype %d, offset 0x%x, width %d, size %d", payload->ModHandle,
                                                                                                              payload->Memtype,
                                                                                                              payload->Offset,
                                                                                                              payload->Width,
                                                                                                              payload->NumElements);

    apiReturn = ApiWriteBlockMemData(payload->ModHandle,
                                     payload->Memtype,
                                     payload->Offset,
                                     payload->Width,
                                     payload->Data,   /* Data to write from */
                                     payload->NumElements,
                                     &bytesWritten);

    ANSLogDebug("ApiWriteBlockMemData returnCode=%d", apiReturn);

    /* Prepare the response frame */
    payloadSize = ANS_RSP_PAYLOAD(Ans1553WriteMemResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,  // copy Transaction ID from CmdFrame
                                   payloadSize,
                                   0,
                                   payloadSize,    // fragment payload=payload as not fragmented
                                   command->header.ansHeader.clientId,  // copy Client ID from CmdFrame
                                   response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    /* Add the frame's payload  */
    responseFrame = (AnsCmdRspFrame*) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    responsePayload = (Ans1553WriteMemResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->BytesWritten  = bytesWritten;

    return responseFrame;
}


AnsCmdRspFrame* handleIoCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    AiUInt8* byteArray = NULL;
    AiUInt16* wordArray = NULL;
    AiUInt32* lwordArray = NULL;
    AnsCmdRspFrame* responseFrame = NULL;

    /*
     * Allocate memory for byteArray, wordArray and lwordArray
     * (16k each).
     */
    byteArray  = (AiUInt8  *) malloc(MAX_TG_CMD_SIZE);
    wordArray  = (AiUInt16 *) malloc(MAX_TG_CMD_SIZE);
    lwordArray = (AiUInt32 *) malloc(MAX_TG_CMD_SIZE);

    if ( NULL == byteArray || NULL == wordArray || NULL == lwordArray )
    {
        if(byteArray  != NULL) free(byteArray);
        if(wordArray  != NULL) free(wordArray);
        if(lwordArray != NULL) free(lwordArray);

        ANSLogError("Out of memory at ApiIo!");
        return NULL;
    }

    responseFrame = processIoCommand(byteArray, wordArray, lwordArray, MAX_TG_CMD_SIZE,
                                     command, response_memory);

    free(byteArray);
    free(wordArray);
    free(lwordArray);

    return responseFrame;
}


static AnsCmdRspFrame* processIoCommand (AiUInt8* byteAck, AiUInt16* wordAck, AiUInt32* lWordAck,
                                         AiUInt32 maxBytesPerBuffer, AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
    AnsStatus ansStatus = AnsStatus_Error;
    short rc_inByteSize = 0; // return code from target (driver)
    short rc_inWordSize = 0;
    short rc_inLWordSize = 0;
    AiUInt32 payloadSize = 0;
    AnsCmdRspFrame* responseFrame = NULL;
    AiUInt8* p_outByteArray = NULL;
    AiUInt16* p_outWordArray = NULL;
    AiUInt8* ackData = NULL;
    Ans1553IoCmdPayload* payload = (Ans1553IoCmdPayload*) commandFrame->payload;
    AiReturn apiReturn = API_OK;
    Ans1553IoResponsePayload* responsePayload = NULL;

    p_outWordArray = (AiUInt16*) payload->Data;
    p_outByteArray = (AiUInt8*) (p_outWordArray + payload->OutWordSize);


    apiReturn = ApiIo(payload->ModHandle, payload->Biu, payload->Cmd, payload->ExpectAck, p_outByteArray,
                         payload->OutByteSize, p_outWordArray, payload->OutWordSize,
                         byteAck, &rc_inByteSize, wordAck, &rc_inWordSize, lWordAck,
                         &rc_inLWordSize);

    if(apiReturn)
    {
        ANSLogError("OutByte size: %d Out Word Size: %d", payload->OutByteSize, payload->OutWordSize);
    }


    ANSLogDebug("ApiIo returnCode=%d", apiReturn);

    /*
     * Ensure that the data length reported by the underlying API is in
     * the expected range.
     */

    if (rc_inByteSize > (AiInt16) maxBytesPerBuffer)
    {
        ANSLogError(" ApiIo returned byteSize > max(%d): %d", maxBytesPerBuffer, rc_inByteSize);
        return NULL;
    }

    if (rc_inWordSize > (AiInt16) (maxBytesPerBuffer / 2))
    {
        ANSLogError(" ApiIo returned wordSize > max(%d): %d", maxBytesPerBuffer / 2, rc_inWordSize);
        return NULL;
    }

    if (rc_inLWordSize > (AiInt16) (maxBytesPerBuffer / 4))
    {
        ANSLogError(" ApiIo returned LWordSize > max(%d): %d", maxBytesPerBuffer / 4, rc_inLWordSize);
        return NULL;
    }

    /*
     * Prepare the response frame.
     */
    payloadSize = ANS_RSP_PAYLOAD(Ans1553IoResponsePayload);
    payloadSize -= 1; /* - dummy data */
    payloadSize += rc_inByteSize + (rc_inWordSize * 2) + (rc_inLWordSize * 4);

    ansStatus = ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy Transaction ID
                                  payloadSize, 0, payloadSize, // fragment payload=payload as not fragmentated
                                  commandFrame->header.ansHeader.clientId, // copy Client ID
                                  responseMemory);

    if (AnsStatus_OK != ansStatus)
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
    responseFrame->header.functionId = commandFrame->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    responsePayload = (Ans1553IoResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->InByteSize  = rc_inByteSize;
    responsePayload->InWordSize  = rc_inWordSize;
    responsePayload->InLWordSize = rc_inLWordSize;

    /*
     * Copy the output data back to the caller's memory.
     */
    ackData = responsePayload->Data;

    if (rc_inLWordSize > 0)
    {
        memcpy(ackData, lWordAck, rc_inLWordSize * sizeof(AiUInt32));
        ackData += rc_inLWordSize * sizeof(AiUInt32);
    }

    if (rc_inWordSize > 0)
        {
            memcpy(ackData, wordAck, rc_inWordSize * sizeof(AiUInt16));
            ackData += rc_inWordSize * sizeof(AiUInt16); /* Advance the write pointer    */
        }

    if (rc_inByteSize > 0)
    {
        memcpy(ackData, byteAck, rc_inByteSize);
    }

    return responseFrame; /** \return Final response frame on success     */
}


AnsCmdRspFrame* handleReadVersionCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    AiUInt32 payloadSize = 0;
    Ans1553ReadVersionCmdPayload* payload = (Ans1553ReadVersionCmdPayload*) command->payload;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553ReadVersionResponsePayload* responsePayload = NULL;
    AiReturn apiReturn = API_OK;

    TY_VER_INFO version;

    memset( &version, 0, sizeof(TY_VER_INFO) );

    ANSLogDebug("Handling 'Read Version' command for module 0x%08X", payload->ModHandle );

    apiReturn = ApiReadVersion( payload->ModHandle, payload->VersionId, &version );

    ANSLogDebug("ApiReadVersion returnCode=%d eId=%d %s: %s", apiReturn, payload->VersionId, version.ac_Description, version.ac_FullVersion );

    payloadSize = ANS_RSP_PAYLOAD(Ans1553ReadVersionResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,
                                   payloadSize, 0, payloadSize,
                                   command->header.ansHeader.clientId, response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    /* Add the response payload. */
    responsePayload = (Ans1553ReadVersionResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    memcpy( &responsePayload->Version, &version, sizeof(TY_VER_INFO) );

    return responseFrame;

}





AnsCmdRspFrame* handleGetDriverVersionCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    AiUInt32 payloadSize = 0;
    Ans1553GetDriverVersionCmdPayload* payload = (Ans1553GetDriverVersionCmdPayload*) command->payload;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553GetDriverVersionResponsePayload* responsePayload = NULL;
    AiReturn apiReturn = API_OK;
    AiUInt32 driverVersion = 0;
    AiUInt32 driverBuild = 0;

    ANSLogDebug("Handling 'Get Driver Version' command for module 0x%08X", payload->ModHandle );

    apiReturn = ApiCmdGetSysDrvVersion( payload->ModHandle, &driverVersion, &driverBuild );

    ANSLogDebug("ApiCmdGetSysDrvVersion returnCode=%d version: %d, build %d", apiReturn, driverVersion, driverBuild);

    payloadSize = ANS_RSP_PAYLOAD(Ans1553GetDriverVersionResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,
                                   payloadSize, 0, payloadSize,
                                   command->header.ansHeader.clientId, response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    /* Add the response payload. */
    responsePayload = (Ans1553GetDriverVersionResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->SysDrvBuild = driverBuild;
    responsePayload->SysDrvVer = driverVersion;

    return responseFrame;

}


AnsCmdRspFrame* handleIoCommandStruct(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553IoCommandStructCmdPayload* payload = (Ans1553IoCommandStructCmdPayload*) command->payload;
    TY_MIL_COM_ACK* targetAck = NULL;
    AiReturn apiReturn = API_OK;
    AiUInt32 payloadSize = 0;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553IoCommandStructResponsePayload* responsePayload = NULL;
    TY_MIL_COM* targetCommand = NULL;
    AiBoolean success = TRUE;

    ANSLogDebug("Handling 'IoStruct' command for module 0x%08X", payload->ModHandle);

    success = TRUE;
    do
    {
        targetCommand = malloc(payload->Size);
        if(!targetCommand)
        {
            ANSLogError("Failed to allocate command memory");
            success = FALSE;
            break;
        }

        targetAck = (TY_MIL_COM_ACK*) malloc(payload->ExpectedAckSize);
        if(!targetAck)
        {
            ANSLogError("Failed to allocate ack memory");
            success = FALSE;
            break;
        }

        memcpy(targetCommand, payload->CommandData, payload->Size);

        apiReturn = ApiIoStruct(payload->ModHandle, targetCommand, targetAck);

        ANSLogDebug("ApiIoStruct returnCode=%d", apiReturn);

        payloadSize = ANS_RSP_PAYLOAD(Ans1553IoCommandStructResponsePayload);
        payloadSize -= 1; /* Dummy ack data member */

        if(apiReturn == API_OK)
        {
            payloadSize += targetAck->ulSize;
        }

        ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId, payloadSize, 0, payloadSize,
                                       command->header.ansHeader.clientId, response_memory);
        if ( AnsStatus_OK != ansStatus )
        {
            success = FALSE;
            break;
        }

        responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
        responseFrame->header.functionId = command->header.functionId;
        responseFrame->header.status = (AiUInt32) ansStatus;

        /* Add the response payload. */
        responsePayload = (Ans1553IoCommandStructResponsePayload*) responseFrame->payload;

        responsePayload->ApiFunctionRc = apiReturn;

        if(apiReturn == API_OK)
        {
            responsePayload->Size = targetAck->ulSize;
            memcpy(responsePayload->ResponseData, targetAck, targetAck->ulSize);
        }
    }while(0);


    if(targetCommand)
    {
        free(targetCommand);
    }

    if(targetAck)
    {
        free(targetAck);
    }

    if(!success)
    {
        return NULL;
    }

    return responseFrame;
}


AnsCmdRspFrame* handleGetDriverInfoCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
    AiUInt32 payloadSize = 0;
    Ans1553GetDriverInfoCmdPayload* payload = (Ans1553GetDriverInfoCmdPayload*) command->payload;
    AnsStatus ansStatus = AnsStatus_OK;
    AnsCmdRspFrame* responseFrame = NULL;
    Ans1553GetDriverInfoResponsePayload* responsePayload = NULL;
    AiReturn apiReturn = API_OK;
    TY_API_DRIVER_INFO xDriverInfo;

    ANSLogDebug("Handling 'Get Driver Info' command for module 0x%08X", payload->ModHandle );

    apiReturn = ApiGetDriverInfo( payload->ModHandle, &xDriverInfo );

    ANSLogDebug("ApiGetDriverInfo returnCode=%d group=%d, connections=%d", apiReturn, xDriverInfo.uc_DeviceGroup, xDriverInfo.ul_OpenConnections );

    payloadSize = ANS_RSP_PAYLOAD(Ans1553GetDriverInfoResponsePayload);

    ansStatus =  ANS_Header_create(command->header.ansHeader.transactionId,
                                   payloadSize, 0, payloadSize,
                                   command->header.ansHeader.clientId, response_memory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    responseFrame = (AnsCmdRspFrame *) response_memory->pMemory;
    responseFrame->header.functionId = command->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    /* Add the response payload. */
    responsePayload = (Ans1553GetDriverInfoResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc   = apiReturn;
    responsePayload->DeviceGroup     = xDriverInfo.uc_DeviceGroup;
    responsePayload->ReservedB2      = xDriverInfo.uc_ReservedB2;
    responsePayload->ReservedB3      = xDriverInfo.uc_ReservedB3;
    responsePayload->ReservedB4      = xDriverInfo.uc_ReservedB4;
    responsePayload->ReservedW1      = xDriverInfo.uw_ReservedW1;
    responsePayload->ReservedW2      = xDriverInfo.uw_ReservedW2;
    responsePayload->DriverFlags     = xDriverInfo.ul_DriverFlags;
    responsePayload->SN              = xDriverInfo.ul_SN;
    responsePayload->BoardConfig     = xDriverInfo.ul_BoardConfig;
    responsePayload->BoardType       = xDriverInfo.ul_BoardType;
    responsePayload->OpenConnections = xDriverInfo.ul_OpenConnections;
    responsePayload->ReservedLW6     = xDriverInfo.ul_ReservedLW6;
    responsePayload->ReservedLW7     = xDriverInfo.ul_ReservedLW7;
    responsePayload->ReservedLW8     = xDriverInfo.ul_ReservedLW8;

    return responseFrame;

}

AnsCmdRspFrame* handleRegisterCallbackCommand(struct AnsBoard* board, struct AnsConnection* connection,
                                              AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553CallbackPayload* command_payload = NULL;

    AiReturn ret;
    AiUInt32 response_size;
    AnsCmdRspFrame* response = NULL;
    Ans1553CallbackResponsePayload* response_payload = NULL;

    TY_INT_FUNC_PTR callback = Ans1553EventHandler;
    

    command_payload = (Ans1553CallbackPayload*) command->payload;

    ANSLogDebug("'register callback' command for module 0x%08x, channel %d and type %d", command_payload->ModHandle,
                                                                                     command_payload->biu,
                                                                                     command_payload->type);
    ret = ApiInstIntHandler(command_payload->ModHandle, command_payload->biu, command_payload->type, callback);
    
    ANSLogDebug("Api1553CallbackRegister returnCode=%d", ret);

    response_size = ANS_RSP_PAYLOAD(Ans1553CallbackResponsePayload);

    if(ANS_Header_create(command->header.ansHeader.transactionId, response_size, 0, response_size,
            command->header.ansHeader.clientId, response_memory) != AnsStatus_OK)
    {
        ANSLogError("%s: Failed to allocate response frame", __FUNCTION__);
        return NULL;
    }

    response = (AnsCmdRspFrame*) response_memory->pMemory;

    response->header.functionId = command->header.functionId;
    response->header.status = AnsStatus_OK;

    response_payload = (Ans1553CallbackResponsePayload*) response->payload;
    response_payload->api_return = ret;

    return response;
}


AnsCmdRspFrame* handleUnregisterCallbackCommand(struct AnsBoard* board, struct AnsConnection* connection,
                                                AnsCmdFrame* command, MemChunk* response_memory)
{
    Ans1553CallbackPayload* command_payload = NULL;
    AiReturn ret;
    AiUInt32 response_size;
    AnsCmdRspFrame* response = NULL;
    Ans1553CallbackResponsePayload* response_payload = NULL;

    command_payload = (Ans1553CallbackPayload*) command->payload;

    ANSLogDebug("'unregister callback' command for module 0x%08x, channel %d and type %d", command_payload->ModHandle,
                                                                                       command_payload->biu,
                                                                                       command_payload->type);

    ret = ApiDelIntHandler(command_payload->ModHandle, command_payload->biu, command_payload->type);

    ANSLogDebug("ApiDelIntHandler returnCode=%d", ret);

    response_size = ANS_RSP_PAYLOAD(Ans1553CallbackResponsePayload);

    if(ANS_Header_create(command->header.ansHeader.transactionId, response_size, 0, response_size,
                         command->header.ansHeader.clientId, response_memory) != AnsStatus_OK)
    {
        ANSLogError("%s: Failed to allocate response frame", __FUNCTION__);
        return NULL;
    }

    response = (AnsCmdRspFrame*) response_memory->pMemory;

    response->header.functionId = command->header.functionId;
    response->header.status = AnsStatus_OK;

    response_payload = (Ans1553CallbackResponsePayload*) response->payload;
    response_payload->api_return = ret;

    return response;
}

