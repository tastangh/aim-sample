/* SPDX-FileCopyrightText: 2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

#ifdef _AIM_WINDOWS
#include "Windows.h"
#endif

#include <string.h>
#include "Aim485Ah.h"
#include "Wrapper.h"
#include "WrapList.h"
//#include "SmartDef.h"
#include "AiOs.h"
#include "Aim485AhErrors.h"

/****************************************************/
/* DLL functions                                    */
/****************************************************/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapConnectToServer(AiChar *ac_SrvName)
{
  AiInt32  w_ServerBoards = 0;
  AiReturn  retVal      = 0;

  retVal = Ah485_LibInit(&w_ServerBoards);

  return w_ServerBoards;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapDisconnectFromServer(AiChar *ac_SrvName)
{
  AiReturn retVal = 0;
  return retVal;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapOpenModule(AiUInt32 ul_ModuleId, AiUInt32 ul_StreamId,
                                                AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle)
{
    AiReturn retVal = AH485_ERR;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo = NULL;
    AiUInt32 ul_TempHandle;


    Ah485_BoardOpen(ul_ModuleId + 1, &ul_TempHandle);
    if (ul_TempHandle != 0)
    {
        *pul_ModuleHandle = ul_TempHandle;
        if( AiFalse == uc_IsAlreadyOpened( *pul_ModuleHandle, &px_DLLModuleInfo ) )
            retVal = uc_AddDLLModuleInfoListEntry( *pul_ModuleHandle );

        retVal = uc_GetDLLModuleInfoListEntry( *pul_ModuleHandle, &px_DLLModuleInfo );
        if( AH485_OK == retVal )
            retVal = SlWrapReadModuleInfo( *pul_ModuleHandle, &px_DLLModuleInfo->x_ModuleInfo );
    }

    return retVal;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapCloseModule(AiUInt32 ul_ModuleHandle)
{
    AiReturn retVal = AH485_OK;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

    if (AiTrue == uc_IsAlreadyOpened(ul_ModuleHandle, &px_DLLModuleInfo))
        retVal = uc_RemoveDLLModuleInfoListEntry(ul_ModuleHandle);
    if (AH485_OK == retVal)
        retVal = Ah485_BoardClose((AiUInt8)ul_ModuleHandle);
    return retVal;
}


static AiUInt32 get_legacy_info(AiUInt32 module_handle, TY_MODULE_INFO *info)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}


static AiUInt32 get_info(AiUInt32 module_handle, TY_MODULE_INFO *info)
{
    AiReturn ret = AH485_OK;
    TY_AH485_BOARD_INFO board_info;
    TY_VER_INFO version;

    Ah485_BoardNameGet((module_handle), info->ac_BoardName);

    ret = Ah485_BoardInfoGet(module_handle, &board_info);
    if (ret == AH485_OK)
    {
        info->ul_SerialNumber = board_info.board_sn;
        info->nvr.boardConfig = board_info.board_cfg;
        info->nvr.boardType = board_info.board_type;
    }

    ret = Ah485_VersionGet(module_handle, AI_FIRMWARE_VER_BIU1, &version);
    if (ret == AH485_OK)
    {
        memcpy(&info->version.firmware[0], &version, sizeof(version));
    }

    memset(&version, 0, sizeof(version));
    ret = Ah485_VersionGet(module_handle, AI_FIRMWARE_VER_BIU2, &version);
    if (ret == AH485_OK)
    {
        memcpy(&info->version.firmware[1], &version, sizeof(version));
    }

    memset(&version, 0, sizeof(version));
    ret = Ah485_VersionGet((AiUInt8)module_handle, AI_MAIN_LCA_VER, &version);
    if (ret == AH485_OK)
    {
        memcpy(&info->version.soc, &version, sizeof(version));
    }

    return AH485_OK;
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadModuleInfo(AiUInt32 ul_ModuleHandle, TY_MODULE_INFO *px_ModuleInfo)
{
    AiReturn retVal = AH485_ERR;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo = NULL;

    if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    {
        return AH485_ERR;
    }

    px_ModuleInfo->ul_ModuleHandle = ul_ModuleHandle;

    return get_info(ul_ModuleHandle, px_ModuleInfo);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapLoadSREC(AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Cnt,
                                               AiUInt32 ul_Offset, AiUInt8 *auc_SRec, AiUInt8 *puc_St,
                                               AiUInt32 *pul_FSize )
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapProgFlash(AiUInt32 ul_ModuleHandle, AiUInt32 ul_SrcAddr,
                                                AiUInt32 ul_Sector, AiUInt32 ul_Size, AiUInt8 *puc_St )
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}

#ifdef UPDATE_PACKAGE
static AiBoolean INLINE(is_update_in_progress(TY_API429_E_UPDATE_STATUS update_status))
{
    return AI485_FUNCTION_NOT_IMPLEMENTED;
}


static AiReturn INLINE(is_update_error(TY_API429_E_UPDATE_STATUS update_status))
{
    return AI485_FUNCTION_NOT_IMPLEMENTED;
}
#endif
/*!
Downloads the given file and updates it by using the on-board esmart.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]  file              The esmart upate package

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error

@remarks n/a
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStart( AiUInt32 module, const char* file)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStatus( AiUInt32 module, AiBoolean * in_progress, int * components, int * total)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageFinish( AiUInt32 module, const char* file)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadMemLWord(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 *pul_Value)
{
  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, NULL ) )
    return AH485_ERR;

  return Ah485_BoardMemRead(ul_ModuleHandle, uc_MemType, ul_Offset, sizeof(AiUInt32), pul_Value);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteMemLWord( AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 ul_Value )
{
  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, NULL ) )
      return AH485_ERR;

  return Ah485_BoardMemWrite(ul_ModuleHandle, uc_MemType, ul_Offset, sizeof(AiUInt32), &ul_Value);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                          AiUInt32 ul_Size, void *p_Data )
{
  AiUInt32 ul_BytesRead;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, NULL ) )
      return AH485_ERR;

  return Ah485_BoardMemBlockRead((AiUInt8)ul_ModuleHandle, uc_MemType, ul_Offset, uc_Width, p_Data, ul_Size, &ul_BytesRead);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                           AiUInt32 ul_Size, void *p_Data )
{
  AiUInt32 ul_BytesWritten;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, NULL ) )
      return AH485_ERR;

  return Ah485_BoardMemBlockWrite(ul_ModuleHandle, uc_MemType, ul_Offset, uc_Width, p_Data, ul_Size, &ul_BytesWritten);
}

_API_EXTERN AiReturn _API_DLL_FUNC ul_AiUtil( AiUInt32 ul_ModuleHandle, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt,
                                              AiUInt32 aul_InLongs[64], AiUInt32 aul_OutLongs[64] )
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSwitchBoard( AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemAlloc(AiUInt32 ul_ModuleHandle, AiUInt32 ulSize, AiUInt32 * pulOffset)
{
    return Ah485_BoardMemAlloc((AiUInt8)ul_ModuleHandle, AI_MEMTYPE_GLOBAL, 0x800000, 0x534D5254 /*'SMRT'*/, pulOffset);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemFree(AiUInt32 ul_ModuleHandle, AiUInt32 ulOffset)
{
    return Ah485_BoardMemFree(ul_ModuleHandle, AI_MEMTYPE_GLOBAL, ulOffset);
}

_API_EXTERN void _API_DLL_FUNC SlWrapReset(AiUInt32 ul_ModuleHandle)
{
    /*Not neccessary or not implemented*/
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapRebootBoard(AiUInt32 ul_ModuleHandle)
{
    return AH485_FUNCTION_NOT_IMPLEMENTED;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSetNovRamValue(AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 ulNovRamValue)
{
    return Ah485_NovRamValueSet(ulModuleHandle, ulNovRamAddr, ulNovRamValue);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapGetNovRamValue(AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 *pulNovRamValue)
{
    return Ah485_NovRamValueGet(ulModuleHandle, ulNovRamAddr, pulNovRamValue);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapStartNovRam(AiUInt32 ulModuleHandle)
{
    return Ah485_NovRamStart(ulModuleHandle);
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapFinalizeNovRam(AiUInt32 ulModuleHandle)
{
    return Ah485_NovRamFinalize(ulModuleHandle);
}
