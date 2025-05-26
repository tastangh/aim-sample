/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Net_io.h
 *
 * NET io functions for libraries with ANS-Client implementations
 * 
 */

//****************************************************
// general includes
//****************************************************
#include <stdio.h>
#include <stdlib.h>
#ifdef _AIM_WINDOWS        // 32-bit Windows specific includes
    #include <WinSock2.h>
     #include <windows.h>
#endif //_AIM_WINDOWS

#ifdef _AIM_VME       // generic driver, specific for VME Platform
#endif //_AIM_VME

#ifdef _AIM_VXI       // AXI 1553 VXI p&p
     #include <windows.h>
     #include <visa.h>
#endif //_AIM_VXI

//****************************************************
// my own include includes
//****************************************************
#include "Ai_cdef.h"
#include "Ai_def.h"

#ifdef _API3910
  #include "Ai3910i_def.h"
  #include "Ai3910i_fnc.h"
  #include "Ai1553_Internal_def.h"
  #include "Ai3910_Internal_def.h"
#else
  #include "Ai1553i_def.h"
  #include "Ai1553i_fnc.h"
  #include "Ai1553_Internal_def.h"
#endif

#include "AiMilCom.h"

/*************************************************************
   defines
**************************************************************/
#define NET_OK                    0
#define NET_ERR                   1
#define NET_ONLINE                2

#define MAX_LENGTH_NS_ENTRY (MAX_COMPUTERNAME_LENGTH + 64)

#define LOGFILE "ANS.Log"

/*************************************************************
   prototypes
**************************************************************/

AiReturn _MilNetOpen( TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle);
AiReturn _MilNetGetModuleHandle(TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle);
AiInt16 _MilNetClose( AiUInt32 bModule, AiBoolean force);
AiInt16 _MilNetIo(AiUInt32 moduleHandle, AiUInt8 biu, 
               AiUInt32 commandCode, AiInt16 expectedAckCode, 
               AiUInt8* byteOutData, AiInt16 byteOutDataUnits, 
               AiUInt16* wordOutData, AiInt16 wordOutDataUnits, 
               AiUInt8* byteInData, AiInt16* byteInDataUnits, 
               AiUInt16* wordInData, AiInt16* wordInDataUnits,
               AiUInt32* lwordInData, AiInt16* lwordInDataUnits);
AiInt16 _MilNetIoStruct(AiUInt32 moduleHandle, TY_MIL_COM* pTargetCommand, TY_MIL_COM_ACK* pReturnData);
AiInt16 _MilNetGetDriverInfo( AiUInt32 ulModule, TY_API_DRIVER_INFO *px_DriverInfo );
AiInt16 _MilNetReadVersion( AiUInt32 moduleHandle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion );
AiInt16 _MilNetGetSysDrvVersion( AiUInt32 bModule, AiUInt32 *pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild);

AiInt16 _NetCmdScopeSetup( AiUInt32 bModule, TY_API_SCOPE_SETUP *px_ScopeSetup);
AiInt16 _NetCmdScopeStart( AiUInt32 bModule);
AiInt16 _NetCmdScopeStop( AiUInt32 bModule);
AiInt16 _NetCmdScopeStatus( AiUInt32 bModule, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft);
AiInt16 _NetProvideScopeBuffers(AiUInt32 bModule, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER *ax_ScopeBuffers[]);

AiInt16 _MilNetReadMemData( AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                                                              void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead );
AiInt16 _MilNetWriteMemData( AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                                                               void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten );


AiInt16 _MilNetDataQueueOpen(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 * queue_size);
AiInt16 _MilNetDataQueueClose(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 id );
AiInt16 _MilNetDataQueueControl(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 mode);
AiInt16 _MilNetDataQueueRead(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_DATA_QUEUE_READ *px_QueueData, TY_API_DATA_QUEUE_STATUS * info);


AiInt16 _MilNetGetDeviceConfig( AiUInt32 ulModHandle, TY_API_DEVICE_CONFIG * pxConfig );
AiInt16 _MilNetSetDeviceConfig(AiUInt32 ulModHandle, TY_API_DEVICE_CONFIG * pxConfig);



/*! \brief Registers a channel event callback for a remote device
*
* @param ulModHandle the handle to the device the channel belongs to
* @param uc_Biu the biu to register event callback for
* @param uc_Type the interrupt type to register
* @param pf_IntFunc the function to call when event from the channel is received
* @return API_OK on success, an error code otherwise
*/
AiReturn _MilNetCallbackRegister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc);


/*! \brief Unregisters a channel event callback for a remote device
*
* @param ulModHandle the handle to the device the channel belongs to
* @param uc_Biu the biu to remove the event
* @param uc_Type the interrupt type to unregister
* @return API_OK on success, an error code otherwise
*/
AiReturn _MilNetCallbackUnregister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type);

AiReturn Net1553DeviceInit(AiUInt32 ulModHandle);

void Net1553DeviceFree(AiUInt32 ulModHandle);

