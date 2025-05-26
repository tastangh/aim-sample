/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiGen.c
 *
 * API Command module of the Application Interface
 * 
 */




//****************************************************
// platform specific specific includes
//****************************************************
#include "net_io.h"
#include "AiOs.h"

//****************************************************
// general includes
//****************************************************
#include <stdio.h>
#include <string.h> //for strcpy

//****************************************************
// my own includes
//****************************************************
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Ai_mutex.h"

#if defined(_AIM_VME)  || defined(_AIM_RTX64) || defined(_AIM_LINUX) || defined(_AIM_VXI)
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
#else
  #include "Ai3910i_def.h"
  #include "Ai3910i_fnc.h"
  #include "Ai1553_Internal_def.h"
  #include "Ai3910_Internal_def.h"
#endif

#include "AiGenIo.h"
#include "BWTS_API.H"
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "stdlib.h" /* for malloc and free */
#include "AiMilCom.h"
#include "hw/ApiHw.h"

//****************************************************
// platform specific specific includes
//****************************************************
#ifdef _AIM_WINDOWS
#include "AiMPW_io.h"
#endif //_AIM_WINDOWS

#ifdef _AIM_RTX64
#include "AimRtx64_io.h"
#endif

#ifdef _AIM_VME
#include "Aim1553_io.h"
#endif //_AIM_VME


#ifdef _AIM_VXI
#include <visa.h>
#include "visa_io.h"
#endif //_AIM_VXI

#ifdef _AIM_LINUX
#include "Aim1553_io.h"
#endif //Linux



/* This function is used to define OS specific members of the device info
* structure.
* Parameters:  TY_DEVICE_INFO* - pointer to the device info struct to initialize
*/
void _ApiInitializeDeviceOsSpecific( TY_DEVICE_INFO * p_DeviceInfo );
void _ApiSetDeviceInfoConfig(        TY_DEVICE_INFO * p_DeviceInfo );


/* This function sets OS specific values of device configuration
e.g. DMA capability or number of interrupt requests */
void  _ApiSetDeviceInfoConfigOsSpecific(TY_DEVICE_INFO* p_DeviceInfo);

AiChar buf[2000];


//**************************************************************************
//
//   Module : A1553_API                 Submodule : ApiGetLibraryInfo
//
//   Author : Patrick Giesel            Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 25.09.06     Update : 25.09.06
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function is used to return general library information and can be called
//    every time.
//
//**************************************************************************

AiInt16 ApiGetLibraryInfo_(TY_API_LIB_INFO *px_LibInfo)
{
    px_LibInfo->ul_AttachedApps = ul_AttachedApps;
    
    return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetLibraryInfo(TY_API_LIB_INFO *px_LibInfo)
{
	AiInt16 retval;

	retval = ApiGetLibraryInfo_(px_LibInfo);

	TRACE_BEGIN
	TRACE("    TY_API_LIB_INFO x_LibInfo;\n");
	TRACE_FCTA("ApiGetLibraryInfo", API_OK); 
	TRACE_RPARE("&x_LibInfo");
	TRACE_FCTE;
	TRACE_END

	return retval;
}



//**************************************************************************
//
//   Module : A1553_API                 Submodule : ApiGetDriverInfo
//
//   Author : Martin Haag              Project   : 1553/3910
//
//---------------------------------------------------------------------------
//   Create : 21.11.11     Update : 21.11.11
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : See definition of TY_API_DRIVER_INFO
//
//    Description :
//    Returns information from the system driver.This function can be called 
//    even if the on board components are not running or the device 
//    is not yet initialized.
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetDriverInfo( AiUInt32 ulModule, TY_API_DRIVER_INFO * px_DriverInfo )
{
    AiInt16 wRetVal = API_OK;

    wRetVal = ApiGetDriverInfo_( ulModule, px_DriverInfo );
    TRACE_BEGIN
        TRACE("        TY_API_DRIVER_INFO x_DriverInfo;\n");
    TRACE_FCTA("ApiGetDriverInfo", API_OK); 
    TRACE_PARA(ulModule);
    TRACE_RPARE("&x_DriverInfo");
    TRACE_FCTE;
    TRACE_END

        return(wRetVal);
}



//**************************************************************************
//
//   Module : A1553_API                 Submodule : ApiGetBoardInfo
//
//   Author : Patrick Giesel            Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 25.09.06     Update : 25.09.06
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function is used to return board information and can be called
//    every time.
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetBoardInfo(AiUInt32 bModule, TY_API_BOARD_INFO *px_BoardInfo)
{
	AiInt16 retval;
	
	retval = ApiGetBoardInfo_(bModule, px_BoardInfo);

	TRACE_BEGIN
	TRACE("        TY_API_BOARD_INFO x_BoardInfo;\n");
	TRACE_FCTA("ApiGetBoardInfo", retval); 
	TRACE_PARA(bModule);
	TRACE_RPARE("&x_BoardInfo");
	TRACE_FCTE;
	TRACE_END

	return retval;
}

AiInt16 ApiGetBoardInfo_(AiUInt32 bModule, TY_API_BOARD_INFO *px_BoardInfo)
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    px_BoardInfo->ul_DeviceType        = pDevice->x_BoardInfo.ul_DeviceType;
    px_BoardInfo->ul_NumberOfChannels  = pDevice->x_BoardInfo.ul_NumberOfChannels;
    px_BoardInfo->ul_NumberOfBiu       = pDevice->x_BoardInfo.ul_NumberOfBiu;
    px_BoardInfo->ul_NovRamBoardType   = pDevice->x_BoardInfo.ul_NovRamBoardType;
    px_BoardInfo->ul_NovRamBoardConfig = pDevice->x_BoardInfo.ul_NovRamBoardConfig;
    px_BoardInfo->ul_SerialNumber      = pDevice->x_BoardInfo.ul_SerialNumber;
    px_BoardInfo->ul_PartNumber        = pDevice->x_BoardInfo.ul_PartNumber;
    px_BoardInfo->ul_GlobalRamSize     = pDevice->x_BoardInfo.ul_GlobalRamSize;
    px_BoardInfo->ul_SharedRamSize     = pDevice->x_BoardInfo.ul_SharedRamSize;
    px_BoardInfo->ul_GlobalRamStartOffset[0] = pDevice->x_BoardInfo.ul_GlobalRamStartOffset[0];
    px_BoardInfo->ul_GlobalRamStartOffset[1] = pDevice->x_BoardInfo.ul_GlobalRamStartOffset[1];
    px_BoardInfo->ul_GlobalRamStartOffset[2] = pDevice->x_BoardInfo.ul_GlobalRamStartOffset[2];
    px_BoardInfo->ul_GlobalRamStartOffset[3] = pDevice->x_BoardInfo.ul_GlobalRamStartOffset[3];
    px_BoardInfo->x_BoardCapabilities.ul_CanChangeAmplitude         = pDevice->x_BoardInfo.x_BoardCapabilities.ul_CanChangeAmplitude;
    px_BoardInfo->x_BoardCapabilities.x_CouplingCapabilities.ul_All = pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.ul_All;
    px_BoardInfo->x_BoardCapabilities.x_IrigCapabilities.ul_All     = pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.ul_All;

    return API_OK;
}

//**************************************************************************
//
//  ApcEnableErrMsg
//
//**************************************************************************
void _API_DLL_FUNC ApcEnableErrMsg(AiUInt32 bModule,AiUInt8 ena)
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice != NULL )
        pDevice->err_msg = ena;
} // end: ApcEnableErrMsg


//**************************************************************************
//
//  ApiGetDrvHandle
//
//**************************************************************************
AI_LIB_FUNC AiHandle AI_CALL_CONV ApiGetDrvHandle(AiUInt32 module)
{
    TRACE_BEGIN
        TRACE_FCTA("ApiGetDrvHandle", API_OK); 
    TRACE_PARE(module);
    TRACE_FCTE;
    TRACE_END
        return _ApiGetDrvHandle( module );
} // end: ApiGetDrvHandle


//**************************************************************************
//
//  ApiInstIntHandler
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandler( AiUInt32 bModule, AiUInt8 biu,
                                                    AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc )
{
    AiInt16 uw_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( uc_Type > API_INT_TRG_TT )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == pf_IntFunc )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {

        uw_RetVal = (AiInt16) _InstIntHandler( bModule, biu, uc_Type, pf_IntFunc );

    }
    v_ExamineRetVal( "ApiInstIntHandler", uw_RetVal );
    TRACE_BEGIN
        TRACE("        TY_INT_FUNC_PTR f_IntFunc;\n");
    TRACE_FCTA("ApiInstIntHandler", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(uc_Type);
    TRACE_RPARE("&f_IntFunc");
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiInstIntHandler


//**************************************************************************
//
//  ApiDelIntHandler
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandler( AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type )
{
    AiInt16 uw_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( uc_Type > API_INT_TRG_TT )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {

        uw_RetVal = _DelIntHandler( bModule, biu, uc_Type );

    }
    v_ExamineRetVal( "ApiDelIntHandler", uw_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiDelIntHandler", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARE(uc_Type);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiDelIntHandler



//**************************************************************************
//
//  ApiFlushIntHandler
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFlushIntHandler( AiUInt32 bModule )
{
    AiInt16 uw_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else
    {

        uw_RetVal = _ApiOsFlushInterrupts( bModule );

    }
    v_ExamineRetVal( "ApiFlushIntHandler", uw_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiFlushIntHandler", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiFlushIntHandler


//**************************************************************************
//
//  ApiSetDllDbgLevel
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetDllDbgLevel( AiUInt32 ul_DllDbgLevel )
{
    AiInt16               uw_RetVal        = API_OK;
    TY_DEVICE_INFO_LIST * px_CurrentEntry  = NULL;
    AiUInt8               ucEnaDis         = API_DIS;

    TRACE_BEGIN
        TRACE_FCTA("BEGIN ApiSetDllDbgLevel", uw_RetVal); 
    TRACE_PARE(ul_DllDbgLevel);
    TRACE_FCTE;
    TRACE_END

    Aiul_DbgLevel = ul_DllDbgLevel;  // changed 16.01.01 mr (Ai added)

    if( (Aiul_DbgLevel & DBG_ERRORMSG) != 0 )
        ucEnaDis = API_ENA;
    else
        ucEnaDis = API_DIS;

    for( px_CurrentEntry  = px_MilDeviceInfoListStart;
        px_CurrentEntry != NULL;
        px_CurrentEntry  = px_CurrentEntry->px_Next )
    {
        px_CurrentEntry->x_DeviceInfo.err_msg = ucEnaDis;

    }


    TRACE_BEGIN
        TRACE_FCTA("END ApiSetDllDbgLevel", uw_RetVal); 
    TRACE_PARE(ul_DllDbgLevel);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiSetDllDbgLevel

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetGlobalTestValue(AiUInt32 value)
{
    Aiul_GlobalTestValue = value;

    TRACE_BEGIN
        TRACE_FCTA("ApiSetGlobalTestValue", API_OK);
    TRACE_PARE(value);
    TRACE_FCTE;
    TRACE_END

    return API_OK;
}

AI_LIB_FUNC AiUInt32 AI_CALL_CONV ApiGetGlobalTestValue()
{
    TRACE_BEGIN
        TRACE_FCTA("ApiGetGlobalTestValue", Aiul_GlobalTestValue);
    TRACE_FCTE;
    TRACE_END

    return Aiul_GlobalTestValue;
}


//**************************************************************************
//
//  ApiSetPnpCallback
//
//**************************************************************************

AI_LIB_FUNC void AI_CALL_CONV ApiSetPnpCallback(void *PnPCallbackFunction)
{
#ifndef _AIM_VME
    v_MilPnPCallback = (TY_PNP_CALLBACK_FUNC_PTR) PnPCallbackFunction;
#endif
}

//**************************************************************************
//
//  ApiInit
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInit(void)
{
    AiInt16 uw_RetVal = ApiInit_();
    TRACE_BEGIN
        TRACE_FCTA("ApiInit", uw_RetVal); 
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiInit

//**************************************************************************
//
//  ApiExit
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExit(void)
{
    AiInt16 uw_RetVal = ApiExit_();
    TRACE_BEGIN
        TRACE_FCTA("ApiExit", uw_RetVal); 
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiExit

//**************************************************************************
//
//  ApiExtInit
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtInit(AiUInt32 bModule, AiUInt32 par1, AiUInt32 par2)
{
    AiInt16 uw_RetVal = _ApiExtInit(bModule,par1,par2);
    TRACE_BEGIN
        TRACE_FCTA("ApiExtInit", uw_RetVal); 
    TRACE_PARA(par1);
    TRACE_PARE(par2);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiExtInit


//**************************************************************************
//
//  ApiOpen
//
//**************************************************************************
#if defined(_AIM_WINDOWS) || defined(_AIM_RTX64) || defined(_AIM_VXI) || defined(_AIM_LINUX)
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiOpen(AiUInt32 bModule, AiChar *ac_SrvName, AiUInt32 *bModuleHandle)
{
    AiInt16 uw_RetVal = API_OK;
    TY_API_OPEN x_ApiOpen;

#else
    AI_LIB_FUNC AiReturn AI_CALL_CONV ApiOpen(AiUInt32 bModule)
{
    AiInt16 uw_RetVal = API_OK;
    TY_API_OPEN x_ApiOpen;
    AiChar ac_SrvName[10];
    AiUInt32 *bModuleHandle;
    sprintf(ac_SrvName, "local");
    bModuleHandle = &bModule;
#endif

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( NULL == ac_SrvName )
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else if( NULL == bModuleHandle )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        x_ApiOpen.ul_Module = bModule;
        x_ApiOpen.ul_Stream = 0xFFFFFFFF;
        strcpy(x_ApiOpen.ac_SrvName, ac_SrvName);
        uw_RetVal = ApiOpenEx_(&x_ApiOpen, bModuleHandle);
    }

    v_ExamineRetVal( "ApiOpen", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        AiUInt32 bModuleHandle;\n");
    TRACE_FCTA("ApiOpen", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_RPARA(ac_SrvName);
    TRACE_RPARE("&bModuleHandle");
    TRACE1("    --> bModuleHandle = 0x%x\n", *bModuleHandle);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

AiInt16 ApiOpenEx_(TY_API_OPEN *px_ApiOpen, AiUInt32 *pul_ModuleHandle)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO  * pDevice = NULL;

    AiUInt32 ul_ModuleHandle;

    if (!AiOsStringCmp(px_ApiOpen->ac_SrvName, "local"))
    {
         _MilNetGetModuleHandle(px_ApiOpen, &ul_ModuleHandle);
    }
    else
    {
        ul_ModuleHandle = CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, 0);
    }
    pDevice = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);
    if (pDevice == NULL)
    {
        sprintf(buf, "pDevice == NULL for module %x ...\n", *pul_ModuleHandle);
        DEBUGOUT(DBG_ERROR, FKT_NTOPEN, buf);
        return API_ERR_WRONG_MODULE;
    }

    ai_mutex_lock(pDevice->_hInitMutex);

    uw_RetVal = _ApiOpen(px_ApiOpen, &ul_ModuleHandle);
    if (uw_RetVal != API_OK)
    {
        *pul_ModuleHandle = ul_ModuleHandle;
        ai_mutex_release(pDevice->_hInitMutex);
        return uw_RetVal;
    }
    if (pDevice->ulOpenCount == 1)
    {
        uw_RetVal = ApiInitDeviceConfig(ul_ModuleHandle);
        if (uw_RetVal != API_OK)
        {
            *pul_ModuleHandle = ul_ModuleHandle;
            ai_mutex_release(pDevice->_hInitMutex);
            return uw_RetVal;
        }
    }

    ai_mutex_release(pDevice->_hInitMutex);

    *pul_ModuleHandle = ul_ModuleHandle;
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiOpenEx(TY_API_OPEN *px_ApiOpen, AiUInt32 *pul_ModuleHandle)
{
    AiInt16  uw_RetVal = API_OK;

    if( px_ApiOpen->ul_Module >= MAX_API_MODULE )
        return API_ERR_WRONG_MODULE;

    if( (px_ApiOpen->ul_Stream > MAX_API_STREAM) && (px_ApiOpen->ul_Stream != 0xFFFFFFFF))
        return API_ERR_WRONG_STREAM;

    if( pul_ModuleHandle == NULL )
        return API_ERR_PARAM2_IS_NULL;

    
    uw_RetVal = ApiOpenEx_( px_ApiOpen, pul_ModuleHandle );

    
    v_ExamineRetVal( "ApiOpenEx", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_OPEN x_ApiOpen;\n");
    TRACE("        AiUInt32    ul_ModuleHandle;\n");
    TRACE1("        x_ApiOpen.ul_Module = 0x%x;\n", px_ApiOpen->ul_Module);
    TRACE1("        x_ApiOpen.ul_Stream = 0x%x;\n", px_ApiOpen->ul_Stream);
    TRACE1("        strcpy(x_ApiOpen.ac_SrvName, \"%s\");\n", px_ApiOpen->ac_SrvName);
    TRACE_FCTA("ApiOpenEx", uw_RetVal); 
    TRACE_RPARA("&x_ApiOpen");
    TRACE_RPARE("&ul_ModuleHandle");
    TRACE_FCTE;
    TRACE1("    --> ul_ModuleHandle = 0x%x\n", *pul_ModuleHandle);
    TRACE_END
    
    return uw_RetVal;
}



AiInt16 uw_ApiGetBiuFromStream( AiUInt32 ul_Module, AiUInt32 ul_OldBiu, AiUInt32 *pul_Biu)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ul_Stream = ((ul_Module & API_STREAM_MASK) >> API_STREAM_POS);
    AiUInt32 ul_AccessHsBiu = ((ul_Module & API_HS_ACCESS) >> 31);
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ul_Module );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( NULL == pul_Biu )
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else
    {
        *pul_Biu = ul_OldBiu;

        if( 0 != ul_Stream )
            uw_RetVal = ApiHwStreamToBiu(&pDevice->x_BoardInfo, ul_Stream, ul_AccessHsBiu, pul_Biu);
    }

    return( uw_RetVal );
}

//**************************************************************************
//
//  ApiClose
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiClose(AiUInt32 bModule)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else
    {
        uw_RetVal = ApiClose_(bModule);
    }

    v_ExamineRetVal( "ApiClose", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiClose", uw_RetVal); 
    TRACE_PARE(bModule);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}


//**************************************************************************
//
//  Define error lookup table for this module
//  Provide function to return a readable error message
//
//**************************************************************************

DEFINE_AIMILERRORS_TABLE

const char * ApiGetErrorDescription_(AiReturn error_code)
{
    AiUInt32     ulIndex     = 0;
    const char * pszErrorMsg = NULL;

    for( ulIndex = 0; axMilErrorDescription[ulIndex].ulNumber != 0; ulIndex++ )
    {
		if (axMilErrorDescription[ulIndex].ulNumber == (AiUInt32)error_code)
        {
            /* found error message */
            pszErrorMsg = axMilErrorDescription[ulIndex].sZFileName;
            break;
        }
        else if( axMilErrorDescription[ulIndex].ulNumber == API_ERR_MAXIMUM )
        {
            /* reached end of the table, use default error message */
            pszErrorMsg = axMilErrorDescription[ulIndex].sZFileName;
            break;
        }
    }

    return pszErrorMsg;
}

AI_LIB_FUNC const char * AI_CALL_CONV ApiGetErrorDescription(AiInt16 wErrorCode)
{
    return ApiGetErrorMessage((AiReturn)wErrorCode);
}

AI_LIB_FUNC const char * AI_CALL_CONV ApiGetErrorMessage(AiReturn error_code)
{
    const char * pszErrorMsg = NULL;

    pszErrorMsg = ApiGetErrorDescription_(error_code);

    TRACE_BEGIN
        TRACE("    AiInt16   wErrorCode;\n");
    TRACE1("    error_code = 0x%x;\n", error_code);

    TRACE_FCTA("ApiGetErrorDescription", API_OK);
    TRACE_RPARE("error_code");
    TRACE_FCTE;

    TRACE1("    --> pszErrorMsg = %s;\n", pszErrorMsg);
    TRACE_END

    return pszErrorMsg;
}


//**************************************************************************
//
//  Initialize device name for this module handle
//
//**************************************************************************

/*! \enum board_type_anet_ma
* Enumeration of all possible board types for ANET-MxAy platform
*/
enum mil_board_type_anet_ma
{
    BOARD_TYPE_ANET_M1A4 = 0x2110,
    BOARD_TYPE_ANET_M1A8 = 0x2210,
    BOARD_TYPE_ANET_M1A12 = 0x2310,
    BOARD_TYPE_ANET_M2A4 = 0x21A0,
    BOARD_TYPE_ANET_M2A8 = 0x22A0,
    BOARD_TYPE_ANET_M2A12 = 0x23A0,
};



static INLINE(void ApiSetBoardNameAnetMa(TY_DEVICE_INFO * pDevice, char * prot))
{
    switch ((enum mil_board_type_anet_ma) (pDevice->x_IniPar.board_type & 0xFFFF))
    {
    case BOARD_TYPE_ANET_M1A4:
        sprintf(prot, "M1A4");
        break;
    case BOARD_TYPE_ANET_M1A8:
        sprintf(prot, "M1A8");
        break;
    case BOARD_TYPE_ANET_M1A12:
        sprintf(prot, "M1A12");
        break;
    case BOARD_TYPE_ANET_M2A4:
        sprintf(prot, "M2A4");
        break;
    case BOARD_TYPE_ANET_M2A8:
        sprintf(prot, "M2A8");
        break;
    case BOARD_TYPE_ANET_M2A12:
        sprintf(prot, "M2A12");
        break;
    default:
        sprintf(prot, "MxAy");
        break;
    }
};


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetBoardName( AiUInt32 ulModHandle )
{
    TY_DEVICE_INFO * pDevice    = _ApiGetDeviceInfoPtrByModule( ulModHandle );
    size_t           iStrLength = 0;

    char strPlattform[         MAX_BOARD_NAME_LENGTH];
    char strPlattformExtension[MAX_BOARD_NAME_LENGTH];
    char strProtokoll[         MAX_BOARD_NAME_LENGTH];
    char strChannels[          MAX_BOARD_NAME_LENGTH];
    char strFunction[          MAX_BOARD_NAME_LENGTH];
    char strHardware[          MAX_BOARD_NAME_LENGTH];

    /* -- plattform --- */
    strncpy(strPlattform,          ApiHwPlatformName(     pDevice->x_IniPar.board_config & 0x000000FF), MAX_BOARD_NAME_LENGTH);
    strncpy(strPlattformExtension, ApiHwPlatformExtension(pDevice->x_IniPar.board_config & 0x000000FF), MAX_BOARD_NAME_LENGTH);

    /* -- protoll --- */

    switch( pDevice->x_IniPar.prot )
    {
    case API_PROTOCOL_1553 :
        strncpy(strProtokoll, "1553", MAX_BOARD_NAME_LENGTH);
        break;
    case API_PROTOCOL_3910 :
        strncpy(strProtokoll, "3910", MAX_BOARD_NAME_LENGTH);
        break;
    case API_PROTOCOL_EFEX :
        strncpy(strProtokoll, "EFEx", MAX_BOARD_NAME_LENGTH);
        break;
    case API_PROTOCOL_1553_3910 :
    case API_PROTOCOL_1553_EFEX :
        strncpy(strProtokoll, "EFA",  MAX_BOARD_NAME_LENGTH);
        break;
    default:
        strncpy(strProtokoll, "???",  MAX_BOARD_NAME_LENGTH);
        break;
    }

    /* -- channels --- */

    if(    (pDevice->x_IniPar.prot == API_PROTOCOL_1553_3910)
        || (pDevice->x_IniPar.prot == API_PROTOCOL_1553_EFEX) )
    {
        /* ACXEFA-1 has two   channels 1*1553 + 1*3910 */
        /* ACXEFA-2 has three channels 2*1553 + 1*3910 */
        /* ACXEFA-4 has five  channels 4*1553 + 1*3910 */
        sprintf( strChannels, "%d", pDevice->x_IniPar.chns - 1 );
    }
    else
    {
        sprintf( strChannels, "%d", pDevice->x_IniPar.chns );
    }


    /* -- functions --- */

    switch( pDevice->x_IniPar.bt[0] )
    {
    case API_TYPE_SIM_MON :
        strncpy(strFunction, "",  MAX_BOARD_NAME_LENGTH);
        break;
    case API_TYPE_SIM_ONLY :
        strncpy(strFunction, "S", MAX_BOARD_NAME_LENGTH);
        break;
    case API_TYPE_MON_ONLY :
        strncpy(strFunction, "M", MAX_BOARD_NAME_LENGTH);
        break;
    default:
        strncpy(strFunction, "",  MAX_BOARD_NAME_LENGTH);
        break;
    }


    if ((pDevice->x_IniPar.board_config & 0xFF) == AI_PLATFORM_ANET_AYS_MA)
    {
        /* Patch name for mixed board */
        strProtokoll[0] = '\0';
        ApiSetBoardNameAnetMa(pDevice, strChannels);
    }

    if ((pDevice->x_IniPar.board_config & 0xFF) == AI_PLATFORM_XMC_ARTIXUS)
    {
        if  (pDevice->x_BoardInfo.x_BoardCapabilities.ul_CanChangeAmplitude)
        {
            strncpy(strHardware, "T", MAX_BOARD_NAME_LENGTH);
        }
        else 
        {
            strncpy(strHardware, "", MAX_BOARD_NAME_LENGTH);
        }
    }
    else
    {
        strncpy(strHardware, "", MAX_BOARD_NAME_LENGTH);
    }


    iStrLength =  strlen(strPlattform)
        + strlen(strProtokoll)
        + strlen(strChannels)
        + strlen(strHardware)
        + strlen(strFunction)
        + strlen(strPlattformExtension)
        + 3 /*-*/;

    if( iStrLength >= MAX_BOARD_NAME_LENGTH )
        sprintf(pDevice->pszBoardName, "Boardname overflow.");
    else if( (strFunction[0] == '\0') && (strPlattformExtension[0] == '\0') )
        AiOsSnprintf( pDevice->pszBoardName, sizeof(pDevice->pszBoardName), AiTrue, "%s%s-%s%s", strPlattform, strProtokoll, strChannels, strHardware);
    else if( strFunction[0] == '\0' )
        AiOsSnprintf( pDevice->pszBoardName, sizeof(pDevice->pszBoardName), AiTrue, "%s%s-%s%s-%s", strPlattform, strProtokoll, strChannels, strHardware, strPlattformExtension );
    else if( strPlattformExtension[0] == '\0' )
        AiOsSnprintf( pDevice->pszBoardName, sizeof(pDevice->pszBoardName), AiTrue, "%s%s-%s%s-%s", strPlattform, strProtokoll, strChannels, strHardware, strFunction );
    else
        AiOsSnprintf( pDevice->pszBoardName, sizeof(pDevice->pszBoardName), AiTrue, "%s%s-%s%s-%s-%s", strPlattform, strProtokoll, strChannels, strHardware, strFunction, strPlattformExtension );


    return 0;
}



//**************************************************************************
//
//  Return device name for this module handle
//
//**************************************************************************

const char * ApiGetBoardName_( AiUInt32 ulModHandle )
{
    TY_DEVICE_INFO  * pDevice      = NULL;

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    return pDevice->pszBoardName;
}

AI_LIB_FUNC const char * AI_CALL_CONV ApiGetBoardName(AiUInt32 ulModHandle)
{
    const char * pszBoardName = NULL;

    
    pszBoardName = ApiGetBoardName_( ulModHandle );


    TRACE_BEGIN
    TRACE( "    AiUInt32  ulModHandle;\n");

    TRACE_FCTA("ApiGetBoardName", API_OK); 
    TRACE_RPARA("ulModHandle");
    TRACE_FCTE;

    TRACE1("    --> name = %s;\n", pszBoardName );
    TRACE_END

    return pszBoardName;
}


const char * ApiGetDevicePath_( AiUInt32 ulModHandle )
{
    TY_DEVICE_INFO  * pDevice      = NULL;

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    return pDevice->DevicePath;
}

AI_LIB_FUNC const char * AI_CALL_CONV ApiGetDevicePath(AiUInt32 ulModHandle)
{
    const char * pszDevicePath = NULL;


    pszDevicePath = ApiGetDevicePath_( ulModHandle );


    TRACE_BEGIN
    TRACE( "    AiUInt32  ulModHandle;\n");

    TRACE_FCTA("ApiGetDevicePath", API_OK);
    TRACE_RPARA("ulModHandle");
    TRACE_FCTE;

    TRACE1("    --> path = %s;\n", pszDevicePath );
    TRACE_END

    return pszDevicePath;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInitDeviceConfig(AiUInt32 ulModHandle)
{
	AiInt16           uw_RetVal = API_OK;
	TY_DEVICE_INFO* pDevice = NULL;
    TY_API_DRIVER_INFO x_DriverInfo = {0};
	AiUInt8           uc_BspCompatibility = 0;



	TY_API_DRIVER_INFO xDriverInfo;

	memset(&xDriverInfo, 0, sizeof(xDriverInfo));

	pDevice = _ApiGetDeviceInfoPtrByModule(ulModHandle);

	if (pDevice == NULL)
	{
		return API_ERR_NO_MODULE_EXTENSION;
	}

	if ((ulModHandle & API_STREAM_MASK) == 0 && pDevice->bOpenExUsed)
	{
		// if a device is opened with ApiOpenEx and stream 0, we do not access the hardware
		// this is required to access a board via read/write memdata only
		// if the ASP is not running.

		uw_RetVal = ApiGetDriverInfo_(ulModHandle, &xDriverInfo);

		if (uw_RetVal != API_OK)
		{
			/* Do not report an error if ApiGetDriverInfo fails
			because the IOCTL might not be available.
			We can still do read/write mem/usb data */
			DEBUGOUT(DBG_INIT, "ApiInitDeviceConfig", "ApiGetDriverInfo failed");
		}
		else
		{
			pDevice->uc_DeviceGroup = xDriverInfo.uc_DeviceGroup;
			pDevice->x_BoardInfo.ul_SerialNumber = xDriverInfo.ul_SN;
			pDevice->x_IniPar.board_config = xDriverInfo.ul_BoardConfig;
			pDevice->x_IniPar.board_type = xDriverInfo.ul_BoardType;
		}
	}
	else
	{
		// -- Legacy TSW version read required for new host to target communication ---
		uw_RetVal = ApiReadTswCompatVersion(ulModHandle, &pDevice->ax_VersionInfo.x_TargetSWVer);

        if (uw_RetVal != API_OK)
            return uw_RetVal;

        // -- Get device group from Driver ---
        uw_RetVal = ApiGetDriverInfo(ulModHandle, &x_DriverInfo);

        if (uw_RetVal != API_OK)
            return uw_RetVal;
        else
            pDevice->uc_DeviceGroup = x_DriverInfo.uc_DeviceGroup;

		// -- initialize stream to get board infomation ---
		uw_RetVal = ApiCmdIniLegacy_(ulModHandle, API_INIT_MODE_READ, &pDevice->x_IniPar);
		if (uw_RetVal != API_OK)
			return uw_RetVal;

		// -- Check if stream is valid ---
		if (GET_STREAM_ID(ulModHandle) > pDevice->x_BoardInfo.ul_NumberOfChannels)
			return API_ERR_WRONG_STREAM;

		// -- read versions ---
		ApiReadBSPVersionEx(ulModHandle, &pDevice->ax_VersionInfo, &uc_BspCompatibility);

		ApiCmdSysGetMemPartition(ulModHandle, 0, &pDevice->x_MemInfo);

		// -- configure library ---
		_ApiSetDeviceInfoConfig(pDevice);
		ApiHwDeviceInfoStreamInit(pDevice);
	}

	ApiSetBoardName(ulModHandle);
	_ApiOsInitDeviceStruct(pDevice);

	if (IS_REMOTE(ulModHandle))
	{
		uw_RetVal = Net1553DeviceInit(ulModHandle);
	}

	return API_OK;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiGetDeviceConfig( AiUInt32 ulModHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    AiInt16          wRetVal   = API_OK;
    AiUInt8          ucAnsUsed = 0;
    TY_DEVICE_INFO * pDevice   = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    do
    {
        // -- check input parameters ---

        if( pDevice  == NULL ){ wRetVal = API_ERR_NO_MODULE_EXTENSION;   break; }
        if( pxConfig == NULL ){ wRetVal = API_ERR_PARAM2_IS_NULL;        break; }

        // -- clear struct ---
        memset( pxConfig, 0, sizeof(TY_API_DEVICE_CONFIG) );

        // -- if we have a server connection get config from ANS ---

        wRetVal = ApiGetDeviceConfig_( ulModHandle, &ucAnsUsed, pxConfig );

        if( ucAnsUsed != 0 )
            break;

        // -- no server connection, get local config ---

        pxConfig->uc_DataQueueMemoryType = pDevice->x_Config.uc_DataQueueMemoryType;
        pxConfig->uc_DataQueueMode       = pDevice->x_Config.uc_DataQueueMode;
        pxConfig->uc_DmaEnabled          = pDevice->x_Config.uc_DmaEnabled;
        pxConfig->ul_DmaMinimumSize      = pDevice->x_Config.ul_DmaMinimumSize;
        pxConfig->ul_IntRequestCount     = pDevice->x_Config.ul_IntRequestCount;
        pxConfig->ul_DriverFlags         = pDevice->x_Config.ul_DriverFlags;
    }while(0);

    TRACE_BEGIN
        TRACE("        AiUInt32             ulModHandle;\n");
    TRACE("        TY_API_DEVICE_CONFIG xConfig;\n");
    TRACE_FCTA("ApiGetDeviceConfig", wRetVal); 
    TRACE_RPARA("ulModHandle");
    TRACE_RPARE("&xConfig");
    TRACE_FCTE;
    TRACE1("    --> pxConfig->uc_DataQueueMemoryType = 0x%x\n", pxConfig->uc_DataQueueMemoryType );
    TRACE1("    --> pxConfig->uc_DmaEnabled          = 0x%x\n", pxConfig->uc_DmaEnabled          );
    TRACE1("    --> pxConfig->ul_DmaMinimumSize.     = 0x%x\n", pxConfig->ul_DmaMinimumSize      );
    TRACE1("    --> pxConfig->ul_IntRequestCount     = 0x%x\n", pxConfig->ul_IntRequestCount     );
    TRACE_END

        return wRetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetDeviceConfig( AiUInt32 ulModHandle, TY_API_DEVICE_CONFIG * pxConfig )
{
    AiInt16          wRetVal   = API_OK;
    AiUInt8          ucAnsUsed = 0;
    TY_DEVICE_INFO * pDevice   = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    do
    {
        // -- check input parameters ---

        if( pDevice  == NULL ){ 
            sprintf(buf, "pDevice == NULL for module %x ...\n", ulModHandle);
            DEBUGOUT(DBG_ERROR, FKT_DLLINIT, buf);
            wRetVal = API_ERR_NO_MODULE_EXTENSION;   break; }
        if( pxConfig == NULL ){ wRetVal = API_ERR_PARAM2_IS_NULL;        break; }

        // -- if we have a server connection set config on ANS ---

        ai_mutex_lock(pDevice->_hInitMutex);

        wRetVal = ApiSetDeviceConfig_(ulModHandle, &ucAnsUsed, pxConfig);

        // -- always set config on local ---

        pDevice->x_Config.uc_DataQueueMemoryType = pxConfig->uc_DataQueueMemoryType;
        pDevice->x_Config.uc_DataQueueMode       = pxConfig->uc_DataQueueMode;
        pDevice->x_Config.uc_DmaEnabled          = pxConfig->uc_DmaEnabled;
        pDevice->x_Config.ul_DmaMinimumSize      = pxConfig->ul_DmaMinimumSize;
        pDevice->x_Config.ul_DriverFlags         = pxConfig->ul_DriverFlags;

        if( pxConfig->ul_IntRequestCount < INT_REQUEST_COUNT_MAX )
            // we have fixed size arrays here
            pDevice->x_Config.ul_IntRequestCount = pxConfig->ul_IntRequestCount;
        else
            pDevice->x_Config.ul_IntRequestCount = INT_REQUEST_COUNT_MAX;


        // -- OS specific driver flags ---

        _ApiSetDeviceConfigDriverFlags( ulModHandle, pxConfig->ul_DriverFlags );
        
        ai_mutex_release(pDevice->_hInitMutex);


    }while(0);

    TRACE_BEGIN
        TRACE("        AiUInt32             ulModHandle;\n");
    TRACE("        TY_API_DEVICE_CONFIG xConfig;\n");
    TRACE1("       xConfig.uc_DataQueueMemoryType = 0x%x;\n", pxConfig->uc_DataQueueMemoryType );
    TRACE1("       xConfig.uc_DmaEnabled          = 0x%x;\n", pxConfig->uc_DmaEnabled          );
    TRACE1("       xConfig.ul_DmaMinimumSize.     = 0x%x;\n", pxConfig->ul_DmaMinimumSize      );
    TRACE1("       xConfig.ul_IntRequestCount     = 0x%x;\n", pxConfig->ul_IntRequestCount     );

    TRACE_FCTA("ApiSetDeviceConfig", wRetVal); 
    TRACE_RPARA("ulModHandle");
    TRACE_RPARE("&xConfig");
    TRACE_FCTE;
    TRACE_END


        return wRetVal;
}




//**************************************************************************
//
//  ApiErr
//
//**************************************************************************
AiInt16 ApiErr(AiUInt32 bModule, AiInt16 tg_status, AiUInt32 cmd,
               AiUInt16 ack, AiInt16 *status)
{
    char szStr[256];
    char szStr1[128];
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if (pDevice->err_msg == API_ON)
    {
        switch(tg_status)
        {
        case TG_API_IO_ERR:
            sprintf(szStr,"(%d) General I/O Error CMD:%08Xh ACK:%04Xh!", tg_status, (int)cmd, ack);
            break;
        case TG_ACK:
            sprintf(szStr,"(%d) ACK returned CMD:%08Xh ACK:%04Xh!", tg_status, (int)cmd, ack);
            break;
        case TG_ACK2:
            sprintf(szStr,"(%d) ACK2 returned CMD:%08Xh ACK:%04Xh!", tg_status, (int)cmd, ack);
            break;
        case TG_NAK:
            sprintf(szStr,"(%d) NAK returned CMD:%08Xh ACK:%04Xh!", tg_status, (int)cmd, ack);
            break;
        case TG_ACK_ERR:
            sprintf(szStr,"(%d) Acknowledge Error CMD:%08Xh ACK:%04Xh!", tg_status, (int)cmd, ack);
            break;
        case TG_ERR_CLOSED:
            sprintf(szStr,"(%d) Target Communication closed !",tg_status);
            break;
        default:
            sprintf(szStr,"ERROR NO: (%d) CMD:%08Xh!",tg_status,(int)cmd);
            break;
        }

        sprintf(szStr1,"API Application Interface Error [API%d]",(bModule & API_MODULE_MASK)+1);

#if !defined(_AIM_VME)  && !defined(_AIM_LINUX) && !defined(_AIM_RTX64) && !defined(_AIM_LVRT)  
        MessageBox(0,szStr,szStr1,MB_OK);
#else
        /* 29.07.02 */
        printf("%s\n",szStr1);
#endif

    }

    // status = tg_status
    *status = tg_status;

    // abort communication
    return(API_OFF);
} // end: ApiErr


//**************************************************************************
//
//  ApiExtIo
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtIo(AiUInt32 bModule, AiUInt8 biu, AiUInt32 cmd, AiInt16 expect,
                 AiUInt8 *out_byte_p, AiInt16 out_byte_size,
                 AiUInt16 *out_word_p, AiInt16 out_word_size,
                 AiUInt8 *in_byte_p, AiInt16 *in_byte_size,
                 AiUInt16 *in_word_p, AiInt16 *in_word_size,
                 AiUInt32 *in_lword_p, AiInt16 *in_lword_size)
{
    AiInt16   i, size, size_back, status, dat_index, tg_status, repeat;
    AiUInt8   *byte_p;
    AiUInt32   bLocModule;
    AiUInt16  *word_p;
    AiUInt32  *lword_p;
    AiChar    str[100];
    AiInt16   uw_RetVal = API_OK;
    AiUInt8 *tcom_info;
    AiUInt8 *tack_info;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    tcom_info = (AiUInt8 *)AiOsMalloc(MAX_TG_CMD_SIZE);
    tack_info = (AiUInt8 *)AiOsMalloc(MAX_TG_CMD_SIZE);

    /* in case an alloc failed */
    if( (NULL == tcom_info) || (NULL == tack_info) )
    {
        sprintf(str,"Error detected: _ApiIo() for module:%d, biu:%d is unable to allocate memory\n", bModule & API_MODULE_MASK, biu);
        DEBUGOUT( DBG_ERROR, FKT_APIIO, str );  
        uw_RetVal = API_ERR_MALLOC_FAILED;
    }

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && (biu > MAX_BIU) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (cmd < 1) || (cmd > 4) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (TG_ACK != expect) && (TG_ACK2 != expect) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( NULL == out_byte_p )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else if( NULL == out_word_p )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == in_byte_p )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else if( NULL == in_byte_size )
        uw_RetVal = API_ERR_PARAM10_IS_NULL;
    else if( NULL == in_word_p )
        uw_RetVal = API_ERR_PARAM11_IS_NULL;
    else if( NULL == in_word_size )
        uw_RetVal = API_ERR_PARAM12_IS_NULL;
    else if( NULL == in_lword_p )
        uw_RetVal = API_ERR_PARAM13_IS_NULL;
    else if( NULL == in_lword_size )
        uw_RetVal = API_ERR_PARAM14_IS_NULL;
    else
    {


        bLocModule = (bModule & API_MODULE_MASK);

        _SyncWait(bModule, pDevice->_hMutex);

        status = API_OK;
        repeat = API_ON;

        while (repeat == API_ON)
        {
            byte_p = out_byte_p;
            word_p = out_word_p;

            *in_byte_size = 0;
            *in_word_size = 0;
            *in_lword_size = 0;

            // calculate size of command array
            size = (AiInt16)(out_byte_size + (2 * out_word_size) + 7);

            tcom_info[0] = biu;
            tcom_info[1] = (AiUInt8)((cmd >> 24) & 0x000000FF); // changed pg 10.01.2000
            tcom_info[2] = (AiUInt8)((cmd >> 16) & 0x000000FF);
            tcom_info[3] = (AiUInt8)((cmd >> 8)  & 0x000000FF);
            tcom_info[4] = (AiUInt8)(cmd & 0x000000FF);
            tcom_info[5] = (AiUInt8)((size >> 8) & 0x00FF);
            tcom_info[6] = (AiUInt8)(size & 0x00FF);
            dat_index = 7;

            // Append byte-message
            for (i=0; i<out_byte_size; i++)
            {
                tcom_info[dat_index+i] = *byte_p;
                ++byte_p;
            }

            // Append word-message
            dat_index = (AiInt16)(dat_index + out_byte_size);
            for (i=0; i<2*out_word_size; i=i+2)
            {
                tcom_info[dat_index+i] = (AiUInt8)(*word_p / 256);
                tcom_info[dat_index+i+1] = (AiUInt8)(*word_p & 0x00FF);
                ++word_p;
            }

            // Put instruction to driver / get acknowledge info
            tg_status = _ApiExtIo(bModule, &tcom_info[0], &tack_info[0]);

            if (tg_status == API_OK)
                repeat = API_OFF;
            else
            {
                sprintf(str,"Error Ext(1) detected - Cmd(%02d,%02x)",bLocModule,tcom_info[1]);
                //            MessageBox(0,"API1553",str,MB_OK);
                repeat = API_OFF;
            }

            // Get acknowledge data or emulate
            if ((status == API_OK) && (repeat == API_OFF))
            {
                // no acknowledge emulation
                size_back = 0;
                tg_status = tack_info[1]; // ACK byte
                switch (tg_status)
                {
                case TG_XMT_ACK: // 0x20
                    tg_status = TG_ACK;
                    break;

                case TG_XMT_NAK: // 0x24
                    tg_status = TG_NAK;
                    break;

                case TG_XMT_ACK2: // 0x22
                    tg_status = TG_ACK2;
                    size_back = tack_info[2];
                    size_back = (AiInt16)(((size_back << 8) + tack_info[3]) - 4);
                    break;

                default:
                    tg_status = TG_ACK_ERR;
                    break;

                } // end switch tg_status


                // compare tg_status with expected value */
                if (tg_status != expect)
                {
                    sprintf(str,"Error Ext(2) detected - Ack(%02d,%02x)",bLocModule,tack_info[1]);
                    //               MessageBox(0,"API1553",str,MB_OK);
                    repeat = API_OFF;
                }
                else
                {
                    byte_p = in_byte_p;
                    word_p = in_word_p;
                    lword_p = in_lword_p;
                    repeat = API_OFF;

                    // get acknowledge data info
                    switch(cmd)
                    {
                        /* System Commands */
                    case TG_EXT_PCI_INI:
                        *in_lword_size = 16;
                        for (i=0; i<(4 * *in_lword_size); i+=4)
                        {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_EXT_PCI_IP_LW_WRITE:
                        break;
                    case TG_EXT_PCI_IP_LW_READ:
                        *in_lword_size = 1;
                        for (i=0; i<(4 * *in_lword_size); i+=4)
                        {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_EXT_PCI_IP_ALLOC_MEM:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                        /* 10.06.99 */
                    case TG_EXT_SET_REPLAY_ADDR:
                        break;
                    default :
                        status = API_ERR_UNKNOWN_COMMAND;
                        repeat = API_OFF;
                        break;
                    } // end switch apc cmd
                }
            }
        } // end while

        _SyncRelease(bModule, pDevice->_hMutex);
        uw_RetVal = status;
    }

    if(NULL != tcom_info) AiOsFree(tcom_info);
    if(NULL != tack_info) AiOsFree(tack_info);

    v_ExamineRetVal( "ApiExtIo", uw_RetVal );

    return(uw_RetVal);
} // end: ApiExtIo


//**************************************************************************
//
//  ApiEmul
//
//**************************************************************************
#ifdef __BORLANDC__
#pragma option -w-par
#endif


//**************************************************************************
//
//  ApiIoStruct for new target communication
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiIoStruct( AiUInt32 ulModHandle, TY_MIL_COM * pTargetCommand, TY_MIL_COM_ACK * pReturnData )
{
    AiInt16   uwRetVal    = API_OK;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    if( pDevice == NULL )
        // Device extension not available.
        return API_ERR_DEVICE_NOT_FOUND;

    if( GET_SERVER_ID(ulModHandle) )
        return _MilNetIoStruct(ulModHandle, pTargetCommand, pReturnData);

    if( ! IsTswVersionGreaterOrEqual( pDevice, 20, 00 ) )
        // Target software to old to support new h2t commands
        return API_ERR_WRONG_TSW_VERSION;

    // continue with command execution
    pDevice->tcom_status = API_ON;

    uwRetVal = _ApiIo( ulModHandle, (AiUInt8*)pTargetCommand, (AiUInt8*)pReturnData );

    if( uwRetVal == API_OK )
    {
        uwRetVal = (AiInt16)pReturnData->ulError;
    }

    pDevice->tcom_status = API_OFF;

    return uwRetVal;
}

//**************************************************************************
//
//  ApiIo
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiIo(AiUInt32 bModule, AiUInt8 biu, AiUInt32 cmd, AiInt16 expect,
              AiUInt8  *out_byte_p, AiInt16 out_byte_size,
              AiUInt16 *out_word_p, AiInt16 out_word_size,
              AiUInt8  *in_byte_p,  AiInt16 *in_byte_size,
              AiUInt16 *in_word_p,  AiInt16 *in_word_size,
              AiUInt32 *in_lword_p, AiInt16 *in_lword_size)
{
    AiUInt16 uwBytesInterpreted = 0;
    AiInt16   i, size, size_back, status, dat_index, tg_status, repeat;
    AiUInt8   *byte_p;
    AiUInt32   bLocModule;
    AiUInt16  *word_p;
    AiUInt32  *lword_p;
    AiChar    str[200], str2[100];
    AiInt16   uw_RetVal = API_OK;
    AiUInt32  ul_Biu;

    AiUInt8 *tcom_info;
    AiUInt8 *tack_info;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;




    if( GET_SERVER_ID(bModule) )
    {
        return _MilNetIo(bModule, biu, cmd, expect,
                      out_byte_p, out_byte_size,
                      out_word_p, out_word_size,
                      in_byte_p,  in_byte_size,
                      in_word_p,  in_word_size,
                      in_lword_p, in_lword_size);
    }





    tcom_info = (AiUInt8 *)AiOsMalloc(MAX_TG_CMD_SIZE);
    tack_info = (AiUInt8 *)AiOsMalloc(MAX_TG_CMD_SIZE);

    /* in case an alloc failed */
    if( (NULL == tcom_info) || (NULL == tack_info) )
    {
        sprintf(str,"Error detected: _ApiIo() for module:%d, biu:%d is unable to allocate memory\n", bModule & API_MODULE_MASK, biu);
        DEBUGOUT( DBG_ERROR, FKT_APIIO, str );  
        uw_RetVal = API_ERR_MALLOC_FAILED;
    }

    if( (Aiul_DbgLevel & DBG_PARAMCHK) != 0 ) { 
        if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
            uw_RetVal = API_ERR_WRONG_MODULE;
        else if( (0 == (bModule & API_STREAM_MASK)) && (biu > MAX_BIU) )
            uw_RetVal = API_ERR_WRONG_BIU;
        else if( (TG_ACK != expect) && (TG_ACK2 != expect) )
            uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
        else if( (NULL == out_byte_p) && (0 != out_byte_size) )
            uw_RetVal = API_ERR_PARAM5_IS_NULL;
        else if( (NULL == out_word_p) && (0 != out_word_size) )
            uw_RetVal = API_ERR_PARAM7_IS_NULL;
        else if( NULL == in_byte_p )
            uw_RetVal = API_ERR_PARAM9_IS_NULL;
        else if( NULL == in_byte_size )
            uw_RetVal = API_ERR_PARAM10_IS_NULL;
        else if( NULL == in_word_p )
            uw_RetVal = API_ERR_PARAM11_IS_NULL;
        else if( NULL == in_word_size )
            uw_RetVal = API_ERR_PARAM12_IS_NULL;
        else if( NULL == in_lword_p )
            uw_RetVal = API_ERR_PARAM13_IS_NULL;
        else if( NULL == in_lword_size )
            uw_RetVal = API_ERR_PARAM14_IS_NULL;
    }

    if(uw_RetVal==API_OK)
    {
        bLocModule = (bModule & API_MODULE_MASK);

        _SyncWait(bLocModule, pDevice->_hMutex);

        // continue with command execution
        pDevice->tcom_status = API_ON;

        status = API_OK;
        repeat = API_ON;

        while (repeat == API_ON)
        {
            byte_p = out_byte_p;
            word_p = out_word_p;

            *in_byte_size = 0;
            *in_word_size = 0;
            *in_lword_size = 0;

            // calculate size of command array
            size = (AiInt16)(out_byte_size + (2 * out_word_size) + 7);

            // if biu coded in module handle (opened with ApiOpenEx, biu bits > 0), use it,
            // otherwise (opened with old ApiOpen) use biu coded in "biu" parameter
            uw_ApiGetBiuFromStream( bModule, biu, &ul_Biu );

            // Generate command instruction to target
            tcom_info[0] = (AiUInt8)ul_Biu;
            tcom_info[1] = (AiUInt8)((cmd >> 24) & 0x000000FF);
            tcom_info[2] = (AiUInt8)((cmd >> 16) & 0x000000FF);
            tcom_info[3] = (AiUInt8)((cmd >> 8)  & 0x000000FF);
            tcom_info[4] = (AiUInt8)(cmd & 0x000000FF);
            tcom_info[5] = (AiUInt8)((size >> 8) & 0x00FF);
            tcom_info[6] = (AiUInt8)(size & 0x00FF);
            dat_index = 7;

            // Append byte-message
            for (i=0; i<out_byte_size; i++)
            {
                tcom_info[dat_index+i] = *byte_p;
                ++byte_p;
            }

            // Append word-message
            dat_index = (AiInt16)(dat_index + out_byte_size);
            for (i=0; i<2*out_word_size; i=i+2)
            {
                tcom_info[dat_index+i]   = (AiUInt8)(*word_p / 256);
                tcom_info[dat_index+i+1] = (AiUInt8)(*word_p & 0x00FF);
                ++word_p;
            }

            // Put instruction to driver / get acknowledge info
            tg_status = _ApiIo(bModule, &tcom_info[0], &tack_info[0]);      // pass bModule 21.03.2001 MR

            if (tg_status == API_OK)
                repeat = API_OFF;
            else
            {
                // Error in driver connection

                sprintf(str,"Error detected: _ApiIo() for module:%d, biu:%d returned API_ERR (cmd=%08X,tg=%d,err_msg=%d)\n",
                    bLocModule, biu, (tcom_info[1] << 24) + (tcom_info[2] << 16) +
                    (tcom_info[3] << 8) + (tcom_info[4]),
                    tg_status,pDevice->err_msg);
                sprintf( str2, "API1553 Module:%d Biu:%d", bLocModule, ul_Biu);

                if ((pDevice->err_msg) == API_ON) {

#if !defined(_AIM_VME) && !defined(_AIM_LINUX)  && !defined(_AIM_RTX64) && !defined(_AIM_LVRT)  
                    MessageBox(0,str, str2,MB_OK);
#else
                    printf("%s\n%s\n",str, str2);
#endif
                }

                DEBUGOUT( DBG_ERROR, FKT_APIIO, str );
                repeat = ApiErr(bModule, tg_status, (AiUInt32)( ((tcom_info[1] << 24) + (tcom_info[2] << 16) +
                    (tcom_info[3] << 8) + (tcom_info[4])) | (tcom_info[0] << 24) ) /*cmd*/,
                    (AiUInt16)((tack_info[0] << 8) + tack_info[1])/*ack*/, &status);
            }

            // Get acknowledge data or emulate
            if ((status == API_OK) && (repeat == API_OFF))
            {
                // no acknowledge emulation
                size_back = 0;
                tg_status = tack_info[1]; // ACK byte
                switch (tg_status)
                {
                case TG_XMT_ACK: // 0x20
                    tg_status = TG_ACK;
                    break;

                case TG_XMT_NAK: // 0x24
                    tg_status = TG_NAK;
                    break;

                case TG_XMT_ACK2: // 0x22
                    tg_status = TG_ACK2;
                    size_back = tack_info[2];
                    size_back = (AiInt16)(((size_back << 8) + tack_info[3]) - 4);
                    break;

                default:
                    tg_status = TG_ACK_ERR;
                    break;
                } // end switch tg_status

                // compare tg_status with expected value */
                if (tg_status != expect)
                {
                    // Target error
                    sprintf(str,"Error detected: Did not get expected ACK for module:%d, biu:%d (cmd=%08X,expected=%d,ret=%d,NAK_INFO=%08X)\n",
                        bLocModule, ul_Biu, (tcom_info[1] << 24) + (tcom_info[2] << 16) + (tcom_info[3] << 8) + (tcom_info[4]),
                        expect,tg_status, (tack_info[4] << 24) + (tack_info[5] << 16) + (tack_info[6] << 8) + (tack_info[7]));
                    sprintf( str2, "API1553 Module:%d Biu:%d", bLocModule, ul_Biu); // added module and biu info xxxx MR

                    if ((pDevice->err_msg) == API_ON)

#if !defined(_AIM_VME) && !defined(_AIM_LINUX) && !defined(_AIM_RTX64) && !defined(_AIM_LVRT)  
                        MessageBox(0,str, str2,MB_OK);
#else
                        /* 29.07.02 */
                        printf("%s\n%s\n",str, str2);
#endif

                    DEBUGOUT( DBG_ERROR, FKT_APIIO, str );
                    repeat = ApiErr(bModule, tg_status, (AiUInt32)( ((tcom_info[1] << 24) + (tcom_info[2] << 16) +
                        (tcom_info[3] << 8) + (tcom_info[4])) | (tcom_info[0] << 24) ) /*cmd*/,
                        (AiUInt16)((tack_info[0] << 8) + tack_info[1])/*ack*/, &status);

                    // Return NAK_INFO as return value
                    status = (tack_info[4] << 24) + (tack_info[5] << 16) + (tack_info[6] << 8) + (tack_info[7]);
                }
                else
                {
                    byte_p  = in_byte_p;
                    word_p  = in_word_p;
                    lword_p = in_lword_p;
                    repeat  = API_OFF;

                    // get acknowledge data info
                    switch(cmd)
                    {
                        /* System Commands */
                    case TG_API_INI:
                        *in_byte_size = 7;
                        uwBytesInterpreted = 7;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }

                        *in_word_size = 4;
                        uwBytesInterpreted += 4 * 2;
                        for (i=*in_byte_size; i<(*in_byte_size+(2 * *in_word_size)); i+=2)
                        {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;
                        }

                        uwBytesInterpreted += 27 * 4;
                        if( uwBytesInterpreted < size_back )
                        {
                            // TSW >= 18.90
                            uwBytesInterpreted += 3 * 4;
                            *in_lword_size = 30;
                        }
                        else
                        {
                            // TSW < 18.90
                            *in_lword_size = 27;
                        }

                        for (i=(*in_byte_size+(2 * *in_word_size)); i<((*in_byte_size+(2 * *in_word_size))+(4 * *in_lword_size)); i+=4)
                        {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_RESET:
                        *in_byte_size = 20;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_READ_SW_VERS:
                        if( 22 == size_back )
                        {
                            *in_word_size = 1;
                            *in_lword_size = 5;
                        }
                        else
                        {
                            *in_word_size = 2;
                            *in_lword_size = 4;
                        }

                        for (i=0; i<(2 * *in_word_size); i+=2)
                        {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;
                        }

                        for (i=(2 * *in_word_size); i<((2 * *in_word_size)+(4 * *in_lword_size)); i+=4)
                        {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_READ_SW_VERS_EX:
                        *in_lword_size = 12;
                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_LOAD_SREC:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        *in_lword_size = 1;
                        for (i=*in_byte_size; i<(*in_byte_size+(4 * *in_lword_size)); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_PROG_FLASH:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                        /* Data Buffer Commands */
                    case TG_API_RAM_WRITE:
                    case TG_API_RAM_LWORD_WR:
                    case TG_API_RAM_WORD_WR:
                    case TG_API_RAM_BYTE_WR:
                        break;
                    case TG_API_RAM_READ:
                        *in_word_size = (AiInt16)(size_back / 2);
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;
                        }
                        break;
                    case TG_API_RAM_LWORD_RD:
                        *in_lword_size = 1;
                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_RAM_WORD_RD:
                        *in_word_size = 1;
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;
                        }
                        break;
                    case TG_API_RAM_BYTE_RD:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_BUF_BWTS_LD_MSG:
                        *in_lword_size = 1;
                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;

                        /* special commands only for AXI*/
                    case TG_API_SET_AXI_TRG_MUX:        
                        break;
                        /* GenIo Commands */
                    case TG_API_GEN_IO_GET_BOARDS:
                        *in_byte_size = 1;
                        for (i=0; i<(*in_byte_size); i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        *in_lword_size = 16;
                        for (i=0; i<(4 * *in_lword_size); i+=4)
                        {
                            *lword_p = tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            *lword_p = (*lword_p << 8) + tack_info[8+i];
                            lword_p++;
                        }

                    case TG_API_GEN_IO_ADDR_INIT:
                    case TG_API_GEN_IO_OUT_CHN_WRITE:
                    case TG_API_GEN_IO_STASK_CTRL:
                        break;
                    case TG_API_GEN_IO_READ:
                    case TG_API_GEN_IO_IN_CHN_READ:
                    case TG_API_GEN_IO_OUT_CHN_READ:
                        *in_lword_size = 1;
                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;

                        /* BWTS Commands */
                    case TG_API_IP_ADDR_INIT:
                    case TG_API_IP_OUT_CHN_WRITE:
                    case TG_API_SYS_REC_SS:
                    case TG_API_SYS_REC_CNF:
                    case TG_API_SYS_REC_COPY:
                    case TG_API_SYS_STASK_CTRL:
                        break;
                    case TG_API_IP_IN_CHN_READ:
                    case TG_API_IP_OUT_CHN_READ:
                        *in_lword_size = 1;
                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;
                    case TG_API_SYS_REC_STATUS:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        *in_lword_size = 3;
                        for (i=*in_byte_size; i<(*in_byte_size+(4 * *in_lword_size)); i+=4) {

                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;

                    case TG_API_PARA_DEF:
                    case TG_API_SCOPE_SETUP:
                    case TG_API_SCOPE_TRG_DEF:
                        break;
                    case TG_API_SCOPE_CALIBRATE: 
                        *in_lword_size = (AiInt16)(size_back / 4);

                        if( *in_lword_size > 7 ) 
                            *in_lword_size = 7;

                        for (i=0; i<(4 * *in_lword_size); i+=4) {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;

                    case TG_API_UTIL:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++) {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        *in_lword_size = (AiInt16)((size_back - *in_byte_size) / 4);
                        for (i=*in_byte_size; i<(*in_byte_size+(4 * *in_lword_size)); i+=4)
                        {
                            *lword_p = tack_info[4+i];
                            *lword_p = (*lword_p << 8) + tack_info[5+i];
                            *lword_p = (*lword_p << 8) + tack_info[6+i];
                            *lword_p = (*lword_p << 8) + tack_info[7+i];
                            lword_p++;
                        }
                        break;

#ifdef _CMPL4HATS
                    case TG_API_EXT_TPMC_INI: 
                        break;
                    case TG_API_EXT_TPMC_DIN_READ:
                        *in_word_size = 2;
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;
                        }
                        break;
                    case TG_API_EXT_TPMC_DOUT_WRITE:
                        break;
                    case TG_API_EXT_TPMC_LED_CON:
                        break;
                    case TG_API_EXT_TPMC_DAC_WRITE:
                        break;
                    case TG_API_EXT_TPMC_DAC_READ:
                        *in_word_size = (AiInt16)(size_back / 2);
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;               
                        }
                        break;
                    case TG_API_EXT_TPMC_ADC_READ:
                        *in_word_size = (AiInt16)(size_back / 2);
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;               
                        }
                        break;
                    case TG_API_EXT_TPMC_TEST:
                        break;
                    case TG_API_EXT_LVDT_INI:
                        break;
                    case TG_API_EXT_LVDT_OUT_WRITE:
                        break;
                    case TG_API_EXT_LVDT_OUT_READ:
                        *in_word_size = (AiInt16)(size_back / 2);
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;               
                        }
                        break;
                    case TG_API_EXT_LVDT_IN_READ:
                        *in_word_size = (AiInt16)(size_back / 2);
                        for (i=0; i<(2 * *in_word_size); i+=2) {
                            *word_p = tack_info[4+i];
                            *word_p = (*word_p << 8) + tack_info[5+i];
                            word_p++;               
                        }
                        break;
#endif            

#ifdef _API3910
                        /* GPIO Commands */
                    case TG_API_HS_GPIO_RESET:
                        *in_byte_size = size_back;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_HS_GPIO_BROADC_CMD:
                        break;
                    case TG_API_HS_GPIO_BROADC_WRITE:
                        break;
                    case TG_API_HS_GPIO_CMD:
                        break;
                    case TG_API_HS_GPIO_STATUS:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_HS_GPIO_READ:
                        *in_byte_size = 1;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_HS_GPIO_WRITE:
                        break;
                    case TG_API_HS_GPIO_READ_BLOCK:
                        *in_byte_size = size_back;
                        for (i=0; i<*in_byte_size; i++)
                        {
                            *byte_p = tack_info[4+i];
                            byte_p++;
                        }
                        break;
                    case TG_API_HS_GPIO_WRITE_BLOCK:
                        break;

#endif /* _API3910 */


                    default :
                        sprintf(str,"Module=%d, biu=%d, Error in ApiIo -> jumped into default section of switch() statement\n",
                            bLocModule, ul_Biu );
                        DEBUGOUT( DBG_ERROR, FKT_APIIO, str );

                        status = API_ERR_UNKNOWN_COMMAND;
                        repeat = API_OFF;
                        break;
                    } // end switch apc cmd
                }
            }
        } // end while

        pDevice->tcom_status = API_OFF;

        _SyncRelease(bLocModule, pDevice->_hMutex);

        uw_RetVal = status;
    }

    v_ExamineRetVal( "ApiIo", uw_RetVal );

    if(NULL != tcom_info) AiOsFree(tcom_info);
    if(NULL != tack_info) AiOsFree(tack_info);

    return(uw_RetVal);
} // end: ApiIo




void _ApiInitializeDeviceInfo( TY_DEVICE_INFO * pInfo )
{

    /* clear all entries */
    memset( pInfo, 0, sizeof(*pInfo) );

    pInfo->_hDrv        = INVALID_HANDLE_VALUE;
    pInfo->err_msg      = API_ON;

    /* Now initialize OS specific members */
    _ApiInitializeDeviceOsSpecific(pInfo);

}


void _ApiSetDeviceInfoConfig( TY_DEVICE_INFO * p_DeviceInfo )
{
    /* -- initialize device config --- */

    if( p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_AYE )
    {
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 1; // direct
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }
    else if (p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_AYS_ASP)
    {
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 1; // direct
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_GLOBAL;
    }
    else if (p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_ZYNQMP_ASP)
    {
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 0; // legacy
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }
    else if (p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_ARTIX7)
    {
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 0; // legacy
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }
    else if (p_DeviceInfo->uc_DeviceGroup == AI_DEVICE_ARTIXUS)
    {
        //@AIM_FIXME: AXCX1553 should be set to direct as well as soon aas the real hw arrives (Mockup has only 1MB memory)
        p_DeviceInfo->x_Config.uc_DataQueueMode = 0; // legacy
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }
    else
    {
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 0; // legacy
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }

    if (GET_SERVER_ID(p_DeviceInfo->ul_ModuleNo))
    {
        /* When ANS is used reading the data might be to slow,
           so we better let the TSW handle it on the server or board.
        */
        p_DeviceInfo->x_Config.uc_DataQueueMode       = 0; // legacy
        p_DeviceInfo->x_Config.uc_DataQueueMemoryType = API_MEMTYPE_SHARED;
    }

    /* default values */
    p_DeviceInfo->x_Config.uc_DmaEnabled      = 0;
    p_DeviceInfo->x_Config.ul_DmaMinimumSize  = 0;
    p_DeviceInfo->x_Config.ul_IntRequestCount = 1;
    p_DeviceInfo->x_Config.ul_DriverFlags     = 0;

    _ApiSetDeviceInfoConfigOsSpecific(p_DeviceInfo);
}




//***************************************************************************
//***              D E V I C E L I S T     C O M M A N D S
//***************************************************************************

/****************************************************************************
Function    : _ApiCreateDeviceListInfo
-----------------------------------------------------------------------------

Inputs      : Device Name
Type of device

Outputs     : Pointer to a Device List Element

Description : Alocates a new Device List Element and stores the Parameters

*****************************************************************************/
TY_DEVICE_INFO_LIST * _ApiCreateMilDeviceListInfo( char* DevicePath)
{
    TY_DEVICE_INFO_LIST * px_DevListEntry          = NULL;
    TY_DEVICE_INFO_LIST * px_LastLocalDevListEntry = NULL;
    TY_DEVICE_INFO      * px_DeviceInfo            = NULL;
    AiUInt32              isExternalDevice         = 0;
    AiUInt32              b_ServerId = 1;
    char str[100]; // for DEBUGOUT

    /* check if device is remote or local */
    if( !strncmp("Remote", DevicePath, 6) )
        isExternalDevice = 1;
    else
        isExternalDevice = 0;

    //sprintf(str, "Add external? %i\n", isExternalDevice);
    //DEBUGOUT( DBG_INIT, " CreateDeviceList", str );

    /* Allocate List Element */
    px_DevListEntry = (TY_DEVICE_INFO_LIST *)AiOsMalloc(sizeof(TY_DEVICE_INFO_LIST) );
    if (NULL != px_DevListEntry)
    {
        px_DeviceInfo = &px_DevListEntry->x_DeviceInfo;

        _ApiInitializeDeviceInfo( px_DeviceInfo );

        /* Get Memory for Device Path Name and copy it */
        px_DeviceInfo->DevicePathLen = strlen(DevicePath);
        px_DeviceInfo->DevicePath = (char*)AiOsMalloc(px_DeviceInfo->DevicePathLen+1);

        if (NULL != px_DeviceInfo->DevicePath)
        {
            strcpy( px_DeviceInfo->DevicePath, DevicePath);

            /* preset all other Values Values*/
            px_DeviceInfo->_hInterruptThread = INVALID_HANDLE_VALUE;

            /* Store Device Info in List */
            if (NULL != px_MilDeviceInfoListStart)
            {
                /* Add at tail*/
                px_DevListEntry->px_Next = NULL;
                px_DevListEntry->px_Prev = px_MilDeviceInfoListEnd;

                px_MilDeviceInfoListEnd->px_Next = px_DevListEntry;
                px_MilDeviceInfoListEnd          = px_DevListEntry;

                if( isExternalDevice ){
                    /* set module number for remote board */
                    sscanf(DevicePath, "Remote(%d:%d)", &(px_DevListEntry->x_DeviceInfo.ul_ModuleNo), &b_ServerId);
                    px_DevListEntry->x_DeviceInfo.ul_ModuleNo |= (b_ServerId << API_SERVER_POS);
                }
                else {
                    /* set module number for local board */
                    /* find last local board. The new device a moduleID one higher  */
                    px_LastLocalDevListEntry = px_DevListEntry->px_Prev;
                    while( px_LastLocalDevListEntry != NULL ){
                        if( GET_SERVER_ID(px_LastLocalDevListEntry->x_DeviceInfo.ul_ModuleNo) ){
                            /* if the previous board is a remote board, ignore it and continue search */
                            px_LastLocalDevListEntry = px_LastLocalDevListEntry->px_Prev;
                        }
                        else{
                            /* if the previous board is a local board, use its ModuleId + 1 */
                            px_DevListEntry->x_DeviceInfo.ul_ModuleNo = px_LastLocalDevListEntry->x_DeviceInfo.ul_ModuleNo +1;
                            break;
                        }
                    }
                    /* if we didn't find a local board, set ModuleId to 0 */
                    if( px_LastLocalDevListEntry == NULL )
                        px_DevListEntry->x_DeviceInfo.ul_ModuleNo = 0;
                }
            }
            else
            {
                /* is First */
                px_DevListEntry->px_Next = NULL;
                px_DevListEntry->px_Prev = NULL;

                px_MilDeviceInfoListStart = px_MilDeviceInfoListEnd = px_DevListEntry;

                if( isExternalDevice ) {
                    sscanf(DevicePath, "Remote(%d:%d)", &(px_DevListEntry->x_DeviceInfo.ul_ModuleNo), &b_ServerId);
                    px_DevListEntry->x_DeviceInfo.ul_ModuleNo |= (b_ServerId << API_SERVER_POS);
                }
                else {
                    px_DevListEntry->x_DeviceInfo.ul_ModuleNo = 0;
                }
            }

            sprintf(str, "ModuleId %x\n", px_DevListEntry->x_DeviceInfo.ul_ModuleNo);
            DEBUGOUT(DBG_INIT, " CreateDeviceList", str);
            /* create unnamed mutex */
            (&px_DevListEntry->x_DeviceInfo)->_hInitMutex = ai_mutex_create();
            sprintf(str, "created Init mutex : normal=%p \n", px_DevListEntry->x_DeviceInfo._hInitMutex);
            DEBUGOUT(DBG_MUTEX, " _ApiCreateMilDeviceListInfo", str);
            _ApiOsInitializeLocks(px_DevListEntry->x_DeviceInfo.ul_ModuleNo);

        }
        else
        {
            /* Error Got No memory for Device Path */
            AiOsFree(px_DevListEntry);
            px_DevListEntry = NULL;
        }
    }

    return px_DevListEntry;
} /* Ai429IntCreateDeviceListInfo */

/****************************************************************************
Function    : Ai429IntDeleteDeviceListInfo
-----------------------------------------------------------------------------

Inputs      : -

Outputs     : -

Description : Deletes a device from the list

*****************************************************************************/
AiUInt32 _ApiDeleteMilDeviceListInfo(   TY_DEVICE_INFO_LIST *px_DevListEntry, 
                                  TY_DEVICE_INFO *px_DeviceInfo)
{
    AiUInt32 ul_RetVal = 0;
    TY_DEVICE_INFO_LIST *px_Entry = px_MilDeviceInfoListStart;

    while ( px_Entry != NULL)
    {
        if ( px_DevListEntry !=NULL )
        {  /* Search by List entry pointer */
            if (px_Entry == px_DevListEntry)
                break;
        }
        else
        {  /* Search by Device Info pointer */
            if (&px_Entry->x_DeviceInfo == px_DeviceInfo)
            {
                _ApiOsFreeLocks(px_DeviceInfo->ul_ModuleNo);

                ai_mutex_free(px_DeviceInfo->_hInitMutex);
                px_DeviceInfo->_hInitMutex = 0;
                break;
            }
        }
        px_Entry = px_Entry->px_Next;
    }

    if ( NULL != px_Entry)
    {
        /* Entry found in List, take out of List*/
        /* Handle if Start or End of List */
        if ( (px_Entry == px_MilDeviceInfoListStart) ||  
            (px_Entry == px_MilDeviceInfoListEnd) )
        {
            if (px_Entry == px_MilDeviceInfoListStart)
            {
                px_MilDeviceInfoListStart = px_MilDeviceInfoListStart->px_Next;
                if (NULL != px_MilDeviceInfoListStart)
                    px_MilDeviceInfoListStart->px_Prev = NULL; 
            }
            if (px_Entry == px_MilDeviceInfoListEnd)
            {
                px_MilDeviceInfoListEnd = px_MilDeviceInfoListEnd->px_Prev;
                if (NULL != px_MilDeviceInfoListEnd)
                    px_MilDeviceInfoListEnd->px_Next = NULL;
            }
        }
        else
        {
            px_Entry->px_Prev->px_Next = px_Entry->px_Next;
            px_Entry->px_Next->px_Prev = px_Entry->px_Prev;
        }

        /* Free Memory */
        if ( NULL != px_Entry->x_DeviceInfo.px_ScopeSetup) {
            AiOsFree( px_Entry->x_DeviceInfo.px_ScopeSetup);
        }
        AiOsFree( px_Entry->x_DeviceInfo.DevicePath );
        AiOsFree( px_Entry );
    }
    else
    {
        /* Entry not available */
        ul_RetVal = 0xffffffff; /* == (AiUInt32) -1; */
    }
    return ul_RetVal;
} // Ai429IntDeleteDeviceListInfo


/****************************************************************************
Function    : Ai429IntDeleteDeviceListInfo
-----------------------------------------------------------------------------

Inputs      : -

Outputs     : -

Description : Deletes a device from the list

*****************************************************************************/

void _ApiClearDeviceList ( void )
{
    Aiapi_max_boards = 0;

	while( NULL != px_MilDeviceInfoListStart )
        _ApiDeleteMilDeviceListInfo( px_MilDeviceInfoListStart, NULL );
}


/****************************************************************************
Function    : Ai429IntGetDeviceInfoPtrByName
-----------------------------------------------------------------------------

Inputs      : DevicePath - the name the device to be found has

Outputs     : -

Description : Returns the device that has the same name as the input parameter

*****************************************************************************/
TY_DEVICE_INFO * _ApiGetDeviceInfoPtrByName( char* DevicePath)
{
    TY_DEVICE_INFO_LIST *px_DevListEntry = px_MilDeviceInfoListStart;
    TY_DEVICE_INFO * px_Ret = NULL;

    while ( px_DevListEntry != NULL)
    {
        if ( 0 == strcmp( px_DevListEntry->x_DeviceInfo.DevicePath, DevicePath) )
        {
            px_Ret = &px_DevListEntry->x_DeviceInfo;
            break; 
        }

        px_DevListEntry = px_DevListEntry->px_Next;
    }
    return px_Ret;
} /* Ai429IntGetDeviceInfoPtrByName */


/****************************************************************************
Function    : Ai429IntDeleteDeviceListInfo
-----------------------------------------------------------------------------

Inputs      : -

Outputs     : -

Description : 

*****************************************************************************/
TY_DEVICE_INFO * _ApiGetDeviceInfoPtrByModule( AiUInt32 ulModHandle )
{
    TY_DEVICE_INFO_LIST * px_CurrentEntry = NULL;

    // mask away the stream
    ulModHandle = ulModHandle & ~API_STREAM_MASK;
    // mask away the high speed flag
    ulModHandle = ulModHandle & ~API_HS_ACCESS;

    for( px_CurrentEntry  = px_MilDeviceInfoListStart;
        px_CurrentEntry != NULL;
        px_CurrentEntry  = px_CurrentEntry->px_Next )
    {
        if( px_CurrentEntry->x_DeviceInfo.ul_ModuleNo == ulModHandle )
            return &px_CurrentEntry->x_DeviceInfo;
    }

    return NULL;
}


AiBoolean IsTswVersionGreaterOrEqual(TY_DEVICE_INFO* pDevice, AiUInt32 ulMajor, AiUInt32 ulMinor)
{
    AiUInt32 ulDeviceVersion  = 0;
    AiUInt32 ulCompareVersion = 0;

    ulDeviceVersion = (pDevice->ax_VersionInfo.x_TargetSWVer.ul_MajorVer*100) + pDevice->ax_VersionInfo.x_TargetSWVer.ul_MinorVer;

    ulCompareVersion = (ulMajor*100) + ulMinor;


    /* Check if temporary switch is supported */
    if (ulDeviceVersion >= ulCompareVersion)
        return AiTrue;
    else
        return AiFalse;
}

AiBoolean IsDriverVersionGreaterOrEqual(TY_DEVICE_INFO* pDevice, AiUInt32 ulMajor, AiUInt32 ulMinor)
{
    AiUInt32 ulDeviceVersion  = 0;
    AiUInt32 ulCompareVersion = 0;

    ulDeviceVersion = (pDevice->ax_VersionInfo.x_SysDrvVer.ul_MajorVer*100) + pDevice->ax_VersionInfo.x_SysDrvVer.ul_MinorVer;

    ulCompareVersion = (ulMajor*100) + ulMinor;

    if (ulDeviceVersion >= ulCompareVersion)
        return AiTrue;
    else
        return AiFalse;
}

/* EOF */

