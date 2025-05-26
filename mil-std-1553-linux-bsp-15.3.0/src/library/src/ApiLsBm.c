/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsBm.c
 *
 * LS BM functions
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


#include "AiMilCom.h"
#include "AiMapDllToTsw.h"


extern AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel);



//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***          B U S    M O N I T O R     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMIni_(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM            xInput;
    TY_MIL_COM_ACK        xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIni(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdBMIni_(bModule, biu);
    }

    v_ExamineRetVal( "ApiCmdBMIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMIntrMode
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_INTR' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMIntrMode_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 imod, AiUInt8 smod, AiUInt8 res)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BM_INI_INTR_INPUT    xInput;
    TY_MIL_COM_ACK                     xOutput;

    xInput.imod = imod;
    xInput.smod = smod;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_INTR, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_INTR( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIntrMode(AiUInt32 bModule, AiUInt8 biu, AiUInt8 imod, AiUInt8 smod, AiUInt8 res)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( imod > MAX_BM_IMODE )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( smod > MAX_BM_SMODE )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( res != 0 )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMIntrMode_(bModule, biu, imod, smod, res);
    }

    v_ExamineRetVal( "ApiCmdBMIntrMode", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMIntrMode", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(imod);
    TRACE_PARA(smod);
    TRACE_PARE(res);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMCapMode
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_CAP' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMCapMode_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_CAP_SETUP *pcap)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BM_INI_CAP_INPUT     xInput;
    TY_MIL_COM_ACK                     xOutput;

    xInput.cmod  = pcap->cap_mode;
    xInput.tat   = pcap->cap_tat;
    xInput.mcc   = pcap->cap_mcc;
    xInput.fsize = pcap->cap_fsize;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_CAP, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_CAP( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMCapMode(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_CAP_SETUP *pcap)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pcap )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( pcap->cap_mode > MAX_BM_CAPMODE )
        uw_RetVal = API_ERR_CAPMODE_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMCapMode_( bModule, biu, pcap);
    }

    v_ExamineRetVal( "ApiCmdBMCapMode", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_CAP_SETUP cap;\n");
    TRACE1("        cap.cap_fsize = 0x%x;\n", pcap->cap_fsize);
    TRACE1("        cap.cap_mcc   = 0x%x;\n", pcap->cap_mcc);
    TRACE1("        cap.cap_mode  = 0x%x;\n", pcap->cap_mode);
    TRACE1("        cap.cap_tat   = 0x%x;\n", pcap->cap_tat);
    TRACE1("        cap.padding1  = 0x%x;\n", pcap->padding1);
    TRACE1("        cap.padding2  = 0x%x;\n", pcap->padding2);
    TRACE_FCTA("ApiCmdBMCapMode", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&cap");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMSWXMIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_SWXM' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMSWXMIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 swxm)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BM_INI_SWXM_INPUT     xInput;
    TY_MIL_COM_ACK                     xOutput;

    xInput.swxm  = swxm;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_SWXM, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_SWXM( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMSWXMIni(AiUInt32 bModule, AiUInt8 biu, AiUInt16 swxm)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdBMSWXMIni_(bModule, biu, swxm);
    }

    v_ExamineRetVal( "ApiCmdBMSWXMIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMSWXMIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARE(swxm);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMStart
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_START' instruction to the API1553-DS
//    ASP Driver SW to start the BM mode immediately.
//
//***************************************************************************

AiInt16 ApiCmdBMStart_(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM            xInput;
    TY_MIL_COM_ACK        xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_START, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_START( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStart(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdBMStart_(bModule, biu);
    }

    v_ExamineRetVal( "ApiCmdBMStart", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMStart", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMHalt
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_HALT' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMHalt_(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM            xInput;
    TY_MIL_COM_ACK        xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_HALT, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_HALT( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMHalt(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdBMHalt_( bModule, biu );
    }

    v_ExamineRetVal( "ApiCmdBMHalt", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMHalt", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMStatusRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 29.06.05
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_STATUS_READ' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMStatusRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_STATUS_DSP *pdsp)
{
    AiInt16 uw_RetVal = API_OK;

    TY_MIL_COM                      xInput;
    TY_MIL_COM_LS_BM_STATUS_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_STATUS_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_STATUS_READ( bModule, &xInput, &xOutput );
    pdsp->men      = (AiUInt8)xOutput.men;
    pdsp->msw      = (AiUInt8)xOutput.msw;
    pdsp->msp      = (AiUInt8)xOutput.msp;
    pdsp->padding1 = 0;

    pdsp->glb_msg_cnt      = xOutput.glb_msg_cnt;
    pdsp->glb_err_cnt      = xOutput.glb_err_cnt;
    pdsp->glb_word_cnt_sec = xOutput.glb_word_cnt_sec;
    pdsp->glb_word_cnt_pri = xOutput.glb_word_cnt_pri;

    pdsp->glb_msg_cnt_sec  = xOutput.glb_msg_cnt_sec;
    pdsp->glb_msg_cnt_pri  = xOutput.glb_msg_cnt_pri;
    pdsp->glb_err_cnt_sec  = xOutput.glb_err_cnt_sec;
    pdsp->glb_err_cnt_pri  = xOutput.glb_err_cnt_pri;

    pdsp->bus_load_sec     = xOutput.bus_load_sec;
    pdsp->bus_load_pri     = xOutput.bus_load_pri;
    pdsp->bus_load_sec_avg = xOutput.bus_load_sec_avg;
    pdsp->bus_load_pri_avg = xOutput.bus_load_pri_avg;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStatusRead(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_STATUS_DSP *pdsp)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pdsp )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdBMStatusRead_(bModule, biu, pdsp);
    }

    v_ExamineRetVal( "ApiCmdBMStatusRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_STATUS_DSP dsp;\n");
    TRACE_FCTA("ApiCmdBMStatusRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&dsp");
    TRACE_FCTE;
    TRACE1("    --> dsp.bus_load_pri     = 0x%x\n", pdsp->bus_load_pri);
    TRACE1("    --> dsp.bus_load_pri_avg = 0x%x\n", pdsp->bus_load_pri_avg);
    TRACE1("    --> dsp.bus_load_sec     = 0x%x\n", pdsp->bus_load_sec);
    TRACE1("    --> dsp.bus_load_sec_avg = 0x%x\n", pdsp->bus_load_sec_avg);
    TRACE1("    --> dsp.glb_err_cnt      = 0x%x\n", pdsp->glb_err_cnt);
    TRACE1("    --> dsp.glb_err_cnt_pri  = 0x%x\n", pdsp->glb_err_cnt_pri);
    TRACE1("    --> dsp.glb_err_cnt_sec  = 0x%x\n", pdsp->glb_err_cnt_sec);
    TRACE1("    --> dsp.glb_msg_cnt      = 0x%x\n", pdsp->glb_msg_cnt);
    TRACE1("    --> dsp.glb_msg_cnt_pri  = 0x%x\n", pdsp->glb_msg_cnt_pri);
    TRACE1("    --> dsp.glb_msg_cnt_sec  = 0x%x\n", pdsp->glb_msg_cnt_sec);
    TRACE1("    --> dsp.glb_word_cnt_pri = 0x%x\n", pdsp->glb_word_cnt_pri);
    TRACE1("    --> dsp.glb_word_cnt_sec = 0x%x\n", pdsp->glb_word_cnt_sec);
    TRACE1("    --> dsp.men              = 0x%x\n", pdsp->men);
    TRACE1("    --> dsp.msp              = 0x%x\n", pdsp->msp);
    TRACE1("    --> dsp.msw              = 0x%x\n", pdsp->msw);
    TRACE1("    --> dsp.padding1         = 0x%x\n", pdsp->padding1);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMActRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_ACT_READ' instruction to the API1553-DS

//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMActRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 entry_no, TY_API_BM_ACT_DSP *pact)
{
    AiInt16 uw_RetVal = API_OK;

    TY_MIL_COM_LS_BM_ACT_READ_INPUT    xInput;
    TY_MIL_COM_LS_BM_ACT_READ_OUTPUT  xOutput;

    xInput.entry_no = entry_no;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_ACT_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_ACT_READ( bModule, &xInput, &xOutput );

    pact->cc  = xOutput.cc;
    pact->ec  = xOutput.ec;
    pact->et  = xOutput.et;
    pact->fnd = (AiUInt8)xOutput.fnd;
    pact->rt  = (AiUInt8)xOutput.rt;
    pact->sa  = (AiUInt8)xOutput.sa;
    pact->tr  = (AiUInt8)xOutput.tr;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMActRead(AiUInt32 bModule, AiUInt8 biu, AiUInt16 entry_no, TY_API_BM_ACT_DSP *pact)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( entry_no > MAX_BM_ACT_ENTRY )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == pact )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMActRead_(bModule, biu, entry_no, pact);
    }

    v_ExamineRetVal( "ApiCmdBMActRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_ACT_DSP act;\n");
    TRACE_FCTA("ApiCmdBMActRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(entry_no);
    TRACE_RPARE("&act");
    TRACE_FCTE;
    TRACE1("    --> act.cc  = 0x%x\n", pact->cc);
    TRACE1("    --> act.ec  = 0x%x\n", pact->ec);
    TRACE1("    --> act.et  = 0x%x\n", pact->et);
    TRACE1("    --> act.fnd = 0x%x\n", pact->fnd);
    TRACE1("    --> act.rt  = 0x%x\n", pact->rt);
    TRACE1("    --> act.sa  = 0x%x\n", pact->sa);
    TRACE1("    --> act.tr  = 0x%x\n", pact->tr);
    TRACE_END
    
    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMStackpRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_PTR_READ' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMStackpRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_STACKP_DSP *pstackp)
{
    TY_MIL_COM                             xInput;
    TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT xOutput;

    AiInt16 uw_RetVal = API_OK;


    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_STACK_PTR_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_STACK_PTR_READ( bModule, &xInput, &xOutput );

    pstackp->ctp       = xOutput.ctp;
    pstackp->etp       = xOutput.etp;
    pstackp->padding1  = 0;
    pstackp->padding2  = 0;
    pstackp->status    = (AiUInt8)xOutput.status;
    pstackp->stp       = xOutput.stp;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStackpRead(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_STACKP_DSP *pstackp)
{
	AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pstackp )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMStackpRead_(bModule, biu, pstackp);
    }

    v_ExamineRetVal( "ApiCmdBMStackpRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_STACKP_DSP stackp;\n");
    TRACE_FCTA("ApiCmdBMStackpRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&stackp");
    TRACE_FCTE;
    TRACE1("    --> stackp.ctp      = 0x%x\n", pstackp->ctp);
    TRACE1("    --> stackp.etp      = 0x%x\n", pstackp->etp);
    TRACE1("    --> stackp.padding1 = 0x%x\n", pstackp->padding1);
    TRACE1("    --> stackp.padding2 = 0x%x\n", pstackp->padding2);
    TRACE1("    --> stackp.status   = 0x%x\n", pstackp->status);
    TRACE1("    --> stackp.stp      = 0x%x\n", pstackp->stp);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMStackEntryRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_ENTRY_READ' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMStackEntryRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype, AiUInt8 sgn, 
								AiUInt32 offset, TY_API_BM_STACK_DSP *pentry)
{
    TY_MIL_COM_LS_BM_STACK_ENTRY_READ_INPUT  xInput;
    TY_MIL_COM_LS_BM_STACK_ENTRY_READ_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_STACK_ENTRY_READ, sizeof(xInput), sizeof(xOutput) );

    xInput.mode   = ptype;
    xInput.sgn    = sgn;
    xInput.offset = offset;

    uw_RetVal = API_CMD_STRUCT_BM_STACK_ENTRY_READ( bModule, &xInput, &xOutput );

    pentry->entry      = xOutput.bm_stack_entry;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStackEntryRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype,
                                             AiUInt8 sgn, AiUInt32 offset, TY_API_BM_STACK_DSP *pentry)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( ptype > MAX_BM_STACK_ENTRY_READMODE )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sgn > MAX_BM_STACK_ENTRY_SIGN )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (ptype < API_BM_READ_ABS) && (offset > MAX_BM_STACK_ENTRY_OFFSET) )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( NULL == pentry )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMStackEntryRead_(bModule, biu, ptype, sgn, offset, pentry);
    }

    v_ExamineRetVal( "ApiCmdBMStackEntryRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_STACK_DSP entry;\n");
    TRACE_FCTA("ApiCmdBMStackEntryRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(ptype);
    TRACE_PARA(sgn);
    TRACE_PARA(offset);
    TRACE_RPARE("&entry");
    TRACE_FCTE;
    TRACE1("    --> entry.entry = 0x%x\n", pentry->entry);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMStackEntryFind
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_ENTRY_FIND' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMStackEntryFind_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype, AiUInt8 sgn, AiUInt32 offset,
                                             AiUInt16 fspec, AiUInt16 fdata, AiUInt16 fmask, TY_API_BM_STACK_FND *pfnd)
{
    TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_INPUT  xInput;
    TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_STACK_ENTRY_FIND, sizeof(xInput), sizeof(xOutput) );

    xInput.mode   = ptype;
    xInput.sgn    = sgn;
    xInput.offset = offset;
    xInput.spec   = fspec;
    xInput.etw    = fdata;
    xInput.etm    = fmask;

    uw_RetVal = API_CMD_STRUCT_BM_STACK_ENTRY_FIND( bModule, &xInput, &xOutput );

    pfnd->efnd      = xOutput.fnd;
    pfnd->padding1  = 0;
    pfnd->padding2  = 0;
    pfnd->eptr      = xOutput.eptr;
    pfnd->entry     = xOutput.entry;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMStackEntryFind(AiUInt32 bModule, AiUInt8 biu, AiUInt8 ptype,
                                             AiUInt8 sgn, AiUInt32 offset,
                                             AiUInt16 fspec, AiUInt16 fdata, AiUInt16 fmask,
                                             TY_API_BM_STACK_FND *pfnd)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( ptype >= API_BM_READ_ABS )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sgn > MAX_BM_STACK_ENTRY_SIGN )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (ptype < API_BM_READ_ABS) && (offset > MAX_BM_STACK_ENTRY_OFFSET) )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( (fspec & API_BM_SEARCH_ENTRY_TYPE_MASK) > MAX_BM_ENTRY_TYPE )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( NULL == pfnd )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMStackEntryFind_(bModule, biu, ptype, sgn, offset, fspec, fdata, fmask, pfnd);
    }

    v_ExamineRetVal( "ApiCmdBMStackEntryFind", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_STACK_FND fnd;\n");
    TRACE_FCTA("ApiCmdBMStackEntryFind", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(ptype);
    TRACE_PARA(sgn);
    TRACE_PARA(offset);
    TRACE_PARA(fspec);
    TRACE_PARA(fdata);
    TRACE_PARA(fmask);
    TRACE_RPARE("&fnd");
    TRACE_FCTE;
    TRACE1("    --> fnd.efnd     = 0x%x\n", pfnd->efnd);
    TRACE1("    --> fnd.entry    = 0x%x\n", pfnd->entry);
    TRACE1("    --> fnd.eptr     = 0x%x\n", pfnd->eptr);
    TRACE1("    --> fnd.padding1 = 0x%x\n", pfnd->padding1);
    TRACE1("    --> fnd.padding2 = 0x%x\n", pfnd->padding2);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMRecStatusRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_REC_STATUS_READ' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMRecStatusRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_REC *prec)
{

    TY_MIL_COM                             xInput;
    TY_MIL_COM_LS_BM_REC_STATUS_OUTPUT     xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BM_REC_STATUS_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_REC_STATUS_READ( bModule, &xInput, &xOutput );

    prec->hfi_cnt   = xOutput.hfi_cnt;
    prec->padding1  = 0;
    prec->padding2  = 0;
    prec->saddr     = xOutput.saddr;
    prec->size      = xOutput.size;
    prec->status    = xOutput.status;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRecStatusRead(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_REC *prec)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == prec )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
	{
        uw_RetVal = ApiCmdBMRecStatusRead_(bModule, biu, prec);
    }

    v_ExamineRetVal( "ApiCmdBMRecStatusRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_REC rec;\n");
    TRACE_FCTA("ApiCmdBMRecStatusRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&rec");
    TRACE_FCTE;
    TRACE1("    --> rec.hfi_cnt  = 0x%x\n", prec->hfi_cnt);
    TRACE1("    --> rec.padding1 = 0x%x\n", prec->padding1);
    TRACE1("    --> rec.padding2 = 0x%x\n", prec->padding2);
    TRACE1("    --> rec.saddr    = 0x%x\n", prec->saddr);
    TRACE1("    --> rec.size     = 0x%x\n", prec->size);
    TRACE1("    --> rec.status   = 0x%x\n", prec->status);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMCopyRecDat
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_COPY_REC_DAT' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMCopyRecDat_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_REC *prec)
{
    TY_MIL_COM_LS_BM_COPY_REC_DAT_INPUT   xInput;
    TY_MIL_COM_ACK                        xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.res          = 0;
    xInput.start_offset = prec->saddr;
    xInput.size         = prec->size;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_COPY_REC_DAT, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_COPY_REC_DAT( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMCopyRecDat(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_REC *prec)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == prec )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMCopyRecDat_(bModule, biu, prec);
    }

    v_ExamineRetVal( "ApiCmdBMCopyRecDat", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_REC rec;\n");
    TRACE1("        rec.hfi_cnt  = 0x%x;\n", prec->hfi_cnt);
    TRACE1("        rec.padding1 = 0x%x;\n", prec->padding1);
    TRACE1("        rec.padding2 = 0x%x;\n", prec->padding2);
    TRACE1("        rec.saddr    = 0x%x;\n", prec->saddr);
    TRACE1("        rec.size     = 0x%x;\n", prec->size);
    TRACE1("        rec.status   = 0x%x;\n", prec->status);
    TRACE_FCTA("ApiCmdBMCopyRecDat", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&rec");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMFilterIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_MSG_FLT' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMFilterIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                        AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    TY_MIL_COM_LS_BM_INI_MSG_FLT_INPUT    xInput;
    TY_MIL_COM_ACK                        xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.rt     = rt_addr;
    xInput.rcv_sa = rx_sa;
    xInput.xmt_sa = tx_sa;
    xInput.rcv_mc = rx_mc;
    xInput.xmt_mc = tx_mc;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_MSG_FLT, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_MSG_FLT( bModule, &xInput, &xOutput );

    return uw_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMFilterIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                        AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( rt_addr > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMFilterIni_(bModule, biu, rt_addr, rx_sa, tx_sa, rx_mc, tx_mc);
    }

    v_ExamineRetVal( "ApiCmdBMFilterIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMFilterIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(rx_sa);
    TRACE_PARA(tx_sa);
    TRACE_PARA(rx_mc);
    TRACE_PARE(tx_mc);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMIllegalIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_MSG_ILL' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMIllegalIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                         AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    TY_MIL_COM_LS_BM_INI_MSG_ILL_INPUT    xInput;
    TY_MIL_COM_ACK                        xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.rt     = rt_addr;
    xInput.rcv_sa = rx_sa;
    xInput.xmt_sa = tx_sa;
    xInput.rcv_mc = rx_mc;
    xInput.xmt_mc = tx_mc;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_MSG_ILL, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_MSG_ILL( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIllegalIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                         AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( rt_addr > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMIllegalIni_(bModule, biu, rt_addr, rx_sa, tx_sa, rx_mc, tx_mc);
    }

    v_ExamineRetVal( "ApiCmdBMIllegalIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMIllegalIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(rx_sa);
    TRACE_PARA(tx_sa);
    TRACE_PARA(rx_mc);
    TRACE_PARE(tx_mc);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMFTWIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_FTW' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMFTWIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 htm, AiUInt8 htc, AiUInt8 stm, AiUInt8 stc)
{
    TY_MIL_COM_LS_BM_INI_FTW_INPUT    xInput;
    TY_MIL_COM_ACK                    xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.con     = con;
    xInput.htm     = htm;
    xInput.htc     = htc;
    xInput.stm     = stm;
    xInput.stc     = stc;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_FTW, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_FTW( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMFTWIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 htm, AiUInt8 htc, AiUInt8 stm, AiUInt8 stc)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( con > MAX_BM_FTW_CON )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMFTWIni_(bModule, biu, con, htm, htc, stm, stc); 
    }

    v_ExamineRetVal( "ApiCmdBMFTWIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMFTWIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(htm);
    TRACE_PARA(htc);
    TRACE_PARA(stm);
    TRACE_PARE(stc);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMTIWIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_TIW' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMTIWIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 eti, AiUInt8 aei, AiUInt8 ti0, AiUInt8 ti1)
{
    TY_MIL_COM_LS_BM_INI_TIW_INPUT    xInput;
    TY_MIL_COM_ACK                    xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.con     = con;
    xInput.eti     = eti;
    xInput.aei     = aei;
    xInput.ti0     = ti0;
    xInput.ti1     = ti1;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_TIW, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_TIW( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTIWIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 eti, AiUInt8 aei, AiUInt8 ti0, AiUInt8 ti1)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( con > MAX_BM_TIW_CON )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMTIWIni_(bModule, biu, con, eti, aei, ti0, ti1);
    }

    v_ExamineRetVal( "ApiCmdBMTIWIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMTIWIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(eti);
    TRACE_PARA(aei);
    TRACE_PARA(ti0);
    TRACE_PARE(ti1);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMTCIIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_TCI' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMTCIIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa,
                                     AiUInt8 sa_type, AiUInt8 tagm, AiUInt8 tfct, AiUInt8 tid)
{
    TY_MIL_COM_LS_BM_INI_TCI_INPUT    xInput;
    TY_MIL_COM_ACK                    xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.rt      = rt;
    xInput.sa_mc   = sa;
    xInput.sa_type = sa_type;
    xInput.tagm    = tagm;
    xInput.tfct    = tfct;
    xInput.tid     = tid;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_TCI, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_TCI( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTCIIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa,
                                     AiUInt8 sa_type, AiUInt8 tagm, AiUInt8 tfct, AiUInt8 tid)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sa > MAX_RT_MODECODE )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( sa_type > MAX_RT_SA_TYPE )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( tagm != 0 )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( tfct != 0 )
        uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMTCIIni_(bModule, biu, rt, sa, sa_type, tagm, tfct, tid);
    }

    v_ExamineRetVal( "ApiCmdBMTCIIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBMTCIIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt);
    TRACE_PARA(sa);
    TRACE_PARA(sa_type);
    TRACE_PARA(tagm);
    TRACE_PARA(tfct);
    TRACE_PARE(tid);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMTCBIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_INI_TCB' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdBMTCBIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 tid, AiUInt8 ten, TY_API_BM_TCB *ptcb)
{
    TY_MIL_COM_LS_BM_INI_TCB_INPUT    xInput;
    TY_MIL_COM_ACK                    xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.tid      = tid;
    xInput.ten      = ten;
    xInput.tt       = ptcb->tt;
    xInput.sot      = ptcb->sot;
    xInput.tri      = ptcb->tri;
    xInput.inv      = ptcb->inv;
    xInput.tres     = ptcb->tres;
    xInput.tset     = ptcb->tset;
    xInput.tsp      = ptcb->tsp;
    xInput.next     = ptcb->next;
    xInput.eom      = ptcb->eom;
    xInput.tdw      = ptcb->tdw;
    xInput.tmw      = ptcb->tmw;
    xInput.tuli     = ptcb->tuli;
    xInput.tlli     = ptcb->tlli;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_TCB, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_TCB( bModule, &xInput, &xOutput );

    return uw_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMTCBIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 tid, AiUInt8 ten,
                                     TY_API_BM_TCB *ptcb)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( ten > API_ENA )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( NULL == ptcb )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else if( ptcb->tt > MAX_TCB_TT )
        uw_RetVal = API_ERR_TCBTT_NOT_IN_RANGE;
    else if( ptcb->sot > MAX_TCB_SOT )
        uw_RetVal = API_ERR_TCBSOT_NOT_IN_RANGE;
    else if( ptcb->tri > MAX_TCB_TRI )
        uw_RetVal = API_ERR_TCBTRI_NOT_IN_RANGE;
    else if( ptcb->inv > MAX_TCB_INV )
        uw_RetVal = API_ERR_TCBINV_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMTCBIni_(bModule, biu, tid, ten, ptcb);
    }

    v_ExamineRetVal( "ApiCmdBMTCBIni", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_TCB tcb;\n");
    TRACE1("        tcb.eom      = 0x%x;\n", ptcb->eom);
    TRACE1("        tcb.inv      = 0x%x;\n", ptcb->inv);
    TRACE1("        tcb.next     = 0x%x;\n", ptcb->next);
    TRACE1("        tcb.padding1 = 0x%x;\n", ptcb->padding1);
    TRACE1("        tcb.sot      = 0x%x;\n", ptcb->sot);
    TRACE1("        tcb.tdw      = 0x%x;\n", ptcb->tdw);
    TRACE1("        tcb.tlli     = 0x%x;\n", ptcb->tlli);
    TRACE1("        tcb.tmw      = 0x%x;\n", ptcb->tmw);
    TRACE1("        tcb.tres     = 0x%x;\n", ptcb->tres);
    TRACE1("        tcb.tri      = 0x%x;\n", ptcb->tri);
    TRACE1("        tcb.tset     = 0x%x;\n", ptcb->tset);
    TRACE1("        tcb.tsp      = 0x%x;\n", ptcb->tsp);
    TRACE1("        tcb.tt       = 0x%x;\n", ptcb->tt);
    TRACE1("        tcb.tuli     = 0x%x;\n", ptcb->tuli);
    TRACE_FCTA("ApiCmdBMTCBIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(tid);
    TRACE_PARA(ten);
    TRACE_RPARE("&tcb");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMRTActRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_RT_ACT_READ' instruction to the
//    API1553-DS ASP Driver SW to read the BM RT message/error counter.
//
//***************************************************************************

AiInt16 ApiCmdBMRTActRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, TY_API_BM_RT_ACT *pact)
{
    TY_MIL_COM_LS_BM_RT_ACT_READ_INPUT    xInput;
    TY_MIL_COM_LS_BM_RT_ACT_READ_OUTPUT   xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.rt       = rt;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_RT_ACT_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_RT_ACT_READ( bModule, &xInput, &xOutput );

    pact->ec = xOutput.ec;
    pact->et = xOutput.et;
    pact->mc = xOutput.cc;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRTActRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, TY_API_BM_RT_ACT *pact)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == pact )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMRTActRead_(bModule, biu, rt, pact);
    }

    v_ExamineRetVal( "ApiCmdBMRTActRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_RT_ACT act;\n");
    TRACE_FCTA("ApiCmdBMRTActRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt);
    TRACE_RPARE("&act");
    TRACE_FCTE;
    TRACE1("    --> act.ec = 0x%x\n", pact->ec);
    TRACE1("    --> act.et = 0x%x\n", pact->et);
    TRACE1("    --> act.mc = 0x%x\n", pact->mc);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMRTSAActRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_RT_SA_ACT_READ' instruction to the
//    API1553-DS ASP Driver SW to read the BM RT SA message/error counter.
//
//***************************************************************************
AiInt16 ApiCmdBMRTSAActRead_( AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa, AiUInt8 sa_type, TY_API_BM_RT_ACT * pact )
{
    TY_MIL_COM_LS_BM_RT_SA_ACT_READ_INPUT    xInput;
    TY_MIL_COM_LS_BM_RT_SA_ACT_READ_OUTPUT   xOutput;
    AiInt16 uw_RetVal      = API_OK;

    xInput.rt       = rt;
    xInput.sa       = sa;
    xInput.sa_type  = sa_type;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_RT_SA_ACT_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_RT_SA_ACT_READ( bModule, &xInput, &xOutput );

    pact->ec = xOutput.ec;
    pact->et = xOutput.et;
    pact->mc = xOutput.cc;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMRTSAActRead( AiUInt32           bModule, 
                                          AiUInt8            biu, 
                                          AiUInt8            rt, 
                                          AiUInt8            sa, 
                                          AiUInt8            sa_type,
                                          TY_API_BM_RT_ACT * pact )
{
    AiInt16 uw_RetVal      = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sa > MAX_RT_MODECODE )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( sa_type > API_RT_TYPE_ALL )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( NULL == pact )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMRTSAActRead_( bModule, biu, rt, sa, sa_type, pact );
    }

    v_ExamineRetVal( "ApiCmdBMRTSAActRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_RT_ACT act;\n");
    TRACE_FCTA("ApiCmdBMRTSAActRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt);
    TRACE_PARA(sa);
    TRACE_PARA(sa_type);
    TRACE_RPARE("&act");
    TRACE_FCTE;
    TRACE1("    --> act.ec = 0x%x\n", pact->ec);
    TRACE1("    --> act.et = 0x%x\n", pact->et);
    TRACE1("    --> act.mc = 0x%x\n", pact->mc);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMIniMsgFltRec
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.05.99   Update : 01.05.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Amount of Filter Command Words [cnt]
//                Pointer to Message Filter Input Array [pmrec]
//                         - Filter Command Word (MS-Byte first) [cw]
//                 - First Data Word position in message 'cw' [pos1]
//                 - Last Data Word position in message 'cw' [pos2]
//
//    Outputs   : Instruction acknowledge type [ackfl]
//
//    Description :
//    This function applies the 'API_BM_INI_MSG_FLT_REC' instruction to the
//    API1553-DS ASP Driver SW to init the BM Message Filter Recording
//
//***************************************************************************

AiInt16 ApiCmdBMIniMsgFltRec_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cnt, TY_API_BM_INI_MSG_FLT_REC *pmrec)
{
    TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT  * pxInput;
    TY_MIL_COM_ACK                          xOutput;
    AiUInt32 inputSize = sizeof(TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT) + (cnt * 3 * sizeof(AiUInt8));
    int i,j;
    AiInt16 uw_RetVal = API_OK;

    pxInput = (TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT*)AiOsMalloc( inputSize );

    pxInput->cnt    = cnt;

    for(i=0, j=0; j<cnt; i += 4, j++)
    {
    	pxInput->mrec[i+0] = (AiUInt8)(pmrec[j].cw >> 8);
    	pxInput->mrec[i+1] = (AiUInt8) pmrec[j].cw;
    	pxInput->mrec[i+2] = pmrec[j].pos1;
    	pxInput->mrec[i+3] = pmrec[j].pos2;
    }

    MIL_COM_INIT( &pxInput->cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_INI_MSG_REC_FLT, inputSize, sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_INI_MSG_REC_FLT( bModule, pxInput, &xOutput );

    AiOsFree( pxInput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMIniMsgFltRec(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cnt,
                                           TY_API_BM_INI_MSG_FLT_REC *pmrec)
{
    int i;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (cnt < 1) || (cnt > MAX_API_BM_MSG_FLT_REC) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == pmrec )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        for( i=0; i<cnt; i++ )
        {
            if( API_OK == uw_RetVal )
            {
                if( pmrec[i].pos1 > MAX_WCNT+1 )
                    uw_RetVal = API_ERR_POS1_NOT_IN_RANGE;
                else if( pmrec[i].pos2 > MAX_WCNT+1 )
                    uw_RetVal = API_ERR_POS2_NOT_IN_RANGE;
                else if( pmrec[i].pos1 > pmrec[i].pos2 )
                    uw_RetVal = API_ERR_POS1_GREATER_THAN_POS2;
            }
            else
                break;
        }
    }

    if( API_OK == uw_RetVal )
    {
        uw_RetVal = ApiCmdBMIniMsgFltRec_(bModule, biu, cnt, pmrec);
    }

    v_ExamineRetVal( "ApiCmdBMIniMsgFltRec", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_INI_MSG_FLT_REC mrec[MAX_API_BM_MSG_FLT_REC];\n");
    for(i=0; i<MAX_API_BM_MSG_FLT_REC; i++)
    {
        TRACE1("        mrec[%02d].cw   = ", i); TRACE2("0x%x;\n", pmrec[i].cw);
        TRACE1("        mrec[%02d].pos1 = ", i); TRACE2("0x%x;\n", pmrec[i].pos1);
        TRACE1("        mrec[%02d].pos2 = ", i); TRACE2("0x%x;\n", pmrec[i].pos2);
    }
    TRACE_FCTA("ApiCmdBMIniMsgFltRec", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(cnt);
    TRACE_RPARE("mrec");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMCopyMsgFltRecData
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.05.99   Update : 01.05.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Size of allocated buffer in Words [max_size]
//
//    Outputs   : Instruction acknowledge type [ackfl]
//
//    Description :
//    This function applies the 'API_BM_READ_MSG_FLT_REC' instruction to the
//    API1553-DS ASP Driver SW to read the BM Message Filter Recording data
//
//***************************************************************************
AiInt16 ApiCmdBMCopyMsgFltRecData_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode,
                                  AiUInt8 con, AiUInt32 max_size, AiUInt32 max_msg,
                                  AiUInt8 *ovfl, AiUInt32 *rsize)
{
    TY_MIL_COM_LS_BM_MSG_FLT_REC_INPUT  xInput;
    TY_MIL_COM_LS_BM_MSG_FLT_REC_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.mode     = mode;
    xInput.con      = con;
    xInput.max_size = max_size;
    xInput.max_msg  = max_msg;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_MSG_REC_FLT, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_MSG_REC_FLT( bModule, &xInput, &xOutput );

    *ovfl  = xOutput.ovfl;
    *rsize = xOutput.out_size;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMCopyMsgFltRecData(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode,
                                  AiUInt8 con, AiUInt32 max_size, AiUInt32 max_msg,
                                  AiUInt8 *ovfl, AiUInt32 *rsize)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( mode > MAX_BM_MSGFLT_MODE  )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( con > MAX_BM_MSGFLT_FORMAT  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( NULL == ovfl )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == rsize )
        uw_RetVal = API_ERR_PARAM8_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMCopyMsgFltRecData_(bModule, biu, mode, con, max_size, max_msg, ovfl, rsize);
    }

    v_ExamineRetVal( "ApiCmdBMCopyMsgFltRecData", uw_RetVal );
    TRACE_BEGIN
    TRACE("        AiUInt8  ovfl;\n");
    TRACE("        AiUInt32 rsize;\n");
    TRACE_FCTA("ApiCmdBMCopyMsgFltRecData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(mode);
    TRACE_PARA(con);
    TRACE_PARA(max_size);
    TRACE_PARA(max_msg);
    TRACE_RPARA("&ovfl");
    TRACE_RPARE("&rsize");
    TRACE_FCTE;
    TRACE1("    --> ovfl  = 0x%x;\n", *ovfl);
    TRACE1("    --> rsize = 0x%x;\n", *rsize);
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : ApiLsBM                  Submodule : ApiCmdBMReadMsgFltRec
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.05.99   Update : 08.01.01
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Size of allocated buffer in Words [max_size]
//
//    Outputs   : Instruction acknowledge type [ackfl]
//
//    Description :
//    This function applies the 'API_BM_READ_MSG_FLT_REC' instruction to the
//    API1553-DS ASP Driver SW to read the BM Message Filter Recording data
//
//***************************************************************************
AiInt16 ApiCmdBMReadMsgFltRec_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode,
                                            AiUInt8 con, AiUInt32 max_size, AiUInt16 max_msg,
                                            void *lpBuf, AiUInt8 *ovfl, AiUInt32 *lWordsRead)
{
    AiUInt32 loc_bytes_read = 0;
    AiUInt8 loc_ovfl = 0;
    AiUInt32 loc_words_read = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
    	return API_ERR_NO_MODULE_EXTENSION;

    _SyncWait(bModule, pDevice->_hRecMutex);

    uw_RetVal = ApiCmdBMCopyMsgFltRecData(bModule, biu, mode, con, max_size, max_msg, &loc_ovfl, &loc_words_read);

    if (loc_words_read != 0L)  {
        uw_RetVal = ApiReadBlockMemData(bModule, API_MEMTYPE_SHARED, API_REP_REC_AREA_OFFSET, 1, lpBuf, loc_words_read*2, &loc_bytes_read);
#if _AIM_VME
    	if( !pDevice->_avi1553BoardVwl.ul_EmulatedSHAR )
    		_StructureEndianChange(STR_UINT16_TYPE, lpBuf, loc_words_read);
#endif
    }

    /* Release recording mutex */
    _SyncRelease(bModule, pDevice->_hRecMutex );

    *lWordsRead = loc_words_read;
    *ovfl = loc_ovfl;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMReadMsgFltRec(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode,
                                            AiUInt8 con, AiUInt32 max_size, AiUInt16 max_msg,
                                            void *lpBuf, AiUInt8 *ovfl, AiUInt32 *lWordsRead)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( mode > MAX_BM_MSGFLT_MODE  )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( con > MAX_BM_MSGFLT_FORMAT  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( NULL == lpBuf )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == ovfl )
        uw_RetVal = API_ERR_PARAM8_IS_NULL;
    else if( NULL == lWordsRead )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBMReadMsgFltRec_(bModule, biu, mode, con, max_size, max_msg, lpBuf, ovfl, lWordsRead);
    }
    v_ExamineRetVal( "ApiCmdBMReadMsgFltRec", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt32 lpBuf[40];\n");
    TRACE("        AiUInt8  ovfl;\n");
    TRACE("        AiUInt32 lWordsRead;\n");
    TRACE_FCTA("ApiCmdBMReadMsgFltRec", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(mode);
    TRACE_PARA(con);
    TRACE_PARA(max_size);
    TRACE_PARA(max_msg);
    TRACE_RPARA("lpBuf");
    TRACE_RPARA("&ovfl");
    TRACE_RPARE("&lWordsRead");
    TRACE_FCTE;
    TRACE1("    --> ovfl       = 0x%x;\n", *ovfl);
    TRACE1("    --> lWordsRead = 0x%x;\n", *lWordsRead);
    for(i=0; i<40; i++)
    {
    	TRACE1("    --> lpBuf[%02d] = ", i); TRACE2("0x%x;\n", ((AiUInt16 *)lpBuf)[i]);
    }
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMDytagMonDef
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 11.11.05   Update : 11.11.05
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_DYTAG_MON_DEF' instruction to the
//    API1553-DS ASP Driver SW to define the BM Dytag Monitor.
//
//***************************************************************************

AiInt16 ApiCmdBMDytagMonDef_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_DYTAG_MON_DEF *px_DytagMon)
{
	TY_MIL_COM_LS_BM_DYNTAG_DEF_INPUT  xInput;
	TY_MIL_COM_ACK                     xOutput;
	AiInt16 uw_RetVal = API_OK;

	xInput.id           = px_DytagMon->uc_Id;
	xInput.con          = px_DytagMon->uc_Con;
	xInput.rt           = px_DytagMon->uc_RtAddr;
	xInput.sa           = px_DytagMon->uc_SubAddrMsgId;
	xInput.sa_type      = px_DytagMon->uc_SubAddrMsgIdType;;
	xInput.dyntag_type  = px_DytagMon->uw_DytagType;
	xInput.dyntag_wpos  = px_DytagMon->uw_DytagWPos;
	xInput.dyntag_bpos  = px_DytagMon->uw_DytagBPos;
	xInput.dyntag_blen  = px_DytagMon->uw_DytagBLen;
	xInput.dyntag_step  = px_DytagMon->uw_DytagStep;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_DYNTAG_MON_DEF, sizeof(xInput), sizeof(xOutput) );

	uw_RetVal = API_CMD_STRUCT_BM_DYNTAG_MON_DEF( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMDytagMonDef(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_DYTAG_MON_DEF *px_DytagMon)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_DytagMon )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( (px_DytagMon->uc_Id < MIN_DYTAG_MON_ID) || (px_DytagMon->uc_Id > MAX_DYTAG_MON_ID) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( px_DytagMon->uc_Con > API_ENA )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( px_DytagMon->uc_RtAddr > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( px_DytagMon->uc_SubAddrMsgId > MAX_RT_MODECODE )
        uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else if( px_DytagMon->uc_SubAddrMsgIdType > MAX_RT_SA_TYPE )
        uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMDytagMonDef_(bModule, biu, px_DytagMon);
    }
    
    v_ExamineRetVal( "ApiCmdBMDytagMonDef", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_DYTAG_MON_DEF x_DytagMon;\n");
    TRACE1("        x_DytagMon.uc_Con              = 0x%x;\n", px_DytagMon->uc_Con);
    TRACE1("        x_DytagMon.uc_Id               = 0x%x;\n", px_DytagMon->uc_Id);
    TRACE1("        x_DytagMon.uc_Padding1         = 0x%x;\n", px_DytagMon->uc_Padding1);
    TRACE1("        x_DytagMon.uc_RtAddr           = 0x%x;\n", px_DytagMon->uc_RtAddr);
    TRACE1("        x_DytagMon.uc_SubAddrMsgId     = 0x%x;\n", px_DytagMon->uc_SubAddrMsgId);
    TRACE1("        x_DytagMon.uc_SubAddrMsgIdType = 0x%x;\n", px_DytagMon->uc_SubAddrMsgIdType);
    TRACE1("        x_DytagMon.uw_DytagBLen        = 0x%x;\n", px_DytagMon->uw_DytagBLen);
    TRACE1("        x_DytagMon.uw_DytagBPos        = 0x%x;\n", px_DytagMon->uw_DytagBPos);
    TRACE1("        x_DytagMon.uw_DytagStep        = 0x%x;\n", px_DytagMon->uw_DytagStep);
    TRACE1("        x_DytagMon.uw_DytagType        = 0x%x;\n", px_DytagMon->uw_DytagType);
    TRACE1("        x_DytagMon.uw_DytagWPos        = 0x%x;\n", px_DytagMon->uw_DytagWPos);
    TRACE_FCTA("ApiCmdBMDytagMonDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&x_DytagMon");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBMDytagMonRead
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 11.11.05   Update : 11.11.05
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BM_DYTAG_MON_READ' instruction to the
//    API1553-DS ASP Driver SW to read the actual BM Dytag Monitor status.
//
//***************************************************************************

AiInt16 ApiCmdBMDytagMonRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_DYTAG_MON_READ_CTRL *px_DytagMonCtrl, TY_API_BM_DYTAG_MON_ACT *px_DytagMonAct)
{
    TY_MIL_COM_LS_BM_DYNTAG_MON_READ_INPUT  xInput;
    TY_MIL_COM_LS_BM_DYNTAG_MON_READ_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.id           = px_DytagMonCtrl->uc_Id;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BM_DYNTAG_MON_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_BM_DYNTAG_MON_READ( bModule, &xInput, &xOutput );

    px_DytagMonAct->ul_Stale = xOutput.stale_cnt;
    px_DytagMonAct->ul_Good  = xOutput.good_cnt;
    px_DytagMonAct->ul_Bad   = xOutput.bad_cnt;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBMDytagMonRead(AiUInt32 bModule, AiUInt8 biu, TY_API_BM_DYTAG_MON_READ_CTRL *px_DytagMonCtrl, TY_API_BM_DYTAG_MON_ACT *px_DytagMonAct)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_DytagMonCtrl )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == px_DytagMonAct )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( (px_DytagMonCtrl->uc_Id < MIN_DYTAG_MON_ID) || (px_DytagMonCtrl->uc_Id > MAX_DYTAG_MON_ID) )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBMDytagMonRead_(bModule, biu, px_DytagMonCtrl, px_DytagMonAct);
    }

    v_ExamineRetVal( "ApiCmdBMDytagMonRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BM_DYTAG_MON_READ_CTRL x_DytagMonCtrl;\n");
    TRACE("        TY_API_BM_DYTAG_MON_ACT       x_DytagMonAct;\n");
    TRACE1("        x_DytagMonCtrl.uc_Id = 0x%x;\n", px_DytagMonCtrl->uc_Id);
    TRACE_FCTA("ApiCmdBMDytagMonRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&x_DytagMonCtrl");
    TRACE_RPARE("&x_DytagMonAct");
    TRACE_FCTE;
    TRACE1("    --> x_DytagMonAct.ul_Bad   = 0x%x\n", px_DytagMonAct->ul_Bad);
    TRACE1("    --> x_DytagMonAct.ul_Good  = 0x%x\n", px_DytagMonAct->ul_Good);
    TRACE1("    --> x_DytagMonAct.ul_Stale = 0x%x\n", px_DytagMonAct->ul_Stale);
    TRACE_END

    return uw_RetVal;
}
