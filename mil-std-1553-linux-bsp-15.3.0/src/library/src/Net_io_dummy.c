/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Net_io_dummy.c
 *
 * NET io dummy functions for libraries that do not have an ANS-Client implementations
 * 
 */


#include "Api1553.h"

#define API_SERVER_NOT_REGISTERED 1



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiConnectToServer( AiUChar * pszNetworkAdress, AiInt16 *w_ServerBoards)
{
    return API_SERVER_NOT_REGISTERED;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDisconnectFromServer ( AiUChar * pszNetworkAdress )
{
  return API_OK;
}



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetServerInfo( AiUInt32 moduleHandle, TY_API_SERVERINFO *pServerInfo )
{
  return API_ERR_SERVER;
}


AiReturn _MilNetGetModuleHandle( TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle )
{
  return API_OK;
}


AiReturn _MilNetOpen( TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle )
{
  return API_OK;
}


AiInt16 _MilNetClose( AiUInt32 bModule, AiBoolean force)
{
  return API_ERR_SERVER;
}


AiInt16 _MilNetIo(AiUInt32 moduleHandle, AiUInt8 biu, 
               AiUInt32 commandCode, AiInt16 expectedAckCode, 
               AiUInt8* byteOutData, AiInt16 byteOutDataUnits, 
               AiUInt16* wordOutData, AiInt16 wordOutDataUnits, 
               AiUInt8* byteInData, AiInt16* byteInDataUnits, 
               AiUInt16* wordInData, AiInt16* wordInDataUnits,
               AiUInt32* lwordInData, AiInt16* lwordInDataUnits)
{
  return API_ERR_SERVER;
}


AiInt16 _MilNetReadMemData(AiUInt32 moduleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                        void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead )
{
  return API_ERR_SERVER;
}

AiInt16 _MilNetWriteMemData(AiUInt32 moduleHandle, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                         void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten )
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetGetDriverInfo( AiUInt32 moduleHandle, TY_API_DRIVER_INFO *px_DriverInfo )
{
  return API_ERR_SERVER;
}

AiInt16 _MilNetReadVersion( AiUInt32 moduleHandle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
  return API_ERR_SERVER;
}

AiInt16 _MilNetGetSysDrvVersion(AiUInt32 moduleHandle, AiUInt32 *pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild)
{
  return API_ERR_SERVER;
}


AiInt16 _NetCmdScopeStart(AiUInt32 bModule)
{
    return API_ERR_SERVER;
}


AiInt16 _NetCmdScopeStop(AiUInt32 bModule)
{
    return API_ERR_SERVER;
}


AiInt16 _NetCmdScopeSetup( AiUInt32 bModule, TY_API_SCOPE_SETUP *px_ScopeSetup)
{
    return API_ERR_SERVER;
}
AiInt16 _NetCmdScopeStatus( AiUInt32 bModule, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft)
{
    return API_ERR_SERVER;
}


AiInt16 _NetProvideScopeBuffers(AiUInt32 bModule, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER *ax_ScopeBuffers[])
{
    return API_ERR_SERVER;
}


AiInt16 _MilNetDataQueueOpen(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 * queue_size)
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetDataQueueClose(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id)
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetDataQueueControl(AiUInt32 moduleHandle, AiUInt8 uc_Biu, AiUInt32 id, AiUInt32 mode)
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetDataQueueRead(AiUInt32 moduleHandle, AiUInt8 uc_Biu, TY_API_DATA_QUEUE_READ *px_Queue, TY_API_DATA_QUEUE_STATUS * info)
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetGetDeviceConfig( AiUInt32 moduleHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetSetDeviceConfig( AiUInt32 moduleHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    return API_ERR_SERVER;
}
AiInt16 _MilNetIoStruct(AiUInt32 moduleHandle, TY_MIL_COM* pTargetCommand, TY_MIL_COM_ACK* pReturnData)
{
  return API_ERR_SERVER;
}

AiReturn _MilNetCallbackRegister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc) {
    return API_ERR_SERVER;
}

AiReturn _MilNetCallbackUnregister(AiUInt32 ulModHandle, AiUInt8 uc_Biu, AiUInt8 uc_Type) {
    return API_ERR_SERVER;
}

void Net1553DeviceFree(AiUInt32 ulModHandle) {
    return;
}

AiReturn Net1553DeviceInit(AiUInt32 ulModHandle){
    return API_ERR_SERVER;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServer(AiUInt32 port){
    return API_ERR_SERVER;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServerCallback(TY_API_DISCOVER_CALLBACK * config){
    return API_ERR_SERVER;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDiscoverServerUnregister(TY_API_DISCOVER_CALLBACK * config){
    return API_ERR_SERVER;
}
