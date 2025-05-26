/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS1553_BoardCommands.h
 *
 *  This file contains declarations for
 *  MIL specific, board related commands
 *
 */

#ifndef ANS1553_BOARDCOMMANDS_H_
#define ANS1553_BOARDCOMMANDS_H_


#include "Ai_def.h"
#include "ANS_BoardCommands.h"

/**
 * MIL Open command payload
 */
#pragma pack(1)
typedef struct _Ans1553OpenCmdPayload
{
    AiUInt32   Module;       /*!< API Board Module Number to access */
    AiUInt32   Stream;       /*!< The stream to access              */
} Ans1553OpenCmdPayload;
#pragma pack()


/**
 * MIL Open response payload
 */
#pragma pack(1)
typedef struct _Ans1553OpenResponsePayload
{
    AiUInt32 ModHandle;
    AiInt32  ApiFunctionRc;      /*!< API function return code   */
} Ans1553OpenResponsePayload;
#pragma pack()


/**
 * MIL Close command payload
 */
#pragma pack(1)
typedef struct _Ans1553CloseCmdPayload
{
  AiUInt32 ModHandle;
} Ans1553CloseCmdPayload;
#pragma pack()


/**
 * MIL Close response payload
 */
#pragma pack(1)
typedef struct _Ans1553CloseResponsePayload
{
    AiInt32 ApiFunctionRc;      /*!< API function return code   */

} Ans1553CloseResponsePayload;
#pragma pack()


/**
 * MIL read memory command payload
 */
#pragma pack(1)
typedef struct _Ans1553ReadMemCmdPayload
{
    AiUInt32    ModHandle;      /*!< API Board Module Number to access  */
    AiUInt32    Memtype;        /*!< Memory type to be accessed         */
    AiUInt32    Offset;         /*!< Byte offset address relative to the start of a specific onboard memory described in parameter memtype */
    AiUInt32    Width;          /*!< Width of data entries to read (Used for swapping reasons) */
    AiUInt32    NumElements;    /*!< Amount of data units to read       */

} Ans1553ReadMemCmdPayload;
#pragma pack()


/**
 * MIL read memory response payload
 */
#pragma pack(1)
typedef struct _Ans1553ReadMemResponsePayload
{
    AiInt32     ApiFunctionRc;      /*!< API function return code                   */
    AiUInt32    BytesRead;          /*!< Amount of bytes actually read              */
    AiUInt8     Data[1];            /*!< place-holder for data to read (ByteArray)  */

} Ans1553ReadMemResponsePayload;
#pragma pack()

/**
 * MIL write memory command payload
 */
#pragma pack(1)
typedef struct _Ans1553WriteMemCmdPayload
{
    AiUInt32    ModHandle;      /*!< API Board Module Number to access  */
    AiUInt32    Memtype;        /*!< Memory type to be accessed         */
    AiUInt32    Offset;         /*!< Byte offset address relative to the start of a specific onboard memory described in parameter memtype */
    AiUInt32    Width;          /*!< Width of data entries to write (Used for swapping reasons) */
    AiUInt32    NumElements;    /*!< Amount of data units to write      */
    AiUInt8     Data[1];        /*!< place-holder for data to write (ByteArray)  */

} Ans1553WriteMemCmdPayload;
#pragma pack()


/**
 * MIL write memory response payload
 */
#pragma pack(1)
typedef struct _Ans1553WriteMemResponsePayload
{
    AiInt32     ApiFunctionRc;      /*!< API function return code                  */
    AiUInt32    BytesWritten;       /*!< Amount of bytes actually written          */

} Ans1553WriteMemResponsePayload;
#pragma pack()


/**
 * MIL IO command payload
 */
#pragma pack(1)
typedef struct _Ans1553IoCmdPayload
{
    AiUInt32    ModHandle;               /*!< API Board Module Number to access     */
    AiUInt32    Biu;                     /*!< destination BIU                       */
    AiUInt32    Cmd;                     /*!< command code                          */
    AiInt32     ExpectAck;               /*!< expected acknowledge value            */
    /* Parameters used to command the target */
    AiInt32     OutByteSize;             /*!< amount of bytes to output             */
    AiInt32     OutWordSize;             /*!< amount of words to output             */
    AiUInt8     Data[1];                 /*!< ByteArray for Commands                */
} Ans1553IoCmdPayload;
#pragma pack()


/**
 * MIL IO response payload
 */
#pragma pack(1)
typedef struct _Ans1553IoResponsePayload
{
    AiInt32     ApiFunctionRc;              /*!< API function return code      */
    /* Parameters used to prepare the response */
    AiInt32     InByteSize;                 /*!< Number of bytes in ACK array  */
    AiInt32     InWordSize;                 /*!< Number of words in ACK array  */
    AiInt32     InLWordSize;                /*!< Number of Lwords in ACK array */
    AiUInt8     Data[1];                    /*!< ByteArray for ACK             */
} Ans1553IoResponsePayload;
#pragma pack()



/**
 * MIL GetDriverVersion command payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDriverVersionCmdPayload
{
    AiUInt32    ModHandle;

} Ans1553GetDriverVersionCmdPayload;
#pragma pack()



/**
 * MIL GetDriverVersion response payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDriverVersionResponsePayload
{
    AiInt32     ApiFunctionRc;  /*!< API function return code */
    AiUInt32    SysDrvVer;
    AiUInt32    SysDrvBuild;

} Ans1553GetDriverVersionResponsePayload;
#pragma pack()


/**
 * MIL ReadVersion command payload
 */
#pragma pack(1)
typedef struct _Ans1553ReadVersionCmdPayload
{
    AiUInt32    ModHandle;
    AiUInt32    VersionId;
} Ans1553ReadVersionCmdPayload;
#pragma pack()



/**
 * MIL ReadVersion response payload
 */
#pragma pack(1)
typedef struct _Ans1553ReadVersionResponsePayload
{
    AiInt32     ApiFunctionRc;  /*!< API function return code */
    TY_VER_INFO Version;
} Ans1553ReadVersionResponsePayload;
#pragma pack()


/**
 * MIL structure based IO command payload
 */
#pragma pack(1)
typedef struct _Ans1553IoCommandStructCmdPayload
{
    AiUInt32    ModHandle;
    AiUInt32    Size;
    AiUInt32    ExpectedAckSize;
    AiUInt8     CommandData[1];

} Ans1553IoCommandStructCmdPayload;
#pragma pack()


/**
 * MIL structure based IO response payload
 */
#pragma pack(1)
typedef struct _Ans1553IoCommandStructResponsePayload
{
    AiInt32     ApiFunctionRc;
    AiUInt32    Size;
    AiUInt8     ResponseData[1];

} Ans1553IoCommandStructResponsePayload;
#pragma pack()





/**
 * MIL GetDriverVersion command payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDriverInfoCmdPayload
{
    AiUInt32    ModHandle;

} Ans1553GetDriverInfoCmdPayload;
#pragma pack()



/**
 * MIL GetDriverVersion response payload
 */
#pragma pack(1)
typedef struct _Ans1553GetDriverInfoResponsePayload
{
  AiInt32    ApiFunctionRc;  /*!< API function return code */
  AiUInt8    DeviceGroup;
  AiUInt8    ReservedB2;
  AiUInt8    ReservedB3;
  AiUInt8    ReservedB4;
  AiUInt16   ReservedW1;
  AiUInt16   ReservedW2;
  AiUInt32   DriverFlags;
  AiUInt32   SN;
  AiUInt32   BoardConfig;
  AiUInt32   BoardType;
  AiUInt32   OpenConnections;
  AiUInt32   ReservedLW6;
  AiUInt32   ReservedLW7;
  AiUInt32   ReservedLW8;

} Ans1553GetDriverInfoResponsePayload;
#pragma pack()

/*! This is the command payload for
 * RegisterCallbackID and UnregisterCallbackID board commands
 */
#pragma pack(1)
typedef struct _Ans1553CallbackPayload
{
	AiUInt32 ModHandle;
    AiUInt32 biu;
    AiUInt32 type;

}Ans1553CallbackPayload;
#pragma pack()


/*! This is the command payload for
 * RegisterCallbackID and UnregisterCallbackID board commands
 */
#pragma pack(1)
typedef struct _Ans1553CallbackResponsePayload
{
    AiReturn api_return;

}Ans1553CallbackResponsePayload;
#pragma pack()


/*! This is a 1553 event that will be
 * forwarded by server when any biu event is detected and
 * a callback has been registered.
 */
#pragma pack(1)
typedef struct _Ans1553Event
{
    AiUInt32 ModHandle;
    AiUInt32 biu;
    AiUInt32 type;
    AiUInt32 event_data[4];

}Ans1553Event;
#pragma pack()



#endif /* ANS1553_BOARDCOMMANDS_H_ */
