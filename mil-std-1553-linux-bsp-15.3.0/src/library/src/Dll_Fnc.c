/* SPDX-FileCopyrightText: 2001-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Dll_Fnc.c
 *
 * Driver communication module of the Application interface Contains the communication specific functions for driver communication.
 * 
 */

//****************************************************
// platform specific specific includes
//****************************************************
#include "net_io.h"
#include "AiOs.h"

#include "Ai_mutex.h"

//****************************************************
#include "Ai_cdef.h"

#include "Ai3910i_def.h"
#include "Ai3910i_fnc.h"
#include "Ai1553_Internal_def.h"
#include "Ai3910_Internal_def.h"

// general includes
//****************************************************
#include <stdio.h>
#include <stdlib.h>

#include "Ai3910i_def.h"

#include "DEFS_V.H"
#include "Dll_Fnc.h" /* needed for VxWorks */
#include "AiOs.h"
#include "hw/ApiHw.h"
static AiChar buf[2000];

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

/* Structure that is used to compute
the average sample value during scope offset determination */
struct scope_sample_values
{
    AiUInt32 num_samples_primary;        /*!< total number of samples on primary channel */
    AiUInt32 sample_sum_primary;         /*!< sum of sample values on primary channel */
    AiUInt32 num_samples_secondary;      /*!< total number of samples on secondary channel */
    AiUInt32 sample_sum_secondary;       /*!< sum of sample values on secondary channel */
};


/*! \brief Measures scope pitch from zero-line for a specific scope mode

This function sets up scope for the requested mode e.g. stub 30V. \n
Then it records a probe of scope data on every channel and computes the average
sample value. \n
These values can then be used to compensate the scope offset from zero-line in further
scope recordings.
\param ulModuleHandle the handle of the module to determine scope offsets for
\param[out] p_primary_offset here the computed scope offset for channel A is stored
\param[out] p_secondary_offset here the computed scope offset for channel B is stored
\param scope_coupling the coupling mode to determine offsets for
\return returns API_OK on success, an appropriate error code otherwise */
AiInt16 DetermineScopeOffsets(AiUInt32 ulModuleHandle, AiInt32* p_primary_offset, AiInt32* p_secondary_offset, AiUInt32 scope_coupling);


/*! \brief Measures and compensates the scope offsets from zero-line

This function will perform measurements in order to determine the scope offsets \n
from zero-line. All necessary scope modes, e.g. stub 3V, stub 30V are measured by this function. \n
The measured offsets will be set on the board in order to compensate the pitch from zero-line
in all further scope recordings.
\param ulModuleHandle handle to device to measure scope offsets for
\param pxOffsets optional pointer where measured offsets will be stored
\return returns API_OK on success, an appropriate error code otherwise */
AiInt16 _ApiCmdScopeOffsetCompMeasureAndSet(AiUInt32 ulModuleHandle, TY_API_SCOPE_OFFSETS* pxOffsets);


/*! \brief Buffer handler function for determining scope offsets

Used by function \ref DetermineScopeOffsets as a scope buffer handler. \n
It will count the total number of samples and the sum of their values. \n 
This can be used to calculate the mean value of the sample values 
\param ul_Module handle of the device 
\param p_ScopeBuffer scope buffer with sample data */
void AI_CALL_CONV scope_offset_handler(AiUInt32 ul_Module, struct API_SCOPE_BUFFER* p_ScopeBuffer);




void v_ExamineRetVal( const char *pc_FctName, AiUInt16 uw_RetVal )
{
    char ac_Buf[200];

	if (uw_RetVal != API_OK)
	{
		sprintf(ac_Buf, "ERROR in function %s: return value is 0x%04X (%s)\n", pc_FctName, uw_RetVal, ApiGetErrorDescription(uw_RetVal));
		DEBUGOUT(DBG_ERROR, "                 ", ac_Buf);

		if ((Aiul_DbgLevel & DBG_ERRORMSG) != 0)
		{
			printf("%s", ac_Buf);
		}
	}
}


//***************************************************************************
//
//   _ApiInitMiscGlobals
//
//***************************************************************************
void _ApiInitMiscGlobals(void)
{
    TY_DEVICE_INFO_LIST * px_CurrentEntry  = NULL;
    int                   j                = 0;

    for( px_CurrentEntry  = px_MilDeviceInfoListStart;
        px_CurrentEntry != NULL;
        px_CurrentEntry  = px_CurrentEntry->px_Next )
    {
        for (j=0; j<2; j++) {
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].xfer.max_cnt     = 0L;
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].xfer.start_addr  = 0L;
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].modif.max_cnt    = 0L; /* tbd */
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].modif.start_addr = 0L; /* tbd */
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].instr.max_cnt    = 0L;
            px_CurrentEntry->x_DeviceInfo.api_bc_glob_fw[j].instr.start_addr = 0L;
        }
    }


#if !defined(_AIM_VME) &&  !defined(_AIM_LVRT) &&  !defined(_AIM_RTX64)
    AiChar                buf[256*2];

    // write the log file into temp dir
    AiOsGetTempPath( szLogFileName, 256 );
    strcat( szLogFileName, "aim_mil.log"); //add filename to path
    sprintf( buf, "Writing trace file to: \"%s\" \n", szLogFileName );
    DEBUGOUT( DBG_INIT, FKT_DLLINIT, buf );
#endif



} /* end: _ApiInitMiscGlobals */


//**************************************************************************
//
//  ApiInit_
//
//**************************************************************************
AiInt16 ApiInit_( void)
{
    _ApiInitMiscGlobals();

    return _ApiOsIni();  // Returns number of modules found
}

AiInt16 ApiExit_( void)
{
    return _ApiOsExit();
}

//**************************************************************************
//
//  _ApiExtInit
//
//**************************************************************************
AiInt16 _ApiExtInit(AiUInt32 bModule, AiUInt32 par1, AiUInt32 par2)
{
    if ( GET_SERVER_ID( bModule))
    {
        return API_ERR_SERVER;
    }
    else
    {
        return _ApiOsExtIni(par1,par2);
    }
} // end: _ApiExtInit





//**************************************************************************
//
//  _ApiOpen
//
//**************************************************************************
AiInt16 _ApiOpen(TY_API_OPEN *px_ApiOpen, AiUInt32 * pulModuleHandle)
{
    AiInt16          wRetVal       = 0;
    TY_DEVICE_INFO*  pDevice = NULL;

    if (!AiOsStringCmp(px_ApiOpen->ac_SrvName, "local"))
    {
        wRetVal = _MilNetOpen(px_ApiOpen, pulModuleHandle);

        if (wRetVal != API_OK)
            return wRetVal;
    }
    else
    {

        // create Module Handle
        *pulModuleHandle = CREATE_MODULE_HANDLE(px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream, 0);

        /* Here we have to determine if ApiOpenEx was called for this
         * opening, or the legacy ApiOpen. For this we determine if stream is 0xFFFFFFFF (old ApiOpen).
         * TODO: remove this with next incompatible API change.
         */
        pDevice = _ApiGetDeviceInfoPtrByModule(*pulModuleHandle);

        if (!pDevice)
        {
            sprintf(buf, "pulModuleHandle %x ...ulModule %x Stream%x\n", *pulModuleHandle, px_ApiOpen->ul_Module, px_ApiOpen->ul_Stream);
            DEBUGOUT(DBG_ERROR, FKT_NTOPEN, buf);
            return API_ERR_WRONG_MODULE;
        }

        pDevice->bOpenExUsed = px_ApiOpen->ul_Stream == 0xFFFFFFFFU ? FALSE : TRUE;

        wRetVal = _ApiOsOpen(*pulModuleHandle);

    }
    
    return wRetVal;

} // end: _ApiOpen

//**************************************************************************
//
//  ApiClose_
//
//**************************************************************************
AiInt16 ApiClose_(AiUInt32 ulModHandle )
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );
    AiInt16          wRetVal = 0;

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if(1 == pDevice->ulOpenCount)
    {
        pDevice->_hExtDrv   = 0;
    }

    if ( GET_SERVER_ID( ulModHandle))
    {
        ai_mutex_lock(pDevice->_hInitMutex);

        wRetVal = _MilNetClose(ulModHandle, AiFalse);

        ai_mutex_release(pDevice->_hInitMutex);
        return wRetVal;
    }
    else
    {

        ai_mutex_lock(pDevice->_hInitMutex);

        wRetVal= _ApiOsClose(ulModHandle);

        ai_mutex_release(pDevice->_hInitMutex);
        return wRetVal;
    }
} // end: _ApiClose


//**************************************************************************
//
//  _ApiCmdSetSysDbgLevel
//
//**************************************************************************
AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel)
{
    if ( GET_SERVER_ID( bModule))
        /* @AIMFIXME: no longer supported */
        return API_ERR_SERVER;
    else
        return(_ApiOsSetSysDbgLevel(bModule&API_MODULE_MASK, ul_DbgLevel));

} // end: _ApiCmdSetDebugLevel



//**************************************************************************
//
//  ApiGetDriverInfo_
//
//**************************************************************************
AiInt16 ApiGetDriverInfo_( AiUInt32 ulModule, TY_API_DRIVER_INFO * pxDriverInfo )
{
    if ( GET_SERVER_ID(ulModule))
        return _MilNetGetDriverInfo( ulModule, pxDriverInfo );
    else
    return _ApiOsGetDriverInfo( ulModule, pxDriverInfo );
}


//**************************************************************************
//
//  _ApiCmdGetSysDrvVersion
//
//**************************************************************************
AiInt16 _ApiCmdGetSysDrvVersion(AiUInt32 handle, TY_VER_INFO * version)
{
	TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(handle);

	if (pDevice == NULL)
		return API_ERR_NO_MODULE_EXTENSION;

	if (GET_SERVER_ID(handle))
	{
		return _MilNetReadVersion(handle, AI_SYS_DRV_VER, version);
	}
    else
    {
        return _ApiOsGetSysDrvVersion( pDevice, version);
    }
} // end: _ApiCmdGetSysDrvVersion


//**************************************************************************
//
//  _ApiIo
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIo(AiUInt32 bModule, AiUInt8 *cmd, AiUInt8 *ack)
{
    if ( GET_SERVER_ID( bModule))
    {
        /* The calling structure has changed with the new ANS */
        /* Call the top level ApiIo to access the server      */
        return API_ERR_SERVER;
    }
    else
    {
        return _ApiOsIo(bModule, cmd, ack);
    }
} // end: _ApiIo



//**************************************************************************
//

//  _ApiExtIo
//
//**************************************************************************
AiInt16 _ApiExtIo(AiUInt32 bModule, AiUInt8 *cmd, AiUInt8 *ack)
{
    if ( GET_SERVER_ID( bModule))
        return API_ERR_SERVER;
    else
        return _ApiOsExtIo(bModule&API_MODULE_MASK, cmd, ack);

} // end: _ApiExtIo




//**************************************************************************
//
//  _SyncWait
//
//**************************************************************************

void _SyncWait( AiUInt32 bModule, AiHandle ah_Mutex )
{
    _ApiOsSyncWait( bModule, ah_Mutex );
} // end: _SyncWait


//**************************************************************************
//
//  _SyncRelease
//
//**************************************************************************
void _SyncRelease( AiUInt32 bModule, AiHandle h_Mutex )
{
    _ApiOsSyncRelease( bModule, h_Mutex );
} // end:_SyncRelease



//**************************************************************************
//
//  _StructureEndianChange
//
//**************************************************************************
void _StructureEndianChange(AiUInt16 strType, LPVOID pData, AiUInt32 strCount)
{
    _ApiOsStructureEndianChange(strType, pData, strCount);
}// end: _StructureEndianChange


//**************************************************************************
//
//  _InstIntHandler
//
//**************************************************************************
AiInt16 _InstIntHandler(AiUInt32 bModule, AiUInt8 biu,
                        AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc )
{
	AiInt16 retVal = 0;
    AiUInt32 ul_Biu;
    TY_API_BOARD_INFO x_Info;
    AiUInt32 ul_Stream = ((bModule & API_STREAM_MASK) >> API_STREAM_POS);

    if( 0 != ul_Stream )
    {
        retVal = ApiGetBoardInfo_( bModule, &x_Info);
        if( retVal ) return 1;

        if( ApiHwBiuIntHsFlag(x_Info.ul_DeviceType, biu, ul_Stream) )
            /* set HS flag in module handle */
            bModule |= API_HS_ACCESS;
    }
    uw_ApiGetBiuFromStream( bModule, biu, &ul_Biu );

    if (GET_SERVER_ID(bModule))
        return _MilNetCallbackRegister(bModule, ul_Biu - 1, uc_Type, pf_IntFunc);
    else
        return _ApiOsInstIntHandler( bModule, ul_Biu-1, uc_Type, pf_IntFunc );
}


//**************************************************************************
//
//  _DelIntHandler
//
//**************************************************************************
AiInt16 _DelIntHandler( AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type )
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ul_Biu;
    TY_API_BOARD_INFO x_Info;
    AiUInt32 ul_Stream = ((bModule & API_STREAM_MASK) >> API_STREAM_POS);

    if( 0 != ul_Stream )
    {
        uw_RetVal = ApiGetBoardInfo_( bModule, &x_Info);
        if( API_OK == uw_RetVal )
        {

            if( ApiHwBiuIntHsFlag(x_Info.ul_DeviceType, biu, ul_Stream) )
                /* set HS flag in module handle */
                bModule |= API_HS_ACCESS;
        }
    }
    uw_ApiGetBiuFromStream( bModule, biu, &ul_Biu );


    if (GET_SERVER_ID(bModule))
        return _MilNetCallbackUnregister(bModule, ul_Biu - 1, uc_Type);
    else
        return _ApiOsDelIntHandler( bModule , ul_Biu - 1, uc_Type );
} // end: _DelIntHandler



//**************************************************************************
//
//  _ApiGetDrvHandle
//
//**************************************************************************
AiHandle _ApiGetDrvHandle( AiUInt32 module)
{
    return _ApiOsGetDrvHandle(module & API_MODULE_MASK);
} // end: ApiGetDrvHandle


//**************************************************************************
//
//  ApiReadMemData
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p)
{
    AiInt16 uw_RetVal;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    uw_RetVal = _ApiReadMemData(bModule, memtype, offset, width, data_p);

    TRACE_BEGIN
        if( width == 1 )
            TRACE("    AiUInt8  uc_Data;\n");
    if( width == 2 )
        TRACE("    AiUInt16 uw_Data;\n");
    if( width == 4 )
        TRACE("    AiUInt32 ul_Data;\n");
    TRACE_FCTA("ApiReadMemData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(memtype);
    TRACE_PARA(offset);
    TRACE_PARA(width);
    if( width == 1 )
        TRACE_RPARE("&uc_Data");
    if( width == 2 )
        TRACE_RPARE("&uw_Data");
    if( width == 4 )
        TRACE_RPARE("&ul_Data");
    TRACE_FCTE;
    TRACE_END

        return( uw_RetVal );
}

AiInt16 _ApiReadMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p)
{
    AiUInt32 ul_Temp;
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (1 != width) && (2 != width) && (4 != width) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (offset % width) != 0 ) /* 30.07.02 */
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == data_p )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        if ( GET_SERVER_ID( bModule))
            uw_RetVal = _MilNetReadMemData(bModule, memtype, offset, width, data_p, 1, &ul_Temp);   // access to board via net 
        else
        {
            uw_RetVal =_ApiOsReadMemData(bModule & API_MODULE_MASK,memtype,offset,width,data_p,1,&ul_Temp);
        }
    }
    return( uw_RetVal );
} // end: ApiReadMemData

//**************************************************************************
//
//  ApiWriteMemData
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p)
{
    AiInt16 uw_RetVal;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    uw_RetVal = _ApiWriteMemData(bModule, memtype, offset, width, data_p);

    TRACE_BEGIN
        if( width == 1 )
        {
            TRACE1("        AiUInt8  uc_Data = %d;\n", *(AiUInt8 *)data_p);
        }
        if( width == 2 )
            TRACE1("        AiUInt16 uw_Data = %d;\n", *(AiUInt16 *)data_p);
        if( width == 4 )
            TRACE1("        AiUInt32 ul_Data = %d;\n", *(AiUInt32 *)data_p);
        TRACE_FCTA("ApiWriteMemData", uw_RetVal); 
        TRACE_PARA(bModule);
        TRACE_PARA(memtype);
        TRACE_PARA(offset);
        TRACE_PARA(width);
        if( width == 1 )
            TRACE_RPARE("&uc_Data");
        if( width == 2 )
            TRACE_RPARE("&uw_Data");
        if( width == 4 )
            TRACE_RPARE("&ul_Data");
        TRACE_FCTE;
        TRACE_END

            return( uw_RetVal );
}

AiInt16 _ApiWriteMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p)
{
    AiUInt32 ul_Temp;
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (1 != width) && (2 != width) && (4 != width) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (offset % width) != 0 ) /* 30.07.02 */
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == data_p )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        if ( GET_SERVER_ID( bModule))
            uw_RetVal = _MilNetWriteMemData(bModule, memtype, offset, width, data_p, 1, &ul_Temp);   // access to board via net 
        else
            uw_RetVal =_ApiOsWriteMemData(bModule & API_MODULE_MASK, memtype, offset, width, data_p, 1,&ul_Temp);
    }
    return( uw_RetVal );
} // end: ApiWriteMemData


//**************************************************************************
//
//  ApiReadBlockMemData
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead )
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    uw_RetVal = _ApiReadBlockMemData(bModule, memtype, offset, width, data_p, size, pul_BytesRead);

    TRACE_BEGIN
        TRACE ("        AiUInt32 ul_BytesRead;\n");
    TRACE1("        AiUInt8  auc_Data[%d];\n", width*size);
    TRACE1("        for( int i=0; i<%d; i++)\n", width*size);
    TRACE ("          auc_Data[i] = 0;\n");
    TRACE_FCTA("ApiReadBlockMemData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(memtype);
    TRACE_PARA(offset);
    TRACE_PARA(width);
    TRACE_RPARA("auc_Data");
    TRACE_PARA(size);
    TRACE_RPARE("&ul_BytesRead");
    TRACE_FCTE;
    TRACE1("    --> ul_BytesRead = %d;\n", *pul_BytesRead);
    TRACE_END

        return( uw_RetVal );
}

AiInt16 _ApiReadBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesRead )
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (1 != width) && (2 != width) && (4 != width) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (offset % width) != 0 ) /* 30.07.02 */
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == data_p )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {

        if ( GET_SERVER_ID( bModule))
            uw_RetVal = _MilNetReadMemData(bModule, memtype, offset, width, data_p, size, pul_BytesRead);   // access to board via net 
        else
            uw_RetVal = _ApiOsReadMemData(bModule & API_MODULE_MASK, memtype, offset, width, data_p, size,pul_BytesRead);
    }
    return( uw_RetVal );
} // end: ApiReadBlockMemData


//**************************************************************************
//
//  ApiWriteBlockMemData
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten )
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    uw_RetVal = _ApiWriteBlockMemData(bModule, memtype, offset, width, data_p, size, pul_BytesWritten);

    TRACE_BEGIN
        TRACE ("        AiUInt32 ul_BytesWritten;\n");
    TRACE1("        AiUInt8  auc_Data[%d];\n", width*size);
    TRACE1("        for( int i=0; i<%d; i++)\n", width*size);
    TRACE ("          auc_Data[i] = <VALUE>;\n");
    TRACE_FCTA("ApiWriteBlockMemData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(memtype);
    TRACE_PARA(offset);
    TRACE_PARA(width);
    TRACE_RPARA("auc_Data");
    TRACE_PARA(size);
    TRACE_RPARE("&ul_BytesWritten");
    TRACE_FCTE;
    TRACE1("    --> ul_BytesWritten = %d;\n", *pul_BytesWritten);
    TRACE_END

        return( uw_RetVal );
}

AiInt16 _ApiWriteBlockMemData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width, void* data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten )
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if( (1 != width) && (2 != width) && (4 != width) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (offset % width) != 0 ) /* 30.07.02 */
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == data_p )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        if ( GET_SERVER_ID( bModule))
            uw_RetVal = _MilNetWriteMemData(bModule, memtype, offset, width, data_p, size, pul_BytesWritten);   // access to board via net 
        else
            uw_RetVal = _ApiOsWriteMemData(bModule & API_MODULE_MASK, memtype, offset, width, data_p, size,pul_BytesWritten);
    }

    return( uw_RetVal );
} // end: ApiWriteBlockMemData



//**************************************************************************
//
//  ApiStartPerformanceTimer
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiStartPerformanceTimer(AiUInt32 bModule)
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if ( GET_SERVER_ID( bModule))
        /* @AIMFIXME: no longer supported */
        return API_ERR_SERVER;
    else
    {
		return _ApiOsStartPerformanceTimer(bModule & API_MODULE_MASK);
    }

    return API_OK;
} // end: _ApiStartPerformanceTimer


AiInt16 ApiGetDeviceConfig_( AiUInt32 ulModHandle, AiUInt8 * pucAnsUsed, TY_API_DEVICE_CONFIG * pxConfig )
{
    if( pucAnsUsed ) *pucAnsUsed = 0;

    if ( GET_SERVER_ID( ulModHandle))
    {
        if( pucAnsUsed ) *pucAnsUsed = 1;

        return _MilNetGetDeviceConfig( ulModHandle, pxConfig );
    }
    else
    {
        return API_OK;
    }
}

AiInt16 ApiSetDeviceConfig_( AiUInt32 ulModHandle, AiUInt8 * pucAnsUsed, TY_API_DEVICE_CONFIG * pxConfig )
{
    if( pucAnsUsed ) *pucAnsUsed = 0;

    if ( GET_SERVER_ID( ulModHandle))
    {
        if( pucAnsUsed ) 
            *pucAnsUsed = 1;
        return _MilNetSetDeviceConfig( ulModHandle, pxConfig );
    }
    else
    {
        return API_OK;
    }
}


AiInt16 _ApiSetDeviceConfigDriverFlags( AiUInt32 ulModHandle, AiUInt32 ulDriverFlags )
{
    if ( GET_SERVER_ID( ulModHandle))
    {
        // This function is already called within _MilNetSetDeviceConfig
        return API_OK;
    }
    else
    {
        return _ApiOsSetDeviceConfigDriverFlags(ulModHandle, ulDriverFlags);
    }
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadUsbData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesRead)
{
    AiInt16 w_RetVal = API_ERR;
    AiUInt32 bytesRead = 0;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if(data == NULL)
        return API_ERR_PARAM5_IS_NULL;

    /* This IOCTL is only available with USB devices */
    /* Now call OS specific function that will forward task to system driver */
    if ( GET_SERVER_ID( bModule))
        w_RetVal = API_ERR_SERVER;
    else
        w_RetVal = _ApiReadUsbData( bModule, memtype, offset, size, data, &bytesRead);

    if(p_BytesRead != NULL)
        *p_BytesRead = bytesRead;

    return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteUsbData(AiUInt32 bModule, AiUInt8 memtype, AiUInt32 offset, AiUInt32 size, void* data, AiUInt32* p_BytesWritten)
{
    AiInt16 w_RetVal = API_ERR;
    AiUInt32 bytesWritten = 0;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if(data == NULL)
        return API_ERR_PARAM5_IS_NULL;

    /* This IOCTL is only available with USB devices */

    /* Now call OS specific function that will forward task to system driver */
    if ( GET_SERVER_ID( bModule))
        /* @AIMFIXME: no longer supported */
        w_RetVal = API_ERR_SERVER;
    else
        w_RetVal = _ApiWriteUsbData( bModule, memtype, offset, size, data, &bytesWritten);

    if(p_BytesWritten != NULL)
        *p_BytesWritten = bytesWritten;

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeSetup(AiUInt32 ul_ModuleHandle, TY_API_SCOPE_SETUP *px_ScopeSetup)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ul_ModuleHandle);
    AiUInt8 auc_Info[24];


#if  !defined(_AIM_LVRT)
    if(API_OK == uw_RetVal)
    {
        // allocate scope setup structure in devicelist
        if(!pDevice->px_ScopeSetup)
        {
            pDevice->px_ScopeSetup = (TY_API_SCOPE_SETUP*)AiOsMalloc(sizeof(TY_API_SCOPE_SETUP));
            if(!pDevice->px_ScopeSetup)
            {
                return API_ERR_MALLOC_FAILED;
            }
        }

        // write Scope setup to device list for use with scope buffer functions
        memcpy(pDevice->px_ScopeSetup, px_ScopeSetup, sizeof(TY_API_SCOPE_SETUP));

        /*
        This use might be invalid with ANS. Middle layer _ApiCmdScopeSetup
        might be neccessary.
        */
        uw_RetVal = _ApiOsCmdScopeSetup(ul_ModuleHandle);
        if(uw_RetVal != API_OK)
        {
            return uw_RetVal;
        }

        auc_Info[0]  = (AiUInt8)(px_ScopeSetup->ul_CouplingPri >> 24);
        auc_Info[1]  = (AiUInt8)(px_ScopeSetup->ul_CouplingPri >> 16);
        auc_Info[2]  = (AiUInt8)(px_ScopeSetup->ul_CouplingPri >> 8);
        auc_Info[3]  = (AiUInt8)px_ScopeSetup->ul_CouplingPri;
        auc_Info[4]  = (AiUInt8)(px_ScopeSetup->ul_CouplingSec >> 24);
        auc_Info[5]  = (AiUInt8)(px_ScopeSetup->ul_CouplingSec >> 16);
        auc_Info[6]  = (AiUInt8)(px_ScopeSetup->ul_CouplingSec >> 8);
        auc_Info[7]  = (AiUInt8)px_ScopeSetup->ul_CouplingSec;
        auc_Info[8]  = (AiUInt8)(px_ScopeSetup->ul_SamplingRate >> 24);
        auc_Info[9]  = (AiUInt8)(px_ScopeSetup->ul_SamplingRate >> 16);
        auc_Info[10] = (AiUInt8)(px_ScopeSetup->ul_SamplingRate >> 8);
        auc_Info[11] = (AiUInt8)px_ScopeSetup->ul_SamplingRate;
        auc_Info[12] = (AiUInt8)(px_ScopeSetup->ul_CaptureMode >> 24);
        auc_Info[13] = (AiUInt8)(px_ScopeSetup->ul_CaptureMode >> 16);
        auc_Info[14] = (AiUInt8)(px_ScopeSetup->ul_CaptureMode >> 8);
        auc_Info[15] = (AiUInt8)px_ScopeSetup->ul_CaptureMode;
        auc_Info[16] = (AiUInt8)(px_ScopeSetup->ul_OperationMode >> 24);
        auc_Info[17] = (AiUInt8)(px_ScopeSetup->ul_OperationMode >> 16);
        auc_Info[18] = (AiUInt8)(px_ScopeSetup->ul_OperationMode >> 8);
        auc_Info[19] = (AiUInt8)px_ScopeSetup->ul_OperationMode;
        auc_Info[20] = (AiUInt8)(px_ScopeSetup->ul_SingleShotBuf >> 24);
        auc_Info[21] = (AiUInt8)(px_ScopeSetup->ul_SingleShotBuf >> 16);
        auc_Info[22] = (AiUInt8)(px_ScopeSetup->ul_SingleShotBuf >> 8);
        auc_Info[23] = (AiUInt8)px_ScopeSetup->ul_SingleShotBuf;

        uw_RetVal = ApiIo( ul_ModuleHandle & ~API_STREAM_MASK, 0, TG_API_SCOPE_SETUP, TG_ACK,
            &auc_Info[0], 24, w_p, 0,
            b_p, &tg_os, w_p, &tg_os,
            lw_p, &tg_os);
    }
#endif

    return (uw_RetVal);
}

AiInt16 _API_DLL_FUNC _ApiCmdScopeSetup(AiUInt32 ulModuleHandle, TY_API_SCOPE_SETUP *px_ScopeSetup)
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if ( GET_SERVER_ID( ulModuleHandle)) {
        w_RetVal = _NetCmdScopeSetup( ulModuleHandle, px_ScopeSetup);

        if (API_OK == w_RetVal) {
            // allocate scope setup structure in devicelist
            if(!pDevice->px_ScopeSetup)
            {
                pDevice->px_ScopeSetup = (TY_API_SCOPE_SETUP*)AiOsMalloc(sizeof(TY_API_SCOPE_SETUP));
                if(!pDevice->px_ScopeSetup)
                {
                    return API_ERR_MALLOC_FAILED;
                }
            }

            // write Scope setup to device list for use with scope buffer functions
            memcpy(pDevice->px_ScopeSetup, px_ScopeSetup, sizeof(TY_API_SCOPE_SETUP));
        }

        w_RetVal = _ApiOsCmdScopeSetup(ulModuleHandle);
        if(w_RetVal != API_OK)
        {
            return w_RetVal;
        }


    }

    w_RetVal = _ApiIoCmdScopeSetup( ulModuleHandle, px_ScopeSetup);

    return w_RetVal;
}


/*! \brief OS specific function that creates a scope buffer

This function creates a scope buffer of type \ref TY_API_SCOPE_BUFFER ,
that can be used for recording scope data. \n
It has to be freed with \ref ApiFreeScopeBuffer when no more needed. \n
A user specific ID, a handler function that is called when the buffer is completed or cancelled
and a user definable data pointer can be specified for the buffer..
\param ulModuleHandle handle to the device to create scope buffer for
\param ulID ID of the scope buffer
\param eBufferType the buffer's type according to \ref TY_API_SCOPE_BUFFER_TYPE enumeration.
\param BufferHandler pointer to buffer handler function to set for the buffer.
\param pvUserData pointer to user definable data that can be used in buffer handler
\return returns API_OK on success,  an appropriate error code otherwise */

AI_LIB_FUNC TY_API_SCOPE_BUFFER* AI_CALL_CONV _ApiCreateScopeBuffer(AiUInt32 ulModuleHandle, AiUInt32 ulID, TY_API_SCOPE_BUFFER_TYPE eBufferType,
                                                                     TY_API_SCOPE_BUFFER_HANDLER BufferHandler, void* pvUserData)
{
    /*
    This function to create buffer for scope data will be executed allways local.
    When ProvideScopeBuffers is executed new buffers have to be created on server.
    Data are transfered by RPC callback between local and remote buffer.  
    */

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );
    TY_API_SCOPE_BUFFER * px_Buffer = NULL;
    AiUInt32 ul_BufferSize = 0;

    if( pDevice != NULL )
    {
        switch (pDevice->px_ScopeSetup->ul_OperationMode)
        {
        case API_SCOPE_OPR_CHANNEL_A:
        case API_SCOPE_OPR_CHANNEL_B:
        case API_SCOPE_OPR_CHANNEL_A100:
        case API_SCOPE_OPR_CHANNEL_B100:
            ul_BufferSize = API_SCOPE_SIZE_SINGLE_CH_REC;
            break;
        case API_SCOPE_OPR_CHANNEL_AB:
            ul_BufferSize = API_SCOPE_SIZE_DUAL_CH_REC;
            break;
        default:
            px_Buffer = NULL;
            ul_BufferSize = 0;
            break;
        }

        if (ul_BufferSize > 0) {
            // allocate buffer structure
            px_Buffer = (TY_API_SCOPE_BUFFER*)AiOsMalloc( sizeof(TY_API_SCOPE_BUFFER));
            if(px_Buffer == NULL)
            {
                DEBUGOUT(DBG_ERROR,"_ApiOsCreateScopeBuffer", "Allocation of scope buffer failed\n");
                return NULL;
            }

            // init buffer structure contents to 0
            memset(px_Buffer, 0, sizeof(TY_API_SCOPE_BUFFER));

            // allocate buffer
            px_Buffer->pvBuffer = AiOsMalloc( ul_BufferSize);

            if (NULL == px_Buffer->pvBuffer) {
                DEBUGOUT(DBG_ERROR,"_ApiOsCreateScopeBuffer", "Allocation of scope buffer failed\n");
                AiOsFree( px_Buffer);
                px_Buffer = NULL;
            }
        }

        // initialize buffer structure data
        if (NULL != px_Buffer) {
            px_Buffer->ulBufferSize = ul_BufferSize;
            px_Buffer->eBufferType = eBufferType;
            px_Buffer->ulID = ulID;
            px_Buffer->BufferHandler = BufferHandler;
            px_Buffer->pvUserData = pvUserData;
        }
    }

    return px_Buffer;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiFreeScopeBuffer(AiUInt32 ulModuleHandle, TY_API_SCOPE_BUFFER *px_Buffer)
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    /*
    This function to free buffer for scope data will be executed allways local.
    Free of buffers on server has do be done by another automatic mechanism. 
    */


    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if (NULL != px_Buffer) {
        if (NULL != px_Buffer->pvBuffer) {
            AiOsFree(px_Buffer->pvBuffer);
            px_Buffer->pvBuffer = NULL;
        }
        AiOsFree(px_Buffer);
        px_Buffer = NULL;
    }

    w_RetVal = API_OK;

    return w_RetVal;

}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStart(AiUInt32 ul_ModuleHandle)
{
    AiInt16 w_RetVal = API_OK;

    TY_MIL_COM_SCOPE_START_INPUT   xInput;
    TY_MIL_COM_ACK                 xOutput;

    TY_DEVICE_INFO  * pDevice = NULL;

    pDevice = _ApiGetDeviceInfoPtrByModule( ul_ModuleHandle);

    if( NULL == pDevice)
        return API_ERR_NO_MODULE_EXTENSION;

    if( NULL == pDevice->px_ScopeSetup)
        return API_ERR_NO_SCOPE_EXECUTED;

    if( (ul_ModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE )
        w_RetVal = API_ERR_WRONG_MODULE;
    else
    {

        w_RetVal = _ApiOsCmdScopeStart(ul_ModuleHandle);
        if(w_RetVal != API_OK)
        {
            return w_RetVal;
        }

        /* -- initialize command --- */
        MIL_COM_INIT( &xInput.xCommand, API_STREAM(ul_ModuleHandle), 0, MIL_COM_SCOPE_START, sizeof(xInput), sizeof(xOutput) );

        /* get values for ulMode and ulNoOfCycles */
        xInput.ulMode       = pDevice->px_ScopeSetup->ul_CaptureMode;
        if ( API_SCOPE_START_CONTINUOUS == xInput.ulMode)
        {
            xInput.ulNoOfCycles = 0;     // infinite
        }
        else // API_SCOPE_START_SINGLE
        {
            if (0 == pDevice->ulNumBuffersLeft)
                return API_ERR_NUMBUFFERSLEFT_INVALID;

            switch (pDevice->px_ScopeSetup->ul_OperationMode)
            {
            case API_SCOPE_OPR_CHANNEL_AB:   // dual channel
                xInput.ulNoOfCycles = pDevice->ulNumBuffersLeft / 4;
                if (0 != (pDevice->ulNumBuffersLeft % 4))
                    return API_ERR_NUMBUFFERSLEFT_INVALID;
                break;
            default:                         // all other Operation Modes (single channel, disabled)
                xInput.ulNoOfCycles = pDevice->ulNumBuffersLeft / 2;
                if (0 != (pDevice->ulNumBuffersLeft % 2))
                    return API_ERR_NUMBUFFERSLEFT_INVALID;
                break;
            }
        }

        w_RetVal = ApiIoStruct( ul_ModuleHandle, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );
    }

    v_ExamineRetVal( "_ApiCmdScopeStart", w_RetVal );

    return( w_RetVal );
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStart( AiUInt32 ulModuleHandle )
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if ( GET_SERVER_ID( ulModuleHandle)) {
        // check, if requested number of buffers available on server
        AiUInt32 loopcount = 0;
        AiUInt32 num_buffers;
        AiInt32 difference = 0, last_difference = 0;
        TY_API_SCOPE_STATUS status;
        // Before start check avaiable number of buffers on client and server.
        // (Takes some time until scope buffers are created on server because
        // asynchronous RPC function used. This function returns immediatelly
        // and buffer might be not available on server.)
        // 
        // Loopcount is used for timeout. 
        // When numbers of buffers on server increases (checked by reading 
        // ApiCmdScopeStatus() ) the timeout value "loopcount" is not increased.
        // So it works also for big numbers of buffers.
        do 
        {
            w_RetVal = ApiCmdScopeStatus(ulModuleHandle, &status, &num_buffers);
            difference = pDevice->ulNumBuffersLeft - num_buffers;
            if (difference > 0) {
                AiOsSleep(2);  // the sleep is to give some time to async functions. 
            }
            if (difference >= last_difference) {
                loopcount++;
            }
            last_difference = difference;
        } 
        while((difference > 0) && (loopcount<RPC_TIMEOUT_NUMBER_OF_BUFFERS_SYNC));  
        if (loopcount >= RPC_TIMEOUT_NUMBER_OF_BUFFERS_SYNC) {
            w_RetVal = API_ERR_TIMEOUT;
        }

        if (w_RetVal == API_OK) {
            w_RetVal = _NetCmdScopeStart( ulModuleHandle);

            if(pDevice->scopeThreadState == ScopeThreadSuspended)
            {
                pDevice->scopeThreadState = ScopeThreadStarted;
#ifdef _AIM_WINDOWS
                SetEvent(pDevice->hScopeThreadWakeEvent);
#endif
            }
        }

    }
    else
    {
        w_RetVal = _ApiIoCmdScopeStart( ulModuleHandle);
    }

    return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStatus(AiUInt32 ulModuleHandle, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft)
{
    AiInt16 ret_val = API_OK;
    TY_MIL_COM xInput;
    TY_MIL_COM_SCOPE_STATUS_OUTPUT xOutput;

    TY_DEVICE_INFO  * pDevice = NULL;

    if((ulModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE)
    {
        return API_ERR_WRONG_MODULE;
    }

    if(p_ScopeStatus == NULL)
    {
        return API_ERR_PARAM2_IS_NULL;
    }

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle);

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT(&xInput, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_STATUS, sizeof(xInput), sizeof(xOutput));

    ret_val = ApiIoStruct(ulModuleHandle, &xInput, (TY_MIL_COM_ACK*) &xOutput);
    if(ret_val != API_OK)
    {
        return ret_val;
    }

    *p_ScopeStatus = xOutput.eScopeStatus;

    ret_val = _ApiOsCmdScopeStatus( ulModuleHandle, pulNumBuffersLeft );

    return ret_val;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStatus(AiUInt32 ulModuleHandle, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft)
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if ( GET_SERVER_ID( ulModuleHandle))
        w_RetVal = _NetCmdScopeStatus( ulModuleHandle, p_ScopeStatus, pulNumBuffersLeft);
    else
        w_RetVal = _ApiIoCmdScopeStatus( ulModuleHandle, p_ScopeStatus, pulNumBuffersLeft);

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiIoCmdScopeStop(AiUInt32 ulModuleHandle)
{
    AiInt16 ret_val = API_OK;
    TY_MIL_COM xInput;
    TY_MIL_COM_ACK xOutput;

    TY_DEVICE_INFO  * pDevice = NULL;

    if((ulModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE)
    {
        return API_ERR_WRONG_MODULE;
    }

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle);

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT(&xInput, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_STOP, sizeof(xInput), sizeof(xOutput));

    ret_val = ApiIoStruct(ulModuleHandle, &xInput, &xOutput);

    if (API_OK == ret_val)
    {
        ret_val = _ApiOsCmdScopeStop(ulModuleHandle);
    }

    return ret_val;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeStop(AiUInt32 ulModuleHandle)
{
    AiInt16 ret_val = API_OK;

    TY_DEVICE_INFO  * pDevice = NULL;

    if((ulModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE)
    {
        return API_ERR_WRONG_MODULE;
    }

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle); 

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

#ifdef _AIM_WINDOWS    
    if ( GET_SERVER_ID( ulModuleHandle)) {
        TY_SCOPE_BUFFER_LIST_ELEMENT *px_currScopeBufferListElem;

        ret_val = _NetCmdScopeStop( ulModuleHandle);

        // This sections waits for all buffers canceled. 
        // Specially when buffers without data are defined and scope was stopped.
        // Code section copied from _ApiOsCmdScopeStop() for executing on client.
        {
            DWORD wait_result = 0;
            char* szDriverErrorFormated = NULL;
            char szDriverError[256] = {0};

            if(!pDevice->p_ScopeThreadStateLock
                || !pDevice->hScopeThreadSuspendEvent)
            {
                // Propably ApiCmdScopeSetup was not called so far. 
                // Then continue without action.
            }
            else
            {

                /* Wait until all buffers have been canceled */
                EnterCriticalSection(pDevice->p_ScopeThreadStateLock);

                while(pDevice->scopeThreadState == ScopeThreadStarted)
                {
                    ResetEvent(pDevice->hScopeThreadSuspendEvent); /* This reset is necessary so we don't catch a previous signal that was not waited for */
                    LeaveCriticalSection(pDevice->p_ScopeThreadStateLock);
                    wait_result = WaitForSingleObject(pDevice->hScopeThreadSuspendEvent, INFINITE);
                    if(wait_result != WAIT_OBJECT_0)
                    {
                        sprintf( szDriverError, "Waiting for scope thread suspension event failed: error code 0x%08X\n", GetLastError() );
                        DEBUGOUT( DBG_ERROR, "_ApiOsCmdScopeStop", szDriverError );
                        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL,GetLastError(),0, (LPTSTR) &szDriverErrorFormated,0,NULL);
                        DEBUGOUT( DBG_ERROR, "_ApiOsCmdScopeStop", szDriverErrorFormated );
                        LocalFree(szDriverErrorFormated);
                        return API_ERR; 
                    }
                    EnterCriticalSection(pDevice->p_ScopeThreadStateLock);
                }

                LeaveCriticalSection(pDevice->p_ScopeThreadStateLock);
            }
        }


        // protect scope buffer when cleaning buffer list  
        EnterCriticalSection( &pDevice->ScopeBuffListCriticalSect);  // enabled 20121122

        // remove buffers from device list 
        while (pDevice->ulNumBuffersLeft > 0)
        {
            if (NULL != pDevice->px_ScopeBufferLHead)
            {
                px_currScopeBufferListElem = pDevice->px_ScopeBufferLHead;

                // set pointer to next element
                pDevice->px_ScopeBufferLHead = px_currScopeBufferListElem->p_Next;

                AiOsFree(px_currScopeBufferListElem);

                pDevice->ulNumBuffersLeft--;
            }
            else
            {
                pDevice->ulNumBuffersLeft = 0;
            }

        }
        pDevice->px_ScopeBufferLHead = NULL;
        pDevice->px_ScopeBufferLTail = NULL;

        LeaveCriticalSection( &pDevice->ScopeBuffListCriticalSect);  // enabled 20121122

    }
    else
#endif        
        ret_val = _ApiIoCmdScopeStop( ulModuleHandle);

    return ret_val;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiProvideScopeBuffers(AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER *ax_ScopeBuffers[])
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if ( GET_SERVER_ID( ulModuleHandle)) {
        w_RetVal = _NetProvideScopeBuffers( ulModuleHandle, ulNumBuffers, ax_ScopeBuffers);
    }
    else
    {
        w_RetVal = _ApiOsProvideScopeBuffers( ulModuleHandle, ulNumBuffers, ax_ScopeBuffers, NULL);
    }

    return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiWaitForScopeFinished(AiUInt32 ulModuleHandle, AiInt32 lTimeOut,
                                                           TY_API_SCOPE_WAIT_STATE* p_WaitResultFlags)
{

    if(!p_WaitResultFlags)
    {
        return API_ERR_PARAM3_IS_NULL;
    }

    return _ApiOsWaitForScopeFinished(ulModuleHandle, lTimeOut, p_WaitResultFlags);
}


/*! \brief Define Scope Trigger scope

\param ulModuleHandle handle to the device to reset scope on
\param px_ScopeTrgStrobe trigger strobe definitions
\return returns API_OK on success, an appropriate error code otherwise */

AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeTriggerStrobeDef( AiUInt32 ulModuleHandle, TY_API_SCOPE_TRG_STROBE *px_ScopeTrgStrobe)
{
    AiInt16 w_RetVal = API_OK;

    TY_MIL_COM_SCOPE_TRG_STROBE_INPUT   xInput;
    TY_MIL_COM_ACK                     xOutput;

    TY_DEVICE_INFO  * pDevice = NULL;

    pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle);

    if( NULL == pDevice)
        return API_ERR_NO_MODULE_EXTENSION;

    if( (ulModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE )
        w_RetVal = API_ERR_WRONG_MODULE;
    else
    {
        /* -- initialize command --- */
        MIL_COM_INIT( &xInput.xCommand, 0, 0, MIL_COM_SCOPE_TRIG_STROBE, sizeof(xInput), sizeof(xOutput) );

        /* get values for ulMode and ulNoOfCycles */
        xInput.xTriggerStrobeDef.ul_TrgStrobeDest = px_ScopeTrgStrobe->ul_TrgStrobeDest;
        xInput.xTriggerStrobeDef.ul_TrgStrobeType = px_ScopeTrgStrobe->ul_TrgStrobeType;

        w_RetVal = ApiIoStruct( ulModuleHandle, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );
    }

    v_ExamineRetVal( "_ApiCmdScopeTriggerStrobeDef", w_RetVal );

    return( w_RetVal );
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCmdScopeReset(AiUInt32 ulModuleHandle)
{
    AiInt16 w_RetVal = API_ERR;
    TY_MIL_COM xCommand = {0};
    TY_MIL_COM_ACK xOutput = {0};

    if( (ulModuleHandle & API_MODULE_MASK) >= MAX_API_MODULE )
    {
        w_RetVal = API_ERR_WRONG_MODULE;
    }

    /* Stop scope first */
    w_RetVal = ApiCmdScopeStop(ulModuleHandle);
    if(w_RetVal != API_OK)
    {
        return w_RetVal;
    }

    /* -- initialize reset command --- */
    MIL_COM_INIT( &xCommand, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_RESET, sizeof(xCommand), sizeof(xOutput) );

    w_RetVal = ApiIoStruct( ulModuleHandle, &xCommand, &xOutput );

    v_ExamineRetVal( "_ApiCmdScopeReset", w_RetVal );

    return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV _ApiCreateScopeBufferList(AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* paxBufferList[], 
                                                            TY_API_SCOPE_BUFFER_HANDLER pBufferHandler, void* pvUserData)
{
    TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);
    AiBoolean dual_channel = FALSE;
    AiUInt32 i;

    if(pDevice == NULL)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    if(pDevice->px_ScopeSetup == NULL)
    {
        return API_ERR_NO_SCOPE_EXECUTED;
    }

    switch(pDevice->px_ScopeSetup->ul_OperationMode)
    {
    case API_SCOPE_OPR_CHANNEL_A:
    case API_SCOPE_OPR_CHANNEL_B:
    case API_SCOPE_OPR_CHANNEL_A100:
    case API_SCOPE_OPR_CHANNEL_B100:
        dual_channel = FALSE;
        break;
    case API_SCOPE_OPR_CHANNEL_AB:
        dual_channel = TRUE;
        break;
    default:
        return API_ERR_INVALID_MODE;
    }

    /* In single channel mode we need a multiple of 2 scope buffers,
    in dual channel mode a multiple of 4 */
    if(!dual_channel && (ulNumBuffers % 2))
    {
        return API_ERR_PARAM2_NOT_IN_RANGE;
    }
    else if(dual_channel && (ulNumBuffers % 4))
    {
        return API_ERR_PARAM2_NOT_IN_RANGE;
    }

    for(i = 0; i < ulNumBuffers; i++)
    {
        if(dual_channel && (i % 2))
        {
            /* In dual-channel mode, the buffers have to be created in alternating order starting with primary channel buffer.
            So every odd buffer is for second channel */
            paxBufferList[i] = ApiCreateScopeBuffer(ulModuleHandle, i, SCOPE_BUFFER_SECONDARY, pBufferHandler, pvUserData);
        }
        else
        {
            paxBufferList[i] = ApiCreateScopeBuffer(ulModuleHandle, i, SCOPE_BUFFER_PRIMARY, pBufferHandler, pvUserData);
        }

        if(paxBufferList[i] == NULL)
        {
            return API_ERR_MALLOC_FAILED;
        }
    }

    return API_OK;
}


AiInt16 DetermineScopeOffsets(AiUInt32 ulModuleHandle, AiInt32* p_primary_offset, AiInt32* p_secondary_offset, AiUInt32 scope_coupling)
{
    struct scope_sample_values scope_offset_result = {0};
    TY_API_SCOPE_SETUP xScopeSsetup = {0};
    AiInt16 return_val = API_ERR;
    TY_API_SCOPE_TRG xScopeTrigger = {0};
    TY_API_SCOPE_BUFFER* apx_ScopeBuffers[4] = {0,0,0,0};
    TY_API_SCOPE_WAIT_STATE wait_result = SCOPE_WAIT_TIMEOUT;

    int i = 0;

    if(!p_primary_offset || !p_secondary_offset)
    {
        return API_ERR;
    }

    do
    {
        /* Set up Scope */
        xScopeSsetup.ul_CaptureMode = API_SCOPE_START_SINGLE;
        xScopeSsetup.ul_CouplingPri = scope_coupling;
        xScopeSsetup.ul_CouplingSec = scope_coupling;
        xScopeSsetup.ul_OperationMode = API_SCOPE_OPR_CHANNEL_AB;
        xScopeSsetup.ul_SamplingRate = API_SCOPE_RATE_EVERY;

        return_val = ApiCmdScopeSetup(ulModuleHandle, &xScopeSsetup);
        if(return_val != API_OK)
        {
            break;
        }

        xScopeTrigger.ul_TrgMode = API_SCOPE_MODE_GREATER_THAN;
        xScopeTrigger.ul_TrgSrc = API_SCOPE_SRC_PRIMARY;
        xScopeTrigger.ul_TrgValue = 0;
        xScopeTrigger.ul_TrgFlags = 1;

        return_val = ApiCmdScopeTriggerDef(ulModuleHandle, &xScopeTrigger);
        if(return_val != API_OK)
        {
            break;
        }

        /* Create scope buffers with handler that counts number of samples and
        the sum of their values */
        return_val = ApiCreateScopeBufferList(ulModuleHandle, 4, apx_ScopeBuffers, scope_offset_handler, &scope_offset_result);
        if(return_val != API_OK)
        {
            break;
        }

        return_val = ApiProvideScopeBuffers(ulModuleHandle, 4, apx_ScopeBuffers);
        if(return_val != API_OK)
        {
            break;
        }

        /* Wait until coupling has settled (The relays need some time to adopt settings) */
        AiOsSleep(50);

        return_val = ApiCmdScopeStart(ulModuleHandle);
        if(return_val != API_OK)
        {
            break;
        }

        return_val = ApiWaitForScopeFinished(ulModuleHandle, 5000, &wait_result);

        if(return_val != API_OK)
        {
            break;
        }

        if(wait_result & SCOPE_WAIT_OVERFLOW)
        {
            return_val = API_ERR_OVERFLOW;
            break;
        }
        else if(wait_result & SCOPE_WAIT_TIMEOUT)
        {
            return_val = API_ERR_TIMEOUT;
            break;
        }
        else if(wait_result & SCOPE_WAIT_FINISHED)
        {
            return_val = API_OK;
        }

    }while(0);

    if(return_val != API_OK)
    {
        ApiCmdScopeStop(ulModuleHandle);
        return return_val;
    }

    /* Calculate mean values of the recorded data */
    *p_primary_offset =   ((AiInt32)((scope_offset_result.sample_sum_primary / scope_offset_result.num_samples_primary) - 512) * -1);
    *p_secondary_offset = ((AiInt32)((scope_offset_result.sample_sum_secondary / scope_offset_result.num_samples_secondary) - 512) * -1);

    for(i = 0; i < 4; i++)
    {
        ApiFreeScopeBuffer(ulModuleHandle,apx_ScopeBuffers[i]);
    }

    return return_val;
}


void AI_CALL_CONV scope_offset_handler(AiUInt32 ul_Module, struct API_SCOPE_BUFFER* p_ScopeBuffer)
{
    unsigned int i = 0;
    struct scope_sample_values* p_ScopeOffsetResults = NULL;

    if(!p_ScopeBuffer || !(p_ScopeBuffer->ulFlags & SCOPE_BUFFER_FLAG_FILLED))
    {
        return;
    }

    if(!p_ScopeBuffer->pvUserData)
    {
        return;
    }

    p_ScopeOffsetResults = (struct scope_sample_values*) p_ScopeBuffer->pvUserData;

    for(i = 0; i < NUM_SCOPE_SAMPLES(p_ScopeBuffer->ulDataSize); i++)
    {
        if(p_ScopeBuffer->eBufferType == SCOPE_BUFFER_PRIMARY)
        {
            p_ScopeOffsetResults->num_samples_primary++;
            p_ScopeOffsetResults->sample_sum_primary += SCOPE_SAMPLE_BY_SAMPLE_ID(p_ScopeBuffer->pvBuffer,i);
        }
        else if(p_ScopeBuffer->eBufferType == SCOPE_BUFFER_SECONDARY)
        {
            p_ScopeOffsetResults->num_samples_secondary++;
            p_ScopeOffsetResults->sample_sum_secondary += SCOPE_SAMPLE_BY_SAMPLE_ID(p_ScopeBuffer->pvBuffer,i);
        }
    }
}


AiInt16 _ApiCmdScopeOffsetCompensation(AiUInt32 ulModuleHandle, AiUInt8 ucMode, TY_API_SCOPE_OFFSETS* pxOffsets)
{
    TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);
    TY_MIL_COM_SCOPE_OFFSET_COMP x_ScopeOffsetCompCmd;
    TY_MIL_COM_SCOPE_OFFSET_COMP_OUT x_ScopeOffsetCompAck;
    AiInt16 w_RetVal = API_ERR;

    memset( &x_ScopeOffsetCompCmd, 0, sizeof(x_ScopeOffsetCompCmd) );
    memset( &x_ScopeOffsetCompAck, 0, sizeof(x_ScopeOffsetCompAck) );

    if(pDevice == NULL)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    /* We expect at least target software versio 20.50
    as MIL_COM_SCOPE_OFFSET_COMP command is only available since this version */
    if(!IsTswVersionGreaterOrEqual(pDevice,20,50))
    {
        return API_ERR_WRONG_TSW_VERSION;
    }

    MIL_COM_INIT(&x_ScopeOffsetCompCmd.xCommand, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_OFFSET_COMP, sizeof(x_ScopeOffsetCompCmd), sizeof(x_ScopeOffsetCompAck));

    switch(ucMode)
    {
    case API_SCOPE_OFFSET_COMP_RESET:
        /* Issue 'set offsets' target software command to set all offsets to 0 */
        x_ScopeOffsetCompCmd.ulMode = SCOPE_SET_OFFSETS;
        memset(&x_ScopeOffsetCompCmd.scope_offsets, 0, sizeof(TY_API_SCOPE_OFFSETS));
        w_RetVal = ApiIoStruct(ulModuleHandle, (TY_MIL_COM*) &x_ScopeOffsetCompCmd, (TY_MIL_COM_ACK*) &x_ScopeOffsetCompAck);
        if(w_RetVal == API_OK && pxOffsets != NULL)
        {
            /* return the current settings to the user's structure if available */
            memcpy(pxOffsets, &x_ScopeOffsetCompCmd.scope_offsets, sizeof(TY_API_SCOPE_OFFSETS));
        }
        break;

    case API_SCOPE_OFFSET_COMP_MEASURE:
        w_RetVal = _ApiCmdScopeOffsetCompMeasureAndSet(ulModuleHandle, pxOffsets);
        break;

    case API_SCOPE_OFFSET_COMP_GET:
        /* Issue 'get offsets' target software command to get current offsets */
        x_ScopeOffsetCompCmd.ulMode = SCOPE_GET_OFFSETS;
        w_RetVal = ApiIoStruct(ulModuleHandle, (TY_MIL_COM*) &x_ScopeOffsetCompCmd, (TY_MIL_COM_ACK*) &x_ScopeOffsetCompAck);
        if(w_RetVal == API_OK && pxOffsets != NULL)
        {
            /* return the current settings delivered in target acknowledge to the user's structure if available */
            memcpy(pxOffsets, &x_ScopeOffsetCompAck.scope_offsets, sizeof(TY_API_SCOPE_OFFSETS));
        }
        break;

    default:
        return API_ERR_INVALID_MODE;
    }

    return w_RetVal;
}


AiInt16 _ApiCmdScopeOffsetCompMeasureAndSet(AiUInt32 ulModuleHandle, TY_API_SCOPE_OFFSETS* pxOffsets)
{
    AiInt16 w_RetVal = API_ERR;
    TY_MIL_COM_SCOPE_OFFSET_COMP x_ScopeOffsetCompCmd;
    TY_MIL_COM_SCOPE_OFFSET_COMP_OUT x_ScopeOffsetCompAck;
    AiInt32 primary_offset = 0;
    AiInt32 secondary_offset = 0;
    TY_API_SCOPE_OFFSETS* p_MeasuredOffsets = 0;

    memset( &x_ScopeOffsetCompCmd, 0, sizeof(x_ScopeOffsetCompCmd) );
    memset( &x_ScopeOffsetCompAck, 0, sizeof(x_ScopeOffsetCompAck) );

    p_MeasuredOffsets = &x_ScopeOffsetCompCmd.scope_offsets;

    /* First perform a reset of the scope offsets, so we do the measurement
    without offset compensated values */
    MIL_COM_INIT(&x_ScopeOffsetCompCmd.xCommand, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_OFFSET_COMP, sizeof(x_ScopeOffsetCompCmd), sizeof(x_ScopeOffsetCompAck));
    x_ScopeOffsetCompCmd.ulMode = SCOPE_SET_OFFSETS;
    memset(p_MeasuredOffsets, 0, sizeof(TY_API_SCOPE_OFFSETS));

    w_RetVal = ApiIoStruct(ulModuleHandle, (TY_MIL_COM*) &x_ScopeOffsetCompCmd, (TY_MIL_COM_ACK*) &x_ScopeOffsetCompAck);
    if(w_RetVal)
    {
        return w_RetVal;
    }

    /* Measure offsets for coupling mode stub 30V on both channels */
    w_RetVal = DetermineScopeOffsets(ulModuleHandle, &primary_offset, &secondary_offset, API_SCOPE_COUPLING_STUB);
    if(w_RetVal)
    {
        return w_RetVal;
    }

    p_MeasuredOffsets->lOffsetA_30V_stub = primary_offset;
    p_MeasuredOffsets->lOffsetB_30V_stub = secondary_offset;

    /* Measure offsets for coupling mode stub 3V on both channels */
    w_RetVal = DetermineScopeOffsets(ulModuleHandle, &primary_offset, &secondary_offset, API_SCOPE_COUPLING_STUB_3V);
    if(w_RetVal)
    {
        return w_RetVal;
    }

    p_MeasuredOffsets->lOffsetA_3V_stub = primary_offset;
    p_MeasuredOffsets->lOffsetB_3V_stub = secondary_offset;

    /* A separate measurement for the external coupling modes is not necessary as per statement
    of hardware division. So we just copy the corresponding stub coupling offsets */
    p_MeasuredOffsets->lOffsetA_30V_ext = p_MeasuredOffsets->lOffsetA_30V_stub;
    p_MeasuredOffsets->lOffsetB_30V_ext = p_MeasuredOffsets->lOffsetB_30V_stub;
    p_MeasuredOffsets->lOffsetA_3V_ext = p_MeasuredOffsets->lOffsetA_3V_stub;
    p_MeasuredOffsets->lOffsetB_3V_ext = p_MeasuredOffsets->lOffsetB_3V_stub;


    /* Now issue 'set offsets' target command with the measured offsets to compensate
    them in further scope recordings */
    w_RetVal = ApiIoStruct(ulModuleHandle, (TY_MIL_COM*) &x_ScopeOffsetCompCmd, (TY_MIL_COM_ACK*) &x_ScopeOffsetCompAck);
    if(w_RetVal)
    {
        return w_RetVal;
    }

    if(pxOffsets)
    {
        /* copy measured offsets to user's structure if available */
        memcpy(pxOffsets,p_MeasuredOffsets, sizeof(TY_API_SCOPE_OFFSETS));
    }

    return w_RetVal;
}
