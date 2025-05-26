/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsScope.c
 *
 * LS Digital Scope functions
 * 
 */

#include "AiOs.h"
#include "Ai_cdef.h"
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
#include "DEFS_V.H"
#include "Dll_Fnc.h"

#include "ApiLsScope.h"


//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiCmdScopeSetup
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 20.03.06
//---------------------------------------------------------------------------
//
//    Description : This function setup the MIL-Scope
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeSetup(AiUInt32 ul_Module, TY_API_SCOPE_SETUP *px_ScopeSetup)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ul_Module );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( NULL == px_ScopeSetup )
            uw_RetVal = API_ERR_PARAM2_IS_NULL;
        else if( px_ScopeSetup->ul_CouplingPri > MAX_SCOPE_COUPLING_MODE )
            uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
        else if( px_ScopeSetup->ul_CouplingSec > MAX_SCOPE_COUPLING_MODE )
            uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
        else if( px_ScopeSetup->ul_SamplingRate > MAX_SCOPE_SAMPLING_RATE )
            uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
        else if( px_ScopeSetup->ul_CaptureMode > MAX_SCOPE_CAP_MODE )
            uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
        else if( px_ScopeSetup->ul_OperationMode > MAX_SCOPE_OPR_MODE )
            uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    }

    if(API_OK == uw_RetVal)
    {
        uw_RetVal = _ApiCmdScopeSetup( ul_Module, px_ScopeSetup);
    }

    v_ExamineRetVal( "ApiCmdScopeSetup", uw_RetVal );
    TRACE_BEGIN
        TRACE("        TY_API_SCOPE_SETUP x_ScopeSetup;\n");
    TRACE1("        x_ScopeSetup.ul_CaptureMode   = 0x%x;\n", px_ScopeSetup->ul_CaptureMode);
    TRACE1("        x_ScopeSetup.ul_CouplingPri   = 0x%x;\n", px_ScopeSetup->ul_CouplingPri);
    TRACE1("        x_ScopeSetup.ul_CouplingSec   = 0x%x;\n", px_ScopeSetup->ul_CouplingSec);
    TRACE1("        x_ScopeSetup.ul_OperationMode = 0x%x;\n", px_ScopeSetup->ul_OperationMode);
    TRACE1("        x_ScopeSetup.ul_SamplingRate  = 0x%x;\n", px_ScopeSetup->ul_SamplingRate);
    TRACE1("        x_ScopeSetup.ul_SingleShotBuf = 0x%x;\n", px_ScopeSetup->ul_SingleShotBuf);
    TRACE_FCTA("ApiCmdScopeSetup", uw_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_RPARE("&x_ScopeSetup");
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}



//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiCmdScopeCalibrate
//
//   Author : Martin Haag              Project   : APX-1553-DS
//
//---------------------------------------------------------------------------
//   Create : 09.07.09
//---------------------------------------------------------------------------
//
//    Description : This function calibrates the MilScope
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeCalibrate( AiUInt32 ul_Module, AiUInt8 ucMode, TY_API_SCOPE_CALIBRATION_INFO * pxInfo )
{
    AiInt16 w_RetVal = API_OK;

    AiUInt8  ucWriteBytes[1];
    AiInt16  wReadLWords;
    AiUInt32 aulOutData[7];
    TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule( ul_Module );


    if(pDevice->uc_DeviceGroup == AI_DEVICE_AYE)
    {
        /* -- Compatibility for AYE --- */

        /* The new function ApiCmdScopeOffsetCompensation is not yet integrated
        into the PBA.pro. But we need it to test the APE1553-DS. Thus we
        call the new function here for compatibility. This feature is not
        documented and not wanted. In future versions this function should return
        an error for AyE or be removed completely.
        */
        w_RetVal = ApiCmdScopeOffsetCompensation( ul_Module, ucMode, NULL );

        if( pxInfo )
            memset( pxInfo, 0, sizeof(TY_API_SCOPE_CALIBRATION_INFO) );
    }
    else
    {

        /* -- Calibrate AyX device --- */

        ucWriteBytes[0]  = ucMode;

        w_RetVal = ApiIo( ul_Module & ~API_STREAM_MASK, 0, TG_API_SCOPE_CALIBRATE, TG_ACK2,
            &ucWriteBytes[0], 1, 
            w_p, 0,
            b_p, &tg_os, 
            w_p, &tg_os,
            aulOutData, &wReadLWords );

        if( API_OK == w_RetVal )
        {
            if( wReadLWords >= 5 )
            {
                if( pxInfo != 0 )
                {
                    pxInfo->lOffsetA       = (AiInt32)aulOutData[1];
                    pxInfo->lOffsetB       = (AiInt32)aulOutData[2];
                    pxInfo->lOffsetA100    = (AiInt32)aulOutData[3];
                    pxInfo->lOffsetB100    = (AiInt32)aulOutData[4];

                    if( wReadLWords >= 7 )
                    {
                        pxInfo->lOffsetA100Sec = (AiInt32)aulOutData[5];
                        pxInfo->lOffsetB100Sec = (AiInt32)aulOutData[6];
                    }
                    else
                    {
                        pxInfo->lOffsetA100Sec = (AiInt32)0;
                        pxInfo->lOffsetB100Sec = (AiInt32)0;
                    }
                }
            }
            else
            {
                w_RetVal = API_ERR_WCNT_NOT_IN_RANGE;
            }
        }
    }

    v_ExamineRetVal( "ApiCmdScopeCalibrate", w_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeCalibrate", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(ucMode);
    TRACE_FCTE;
    TRACE("        TY_API_SCOPE_CALIBRATION_INFO pxInfo;\n");
    TRACE1("        pxInfo->lOffsetA       = 0x%x;\n", pxInfo->lOffsetA );
    TRACE1("        pxInfo->lOffsetB       = 0x%x;\n", pxInfo->lOffsetB );
    TRACE1("        pxInfo->lOffsetA100    = 0x%x;\n", pxInfo->lOffsetA100 );
    TRACE1("        pxInfo->lOffsetB100    = 0x%x;\n", pxInfo->lOffsetB100 );
    TRACE1("        pxInfo->lOffsetA100Sec = 0x%x;\n", pxInfo->lOffsetA100Sec );
    TRACE1("        pxInfo->lOffsetB100Sec = 0x%x;\n", pxInfo->lOffsetB100Sec );
    TRACE_END

        return(w_RetVal);
}


//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiCmdScopeTriggerDef
//
//   Author : Patrick Giesel           Project   : E2HLBA
//
//---------------------------------------------------------------------------
//   Create : 20.03.06
//---------------------------------------------------------------------------
//
//    Description : This function setup the MIL-Scope Trigger
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeTriggerDef(AiUInt32 ul_Module, TY_API_SCOPE_TRG *px_ScopeTrg)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt8 auc_Info[28];

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( NULL == px_ScopeTrg )
            uw_RetVal = API_ERR_PARAM2_IS_NULL;
        else if( px_ScopeTrg->ul_TrgMode > MAX_SCOPE_TRG_MODE )
            uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
        else if( px_ScopeTrg->ul_TrgSrc > MAX_SCOPE_TRG_SRC )
            uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
        else if( px_ScopeTrg->ul_TrgValue > MAX_SCOPE_TRG_VALUE )
            uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
        else if( px_ScopeTrg->ul_TrgNbSamples > MAX_SCOPE_TRG_NB_SAMPLES )
            uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
        else if( px_ScopeTrg->ul_TrgDelay > MAX_SCOPE_TRG_DELAY )
            uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
        else if( px_ScopeTrg->ul_TrgTbt > MAX_SCOPE_TRG_TBT )
            uw_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
    }

    if(API_OK == uw_RetVal)
    {
        auc_Info[0]  = (AiUInt8)(px_ScopeTrg->ul_TrgMode >> 24);
        auc_Info[1]  = (AiUInt8)(px_ScopeTrg->ul_TrgMode >> 16);
        auc_Info[2]  = (AiUInt8)(px_ScopeTrg->ul_TrgMode >> 8);
        auc_Info[3]  = (AiUInt8)px_ScopeTrg->ul_TrgMode;
        auc_Info[4]  = (AiUInt8)(px_ScopeTrg->ul_TrgSrc >> 24);
        auc_Info[5]  = (AiUInt8)(px_ScopeTrg->ul_TrgSrc >> 16);
        auc_Info[6]  = (AiUInt8)(px_ScopeTrg->ul_TrgSrc >> 8);
        auc_Info[7]  = (AiUInt8)px_ScopeTrg->ul_TrgSrc;
        auc_Info[8]  = (AiUInt8)(px_ScopeTrg->ul_TrgValue >> 24);
        auc_Info[9]  = (AiUInt8)(px_ScopeTrg->ul_TrgValue >> 16);
        auc_Info[10] = (AiUInt8)(px_ScopeTrg->ul_TrgValue >> 8);
        auc_Info[11] = (AiUInt8)px_ScopeTrg->ul_TrgValue;
        auc_Info[12] = (AiUInt8)(px_ScopeTrg->ul_TrgNbSamples >> 24);
        auc_Info[13] = (AiUInt8)(px_ScopeTrg->ul_TrgNbSamples >> 16);
        auc_Info[14] = (AiUInt8)(px_ScopeTrg->ul_TrgNbSamples >> 8);
        auc_Info[15] = (AiUInt8)px_ScopeTrg->ul_TrgNbSamples;
        auc_Info[16] = (AiUInt8)(px_ScopeTrg->ul_TrgFlags >> 24);
        auc_Info[17] = (AiUInt8)(px_ScopeTrg->ul_TrgFlags >> 16);
        auc_Info[18] = (AiUInt8)(px_ScopeTrg->ul_TrgFlags >> 8);
        auc_Info[19] = (AiUInt8)px_ScopeTrg->ul_TrgFlags;
        auc_Info[20] = (AiUInt8)(px_ScopeTrg->ul_TrgDelay >> 24);
        auc_Info[21] = (AiUInt8)(px_ScopeTrg->ul_TrgDelay >> 16);
        auc_Info[22] = (AiUInt8)(px_ScopeTrg->ul_TrgDelay >> 8);
        auc_Info[23] = (AiUInt8)px_ScopeTrg->ul_TrgDelay;
        auc_Info[24] = (AiUInt8)(px_ScopeTrg->ul_TrgTbt >> 24);
        auc_Info[25] = (AiUInt8)(px_ScopeTrg->ul_TrgTbt >> 16);
        auc_Info[26] = (AiUInt8)(px_ScopeTrg->ul_TrgTbt >> 8);
        auc_Info[27] = (AiUInt8)px_ScopeTrg->ul_TrgTbt;

        uw_RetVal = ApiIo( ul_Module & ~API_STREAM_MASK, 0, TG_API_SCOPE_TRG_DEF, TG_ACK,
            &auc_Info[0], 28, w_p, 0,
            b_p, &tg_os, w_p, &tg_os,
            lw_p, &tg_os);
    }

    v_ExamineRetVal( "ApiCmdScopeTriggerDef", uw_RetVal );
    TRACE_BEGIN
        TRACE("        TY_API_SCOPE_TRG x_ScopeTrg;\n");
    TRACE1("        x_ScopeTrg.ul_TrgDelay       = 0x%x;\n", px_ScopeTrg->ul_TrgDelay);
    TRACE1("        x_ScopeTrg.ul_TrgFlags       = 0x%x;\n", px_ScopeTrg->ul_TrgFlags);
    TRACE1("        x_ScopeTrg.ul_TrgMode        = 0x%x;\n", px_ScopeTrg->ul_TrgMode);
    TRACE1("        x_ScopeTrg.ul_TrgNbSamples   = 0x%x;\n", px_ScopeTrg->ul_TrgNbSamples);
    TRACE1("        x_ScopeTrg.ul_TrgSrc         = 0x%x;\n", px_ScopeTrg->ul_TrgSrc);
    TRACE1("        x_ScopeTrg.ul_TrgTbt         = 0x%x;\n", px_ScopeTrg->ul_TrgTbt);
    TRACE1("        x_ScopeTrg.ul_TrgValue       = 0x%x;\n", px_ScopeTrg->ul_TrgValue);
    TRACE_FCTA("ApiCmdScopeTriggerDef", uw_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_RPARE("&x_ScopeTrg");
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}


AI_LIB_FUNC TY_API_SCOPE_BUFFER* AI_CALL_CONV ApiCreateScopeBuffer(AiUInt32 ulModuleHandle, AiUInt32 ulID, TY_API_SCOPE_BUFFER_TYPE eBufferType,
                                                                    TY_API_SCOPE_BUFFER_HANDLER BufferHandler, void* pvUserData)
{
    AiInt16 uw_RetVal = API_OK;
    TY_API_SCOPE_BUFFER *px_Buffer = NULL;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModuleHandle );

    if( pDevice == NULL )
        uw_RetVal = API_ERR_NO_MODULE_EXTENSION;

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( eBufferType > SCOPE_BUFFER_SECONDARY)
            uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
        else if (NULL == pDevice->px_ScopeSetup)
            uw_RetVal = API_ERR_NO_SCOPE_EXECUTED;
    }

    if(API_OK == uw_RetVal)
    {
        px_Buffer = _ApiCreateScopeBuffer( ulModuleHandle, ulID, eBufferType, BufferHandler, pvUserData); 
    }

    v_ExamineRetVal( "ApiCreateScopeBuffer", uw_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiCreateScopeBuffer", uw_RetVal);
    TRACE_PARA(ulModuleHandle);
    TRACE_PARA(ulID);
    TRACE_PARA(eBufferType);
    TRACE_PPARA(&BufferHandler);
    TRACE_PPARA(pvUserData);
    TRACE_FCTE;
    TRACE_END
        return( px_Buffer);
}

//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiFreeScopeBuffer
//
//   Author : Michael Riedinger        Project   : Milscope (redesign)
//
//---------------------------------------------------------------------------
//   Create : 20.03.06
//---------------------------------------------------------------------------
//
//    Description : This function frees a scope-Buffer wich was allocated.
//      No access to buffer may be done after this function was executed.
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiFreeScopeBuffer(AiUInt32 ulModuleHandle, TY_API_SCOPE_BUFFER *px_Buffer)
{
    AiInt16 uw_RetVal = API_OK;

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( NULL == px_Buffer )
            uw_RetVal = API_ERR_PARAM2_IS_NULL;
    }

    if(API_OK == uw_RetVal)
    {
        uw_RetVal = _ApiFreeScopeBuffer( ulModuleHandle, px_Buffer);
    }

    v_ExamineRetVal( "ApiFreeScopeBuffer", uw_RetVal );
    TRACE_BEGIN
        TRACE("        ApiFreeScopeBuffer px_Buffer;\n");
    TRACE1("        px_Buffer   = 0x%p;\n", px_Buffer);
    TRACE_FCTA("ApiFreeScopeBuffer", uw_RetVal); 
    TRACE_PARA(ulModuleHandle);
    TRACE_RPARE("&x_ScopeSetup");
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeStart(AiUInt32 ul_Module)
{

    AiInt16 w_RetVal = _ApiCmdScopeStart(ul_Module);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeStart", w_RetVal); 
    TRACE_PARE(ul_Module);
    TRACE_FCTE;
    TRACE_END

        return( w_RetVal );
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeStatus(AiUInt32 ulModuleHandle, TY_API_SCOPE_STATUS* p_ScopeStatus, AiUInt32* pulNumBuffersLeft)
{
    AiInt16 ret_val = API_OK;

    if(p_ScopeStatus == NULL)
    {
        return API_ERR_PARAM2_IS_NULL;
    }

    ret_val = _ApiCmdScopeStatus(ulModuleHandle, p_ScopeStatus, pulNumBuffersLeft);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeStatus", ret_val);
    TRACE_PARA(ulModuleHandle);
    TRACE_PARA(*p_ScopeStatus);
    TRACE_PARE(*pulNumBuffersLeft);
    TRACE_FCTE;
    TRACE_END

        return ret_val;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeStop(AiUInt32 ulModuleHandle)
{
    AiInt16 ret_val = API_OK;

    ret_val = _ApiCmdScopeStop(ulModuleHandle);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeStop", ret_val);
    TRACE_PARE(ulModuleHandle);
    TRACE_FCTE;
    TRACE_END

        return ret_val;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeTriggerDefEx(AiUInt32 ulModuleHandle, TY_API_SCOPE_TRG_EX* px_ScopeTrg)
{
    AiInt16 return_val = API_OK;
    TY_MIL_COM_SCOPE_TRIGGER_INPUT xInput;
    TY_MIL_COM_ACK xOutput;

    MIL_COM_INIT(&xInput.xCommand, API_STREAM(ulModuleHandle), 0, MIL_COM_SCOPE_TRIGGER, sizeof(xInput), sizeof(xOutput));
    memcpy(&xInput.trigger_settings, px_ScopeTrg, sizeof(TY_API_SCOPE_TRG_EX));

    return_val = ApiIoStruct(ulModuleHandle, (TY_MIL_COM*) &xInput, &xOutput);

    v_ExamineRetVal("ApiCmdScopeTriggerDefEx", return_val);

    TRACE_BEGIN
        TRACE("        TY_API_SCOPE_TRG_EX x_ScopeTrg;\n");
    TRACE1("        x_ScopeTrg.ul_TrgMode        = %d;\n", px_ScopeTrg->ul_TrgMode);
    TRACE1("        x_ScopeTrg.ul_TrgSrc         = %d;\n", px_ScopeTrg->ul_TrgSrc);
    TRACE1("        x_ScopeTrg.ul_TrgValue       = %d;\n", px_ScopeTrg->ul_TrgValue);
    TRACE1("        x_ScopeTrg.ul_TrgValue2      = %d;\n", px_ScopeTrg->ul_TrgValue2);
    TRACE1("        x_ScopeTrg.ul_TrgNbSamples   = %d;\n", px_ScopeTrg->ul_TrgNbSamples);
    TRACE1("        x_ScopeTrg.ul_TrgNbSamples2  = %d;\n", px_ScopeTrg->ul_TrgNbSamples2);
    TRACE1("        x_ScopeTrg.ul_TrgFlags       = 0x%x;\n", px_ScopeTrg->ul_TrgFlags);
    TRACE1("        x_ScopeTrg.ul_TrgDelay       = %d;\n", px_ScopeTrg->ul_TrgDelay);
    TRACE1("        x_ScopeTrg.ul_TrgTbt         = %d;\n", px_ScopeTrg->ul_TrgTbt);
    TRACE1("        x_ScopeTrg.ul_TrgDiscrete;   = %d;\n", px_ScopeTrg->ul_TrgDiscrete);
    TRACE_FCTA("ApiCmdScopeTriggerDefEx", return_val); 
    TRACE_PARA(ulModuleHandle);
    TRACE_RPARE("&x_ScopeTrg");
    TRACE_FCTE;
    TRACE_END

        return return_val;
}


//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiProvideScopeBuffers
//
//   Author : Michael Riedinger        Project   : Milscope (redesign)
//
//---------------------------------------------------------------------------
//   Create : 30.03.06
//---------------------------------------------------------------------------
//
//    Description : This function provides number of scope-Buffer for scope
//      data. 
//      Multiple calls are allowed to provide all buffers.
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiProvideScopeBuffers( AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* axScopeBuffers[])
{

    AiInt16 w_RetVal = API_OK;
    AiUInt32 i;

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( ulNumBuffers < 1)
            w_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    }

    if(API_OK == w_RetVal)
    {
        w_RetVal = _ApiProvideScopeBuffers(ulModuleHandle, ulNumBuffers, axScopeBuffers);
    }

    TRACE_BEGIN
        TRACE_FCTA("ApiProvideScopeBuffers", w_RetVal); 
    TRACE_PARE(ulModuleHandle);
    TRACE1("        ulNumBuffers  = %d\n", ulNumBuffers);
    for ( i=0; i<ulNumBuffers; i++) {
        TRACE1("    Buffer Number          = %d\n", i);
        TRACE1("        ulBufferSize        = %d\n", axScopeBuffers[i]->ulBufferSize);
        TRACE1("        ulBufferSize        = 0x%08x\n", axScopeBuffers[i]->ulBufferSize);
        TRACE1("        ulID              = %d\n", axScopeBuffers[i]->ulID);
    }
    TRACE_FCTE;
    TRACE_END

        return( w_RetVal );
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWaitForScopeFinished(AiUInt32 ulModuleHandle, AiInt32 lTimeOut,
                                                          TY_API_SCOPE_WAIT_STATE* p_WaitResultFlags)
{
    AiInt16 ret_val = API_ERR;

    if(!p_WaitResultFlags)
    {
        return API_ERR_PARAM3_IS_NULL;
    }

    ret_val = _ApiWaitForScopeFinished(ulModuleHandle, lTimeOut, p_WaitResultFlags);

    TRACE_BEGIN
        TRACE_FCTA("ApiWaitForScopeFinished", ret_val);
    TRACE_PARA(ulModuleHandle);
    TRACE_PARA(lTimeOut);
    TRACE_PARE(*p_WaitResultFlags);
    TRACE_FCTE;
    TRACE_END

        return ret_val;
}

//***************************************************************************
//
//   Module : ApiLsScope               Submodule : ApiCmdScopeTriggerStrobeDef
//
//   Author : Michael Riedinger        Project   : Milscope (redesign)
//
//---------------------------------------------------------------------------
//   Create : 18.06.2012
//---------------------------------------------------------------------------
//
//    Description : In case of a trigger event a strobe can be generated. 
//      This function defines the kind of strobe. 
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeTriggerStrobeDef( AiUInt32 ulModuleHandle, TY_API_SCOPE_TRG_STROBE *px_ScopeTrgStrobe)
{

    AiInt16 w_RetVal = API_OK;

    if( Aiul_DbgLevel & DBG_PARAMCHK )
    {
        if( NULL == px_ScopeTrgStrobe)
            w_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    }

    if(API_OK == w_RetVal)
    {
        w_RetVal = _ApiCmdScopeTriggerStrobeDef(ulModuleHandle, px_ScopeTrgStrobe);
    }

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeTriggerStrobeDef", w_RetVal); 
    TRACE_PARE(ulModuleHandle);
    TRACE1("    TrgStrobeDest           = %d\n", px_ScopeTrgStrobe->ul_TrgStrobeDest);
    TRACE1("    TrgStrobeType           = %d\n", px_ScopeTrgStrobe->ul_TrgStrobeType);
    TRACE_FCTE;
    TRACE_END

        return( w_RetVal );
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeReset(AiUInt32 ulModuleHandle)
{
    AiInt16 w_RetVal = API_ERR;

    w_RetVal = _ApiCmdScopeReset(ulModuleHandle);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeReset", w_RetVal);
    TRACE_PARE(ulModuleHandle);
    TRACE_FCTE;
    TRACE_END

        return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCreateScopeBufferList(AiUInt32 ulModuleHandle, AiUInt32 ulNumBuffers, TY_API_SCOPE_BUFFER* paxBufferList[], 
                                                           TY_API_SCOPE_BUFFER_HANDLER pBufferHandler, void* pvUserData)
{
    AiInt16 w_RetVal = API_ERR;
    TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule(ulModuleHandle);

    if(pDevice == NULL)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    if(pDevice->px_ScopeSetup == NULL)
    {
        return API_ERR_NO_SCOPE_EXECUTED;
    }

    w_RetVal = _ApiCreateScopeBufferList(ulModuleHandle, ulNumBuffers, paxBufferList, pBufferHandler, pvUserData);

    v_ExamineRetVal( "ApiCreateScopeBufferList", w_RetVal );

    TRACE_BEGIN
        TRACE_FCTA("ApiCreateScopeBufferList", w_RetVal);
    TRACE_PARA(ulModuleHandle);
    TRACE_PARA(ulNumBuffers);
    TRACE_PPARA(paxBufferList);
    TRACE_PPARA(pBufferHandler);
    TRACE_PPARE(pvUserData);
    TRACE_FCTE;
    TRACE_END

        return w_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdScopeOffsetCompensation(AiUInt32 ulModuleHandle, AiUInt8 ucMode, TY_API_SCOPE_OFFSETS* pxOffsets)
{
    AiInt16 w_RetVal = API_ERR;

    if(ucMode > API_SCOPE_OFFSET_COMP_GET)
    {
        return API_ERR_INVALID_MODE;
    }

    w_RetVal = _ApiCmdScopeOffsetCompensation(ulModuleHandle, ucMode, pxOffsets);

    v_ExamineRetVal( "ApiCmdScopeOffsetCompensation", w_RetVal );

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdScopeOffsetCompensation", w_RetVal);
    TRACE_PARA(ulModuleHandle);
    TRACE_PARA(ucMode);
    TRACE_PPARE(pxOffsets);
    TRACE_FCTE;
    TRACE_END

        return w_RetVal;
}

