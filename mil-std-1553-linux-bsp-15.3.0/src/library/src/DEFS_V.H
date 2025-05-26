/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file DEFS_V.H
 *
 * External References and Prototypes.
 * 
 */

#ifndef DEFS_V_H
#define DEFS_V_H
#ifdef _AIM_VXI
  #include <stdio.h>

  #include <vpptype.h>
  #include <visa.h>
#endif 

#ifdef _AIM_LVRT
#include "Acxx_io.h"
#endif 


#ifdef _AIM_RTX64
#include "AimRtx64_hw.h"
#endif 

#ifdef _AIM_VME
    #include "stdlib.h"
    #include "Ai_cdef.h"
    #include "AiMilVxWorksIo.h"
#endif

#ifdef _AIM_LINUX
  #include <pthread.h>
#endif


#if defined(_AIM_WINDOWS) || defined(_AIM_RTX64)
#pragma warning( disable : 4996 ) /* disable sprintf warning */
#endif


#include "Ai1553i_def.h"
#include "ApiLsScope.h"
#include "ApiDebug.h"

#ifdef _WIN32
  #ifndef __WIN32__
    #define __WIN32__
  #endif 
#endif 

#ifdef __WIN32__
  #include <windows.h>
  #include <WinBase.h>
#endif

#define DQUEUE_CLOSED 0
#define DQUEUE_OPENED 1


/***************************
 * misc global vars
 ***************************/
extern AiUInt8  *b_p;
extern AiUInt16 *w_p;
extern AiUInt32 *lw_p;
extern AiInt16  tg_os;

#if defined(_AIM_WINDOWS) || defined(_AIM_RTX64) || defined(_AIM_LVRT)
extern FILE    *stream;
extern SYSTEMTIME SystemTime;
#elif defined _AIM_LINUX
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
extern FILE* stream;
#endif

extern char     szLogFileName[256+12];

#include "AiMilCom.h"
#include "AiMilErrors.h"

#include "ApiLsScopeInternal.h"


/****************************************************
 * 32-Bit Windows specific
 ****************************************************/
#if defined(_AIM_WINDOWS)
  typedef struct
  {
    AiUInt32 ul_Api;
    AiUInt32 ul_Init;
    AiUInt32 ul_ExtInit;
    AiUInt32 ul_ExtIo;
    AiUInt32 ul_IntRequest;
    AiUInt32 ul_IntFlush;
    AiUInt32 ul_SetDebug;
    AiUInt32 ul_ReadVersion;
    AiUInt32 ul_ReadBiuData;
    AiUInt32 ul_WriteBiuData;
    AiUInt32 ul_MapVmeBoard;
    AiUInt32 ul_ReadVmeData;
    AiUInt32 ul_WriteVmeData;
    AiUInt32 ul_ReadMemData;
    AiUInt32 ul_WriteMemData;
    AiUInt32 ul_StartPerfTimer;
    AiUInt32 ul_ProgramTcpFlash;
    AiUInt32 ul_JTagExecute;
    AiUInt32 ul_ReadMemDataDma;
    AiUInt32 ul_IntDisable;
    AiUInt32 ul_IntEnable;
    AiUInt32 ul_UsbMemoryRead;
    AiUInt32 ul_UsbMemoryWrite;
    AiUInt32 ul_ReadInfo;
    AiUInt32 ul_ProvideScopeBuf;
    AiUInt32 ul_ReleaseScopeBuf;
    AiUInt32 ul_ProvideScopeBufWait;
	AiUInt32 ul_ReadDrvVersion;
  } TY_IOCTL;

  extern TY_IOCTL x_IoCtl;
#endif 


extern AiUInt16         Aiapi_max_boards;

#if defined(_AIM_WINDOWS) || defined(_AIM_RTX64) || defined(_AIM_VME) || defined(_AIM_LINUX)
  #define DQUEUE_PATH_NAME     "dqueue"

  typedef struct ty_srv
  {
    AiChar   ac_SrvName[ 256 ];
    AiUInt8  bUseNetIO;
    #if defined(_AIM_VME) || defined(_AIM_LINUX)        
      AiHandle h_ServerHandle;
    #else 
      handle_t h_ServerHandle;
    #endif 
  } TY_SRV;



  extern AiUInt16         api_max_ext_boards;
  extern AiUInt8          b_RegisteredServersCount;
  extern char             dbgBuf[ 2000 ];

  extern AiUInt32         ul_AttachedApps;

#endif /* #if defined(_AIM_WINDOWS) || defined(_AIM_RTX64) || defined(_AIM_VME) || defined(_AIM_LINUX) */

#if defined(_AIM_VXI) /* 32-Bit Windows VXI p&p */
  typedef struct
  {
    AiChar   ac_SrvName[ 256 ];
    AiUInt8  bUseNetIO;
#ifdef _AIM_WINDOWS
    handle_t h_ServerHandle;
#endif	
  } TY_SRV;

  typedef struct ram_base
  {
    AiUInt32 TrgCtl;
    AiUInt32 CmdArray;
    AiUInt32 AckArray;
    AiUInt32 BiuIll1;
    AiUInt32 BiuIll2;
    AiUInt32 LSRecArea;
    AiUInt32 LSRepArea;
    AiUInt32 HSRecArea;
    AiUInt32 HSRepArea;
    AiUInt32 universe;
    AiUInt32 galileo;
    AiUInt32 globalRam;
  } TY_RAM_BASE;

  extern AiUInt8          b_RegisteredServersCount;
  extern TY_RAM_BASE      AiramBase_p[ MAX_API_MODULE ];
  extern AiUInt16         Aiapi_max_ext_boards;
  extern char             AidbgBuf[ 2000 ];
  extern AiUInt8          AiBoard[ MAX_API_MODULE ];
  extern TyIntFuncElement Aiax_IntFuncTable[ MAX_API_MODULE][ MAX_BIU ];
  extern AiUInt8          auc_HsReplayFlag[ MAX_API_MODULE ];
  extern AiUInt8          auc_ReplayFlag[ MAX_API_MODULE ][ MAX_BIU ];

  extern AiUInt32         ul_AttachedApps;
#endif /* _AIM_VXI   32-Bit Windows VXI p&p */


typedef struct ty_stream_info {
    AiUInt32 ul_PbiBase;
    AiUInt32 ul_HighSpeed;

} TY_STREAM_INFO;

typedef struct data_queue_direct_setup {

    AiUInt8           ucTriggered;
    AiUInt8           ucFirstRead;
    AiUInt32          ulId;
    AiUInt32          ulBmBaseOffset;
    AiUInt32          ulBmEndOffset;
    AiUInt32          ulBmSize;
    AiUInt32          ulLoadMax;
    AiUInt32          ulLoadTotal;
    AiUInt32          ulLoadCount;
    AiUInt64          ulTotalBytesTransfered;
} TY_API_DATA_QUEUE_DIRECT_SETUP;


#ifdef _AIM_LVRT
  typedef struct Ai1553_Device
  {
    AiUInt16 us_SharedMemSpace;
    AiUInt16 us_GlobalMemSpace;
    AiUInt16 us_IoMemSpace;
    AiUInt32 ul_SharedMemOffset;
    AiUInt8 bIsPMCDevice;
    AiUInt16 us_GlobExtension;
    AiUInt16 us_ModelCode;
  }TY_AI1553_DEVICE;

#endif

typedef struct device_info {
    char *DevicePath;
    size_t DevicePathLen;

#if defined(_AIM_VXI) 
    AiSession h_DrvVXI;
    AiSession h_Instru;
#endif
    AiUInt32 ul_ModuleNo;

    /* Number of ApiOpen calls, that were issued on the board */
    TY_API_INI_INFO       x_IniPar;
    TY_API_DEVICE_CONFIG  x_Config;
    TY_API_VERSION_INFO   ax_VersionInfo;
    TY_API_BOARD_INFO     x_BoardInfo;
    TY_API_GET_MEM_INFO   x_MemInfo;
    TY_API_DATA_QUEUE_DIRECT_SETUP * pxDataQueueDirectSetup[8];

    AiChar                pszBoardName[MAX_BOARD_NAME_LENGTH];

    AiUInt8 uc_DeviceGroup;
    TY_STREAM_INFO x_Stream[MAX_API_STREAM];

    AiHandle _hDrv;
    AiHandle _hExtDrv;
    AiHandle _hRecMutex;
    AiHandle _hMutex;
    struct ai_mutex* _hInitMutex;
    /***************************
     * flags and state vars
     ***************************/
    AiInt16             tg_emul;
    AiInt16             tcom_status;
    AiInt16             err_msg;
    AiInt16             init_flag;
    TY_API_BC_FW_RSRC   api_bc_glob_fw[MAX_API_STREAM];
    AiUInt8             uc_HsSA;

    /* Initialisation updated for modified MAX_API_MODULE */
    AiUInt32    ulOpenCount;

    TY_API_QUEUE_TABLE  ax_QueueTable[ MAX_DATA_QUEUE_ID + 1];


    AiUInt8 auc_ReplayFlag[ MAX_API_STREAM ];
    /* the following flag is used in WriteRepArea() as indicator, whether replay buffer 1 or 2 is active */

    AiHandle _hInterruptThread;
#ifndef _AIM_RTX64
    TyIntFuncElement ax_IntFuncTable[ MAX_API_STREAM ];
#endif

#ifdef _AIM_WINDOWS
    OVERLAPPED       Event;
#endif

#ifdef _AIM_VME
    struct avi_board_vwl_ty _avi1553BoardVwl;
#endif

#ifdef _AIM_LINUX
    pthread_mutex_t InterruptHandlerLock;
#endif

    TY_API_SCOPE_SETUP * px_ScopeSetup;
    TY_SCOPE_BUFFER_LIST_ELEMENT * px_ScopeBufferLHead;
    TY_SCOPE_BUFFER_LIST_ELEMENT * px_ScopeBufferLTail;
    AiUInt32 ulNumBuffersLeft;
    
    CRITICAL_SECTION ScopeBuffListCriticalSect;
    int             scopeThreadState;             /*!< current state of the scope thread */

#ifdef _AIM_WINDOWS
    AiHandle hScopeThread;                            /*!< Handle to the scope thread */
    CRITICAL_SECTION* p_ScopeThreadStateLock;         /*!< Used to synchronize all state changes of the scope thread */
    AiHandle hScopeThreadWakeEvent;                   /*!< Event the scope thread is waiting for while suspended. Used to wake it up */
    AiHandle hScopeThreadSuspendEvent;                /*!< Event the scope thread uses to signal that it has suspended */
#endif

#ifdef _AIM_LINUX
    pthread_t*      pScopeThread;                 /*!< Handle to the scope thread */
    AiUInt32        ulModHandle;
    pthread_mutex_t ScopeThreadStateLock;         /*!< Used to synchronize all state changes of the scope thread */
    pthread_cond_t  hScopeThreadWakeEvent;        /*!< Event the scope thread is waiting for while suspended. Used to wake it up */
    pthread_cond_t  hScopeThreadSuspendEvent;     /*!< Event the scope thread uses to signal that it has suspended */
#endif


#ifdef _AIM_RTX64
    AIM_RTX64_DEVICE_DATA x_HarwareInfo;
#endif 

#ifdef _AIM_LVRT
    TY_AI1553_DEVICE x_HarwareInfo;
    AiUInt8 *p_SharedMemory;
    TY_AEC_BASIC_INFO x_AecBase;

    ViSession MemAccessHandle;  /* For DMA Access */

    AiUInt32          ul_DeviceGroup;

#endif


    AiBoolean bOpenExUsed;  /*!< Signifies if device was opened with ApiOpenEx. TODO: remove this with next incompatible API change */

    struct device_info_os* os_info;  /* Operating Sytem specific properties can be stored here */
    struct mil_net_layer_properties* net_layer; /*!< These are properties specific for remote devices that are attached via ANS */

    TY_HW_INFO HardwareInfo;

} TY_DEVICE_INFO;




typedef struct device_info_List {
    struct device_info_List * px_Next;
    struct device_info_List * px_Prev;
    TY_DEVICE_INFO            x_DeviceInfo;
} TY_DEVICE_INFO_LIST;




/***************************
 * global variables
 ***************************/

/* List of Hardware Devices found  */

extern TY_DEVICE_INFO_LIST * px_MilDeviceInfoListStart;
extern TY_DEVICE_INFO_LIST * px_MilDeviceInfoListEnd;

extern AiInt16          open_errno;

extern TY_SRV           Srv[MAX_API_SERVER_PER_SLOT];










#ifdef _AIM_LINUX
  #include "string.h"
  #define MB_OK 1
  typedef unsigned long ULONG;
#endif 


/* Prototypes */
extern AiInt16 uw_ApiGetBiuFromStream( AiUInt32 ul_Module, AiUInt32 ul_OldBiu, AiUInt32 *pul_Biu);

extern AiInt16 ApiErr(AiUInt32 bModule, AiInt16 tg_status, AiUInt32 cmd, AiUInt16 ack, AiInt16 *status);
extern void ApcTest(AiUInt32 bModule, AiUInt8 bEna);
extern AiInt16 ApiEmul(AiUInt8 biu, AiUInt32 cmd, AiInt16 expect, AiInt16*size_back, AiUInt8 *tcom_info, AiUInt8 *tack_info);
AiUInt8 _FindServerIdByHandle(AiHandle h_ServerHandle, AiUInt8 *bServerID);

AiBoolean IsTswVersionGreaterOrEqual(TY_DEVICE_INFO* pDevice, AiUInt32 ulMajor, AiUInt32 ulMinor);
AiBoolean IsDriverVersionGreaterOrEqual(TY_DEVICE_INFO* pDevice, AiUInt32 ulMajor, AiUInt32 ulMinor);

#endif
