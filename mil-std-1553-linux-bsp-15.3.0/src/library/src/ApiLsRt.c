/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsRt.c
 *
 * LS RT functions
 * 
 */



//***************************************************************************
//
//   Module : APC_API                  Submodule : -
//
//   Author : Marco Motz               Project   : API1553-DS
//
//   Source : C                        Tools     : PC/AT;
//                                                 Borland C/C++ 5.02
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 16.01.01
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Comprised Functions (name of function  / name of source file)
//
//    Description : APC Command module of the Application Interface
//
//***************************************************************************

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

#include <string.h>
#include "DEFS_V.H"
#include "Dll_Fnc.h"

#include "AiMilCom.h"
#include "AiMapDllToTsw.h"


//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***       R E M O T E    T E R M I N A L    C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Operation Control [con],
//                RT Bus Response Control [bus],
//                RT Response Time in usec [resp_time],
//                Next RT ul_RetVal Word [nxw]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_INI' instruction to the API1553-DS
//    ASP Driver SW to initialize the specified RT.
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                              AiUInt8 con, AiUInt8 bus, AiFloat resp_time, AiUInt16 nxw)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( bus > MAX_RT_RESPONSE )
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( (resp_time < MIN_RESP_TIME) || (resp_time > MAX_RESP_TIME) )
        ul_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTIni_(bModule, biu, rt_addr, con, bus, resp_time, nxw);
    }
    
    v_ExamineRetVal( "ApiCmdRTIni", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTIni", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(con);
    TRACE_PARA(bus);
    TRACE_FPARA(resp_time);
    TRACE_PARE(nxw);
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

AiReturn ApiCmdRTIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                              AiUInt8 con, AiUInt8 bus, AiFloat resp_time, AiUInt16 nxw)
{
	AiReturn ul_RetVal = API_OK;
	TY_MIL_COM_LS_RT_INI_INPUT   xInput;
	TY_MIL_COM_ACK               xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_INI, sizeof(xInput), sizeof(xOutput) );

	xInput.rt_addr   = rt_addr;
	xInput.con       = con;
	xInput.bus       = bus;
	xInput.resp_time = (AiUInt8)(resp_time * 4);
	xInput.nxw       = nxw;

	ul_RetVal = API_CMD_STRUCT_RT_INI( bModule, &xInput, &xOutput );

	return ul_RetVal;
}



//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTEnaDis
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 30.07.02   Update : 30.07.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Operation Control [con],
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_ENA_DIS' instruction to the API1553-DS
//    ASP Driver SW to enable or disable the specified RT on the fly.
//
//***************************************************************************

AiReturn ApiCmdRTEnaDis_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 con)
{
    AiReturn                     ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_CON_INPUT   xInput;
    TY_MIL_COM_ACK               xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr   = rt_addr;
    xInput.con       = con;

    ul_RetVal = API_CMD_STRUCT_RT_CON( bModule, &xInput, &xOutput );

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTEnaDis(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 con)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else
    {
    	ul_RetVal = ApiCmdRTEnaDis_(bModule, biu, rt_addr, con);
    }

    v_ExamineRetVal( "ApiCmdRTEnaDis", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTEnaDis", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARE(con);
    TRACE_FCTE;
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTRespTime
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Response Time in usec [resp_time]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_INI_RESP' instruction to the API1553-DS
//    ASP Driver SW to initialize the response time on the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTRespTime_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat resp_time)
{
    AiReturn                        ul_RetVal = API_OK;
	TY_MIL_COM_LS_RT_INI_RESP_INPUT xInput;
	TY_MIL_COM_ACK                  xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_INI_RESP, sizeof(xInput), sizeof(xOutput) );

	xInput.rt_addr   = rt_addr;
	xInput.resp_time = (AiUInt8)(resp_time * 4);

	ul_RetVal = API_CMD_STRUCT_RT_INI_RESP( bModule, &xInput, &xOutput );

	return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTRespTime(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat resp_time)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
    	ul_RetVal = API_ERR_WRONG_BIU;
    else if( (resp_time < MIN_RESP_TIME) || (resp_time > MAX_RESP_TIME) )
    	ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
    	ul_RetVal = ApiCmdRTRespTime_(bModule, biu, rt_addr, resp_time);
    }

    v_ExamineRetVal( "ApiCmdRTRespTime", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTRespTime", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_FPARE(resp_time);
    TRACE_FCTE;
    TRACE_END

    return ul_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTRespTimeGet
//
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//
//    Outputs   : Instruction execution status [ul_RetVal]
//                RT Response Time in usec [resp_time]
//
//    Description :
//    This function reads the response time of a given RT
//
//***************************************************************************

AiReturn ApiCmdRTRespTimeGet_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat *pResp_time)
{
    AiReturn ul_RetVal = API_OK;
    AiUInt32 ul_MemType, ul_Offset;
    AiUInt32 ul_RTDescriptorWord;

    ul_RetVal = ApiCmdSysGetMemLocation_( bModule, biu, TY_GETMEM_RT_DESCR, rt_addr, &ul_MemType, &ul_Offset);
    if( ul_RetVal == API_OK )
    {
        ApiReadMemData( bModule, ul_MemType, ul_Offset, 4, &ul_RTDescriptorWord);

        if( pResp_time )
            *pResp_time = ((ul_RTDescriptorWord & 0x00FF0000)>>16) * (AiFloat)0.25;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTRespTimeGet(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiFloat *pResp_time)
{
	AiReturn ul_RetVal = API_OK;

	ul_RetVal = ApiCmdRTRespTimeGet_(bModule, biu, rt_addr, pResp_time);

	v_ExamineRetVal( "ApiCmdRTRespTimeGet", ul_RetVal );
	TRACE_BEGIN
	TRACE_FCTA("ApiCmdRTRespTimeGet", ul_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_PARA(rt_addr);
	TRACE_RPARE("&pResp_time");
	TRACE_FCTE;
	TRACE_END

	return ul_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTNXW
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                Next RT ul_RetVal Word [nxw]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_INI_NXW' instruction to the API1553-DS
//    ASP Driver SW to initialize the next status word of the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTNXW_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 nxw)
{
    AiReturn                       ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_INI_NXW_INPUT xInput;
    TY_MIL_COM_ACK                 xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_INI_NXW, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr = rt_addr;
    xInput.nxw     = nxw;

    ul_RetVal = API_CMD_STRUCT_RT_INI_NXW( bModule, &xInput, &xOutput );

    return ul_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTNXW(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 nxw)
{
    AiReturn ul_RetVal = API_OK;

	if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
		ul_RetVal = API_ERR_WRONG_MODULE;
	else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		ul_RetVal = API_ERR_WRONG_BIU;
	else
	{
		ul_RetVal = ApiCmdRTNXW_(bModule, biu, rt_addr, nxw);
	}

	v_ExamineRetVal( "ApiCmdRTNXW", ul_RetVal );
	TRACE_BEGIN
	TRACE_FCTA("ApiCmdRTNXW", ul_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_PARA(rt_addr);
	TRACE_PARE(nxw);
	TRACE_FCTE;
	TRACE_END

	return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTLCW
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Last Command Word [lcw]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_INI_LCW' instruction to the API1553-DS
//    ASP Driver SW to initialize the Last Command Word of the specified RT:
//
//***************************************************************************

AiReturn ApiCmdRTLCW_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lcw)
{
	AiReturn ul_RetVal = API_OK;
	TY_MIL_COM_LS_RT_INI_LCW_INPUT xInput;
	TY_MIL_COM_ACK                 xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_INI_LCW, sizeof(xInput), sizeof(xOutput) );

	xInput.rt_addr = rt_addr;
	xInput.lcw     = lcw;

	ul_RetVal = API_CMD_STRUCT_RT_INI_LCW( bModule, &xInput, &xOutput );

	return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTLCW(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lcw)
{
	AiReturn ul_RetVal = API_OK;

	if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
		ul_RetVal = API_ERR_WRONG_MODULE;
	else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		ul_RetVal = API_ERR_WRONG_BIU;
	else
	{
		ul_RetVal = ApiCmdRTLCW_(bModule, biu, rt_addr, lcw);
	}

	v_ExamineRetVal( "ApiCmdRTLCW", ul_RetVal );
	TRACE_BEGIN
	TRACE_FCTA("ApiCmdRTLCW", ul_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARA(biu);
	TRACE_PARA(rt_addr);
	TRACE_PARE(lcw);
	TRACE_FCTE;
	TRACE_END

	return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTLSW
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Last ul_RetVal Word [lsw]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_INI_LSW' instruction to the API1553-DS
//    ASP Driver SW to initialize the Last Status Word of the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTLSW_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lsw)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_INI_LSW_INPUT xInput;
    TY_MIL_COM_ACK                 xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_INI_LSW, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr = rt_addr;
    xInput.lsw     = lsw;

    ul_RetVal = API_CMD_STRUCT_RT_INI_LSW( bModule, &xInput, &xOutput );

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTLSW(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lsw)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else
    {
        ul_RetVal = ApiCmdRTLSW_(bModule, biu, rt_addr, lsw);
    }

    v_ExamineRetVal( "ApiCmdRTLSW", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTLSW", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARE(lsw);
    TRACE_FCTE;
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTBHDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 08.01.01
//---------------------------------------------------------------------------
//    Descriptions

//    ------------
//    Inputs    : RT Buffer Header ID [hid],
//                RT Data Buffer ID [bid],
//                RT ul_RetVal Queue ID [sid],
//                RT Event Queue ID [eid],
//                Queue Size definition [qsize],
//                Buffer Queue Mode [bqm],
//                Buffer Store Mode [bsm],
//                ul_RetVal Queue Mode [sqm],
//                Event Queue Mode [eqm],
//                Reserved [res]
//
//    Outputs   : Instruction execution status [ul_RetVal],
//                Buffer Header Info structure [pbh]
//
//    Description :
//    This function applies the 'API_RT_BH_DEF' instruction to the API1553-DS
//    ASP Driver SW to setup the specified RT Buffer Header descriptor.
//
//***************************************************************************

AiReturn ApiCmdRTBHDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid, AiUInt16 bid, AiUInt16 sid, AiUInt16 eid,
                       AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm, AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                       TY_API_RT_BH_INFO *pbh)
{
	AiReturn ul_RetVal = API_OK;
	TY_MIL_COM_LS_RT_BH_DEF_INPUT  xInput;
	TY_MIL_COM_LS_RT_BH_DEF_OUTPUT xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_BH_DEF, sizeof(xInput), sizeof(xOutput) );

	xInput.hid   = hid;
	xInput.bid   = bid;
	xInput.sid   = sid;
	xInput.eid   = eid;
	xInput.qsize = qsize;
	xInput.bqm   = bqm;
	xInput.bsm   = bsm;
	xInput.sqm   = sqm;
	xInput.eqm   = 0;  /*API_RT_BH_EVS_NO_EQ*/
	xInput.res   = res;

	ul_RetVal = API_CMD_STRUCT_RT_BH_DEF( bModule, &xInput, &xOutput );

	if( (ul_RetVal==0) && pbh )
	{
		pbh->bid       = xOutput.bid;
		pbh->sid       = xOutput.sid;
		pbh->eid       = xOutput.eid;
		pbh->nbufs     = xOutput.nbufs;
		pbh->hid_addr  = xOutput.hid_addr;
		pbh->bq_addr   = xOutput.bid_addr;
		pbh->sq_addr   = xOutput.sid_addr;
		pbh->eq_addr   = xOutput.eid_addr;
	}

	return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTBHDef(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid,
                                                AiUInt16 bid, AiUInt16 sid, AiUInt16 eid,
                                                AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                                                TY_API_RT_BH_INFO *pbh)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( qsize > MAX_QUEUE_SIZE )
        ul_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else if( bqm > API_BQM_HOST_CONTROLLED )
        ul_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
    else if( bsm > API_BSM_RX_KEEP_CURRENT )
        ul_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
    else if( sqm > API_SQM_AS_QSIZE )
        ul_RetVal = API_ERR_PARAM10_NOT_IN_RANGE;
    else if( eqm != 0 )
        ul_RetVal = API_ERR_PARAM11_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTBHDef_(bModule, biu, hid, bid, sid, eid, qsize, bqm, bsm, sqm, eqm, res, pbh);
    }
    
    v_ExamineRetVal( "ApiCmdRTBHDef", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_BH_INFO pbh;\n");
    TRACE_FCTA("ApiCmdRTBHDef", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(hid);
    TRACE_PARA(bid);
    TRACE_PARA(sid);
    TRACE_PARA(eid);
    TRACE_PARA(qsize);
    TRACE_PARA(bqm);
    TRACE_PARA(bsm);
    TRACE_PARA(sqm);
    TRACE_PARA(eqm);
    TRACE_PARA(res);
    TRACE_RPARE("&pbh");
    TRACE_FCTE;
    TRACE1("    --> pbh.bid      = 0x%x\n", pbh->bid);
    TRACE1("    --> pbh.bq_addr  = 0x%x\n", pbh->bq_addr);
    TRACE1("    --> pbh.eid      = 0x%x\n", pbh->eid);
    TRACE1("    --> pbh.eq_addr  = 0x%x\n", pbh->eq_addr);
    TRACE1("    --> pbh.hid_addr = 0x%x\n", pbh->hid_addr);
    TRACE1("    --> pbh.nbufs    = 0x%x\n", pbh->nbufs);
    TRACE1("    --> pbh.sid      = 0x%x\n", pbh->sid);
    TRACE1("    --> pbh.sq_addr  = 0x%x\n", pbh->sq_addr);
    TRACE_END
    
    return ul_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSACon
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Subaddress/Modecode [sa],
//                RT Buffer Header ID [hid],
//                RT Subaddress Type [sa_type],
//                RT Subaddress/Modecode Control (Enable/Disable) [con],
//                RT Response Mode [rmod],
//                RT ul_RetVal Word Mask Control [smod],
//                RT ul_RetVal Word Modification Mask [swm]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_SA_CON' instruction to the API1553-DS
//    ASP Driver SW to control the specified subaddress/modecode of the
//    specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTSACon_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 sa, AiUInt16 hid,
                       AiUInt8 sa_type, AiUInt8 con, AiUInt8 rmod, AiUInt8 smod, AiUInt16 swm)
{
    AiReturn                        ul_RetVal = API_OK;
	TY_MIL_COM_LS_RT_SA_CON_INPUT   xInput;
	TY_MIL_COM_ACK                  xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_SA_CON, sizeof(xInput), sizeof(xOutput) );

	xInput.rt_addr = rt_addr;
	xInput.sa_mc   = sa;
	xInput.sa_hid  = hid;
	xInput.sa_type = sa_type;
	xInput.con     = con;
	xInput.rmod    = rmod;
	xInput.smod    = smod;
	xInput.swm     = swm;

	ul_RetVal = API_CMD_STRUCT_RT_SA_CON( bModule, &xInput, &xOutput );

	return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSACon(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                                AiUInt8 sa, AiUInt16 hid, AiUInt8 sa_type, AiUInt8 con,
                                                AiUInt8 rmod, AiUInt8 smod, AiUInt16 swm)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( sa > MAX_RT_MODECODE )
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (hid < MIN_HEADER_ID) || (hid > MAX_HEADER_ID ) )
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( sa_type > MAX_RT_SA_TYPE )
        ul_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( smod > MAX_SW_MASK_CONTROL )
        ul_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTSACon_(bModule, biu, rt_addr, sa, hid, sa_type, con, rmod, smod, swm);
    }
    
    v_ExamineRetVal( "ApiCmdRTSACon", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTSACon", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(sa);
    TRACE_PARA(hid);
    TRACE_PARA(sa_type);
    TRACE_PARA(con);
    TRACE_PARA(rmod);
    TRACE_PARA(smod);
    TRACE_PARE(swm);
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}


//***************************************************************************
//
//   Module : APC_CMD                  Submodule : ApiCmdRTGlobalCon
//
//   Author : R. Heitzmann             Project   : APC_APPL
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 12.04.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function reads data from the APC board
//
//***************************************************************************

AiReturn ApiCmdRTGlobalCon_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 count, TY_API_RT_SA *lpBuf)
{
    AiUInt32 loc_bytes_written = 0;
    AiReturn ul_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice   = _ApiGetDeviceInfoPtrByModule( bModule );
    TY_MIL_COM_LS_RT_GLOBAL_CON_INPUT  xInput;
    TY_MIL_COM_ACK                     xOutput;

    if( pDevice == NULL )
    	return API_ERR_NO_MODULE_EXTENSION;

    

    /* Set Mutex */
    _SyncWait(bModule, pDevice->_hRecMutex);

    
    
    _StructureEndianChange(STR_TY_API_RT_SA_TYPE, lpBuf, count);

    ul_RetVal = ApiWriteBlockMemData(bModule, API_MEMTYPE_SHARED, API_REP_REC_AREA_OFFSET, 1, lpBuf, count * sizeof(TY_API_RT_SA), &loc_bytes_written);
    if (ul_RetVal == API_OK)
    {
    	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_GLOBAL_CON, sizeof(xInput), sizeof(xOutput) );

    	xInput.count = (count > 511) ? 511 : count;

    	ul_RetVal = API_CMD_STRUCT_RT_GLOBAL_CON( bModule, &xInput, &xOutput );
    }

    _StructureEndianChange(STR_TY_API_RT_SA_TYPE, lpBuf, count);
    
    
    /* Release Mutex */
    _SyncRelease(bModule, pDevice->_hRecMutex);

    
    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGlobalCon(AiUInt32 bModule, AiUInt8 biu, AiUInt16 count, TY_API_RT_SA *lpBuf)
{
    int i = 0;
    AiReturn ul_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( (count < 1) || (count > 511) )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == lpBuf )
        ul_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTGlobalCon_(bModule, biu, count, lpBuf);
    }
    
    v_ExamineRetVal( "ApiCmdRTGlobalCon", ul_RetVal );
    TRACE_BEGIN
    TRACE1("        TY_API_RT_SA lpBuf[%d];\n", count);
    for(i=0; i<count; i++)
    {
    	TRACE1("        lpBuf[%d].bid       = ", i); TRACE2("0x%x;\n", lpBuf[i].bid);
    	TRACE1("        lpBuf[%d].buffer    = ", i); TRACE2("0x%p;\n", lpBuf[i].buffer);
    	TRACE1("        lpBuf[%d].hid       = ", i); TRACE2("0x%x;\n", lpBuf[i].hid);
    	TRACE1("        lpBuf[%d].mode      = ", i); TRACE2("0x%x;\n", lpBuf[i].mode);
    	TRACE1("        lpBuf[%d].nxw       = ", i); TRACE2("0x%x;\n", lpBuf[i].nxw);
    	TRACE1("        lpBuf[%d].resp_time = ", i); TRACE2("0x%x;\n", lpBuf[i].resp_time);
    	TRACE1("        lpBuf[%d].rt        = ", i); TRACE2("0x%x;\n", lpBuf[i].rt);
    	TRACE1("        lpBuf[%d].rt_con    = ", i); TRACE2("0x%x;\n", lpBuf[i].rt_con);
    	TRACE1("        lpBuf[%d].sa_con    = ", i); TRACE2("0x%x;\n", lpBuf[i].sa_con);
        TRACE1("        lpBuf[%d].sa_mc     = ", i); TRACE2("0x%x;\n", lpBuf[i].sa_mc);
        TRACE1("        lpBuf[%d].sa_type   = ", i); TRACE2("0x%x;\n", lpBuf[i].sa_type);
        TRACE1("        lpBuf[%d].smod      = ", i); TRACE2("0x%x;\n", lpBuf[i].smod);
        TRACE1("        lpBuf[%d].swm       = ", i); TRACE2("0x%x;\n", lpBuf[i].swm);
    }
    TRACE_FCTA("ApiCmdRTGlobalCon", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(count);
    TRACE_RPARE("lpBuf");
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSAConErr
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 31.07.2000
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Subaddress/Modecode [sa],
//                RT Subaddress Type [sa_type],
//                RT Subaddress Error Injection [perr]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_SA_CON_ERR' instruction to the API1553-DS
//    ASP Driver SW to control the error injection of the specified RT SA.
//
//***************************************************************************

AiReturn ApiCmdRTSAConErr_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 sa, AiUInt8 sa_type, TY_API_RT_ERR *perr)
{
    AiReturn         ul_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice   = _ApiGetDeviceInfoPtrByModule( bModule );
    TY_MIL_COM_LS_RT_SA_ERR_CON_INPUT   xInput;
    TY_MIL_COM_ACK                      xOutput;

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_SA_ERR_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr = rt_addr;
    xInput.sa_mc   = sa;
    xInput.sa_type = sa_type;

    xInput.type    = perr->type;
    xInput.sync    = perr->sync;
    xInput.contig  = perr->contig;
    xInput.wpos    = (AiUInt8)(perr->err_spec >> 16); // WPos
    xInput.bpos    = (AiUInt8)(perr->err_spec >> 8);  // BPos
    xInput.bc_bits = (AiUInt8)(perr->err_spec);       // BcBits;

    ul_RetVal = API_CMD_STRUCT_RT_SA_ERR_CON( bModule, &xInput, &xOutput );
    
    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAConErr(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, 
		                                           AiUInt8 sa, AiUInt8 sa_type, TY_API_RT_ERR *perr)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( rt_addr > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sa > MAX_RT_MODECODE )
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( sa_type > MAX_RT_SA_TYPE )
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( NULL == perr )
        ul_RetVal = API_ERR_PARAM6_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTSAConErr_(bModule, biu, rt_addr, sa, sa_type, perr);
    }
    
    v_ExamineRetVal( "ApiCmdRTSAConErr", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_ERR err;\n");
    TRACE1("        err.contig    = 0x%x;\n", perr->contig);
    TRACE1("        err.padding1  = 0x%x;\n", perr->padding1);
    TRACE1("        err.err_spec  = 0x%x;\n", perr->err_spec);
    TRACE1("        err.sync      = 0x%x;\n", perr->sync);
    TRACE1("        err.type      = 0x%x;\n", perr->type);
    TRACE_FCTA("ApiCmdRTSAConErr", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(sa);
    TRACE_PARA(sa_type);
    TRACE_RPARE("&err");
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSADytag
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 31.07.2000
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Dynamic Tagging Control (Ena/Dis) [con],
//                RT Dynamic Tagging Setup structure [pdytag]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_SA_DYTAG' instruction to the API1553-DS
//    ASP Driver SW to specify dynamic tagging with the specified RT SA.
//
//***************************************************************************

AiReturn ApiCmdRTDytagDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt16 rt_hid, AiUInt16 mode, TY_API_RT_DYTAG rt_dytag[4])
{
    AiUInt32 i;
    TY_MIL_COM_LS_RT_DYTAG_DEF_INPUT   xInput;
    TY_MIL_COM_ACK                     xOutput;
    AiReturn ul_RetVal = API_OK;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_DYTAG_DEF, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_hid  = rt_hid;
    xInput.con     = con;
    xInput.mode    = mode;

    for( i=0; i<4; i++ )
    {
    	xInput.tag_fct[i] = rt_dytag[i].tag_fct;
    	xInput.min[i]     = rt_dytag[i].min;
    	xInput.max[i]     = rt_dytag[i].max;
    	xInput.wpos[i]    = rt_dytag[i].wpos;
    	xInput.step[i]    = rt_dytag[i].step;
    }

    ul_RetVal = API_CMD_STRUCT_RT_DYTAG_DEF(bModule, &xInput, &xOutput);


    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTDytagDef(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                   AiUInt16 rt_hid, AiUInt16 mode,
                                                   TY_API_RT_DYTAG rt_dytag[4])
{
    AiUInt32 i;
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( con > API_ENA )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (rt_hid < MIN_HEADER_ID) || (rt_hid > MAX_HEADER_ID) )
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( mode > MAX_DYTAG_MODE )
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( API_DYTAG_STD_MODE == mode )
    {
        for( i=0; i<4; i++ )
        {
            if( API_OK == ul_RetVal )
            {
                if( rt_dytag[i].tag_fct > API_DYTAG_FCT_SYNC_COUNTER )
                    ul_RetVal = API_ERR_RTDYTAG_TAGFCT_NOT_IN_RANGE;
                else if( API_DYTAG_FCT_XMT_WORD == rt_dytag[i].tag_fct )
                {
                    if( rt_dytag[i].max > MAX_WPOS )
                        ul_RetVal = API_ERR_RTDYTAG_MAX_NOT_IN_RANGE;
                    else if( rt_dytag[i].step != 0 )
                        ul_RetVal = API_ERR_RTDYTAG_STEP_NOT_IN_RANGE;
                }
            }
        }
    }
    else
    {
        for( i=0; i<4; i++ )
        {
            if( API_OK == ul_RetVal )
            {
                if( (rt_dytag[i].tag_fct & API_DYTAG_FCT_MASK) > API_DYTAG_FCT_SAWTOOTH_8_HIGH )
                    ul_RetVal = API_ERR_RTDYTAG_TAGFCT_NOT_IN_RANGE;
                else if( rt_dytag[i].max != 0 )
                    ul_RetVal = API_ERR_RTDYTAG_MAX_NOT_IN_RANGE;
                else if( rt_dytag[i].step != 0 )
                    ul_RetVal = API_ERR_RTDYTAG_STEP_NOT_IN_RANGE;
            }
            else
                break;
        }
    }

    if( API_OK == ul_RetVal )
    {
    	ul_RetVal = ApiCmdRTDytagDef_(bModule, biu, con, rt_hid, mode, rt_dytag);
    }
    
    v_ExamineRetVal( "ApiCmdRTDytagDef", ul_RetVal );
    TRACE_BEGIN
        TRACE("        TY_API_RT_DYTAG rt_dytag[4];\n");
    for(i=0; i<4; i++)
    {
        TRACE1("        rt_dytag[%d].max     = ", i); TRACE2("0x%x;\n", rt_dytag[i].max);
        TRACE1("        rt_dytag[%d].min     = ", i); TRACE2("0x%x;\n", rt_dytag[i].min);
        TRACE1("        rt_dytag[%d].step    = ", i); TRACE2("0x%x;\n", rt_dytag[i].step);
        TRACE1("        rt_dytag[%d].tag_fct = ", i); TRACE2("0x%x;\n", rt_dytag[i].tag_fct);
        TRACE1("        rt_dytag[%d].wpos    = ", i); TRACE2("0x%x;\n", rt_dytag[i].wpos);
    }
    TRACE_FCTA("ApiCmdRTDytagDef", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(rt_hid);
    TRACE_PARA(mode);
    TRACE_RPARE("rt_dytag");
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTStart
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
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_START' instruction to the API1553-DS
//    ASP Driver SW to start RT operation.
//
//***************************************************************************

AiReturn ApiCmdRTStart_(AiUInt32 bModule, AiUInt8 biu)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM      xInput;
    TY_MIL_COM_ACK  xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_RT_START, sizeof(xInput), sizeof(xOutput) );

    ul_RetVal = API_CMD_STRUCT_RT_START( bModule, &xInput, &xOutput );

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTStart(AiUInt32 bModule, AiUInt8 biu)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else
    {
        ul_RetVal = ApiCmdRTStart_(bModule, biu);
    }
    
    v_ExamineRetVal( "ApiCmdRTStart", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTStart", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTHalt
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
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_HALT' instruction to the API1553-DS
//    ASP Driver SW to stop RT operation.
//
//***************************************************************************

AiReturn ApiCmdRTHalt_(AiUInt32 bModule, AiUInt8 biu)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM     xInput;
    TY_MIL_COM_ACK xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_RT_HALT, sizeof(xInput), sizeof(xOutput) );

    ul_RetVal = API_CMD_STRUCT_RT_HALT( bModule, &xInput, &xOutput );

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTHalt(AiUInt32 bModule, AiUInt8 biu)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else
    {
        ul_RetVal = ApiCmdRTHalt_(bModule, biu);
    }
    
    v_ExamineRetVal( "ApiCmdRTHalt", ul_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTHalt", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTul_RetValRead
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
//    Outputs   : Instruction execution status [ul_RetVal],
//                RT ul_RetVal Display data structure [api_rt_ul_RetVal_dsp]
//
//    Description :
//    This function applies the 'API_RT_ul_RetVal_READ' instruction to the API1553-DS
//    ASP Driver SW to read the RT ul_RetVal information.
//
//***************************************************************************

AiReturn ApiCmdRTStatusRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_RT_STATUS_DSP *api_rt_status_dsp)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM                          xInput;
    TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_RT_STATUS_READ, sizeof(xInput), sizeof(xOutput) );

    ul_RetVal = API_CMD_STRUCT_RT_STATUS_READ( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && api_rt_status_dsp )
    {
    	api_rt_status_dsp->glb_msg_cnt = xOutput.msg_cnt;
    	api_rt_status_dsp->glb_err_cnt = xOutput.err_cnt;
    	api_rt_status_dsp->status      = xOutput.dsp;
    	api_rt_status_dsp->padding1    = 0;
    	api_rt_status_dsp->padding2    = 0;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTStatusRead(AiUInt32 bModule, AiUInt8 biu, TY_API_RT_STATUS_DSP *api_rt_status_dsp)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == api_rt_status_dsp )
        ul_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTStatusRead_(bModule, biu, api_rt_status_dsp);
    }

    v_ExamineRetVal( "ApiCmdRTStatusRead", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_STATUS_DSP api_rt_status_dsp;\n");
    TRACE_FCTA("ApiCmdRTStatusRead", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&api_rt_status_dsp");
    TRACE_FCTE;
    TRACE1("    --> api_rt_status_dsp.glb_err_cnt = 0x%x\n", api_rt_status_dsp->glb_err_cnt);
    TRACE1("    --> api_rt_status_dsp.glb_msg_cnt = 0x%x\n", api_rt_status_dsp->glb_msg_cnt);
    TRACE1("    --> api_rt_status_dsp.status      = 0x%x\n", api_rt_status_dsp->status);
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTMsgRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT address [rt_addr]
//
//    Outputs   : Instruction execution status [ul_RetVal],
//                RT Message ul_RetVal data structure [pdsp]
//
//    Description :
//    This function applies the 'API_RT_MSG_READ' instruction to the API1553-DS
//    ASP Driver SW to read the RT message ul_RetVal information of the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTMsgRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, TY_API_RT_MSG_DSP *pdsp)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_MSG_READ_INPUT  xInput;
    TY_MIL_COM_LS_RT_MSG_READ_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_MSG_READ, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr = rt_addr;

    ul_RetVal = API_CMD_STRUCT_RT_MSG_READ( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && pdsp )
    {
    	pdsp->err_cnt  = xOutput.err_cnt;
    	pdsp->lcw      = xOutput.lcw;
    	pdsp->lsw      = xOutput.lsw;
    	pdsp->msg_cnt  = xOutput.msg_cnt;
    	pdsp->nxw      = xOutput.nxw;
    	pdsp->padding1 = 0;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTMsgRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, TY_API_RT_MSG_DSP *pdsp)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( rt_addr > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == pdsp )
        ul_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTMsgRead_(bModule, biu, rt_addr, pdsp);
    }
    
    v_ExamineRetVal( "ApiCmdRTMsgRead", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_MSG_DSP dsp;\n");
    TRACE_FCTA("ApiCmdRTMsgRead", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_RPARE("&dsp");
    TRACE_FCTE;
    if( pdsp )
    {
    	TRACE1("    --> dsp.err_cnt  = 0x%x\n", pdsp->err_cnt);
    	TRACE1("    --> dsp.lcw      = 0x%x\n", pdsp->lcw);
    	TRACE1("    --> dsp.lsw      = 0x%x\n", pdsp->lsw);
    	TRACE1("    --> dsp.msg_cnt  = 0x%x\n", pdsp->msg_cnt);
    	TRACE1("    --> dsp.nxw      = 0x%x\n", pdsp->nxw);
    }
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTMsgReadAll
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
//    Outputs   : Instruction execution status [ul_RetVal],
//                RT Message ul_RetVal data structure [pall_dsp]
//
//    Description :
//    This function applies the 'API_RT_MSG_READ_ALL' instruction to the API1553-DS
//    ASP Driver SW to read the RT message ul_RetVal information of all RTs.
//
//***************************************************************************

AiReturn ApiCmdRTMsgReadAll_(AiUInt32 bModule, AiUInt8 biu, TY_API_RT_MSG_ALL_DSP *pall_dsp)
{
    int     i         = 0;
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM                           xInput;
    TY_MIL_COM_LS_RT_MSG_READ_ALL_OUTPUT xOutput;


    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_RT_MSG_READ_ALL, sizeof(xInput), sizeof(xOutput) );

    ul_RetVal = API_CMD_STRUCT_RT_MSG_READ_ALL( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && pall_dsp )
    {
    	for( i=0; i<=MAX_RT_ADDR; i++ )
    	{
    		pall_dsp->rt[i].rt_err = xOutput.err_cnt[i];
    		pall_dsp->rt[i].rt_msg = xOutput.msg_cnt[i];
    	}
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTMsgReadAll(AiUInt32 bModule, AiUInt8 biu, TY_API_RT_MSG_ALL_DSP *pall_dsp)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pall_dsp )
        ul_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTMsgReadAll_(bModule, biu, pall_dsp);
    }
    
    v_ExamineRetVal( "ApiCmdRTMsgReadAll", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_MSG_ALL_DSP all_dsp;\n");
    TRACE_FCTA("ApiCmdRTMsgReadAll", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&all_dsp");
    TRACE_FCTE;
    TRACE1("    --> all_dsp = 0x%p\n", pall_dsp);
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSAMsgRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT address [rt_addr],
//                RT subaddress/modecode [sa],
//                RT subaddress type [sa_type],
//                Reserved [res]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//                RT SA Message ul_RetVal data structure [psa_dsp]
//
//    Description :
//    This function applies the 'API_RT_SA_MSG_READ' instruction to the API1553-DS
//    ASP Driver SW to read the RT subaddress/modecode ul_RetVal of the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTSAMsgRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 sa, AiUInt8 sa_type, AiUInt8 clr, TY_API_RT_SA_MSG_DSP *psa_dsp)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  xInput;
    TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_SA_MSG_READ, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr  = rt_addr;
    xInput.sa_mc    = sa;
    xInput.sa_type  = sa_type;
    xInput.clr_mode = clr;

    ul_RetVal = API_CMD_STRUCT_RT_SA_MSG_READ( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && psa_dsp )
    {
    	psa_dsp->bid  = xOutput.bid;
    	psa_dsp->bufp = xOutput.buf;
    	psa_dsp->lcw  = xOutput.lcw;
    	psa_dsp->lsw  = xOutput.lsw;
    	psa_dsp->trw  = xOutput.brw;
    	psa_dsp->ttag = xOutput.ttag;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAMsgRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
                                                    AiUInt8 sa, AiUInt8 sa_type, AiUInt8 clr,
                                                    TY_API_RT_SA_MSG_DSP *psa_dsp)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( rt_addr > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( sa > MAX_RT_MODECODE )
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( sa_type > MAX_RT_SA_TYPE )
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( clr > MAX_CLR )
        ul_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( NULL == psa_dsp )
        ul_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTSAMsgRead_(bModule, biu, rt_addr, sa, sa_type, clr, psa_dsp);
    }
    
    v_ExamineRetVal( "ApiCmdRTSAMsgRead", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_SA_MSG_DSP sa_dsp;\n");
    TRACE_FCTA("ApiCmdRTSAMsgRead", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(sa);
    TRACE_PARA(sa_type);
    TRACE_PARA(clr);
    TRACE_RPARE("&sa_dsp");
    TRACE_FCTE;
    if( psa_dsp )
    {
    	TRACE1("    --> sa_dsp.bid  = 0x%x\n", psa_dsp->bid);
    	TRACE1("    --> sa_dsp.bufp = 0x%x\n", psa_dsp->bufp);
    	TRACE1("    --> sa_dsp.lcw  = 0x%x\n", psa_dsp->lcw);
    	TRACE1("    --> sa_dsp.lsw  = 0x%x\n", psa_dsp->lsw);
    	TRACE1("    --> sa_dsp.trw  = 0x%x\n", psa_dsp->trw);
    	TRACE1("    --> sa_dsp.ttag = 0x%x\n", psa_dsp->ttag);
    }
	TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSAMsgReadEx
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT address [rt_addr],
//                RT subaddress/modecode [sa],
//                RT subaddress type [sa_type],
//                Reserved [res]
//
//    Outputs   : Instruction execution status [ul_RetVal]
//                RT SA Message ul_RetVal data structure [psa_dsp]
//
//    Description :
//    This function applies the 'API_RT_SA_MSG_READ' instruction to the API1553-DS
//    ASP Driver SW to read the RT subaddress/modecode ul_RetVal of the specified RT.
//
//***************************************************************************

AiReturn ApiCmdRTSAMsgReadEx_(AiUInt32 bModule, TY_API_RT_SA_MSG_READ_IN *px_MsgReadIn, TY_API_RT_SA_STATUS_EX *px_RtSaStatus)
{
    AiUInt8 rt_addr, sa, sa_type;
    AiUInt32 biu;
    AiUInt32 BufHeaderIndex, BufHeaderAddr;
    AiUInt32 tmpl, ul_BytesRead;
    TY_API_RT_STATUS_DSP api_rt_status_dsp;
    TY_DEVICE_INFO  *pDevice;
    AiReturn ul_RetVal = API_OK;

    pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    if( pDevice == NULL )
        ul_RetVal = API_ERR_NO_MODULE_EXTENSION;

    memset(px_RtSaStatus, 0, sizeof(*px_RtSaStatus) );

    if(0 == (bModule & API_STREAM_MASK))
        biu     = (AiUInt8) px_MsgReadIn->ul_Biu;
    else
        biu = (bModule & API_STREAM_MASK)>>API_STREAM_POS;
    rt_addr = (AiUInt8) px_MsgReadIn->ul_RtAddr;
    sa      = (AiUInt8) px_MsgReadIn->ul_SA;
    sa_type = (AiUInt8) px_MsgReadIn->ul_SaType;

    if( ul_RetVal == API_OK )
    {
        ul_RetVal = ApiCmdRTGetSABufferHeaderInfo( bModule, (AiUInt8)biu, rt_addr, sa_type, sa, &BufHeaderIndex, &BufHeaderAddr);
    }

    if( ul_RetVal == API_OK )
    {
        ul_RetVal = ApiCmdRTStatusRead( bModule, (AiUInt8)biu, &api_rt_status_dsp);
    }

    if( ul_RetVal == API_OK )
    {
        ApiReadMemData( bModule, API_MEMTYPE_GLOBAL, BufHeaderAddr, 4, &tmpl);
        // px_RtSaStatus->x_XferInfo.ul_ActBufferId = BufHeaderIndex;
        px_RtSaStatus->x_XferInfo.ul_ActBufferId = (tmpl & 0x000000FF);

        tmpl = (tmpl & 0x0003c000) >> 14;
        px_RtSaStatus->ul_BufferQueueSize = 1 << tmpl;
        px_RtSaStatus->x_XferInfo.ul_ActionWord = 0;
        px_RtSaStatus->x_XferInfo.ul_ErrCnt = api_rt_status_dsp.glb_err_cnt;
        px_RtSaStatus->x_XferInfo.ul_XferCnt = api_rt_status_dsp.glb_msg_cnt;

        px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord1 = 0;
        px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord2 = 0;
        px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord3 = 0;
        px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord4 = 0;

        ApiReadMemData( bModule, API_MEMTYPE_GLOBAL, BufHeaderAddr+4, 4, &tmpl);
        ApiReadBlockMemData( bModule, API_MEMTYPE_GLOBAL, pDevice->x_Stream[biu-1].ul_PbiBase + tmpl /*SQ Base Addr*/, 4, px_RtSaStatus->x_XferSQueue, (px_RtSaStatus->ul_BufferQueueSize*4), &ul_BytesRead);
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAMsgReadEx(AiUInt32 bModule, TY_API_RT_SA_MSG_READ_IN *px_MsgReadIn,
                                                      TY_API_RT_SA_STATUS_EX *px_RtSaStatus)
{
    int i = 0;
    AiReturn ul_RetVal = API_OK;


    if( (0 == (bModule & API_STREAM_MASK)) && ((px_MsgReadIn->ul_Biu < MIN_BIU) || (px_MsgReadIn->ul_Biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_MsgReadIn )
        ul_RetVal = API_ERR_PARAM2_IS_NULL;
    else if( px_MsgReadIn->ul_RtAddr > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( px_MsgReadIn->ul_SA > MAX_RT_MODECODE )
        ul_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( px_MsgReadIn->ul_SaType > MAX_RT_SA_TYPE )
        ul_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == px_RtSaStatus )
        ul_RetVal = API_ERR_PARAM3_IS_NULL;

    if( ul_RetVal == API_OK )
    {
        ul_RetVal = ApiCmdRTSAMsgReadEx_(bModule, px_MsgReadIn, px_RtSaStatus);
    }

    v_ExamineRetVal( "ApiCmdRTSAMsgReadEx", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_SA_MSG_READ_IN x_MsgReadIn;\n");
    TRACE("        TY_API_RT_SA_STATUS_EX   x_RtSaStatus;\n");
    TRACE1("        x_MsgReadIn.ul_Biu    = 0x%x;\n", px_MsgReadIn->ul_Biu);
    TRACE1("        x_MsgReadIn.ul_RtAddr = 0x%x;\n", px_MsgReadIn->ul_RtAddr);
    TRACE1("        x_MsgReadIn.ul_SA     = 0x%x;\n", px_MsgReadIn->ul_SA);
    TRACE1("        x_MsgReadIn.ul_SaType = 0x%x;\n", px_MsgReadIn->ul_SaType);
    TRACE_FCTA("ApiCmdRTSAMsgReadEx", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_RPARA("&x_MsgReadIn");
    TRACE_RPARE("&x_RtSaStatus");
    TRACE_FCTE;
    TRACE1("    --> x_RtSaStatus.ul_BufferQueueSize                 = 0x%x\n", px_RtSaStatus->ul_BufferQueueSize);
    TRACE1("    --> x_RtSaStatus.x_XferEQueue[0].ul_EqEntryWord1    = 0x%x\n", px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord1);
    TRACE1("    --> x_RtSaStatus.x_XferEQueue[0].ul_EqEntryWord2    = 0x%x\n", px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord2);
    TRACE1("    --> x_RtSaStatus.x_XferEQueue[0].ul_EqEntryWord3    = 0x%x\n", px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord3);
    TRACE1("    --> x_RtSaStatus.x_XferEQueue[0].ul_EqEntryWord4    = 0x%x\n", px_RtSaStatus->x_XferEQueue[0].ul_EqEntryWord4);
    TRACE1("    --> x_RtSaStatus.x_XferInfo.ul_ActBufferId          = 0x%x\n", px_RtSaStatus->x_XferInfo.ul_ActBufferId);
    TRACE1("    --> x_RtSaStatus.x_XferInfo.ul_ActionWord           = 0x%x\n", px_RtSaStatus->x_XferInfo.ul_ActionWord);
    TRACE1("    --> x_RtSaStatus.x_XferInfo.ul_ErrCnt               = 0x%x\n", px_RtSaStatus->x_XferInfo.ul_ErrCnt);
    TRACE1("    --> x_RtSaStatus.x_XferInfo.ul_XferCnt              = 0x%x\n", px_RtSaStatus->x_XferInfo.ul_XferCnt);
    for(i=0; i<256; i++)
    {
    	TRACE1("    --> x_RtSaStatus.x_XferSQueue[%03d].ul_SqActBufPtr   = ", i); TRACE2("0x%x\n", px_RtSaStatus->x_XferSQueue[i].ul_SqActBufPtr);
    	TRACE1("    --> x_RtSaStatus.x_XferSQueue[%03d].ul_SqCtrlWord    = ", i); TRACE2("0x%x\n", px_RtSaStatus->x_XferSQueue[i].ul_SqCtrlWord);
    	TRACE1("    --> x_RtSaStatus.x_XferSQueue[%03d].ul_SqStatusWords = ", i); TRACE2("0x%x\n", px_RtSaStatus->x_XferSQueue[i].ul_SqStatusWords);
    	TRACE1("    --> x_RtSaStatus.x_XferSQueue[%03d].ul_SqTimeTag     = ", i); TRACE2("0x%x\n", px_RtSaStatus->x_XferSQueue[i].ul_SqTimeTag);
    }
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module :                          Submodule : 
//
//   Author :                          Project   : AVI3910
//
//---------------------------------------------------------------------------
//   Create : 31.07.04   Update : 31.07.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : 
//
//    Description : 
//
//***************************************************************************
AiReturn ApiCmdRTBHRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid, TY_API_RT_BH_INFO *pbh)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_BH_GET_INPUT  xInput;
    TY_MIL_COM_LS_RT_BH_DEF_OUTPUT xOutput; /* Same output as BH def*/

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_BH_GET, sizeof(xInput), sizeof(xOutput) );

    xInput.hid   = hid;

    ul_RetVal = API_CMD_STRUCT_RT_BH_GET( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && pbh )
    {
    	pbh->bid       = xOutput.bid;
    	pbh->sid       = xOutput.sid;
    	pbh->eid       = xOutput.eid;
    	pbh->nbufs     = xOutput.nbufs;
    	pbh->hid_addr  = xOutput.hid_addr;
    	pbh->bq_addr   = xOutput.bid_addr;
    	pbh->sq_addr   = xOutput.sid_addr;
    	pbh->eq_addr   = xOutput.eid_addr;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTBHRead(AiUInt32 bModule, AiUInt8 biu,AiUInt16 hid,TY_API_RT_BH_INFO *pbh)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else
    {
        ul_RetVal = ApiCmdRTBHRead_(bModule, biu, hid, pbh);
    }
    
    v_ExamineRetVal( "ApiCmdRTBHRead", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_BH_INFO pbh;\n");
    TRACE_FCTA("ApiCmdRTBHRead", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(hid);
    TRACE_RPARE("&pbh");
    TRACE_FCTE;
    TRACE1("    --> pbh.bid      = 0x%x\n", pbh->bid);
    TRACE1("    --> pbh.bq_addr  = 0x%x\n", pbh->bq_addr);
    TRACE1("    --> pbh.eid      = 0x%x\n", pbh->eid);
    TRACE1("    --> pbh.eq_addr  = 0x%x\n", pbh->eq_addr);
    TRACE1("    --> pbh.hid_addr = 0x%x\n", pbh->hid_addr);
    TRACE1("    --> pbh.nbufs    = 0x%x\n", pbh->nbufs);
    TRACE1("    --> pbh.sid      = 0x%x\n", pbh->sid);
    TRACE1("    --> pbh.sq_addr  = 0x%x\n", pbh->sq_addr);
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module :                          Submodule : 
//
//   Author :                          Project   : AVI3910
//
//---------------------------------------------------------------------------
//   Create : 31.07.04   Update : 31.07.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : 
//
//    Description : 
//
//***************************************************************************
AiReturn ApiCmdRTGetSABufferHeaderInfo_(AiUInt32 Module, AiUInt8 biu, AiUInt8 RT, AiUInt8 Type,
                                       AiUInt8 SA_MC, AiUInt32* BufHeaderIndex, AiUInt32 *BufHeaderAddr)
{
    AiReturn ul_RetVal = API_ERR;
    TY_MIL_COM_LS_RT_SA_BH_GET_INPUT  xInput;
    TY_MIL_COM_LS_RT_SA_BH_GET_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(Module), 0, MIL_COM_LS_RT_SA_BH_GET, sizeof(xInput), sizeof(xOutput) );

    xInput.rt      = RT;
    xInput.sa_mc   = SA_MC;
    xInput.sa_type = Type;

    ul_RetVal = API_CMD_STRUCT_RT_SA_BH_GET( Module, &xInput, &xOutput );

    if( ul_RetVal == 0 )
    {
    	if( BufHeaderIndex ) *BufHeaderIndex = xOutput.hid;
    	if( BufHeaderAddr  ) *BufHeaderAddr  = xOutput.hid_addr;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetSABufferHeaderInfo(AiUInt32 Module, AiUInt8 biu, AiUInt8 RT, AiUInt8 Type,
                                                                AiUInt8 SA_MC, AiUInt32* BufHeaderIndex, AiUInt32 *BufHeaderAddr)
{
    AiReturn ul_RetVal = API_ERR;

    if( (Module & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (Module & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( RT > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( Type > API_RT_TYPE_TRANSMIT_MODECODE )
        ul_RetVal = API_ERR_TYPE_NOT_IN_RANGE;
    else if( SA_MC > MAX_RT_ADDR )
        ul_RetVal = API_ERR_XMT_SA_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTGetSABufferHeaderInfo_(Module, biu, RT, Type, SA_MC, BufHeaderIndex, BufHeaderAddr);
    }

    v_ExamineRetVal( "ApiCmdGetRTSABufferHeaderInfo", ul_RetVal );
    TRACE_BEGIN
    TRACE("        AiUInt32 BufHeaderIndex;\n");
    TRACE("        AiUInt32 BufHeaderAddr;\n");
    TRACE_FCTA("ApiCmdGetRTSABufferHeaderInfo", ul_RetVal); 
    TRACE_PARA(Module);
    TRACE_PARA(biu);
    TRACE_PARA(RT);
    TRACE_PARA(Type);
    TRACE_PARA(SA_MC);
    TRACE_RPARA("&BufHeaderIndex");
    TRACE_RPARE("&BufHeaderAddr");
    TRACE_FCTE;
    if( BufHeaderIndex ) TRACE1("    --> BufHeaderIndex = 0x%x\n", *BufHeaderIndex);
    if( BufHeaderAddr )  TRACE1("    --> BufHeaderAddr  = 0x%x\n", *BufHeaderAddr);
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module :                          Submodule : 
//
//   Author :                          Project   : AVI3910
//
//---------------------------------------------------------------------------
//   Create : 31.07.04   Update : 31.07.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : 
//
//    Description : 
//
//***************************************************************************
AiReturn ApiCmdRTGetSimulationInfo_(AiUInt32 Module, AiUInt8 biu, AiUInt8 rt, TY_RT_INFO* px_RT_Info)
{
    AiReturn ul_RetVal = API_ERR;
    AiUInt32 i;

    TY_MIL_COM_LS_RT_GET_SIM_INFO_INPUT  xInput;
    TY_MIL_COM_LS_RT_GET_SIM_INFO_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(Module), 0, MIL_COM_LS_RT_GET_SIM_INFO, sizeof(xInput), sizeof(xOutput) );

    xInput.rt = rt;

    ul_RetVal = API_CMD_STRUCT_RT_GET_SIM_INFO( Module, &xInput, &xOutput );

    if( (ul_RetVal==0) && px_RT_Info )
    {
    	px_RT_Info->uc_Mode = xOutput.mode;
    	px_RT_Info->ul_HSMC = xOutput.hs_mc;
    	px_RT_Info->ul_RxMC = xOutput.rx_mc;
    	px_RT_Info->ul_RxSA = xOutput.rx_sa;
    	px_RT_Info->ul_TxMC = xOutput.tx_mc;
    	px_RT_Info->ul_TxSA = xOutput.tx_sa;

    	for( i=0; i<8; i++ )
    	{
    		px_RT_Info->ul_HSRxMID[i] = xOutput.rx_mid[i];
    		px_RT_Info->ul_HSTxMID[i] = xOutput.tx_mid[i];
    	}
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetSimulationInfo(AiUInt32 Module, AiUInt8 biu, AiUInt8 rt, TY_RT_INFO* px_RT_Info)
{
    AiReturn ul_RetVal = API_ERR;

    if( (Module & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (Module & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( rt > MAX_RT_ADDR )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTGetSimulationInfo_(Module, biu, rt, px_RT_Info);
    }

    v_ExamineRetVal( "ApiCmdRTGetSimulationInfo", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_RT_INFO x_RT_Info;\n");
    TRACE_FCTA("ApiCmdRTGetSimulationInfo", ul_RetVal); 
    TRACE_PARA(Module);
    TRACE_PARA(biu);
    TRACE_PARA(rt);
    TRACE_RPARE("&x_RT_Info");
    TRACE_FCTE;
    if( px_RT_Info )
    {
    	TRACE1("    --> x_RT_Info.uc_Mode    = 0x%x\n", px_RT_Info->uc_Mode);
    	TRACE1("    --> x_RT_Info.ul_HSMC    = 0x%x\n", px_RT_Info->ul_HSMC);
    	TRACE1("    --> x_RT_Info.ul_HSRxMID = 0x%p\n", px_RT_Info->ul_HSRxMID);
    	TRACE1("    --> x_RT_Info.ul_HSTxMID = 0x%p\n", px_RT_Info->ul_HSTxMID);
    	TRACE1("    --> x_RT_Info.ul_RxMC    = 0x%x\n", px_RT_Info->ul_RxMC);
    	TRACE1("    --> x_RT_Info.ul_RxSA    = 0x%x\n", px_RT_Info->ul_RxSA);
    	TRACE1("    --> x_RT_Info.ul_TxMC    = 0x%x\n", px_RT_Info->ul_TxMC);
    	TRACE1("    --> x_RT_Info.ul_TxSA    = 0x%x\n", px_RT_Info->ul_TxSA);
    }
    TRACE_END
    
    return ul_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRt.cpp              Submodule : ApiCmdRTGetDytagDef
//
//   Author : Jochen Pfaadt            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 16.11.04   Update : 16.11.2004
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Buffer Header ID [rt_hid],
//
//    Outputs   : RT Dynamic Tagging Mode [mode]
//                RT Dynamic Tagging Setup structure [bc_dytag]
//
//    Description :
//    This function reads the  dynamic tagging information of the specified RT transfer.
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetDytagDef(AiUInt32 bModule, AiUInt8 biu, AiUInt16 rt_hid, AiUInt16 *mode,
                                                      TY_API_RT_DYTAG *rt_dytag)
{
    int i = 0;
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( (rt_hid < MIN_HEADER_ID) || (rt_hid > MAX_HEADER_ID) )
        ul_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    {
        ul_RetVal = ApiCmdRTGetDytagDef_(bModule, biu, rt_hid, mode, rt_dytag);
    }
    
    v_ExamineRetVal( "ApiCmdRTGetDytagDef", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_DYTAG rt_dytag[4];\n");
    TRACE("        AiUInt16        mode;\n");
    TRACE_FCTA("ApiCmdRTGetDytagDef", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_hid);
    TRACE_RPARA("&mode");
    TRACE_RPARE("rt_dytag");
    TRACE_FCTE;
    if( mode )
    	TRACE1("    --> mode    = 0x%x\n", *mode);
    for(i=0; i<4; i++)
    {
        TRACE1("    --> rt_dytag[%d].max     = ", i); TRACE2("0x%x\n", rt_dytag[i].max    );
        TRACE1("    --> rt_dytag[%d].min     = ", i); TRACE2("0x%x\n", rt_dytag[i].min    );
        TRACE1("    --> rt_dytag[%d].step    = ", i); TRACE2("0x%x\n", rt_dytag[i].step   );
        TRACE1("    --> rt_dytag[%d].tag_fct = ", i); TRACE2("0x%x\n", rt_dytag[i].tag_fct);
        TRACE1("    --> rt_dytag[%d].wpos    = ", i); TRACE2("0x%x\n", rt_dytag[i].wpos   );
    }
    TRACE_END
    
    return ul_RetVal;
}

AiReturn ApiCmdRTGetDytagDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 rt_hid, AiUInt16 *mode, TY_API_RT_DYTAG *rt_dytag)
{
    int i = 0;
    AiReturn ul_RetVal = API_OK;

    TY_MIL_COM_LS_RT_DYTAG_GET_INPUT  xInput;
    TY_MIL_COM_LS_RT_DYTAG_GET_OUTPUT xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_DYTAG_GET, sizeof(xInput), sizeof(xOutput) );
    
    xInput.rt_hid = rt_hid;
    
    ul_RetVal = API_CMD_STRUCT_RT_DYTAG_GET( bModule, &xInput, &xOutput );

    if( ul_RetVal==0 )
    {
    	if( mode ) *mode = xOutput.mode;

    	if( rt_dytag )
    	{
    		for( i=0; i<4; i++ )
    		{
    			rt_dytag[i].tag_fct = xOutput.tag_fct[i];
    			rt_dytag[i].min     = xOutput.min[i];
    			rt_dytag[i].max     = xOutput.max[i];
    			rt_dytag[i].step    = xOutput.step[i];
    			rt_dytag[i].wpos    = xOutput.wpos[i];
    		}
    	}
    }

    return ul_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRt.cpp              Submodule : ApiCmdRTGetSAConErr
//
//   Author : Jochen Pfaadt            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 16.11.04   Update : 13.09.2005
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Buffer Header ID [rt_hid],
//
//    Outputs   : RT Dynamic Tagging Mode [mode]
//                RT Dynamic Tagging Setup structure [bc_dytag]
//
//    Description :
//    This function reads the error injection data of a rt/sa
//
//***************************************************************************
AiReturn ApiCmdRTGetSAConErr_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr, AiUInt8 sa, AiUInt8 sa_type, TY_API_RT_ERR *perr)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_SA_ERR_GET_INPUT   xInput;
    TY_MIL_COM_LS_RT_SA_ERR_GET_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_RT_SA_ERR_GET, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_addr   = rt_addr;
    xInput.sa_mc     = sa;
    xInput.sa_type   = sa_type;


    ul_RetVal = API_CMD_STRUCT_RT_SA_ERR_GET( bModule, &xInput, &xOutput );

    if( (ul_RetVal==0) && perr )
    {   
    	perr->type     = xOutput.type;
    	perr->sync     = xOutput.sync;
    	perr->contig   = xOutput.contig;
    	perr->padding1 = 0;
    	perr->err_spec = (xOutput.wpos<<16) + (xOutput.bpos<<8) + (xOutput.bc_bits); /* WPOS + BPOS + BCBITS */ ;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTGetSAConErr(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rt_addr,
		                                              AiUInt8 sa, AiUInt8 sa_type, TY_API_RT_ERR *perr)
{
    AiReturn ul_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    {
        ul_RetVal = ApiCmdRTGetSAConErr_(bModule, biu, rt_addr, sa, sa_type, perr);
    }
    
    v_ExamineRetVal( "ApiCmdRTGetSAConErr", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_ERR err;\n");
    TRACE_FCTA("ApiCmdRTGetSAConErr", ul_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rt_addr);
    TRACE_PARA(sa);
    TRACE_PARA(sa_type);
    TRACE_RPARE("&err");
    TRACE_FCTE;
    if( perr )
    {
    	TRACE1("    --> err.contig    = 0x%x\n", perr->contig);
    	TRACE1("    --> err.padding1  = 0x%x\n", perr->padding1);
    	TRACE1("    --> err.err_spec  = 0x%x\n", perr->err_spec);
    	TRACE1("    --> err.sync      = 0x%x\n", perr->sync);
    	TRACE1("    --> err.type      = 0x%x\n", perr->type);
    }
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRt.cpp              Submodule : ApiCmdRTModeCtrl
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 19.02.07   Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Mode definition structure
//
//    Outputs   : none
//
//    Description :
//    This function is used to set the RT Mode. 
//
//***************************************************************************
AiReturn ApiCmdRTModeCtrl_(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_RT_MODE_CTRL *px_RtModeCtrl)
{
    AiReturn ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_MODE_CTRL_INPUT   xInput;
    TY_MIL_COM_ACK                     xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_LS_RT_MODE_CTRL, sizeof(xInput), sizeof(xOutput) );

    xInput.rt_mode = px_RtModeCtrl->ul_RtMode;
    xInput.rt_ctrl = px_RtModeCtrl->ul_Ctrl;
    xInput.param1  = px_RtModeCtrl->ul_Param1;
    xInput.param2  = px_RtModeCtrl->ul_Param2;
    xInput.param3  = px_RtModeCtrl->ul_Param3;

    ul_RetVal = API_CMD_STRUCT_RT_MODE_CTRL( ul_Module, &xInput, &xOutput );

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTModeCtrl( AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_RT_MODE_CTRL *px_RtModeCtrl)
{
    AiReturn ul_RetVal = API_OK;

    if( (ul_Module & API_MODULE_MASK) >= MAX_API_MODULE )
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if( (0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        ul_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_RtModeCtrl )
        ul_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( (px_RtModeCtrl->ul_RtMode < MIN_RT_MODE_CTRL) || (px_RtModeCtrl->ul_RtMode > MAX_RT_MODE_CTRL) )
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;

    if (API_OK == ul_RetVal)
    {
        if(API_RT_MODE_CONFIGURED_DYTAGS == px_RtModeCtrl->ul_RtMode)
        {
            if( (px_RtModeCtrl->ul_Ctrl < API_DIS) || (px_RtModeCtrl->ul_Ctrl > API_ENA) ) 
                ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
            else if( (px_RtModeCtrl->ul_Param1 > MAX_RT_ADDR) && (px_RtModeCtrl->ul_Param1 != ALL_RT_ADDR) )
                ul_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
        }
    }

    ul_RetVal = ApiCmdRTModeCtrl_(ul_Module, uc_Biu, px_RtModeCtrl);

    v_ExamineRetVal( "ApiCmdRTModeCtrl", ul_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RT_MODE_CTRL x_RtModeCtrl;\n");
    TRACE1("        x_RtModeCtrl.ul_Ctrl   = 0x%x;\n", px_RtModeCtrl->ul_Ctrl);
    TRACE1("        x_RtModeCtrl.ul_Param1 = 0x%x;\n", px_RtModeCtrl->ul_Param1);
    TRACE1("        x_RtModeCtrl.ul_Param2 = 0x%x;\n", px_RtModeCtrl->ul_Param2);
    TRACE1("        x_RtModeCtrl.ul_Param3 = 0x%x;\n", px_RtModeCtrl->ul_Param3);
    TRACE1("        x_RtModeCtrl.ul_RtMode = 0x%x;\n", px_RtModeCtrl->ul_RtMode);
    TRACE_FCTA("ApiCmdRTModeCtrl", ul_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_RPARE("&x_RtModeCtrl");
    TRACE_FCTE;
    TRACE_END

    return ul_RetVal;
}

//***************************************************************************
//
//   Module : ApiLsRt                  Submodule : ApiCmdRTSADWCSet
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 18.02.19   Update : 18.02.19
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Subaddress/Modecode [sa],
//                RT Subaddress Type [sa_type],
//                RT DWC [px_DwcSet],
//
//    Outputs   : Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_SA_DWC_SET' instruction to the API1553-DS
//    ASP Driver SW to set the defined word count of the
//    specified RT SA.
//
//***************************************************************************

AiReturn ApiCmdRTSADWCSet_(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr, AiUInt8 uc_SA,
    AiUInt8 uc_SAType, AiUInt32 ul_WordCnt)
{
    AiReturn                         ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_DWC_SET_INPUT   xInput;
    TY_MIL_COM_ACK                  xOutput;
    
    MIL_COM_INIT(&xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_LS_RT_DWC_SET, sizeof(xInput), sizeof(xOutput));

    xInput.rt_addr = uc_RtAddr;
    xInput.sa_mc = uc_SA;
    xInput.sa_type = uc_SAType;
    xInput.word_cnt = ul_WordCnt;

    ul_RetVal = API_CMD_STRUCT_RT_DWC_SET(ul_Module, &xInput, &xOutput);

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSADWCSet(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
    AiUInt8 uc_SA, AiUInt8 uc_SAType, AiUInt32 ul_WordCnt)
{
    AiReturn ul_RetVal = API_OK;

    if ((ul_Module & API_MODULE_MASK) >= MAX_API_MODULE)
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if ((0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)))
        ul_RetVal = API_ERR_WRONG_BIU;
    else if (uc_SA > MAX_RT_MODECODE)
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if (uc_SAType > API_RT_TYPE_TRANSMIT_SA)
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if (ul_WordCnt > MAX_RT_DEFINED_WORD_CNT)
        ul_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else
    {
        ul_RetVal = ApiCmdRTSADWCSet_(ul_Module, uc_Biu, uc_RtAddr, uc_SA, uc_SAType, ul_WordCnt);
    }

    v_ExamineRetVal("ApiCmdRTSADWCSet", ul_RetVal);
    TRACE_BEGIN
    TRACE1("        ul_WordCnt   = 0x%x;\n", ul_WordCnt);
    TRACE_FCTA("ApiCmdRTSADWCSet", ul_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_PARA(uc_RtAddr);
    TRACE_PARA(uc_SA);
    TRACE_PARA(uc_SAType);
    TRACE_PARA(ul_WordCnt);
    TRACE_FCTE;
    TRACE_END

    return ul_RetVal;
}


//***************************************************************************
//
//   Module : ApiLsRt                  Submodule : ApiCmdRTSADWCGet
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 18.02.19   Update : 18.02.19
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Address [rt_addr],
//                RT Subaddress/Modecode [sa],
//                RT Subaddress Type [sa_type]
//
//    Outputs   : RT DWC [px_DwcSet]
//                Instruction execution status [ul_RetVal]
//
//    Description :
//    This function applies the 'API_RT_SA_DWC_GET' instruction to the API1553-DS
//    ASP Driver SW to get the defined word count of the
//    specified RT SA.
//
//***************************************************************************

AiReturn ApiCmdRTSADWCGet_(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr, AiUInt8 uc_SA,
    AiUInt8 uc_SAType, AiUInt32 *pul_WordCnt)
{
    AiReturn                         ul_RetVal = API_OK;
    TY_MIL_COM_LS_RT_DWC_GET_INPUT   xInput;
    TY_MIL_COM_LS_RT_DWC_GET_OUTPUT  xOutput;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_LS_RT_DWC_GET, sizeof(xInput), sizeof(xOutput));

    xInput.rt_addr = uc_RtAddr;
    xInput.sa_mc = uc_SA;
    xInput.sa_type = uc_SAType;

    ul_RetVal = API_CMD_STRUCT_RT_DWC_GET(ul_Module, &xInput, &xOutput);

    if ((ul_RetVal == 0) && pul_WordCnt)
    {
        *pul_WordCnt = xOutput.word_cnt;
    }

    return ul_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSADWCGet(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
    AiUInt8 uc_SA, AiUInt8 uc_SAType, AiUInt32 *pul_WordCnt)
{
    AiReturn ul_RetVal = API_OK;

    if ((ul_Module & API_MODULE_MASK) >= MAX_API_MODULE)
        ul_RetVal = API_ERR_WRONG_MODULE;
    else if ((0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)))
        ul_RetVal = API_ERR_WRONG_BIU;
    else if (uc_SA > MAX_RT_MODECODE)
        ul_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if (uc_SAType > API_RT_TYPE_TRANSMIT_SA)
        ul_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if (pul_WordCnt == NULL)
        ul_RetVal = API_ERR_PARAM6_IS_NULL;
    else
    {
        ul_RetVal = ApiCmdRTSADWCGet_(ul_Module, uc_Biu, uc_RtAddr, uc_SA, uc_SAType, pul_WordCnt);
    }

    v_ExamineRetVal("ApiCmdRTSADWCGet", ul_RetVal);
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTSADWCGet", ul_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_PARA(uc_RtAddr);
    TRACE_PARA(uc_SA);
    TRACE_PARA(uc_SAType);
    TRACE_RPARE("pul_WordCnt");
    TRACE_FCTE;
    if (pul_WordCnt)
    {
        TRACE1("    --> pul_WordCnt    = 0x%x\n", *pul_WordCnt);
    }
    TRACE_END


    return ul_RetVal;
}
