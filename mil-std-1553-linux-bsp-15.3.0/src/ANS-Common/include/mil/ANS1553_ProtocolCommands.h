/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS1553_BoardCommands.h
 *
 *  This file contains declarations for
 *  MIL specific, board related commands
 *
 */

#ifndef ANS1553_PROTOCOLCOMMANDS_H_
#define ANS1553_PROTOCOLCOMMANDS_H_


/*! \enum ProtocolCommandID
 * Enumeration for protocol related command IDs \n
 * All values must be greater than BoardCommandID
 * specific board commands.
 */
typedef enum eProtocolCommandID
{
  ProtocolDataQueueOpenID    = 256,
  ProtocolDataQueueCloseID   = 257,
  ProtocolDataQueueControlID = 258,
  ProtocolDataQueueReadID    = 259,
  ProtocolGetDeviceConfigID  = 260,
  ProtocolSetDeviceConfigID  = 261
}ProtocolCommandID;




/**
 * MIL DataQueueOpen command payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueOpenPayload
{
    AiUInt32    ModHandle;
    AiUInt8     Id;
} Ans1553CmdDataQueueOpenPayload;
#pragma pack()


/**
 * MIL DataQueueOpen response payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueOpenResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
  AiUInt32   QueueSize;
} Ans1553CmdDataQueueOpenResponsePayload;
#pragma pack()





/**
 * MIL DataQueueClose command payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueClosePayload
{
    AiUInt32    ModHandle;
    AiUInt8     Id;
} Ans1553CmdDataQueueClosePayload;
#pragma pack()


/**
 * MIL DataQueueClose response payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueCloseResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
} Ans1553CmdDataQueueCloseResponsePayload;
#pragma pack()





/**
 * MIL DataQueueControl command payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueControlPayload
{
    AiUInt32    ModHandle;
    AiUInt8     Id;
    AiUInt8     Mode;
} Ans1553CmdDataQueueControlPayload;
#pragma pack()


/**
 * MIL DataQueueControl response payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueControlResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
} Ans1553CmdDataQueueControlResponsePayload;
#pragma pack()





/**
 * MIL DataQueueRead command payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueReadPayload
{
    AiUInt32    ModHandle;
    AiUInt8     Id;
    AiUInt8     ReservedB1;
    AiUInt16    ReservedW1;
    AiUInt32    BytesToRead;
} Ans1553CmdDataQueueReadPayload;
#pragma pack()


/**
 * MIL DataQueueRead response payload
 */
#pragma pack(1)
typedef struct _Ans1553CmdDataQueueReadResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
  AiUInt32   Status;
  AiUInt32   BytesTransferred;
  AiUInt32   BytesInQueue;
  AiUInt32   TotalBytesTransferredLo;
  AiUInt32   TotalBytesTransferredHi;
  AiUInt8    Data[1];
} Ans1553CmdDataQueueReadResponsePayload;
#pragma pack()





/**
 * MIL GetDeviceConfig command payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDeviceConfigPayload
{
    AiUInt32    ModHandle;
} Ans1553GetDeviceConfigPayload;
#pragma pack()


/**
 * MIL GetDeviceConfig response payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDeviceConfigResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
  AiUInt8    DmaEnabled;
  AiUInt8    DataQueueMemoryType;
  AiUInt8    DataQueueMode;
  AiUInt8    ReservedB4;
  AiUInt16   ReservedW1;
  AiUInt16   ReservedW2;
  AiUInt32   DmaMinimumSize;
  AiUInt32   IntRequestCount;
  AiUInt32   DriverFlags;
  AiUInt32   ReservedLW4;
  AiUInt32   ReservedLW5;
  AiUInt32   ReservedLW6;
  AiUInt32   ReservedLW7;
  AiUInt32   ReservedLW8;
} Ans1553GetDeviceConfigResponsePayload;
#pragma pack()




/**
 * MIL SetDeviceConfig command payload
 */
#pragma pack(1)
typedef struct _Ans1553SetDeviceConfigPayload
{
    AiUInt32   ModHandle;
    AiUInt8    DmaEnabled;
    AiUInt8    DataQueueMemoryType;
    AiUInt8    DataQueueMode;
    AiUInt8    ReservedB4;
    AiUInt16   ReservedW1;
    AiUInt16   ReservedW2;
    AiUInt32   DmaMinimumSize;
    AiUInt32   IntRequestCount;
    AiUInt32   DriverFlags;
    AiUInt32   ReservedLW4;
    AiUInt32   ReservedLW5;
    AiUInt32   ReservedLW6;
    AiUInt32   ReservedLW7;
    AiUInt32   ReservedLW8;
} Ans1553SetDeviceConfigPayload;
#pragma pack()


/**
 * MIL SetDeviceConfig response payload
 */
#pragma pack(1)
typedef struct _Ans1553SetDeviceConfigResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
} Ans1553SetDeviceConfigResponsePayload;
#pragma pack()

#endif /* ANS1553_PROTOCOLCOMMANDS_H_ */
