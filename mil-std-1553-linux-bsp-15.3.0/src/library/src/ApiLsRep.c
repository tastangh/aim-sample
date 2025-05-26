/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsRep.c
 *
 * LS replay functions
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

//***************************************************************************
//
//   Module : ApiLsRep                  Submodule : ApiCmdReplayIni
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 01.08.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Clear Entry Bit [cet],
//                No Replay Count [nct],
//                Cyclic operation [cyc],
//                No Intermessage gap [nimg],
//                Absolute Long Timetag Replay [alt],
//                Relative Long Tag [rlt],
//                Replay Interrupt Mode [rint],
//                        Absolute Minute Offset [min],
//                        Absolute Microsecond Offset [msec],
//                        Offset sign [sign]
//                Recording File Size in Bytes[fsize]
//
//    Outputs   : Instruction execution status [status],
//
//    Description :
//    This function applies the 'TG_API_REP_INI' instruction to the API1553-DS
//    ASP Driver SW to initialize the Replay Mode.
//
//***************************************************************************

AiInt16 ApiCmdReplayIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cet, AiUInt8 nct, AiUInt8 cyc, AiUInt8 nimg,
                         AiUInt8 alt, AiUInt8 rlt, AiUInt8 rint, AiUInt32 min, AiUInt32 msec, AiUInt8 sign, AiUInt32 fsize)
{
	AiInt16 uw_RetVal = API_OK;
	AiUInt32 ul_CalculatedBiu = 0;
	AiChar buf[128];

	TY_MIL_COM_REP_INI_INPUT    xInput;
	TY_MIL_COM_ACK              xOutput;

	TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
	if( pDevice == NULL )
		return API_ERR_NO_MODULE_EXTENSION;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_REP_INI, sizeof(xInput), sizeof(xOutput) );

	xInput.cet   = cet;
	xInput.nct   = nct;
	xInput.cyc   = cyc;
	xInput.nimg  = nimg;
	xInput.alt   = alt;
	xInput.rlt   = rlt;
	xInput.rint  = rint;
	xInput.min   = min;
	xInput.msec  = msec;
	xInput.sign  = sign;
	xInput.fsize = fsize;

	uw_RetVal = API_CMD_STRUCT_REP_INI( bModule, &xInput, &xOutput );

	uw_ApiGetBiuFromStream( bModule, biu, &ul_CalculatedBiu);

	sprintf( buf, "[%d]\n", ul_CalculatedBiu-1 );
	DEBUGOUT( DBG_WRITEREP, __FUNCTION__, buf );

	/* the following flag is used in WriteRepArea() as indicator, whether replay buffer 1 or 2 is active */
	pDevice->auc_ReplayFlag[ ul_CalculatedBiu-1 ] = 0; 

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 cet,
                                                  AiUInt8 nct, AiUInt8 cyc, AiUInt8 nimg,
                                                  AiUInt8 alt, AiUInt8 rlt, AiUInt8 rint,
                                                  AiUInt32 min, AiUInt32 msec, AiUInt8 sign, AiUInt32 fsize)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else if( cet != API_REP_CET )
		uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
	else if( alt > API_REP_ABS_TIME )
		uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
	else if( rlt > API_REP_RLT_ALL )
		uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
	else if( rint > API_REP_HFI_INT )
		uw_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
	else
	{
		uw_RetVal = ApiCmdReplayIni_(bModule, biu, cet, nct, cyc, nimg, alt, rlt, rint, min, msec, sign, fsize);
	}

	v_ExamineRetVal( "ApiCmdReplayIni", uw_RetVal );

	TRACE_BEGIN
	TRACE_FCTA("ApiCmdReplayIni", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_PARA(cet);
	TRACE_PARA(nct);
	TRACE_PARA(cyc);
	TRACE_PARA(nimg);
	TRACE_PARA(alt);
	TRACE_PARA(rlt);
	TRACE_PARA(rint);
	TRACE_PARA(min);
	TRACE_PARA(msec);
	TRACE_PARA(sign);
	TRACE_PARE(fsize);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRep                  Submodule : ApiCmdReplayStart
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 01.08.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_REP_START' instruction to the API1553-DS
//    ASP Driver SW to start the Replay mode immediately.
//
//***************************************************************************

AiInt16 ApiCmdReplayStart_(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM      xInput;
	TY_MIL_COM_ACK  xOutput;

	MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_REP_START, sizeof(xInput), sizeof(xOutput) );

	uw_RetVal = API_CMD_STRUCT_REP_START( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStart(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else
	{
		uw_RetVal = ApiCmdReplayStart_(bModule, biu);
	}

	v_ExamineRetVal( "ApiCmdReplayStart", uw_RetVal );

	TRACE_BEGIN
	TRACE_FCTA("ApiCmdReplayStart", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARE(biu);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRep                  Submodule : ApiCmdReplayStop
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 01.08.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_REP_HALT' instruction to the API1553-DS
//    ASP Driver SW to initialize the BM mode.
//
//***************************************************************************

AiInt16 ApiCmdReplayStop_(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM      xInput;
	TY_MIL_COM_ACK  xOutput;

	MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_REP_HALT, sizeof(xInput), sizeof(xOutput) );

	uw_RetVal = API_CMD_STRUCT_REP_HALT( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStop(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else
	{
		uw_RetVal = ApiCmdReplayStop_(bModule, biu);
	}

	v_ExamineRetVal( "ApiCmdReplayStop", uw_RetVal );

	TRACE_BEGIN
	TRACE_FCTA("ApiCmdReplayStop", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARE(biu);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}


//***************************************************************************
//
//   Module : ApiLsRep                  Submodule : ApiCmdReplayStatus
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 01.08.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_REP_STATUS_READ' instruction to the API1553-DS
//    ASP Driver SW to read the Replay Status.
//
//***************************************************************************
AiInt16 ApiCmdReplayStatus_(AiUInt32 bModule, AiUInt8 biu, TY_API_REP_STATUS *prep)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM                   xInput;
	TY_MIL_COM_REP_STATUS_OUTPUT xOutput;

	MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_REP_STATUS, sizeof(xInput), sizeof(xOutput) );

	uw_RetVal = API_CMD_STRUCT_REP_STATUS( bModule, &xInput, &xOutput );

	prep->rpi_cnt   = xOutput.rpi_cnt;
	prep->saddr     = xOutput.saddr;
	prep->size      = xOutput.size;
	prep->entry_cnt = xOutput.raec;
	prep->status    = xOutput.status;
	prep->padding1  = 0;
	prep->padding2  = 0;

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayStatus(AiUInt32 bModule, AiUInt8 biu, TY_API_REP_STATUS *prep)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else if( NULL == prep )
		uw_RetVal = API_ERR_PARAM3_IS_NULL;
	else
	{
		uw_RetVal = ApiCmdReplayStatus_(bModule, biu, prep);
	}

	v_ExamineRetVal( "ApiCmdReplayStatus", uw_RetVal );

	TRACE_BEGIN
	TRACE("        TY_API_REP_STATUS rep;\n");
	TRACE_FCTA("ApiCmdReplayStatus", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_RPARE("&rep");
	TRACE_FCTE;
	TRACE1("    --> rep.entry_cnt = 0x%x\n", prep->entry_cnt);
	TRACE1("    --> rep.padding1  = 0x%x\n", prep->padding1);
	TRACE1("    --> rep.padding2  = 0x%x\n", prep->padding2);
	TRACE1("    --> rep.rpi_cnt   = 0x%x\n", prep->rpi_cnt);
	TRACE1("    --> rep.saddr     = 0x%x\n", prep->saddr);
	TRACE1("    --> rep.size      = 0x%x\n", prep->size);
	TRACE1("    --> rep.status    = 0x%x\n", prep->status);
	TRACE_END

	return uw_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRep                  Submodule : ApiCmdReplayRT
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 01.08.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Replay con [con]
//                RT mode         [mode]
//                RT            [rt]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_REP_RT_CON' instruction to the
//    API1553-DS ASP Driver SW to set the RT Replay mode.
//
//***************************************************************************

AiInt16 ApiCmdReplayRT_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 mode, AiUInt8 rt)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM_REP_RT_CON_INPUT xInput;
	TY_MIL_COM_ACK              xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_REP_RT_CON, sizeof(xInput), sizeof(xOutput) );

	xInput.rt    = rt;
	xInput.mode  = mode;
	xInput.con   = con;

	uw_RetVal = API_CMD_STRUCT_REP_RT_CON( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReplayRT(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 mode, AiUInt8 rt)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( con > API_MODE_SINGLE_RT )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( mode > API_ENA )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( rt > MAX_RT_ADDR )
    	uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else
    {
    	uw_RetVal = ApiCmdReplayRT_(bModule, biu, con, mode, rt);
    }

    v_ExamineRetVal( "ApiCmdReplayRT", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdReplayRT", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(mode);
    TRACE_PARE(rt);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

