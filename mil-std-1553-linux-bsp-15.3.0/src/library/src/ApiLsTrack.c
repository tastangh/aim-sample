/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsTrack.h
 *
 * LS track functions
 * 
 */

#include "AiOs.h"
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
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "AiMapDllToTsw.h"



//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackDef
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.01.08
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//                Buffer Data [data]
//
//    Outputs   : Instruction execution status [status]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackDef(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt8 uc_BT,
                                                 AiUInt16 uw_XidRtSa, AiUInt8 uc_Mode, AiUInt16 uw_TrackStartPos,
                                                 AiUInt8 uc_TrackBPos, AiUInt8 uc_TrackBLen, AiUInt16 uw_TrackLen,
                                                 AiUInt16 uw_MultiplexedTrackNb, AiUInt16 uw_ContinousTrackNb,
                                                 AiUInt32 *pul_TrackBufferStartAddr)
{
    AiInt16              uw_RetVal = API_OK;
    TY_API_TRACK_DEF_IN  x_TrackDefIn;
    TY_API_TRACK_DEF_OUT x_TrackDefOut;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( uc_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( uc_Mode > API_ENA )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( API_ENA == uc_Mode )
    {
        if( (0 == uc_BT) || (uc_BT > API_TRACK_TYPE_MUX_TRACK)  )
            uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
        else 
            if(API_TRACK_TYPE_MUX_TRACK != uc_BT)
            {    
                if( (API_BC_MODE == uc_BT) &&
                    ((((uw_XidRtSa & API_TRACK_XFERID_MASK) >> API_TRACK_XFERID_POS) < MIN_XFER_ID) ||
                    (((uw_XidRtSa & API_TRACK_XFERID_MASK) >> API_TRACK_XFERID_POS) > MAX_XFER_ID)) )
                    uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
                else if( (API_RT_MODE == uc_BT) &&
                    (((uw_XidRtSa & API_TRACK_RTADDR_MASK) >> API_TRACK_RTADDR_POS) > MAX_RT_ADDR) )
                    uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
                else if( (API_RT_MODE == uc_BT) &&
                    (((uw_XidRtSa & API_TRACK_SA_MID_MASK) >> API_TRACK_SA_MID_POS) > MAX_RT_MODECODE) )
                    uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
                else if( (uw_ContinousTrackNb < MIN_CONT_TRACK_NB) || (uw_ContinousTrackNb > MAX_CONT_TRACK_NB) )
                    uw_RetVal = API_ERR_PARAM12_NOT_IN_RANGE;
            }
            else
            {
                // uw_XidRtSa is used to specify the related track in MUX_TRACK mode
                if(  uw_XidRtSa > MAX_TRACK_ID  )
                    uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
            }

            if(API_OK == uw_RetVal)
            { 
                if( uw_TrackStartPos > (MAX_DATA_WORDPOS-1) )
                    uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
                else if( uc_TrackBPos > MAX_DATA_BITPOS )
                    uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
                else if( (uc_TrackBLen < MIN_DATA_BITLEN) || (uc_TrackBLen > MAX_DATA_BITLEN) )
                    uw_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
                else if( (uw_TrackLen < MIN_TRACK_LEN) || (uw_TrackLen > MAX_TRACK_LEN) )
                    uw_RetVal = API_ERR_PARAM10_NOT_IN_RANGE;
                else if( NULL == pul_TrackBufferStartAddr )
                    uw_RetVal = API_ERR_PARAM13_IS_NULL;
            }
    }

    if( API_OK == uw_RetVal )
    {
        x_TrackDefIn.ul_TrackId            = uc_TrackId;
        x_TrackDefIn.ul_BT                 = uc_BT;
        x_TrackDefIn.ul_XidRtSa            = uw_XidRtSa;
        x_TrackDefIn.ul_Mode               = uc_Mode;
        x_TrackDefIn.ul_TrackStartPos      = uw_TrackStartPos;
        x_TrackDefIn.ul_TrackBPos          = uc_TrackBPos;
        x_TrackDefIn.ul_TrackBLen          = uc_TrackBLen;
        x_TrackDefIn.ul_TrackLen           = uw_TrackLen;
        x_TrackDefIn.ul_MultiplexedTrackNb = uw_MultiplexedTrackNb;
        x_TrackDefIn.ul_ContinousTrackNb   = uw_ContinousTrackNb;
        x_TrackDefIn.l_Offset              = 0;
        uw_RetVal = ApiCmdTrackDefEx_( bModule, biu, &x_TrackDefIn, &x_TrackDefOut );
        if( API_OK == uw_RetVal )
        {
            *pul_TrackBufferStartAddr = x_TrackDefOut.ul_TrackBufferStartAddr;
        }
    }

    v_ExamineRetVal( "ApiCmdTrackDef", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        AiUInt32 ul_TrackBufferStartAddr;\n");
    TRACE_FCTA("ApiCmdTrackDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(uc_TrackId);
    TRACE_PARA(uc_BT);
    TRACE_PARA(uw_XidRtSa);
    TRACE_PARA(uc_Mode);
    TRACE_PARA(uw_TrackStartPos);
    TRACE_PARA(uc_TrackBPos);
    TRACE_PARA(uc_TrackBLen);
    TRACE_PARA(uw_TrackLen);
    TRACE_PARA(uw_MultiplexedTrackNb);
    TRACE_PARA(uw_ContinousTrackNb);
    TRACE_RPARE("&ul_TrackBufferStartAddr");
    TRACE_FCTE;
    TRACE1("    --> ul_TrackBufferStartAddr = 0x%x\n", *pul_TrackBufferStartAddr);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackDefEx
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.05.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//                Buffer Data [data]
//
//    Outputs   : Instruction execution status [status]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    
//
//***************************************************************************
AiInt16 ApiCmdTrackDefEx_(AiUInt32 bModule, AiUInt8 biu, 
                                                   TY_API_TRACK_DEF_IN  *px_TrackDefIn,
                                                   TY_API_TRACK_DEF_OUT *px_TrackDefOut )
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_SYS_TRACK_DEF_INPUT  xInput;
    TY_MIL_COM_LS_SYS_TRACK_DEF_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_TRACK_DEF, sizeof(xInput), sizeof(xOutput) );

    xInput.track_id   = px_TrackDefIn->ul_TrackId;
    xInput.bt         = px_TrackDefIn->ul_BT;
    xInput.xid_rt_sa  = px_TrackDefIn->ul_XidRtSa;
    xInput.mode       = px_TrackDefIn->ul_Mode;
    xInput.start      = px_TrackDefIn->ul_TrackStartPos;
    xInput.bit_pos    = px_TrackDefIn->ul_TrackBPos;
    xInput.bit_len    = px_TrackDefIn->ul_TrackBLen;
    xInput.size       = px_TrackDefIn->ul_TrackLen;
    xInput.states     = px_TrackDefIn->ul_MultiplexedTrackNb;
    xInput.file_count = px_TrackDefIn->ul_ContinousTrackNb;
    xInput.offset     = px_TrackDefIn->l_Offset;

    uw_RetVal = API_CMD_STRUCT_SYS_TRACK_DEF( bModule, &xInput, &xOutput );

    px_TrackDefOut->ul_TrackBufferStartAddr = xOutput.buffer_start;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackDefEx(AiUInt32 bModule, AiUInt8 biu, 
                                                   TY_API_TRACK_DEF_IN  *px_TrackDefIn,
                                                   TY_API_TRACK_DEF_OUT *px_TrackDefOut )
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_TrackDefOut )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( px_TrackDefIn->ul_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_SUBPARAM1_NOT_IN_RANGE;
    else if( px_TrackDefIn->ul_Mode > API_ENA )
        uw_RetVal = API_ERR_SUBPARAM4_NOT_IN_RANGE;
    else if( API_ENA == px_TrackDefIn->ul_Mode )
    {
        if( (0 == px_TrackDefIn->ul_BT) || (px_TrackDefIn->ul_BT > API_TRACK_TYPE_MUX_TRACK)  )
            uw_RetVal = API_ERR_SUBPARAM2_NOT_IN_RANGE;
        else 
            if(API_TRACK_TYPE_MUX_TRACK != px_TrackDefIn->ul_BT)
            {    
                if( (API_BC_MODE == px_TrackDefIn->ul_BT) &&
                    ((((px_TrackDefIn->ul_XidRtSa & API_TRACK_XFERID_MASK) >> API_TRACK_XFERID_POS) < MIN_XFER_ID) ||
                    (((px_TrackDefIn->ul_XidRtSa & API_TRACK_XFERID_MASK) >> API_TRACK_XFERID_POS) > MAX_XFER_ID)) )
                    uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
                else if( (API_RT_MODE == px_TrackDefIn->ul_BT) &&
                    (((px_TrackDefIn->ul_XidRtSa & API_TRACK_RTADDR_MASK) >> API_TRACK_RTADDR_POS) > MAX_RT_ADDR) )
                    uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
                else if( (API_RT_MODE == px_TrackDefIn->ul_BT) &&
                    (((px_TrackDefIn->ul_XidRtSa & API_TRACK_SA_MID_MASK) >> API_TRACK_SA_MID_POS) > MAX_RT_MODECODE) )
                    uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
                else if( (px_TrackDefIn->ul_ContinousTrackNb < MIN_CONT_TRACK_NB) || (px_TrackDefIn->ul_ContinousTrackNb > MAX_CONT_TRACK_NB) )
                    uw_RetVal = API_ERR_SUBPARAM10_NOT_IN_RANGE;
            }
            else
            {
                // uw_XidRtSa is used to specify the related track in MUX_TRACK mode
                if(  px_TrackDefIn->ul_XidRtSa > MAX_TRACK_ID  )
                    uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
            }

            if(API_OK == uw_RetVal)
            { 
                if( px_TrackDefIn->ul_TrackStartPos > (MAX_DATA_WORDPOS-1) )
                    uw_RetVal = API_ERR_SUBPARAM5_NOT_IN_RANGE;
                else if( px_TrackDefIn->ul_TrackBPos > MAX_DATA_BITPOS )
                    uw_RetVal = API_ERR_SUBPARAM6_NOT_IN_RANGE;
                else if( (px_TrackDefIn->ul_TrackBLen < MIN_DATA_BITLEN) || (px_TrackDefIn->ul_TrackBLen > MAX_DATA_BITLEN) )
                    uw_RetVal = API_ERR_SUBPARAM7_NOT_IN_RANGE;
                else if( (px_TrackDefIn->ul_TrackLen < MIN_TRACK_LEN) || (px_TrackDefIn->ul_TrackLen > MAX_TRACK_LEN) )
                    uw_RetVal = API_ERR_SUBPARAM8_NOT_IN_RANGE;
                else if( NULL == px_TrackDefOut )
                    uw_RetVal = API_ERR_PARAM4_IS_NULL;
            }
    }

    if( API_OK == uw_RetVal )
    {
        uw_RetVal = ApiCmdTrackDefEx_(bModule, biu, px_TrackDefIn, px_TrackDefOut);
    }
    
    v_ExamineRetVal( "ApiCmdTrackDefEx", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        TY_API_TRACK_DEF_IN  x_TrackDefIn;\n");
    TRACE("        TY_API_TRACK_DEF_OUT x_TrackDefOut;\n");
    TRACE1("        x_TrackDefIn.l_Offset              = 0x%x;\n", px_TrackDefIn->l_Offset);
    TRACE1("        x_TrackDefIn.ul_BT                 = 0x%x;\n", px_TrackDefIn->ul_BT);
    TRACE1("        x_TrackDefIn.ul_ContinousTrackNb   = 0x%x;\n", px_TrackDefIn->ul_ContinousTrackNb);
    TRACE1("        x_TrackDefIn.ul_Mode               = 0x%x;\n", px_TrackDefIn->ul_Mode);
    TRACE1("        x_TrackDefIn.ul_MultiplexedTrackNb = 0x%x;\n", px_TrackDefIn->ul_MultiplexedTrackNb);
    TRACE1("        x_TrackDefIn.ul_TrackBLen          = 0x%x;\n", px_TrackDefIn->ul_TrackBLen);
    TRACE1("        x_TrackDefIn.ul_TrackBPos          = 0x%x;\n", px_TrackDefIn->ul_TrackBPos);
    TRACE1("        x_TrackDefIn.ul_TrackId            = 0x%x;\n", px_TrackDefIn->ul_TrackId);
    TRACE1("        x_TrackDefIn.ul_TrackLen           = 0x%x;\n", px_TrackDefIn->ul_TrackLen);
    TRACE1("        x_TrackDefIn.ul_TrackStartPos      = 0x%x;\n", px_TrackDefIn->ul_TrackStartPos);
    TRACE1("        x_TrackDefIn.ul_XidRtSa            = 0x%x;\n", px_TrackDefIn->ul_XidRtSa);
    TRACE_FCTA("ApiCmdTrackDefEx", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&x_TrackDefIn");
    TRACE_RPARE("&x_TrackDefOut");
    TRACE_FCTE;
    TRACE1("    --> x_TrackDefOut.ul_TrackBufferStartAddr = 0x%x\n", px_TrackDefOut->ul_TrackBufferStartAddr);
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackRead
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.05.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//
//    Outputs   : Instruction execution status [status]
//                Buffer Data [data]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt16 uw_MultiplexedTrackIndex,
                                                  AiUInt8 uc_ReadMode, AiUInt8 *puc_DataValid, AiUInt16 *puw_TrackDataWords)
{
    int i = 0;
    AiInt16               uw_RetVal = API_OK;
    TY_API_TRACK_READ_IN  x_TrackReadIn;
    TY_API_TRACK_READ_OUT x_TrackReadOut;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( uc_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( uw_MultiplexedTrackIndex > (MAX_MULTI_TRACK_NB-1) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( uc_ReadMode > MAX_TRACK_READ_MODE )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( NULL == puc_DataValid )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if( NULL == puw_TrackDataWords )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
        x_TrackReadOut.puw_TrackDataWords      = puw_TrackDataWords;

        x_TrackReadIn.ul_TrackId               = uc_TrackId;
        x_TrackReadIn.ul_MultiplexedTrackIndex = uw_MultiplexedTrackIndex;
        x_TrackReadIn.ul_ReadMode              = uc_ReadMode;

        uw_RetVal = ApiCmdTrackReadEx_(bModule, biu, &x_TrackReadIn, &x_TrackReadOut);
        if( API_OK == uw_RetVal )
        {
            *puc_DataValid      = (AiUInt8)x_TrackReadOut.ul_DataValid;
        }
    }

    v_ExamineRetVal( "ApiCmdTrackRead", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt8  uc_DataValid;\n");
    TRACE("        AiUInt16 auw_TrackDataWords[32];\n");
    TRACE_FCTA("ApiCmdTrackRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(uc_TrackId);
    TRACE_PARA(uw_MultiplexedTrackIndex);
    TRACE_PARA(uc_ReadMode);
    TRACE_RPARA("&uc_DataValid");
    TRACE_RPARE("auw_TrackDataWords");
    TRACE_FCTE;
    for(i=0; i<32; i++)
    {
    	TRACE1("    --> auw_TrackDataWords[%02d] = ", i); TRACE2("0x%x\n", puw_TrackDataWords[i]);
    }
    TRACE1("    --> uc_DataValid           = 0x%x\n", *puc_DataValid);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackReadEx
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 23.01.08
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//
//    Outputs   : Instruction execution status [status]
//                Buffer Data [data]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    
//
//***************************************************************************
AiInt16 ApiCmdTrackReadEx_(AiUInt32 bModule, AiUInt8 biu, 
                                                    TY_API_TRACK_READ_IN *px_TrackReadIn,
                                                    TY_API_TRACK_READ_OUT *px_TrackReadOut )
{
    AiInt16    uw_RetVal       = API_OK;
    AiUInt16 * puwOutputMemory = NULL;

    TY_MIL_COM_LS_SYS_TRACK_READ_INPUT    xInput;
    TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT * pxOutput = NULL;


    puwOutputMemory = (AiUInt16*)AiOsMalloc(MAX_TG_CMD_SIZE); /* We dont know how big the output buffer is so we use the maximum */
    pxOutput        = (TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT*)puwOutputMemory;


    xInput.id    = px_TrackReadIn->ul_TrackId;
    xInput.index = px_TrackReadIn->ul_MultiplexedTrackIndex;
    xInput.mode  = px_TrackReadIn->ul_ReadMode;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_TRACK_READ, sizeof(xInput), MAX_TG_CMD_SIZE );

    uw_RetVal = API_CMD_STRUCT_SYS_TRACK_READ( bModule, &xInput, pxOutput );

    if( API_OK == uw_RetVal )
    {
    	px_TrackReadOut->ul_DataValid      = pxOutput->valid;
    	px_TrackReadOut->ul_LastTT         = pxOutput->last_time_tag;
    	memcpy( px_TrackReadOut->puw_TrackDataWords, &pxOutput->data, pxOutput->count * sizeof(AiUInt16) );
    }

    AiOsFree(puwOutputMemory);

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackReadEx(AiUInt32 bModule, AiUInt8 biu, 
                                                    TY_API_TRACK_READ_IN *px_TrackReadIn,
                                                    TY_API_TRACK_READ_OUT *px_TrackReadOut )
{
    int        i               = 0;
    AiInt16    uw_RetVal       = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_TrackReadIn )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( px_TrackReadIn->ul_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_SUBPARAM1_NOT_IN_RANGE;
    else if( px_TrackReadIn->ul_MultiplexedTrackIndex > (MAX_MULTI_TRACK_NB-1) )
        uw_RetVal = API_ERR_SUBPARAM2_NOT_IN_RANGE;
    else if( px_TrackReadIn->ul_ReadMode > MAX_TRACK_READ_MODE )
        uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
    else if( NULL == px_TrackReadOut )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == px_TrackReadOut->puw_TrackDataWords )
        uw_RetVal = API_ERR_SUBPARAM6_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdTrackReadEx_(bModule, biu, px_TrackReadIn, px_TrackReadOut);
    }

    v_ExamineRetVal( "ApiCmdTrackReadEx", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_TRACK_READ_IN  x_TrackReadIn;\n");
    TRACE("        TY_API_TRACK_READ_OUT x_TrackReadOut;\n");
    TRACE1("        x_TrackReadIn.ul_MultiplexedTrackIndex = 0x%x;\n", px_TrackReadIn->ul_MultiplexedTrackIndex);
    TRACE1("        x_TrackReadIn.ul_ReadMode              = 0x%x;\n", px_TrackReadIn->ul_ReadMode);
    TRACE1("        x_TrackReadIn.ul_TrackId               = 0x%x;\n", px_TrackReadIn->ul_TrackId);
    TRACE_FCTA("ApiCmdTrackReadEx", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&x_TrackDefIn");
    TRACE_RPARE("&x_TrackDefOut");
    TRACE_FCTE;
    for(i=0; i<32; i++)
    {
    	TRACE1("    --> x_TrackReadOut.puw_TrackDataWords[%02d] = ", i); TRACE2("0x%x\n", px_TrackReadOut->puw_TrackDataWords[i]);
    }
    TRACE1("    --> x_TrackReadOut.ul_DataValid           = 0x%x\n", px_TrackReadOut->ul_DataValid);
    TRACE1("    --> x_TrackReadOut.ul_LastTT              = 0x%x\n", px_TrackReadOut->ul_LastTT);
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackScan
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 23.01.08
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : 
//
//    Description :
//    
//
//***************************************************************************
AiInt16 ApiCmdTrackScan_(AiUInt32 bModule, AiUInt8 biu, 
                                                  TY_API_TRACK_SCAN_IN *px_TrackScanIn,
                                                  TY_API_TRACK_SCAN_OUT *px_TrackScanOut )
{
	AiInt16 uw_RetVal = API_OK;
	AiUInt32 ul_Index;
	AiUInt16 * puwOutputMemory = NULL;

	TY_MIL_COM_LS_SYS_TRACK_SCAN_INPUT    xInput;
	TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT * pxOutput = NULL;

	puwOutputMemory = (AiUInt16*)AiOsMalloc(MAX_TG_CMD_SIZE); /* We dont know how big the output buffer is so we use the maximum */
	pxOutput        = (TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT*)puwOutputMemory;


	xInput.id    = px_TrackScanIn->ul_TrackId;
	xInput.index = px_TrackScanIn->ul_ChunkNb;
	xInput.size  = px_TrackScanIn->ul_ChunkSize;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_TRACK_SCAN, sizeof(xInput), MAX_TG_CMD_SIZE );

	uw_RetVal = API_CMD_STRUCT_SYS_TRACK_SCAN( bModule, &xInput, pxOutput );

	if( API_OK == uw_RetVal )
	{
		px_TrackScanOut->ul_NumberOfReturnedStates = pxOutput->count;
		px_TrackScanOut->ul_MoreData               = pxOutput->more;

		for( ul_Index=0; ul_Index<px_TrackScanIn->ul_ChunkSize; ul_Index++ )
		{
			if( ul_Index<px_TrackScanOut->ul_NumberOfReturnedStates )
				px_TrackScanOut->pul_ReturnedStates[ul_Index] = pxOutput->data[ul_Index];
			else
				px_TrackScanOut->pul_ReturnedStates[ul_Index] = 0xFFFFFFFF;
		}
	}
    AiOsFree(puwOutputMemory);

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackScan(AiUInt32 bModule, AiUInt8 biu, 
                                                  TY_API_TRACK_SCAN_IN *px_TrackScanIn,
                                                  TY_API_TRACK_SCAN_OUT *px_TrackScanOut )
{
    AiUInt32 i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_TrackScanIn )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( px_TrackScanIn->ul_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_SUBPARAM1_NOT_IN_RANGE;
    else if( (px_TrackScanIn->ul_ChunkSize < 1) || (px_TrackScanIn->ul_ChunkSize > MAX_TRACK_CHUNKSIZE) )
        uw_RetVal = API_ERR_SUBPARAM3_NOT_IN_RANGE;
    else if( NULL == px_TrackScanOut )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == px_TrackScanOut->pul_ReturnedStates )
        uw_RetVal = API_ERR_SUBPARAM6_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdTrackScan_(bModule, biu, px_TrackScanIn, px_TrackScanOut);
    }

    v_ExamineRetVal( "ApiCmdTrackScan", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_TRACK_SCAN_IN  x_TrackScanIn;\n");
    TRACE("        TY_API_TRACK_SCAN_OUT x_TrackScanOut;\n");
    TRACE1("        x_TrackScanIn.ul_ChunkNb   = 0x%x;\n", px_TrackScanIn->ul_ChunkNb);
    TRACE1("        x_TrackScanIn.ul_ChunkSize = 0x%x;\n", px_TrackScanIn->ul_ChunkSize);
    TRACE1("        x_TrackScanIn.ul_TrackId   = 0x%x;\n", px_TrackScanIn->ul_TrackId);
    TRACE_FCTA("ApiCmdTrackScan", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&x_TrackScanIn");
    TRACE_RPARE("&x_TrackScanOut");
    TRACE_FCTE;
    for(i=0; i< px_TrackScanIn->ul_ChunkSize; i++)
    {
    	TRACE1("    --> x_TrackScanOut.pul_ReturnedStates[%02d] = ", i); TRACE2("0x%x\n", px_TrackScanOut->pul_ReturnedStates[i]);
    }
    TRACE1("    --> x_TrackReadOut.ul_MoreData               = 0x%x\n", px_TrackScanOut->ul_MoreData);
    TRACE1("    --> x_TrackReadOut.ul_NumberOfReturnedStates = 0x%x\n", px_TrackScanOut->ul_NumberOfReturnedStates);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APILSBUF                 Submodule : ApiCmdTrackPreAlloc
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 29.02.08
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : 
//
//    Description :
//    
//
//***************************************************************************
AiInt16 ApiCmdTrackPreAlloc_(AiUInt32 bModule, AiUInt8 biu, 
                                                      TY_API_TRACK_PREALLOC_IN *px_TrackPreAllocIn,
                                                      TY_API_TRACK_PREALLOC_OUT *px_TrackPreAllocOut )
{
	AiUInt32 i = 0;
	AiInt16 uw_RetVal = API_OK;
	AiUInt32 ul_Index;

	AiUInt16 * puwInputMemory  = NULL;
	AiUInt16 * puwOutputMemory = NULL;

	TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT  * pxInput;
	TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT * pxOutput;

	AiUInt32 ulInputSize  = sizeof(TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT ) + px_TrackPreAllocIn->ul_PreAllocNb * sizeof(AiUInt16);
	AiUInt32 ulOutputSize = sizeof(TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT) + px_TrackPreAllocIn->ul_PreAllocNb * sizeof(AiUInt32);

	puwInputMemory  =  (AiUInt16*)AiOsMalloc( ulInputSize );
	puwOutputMemory =  (AiUInt16*)AiOsMalloc( ulOutputSize );

	pxInput         = (TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT*)puwInputMemory;
	pxOutput        = (TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT*)puwOutputMemory;

	MIL_COM_INIT( &pxInput->cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_TRACK_ALLOC, ulInputSize, ulOutputSize );

	pxInput->id    = px_TrackPreAllocIn->ul_TrackId;
	pxInput->count = px_TrackPreAllocIn->ul_PreAllocNb;

	for( i=0; i<pxInput->count; i++ )
		pxInput->states[i] = px_TrackPreAllocIn->pul_MuxStates[i];

	pxInput->cmd.ulSwapControl = pxInput->count << 16;

	uw_RetVal = API_CMD_STRUCT_SYS_TRACK_ALLOC( bModule, pxInput, pxOutput );

	for( ul_Index = 0; ul_Index<px_TrackPreAllocIn->ul_PreAllocNb; ul_Index++ )
	{
		/* Set output for compatibility 0 => no more memory */
		if( ul_Index < pxOutput->count )
			px_TrackPreAllocOut->pul_TrackBufferStartAddr[ul_Index] = pxOutput->buffer_start[ul_Index];
		else
			px_TrackPreAllocOut->pul_TrackBufferStartAddr[ul_Index] = 0;
	}

    AiOsFree(puwInputMemory);
    AiOsFree(puwOutputMemory);

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTrackPreAlloc(AiUInt32 bModule, AiUInt8 biu, 
                                                      TY_API_TRACK_PREALLOC_IN *px_TrackPreAllocIn,
                                                      TY_API_TRACK_PREALLOC_OUT *px_TrackPreAllocOut )
{
    AiUInt32 i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_TrackPreAllocIn )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == px_TrackPreAllocIn->pul_MuxStates )
        uw_RetVal = API_ERR_SUBPARAM3_IS_NULL;
    else if( px_TrackPreAllocIn->ul_TrackId > MAX_TRACK_ID )
        uw_RetVal = API_ERR_SUBPARAM1_NOT_IN_RANGE;
    else if( (px_TrackPreAllocIn->ul_PreAllocNb < 1) || (px_TrackPreAllocIn->ul_PreAllocNb > MAX_TRACK_PREALLOC_STATES) )
        uw_RetVal = API_ERR_SUBPARAM2_NOT_IN_RANGE;
    else if( NULL == px_TrackPreAllocOut )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == px_TrackPreAllocOut->pul_TrackBufferStartAddr )
        uw_RetVal = API_ERR_SUBPARAM4_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdTrackPreAlloc_(bModule, biu, px_TrackPreAllocIn, px_TrackPreAllocOut);
    }

    v_ExamineRetVal( "ApiCmdTrackPreAlloc", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_TRACK_PREALLOC_IN  x_TrackPreAllocIn;\n");
    TRACE("        TY_API_TRACK_PREALLOC_OUT x_TrackPreAllocOut;\n");
    TRACE1("        x_TrackPreAllocIn.ul_PreAllocNb     = 0x%x;\n", px_TrackPreAllocIn->ul_PreAllocNb);
    TRACE1("        x_TrackPreAllocIn.ul_TrackId        = 0x%x;\n", px_TrackPreAllocIn->ul_TrackId);
    for(i=0; i<px_TrackPreAllocIn->ul_PreAllocNb; i++)
    {
    	TRACE1("        x_TrackPreAllocIn.pul_MuxStates[%02d] = ", i); TRACE2("0x%x;\n", px_TrackPreAllocIn->pul_MuxStates[i]);
    }
    TRACE_FCTA("ApiCmdTrackPreAlloc", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&x_TrackPreAllocIn");
    TRACE_RPARE("&x_TrackPreAllocOut");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

