/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsCal.c
 *
 * LS coupling functions
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
#include "AiMapDllToTsw.h"



//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***       C A L I B R A T I O N     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdCalSigCon
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : MILbus Channel [bus]
//                Control Calibration signal (enable/disable) [con]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_CAL_SIG_CON' instruction to the API1553-DS
//    ASP Driver SW to control the calibration signal on the selected MILbus.
//
//***************************************************************************

AiInt16 ApiCmdCalSigCon_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 con)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_SYS_CAL_CON_INPUT   xInput;
    TY_MIL_COM_ACK                    xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_CAL_SIG_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.bus       = bus;
    xInput.con       = con;

    uw_RetVal = API_CMD_STRUCT_SYS_CAL_SIG_CON( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalSigCon(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 con)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (bus < API_CAL_BUS_PRIMARY) || (bus > MAX_BUS)  )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( con > API_ENA  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdCalSigCon_(bModule, biu, bus, con);
    }

    v_ExamineRetVal( "ApiCmdCalSigCon", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdCalSigCon", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bus);
    TRACE_PARE(con);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//

//   Module : APC_API                  Submodule : ApiCmdCalCplCon
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : MILbus Channel [bus]
//                MILbus Coupling mode [cpl]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_CAL_CPL_CON' instruction to the API1553-DS
//    ASP Driver SW to control the coupling mode on the selected MILbus.
//
//***************************************************************************

AiInt16 ApiCmdCalCplCon_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 cpl)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM_LS_SYS_CAL_CON_INPUT   xInput;
	TY_MIL_COM_ACK                    xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_CAL_CPL_CON, sizeof(xInput), sizeof(xOutput) );

	xInput.bus       = bus;
	xInput.con       = cpl;

	uw_RetVal = API_CMD_STRUCT_SYS_CAL_CPL_CON( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalCplCon(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 cpl)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (bus < API_CAL_BUS_PRIMARY) || (bus > MAX_BUS)  )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( cpl > MAX_CPL_MODE )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdCalCplCon_(bModule, biu, bus, cpl);
    }
    
    v_ExamineRetVal( "ApiCmdCalCplCon", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdCalCplCon", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bus);
    TRACE_PARE(cpl);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdCalXmtCon
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : MILbus Channel [bus]
//                Amplitude control [amp]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_CAL_XMT_CON' instruction to the API1553-DS
//    ASP Driver SW to control the output amplitude on the selected MILbus.
//
//***************************************************************************

AiInt16 ApiCmdCalXmtCon_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 amp)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM_LS_SYS_CAL_CON_INPUT   xInput;
	TY_MIL_COM_ACK                    xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_CAL_XMT_CON, sizeof(xInput), sizeof(xOutput) );

	xInput.bus       = bus;
	xInput.con       = amp;

	uw_RetVal = API_CMD_STRUCT_SYS_CAL_XMT_CON( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalXmtCon(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bus, AiUInt8 amp)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
    else if ((bus < API_CAL_XMT_FULL_RANGE_PRI) || (bus > API_CAL_XMT_HIGHRES_RANGE))
		uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
	else
	{
		uw_RetVal = ApiCmdCalXmtCon_(bModule, biu, bus, amp);
	}

	v_ExamineRetVal( "ApiCmdCalXmtCon", uw_RetVal );

	TRACE_BEGIN
	TRACE_FCTA("ApiCmdCalXmtCon", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_PARA(bus);
	TRACE_PARE(amp);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}



AiInt16 ApiCmdCalTransCon_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 trans_mode )
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_SYS_CAL_TRANS_INPUT   xInput;
    TY_MIL_COM_ACK                      xOutput;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_SYS_CAL_TRANS, sizeof(xInput), sizeof(xOutput));

    xInput.con = trans_mode;

    uw_RetVal = API_CMD_STRUCT_SYS_CAL_TRANS(bModule, &xInput, &xOutput);

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdCalTransCon(AiUInt32 bModule, AiUInt8 biu, AiUInt8 trans_mode)
{
    AiInt16 uw_RetVal = API_OK;

    if ((0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)))
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdCalTransCon_(bModule, biu, trans_mode);
    }

    v_ExamineRetVal("ApiCmdCalTransCon", uw_RetVal);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdCalTransCon", uw_RetVal);
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(trans_mode);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

