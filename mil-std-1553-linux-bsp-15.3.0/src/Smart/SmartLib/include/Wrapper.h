/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
Wrapper.h
Contains common wrapper interface functions and defines for the SmartWrp library.

@remarks 
The functions have to be implemented for each new protocol to be supported.
*/

#include "Ai_cdef.h"
#include "Ai_def.h"


/***********************************************************/
/* Wrapper Interface Functions                             */
/*                                                         */
/* The functions have to be implemented for each new       */
/* protocol to be supported.                               */
/***********************************************************/




struct ty_wrap_versions
{
    TY_VER_INFO target_sw;
    TY_VER_INFO firmware[4];
    TY_VER_INFO io_lca[4];
    TY_VER_INFO soc;
    TY_VER_INFO tcp;
    TY_VER_INFO monitor;
};

struct ty_novram
{
  AiUInt32 boardType;
  AiUInt32 boardConfig;
};

typedef struct
{
    AiChar               ac_BoardName[80];
    AiUInt32             ul_ModuleHandle;
    AiUInt32             ul_SerialNumber;

    AiUInt32             ul_NovramChanged;  /* indicates if the novram has to be finalized */

    AiUInt32             ul_Prot;  /* required in Milbus/STANAG */

    struct ty_novram     nvr;
    struct ty_wrap_versions  version;
} TY_MODULE_INFO;


/***************************/


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapConnectToServer(     AiChar *ac_SrvName);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapDisconnectFromServer(AiChar *ac_SrvName);

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapOpenModule(      AiUInt32 ul_ModuleId, AiUInt32 ul_StreamId, AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapCloseModule(     AiUInt32 ul_ModuleHandle);

_API_EXTERN void     _API_DLL_FUNC SlWrapReset(AiUInt32 ul_ModuleHandle);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapRebootBoard(AiUInt32 ul_ModuleHandle);

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadMemLWord(    AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 *pul_Value );
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteMemLWord(   AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 ul_Value );
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadBlockMemData( AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType,
                                                          AiUInt32 ul_Offset, AiUInt8 uc_Width, AiUInt32 size, void *p_Data );
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteBlockMemData( AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType,
                                                          AiUInt32 ul_Offset, AiUInt8 uc_Width, AiUInt32 size, void *p_Data );

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadModuleInfo(  AiUInt32 ul_ModuleHandle, TY_MODULE_INFO *px_ModuleInfo);

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapLoadSREC(        AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Cnt,
                                                          AiUInt32 ul_Offset, AiUInt8 *auc_SRec, AiUInt8 *puc_St,
                                                          AiUInt32 *pul_FSize );
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapProgFlash(       AiUInt32 ul_ModuleHandle, AiUInt32 ul_SrcAddr,
                                                          AiUInt32 ul_Sector, AiUInt32 ul_Size, AiUInt8 *puc_St );

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStart( AiUInt32 ul_ModuleHandle, const char* file);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStatus( AiUInt32 ul_ModuleHandle, AiBoolean* done, int * components, int * total);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageFinish( AiUInt32 ul_ModuleHandle, const char* file);

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSwitchBoard(     AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult );

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemAlloc(   AiUInt32 ul_ModuleHandle, AiUInt32 ulSize, AiUInt32 * pulOffset);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemFree(    AiUInt32 ul_ModuleHandle, AiUInt32 ul_Offset);

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSetNovRamValue(  AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 ulNovRamValue);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapStartNovRam(     AiUInt32 ul_ModuleHandle);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapFinalizeNovRam(  AiUInt32 ul_ModuleHandle);
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapGetNovRamValue(  AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 *pulNovRamValue);
