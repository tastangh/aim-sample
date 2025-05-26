/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Aim1553_io.h
 *
 * IO layer interface
 * 
 */


/* actually defined in target software but include file cannot be included here
 * without breaking a lot of other platforms */
#define MAX_API_IR_EVENTS 256

#include "ApiLsDQue.h"
#include "DEFS_V.H"

/*TD Type definition of a interrupt information structure.		*/
/*TD Note: The API defines this structures as TY_API_INT_INFO.		*/
/*TD Type definition of a interrupt information structure. */
/*TD Note: The API defines this structures as TY_API_INT_INFO. */
typedef struct
{
   unsigned int structsize;                /* size of this structure, used to verify the integrity */
   unsigned char uc_ModuleNumber;          /* Module Number */
   unsigned int ul_EntryCnt;               /* number of entries collected */
   TY_API_INTR_LOGLIST_ENTRY* px_Loglist;  /*List of loglist entries */
} AIM_INT_INFO;

// Data queue header locking relevant defines
#define TURN_ASP 1
#define TURN_DLL 2

extern const char MIL_USB_DEV_PREFIX[];
extern const char DEVICE_DIRECTORY[];

// Data Queue Header struct definition
typedef struct
{
  AiUInt32  ul_Status;
  AiUInt32  ul_WritePtr;
  AiUInt32  ul_ReadPtr;
  AiUInt32  ul_DataStartPtr;
  AiUInt32  ul_DataEndPtr;
  AiUInt32  ul_PutWrapCnt;
  AiUInt32  ul_GetWrapCnt;
  AiUInt32  ul_LockASP;
  AiUInt32  ul_LockDll;
  AiUInt32  ul_Turn;
} TY_API_DQUEUE_HEADER;

#define	FKT_INSTINTHANDLER	"  _LinuxInstIntHandler"
#define	FKT_DELINTHANDLER	"   _LinuxDelIntHandler"


AiInt16 _ApiOsIni(void);
AiInt16 _ApiOsClose(AiUInt32 ui_ModuleHandle);
AiInt16 _ApiOsOpen(AiUInt32 ui_ModuleHandle);
AiInt16 _ApiOsInitializeLocks( AiUInt32 ulModHandle );
AiInt16 _ApiOsFreeLocks( AiUInt32 ulModHandle );
AiInt16 _ApiOsIo(AiUInt32 ui_ModuleHandle, AiUInt8* cmd, AiUInt8* ack);

AiInt16 _ApiOsDelIntHandler(AiUInt32 ul_Module, AiUInt8 biu, AiUInt8 uc_Type );
AiInt16 _ApiOsInstIntHandler(AiUInt32 ul_Module, AiUInt8 biu,AiUInt8 uc_Type,
                              TY_INT_FUNC_PTR pf_IntFunc );
AiInt16 _ApiOsGetSysDrvVersion(TY_DEVICE_INFO* p_Device, TY_VER_INFO* version);

AiInt16 _ApiOsReadMemData(AiUInt32 ui_ModuleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                              void* data_p, AiUInt32 size, AiUInt32* pul_BytesRead);

AiInt16 _ApiOsWriteMemData(AiUInt32 ui_ModuleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                               void* data_p, AiUInt32 size, AiUInt32* pul_BytesWritten);

/*! \brief This function sets device specific driver flags.

    e.g. interrupts of the device can be enabled/disabled
    \param ulModHandle handle to the device that flags are set for
    \param ulDriverFlags driver flags to set
    \return returns 0 on success */
AiInt16 _ApiOsSetDeviceConfigDriverFlags( AiUInt32 ulModHandle, AiUInt32 ulDriverFlags );

AiReturn _ApiOsInitDeviceStruct( TY_DEVICE_INFO * pDevice );
void     _ApiOsFreeDeviceStruct( TY_DEVICE_INFO* pDevice );

/* This function handles arrival of a new AIM board.
   It will create a new device structure in the global device list.
   Parameters: char* - path to the newly arrived device's device file
   Return: returns 0 on success. */
int MilDeviceArrival(char* s_DevicePath);

int MilDeviceRemoval(char* s_DevicePath);

/* This function handles the host channel commands
 * as used for the eSmart update. E.g. ASC1553.
   Return: returns 0 on success. */
AiReturn _ApiOsIoChannelCommand(TY_DEVICE_INFO* device, int comChannel, void* command, size_t size,
                                  void* response, size_t expectedResponseSize);

/* This functions are not implemented on this OS */
#define _ApiOsStartPerformanceTimer(x)     API_ERR_OS_NOT_SUPPORTED
#define _ApiOsExtIni(x,y)			       API_ERR_OS_NOT_SUPPORTED
#define _ApiOsGetDrvHandle(x)		       NULL
#define _ApiOsSyncWait(x,y)
#define _ApiOsSyncRelease(x,y)
#define _ApiOsExtIo(x,y,z)                 API_ERR_OS_NOT_SUPPORTED
#define _ApiOsSetSysDbgLevel(x,y)          API_ERR_OS_NOT_SUPPORTED
#define _ApiOsStructureEndianChange(x,y,z)

