/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
Sim_Wrap.cpp
Contains common interface functions for the SmartWrp library.
*/


#include <stdlib.h>
#include <string.h>
#include "AiOs.h"
#include "Wrapper.h"
#include "WrapList.h"
#include "SmartDef.h"





#define TARGET_SW_MAJOR_VERSION  20
#define BOARD_CONFIG 0x91
#define BOARD_TYPE 0x3011
#define SERIAL_NUMBER 100



/****************************************************/
/* DLL functions                                    */
/****************************************************/

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapConnectToServer(AiChar *ac_SrvName)
{
  return 1; 
}
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapDisconnectFromServer(AiChar *ac_SrvName)
{
  return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapOpenModule(AiUInt32 ul_ModuleId, AiUInt32 ul_StreamId,
                                                AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle)
{
    return ul_ModuleId;
}
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapCloseModule(AiUInt32 ul_ModuleHandle)
{
  return 0;
}

_API_EXTERN AiUInt8 _API_DLL_FUNC uc_CanApiBoardInfo(AiUInt32 ul_ModuleHandle)
{
    return AiTrue;
}

_API_EXTERN AiReturn _API_DLL_FUNC ul_AiUtil(AiUInt32 ul_ModuleHandle, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt,
    AiUInt32 aul_InLongs[64], AiUInt32 aul_OutLongs[64])
{
    return 0;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadModuleInfo(AiUInt32 ul_ModuleHandle, TY_MODULE_INFO *px_ModuleInfo)
{
    int i;

    strcpy(px_ModuleInfo->ac_BoardName, "SimulatedBoard");
    px_ModuleInfo->nvr.boardConfig = BOARD_CONFIG;
    px_ModuleInfo->nvr.boardType = BOARD_TYPE;

    px_ModuleInfo->ul_ModuleHandle = 0;
    px_ModuleInfo->ul_NovramChanged = 0;
    px_ModuleInfo->ul_Prot = 0;
    px_ModuleInfo->ul_SerialNumber = SERIAL_NUMBER;

    for (i = 0; i < 4; i++)
    {
        memset(&px_ModuleInfo->version.firmware[i], 0, sizeof(TY_VER_INFO));
        strcpy(px_ModuleInfo->version.firmware[i].ac_Description, "Firmware");
        strcpy(px_ModuleInfo->version.firmware[i].ac_FullVersion, "Sim");
        px_ModuleInfo->version.firmware[i].ul_MajorVer = 1;

        memset(&px_ModuleInfo->version.io_lca[i], 0, sizeof(TY_VER_INFO));
        strcpy(px_ModuleInfo->version.io_lca[i].ac_Description, "io_lca");
        strcpy(px_ModuleInfo->version.io_lca[i].ac_FullVersion, "Sim");
        px_ModuleInfo->version.io_lca[i].ul_MajorVer = 1;
    }

    memset(&px_ModuleInfo->version.monitor, 0, sizeof(TY_VER_INFO));
    strcpy(px_ModuleInfo->version.monitor.ac_Description, "monitor");
    strcpy(px_ModuleInfo->version.monitor.ac_FullVersion, "Sim");
    px_ModuleInfo->version.monitor.ul_MajorVer = 1;

    memset(&px_ModuleInfo->version.soc, 0, sizeof(TY_VER_INFO));
    strcpy(px_ModuleInfo->version.soc.ac_Description, "soc");
    strcpy(px_ModuleInfo->version.soc.ac_FullVersion, "Sim");
    px_ModuleInfo->version.soc.ul_MajorVer = 1;

    memset(&px_ModuleInfo->version.target_sw, 0, sizeof(TY_VER_INFO));
    strcpy(px_ModuleInfo->version.target_sw.ac_Description, "target_sw");
    strcpy(px_ModuleInfo->version.target_sw.ac_FullVersion, "Sim");
    px_ModuleInfo->version.target_sw.ul_MajorVer = TARGET_SW_MAJOR_VERSION;

    memset(&px_ModuleInfo->version.tcp, 0, sizeof(TY_VER_INFO));
    strcpy(px_ModuleInfo->version.tcp.ac_Description, "tcp");
    strcpy(px_ModuleInfo->version.tcp.ac_FullVersion, "Sim");
    px_ModuleInfo->version.tcp.ul_MajorVer = 1;

    return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapLoadSREC(AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Cnt,
                                               AiUInt32 ul_Offset, AiUInt8 *auc_SRec, AiUInt8 *puc_St,
                                               AiUInt32 *pul_FSize )
{
    return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapProgFlash(AiUInt32 ul_ModuleHandle, AiUInt32 ul_SrcAddr,
                                                AiUInt32 ul_Sector, AiUInt32 ul_Size, AiUInt8 *puc_St )
{
  return 0;
}



/**********************************************************************************************************
***                                        E-Smart udpate                                               ***
***********************************************************************************************************/




_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStart( AiUInt32 module, const char* file)
{
	return 0;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStatus( AiUInt32 module, AiBoolean * in_progress, int * components, int * total)
{
	return 0;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageFinish( AiUInt32 module, const char* file)
{
	return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadMemLWord(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 *pul_Value)
{
  return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteMemLWord( AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 ul_Value )
{
  return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                          AiUInt32 ul_Size, void *p_Data )
{
    return 0;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                           AiUInt32 ul_Size, void *p_Data )
{
  return 0;
}

_API_EXTERN AiUInt32 _API_DLL_FUNC ul_AiExecSys( AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Con )
{
  return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSwitchBoard( AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult)
{
  return 0;
}

_API_EXTERN AiUInt8 _API_DLL_FUNC uc_CanApiReboot(AiUInt32 ul_ModuleHandle)
{
  return 0;
}








_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemAlloc(AiUInt32 ul_ModuleHandle, AiUInt32 ulSize, AiUInt32 * pulOffset)
{
    return 0;
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemFree(AiUInt32 ul_ModuleHandle, AiUInt32 ulOffset)
{
    return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapRebootBoard(AiUInt32 ul_ModuleHandle)
{
    return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSetNovRamValue(AiUInt32 ulModuleHandle,
    AiUInt32 ulNovRamAddr,
    AiUInt32 ulNovRamValue)
{
    return 0;
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapGetNovRamValue(AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 *pulNovRamValue)
{
    return 0;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapStartNovRam(AiUInt32 ul_ModuleHandle)
{
    return 0;
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapFinalizeNovRam(AiUInt32 ul_ModuleHandle)
{
    return 0;
}
