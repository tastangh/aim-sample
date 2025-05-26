/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */


/*! \file BoardProtocolHandlers.c
 *
 *  This file contains definitions for the board
 *  command handler functions of the ANS1553
 */


#include "BoardProtocolHandlers.h"
#include "mil/ANS1553_ProtocolCommands.h"
#include "ANS_Log.h"
#include "Api1553.h"

#include <string.h>

#define MODULENAME "BoardProtocolHandler"




AnsCmdRspFrame* handleProtocolCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory)
{
  AnsCmdRspFrame* responseFrame = NULL;

  ANSLogDebug("Handling protocol command id %d", command->header.functionId );

  switch (command->header.functionId)
  {
  case ProtocolDataQueueOpenID:
      responseFrame = handleDataQueueOpenCommand( command, response_memory );
      break;
  case ProtocolDataQueueCloseID:
      responseFrame = handleDataQueueCloseCommand( command, response_memory );
      break;
  case ProtocolDataQueueControlID:
      responseFrame = handleDataQueueControlCommand( command, response_memory );
      break;
  case ProtocolDataQueueReadID:
      responseFrame = handleDataQueueReadCommand( command, response_memory );
      break;
  case ProtocolGetDeviceConfigID:
      responseFrame = handleGetDeviceConfigCommand( command, response_memory );
      break;
  case ProtocolSetDeviceConfigID:
      responseFrame = handleSetDeviceConfigCommand( command, response_memory );
      break;
  default:
      ANSLogError( MODULENAME ": ERROR: No handler for protocol command with ID %u provided.", command->header.functionId );
      responseFrame = NULL;
      break;
  }

  return responseFrame;
}




AnsCmdRspFrame* handleDataQueueOpenCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
    Ans1553CmdDataQueueOpenPayload*         payload         = (Ans1553CmdDataQueueOpenPayload*) commandFrame->payload;
    AiReturn                                apiReturn       = API_OK;
    AiUInt32                                payloadSize     = 0;
    AnsStatus                               ansStatus       = AnsStatus_OK;
    AnsCmdRspFrame*                         responseFrame   = NULL;
    Ans1553CmdDataQueueOpenResponsePayload* responsePayload = NULL;
    AiUInt32                                queueSize       = 0;

    ANSLogDebug("Handling Protocol Data Queue Open command for module %d queue id = %d", payload->ModHandle, payload->Id );

    /* Extract the function payload */

    apiReturn = ApiAnsDataQueueOpen(payload->ModHandle, payload->Id, &queueSize);

    ANSLogDebug("ApiCmdDataQueueOpen returnCode=%d queue_size=%d", apiReturn, (int)queueSize);

    /* Prepare the response frame */
    payloadSize = ANS_RSP_PAYLOAD(Ans1553CmdDataQueueOpenResponsePayload);

    ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy transaction ID
                                   payloadSize,
                                   0,
                                   payloadSize,    // fragment payload=payload as not fragmented
                                   commandFrame->header.ansHeader.clientId,  // copy Client ID
                                   responseMemory);

    if ( AnsStatus_OK != ansStatus )
    {
        return NULL;
    }

    /* Add the frame's payload  */
    responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
    responseFrame->header.functionId = commandFrame->header.functionId;
    responseFrame->header.status = (AiUInt32) ansStatus;

    responsePayload = (Ans1553CmdDataQueueOpenResponsePayload*) responseFrame->payload;

    responsePayload->ApiFunctionRc = apiReturn;
    responsePayload->QueueSize     = queueSize;

    return responseFrame;
}


AnsCmdRspFrame* handleDataQueueCloseCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
  Ans1553CmdDataQueueClosePayload*         payload         = (Ans1553CmdDataQueueClosePayload*) commandFrame->payload;
  AiReturn                                 apiReturn       = API_OK;
  AiUInt32                                 payloadSize     = 0;
  AnsStatus                                ansStatus       = AnsStatus_OK;
  AnsCmdRspFrame*                          responseFrame   = NULL;
  Ans1553CmdDataQueueCloseResponsePayload* responsePayload = NULL;

  ANSLogDebug("Handling Protocol Data Queue Close command for module %d queue id = %d", payload->ModHandle, payload->Id );

  /* Extract the function payload */

  apiReturn = ApiAnsDataQueueClose(payload->ModHandle, payload->Id);

  ANSLogDebug("ApiCmdDataQueueClose returnCode=%d", apiReturn);

  /* Prepare the response frame */
  payloadSize = ANS_RSP_PAYLOAD(Ans1553CmdDataQueueCloseResponsePayload);

  ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy transaction ID
                                 payloadSize,
                                 0,
                                 payloadSize,    // fragment payload=payload as not fragmented
                                 commandFrame->header.ansHeader.clientId,  // copy Client ID
                                 responseMemory);

  if ( AnsStatus_OK != ansStatus )
  {
      return NULL;
  }

  /* Add the frame's payload  */
  responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
  responseFrame->header.functionId = commandFrame->header.functionId;
  responseFrame->header.status = (AiUInt32) ansStatus;

  responsePayload = (Ans1553CmdDataQueueCloseResponsePayload*) responseFrame->payload;

  responsePayload->ApiFunctionRc = apiReturn;

  return responseFrame;
}








AnsCmdRspFrame* handleDataQueueControlCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
  Ans1553CmdDataQueueControlPayload*         payload         = (Ans1553CmdDataQueueControlPayload*) commandFrame->payload;
  AiReturn                                   apiReturn       = API_OK;
  AiUInt32                                   payloadSize     = 0;
  AnsStatus                                  ansStatus       = AnsStatus_OK;
  AnsCmdRspFrame*                            responseFrame   = NULL;
  Ans1553CmdDataQueueControlResponsePayload* responsePayload = NULL;

  ANSLogDebug("Handling Protocol Data Queue Control command mode %d for module %d queue id = %d", payload->Mode, payload->ModHandle, payload->Id );
  /* Extract the function payload */

  apiReturn = ApiAnsDataQueueControl(payload->ModHandle, payload->Id, payload->Mode);

  ANSLogDebug("ApiCmdDataQueueControl returnCode=%d", apiReturn);

  /* Prepare the response frame */
  payloadSize = ANS_RSP_PAYLOAD(Ans1553CmdDataQueueControlResponsePayload);

  ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy transaction ID
                                 payloadSize,
                                 0,
                                 payloadSize,    // fragment payload=payload as not fragmented
                                 commandFrame->header.ansHeader.clientId,  // copy Client ID
                                 responseMemory);

  if ( AnsStatus_OK != ansStatus )
  {
      return NULL;
  }

  /* Add the frame's payload  */
  responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
  responseFrame->header.functionId = commandFrame->header.functionId;
  responseFrame->header.status = (AiUInt32) ansStatus;

  responsePayload = (Ans1553CmdDataQueueControlResponsePayload*) responseFrame->payload;

  responsePayload->ApiFunctionRc = apiReturn;

  return responseFrame;
}

AnsCmdRspFrame* handleDataQueueReadCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
  Ans1553CmdDataQueueReadPayload*         payload         = (Ans1553CmdDataQueueReadPayload*) commandFrame->payload;
  AiUInt32                                payloadSize     = 0;
  AnsStatus                               ansStatus       = AnsStatus_OK;
  AnsCmdRspFrame*                         responseFrame   = NULL;
  AiReturn                                apiReturn       = API_OK;
  MemChunk                                data;
  Ans1553CmdDataQueueReadResponsePayload* responsePayload = NULL;
  TY_API_DATA_QUEUE_READ                  dataQueue;
  TY_API_DATA_QUEUE_STATUS                dataQueueStatus;

  MemChunk_init(&data);

  memset(&dataQueue, 0, sizeof(dataQueue));
  memset(&dataQueueStatus, 0, sizeof(dataQueueStatus));

  ANSLogDebug("Handling Protocol Data Queue Read command size %d for module %d queue id = %d", payload->BytesToRead, payload->ModHandle, payload->Id);

  if (!MemChunk_allocate(&data, payload->BytesToRead))
  {
    ANSLogError("handleDataQueueReadCommand out of memory");
    return NULL;
  }

  /* Input parameters */
  dataQueue.id            = payload->Id;
  dataQueue.bytes_to_read = payload->BytesToRead;

  /* Output parameters */
  dataQueue.buffer       = data.pMemory;

  apiReturn = ApiAnsDataQueueRead(payload->ModHandle, &dataQueue, &dataQueueStatus);

  ANSLogDebug("ApiReadBlockMemData returnCode=%d", apiReturn);


  /* Prepare the response frame */
  
  /* calc payload size */
  if (dataQueueStatus.bytes_transfered > payload->BytesToRead)
  {
      ANSLogError("ApiAnsDataQueueRead too much data returned %d > %d", dataQueueStatus.bytes_transfered, payload->BytesToRead );
      dataQueueStatus.bytes_transfered = payload->BytesToRead;
  }


  payloadSize  = ANS_RSP_PAYLOAD(Ans1553CmdDataQueueReadResponsePayload);
  payloadSize -= 1; /* dummy data entry */
  payloadSize += dataQueueStatus.bytes_transfered;

  ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId,  // copy Transaction ID
                                 payloadSize,
                                 0,
                                 payloadSize,    // fragment payload is the same as payload if no fragmentation is used
                                 commandFrame->header.ansHeader.clientId,   // copy Client ID
                                 responseMemory);

  if ( AnsStatus_OK != ansStatus )
  {
      MemChunk_free(&data);
      ANSLogError("handleDataQueueReadCommand out of memory on header create");
      return NULL;
  }

  /* Point to the memory where the data has to be read to */
  responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
  responsePayload = (Ans1553CmdDataQueueReadResponsePayload*) responseFrame->payload;

  
  memcpy(responsePayload->Data, dataQueue.buffer, dataQueueStatus.bytes_transfered);


  /* Add the frame's payload  */

  responseFrame->header.functionId = commandFrame->header.functionId;
  responseFrame->header.status     = (AiUInt32) ansStatus;

  responsePayload->ApiFunctionRc           = apiReturn;
  responsePayload->Status                  = dataQueueStatus.status;
  responsePayload->BytesTransferred        = dataQueueStatus.bytes_transfered;
  responsePayload->BytesInQueue            = dataQueueStatus.bytes_in_queue;
  responsePayload->TotalBytesTransferredLo = (AiUInt32)(dataQueueStatus.total_bytes_transfered);
  responsePayload->TotalBytesTransferredHi = (AiUInt32)(dataQueueStatus.total_bytes_transfered >> 32);

  MemChunk_free(&data);

  return responseFrame;
}


AnsCmdRspFrame* handleGetDeviceConfigCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
  Ans1553GetDeviceConfigPayload*         payload         = (Ans1553GetDeviceConfigPayload*) commandFrame->payload;
  AiUInt32                               moduleHandle    = 0;
  AiReturn                               apiReturn       = API_OK;
  AiUInt32                               payloadSize     = 0;
  AnsStatus                              ansStatus       = AnsStatus_OK;
  AnsCmdRspFrame*                        responseFrame   = NULL;
  Ans1553GetDeviceConfigResponsePayload* responsePayload = NULL;
  TY_API_DEVICE_CONFIG                   deviceConfig;


  ANSLogDebug("Handling Protocol Get Device Config command for module %d", payload->ModHandle );
  /* Extract the function payload */

  moduleHandle               = payload->ModHandle;

  apiReturn = ApiGetDeviceConfig( moduleHandle, &deviceConfig );

  ANSLogDebug("ApiGetDeviceConfig returnCode=%d", apiReturn);

  /* Prepare the response frame */
  payloadSize = ANS_RSP_PAYLOAD(Ans1553GetDeviceConfigResponsePayload);

  ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy transaction ID
                                 payloadSize,
                                 0,
                                 payloadSize,    // fragment payload=payload as not fragmented
                                 commandFrame->header.ansHeader.clientId,  // copy Client ID
                                 responseMemory);

  if ( AnsStatus_OK != ansStatus )
  {
      return NULL;
  }

  /* Add the frame's payload  */
  responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
  responseFrame->header.functionId = commandFrame->header.functionId;
  responseFrame->header.status = (AiUInt32) ansStatus;

  responsePayload = (Ans1553GetDeviceConfigResponsePayload*) responseFrame->payload;

  responsePayload->ApiFunctionRc       = apiReturn;
  responsePayload->DmaEnabled          = deviceConfig.uc_DmaEnabled;
  responsePayload->DataQueueMemoryType = deviceConfig.uc_DataQueueMemoryType;
  responsePayload->DataQueueMode       = deviceConfig.uc_DataQueueMode;
  responsePayload->ReservedB4          = deviceConfig.uc_ReservedB4;
  responsePayload->ReservedW1          = deviceConfig.uw_ReservedW1;
  responsePayload->ReservedW2          = deviceConfig.uw_ReservedW2;
  responsePayload->DmaMinimumSize      = deviceConfig.ul_DmaMinimumSize;
  responsePayload->IntRequestCount     = deviceConfig.ul_IntRequestCount;
  responsePayload->DriverFlags         = deviceConfig.ul_DriverFlags;
  responsePayload->ReservedLW4         = deviceConfig.ul_ReservedLW4;
  responsePayload->ReservedLW5         = deviceConfig.ul_ReservedLW5;
  responsePayload->ReservedLW6         = deviceConfig.ul_ReservedLW6;
  responsePayload->ReservedLW7         = deviceConfig.ul_ReservedLW7;
  responsePayload->ReservedLW8         = deviceConfig.ul_ReservedLW8;

  return responseFrame;
}

AnsCmdRspFrame* handleSetDeviceConfigCommand(AnsCmdFrame* commandFrame, MemChunk* responseMemory)
{
  Ans1553SetDeviceConfigPayload*         payload         = (Ans1553SetDeviceConfigPayload*) commandFrame->payload;
  AiUInt32                               moduleHandle    = 0;
  AiReturn                               apiReturn       = API_OK;
  AiUInt32                               payloadSize     = 0;
  AnsStatus                              ansStatus       = AnsStatus_OK;
  AnsCmdRspFrame*                        responseFrame   = NULL;
  Ans1553SetDeviceConfigResponsePayload* responsePayload = NULL;
  TY_API_DEVICE_CONFIG                   deviceConfig;


  ANSLogDebug("Handling Protocol Set Device Config command for module %d", payload->ModHandle );
  /* Extract the function payload */

  moduleHandle                        = payload->ModHandle;
  deviceConfig.uc_DmaEnabled          = payload->DmaEnabled;
  deviceConfig.uc_DataQueueMemoryType = payload->DataQueueMemoryType;
  deviceConfig.uc_DataQueueMode       = payload->DataQueueMode;
  deviceConfig.uc_ReservedB4          = payload->ReservedB4;
  deviceConfig.uw_ReservedW1          = payload->ReservedW1;
  deviceConfig.uw_ReservedW2          = payload->ReservedW2;
  deviceConfig.ul_DmaMinimumSize      = payload->DmaMinimumSize;
  deviceConfig.ul_IntRequestCount     = payload->IntRequestCount;
  deviceConfig.ul_DriverFlags         = payload->DriverFlags;
  deviceConfig.ul_ReservedLW4         = payload->ReservedLW4;
  deviceConfig.ul_ReservedLW5         = payload->ReservedLW5;
  deviceConfig.ul_ReservedLW6         = payload->ReservedLW6;
  deviceConfig.ul_ReservedLW7         = payload->ReservedLW7;
  deviceConfig.ul_ReservedLW8         = payload->ReservedLW8;

  apiReturn = ApiSetDeviceConfig( moduleHandle, &deviceConfig );

  ANSLogDebug("ApiSetDeviceConfig returnCode=%d", apiReturn);

  /* Prepare the response frame */
  payloadSize = ANS_RSP_PAYLOAD(Ans1553GetDeviceConfigResponsePayload);

  ansStatus =  ANS_Header_create(commandFrame->header.ansHeader.transactionId, // copy transaction ID
                                 payloadSize,
                                 0,
                                 payloadSize,    // fragment payload=payload as not fragmented
                                 commandFrame->header.ansHeader.clientId,  // copy Client ID
                                 responseMemory);

  if ( AnsStatus_OK != ansStatus )
  {
      return NULL;
  }

  /* Add the frame's payload  */
  responseFrame = (AnsCmdRspFrame *) responseMemory->pMemory;
  responseFrame->header.functionId = commandFrame->header.functionId;
  responseFrame->header.status = (AiUInt32) ansStatus;

  responsePayload = (Ans1553SetDeviceConfigResponsePayload*) responseFrame->payload;

  responsePayload->ApiFunctionRc       = apiReturn;

  return responseFrame;

}


