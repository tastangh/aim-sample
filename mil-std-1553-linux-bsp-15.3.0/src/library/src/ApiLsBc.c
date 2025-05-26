/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsBc.c
 *
 * LS BC functions
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
#include "ApiLsBc.h"


extern AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel);



//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***          B U S    C O N T R O L L E R     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Retry Control [retr]
//                Service Request/Vector Word Control [svrq]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_INI' instruction to the API1553-DS
//    ASP Driver SW to initialize the BC.
//
//***************************************************************************

AiInt16 ApiCmdBCIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 retr, AiUInt8 svrq, AiUInt8 tbm, AiUInt8 gsb)
{
	TY_MIL_COM_LS_BC_INI_INPUT         xInput;
	TY_MIL_COM_ACK                     xOutput;
	AiInt16 uw_RetVal = API_OK;

	xInput.retries = retr;
	xInput.svrq    = svrq;
	xInput.tbm     = tbm;
	xInput.gsb     = gsb;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_INI, sizeof(xInput), sizeof(xOutput) );

	uw_RetVal = API_CMD_STRUCT_LS_BC_INI( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 retr, AiUInt8 svrq, AiUInt8 tbm, AiUInt8 gsb)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( retr > MAX_RETRY )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (svrq != API_DIS) && (svrq != API_ENA) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( tbm > MAX_TBM )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( gsb > API_BC_XFER_BUS_SECONDARY )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBCIni_(bModule, biu, retr, svrq, tbm, gsb);
    }
    v_ExamineRetVal( "ApiCmdBCIni", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(retr);
    TRACE_PARA(svrq);
    TRACE_PARA(tbm);
    TRACE_PARE(gsb);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCIni

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCBHDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 21.10.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Buffer Header ID [hid],
//                BC Data Buffer ID [bid],
//                BC Status Queue ID [sid],
//                BC Event Queue ID [eid],
//                Queue Size definition [qsize],
//                Buffer Queue Mode [bqm],
//                Buffer Store Mode [bsm],
//                Status Queue Mode [sts],
//                Event Queue Mode [evs],
//                Double Buffer Mode [dbm]
//
//    Outputs   : Instruction execution status [status],
//                Buffer Header Info structure [pbh]
//
//    Description :
//    This function applies the 'TG_API_BC_BH_DEF' instruction to the API1553-DS
//    ASP Driver SW to setup the specified BC Buffer Header descriptor.
//
//***************************************************************************

AiInt16 ApiCmdBCBHDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid,
                                                AiUInt16 bid, AiUInt16 sid, AiUInt16 eid,
                                                AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                AiUInt8 sqm, AiUInt8 eqm, AiUInt8 dbm,
                                                TY_API_BC_BH_INFO *pbh)
{
    TY_MIL_COM_LS_BC_BH_DEF_INPUT  xInput;
    TY_MIL_COM_LS_BC_BH_DEF_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.hid     = hid;
    xInput.bid     = bid;
    xInput.sid     = sid;
    xInput.eid     = eid;
    xInput.qsize   = qsize;
    xInput.bqm     = bqm;
    xInput.bsm     = bsm;
    xInput.sqm     = sqm;
    xInput.eqm     = eqm;
    xInput.dbm     = dbm;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_BH_DEF, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_BH_DEF( bModule, &xInput, &xOutput );


    pbh->bid        = xOutput.bid;
    pbh->bq_addr    = xOutput.bq_addr;
    pbh->eid        = xOutput.eid;
    pbh->eq_addr    = xOutput.eq_addr;
    pbh->hid_addr   = xOutput.hid_addr;
    pbh->nbufs      = xOutput.nbufs;
    pbh->sid        = xOutput.sid;
    pbh->sq_addr    = xOutput.sq_addr;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCBHDef(AiUInt32 bModule, AiUInt8 biu, AiUInt16 hid,
                                                AiUInt16 bid, AiUInt16 sid, AiUInt16 eid,
                                                AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                AiUInt8 sqm, AiUInt8 eqm, AiUInt8 dbm,
                                                TY_API_BC_BH_INFO *pbh)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( sid != 0 )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( eid != 0 )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( qsize > MAX_QUEUE_SIZE )
        uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else if( bqm > MAX_BQM )
        uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
    else if( bsm > MAX_BSM )
        uw_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
    else if( sqm > API_SQM_AS_QSIZE )
        uw_RetVal = API_ERR_PARAM10_NOT_IN_RANGE;
    else if( eqm != 0 )
        uw_RetVal = API_ERR_PARAM11_NOT_IN_RANGE;
    else if( dbm != 0 )
        uw_RetVal = API_ERR_PARAM12_NOT_IN_RANGE;
    else if( NULL == pbh )
        uw_RetVal = API_ERR_PARAM13_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdBCBHDef_(bModule, biu, hid, bid, sid, eid, qsize, bqm, bsm, sqm, eqm, dbm, pbh);
    }
    v_ExamineRetVal( "ApiCmdBCBHDef", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_BC_BH_INFO pbh;\n");
    TRACE_FCTA("ApiCmdBCBHDef", uw_RetVal); 
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
    TRACE_PARA(dbm);
    TRACE_RPARE("&pbh");
    TRACE_FCTE;
    TRACE1("    --> pbh.bid     = 0x%x\n", pbh->bid);
    TRACE1("    --> pbh.bq_addr = 0x%x\n", pbh->bq_addr);
    TRACE1("    --> pbh.eid     = 0x%x\n", pbh->eid);
    TRACE1("    --> pbh.eq_addr = 0x%x\n", pbh->eq_addr);
    TRACE1("    --> pbh.hid     = 0x%x\n", pbh->hid_addr);
    TRACE1("    --> pbh.nbufs   = 0x%x\n", pbh->nbufs);
    TRACE1("    --> pbh.sid     = 0x%x\n", pbh->sid);
    TRACE1("    --> pbh.sq_addr = 0x%x\n", pbh->sq_addr);
    TRACE_END

    return(uw_RetVal);
} // end: ApiCmdBCBHDef

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCXferDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 21.10.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Transfer description [pxfer]
//
//    Outputs   : Instruction execution status [status]
//                Transfer Descriptor address [desc_addr]
//
//    Description :
//    This function applies the 'TG_API_BC_XFER_DEF' instruction to the API1553-DS
//    ASP Driver SW to setup the specified BC Transfer descriptor.
//
//***************************************************************************

AiInt16 ApiCmdBCXferDef_(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_XFER *pxfer, AiUInt32 *desc_addr)
{
    TY_MIL_COM_LS_BC_XFER_DEF_INPUT  xInput;
    TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT xOutput;
    TY_DEVICE_INFO  * pDevice = NULL;
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ul_Biu;

    pDevice = _ApiGetDeviceInfoPtrByModule(bModule);

    memset(&xOutput, 0, sizeof(xOutput));
    /* Prepare output of address for error case. 
       When the TSW command returns with an error the address is undefined but the PBA.pro performs 
       a read/write mem data operation on the address without checking. */
    uw_RetVal = uw_ApiGetBiuFromStream(bModule, biu, &ul_Biu);
    if (!uw_RetVal)
        xOutput.addr = pDevice->x_MemInfo.ax_BiuAddr[ul_Biu-1].ul_BcXferDesc + pxfer->xid * (8*4L);


    xInput.xid        = pxfer->xid;
    xInput.hid        = pxfer->hid;
    xInput.xfer_type  = pxfer->type;
    xInput.chn        = pxfer->chn;
    xInput.xmt_rt     = pxfer->xmt_rt;
    xInput.rcv_rt     = pxfer->rcv_rt;
    xInput.xmt_sa     = pxfer->xmt_sa;
    xInput.rcv_sa     = pxfer->rcv_sa;
    xInput.wcnt       = pxfer->wcnt;
    xInput.xfer_ir    = pxfer->tic;
    xInput.xfer_hlt   = pxfer->hlt;
    xInput.rte        = pxfer->rte;
    xInput.sxh        = pxfer->sxh;
    xInput.rsp        = pxfer->rsp;
    xInput.res        = pxfer->res;
    xInput.swxm       = pxfer->swxm;
    xInput.err_type    = pxfer->err.type;
    xInput.err_wpos    = (pxfer->err.err_spec >> 16) & 0xFF;
    xInput.err_bpos    = (pxfer->err.err_spec >> 8)  & 0xFF;
    xInput.err_sync    = pxfer->err.sync;
    xInput.err_contig  = pxfer->err.contig;
    xInput.err_bc_bits = (pxfer->err.err_spec) & 0xFF;
    xInput.delay_mode = pxfer->gap_mode;
    xInput.gap        = pxfer->gap;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_XFER_DEF, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_XFER_DEF( bModule, &xInput, &xOutput );

    if( desc_addr != NULL )
        *desc_addr = xOutput.addr;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDef(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_XFER *pxfer, AiUInt32 *desc_addr)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pxfer )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == desc_addr )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( (pxfer->xid < MIN_XFER_ID) || (pxfer->xid > MAX_XFER_ID) )
        uw_RetVal = API_ERR_XID_NOT_IN_RANGE;
    else if( pxfer->type > API_BC_TYPE_RTRT )
        uw_RetVal = API_ERR_TYPE_NOT_IN_RANGE;
    else if( pxfer->chn > API_BC_XFER_BUS_SECONDARY )
        uw_RetVal = API_ERR_CHN_NOT_IN_RANGE;
    else if( pxfer->xmt_rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_XMT_RT_NOT_IN_RANGE;
    else if( pxfer->rcv_rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_RCV_RT_NOT_IN_RANGE;
    else if( pxfer->xmt_sa > MAX_RT_ADDR )
        uw_RetVal = API_ERR_XMT_SA_NOT_IN_RANGE;
    else if( pxfer->rcv_sa > MAX_RT_ADDR )
        uw_RetVal = API_ERR_RCV_SA_NOT_IN_RANGE;
    else if( pxfer->wcnt > MAX_WCNT )
        uw_RetVal = API_ERR_WCNT_NOT_IN_RANGE;
    else if( pxfer->tic > API_BC_TIC_INT_ON_STAT_EXCEPT )
        uw_RetVal = API_ERR_TIC_NOT_IN_RANGE;
    else if( pxfer->hlt > API_BC_HLT_HALT_ON_ANY_INT )
        uw_RetVal = API_ERR_HLT_NOT_IN_RANGE;
    else if( pxfer->sxh > API_BC_SRVW_MULTIPLE )
        uw_RetVal = API_ERR_SXH_NOT_IN_RANGE;
    else if( pxfer->rsp > API_BC_RSP_NO_SW2_EXPECTED )
        uw_RetVal = API_ERR_RSP_NOT_IN_RANGE;
    else if( pxfer->gap_mode > API_BC_GAP_MODE_FAST )
        uw_RetVal = API_ERR_GAP_MODE_NOT_IN_RANGE;
    else if( pxfer->err.type > MAX_ERR_TYPE )
        uw_RetVal = API_ERR_ERRTYPE_NOT_IN_RANGE;
    else if( ((API_ERR_TYPE_NO_INJECTION != pxfer->err.type) && (API_ERR_TYPE_WORD_CNT_HIGH == pxfer->err.type) &&
        (API_ERR_TYPE_WORD_CNT_LOW != pxfer->err.type)) &&
        (((pxfer->err.err_spec & API_BC_ERRSPEC_WPOS_MASK) >> API_BC_ERRSPEC_WPOS_POS) > MAX_ERR_WPOS) )
        uw_RetVal = API_ERR_ERRSPEC_WPOS_NOT_IN_RANGE;
    else if( (((API_ERR_TYPE_MANCHESTER_LOW == pxfer->err.type) || (API_ERR_TYPE_MANCHESTER_HIGH == pxfer->err.type) ||
        (API_ERR_TYPE_ZERO_CROSS_NEG == pxfer->err.type) || (API_ERR_TYPE_ZERO_CROSS_POS == pxfer->err.type))) &&
        ((((pxfer->err.err_spec & API_BC_ERRSPEC_BPOS_MASK) >> API_BC_ERRSPEC_BPOS_POS) < MIN_ERR_BPOS) ||
        (((pxfer->err.err_spec & API_BC_ERRSPEC_BPOS_MASK) >> API_BC_ERRSPEC_BPOS_POS) > MAX_ERR_BPOS)) )
        uw_RetVal = API_ERR_ERRSPEC_BPOS_NOT_IN_RANGE;
    else if( (((API_ERR_TYPE_BIT_CNT_LOW == pxfer->err.type) || (API_ERR_TYPE_BIT_CNT_HIGH == pxfer->err.type))) &&
        ((((pxfer->err.err_spec & API_BC_ERRSPEC_BCBITS_MASK) >> API_BC_ERRSPEC_BCBITS_POS) < MIN_ERR_BCBITS) ||
        (((pxfer->err.err_spec & API_BC_ERRSPEC_BCBITS_MASK) >> API_BC_ERRSPEC_BCBITS_POS) > MAX_ERR_BCBITS)) )
        uw_RetVal = API_ERR_ERRSPEC_BCBITS_NOT_IN_RANGE;
    else
    {
        if( pxfer->gap_mode < API_BC_GAP_MODE_FAST )
        {
            if( pxfer->gap > MAX_GAP_TIME_MODE_01 )
                uw_RetVal = API_ERR_GAP_NOT_IN_RANGE;
        }
        else
        {
            if( pxfer->gap > MAX_GAP_TIME_MODE_2 )
                uw_RetVal = API_ERR_GAP_NOT_IN_RANGE;
        }

        if( API_OK == uw_RetVal )
        {
            uw_RetVal = ApiCmdBCXferDef_(bModule, biu, pxfer, desc_addr);
        }
    }
    v_ExamineRetVal( "ApiCmdBCXferDef", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_BC_XFER xfer;\n");
    TRACE("        AiUInt32       desc_addr;\n");
    TRACE1("        xfer.chn      = 0x%x;\n", pxfer->chn);
    TRACE1("        xfer.err_type = 0x%x;\n", pxfer->err.type);
    TRACE1("        xfer.err_spec = 0x%x;\n", pxfer->err.err_spec);
    TRACE1("        xfer.gap      = 0x%x;\n", pxfer->gap);
    TRACE1("        xfer.gap_mode = 0x%x;\n", pxfer->gap_mode);
    TRACE1("        xfer.hid      = 0x%x;\n", pxfer->hid);
    TRACE1("        xfer.hlt      = 0x%x;\n", pxfer->hlt);
    TRACE1("        xfer.rcv_rt   = 0x%x;\n", pxfer->rcv_rt);
    TRACE1("        xfer.rcv_sa   = 0x%x;\n", pxfer->rcv_sa);
    TRACE1("        xfer.res      = 0x%x;\n", pxfer->res);
    TRACE1("        xfer.rsp      = 0x%x;\n", pxfer->rsp);
    TRACE1("        xfer.rte      = 0x%x;\n", pxfer->rte);
    TRACE1("        xfer.swxm     = 0x%x;\n", pxfer->swxm);
    TRACE1("        xfer.sxh      = 0x%x;\n", pxfer->sxh);
    TRACE1("        xfer.tic      = 0x%x;\n", pxfer->tic);
    TRACE1("        xfer.type     = 0x%x;\n", pxfer->type);
    TRACE1("        xfer.wcnt     = 0x%x;\n", pxfer->wcnt);
    TRACE1("        xfer.xid      = 0x%x;\n", pxfer->xid);
    TRACE1("        xfer.xmt_rt   = 0x%x;\n", pxfer->xmt_rt);
    TRACE1("        xfer.xmt_sa   = 0x%x;\n", pxfer->xmt_sa);
    TRACE_FCTA("ApiCmdBCXferDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&xfer");
    TRACE_RPARE("&desc_addr");
    TRACE_FCTE;
    if( desc_addr != NULL )
        TRACE1("    --> desc_addr = 0x%x\n", *desc_addr);
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCXferDef

//---------------------------------------------------------------------------
//   Create : 01.08.12   Update : 01.08.12
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Transfer error description [pError]
//
//    Description :
//    This function changes the LS BC error injection on the fly. The parameters
//    are the same as for ApiCmdBCXferDef TY_API_BC_XFER.TY_API_BC_ERR
//
//***************************************************************************
AiInt16 ApiCmdBCXferDefErr_(AiUInt32 ulModule, AiUInt8 biu, AiUInt16 xid, TY_API_BC_ERR * pError )
{
    AiInt16                         wRetVal = API_OK;
    TY_DEVICE_INFO                * pDevice = _ApiGetDeviceInfoPtrByModule( ulModule );
    TY_MIL_COM_LS_XFER_ERROR_INPUT  xInput;
    TY_MIL_COM_ACK                  xOutput;

    if( IsTswVersionGreaterOrEqual( pDevice, 20, 40 ) )
    {
        // -- initialize command ---
        MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModule), biu, MIL_COM_LS_BC_XFER_ERROR,  sizeof(xInput),  sizeof(xOutput) );

        xInput.ulXid = xid;

        xInput.xError.ul_Type     = pError->type;
        xInput.xError.ul_Contig   = pError->contig;
        xInput.xError.ul_Sync     = pError->sync;
        xInput.xError.ul_ErrSpec  = pError->err_spec;
        xInput.xError.ul_Padding1 = pError->padding1;

        wRetVal = API_CMD_STRUCT_LS_BC_XFER_ERROR( ulModule, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );
    }
    else
    {
        wRetVal = API_ERR_WRONG_TSW_VERSION;
    }

    return wRetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDefErr(AiUInt32 ulModule, AiUInt8 biu, AiUInt16 xid, TY_API_BC_ERR * pError )
{
    AiInt16 wRetVal = API_OK;

    wRetVal = ApiCmdBCXferDefErr_( ulModule, biu, xid, pError );

    v_ExamineRetVal( "ApiCmdBCXferDef", wRetVal );
    TRACE_BEGIN
        TRACE1("       xid           = %d;\n",   xid );
    TRACE("        TY_API_BC_ERR xErr;\n");
    TRACE1("       xErr.err_type = 0x%x;\n", pError->type     );
    TRACE1("       xErr.err_spec = 0x%x;\n", pError->err_spec );
    TRACE1("       xErr.contig   = 0x%x;\n", pError->contig   );
    TRACE1("       xErr.padding1 = 0x%x;\n", pError->padding1 );
    TRACE1("       xErr.sync     = 0x%x;\n", pError->sync     );
    TRACE_FCTA("ApiCmdBCXferDefErr", wRetVal); 
    TRACE_PARA(ulModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&pError");
    TRACE_FCTE;
    TRACE_END
    
    return wRetVal;
} // end: ApiCmdBCXferDef

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCXferCtrl
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Transfer Identifier [xid]
//                Enable/Disable BC Transfer [mode]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_XFER_CTRL' instruction to the API1553-DS
//    ASP Driver SW to control the specified BC Transfer descriptor.
//
//***************************************************************************

AiInt16 ApiCmdBCXferCtrl_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt8 mode)
{
    TY_MIL_COM_LS_BC_XFER_CTRL_INPUT xInput;
    TY_MIL_COM_ACK                   xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.xid    = xid;
    xInput.mode   = mode;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_XFER_CTRL, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_XFER_CTRL( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferCtrl(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt8 mode)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (xid < MIN_XFER_ID) || (xid > MAX_XFER_ID) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBCXferCtrl_(bModule, biu, xid, mode);
    }
    v_ExamineRetVal( "ApiCmdBCXferCtrl", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCXferCtrl", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(xid);
    TRACE_PARE(mode);
    TRACE_FCTE;
    TRACE_END

    return(uw_RetVal);
} // end: ApiCmdBCXferCtrl



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDescGet( AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt16 ulXid, AiUInt32 ulMode, AiUInt32 aulValue[4] )
{
    AiInt16 wRetVal  = API_OK;

    TY_MIL_COM_LS_XFER_DESC_GET_INPUT   xInput;
    TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT  xOutput;

    memset( &xInput,  0, sizeof(xInput)  );
    memset( &xOutput, 0, sizeof(xOutput) );

    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModHandle), ucBiu, MIL_COM_LS_BC_XFER_DESC_GET, sizeof(xInput), sizeof(xOutput) );



    xInput.ulXid   = ulXid;
    xInput.ulMode  = ulMode;

    wRetVal = API_CMD_STRUCT_LS_BC_XFER_DESC_GET( ulModHandle, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );

    if( wRetVal != 0 )
        // Error while executing target command
        return wRetVal;

    aulValue[0] = xOutput.aulValue[0];
    aulValue[1] = xOutput.aulValue[1];
    aulValue[2] = xOutput.aulValue[2];
    aulValue[3] = xOutput.aulValue[3];

    return wRetVal;
}



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferDescMod( AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt16 ulXid, AiUInt32 ulMode, AiUInt32 aulValue[4] )
{
    AiInt16 wRetVal  = API_OK;

    TY_MIL_COM_LS_XFER_DESC_MOD_INPUT   xInput;
    TY_MIL_COM_ACK                      xOutput;

    memset( &xInput,  0, sizeof(xInput)  );
    memset( &xOutput, 0, sizeof(xOutput) );

    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModHandle), ucBiu, MIL_COM_LS_BC_XFER_DESC_MOD, sizeof(xInput), sizeof(xOutput) );

    xInput.ulXid       = ulXid;
    xInput.ulMode      = ulMode;
    xInput.aulValue[0] = aulValue[0];
    xInput.aulValue[1] = aulValue[1];
    xInput.aulValue[2] = aulValue[2];
    xInput.aulValue[3] = aulValue[3];

    wRetVal = API_CMD_STRUCT_LS_BC_XFER_DESC_MOD( ulModHandle, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );

    return wRetVal;
}



//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCDytagDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 31.07.2000
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Dynamic Tagging Control (Ena/Dis) [con],
//                BC Dynamic Tagging Setup structure [pdytag]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_XFER_DYTAG' instruction to the API1553-DS
//    ASP Driver SW to specify dynamic tagging with the specified BC transfer.
//
//***************************************************************************

AiInt16 ApiCmdBCDytagDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                   AiUInt16 bc_hid, AiUInt16 mode,
                                                   TY_API_BC_DYTAG *bc_dytag)
{
    TY_MIL_COM_LS_BC_DYTAG_DEF_INPUT    xInput;
    TY_MIL_COM_ACK                      xOutput;
    AiUInt32 i;
    AiInt16 uw_RetVal = API_OK;

    xInput.con     = con; // Enable-Disable
    xInput.bc_hid  = bc_hid;
    xInput.mode    = mode;

    for( i=0; i<4; i++ )
    {
        xInput.tag_fct[i]  = bc_dytag[i].tag_fct;
        xInput.min[i]      = bc_dytag[i].min;
        xInput.max[i]      = bc_dytag[i].max;
        xInput.step[i]     = bc_dytag[i].step;
        xInput.wpos[i]     = bc_dytag[i].wpos;
    }

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_DYTAG_DEF, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_DYTAG_DEF( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCDytagDef(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                   AiUInt16 bc_hid, AiUInt16 mode,
                                                   TY_API_BC_DYTAG *bc_dytag)
{
    AiUInt32 i;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( con > API_ENA )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( mode > MAX_DYTAG_MODE )
        return API_ERR_PARAM5_NOT_IN_RANGE;

    if( API_DYTAG_STD_MODE == mode )
    {
        for( i=0; i<4; i++ )
        {
            if( bc_dytag[i].tag_fct > API_DYTAG_FCT_SYNC_COUNTER )
                return API_ERR_BCDYTAG_TAGFCT_NOT_IN_RANGE;
            if( API_DYTAG_FCT_XMT_WORD == bc_dytag[i].tag_fct )
            {
                if( bc_dytag[i].max > MAX_WPOS )
                    return API_ERR_BCDYTAG_MAX_NOT_IN_RANGE;
                if( bc_dytag[i].step != 0 )
                    return API_ERR_BCDYTAG_STEP_NOT_IN_RANGE;
            }
        }
    }
    else
    {
        for( i=0; i<4; i++ )
        {
            if( (bc_dytag[i].tag_fct & API_DYTAG_FCT_MASK) > MAX_DYTAG_FCT )
                return API_ERR_BCDYTAG_TAGFCT_NOT_IN_RANGE;
            if( bc_dytag[i].max != 0 )
                return API_ERR_BCDYTAG_MAX_NOT_IN_RANGE;
            if( bc_dytag[i].step != 0 )
                return API_ERR_BCDYTAG_STEP_NOT_IN_RANGE;
        }
    }


    if( ((bc_dytag[0].wpos > MAX_WPOS) && (bc_dytag[0].tag_fct != API_DYTAG_FCT_DISABLE)) ||
        ((bc_dytag[1].wpos > MAX_WPOS) && (bc_dytag[1].tag_fct != API_DYTAG_FCT_DISABLE)) ||
        ((bc_dytag[2].wpos > MAX_WPOS) && (bc_dytag[2].tag_fct != API_DYTAG_FCT_DISABLE)) ||
        ((bc_dytag[3].wpos > MAX_WPOS) && (bc_dytag[3].tag_fct != API_DYTAG_FCT_DISABLE)) )
        return API_ERR_BCDYTAG_WPOS_NOT_IN_RANGE;


    uw_RetVal = ApiCmdBCDytagDef_(bModule, biu, con, bc_hid, mode, bc_dytag);

    v_ExamineRetVal( "ApiCmdBCDytagDef", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_BC_DYTAG bc_dytag;\n");
    TRACE1("        bc_dytag.max     = 0x%x;\n", bc_dytag->max);
    TRACE1("        bc_dytag.min     = 0x%x;\n", bc_dytag->min);
    TRACE1("        bc_dytag.step    = 0x%x;\n", bc_dytag->step);
    TRACE1("        bc_dytag.tag_fct = 0x%x;\n", bc_dytag->tag_fct);
    TRACE1("        bc_dytag.wpos    = 0x%x;\n", bc_dytag->wpos);
    TRACE_FCTA("ApiCmdBCDytagDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(bc_hid);
    TRACE_PARA(mode);
    TRACE_RPARE("&bc_dytag");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCDytagDef

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCFrameDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Minor Frame description [pframe]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_FRAME_DEF' instruction to the API1553-DS
//    ASP Driver SW to specify a BC Minor Frame.
//
//***************************************************************************

AiInt16 ApiCmdBCFrameDef_(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_FRAME  *pframe)
{
    TY_MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT xInput;
    TY_MIL_COM_ACK                         xOutput;

    AiUInt32 ulInputSize  = MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT_MINIMUM_SIZE +  pframe->cnt * sizeof(TY_BC_INSTRUCTION);
    AiInt16 i = 0;
    AiInt16 uw_RetVal = API_OK;

    // Prepare BC Minor Frame Sequence
    xInput.frame_id   = pframe->id;
    xInput.xfer_cnt   = pframe->cnt; // Transfers in Minor Frame
    for (i=0; (i < pframe->cnt) && (i < MAX_API_BC_XFRAME); i++)
    {
        xInput.data[i].instr = pframe->instr[i];
        xInput.data[i].xid   = pframe->xid[i];
    }

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_MINOR_FRAME_DEF, ulInputSize, sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_MINOR_FRAME_DEF( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCFrameDef(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_FRAME  *pframe)
{
    AiInt16 i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( NULL == pframe )
        return API_ERR_PARAM3_IS_NULL;
    if( (pframe->id < MIN_API_BC_MFRAME) || (pframe->id > MAX_API_BC_MFRAME) )
        return API_ERR_FRAME_ID_NOT_IN_RANGE;
    if( (pframe->cnt < MIN_API_BC_XFRAME) || (pframe->cnt > MAX_API_BC_XFRAME) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;

    for( i=0; i<pframe->cnt; i++ )
    {
        if( (pframe->instr[i] < MIN_INSTR_NR) || (pframe->instr[i] > MAX_INSTR_NR))
            return API_ERR_FRAME_INSTR_NOT_IN_RANGE;

        if( API_BC_INSTR_TRANSFER == pframe->instr[i] )
        {
            if( (pframe->xid[i] < MIN_XFER_ID) || (pframe->xid[i] > MAX_XFER_ID) )
                return API_ERR_FRAME_XID_NOT_IN_RANGE;
        }
        else if( API_BC_INSTR_STROBE == pframe->instr[i] )
        {
            if( pframe->xid[i] != 0 )
                return API_ERR_FRAME_XID_NOT_IN_RANGE;
        }
    }

    uw_RetVal = ApiCmdBCFrameDef_(bModule, biu, pframe);
    
    v_ExamineRetVal( "ApiCmdBCFrameDef", uw_RetVal );

    TRACE_BEGIN
    TRACE ("        TY_API_BC_FRAME frame;\n");
    TRACE1("        frame.cnt      = 0x%x;\n", pframe->cnt);
    TRACE1("        frame.id       = 0x%x;\n", pframe->id);
    for(i=0; i<pframe->cnt; i++)
    {
        TRACE1("        frame.instr[%d] = ", i); TRACE2("0x%x;\n", pframe->instr[i]);
    }
    TRACE_FCTA("ApiCmdBCFrameDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&frame");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCFrameDef

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCMFrameDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Major Frame description [pmframe]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_MFRAME_DEF' instruction to the API1553-DS
//    ASP Driver SW to specify the BC Major Frame.
//
//***************************************************************************
AiInt16 ApiCmdBCMFrameDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt32 frame_cnt, AiUInt8 *frame_ids)
{
    AiInt16  retval = 0;
    AiUInt32 i      = 0;
    TY_MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT xInput;
    TY_MIL_COM_ACK                         xOutput;

    AiUInt32 ulInputSize  = MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT_MINIMUM_SIZE +  frame_cnt * sizeof(AiUInt32);

    // Prepare BC Major Frame Sequence
    xInput.frame_cnt   = frame_cnt; // Minor Frames in Major Frame
    for (i=0; (i < frame_cnt) && (i < MAX_API_BC_MFRAME_EX); i++)
    {
        xInput.frame_ids[i] = frame_ids[i];
    }

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_MAJOR_FRAME_DEF, ulInputSize, sizeof(xOutput) );

    retval =  API_CMD_STRUCT_LS_BC_MAJOR_FRAME_DEF( bModule, &xInput, &xOutput );

    return retval;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCMFrameDef(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_MFRAME  *pmframe)
{
    AiInt16 i         = 0;
    AiInt16 uw_RetVal = API_OK;



    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;

    if( NULL == pmframe )
        return API_ERR_PARAM3_IS_NULL;

    if( (pmframe->cnt < MIN_API_BC_MFRAME) || (pmframe->cnt > MAX_API_BC_MFRAME_EX) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;

    for( i=0; i<pmframe->cnt; i++ )
    {
        if( (pmframe->fid[i] < MIN_API_BC_MFRAME_ID) || (pmframe->fid[i] > MAX_API_BC_MFRAME_ID))
            return API_ERR_FRAME_FID_NOT_IN_RANGE;
    }



    uw_RetVal = ApiCmdBCMFrameDef_( bModule, biu, pmframe->cnt, pmframe->fid );




    v_ExamineRetVal( "ApiCmdBCMFrameDef", uw_RetVal );

    TRACE_BEGIN
    TRACE ("        TY_API_BC_MFRAME mframe;\n");
    TRACE1("        mframe.cnt     = 0x%x;\n", pmframe->cnt);
    for(i=0; i<pmframe->cnt; i++)
    {
        TRACE1("        mframe.fid[%02d] = ", i); TRACE2("0x%x;\n", pmframe->fid[i]);
    }
    TRACE_FCTA("ApiCmdBCMFrameDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&mframe");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCMFrameDef


//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCMFrameDefEx
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 19.10.04   Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Major Frame description [pmframe]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_MFRAME_DEF' instruction to the API1553-DS
//    ASP Driver SW to specify the BC Major Frame.
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCMFrameDefEx(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_MFRAME_EX  *pmframe)
{
    AiInt32 i         = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( NULL == pmframe )
        return API_ERR_PARAM3_IS_NULL;
    if( (pmframe->cnt < MIN_API_BC_MFRAME) || (pmframe->cnt > MAX_API_BC_MFRAME_EX) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;

    for( i=0; i<pmframe->cnt; i++ )
    {
        if( (pmframe->fid[i] < MIN_API_BC_MFRAME_ID) || (pmframe->fid[i] > MAX_API_BC_MFRAME_ID))
            return API_ERR_FRAME_FID_NOT_IN_RANGE;
    }


    uw_RetVal = ApiCmdBCMFrameDef_( bModule, biu, pmframe->cnt, pmframe->fid );


    v_ExamineRetVal( "ApiCmdBCMFrameDefEx", uw_RetVal );

    TRACE_BEGIN
    TRACE ("        TY_API_BC_MFRAME_EX mframe;\n");
    TRACE1("        mframe.cnt      = 0x%x;\n", pmframe->cnt);
    TRACE1("        mframe.padding1 = 0x%x;\n", pmframe->padding1);
    for(i=0; i<pmframe->cnt; i++)
    {
        TRACE1("        mframe.fid[%d]  = ", i); TRACE2("0x%x;\n", pmframe->fid[i]);
    }
    TRACE_FCTA("ApiCmdBCMFrameDefEx", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&mframe");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCMFrameDefEx

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCStart
//
//   Author : P. Giesel                Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 28.08.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC start mode (Imm/ExtTrg/DBC) [smod]
//                BC operation mode (cyclic/single shots) [cnt]
//                Minor Frame time in 0.1 msec [frame_time]
//                Instruction Table Start Address [saddr]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_START' instruction to the API1553-DS
//    ASP Driver SW to start the BC operation.
//
//***************************************************************************


AiInt16 ApiCmdBCStart_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 smod, AiUInt32 cnt, AiFloat frame_time, AiUInt32 saddr, AiUInt32 *maj_frame_addr, AiUInt32 *min_frame_addr)
{
    TY_MIL_COM_LS_BC_START_INPUT  xInput;
    TY_MIL_COM_LS_BC_START_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;

    xInput.smod             = smod;
    xInput.cnt              = cnt;
    xInput.frame_time_in_ms = (AiUInt32)(frame_time*1000);
    xInput.saddr            = saddr;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_START, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_START( bModule, &xInput, &xOutput );


    if( API_OK == uw_RetVal )
    {
        if( NULL != maj_frame_addr )
            *maj_frame_addr = xOutput.major_frame_addr;

        if( NULL == min_frame_addr )
        {
            for( i=0; i<MAX_API_BC_MFRAME; i++ )
                min_frame_addr[i] = 0L;

            if (smod == API_BC_START_SETUP)
            {
                for( i=0; i<MAX_API_BC_MFRAME; i++ )
                    min_frame_addr[i] = xOutput.minor_frame_addr[i];
            }
        }
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCStart(AiUInt32 bModule, AiUInt8 biu, AiUInt8 smod, AiUInt32 cnt, AiFloat frame_time, AiUInt32 saddr, AiUInt32 *maj_frame_addr, AiUInt32 *min_frame_addr)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( (smod < MIN_BC_START_MODE) || (smod > MAX_BC_START_MODE) )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( (API_BC_START_IMMEDIATELY == smod) || (API_BC_START_EXT_TRIGGER == smod) || 
        (API_BC_START_RT_MODECODE == smod) || (API_BC_START_INSTR_TABLE == smod))
    {
        if( (frame_time < MIN_BC_FRAME_TIME) || (frame_time > MAX_BC_FRAME_TIME) ) 
            return API_ERR_PARAM5_NOT_IN_RANGE;
    }

    uw_RetVal = ApiCmdBCStart_(bModule, biu, smod, cnt, frame_time, saddr, maj_frame_addr, min_frame_addr);
    
    v_ExamineRetVal( "ApiCmdBCStart", uw_RetVal );

    TRACE_BEGIN
    TRACE ("        AiUInt32 maj_frame_addr;\n");
    TRACE ("        AiUInt32 min_frame_addr[MAX_API_BC_MFRAME];\n");
    TRACE_FCTA("ApiCmdBCStart", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(smod);
    TRACE_PARA(cnt);
    TRACE_FPARA(frame_time);
    TRACE_PARA(saddr);
    TRACE_RPARA("&maj_frame_addr");
    TRACE_RPARE("&min_frame_addr");
    TRACE_FCTE;
    TRACE1("    --> maj_frame_addr      = 0x%x\n", *maj_frame_addr);
    for(i=0; i<MAX_API_BC_MFRAME; i++)
    {
        TRACE1("    --> min_frame_addr[%02d]  = ", i); TRACE2("0x%x\n", min_frame_addr[i]);
    }
    TRACE_END

    return(uw_RetVal);
} // end: ApiCmdBCStart


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCHalt
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
//    This function applies the 'TG_API_BC_HALT' instruction to the API1553-DS
//    ASP Driver SW to stop the BC operation.
//
//***************************************************************************

AiInt16 ApiCmdBCHalt_(AiUInt32 bModule, AiUInt8 biu)
{
    TY_MIL_COM        xInput;
    TY_MIL_COM_ACK    xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BC_HALT, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_HALT( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCHalt(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;

    uw_RetVal = ApiCmdBCHalt_(bModule, biu);

    v_ExamineRetVal( "ApiCmdBCHalt", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCHalt", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCHalt

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCStatusRead
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
//                BC Status Display structure [pdsp]
//
//    Description :
//    This function applies the 'TG_API_BC_STATUS_READ' instruction to the API1553-DS
//    ASP Driver SW to read the current the BC status.
//
//***************************************************************************

AiInt16 ApiCmdBCStatusRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_STATUS_DSP *pdsp)
{
    TY_MIL_COM                          xInput;
    TY_MIL_COM_LS_BC_STATUS_READ_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BC_STATUS_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_STATUS_READ( bModule, &xInput, &xOutput );

    pdsp->glb_err_cnt    = xOutput.bc_ec;
    pdsp->glb_msg_cnt    = xOutput.bc_mc;
    pdsp->hip            = xOutput.hip;
    pdsp->hxfer          = xOutput.hxfer;
    pdsp->mfc            = xOutput.mfc;
    pdsp->padding1       = 0;
    pdsp->status         = xOutput.status;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCStatusRead(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_STATUS_DSP *pdsp)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( NULL == pdsp )
        return API_ERR_PARAM3_IS_NULL;

    uw_RetVal = ApiCmdBCStatusRead_( bModule, biu, pdsp);

    v_ExamineRetVal( "ApiCmdBCStatusRead", uw_RetVal );

    TRACE_BEGIN
    TRACE ("    TY_API_BC_STATUS_DSP dsp;\n");
    TRACE_FCTA("ApiCmdBCStatusRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&dsp");
    TRACE_FCTE;
    TRACE1("    --> dsp.glb_err_cnt = 0x%x\n", pdsp->glb_err_cnt);
    TRACE1("    --> dsp.glb_msg_cnt = 0x%x\n", pdsp->glb_msg_cnt);
    TRACE1("    --> dsp.hip         = 0x%x\n", pdsp->hip);
    TRACE1("    --> dsp.hxfer       = 0x%x\n", pdsp->hxfer);
    TRACE1("    --> dsp.mfc         = 0x%x\n", pdsp->mfc);
    TRACE1("    --> dsp.padding1    = 0x%x\n", pdsp->padding1);
    TRACE1("    --> dsp.status      = 0x%x\n", pdsp->status);
    TRACE_END

    return(uw_RetVal);
} // end: ApiCmdBCStatusRead

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCXferRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Transfer identifier [xid]
//                Buffer Status Flag control [clr]
//
//    Outputs   : Instruction execution status [status]
//                BC Transfer Descriptor Status structure [api_bc_xfer_dsp]
//
//    Description :
//    This function applies the 'TG_API_BC_XFER_READ' instruction to the API1553-DS
//    ASP Driver SW to read the current the BC Transfer descriptor status.
//
//***************************************************************************

AiInt16 ApiCmdBCXferRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt16 clr,
                                                   TY_API_BC_XFER_DSP *pxfer_dsp)
{
    TY_MIL_COM_LS_BC_XFER_READ_INPUT   xInput;
    TY_MIL_COM_LS_BC_XFER_READ_OUTPUT  xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.xid   = xid;
    xInput.clr   = clr;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_XFER_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_XFER_READ( bModule, &xInput, &xOutput );

    pxfer_dsp->bid        = xOutput.bid;
    pxfer_dsp->brw        = xOutput.brw;
    pxfer_dsp->bufp       = xOutput.bufp;
    pxfer_dsp->cw1        = xOutput.cw1;
    pxfer_dsp->cw2        = xOutput.cw2;
    pxfer_dsp->err_cnt    = xOutput.err_cnt;
    pxfer_dsp->lvw        = xOutput.lvw;
    pxfer_dsp->msg_cnt    = xOutput.msg_cnt;
    pxfer_dsp->srvreq_cnt = xOutput.srvreq_cnt;
    pxfer_dsp->st1        = xOutput.st1;
    pxfer_dsp->st2        = xOutput.st2;
    pxfer_dsp->ttag       = xOutput.ttag;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferRead(AiUInt32 bModule, AiUInt8 biu, AiUInt16 xid, AiUInt16 clr,
                                                   TY_API_BC_XFER_DSP *pxfer_dsp)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( (xid < MIN_XFER_ID) || (xid > MAX_XFER_ID) )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( clr > MAX_CLR )
        return API_ERR_PARAM4_NOT_IN_RANGE;
    if( NULL == pxfer_dsp )
        return API_ERR_PARAM5_IS_NULL;


    uw_RetVal = ApiCmdBCXferRead_( bModule, biu, xid, clr, pxfer_dsp);

    v_ExamineRetVal( "ApiCmdBCXferRead", uw_RetVal );

    TRACE_BEGIN
    TRACE ("    TY_API_BC_XFER_DSP xfer_dsp;\n");
    TRACE_FCTA("ApiCmdBCXferRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(xid);
    TRACE_PARA(clr);
    TRACE_RPARE("&xfer_dsp");
    TRACE_FCTE;
    TRACE1("    --> xfer_dsp.bid        = 0x%x\n", pxfer_dsp->bid);
    TRACE1("    --> xfer_dsp.brw        = 0x%x\n", pxfer_dsp->brw);
    TRACE1("    --> xfer_dsp.bufp       = 0x%x\n", pxfer_dsp->bufp);
    TRACE1("    --> xfer_dsp.cw1        = 0x%x\n", pxfer_dsp->cw1);
    TRACE1("    --> xfer_dsp.cw2        = 0x%x\n", pxfer_dsp->cw2);
    TRACE1("    --> xfer_dsp.err_cnt    = 0x%x\n", pxfer_dsp->err_cnt);
    TRACE1("    --> xfer_dsp.lvw        = 0x%x\n", pxfer_dsp->lvw);
    TRACE1("    --> xfer_dsp.msg_cnt    = 0x%x\n", pxfer_dsp->msg_cnt);
    TRACE1("    --> xfer_dsp.srvreq_cnt = 0x%x\n", pxfer_dsp->srvreq_cnt);
    TRACE1("    --> xfer_dsp.st1        = 0x%x\n", pxfer_dsp->st1);
    TRACE1("    --> xfer_dsp.st2        = 0x%x\n", pxfer_dsp->st2);
    TRACE1("    --> xfer_dsp.ttag       = 0x%x\n", pxfer_dsp->ttag);
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCXferRead

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCXferReadEx
//
//   Author : Jochen Pfaadt            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 07.08.07   Update : 07.08.07
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Transfer identifier [xid]
//                Buffer Status Flag control [clr]
//
//    Outputs   : BC Transfer Descriptor Status structure [px_XferStat]
//
//    Description :
//    This function applies the 'TG_API_BC_XFER_READ' instruction to the API1553-DS
//    ASP Driver SW to read the current the BC Transfer descriptor status. Also it
//    reads the status queue entries.
//
//***************************************************************************

AiInt16 ApiCmdBCXferReadEx_(AiUInt32 bModule, TY_API_BC_XFER_READ_IN *px_XferReadIn,
                                                     TY_API_BC_XFER_STATUS_EX *px_XferStat)
{
    TY_API_BC_XFER_DSP xfer_dsp;
    TY_API_BC_BH_INFO x_BufHeader;
    TY_API_BC_XFER x_BCXfer;
    AiUInt32 ul_BytesRead;
    AiUInt16 xid;
    AiInt16  uw_RetVal;
    AiUInt8  biu;
    AiUInt32 sq_queue_entry;
    AiUInt32 temp, CurrentBufferIndex;

    uw_RetVal = API_OK;
    memset(px_XferStat, 0, sizeof(*px_XferStat) );

    biu = (AiUInt8)  px_XferReadIn->ul_Biu;
    xid = (AiUInt16) px_XferReadIn->ul_XferId;

    if(uw_RetVal==API_OK)
    {
        uw_RetVal = ApiCmdBCXferRead( bModule, biu, xid, (AiUInt16)(px_XferReadIn->ul_Clear), &xfer_dsp);

        if(uw_RetVal==API_OK)
        {
            uw_RetVal = ApiCmdBCGetXferDef(bModule, biu, xid, &x_BCXfer);
        }

        if(uw_RetVal==API_OK)
        {
            uw_RetVal = ApiCmdBCBHRead( bModule, biu, x_BCXfer.hid, &x_BufHeader);
            if(uw_RetVal==API_OK)
            {
                px_XferStat->ul_BufferQueueSize              = x_BufHeader.nbufs;

                px_XferStat->x_XferInfo.ul_ActBufferId       = xfer_dsp.bid;
                px_XferStat->x_XferInfo.ul_ActionWord        = 0; //only used for high speed
                px_XferStat->x_XferInfo.ul_XferCnt           = xfer_dsp.msg_cnt; 
                px_XferStat->x_XferInfo.ul_ErrCnt            = xfer_dsp.err_cnt;
                px_XferStat->x_XferInfo.ul_LastVectorWord    = xfer_dsp.lvw;
                px_XferStat->x_XferInfo.ul_ServiceRequestCnt = xfer_dsp.srvreq_cnt;

                px_XferStat->x_XferEQueue[0].ul_EqEntryWord1 = 0;
                px_XferStat->x_XferEQueue[0].ul_EqEntryWord2 = 0;
                px_XferStat->x_XferEQueue[0].ul_EqEntryWord3 = 0;
                px_XferStat->x_XferEQueue[0].ul_EqEntryWord4 = 0;

                _ApiReadBlockMemData( bModule, API_MEMTYPE_GLOBAL, x_BufHeader.sq_addr, 4, px_XferStat->x_XferSQueue, (px_XferStat->ul_BufferQueueSize*4), &ul_BytesRead);
                
                /* read the Buffer Headr Control Word to get the Current Buffer Index*/
                _ApiReadBlockMemData(bModule, API_MEMTYPE_GLOBAL, x_BufHeader.hid_addr, 4, &temp, 1, &ul_BytesRead);
                CurrentBufferIndex = temp & 0xFF;

                /*  FW points to the next SQ Entry*/
                if (CurrentBufferIndex == 0)
                {
                    /* get last SQ Entry */
                    sq_queue_entry = px_XferStat->ul_BufferQueueSize - 1;
                }
                else
                {
                    /* get the previous SQ Entry */
                    sq_queue_entry = CurrentBufferIndex-1;
                }

                px_XferStat->x_XferSQueue[sq_queue_entry].ul_SqCtrlWord |= (xfer_dsp.brw << 16);
            }
        }
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCXferReadEx(AiUInt32 bModule, TY_API_BC_XFER_READ_IN *px_XferReadIn,
                                                     TY_API_BC_XFER_STATUS_EX *px_XferStat)
{
    AiUInt32 i = 0;
    AiInt16  uw_RetVal = API_OK;

    if( NULL == px_XferReadIn )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( (0 == (bModule & API_STREAM_MASK)) && ((px_XferReadIn->ul_Biu < MIN_BIU) || (px_XferReadIn->ul_Biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (px_XferReadIn->ul_XferId < MIN_XFER_ID) || (px_XferReadIn->ul_XferId > MAX_XFER_ID) )
        uw_RetVal = API_ERR_XID_NOT_IN_RANGE;
    else if( px_XferReadIn->ul_Clear > MAX_CLR )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == px_XferStat )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;

    uw_RetVal = ApiCmdBCXferReadEx_( bModule, px_XferReadIn, px_XferStat);

    v_ExamineRetVal( "ApiCmdBCXferReadEx", uw_RetVal );
    TRACE_BEGIN
    TRACE ("        TY_API_BC_XFER_READ_IN   x_XferReadIn;\n");
    TRACE ("        TY_API_BC_XFER_STATUS_EX x_XferStat;\n");
    TRACE1("        x_XferReadIn.ul_Biu    = 0x%x;\n", px_XferReadIn->ul_Biu);
    TRACE1("        x_XferReadIn.ul_Clear  = 0x%x;\n", px_XferReadIn->ul_Clear);
    TRACE1("        x_XferReadIn.ul_XferId = 0x%x;\n", px_XferReadIn->ul_XferId);
    TRACE_FCTA("ApiCmdBCXferReadEx", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_RPARA("&x_XferReadIn");
    TRACE_RPARE("&x_XferStat");
    TRACE_FCTE;
    TRACE1("    --> x_XferStat.ul_BufferQueueSize              = 0x%x\n", px_XferStat->ul_BufferQueueSize);
    TRACE1("    --> x_XferStat.x_XferEQueue[0].ul_EqEntryWord1 = 0x%x\n", px_XferStat->x_XferEQueue[0].ul_EqEntryWord1);
    TRACE1("    --> x_XferStat.x_XferEQueue[0].ul_EqEntryWord2 = 0x%x\n", px_XferStat->x_XferEQueue[0].ul_EqEntryWord2);
    TRACE1("    --> x_XferStat.x_XferEQueue[0].ul_EqEntryWord3 = 0x%x\n", px_XferStat->x_XferEQueue[0].ul_EqEntryWord3);
    TRACE1("    --> x_XferStat.x_XferEQueue[0].ul_EqEntryWord4 = 0x%x\n", px_XferStat->x_XferEQueue[0].ul_EqEntryWord4);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_ActBufferId       = 0x%x\n", px_XferStat->x_XferInfo.ul_ActBufferId);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_ActionWord        = 0x%x\n", px_XferStat->x_XferInfo.ul_ActionWord);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_ErrCnt            = 0x%x\n", px_XferStat->x_XferInfo.ul_ErrCnt);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_LastVectorWord    = 0x%x\n", px_XferStat->x_XferInfo.ul_LastVectorWord);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_ServiceRequestCnt = 0x%x\n", px_XferStat->x_XferInfo.ul_ServiceRequestCnt);
    TRACE1("    --> x_XferStat.x_XferInfo.ul_XferCnt           = 0x%x\n", px_XferStat->x_XferInfo.ul_XferCnt);
    for(i=0; i<px_XferStat->ul_BufferQueueSize; i++ )
    {
        TRACE1("    --> x_XferStat.x_XferSQueue[i].ul_SqActBufPtr  = 0x%x\n", px_XferStat->x_XferSQueue[i].ul_SqActBufPtr);
        TRACE1("    --> x_XferStat.x_XferSQueue[i].ul_SqCtrlWord   = 0x%x\n", px_XferStat->x_XferSQueue[i].ul_SqCtrlWord);
        TRACE1("    --> x_XferStat.x_XferSQueue[i].ul_SqStatusWords= 0x%x\n", px_XferStat->x_XferSQueue[i].ul_SqStatusWords);
        TRACE1("    --> x_XferStat.x_XferSQueue[i].ul_SqTimeTag    = 0x%x\n", px_XferStat->x_XferSQueue[i].ul_SqTimeTag);
    }
    TRACE_END
    
    return(uw_RetVal);
} // end: ApiCmdBCXferReadEx

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCAcycPrep
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Acyclic Frame description [pframe]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_BC_ACYC_MSG_PREP' instruction to the API1553-DS
//    ASP Driver SW to specify a BC Acyclic Frame.
//
//***************************************************************************

AiInt16 ApiCmdBCAcycPrep_(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_ACYC *pacyc)
{
    TY_MIL_COM_LS_BC_ACYC_PREP_INPUT xInput;
    TY_MIL_COM_ACK                   xOutput;
    AiInt16 i = 0;
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ulInputSize  = MIL_COM_LS_BC_ACYC_PREP_INPUT_MINIMUM_SIZE +  pacyc->cnt * sizeof(TY_BC_INSTRUCTION);

    // Prepare Acyclic Frame Sequence
    xInput.xfer_cnt   = pacyc->cnt;
    for (i=0; (i < pacyc->cnt) && (i < MAX_API_BC_XFRAME); i++)
    {
        xInput.data[i].instr = pacyc->instr[i];
        xInput.data[i].xid   = pacyc->xid[i];
    }

    // Since malloc is not deterministic on RT systems we have a maximum size struct on the stack.
    // However, we transfer only the used part (ulInputSize) to the TSW.
    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_ACYC_PREP, ulInputSize, sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_ACYC_PREP(bModule, &xInput, &xOutput);

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAcycPrep(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_ACYC *pacyc)
{
    AiInt16 i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( NULL == pacyc )
        return API_ERR_PARAM3_IS_NULL;
    if( (pacyc->cnt < MIN_API_BC_XFRAME) || (pacyc->cnt > MAX_API_BC_XACYC) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;
    for( i=0; i<pacyc->cnt; i++ )
    {
        if( (pacyc->instr[i] < MIN_INSTR_NR) || (pacyc->instr[i] > MAX_INSTR_NR))
            return API_ERR_FRAME_INSTR_NOT_IN_RANGE;

        if( API_BC_INSTR_TRANSFER == pacyc->instr[i] )
        {
            if( (pacyc->xid[i] < MIN_XFER_ID) || (pacyc->xid[i] > MAX_XFER_ID) )
                return API_ERR_FRAME_XID_NOT_IN_RANGE;
        }
        if( API_BC_INSTR_WAIT == pacyc->instr[i] )
            return API_ERR_FRAME_XID_NOT_IN_RANGE;
        if( API_BC_INSTR_STROBE == pacyc->instr[i] )
        {
            if( pacyc->xid[i] != 0 )
                return API_ERR_FRAME_XID_NOT_IN_RANGE;
        }
    }


    uw_RetVal = ApiCmdBCAcycPrep_(bModule, biu, pacyc);


    v_ExamineRetVal( "ApiCmdBCAcycPrep", uw_RetVal );
    TRACE_BEGIN
    TRACE ("        TY_API_BC_ACYC   acyc;\n");
    TRACE1("        acyc.cnt      = 0x%x;\n", pacyc->cnt);
    TRACE1("        acyc.padding1 = 0x%x;\n", pacyc->padding1);
    for(i=0; i<pacyc->cnt; i++ )
    {
        TRACE1("        acyc.instr[%d] = ", i); TRACE2("0x%x;\n", pacyc->instr[i]);
        TRACE1("        acyc.xid[%d]   = ", i); TRACE2("0x%x;\n", pacyc->xid[i]);
    }
    TRACE_FCTA("ApiCmdBCAcycPrep", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&acyc");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCAcycPrep

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCAcycSend
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
//    This function applies the 'TG_API_BC_ACYC_MSG_SEND' instruction to the API1553-DS
//    ASP Driver SW to start the BC Acyclic Frame.
//
//***************************************************************************

AiInt16 ApiCmdBCAcycSend_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode, AiUInt32 timetag_high, AiUInt32 timetag_low )
{
    TY_MIL_COM_LS_BC_ACYC_SEND_INPUT   xInput;
    TY_MIL_COM_ACK                     xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.mode       = mode;
    xInput.ttag_high  = timetag_high;
    xInput.ttag_low   = timetag_low;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_ACYC_SEND, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_ACYC_SEND( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAcycSend(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mode, AiUInt32 timetag_high, AiUInt32 timetag_low )
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( mode > MAX_ACYC_MODE )
        return API_ERR_PARAM3_NOT_IN_RANGE;


    uw_RetVal = ApiCmdBCAcycSend_( bModule, biu, mode, timetag_high, timetag_low );


    v_ExamineRetVal( "ApiCmdBCAcycSend", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCAcycSend", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(mode);
    TRACE_PARA(timetag_high);
    TRACE_PARE(timetag_low);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
} // end: ApiCmdBCAcycSend


//***************************************************************************
//
//   Submodule : ApiCmdBCAcycPrepAndSendTransferBlocking
//
//---------------------------------------------------------------------------
//   Create : 16.06.2020   Update : 16.06.2020
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : xfer            - A predefined transfer descriptor which can be overwritten before the transfer is send
//                data            - The transmit data
//
//    Outputs   : data            - The receive data
//                transfer_status - The transfer status queue entry.
//
//    Description :
//    This function combines ApiCmdBcXferDef, APiCmdBufDef, ApiCmdBcAcycPrep, ApiCmdBcAcycSend and ApiCmdBCStatusRead into a single TSW command for performance reasons.
//
//***************************************************************************

AiReturn ApiCmdBCAcycPrepAndSendTransferBlocking_(AiUInt32 ulModHandle, AiUInt8 biu, TY_API_BC_XFER * xfer, AiUInt16 data[32], TY_API_BC_XFER_DSP * transfer_status)
{
    MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_INPUT   xInput;
    MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_OUTPUT  xOutput;
    AiReturn ulRetVal = API_OK;
    int i;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(ulModHandle), 0, MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER, sizeof(xInput), sizeof(xOutput));

    // BC xfer def struct members
    xInput.bc_xfer_def.xid         = xfer->xid;
    xInput.bc_xfer_def.hid         = xfer->hid;
    xInput.bc_xfer_def.xfer_type   = xfer->type;
    xInput.bc_xfer_def.chn         = xfer->chn;
    xInput.bc_xfer_def.xmt_rt      = xfer->xmt_rt;
    xInput.bc_xfer_def.rcv_rt      = xfer->rcv_rt;
    xInput.bc_xfer_def.xmt_sa      = xfer->xmt_sa;
    xInput.bc_xfer_def.rcv_sa      = xfer->rcv_sa;
    xInput.bc_xfer_def.wcnt        = xfer->wcnt;
    xInput.bc_xfer_def.xfer_ir     = xfer->tic;
    xInput.bc_xfer_def.xfer_hlt    = xfer->hlt;
    xInput.bc_xfer_def.rte         = xfer->rte;
    xInput.bc_xfer_def.sxh         = xfer->sxh;
    xInput.bc_xfer_def.rsp         = xfer->rsp;
    xInput.bc_xfer_def.res         = xfer->res;
    xInput.bc_xfer_def.swxm        = xfer->swxm;
    xInput.bc_xfer_def.err_type    = xfer->err.type;
    xInput.bc_xfer_def.err_wpos    = (xfer->err.err_spec >> 16) & 0xFF;
    xInput.bc_xfer_def.err_bpos    = (xfer->err.err_spec >> 8)  & 0xFF;
    xInput.bc_xfer_def.err_sync    = xfer->err.sync;
    xInput.bc_xfer_def.err_contig  = xfer->err.contig;
    xInput.bc_xfer_def.err_bc_bits = (xfer->err.err_spec) & 0xFF;
    xInput.bc_xfer_def.delay_mode  = xfer->gap_mode;
    xInput.bc_xfer_def.gap         = xfer->gap;

    // BC data buffer
    xInput.buf_def.bt    = API_BC_BUF;
    xInput.buf_def.hid   = xfer->hid;
    xInput.buf_def.bid   = 0;
    xInput.buf_def.count = 32;

    for (i=0; i<32; i++)
        xInput.buf_def.data[i]   = data[i];

    ulRetVal = API_CMD_STRUCT_LS_BC_ACYC_PREP_AND_SEND_TRANSFER(ulModHandle, &xInput, &xOutput);

    for (i=0; i<32; i++)
        data[i] = xOutput.buf_read.data[i];

    transfer_status->bid        = xOutput.bc_xfer_read.bid;
    transfer_status->brw        = xOutput.bc_xfer_read.brw;
    transfer_status->bufp       = xOutput.bc_xfer_read.bufp;
    transfer_status->cw1        = xOutput.bc_xfer_read.cw1;
    transfer_status->cw2        = xOutput.bc_xfer_read.cw2;
    transfer_status->err_cnt    = xOutput.bc_xfer_read.err_cnt;
    transfer_status->lvw        = xOutput.bc_xfer_read.lvw;
    transfer_status->msg_cnt    = xOutput.bc_xfer_read.msg_cnt;
    transfer_status->srvreq_cnt = xOutput.bc_xfer_read.srvreq_cnt;
    transfer_status->st1        = xOutput.bc_xfer_read.st1;
    transfer_status->st2        = xOutput.bc_xfer_read.st2;
    transfer_status->ttag       = xOutput.bc_xfer_read.ttag;


    return ulRetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAcycPrepAndSendTransferBlocking(AiUInt32 ulModHandle, AiUInt8 biu, TY_API_BC_XFER * xfer, AiUInt16 data[32], TY_API_BC_XFER_DSP * transfer_status)
{
    AiReturn ulRetVal = API_OK;

    if ((0 == (ulModHandle & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)))
        return API_ERR_WRONG_BIU;

    ulRetVal = ApiCmdBCAcycPrepAndSendTransferBlocking_(ulModHandle, biu, xfer, data, transfer_status);

    v_ExamineRetVal("ApiCmdBCAcycPrepAndSendTransferBlocking", ulRetVal);

    TRACE_BEGIN
        TRACE_FCTA("ApiCmdBCAcycPrepAndSendTransferBlocking", ulRetVal);
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_FCTE;
    TRACE_END

        return ulRetVal;
}




//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCReadSrvReqVecStatus
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 08.01.01   Update : 08.01.01
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : rt address [uc_RtAddr]
//
//    Outputs   : pointer to service request vector word status structure [px_SrvReqVecStatus]
//
//    Description :
//    This function applies the 'TG_API_BC_SRVW_READ' instruction to the API1553-DS
//    ASP Driver SW to read the service request vector word status information.
//
//***************************************************************************

AiInt16 ApiCmdBCSrvReqVecStatus_(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                          TY_API_BC_SRVW *px_SrvReqVecStatus)
{
    TY_MIL_COM_LS_BC_SRVW_REQ_READ_INPUT  xInput;
    TY_MIL_COM_LS_BC_SRVW_REQ_READ_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.rt  = uc_RtAddr;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_SRVW_REQ_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_SRVW_REQ_READ( bModule, &xInput, &xOutput );

    px_SrvReqVecStatus->uw_XferId       = xOutput.xid;
    px_SrvReqVecStatus->uw_LastVecWord  = xOutput.lvw;
    px_SrvReqVecStatus->ul_SrvReqCnt    = xOutput.src;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCSrvReqVecStatus(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                          TY_API_BC_SRVW *px_SrvReqVecStatus)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( uc_RtAddr > MAX_RT_ADDR )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( NULL == px_SrvReqVecStatus )
        return API_ERR_PARAM4_IS_NULL;

    uw_RetVal = ApiCmdBCSrvReqVecStatus_(bModule, uc_Biu, uc_RtAddr, px_SrvReqVecStatus);
    
    v_ExamineRetVal( "ApiCmdBCSrvReqVecStatus", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_SRVW x_SrvReqVecStatus;\n");
    TRACE_FCTA("ApiCmdBCSrvReqVecStatus", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(uc_Biu);
    TRACE_PARA(uc_RtAddr);
    TRACE_RPARE("&x_SrvReqVecStatus");
    TRACE_FCTE;
    TRACE1("    --> x_SrvReqVecStatus.ul_SrvReqCnt   = 0x%x\n", px_SrvReqVecStatus->ul_SrvReqCnt);
    TRACE1("    --> x_SrvReqVecStatus.uw_LastVecWord = 0x%x\n", px_SrvReqVecStatus->uw_LastVecWord);
    TRACE1("    --> x_SrvReqVecStatus.uw_XferId      = 0x%x\n", px_SrvReqVecStatus->uw_XferId);
    TRACE_END

    return uw_RetVal;

} // end: ApiCmdBCSrvReqVecStatus

//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCReadSrvReqVecStatus
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 08.01.01   Update : 08.01.01
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : rt address [uc_RtAddr]
//
//    Outputs   : pointer to service request vector word status structure [px_SrvReqVecStatus]
//
//    Description :
//    This function applies the 'TG_API_BC_SRVW_READ' instruction to the API1553-DS
//    ASP Driver SW to read the service request vector word status information.
//
//***************************************************************************

AiInt16 ApiCmdBCSrvReqVecCon_(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                       TY_API_BC_SRVW_CON *px_SrvReqVecCon)
{
    TY_MIL_COM_LS_BC_SRVW_REQ_CON_INPUT  xInput;
    TY_MIL_COM_ACK                       xOutput;
    AiInt16  uw_RetVal = API_OK;

    xInput.rt  = uc_RtAddr;
    xInput.con = px_SrvReqVecCon->uc_SubAddress;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_SRVW_REQ_CON, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_SRVW_REQ_CON( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCSrvReqVecCon(AiUInt32 bModule, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                       TY_API_BC_SRVW_CON *px_SrvReqVecCon)
{
    AiInt16  uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( uc_RtAddr > MAX_RT_ADDR )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( NULL == px_SrvReqVecCon )
        return API_ERR_PARAM4_IS_NULL;

    uw_RetVal = ApiCmdBCSrvReqVecCon_(bModule, uc_Biu, uc_RtAddr, px_SrvReqVecCon);

    v_ExamineRetVal( "ApiCmdBCSrvReqVecCon", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_SRVW x_SrvReqVecCon;\n");
    TRACE1("        x_SrvReqVecCon.uc_Padding1   = 0x%x;\n", px_SrvReqVecCon->uc_Padding1);
    TRACE1("        x_SrvReqVecCon.uc_SubAddress = 0x%x;\n", px_SrvReqVecCon->uc_SubAddress);
    TRACE1("        x_SrvReqVecCon.ul_Reserved1  = 0x%x;\n", px_SrvReqVecCon->ul_Reserved1);
    TRACE1("        x_SrvReqVecCon.ul_Reserved2  = 0x%x;\n", px_SrvReqVecCon->ul_Reserved2);
    TRACE1("        x_SrvReqVecCon.ul_Reserved3  = 0x%x;\n", px_SrvReqVecCon->ul_Reserved3);
    TRACE1("        x_SrvReqVecCon.uc_Padding2   = 0x%x;\n", px_SrvReqVecCon->uw_Padding2);
    TRACE_FCTA("ApiCmdBCSrvReqVecCon", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(uc_Biu);
    TRACE_PARA(uc_RtAddr);
    TRACE_RPARE("&x_SrvReqVecCon");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;

} // end: ApiCmdBCSrvReqVecCon

//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCInstrTblIni
//
//   Author : Marco Motz               Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 24.08.02   Update : 24.08.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AiInt16 ApiCmdBCInstrTblIni_(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;
	TY_API_GET_MEM_INFO x_MemInfo;
	AiUInt32 ul_Biu;
	TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

	uw_RetVal = ApiCmdSysGetMemPartition(bModule, 0, &x_MemInfo);
	if( API_OK == uw_RetVal )
	{
		uw_RetVal = uw_ApiGetBiuFromStream( bModule, biu, &ul_Biu);
		if( API_OK == uw_RetVal )
		{
			/* save information */
			pDevice->api_bc_glob_fw[ul_Biu-1].xfer.max_cnt     = x_MemInfo.ax_BiuCnt[ul_Biu-1].ul_BcXferDesc;
			pDevice->api_bc_glob_fw[ul_Biu-1].xfer.start_addr  = x_MemInfo.ax_BiuAddr[ul_Biu-1].ul_BcXferDesc;
			pDevice->api_bc_glob_fw[ul_Biu-1].instr.max_cnt    = x_MemInfo.ax_BiuSize[ul_Biu-1].ul_BcHipInstr / 4L;
			pDevice->api_bc_glob_fw[ul_Biu-1].instr.start_addr = x_MemInfo.ax_BiuAddr[ul_Biu-1].ul_BcHipInstr;
		}
	}

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCInstrTblIni(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdBCInstrTblIni_(bModule, biu);
    }

    v_ExamineRetVal( "ApiCmdBCInstrTblIni", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCInstrTblIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;

} /* end: ApiCmdBCInstrTblIni */

//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCInstrTblGetAddrFromLabel
//
//   Author : Marco Motz               Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 24.08.02   Update : 24.08.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AiInt16 ApiCmdBCInstrTblGetAddrFromLabel_(AiUInt32 bModule, AiUInt8 biu, 
                                                                   AiUInt32 label, AiUInt32 cnt, TY_API_BC_FW_INSTR *tbl, 
                                                                   AiUInt32 *raddr, AiUInt32 *line)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i,addr;

    addr = 0L;
    *line = 0L;

    if (tbl != NULL)
    {
    	for (i=0; i<cnt; i++)
    	{
    		if (tbl[i].label != 0)
    		{
    			if (tbl[i].label == label)
    			{
    				addr = tbl[i].laddr;
    				*line = i;
    				break;
    			}
    		}
    	}
    }

    /* address value */
    *raddr = addr;

    return uw_RetVal;

}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCInstrTblGetAddrFromLabel(AiUInt32 bModule, AiUInt8 biu, 
                                                                   AiUInt32 label, AiUInt32 cnt, TY_API_BC_FW_INSTR *tbl, 
                                                                   AiUInt32 *raddr, AiUInt32 *line)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == tbl )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else if( NULL == raddr )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if( NULL == line )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBCInstrTblGetAddrFromLabel_(bModule, biu, label, cnt, tbl, raddr, line);
    }

    v_ExamineRetVal( "ApiCmdBCInstrTblGetAddrFromLabel", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_FW_INSTR tbl;\n");
    TRACE("        AiUInt32           raddr;\n");
    TRACE("        AiUInt32           line;\n");
    for(i=0; i<cnt; i++)
    {
        TRACE1("        tbl[%d].label = ", i); TRACE2("0x%x;\n", tbl[i].label);
        TRACE1("        tbl[%d].laddr = ", i); TRACE2("0x%x;\n", tbl[i].laddr);
        TRACE1("        tbl[%d].op    = ", i); TRACE2("0x%x;\n", tbl[i].op);
        TRACE1("        tbl[%d].par1  = ", i); TRACE2("0x%x;\n", tbl[i].par1);
        TRACE1("        tbl[%d].par2  = ", i); TRACE2("0x%x;\n", tbl[i].par2);
        TRACE1("        tbl[%d].res   = ", i); TRACE2("0x%x;\n", tbl[i].res);
    }
    TRACE_FCTA("ApiCmdBCInstrTblGetAddrFromLabel", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(label);
    TRACE_PARA(cnt);
    TRACE_RPARA("&tbl");
    TRACE_RPARA("&raddr");
    TRACE_RPARE("&line");
    TRACE_FCTE;
    TRACE1("    --> raddr = 0x%x\n", *raddr);
    TRACE1("    --> line  = 0x%x\n", *line);
    TRACE_END

    return uw_RetVal;
} /* end: ApiCmdBCInstrTblGetAddrFromLabel */

//***************************************************************************
//
//   Module : APILSBC                  Submodule : ApiCmdBCInstrTblGen
//
//   Author : Marco Motz               Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 24.08.02   Update : 24.08.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AiInt16 ApiCmdBCInstrTblGen_(AiUInt32 bModule,         AiUInt8 biu, 
                                                      AiUInt8 mode,            AiUInt32 cnt, 
                                                      AiUInt32 dest_cnt,       AiUInt32 dest_offset, 
                                                      TY_API_BC_FW_INSTR *tbl, AiUInt32 *ctbl, 
                                                      AiUInt32 *err_line,      AiUInt8 *status)
{
	AiInt16 uw_RetVal = API_OK;
	AiUInt32 i,j,op,p1,p2,addr,saddr,max_addr,instr,label,line;
	AiUInt8 st;
	AiUInt32 ul_Temp;
	AiUInt32 ul_Biu;
	TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );


	st = API_OK;
	*err_line = 0L;

	uw_RetVal = uw_ApiGetBiuFromStream( bModule, biu, &ul_Biu);
	if( API_OK == uw_RetVal )
	{
		/* Available modes: */
		/* 0 = clear entire tbl */
		/* 1 = tbl conversion only */
		/* 2 = write ctbl only */
		/* 3 = tbl conversion & write ctbl */
		/* 4 = tbl conversion only (using "dest_offset") */
		/* 5 = tbl conversion & write ctbl (using "dest_offset") */
		if (mode == 0)
		{
			/* clear table */
			for (i=0; i<cnt; i++)
			{
				tbl[i].label = 0x0000;
				tbl[i].op    = 0x00;
				tbl[i].res   = 0x00;
				tbl[i].par1  = 0L;  
				tbl[i].par2  = 0L;  
				tbl[i].laddr = 0L;  
			}
		}

		if ((mode == 1) || (mode == 3) || (mode == 4) || (mode == 5))
		{
			/* Check on identical Label IDs */
			for (i=0; i<cnt; i++)
			{
				if (tbl[i].label != 0)
				{
					label = tbl[i].label; 
					for (j=0; j<cnt; j++)
					{
						if (j!=i)
						{
							if (tbl[j].label == label)
							{
								st = API_ERR;
								*err_line = i;
								break;
							}
						}
					}
				}
			}

			/* Set Address value for Label IDs */
			if (st == API_OK)
			{
				for (i=0; i<cnt; i++)
				{
					if (tbl[i].label != 0)
					{
						if ((mode == 1) || (mode == 3))
							tbl[i].laddr = pDevice->api_bc_glob_fw[ul_Biu-1].instr.start_addr + 4*i;
						else
							tbl[i].laddr = pDevice->api_bc_glob_fw[ul_Biu-1].instr.start_addr + 4*i + dest_offset;
					} 
				}
			}

			/* Generate Instruction image for Global Memory */
			if (st == API_OK)
			{
				for (i=0; i<cnt; i++)
				{
					/* Read instruction line */
					op = tbl[i].op;
					p1 = tbl[i].par1;
					p2 = tbl[i].par2;

					instr = 0L;

					/* Build instruction */
					switch(op)
					{
					case API_BC_FWI_XFER: /* Transfer */
						if (p1  < pDevice->api_bc_glob_fw[ul_Biu-1].xfer.max_cnt)
						{
							addr = pDevice->api_bc_glob_fw[ul_Biu-1].xfer.start_addr;

							/* Make sure addr is a PBI relative offset */
							if( addr > pDevice->x_BoardInfo.ul_GlobalRamStartOffset[ul_Biu-1] )
								addr -= pDevice->x_BoardInfo.ul_GlobalRamStartOffset[ul_Biu-1];

#ifdef _API3910
							if (p2 != 0)
							{
								/* Only used for HS-RTRT or HS-RTBC Xfers due to second LS */
								if ((p2 == API_HS_BC_TYPE_RTRT) ||
										(p2 == API_HS_BC_TYPE_RTBR) ||            
										(p2 == APIEF_BC_XFER_TYPE_EERTRT) ||      /* EFA  to EFA             */
										(p2 == APIEF_BC_XFER_TYPE_E_RTBR) ||      /* EFA  to EFA   Broadcast */
										(p2 == APIEF_BC_XFER_TYPE_E_RTBRMIXED)) { /* EFA  to MIX   Broadcast */
									/* Set address to second LS Transfer Descriptor */
									addr += ((p1 + pDevice->api_bc_glob_fw[ul_Biu-1].xfer.max_cnt + 1L) * MAX_API_BC_FW_XFER_DESC_SIZE * 4L);
								}
								else 
									if ((p2 == APIEF_BC_XFER_TYPE_XXRTRT    ) ||    /* EFEX to EFEX            */
											(p2 == APIEF_BC_XFER_TYPE_MCRT     ) ||    /* EFEX Modecode           */
											(p2 == APIEF_BC_XFER_TYPE_MCBR     )) {    /* EFEX Modecode Broadcast */
										if (p1 != 0) {
											op = API_BC_FWI_EFEX_XFER; /* pure EFEX */
											addr = p1; /* HS-ID */ 
										}
										else
											st = API_ERR; 
									}  
							}   
							else
#endif
								addr += (p1 * MAX_API_BC_FW_XFER_DESC_SIZE * 4L);
							/* Write Instruction */    
							instr = API_BC_FW_MAKE_INSTR(op,addr);
						}
						else
							st = API_ERR; 
						break;

					case API_BC_FWI_EFEX_XFER: /* EFEX Transfer */
						if (p1  < pDevice->api_bc_glob_fw[ul_Biu-1].xfer.max_cnt)
						{
							instr = API_BC_FW_MAKE_INSTR(op,p1/*HSID*/);
						}
						else
							st = API_ERR; 
						break;

					case API_BC_FWI_CALL: /* Call Subtable */
						if (p1 != 0)
						{
							ApiCmdBCInstrTblGetAddrFromLabel(bModule, biu, p1, cnt, tbl, &addr, &line);
							if (addr != 0L)
							{
								instr = API_BC_FW_MAKE_INSTR(op,addr);
							}
							else
								st = API_ERR; 
						}
						else
							st = API_ERR; 
						break;

					case API_BC_FWI_RET: /* Return from Subtable */
						instr = API_BC_FW_MAKE_INSTR(op,0L);
						break;

					case API_BC_FWI_JUMP: /* Jump to Subtable */
						if (p1 != 0)
						{
							ApiCmdBCInstrTblGetAddrFromLabel(bModule, biu, p1, cnt,tbl, &addr, &line);
							if (addr != 0L)
							{
								instr = API_BC_FW_MAKE_INSTR(op,addr);
							}
							else
								st = API_ERR; 
						}
						else
							//                  st = API_ERR;               
						instr = API_BC_FW_MAKE_INSTR(op,0); /* needed for termination of Low-Priority lists */
						break;
					case API_BC_FWI_SKIP: /* Relative Branch (Skip) */
						if (p1 != 0)
						{
							instr = API_BC_FW_MAKE_INSTR(op, (p1 & 0xFFFFL) | ((p2 & 0x3L) << 24));
						}
						else
							st = API_ERR; 
						break;

					case API_BC_FWI_WTRG: /* Wait for BC Trigger input */
						instr = API_BC_FW_MAKE_INSTR(op, (p2 & 0x3L));
						break;

					case API_BC_FWI_STRB: /* Strobe BC Trigger output */
						instr = API_BC_FW_MAKE_INSTR(op,0L);
						break;

					case API_BC_FWI_DJZ: /* Decrement and Jump on Zero */
						if (p1 != 0)
						{
							ApiCmdBCInstrTblGetAddrFromLabel(bModule, biu, p1, cnt, tbl, &addr, &line);
							if (addr != 0L)
							{
								instr = API_BC_FW_MAKE_INSTR(op,addr);
							}
							else
								st = API_ERR; 
						}
						else
							st = API_ERR; 
						break;

					case API_BC_FWI_WMFT: /* Wait for next Minor Frame Time slot */
						instr = API_BC_FW_MAKE_INSTR(op,0L);
						break;

					case API_BC_FWI_HALT: /* BC Operation Halt */
						instr = API_BC_FW_MAKE_INSTR(op,0L);
						break;

                    case API_BC_FWI_DELAY: /* Delay */
                        instr = API_BC_FW_MAKE_INSTR(op, (p1 & 0x0000FFFFL));
                        break;

                    case API_BC_FWI_MODIFY: /* Modify instruction with PBI Relative GRAM pointer (26 bit) to Modify Descriptor*/
                        /* Modify descriptors can be created with ApiCmdBCInstrModifyDefine at end of instruction table
                           The Modify Descriptorts grow from top to bottom starting with id=1 */
                        instr = API_BC_FW_MAKE_INSTR(op, (p1 & 0x3FFFFFF));
                        break;
                        
					case API_BC_FWI_CMFT: /* Change Minor Frame Time */
						instr = API_BC_FW_MAKE_INSTR(op, (p1 & 0x000FFFFFL));
						break;
					case API_BC_FWI_RESMF: /* Reset Major Frame */
						instr = API_BC_FW_MAKE_INSTR(op, ((p1 & 0x1) << 24));
						break;
                    case API_BC_FWI_RET_ON_XFER_ERR: /* Return on Transfer Error */
                        instr = API_BC_FW_MAKE_INSTR(op, 0L);
                        break;
					}

					/* Break loop on error */
					if (st == API_ERR)
					{
						*err_line = i;
						break;
					}
					else {
						/* Write instruction word to array */
						ctbl[i] = instr;
					}
				} /* end for */
			} 
		}

		/* Write ctbl to Global memory */
		if ((mode == 2) || (mode == 3) || (mode == 5))
		{
			if (st == API_OK)
			{
				saddr    = pDevice->api_bc_glob_fw[ul_Biu-1].instr.start_addr + dest_offset;
				max_addr = pDevice->api_bc_glob_fw[ul_Biu-1].instr.start_addr +
						(pDevice->api_bc_glob_fw[ul_Biu-1].instr.max_cnt * 4L);
				if ((saddr + (4 * dest_cnt)) <= max_addr)
				{ /* check the range */
					if ((mode == 2) || (mode == 3))
						_ApiWriteBlockMemData(bModule,API_MEMTYPE_GLOBAL,saddr,sizeof(AiUInt32),ctbl+dest_offset/4,dest_cnt, &ul_Temp);
					else
						_ApiWriteBlockMemData(bModule,API_MEMTYPE_GLOBAL,saddr,sizeof(AiUInt32),ctbl,dest_cnt, &ul_Temp);
				}
				else
				{
					st = API_ERR;
					*err_line = 0L;
				}
			}
		}
	}
	else
	{
		st = API_ERR;
		*err_line = 0L;
	}

	/* status value */
	*status = st;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCInstrTblGen(AiUInt32 bModule,         AiUInt8 biu, 
                                                      AiUInt8 mode,            AiUInt32 cnt, 
                                                      AiUInt32 dest_cnt,       AiUInt32 dest_offset, 
                                                      TY_API_BC_FW_INSTR *tbl, AiUInt32 *ctbl, 
                                                      AiUInt32 *err_line,      AiUInt8 *status)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;


    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == tbl )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == ctbl )
        uw_RetVal = API_ERR_PARAM8_IS_NULL;
    else if( NULL == err_line )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else if( NULL == status )
        uw_RetVal = API_ERR_PARAM10_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBCInstrTblGen_(bModule, biu, mode, cnt, dest_cnt, dest_offset, tbl, ctbl, err_line, status);
    }

    v_ExamineRetVal( "ApiCmdBCInstrTblGen", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_FW_INSTR tbl;\n");
    TRACE("        AiUInt32           ctbl;\n");
    TRACE("        AiUInt32           err_line;\n");
    TRACE("        AiUInt32           status;\n");
    for(i=0; i<cnt; i++)
    {
        TRACE1("        tbl[%d].label = ", i); TRACE2("0x%x;\n", tbl[i].label);
        TRACE1("        tbl[%d].laddr = ", i); TRACE2("0x%x;\n", tbl[i].laddr);
        TRACE1("        tbl[%d].op    = ", i); TRACE2("0x%x;\n", tbl[i].op);
        TRACE1("        tbl[%d].par1  = ", i); TRACE2("0x%x;\n", tbl[i].par1);
        TRACE1("        tbl[%d].par2  = ", i); TRACE2("0x%x;\n", tbl[i].par2);
        TRACE1("        tbl[%d].res   = ", i); TRACE2("0x%x;\n", tbl[i].res);
    }
    TRACE_FCTA("ApiCmdBCInstrTblGen", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(mode);
    TRACE_PARA(cnt);
    TRACE_PARA(dest_cnt);
    TRACE_PARA(dest_offset);
    TRACE_RPARA("&tbl");
    TRACE_RPARA("&ctbl");
    TRACE_RPARA("&err_line");
    TRACE_RPARE("&status");
    TRACE_FCTE;
    TRACE1("    --> ctbl     = 0x%x\n", *ctbl);
    TRACE1("    --> err_line = 0x%x\n", *err_line);
    TRACE1("    --> status   = 0x%x\n", *status);
    TRACE_END

    return uw_RetVal;

} /* end: ApiCmdBCInstrTblGen */




AiReturn ApiCmdBCInstrModifyDefine_(AiUInt32 bModule, AiUInt8 biu, AiUInt32 id, TY_API_BC_MODIFY_INSTRUCTION * modify, AiUInt32 * offset)
{
    TY_MIL_COM_LS_BC_MODIFY_DEF_INPUT     xInput;
    TY_MIL_COM_LS_BC_MODIFY_DEF_OUTPUT    xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_MODIFY_DEF, sizeof(xInput), sizeof(xOutput));

    xInput.id = id;
    memcpy(&xInput.modify, modify, sizeof(TY_API_BC_MODIFY_INSTRUCTION));

    uw_RetVal = API_CMD_STRUCT_LS_BC_MODIFY_DEF(bModule, &xInput, &xOutput);

    *offset = xOutput.offset;

    return uw_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCInstrModifyDefine(AiUInt32 bModule, AiUInt8 biu, AiUInt32 id, TY_API_BC_MODIFY_INSTRUCTION * modify, AiUInt32 * offset)
{
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(bModule);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if (id == 0)
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if (NULL == modify)
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if (NULL == offset)
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBCInstrModifyDefine_(bModule, biu, id, modify, offset);
    }

    v_ExamineRetVal("ApiCmdBCInstrModifyDefine", uw_RetVal);

    return uw_RetVal;

} /* end: ApiCmdBCInstrTblGen */



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
AiInt16 ApiCmdBCGetMajorFrameDefinition_(AiUInt32 bModule, AiUInt8 biu, TY_API_BC_MFRAME* px_BCMajorFrame)
{
    TY_MIL_COM                               xInput;
    TY_MIL_COM_LS_BC_MAJOR_FRAME_GET_OUTPUT  xOutput;
    AiInt16 uw_RetVal = API_OK;
    int i;


    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_LS_BC_MAJOR_FRAME_GET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_MAJOR_FRAME_GET( bModule, &xInput, &xOutput );

    if( uw_RetVal == 0 )
    {
        px_BCMajorFrame->cnt = xOutput.cnt;
        for(i=0;i<px_BCMajorFrame->cnt;i++)
            px_BCMajorFrame->fid[i] = xOutput.minor_frame[i];
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetMajorFrameDefinition (AiUInt32 bModule, AiUInt8 biu, TY_API_BC_MFRAME* px_BCMajorFrame)
{
    AiInt16 uw_RetVal = API_OK;
    int i;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;

    uw_RetVal = ApiCmdBCGetMajorFrameDefinition_(bModule, biu, px_BCMajorFrame);

    v_ExamineRetVal( "ApiCmdBCGetMajorFrameDefinition", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_BC_MFRAME x_BCMajorFrame;\n");
    TRACE_FCTA("ApiCmdBCGetMajorFrameDefinition", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&x_BCMajorFrame");
    TRACE_FCTE;
    for(i=0; i<px_BCMajorFrame->cnt; i++)
    {
        TRACE1("    --> x_BCMajorFrame.fid[%d] = ", i); TRACE2("0x%x;\n", px_BCMajorFrame->fid[i]);
    }
    TRACE_END

    return uw_RetVal;
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
AiInt16 ApiCmdBCGetMinorFrameDefinition_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 FrameID, TY_API_BC_FRAME* px_BCMinorFrame)
{
    TY_MIL_COM_LS_BC_MINOR_FRAME_GET_INPUT   xInput;
    TY_MIL_COM_LS_BC_MINOR_FRAME_GET_OUTPUT  xOutput;
    AiInt16 uw_RetVal = API_OK;
    int i;

    xInput.frame_id  = FrameID;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_MINOR_FRAME_GET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_MINOR_FRAME_GET( bModule, &xInput, &xOutput );

    if( uw_RetVal == 0 )
    {
        px_BCMinorFrame->id  = xOutput.id;
        px_BCMinorFrame->cnt = xOutput.cnt;
        for(i=0;i<px_BCMinorFrame->cnt;i++)
        {
            px_BCMinorFrame->instr[i] = xOutput.instr[i];
            px_BCMinorFrame->xid[i] = xOutput.xid[i];
        }
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetMinorFrameDefinition (AiUInt32 bModule, AiUInt8 biu, AiUInt8 FrameID, TY_API_BC_FRAME* px_BCMinorFrame)
{
    AiInt16 uw_RetVal = API_OK;
    int i;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( (FrameID < MIN_API_BC_MFRAME) || (FrameID > MAX_API_BC_MFRAME) )
        return API_ERR_FRAME_ID_NOT_IN_RANGE;

    uw_RetVal = ApiCmdBCGetMinorFrameDefinition_(bModule, biu, FrameID, px_BCMinorFrame);
    
    v_ExamineRetVal( "ApiCmdBCGetMinorFrameDefinition", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_FRAME x_BCMinorFrame;\n");
    TRACE_FCTA("ApiCmdBCGetMinorFrameDefinition", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(FrameID);
    TRACE_RPARE("&x_BCMinorFrame");
    TRACE_FCTE;
    TRACE1("    --> x_BCMinorFrame.id = 0x%x", px_BCMinorFrame->id);
    for(i=0; i<px_BCMinorFrame->cnt; i++)
    {
        TRACE1("    --> x_BCMinorFrame.instr[%d] = ", i); TRACE2("0x%x;\n", px_BCMinorFrame->instr[i]);
        TRACE1("    --> x_BCMinorFrame.xid[%d]   = ", i); TRACE2("0x%x;\n", px_BCMinorFrame->xid[i]);
    }
    TRACE_END

    return uw_RetVal;
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
AiInt16 ApiCmdBCGetXferDef_(AiUInt32 Module, AiUInt8 Biu,
                                                     AiUInt16 uw_XferId,
                                                     TY_API_BC_XFER* px_BCXfer)
{
    TY_MIL_COM_LS_BC_XFER_DEF_GET_INPUT  xInput;
    TY_MIL_COM_LS_BC_XFER_DEF_GET_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_ERR;
    
    xInput.xid  = uw_XferId;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(Module), 0, MIL_COM_LS_BC_XFER_DEF_GET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_XFER_DEF_GET( Module, &xInput, &xOutput );

    px_BCXfer->xid          = xOutput.xid;
    px_BCXfer->hid          = xOutput.hid;
    px_BCXfer->swxm         = xOutput.swxm;
    px_BCXfer->gap          = xOutput.gap;
    px_BCXfer->type         = xOutput.xfer_type;
    px_BCXfer->chn          = xOutput.chn;
    px_BCXfer->xmt_rt       = xOutput.xmt_rt;
    px_BCXfer->rcv_rt       = xOutput.rcv_rt;
    px_BCXfer->xmt_sa       = xOutput.xmt_sa;
    px_BCXfer->rcv_sa       = xOutput.rcv_sa;
    px_BCXfer->wcnt         = xOutput.wcnt;
    px_BCXfer->tic          = xOutput.xfer_ir;
    px_BCXfer->hlt          = xOutput.xfer_hlt;
    px_BCXfer->rte          = xOutput.rte;
    px_BCXfer->res          = xOutput.res;
    px_BCXfer->sxh          = xOutput.sxh;
    px_BCXfer->rsp          = xOutput.rsp;
    px_BCXfer->err.type     = xOutput.err_type;
    px_BCXfer->err.sync     = xOutput.err_sync;
    px_BCXfer->err.contig   = xOutput.err_contig;
    px_BCXfer->err.padding1 = xOutput.err_padding;
    px_BCXfer->err.err_spec = (xOutput.err_wpos<<16) + (xOutput.err_bpos<<8) + xOutput.err_contig;
    px_BCXfer->gap_mode     = xOutput.gap_mode;

    return uw_RetVal;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetXferDef(AiUInt32 Module, AiUInt8 Biu,
                                                     AiUInt16 uw_XferId,
                                                     TY_API_BC_XFER* px_BCXfer)
{
    AiInt16 uw_RetVal = API_ERR;
    
    if( (0 == (Module & API_STREAM_MASK)) && ((Biu < MIN_BIU) || (Biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( (uw_XferId < MIN_XFER_ID) || (uw_XferId > MAX_XFER_ID) )
        return API_ERR_XID_NOT_IN_RANGE;

    uw_RetVal = ApiCmdBCGetXferDef_(Module, Biu, uw_XferId, px_BCXfer);

    v_ExamineRetVal( "ApiCmdBCGetXferDef", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_XFER x_BCXfer;\n");
    TRACE_FCTA("ApiCmdBCGetXferDef", uw_RetVal); 
    TRACE_PARA(Module);
    TRACE_PARA(Biu);
    TRACE_PARA(uw_XferId);
    TRACE_RPARE("&x_BCXfer");
    TRACE_FCTE;
    TRACE1("    --> x_BCXfer.chn      = 0x%x", px_BCXfer->chn);
    TRACE1("    --> x_BCXfer.err_type = 0x%x", px_BCXfer->err.type);
    TRACE1("    --> x_BCXfer.err_spec = 0x%x", px_BCXfer->err.err_spec);
    TRACE1("    --> x_BCXfer.gap      = 0x%x", px_BCXfer->gap);
    TRACE1("    --> x_BCXfer.gap_mode = 0x%x", px_BCXfer->gap_mode);
    TRACE1("    --> x_BCXfer.hid      = 0x%x", px_BCXfer->hid);
    TRACE1("    --> x_BCXfer.hlt      = 0x%x", px_BCXfer->hlt);
    TRACE1("    --> x_BCXfer.rcv_rt   = 0x%x", px_BCXfer->rcv_rt);
    TRACE1("    --> x_BCXfer.rcv_sa   = 0x%x", px_BCXfer->rcv_sa);
    TRACE1("    --> x_BCXfer.res      = 0x%x", px_BCXfer->res);
    TRACE1("    --> x_BCXfer.rsp      = 0x%x", px_BCXfer->rsp);
    TRACE1("    --> x_BCXfer.rte      = 0x%x", px_BCXfer->rte);
    TRACE1("    --> x_BCXfer.swxm     = 0x%x", px_BCXfer->swxm);
    TRACE1("    --> x_BCXfer.sxh      = 0x%x", px_BCXfer->sxh);
    TRACE1("    --> x_BCXfer.tic      = 0x%x", px_BCXfer->tic);
    TRACE1("    --> x_BCXfer.type     = 0x%x", px_BCXfer->type);
    TRACE1("    --> x_BCXfer.wcnt     = 0x%x", px_BCXfer->wcnt);
    TRACE1("    --> x_BCXfer.xid      = 0x%x", px_BCXfer->xid);
    TRACE1("    --> x_BCXfer.xmt_rt   = 0x%x", px_BCXfer->xmt_rt);
    TRACE1("    --> x_BCXfer.xmt_sa   = 0x%x", px_BCXfer->xmt_sa);
    TRACE_END

    return uw_RetVal;
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
AiInt16 ApiCmdBCBHRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 uw_HeaderId, TY_API_BC_BH_INFO *pbh)
{
    TY_MIL_COM_LS_BC_BH_READ_INPUT  xInput;
    TY_MIL_COM_LS_BC_BH_READ_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_OK;

    xInput.hid  = uw_HeaderId;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_BH_READ, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_BH_READ( bModule, &xInput, &xOutput );

    pbh->bid       = xOutput.bid;
    pbh->bq_addr   = xOutput.bq_addr;
    pbh->eid       = xOutput.eid;
    pbh->eq_addr   = xOutput.eq_addr;
    pbh->hid_addr  = xOutput.hid_addr;
    pbh->nbufs     = xOutput.nbufs;
    pbh->sid       = xOutput.sid;
    pbh->sq_addr   = xOutput.sq_addr;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCBHRead (AiUInt32 bModule, AiUInt8 biu, AiUInt16 uw_HeaderId, TY_API_BC_BH_INFO *pbh)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;


    uw_RetVal = ApiCmdBCBHRead_(bModule, biu, uw_HeaderId, pbh);

    v_ExamineRetVal( "ApiCmdBCBHRead", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_BC_BH_INFO pbh;\n");
    TRACE_FCTA("ApiCmdBCBHRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(uw_HeaderId);
    TRACE_RPARE("&pbh");
    TRACE_FCTE;
    TRACE1("    --> pbh.bid      = 0x%x", pbh->bid);
    TRACE1("    --> pbh.bq_addr  = 0x%x", pbh->bq_addr);
    TRACE1("    --> pbh.eid      = 0x%x", pbh->eid);
    TRACE1("    --> pbh.eq_addr  = 0x%x", pbh->eq_addr);
    TRACE1("    --> pbh.hid_addr = 0x%x", pbh->hid_addr);
    TRACE1("    --> pbh.nbufs    = 0x%x", pbh->nbufs);
    TRACE1("    --> pbh.sid      = 0x%x", pbh->sid);
    TRACE1("    --> pbh.sq_addr  = 0x%x", pbh->sq_addr);
    TRACE_END

    return uw_RetVal;
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
AiInt16 ApiCmdBCGetXferBufferHeaderInfo_(AiUInt32 bModule, AiUInt8 Biu,
                                                                   AiUInt32 XferId, AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr)
{
    TY_MIL_COM_LS_BC_BH_INFO_INPUT  xInput;
    TY_MIL_COM_LS_BC_BH_INFO_OUTPUT xOutput;
    AiInt16 uw_RetVal = API_ERR;

    xInput.xid  = XferId;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_BH_INFO, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_BH_INFO( bModule, &xInput, &xOutput );

    *BufHeaderIndex = xOutput.bid;
    *BufHeaderAddr  = xOutput.buf_addr;

    return uw_RetVal;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetXferBufferHeaderInfo (AiUInt32 bModule, AiUInt8 Biu,
                                                                   AiUInt32 XferId, AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr)
{
    AiInt16 uw_RetVal = API_ERR;

    if( (0 == (bModule & API_STREAM_MASK)) && ((Biu < MIN_BIU) || (Biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( (XferId < MIN_XFER_ID) || (XferId > MAX_XFER_ID) )
        return API_ERR_XID_NOT_IN_RANGE;

    uw_RetVal = ApiCmdBCGetXferBufferHeaderInfo_(bModule, Biu, XferId, BufHeaderIndex, BufHeaderAddr);

    v_ExamineRetVal( "ApiCmdGetXferBufferHeaderInfo", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt32 BufHeaderIndex;\n");
    TRACE("        AiUInt32 BufHeaderAddr;\n");
    TRACE_FCTA("ApiCmdGetXferBufferHeaderInfo", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(Biu);
    TRACE_PARA(XferId);
    TRACE_RPARA("&BufHeaderIndex");
    TRACE_RPARE("&BufHeaderAddr");
    TRACE_FCTE;
    TRACE1("    --> BufHeaderIndex = 0x%x", *BufHeaderIndex);
    TRACE1("    --> BufHeaderAddr  = 0x%x", *BufHeaderAddr);
    TRACE_END

    return uw_RetVal;
} 

//***************************************************************************
//
//   Module : ApiLsBc.cpp              Submodule : ApiCmdBCGetDytagDef
//
//   Author : Jochen Pfaadt            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 16.11.04   Update : 16.11.2004
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Buffer Header ID [bc_hid],
//
//    Outputs   : BC Dynamic Tagging Mode [mode]
//                BC Dynamic Tagging Setup structure [bc_dytag]
//
//    Description :
//    This function reads the  dynamic tagging information of the specified BC transfer.
//
//***************************************************************************

AiInt16 ApiCmdBCGetDytagDef_(AiUInt32 bModule, AiUInt8 biu, 
                                                      AiUInt16 bc_hid, AiUInt16 *mode,
                                                      TY_API_BC_DYTAG *bc_dytag)
{
    TY_MIL_COM_LS_BC_DYTAG_DEF_GET_INPUT    xInput;
    TY_MIL_COM_LS_BC_DYTAG_DEF_GET_OUTPUT   xOutput;
    AiInt16 uw_RetVal = API_OK;
    int i = 0;

    xInput.bc_hid  = bc_hid;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BC_DYTAG_DEF_GET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_LS_BC_DYTAG_DEF_GET( bModule, &xInput, &xOutput );

    *mode = xOutput.mode;

    for( i=0; i<4; i++ )
    {
        bc_dytag[i].tag_fct = xOutput.tag_fct[i];
        bc_dytag[i].min     = xOutput.min[i];
        bc_dytag[i].max     = xOutput.max[i];
        bc_dytag[i].step    = xOutput.step[i];
        bc_dytag[i].wpos    = xOutput.wpos[i];
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCGetDytagDef(AiUInt32 bModule, AiUInt8 biu, 
                                                      AiUInt16 bc_hid, AiUInt16 *mode,
                                                      TY_API_BC_DYTAG *bc_dytag)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;

    uw_RetVal = ApiCmdBCGetDytagDef_(bModule, biu, bc_hid, mode, bc_dytag);

    v_ExamineRetVal( "ApiCmdBCGetDytagDef", uw_RetVal );
    TRACE_BEGIN
    TRACE("        AiUInt16        mode;\n");
    TRACE("        TY_API_BC_DYTAG bc_dytag;\n");
    TRACE_FCTA("ApiCmdBCGetDytagDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bc_hid);
    TRACE_RPARA("&mode");
    TRACE_RPARE("&bc_dytag");
    TRACE_FCTE;
    TRACE1("    --> mode             = 0x%x", *mode);
    TRACE1("    --> bc_dytag.max     = 0x%x", bc_dytag->max);
    TRACE1("    --> bc_dytag.min     = 0x%x", bc_dytag->min);
    TRACE1("    --> bc_dytag.step    = 0x%x", bc_dytag->step);
    TRACE1("    --> bc_dytag.tag_fct = 0x%x", bc_dytag->tag_fct);
    TRACE1("    --> bc_dytag.wpos    = 0x%x", bc_dytag->wpos);
    TRACE_END

    return(uw_RetVal);
} // end: ApiCmdBCGetDytagDef

//***************************************************************************
//
//   Module : ApiLsBc.cpp              Submodule : ApiCmdBCModeCtrl
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 05.02.07   Update : 19.02.07
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Mode definition structure
//
//    Outputs   : none
//
//    Description :
//    This function is used to set the BC Mode. (e.g. MC17 sync conter is included 
//    in attached transfer)
//
//***************************************************************************
AiInt16 ApiCmdBCModeCtrl_( AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_BC_MODE_CTRL *px_BcModeCtrl)
{
    TY_MIL_COM_LS_BC_MODE_CTRL_INPUT   xInput;
    TY_MIL_COM_ACK                     xOutput;
    AiInt16 w_RetVal = API_OK;

    xInput.mode   = px_BcModeCtrl->ul_BcMode;
    xInput.ctrl   = px_BcModeCtrl->ul_Ctrl;
    xInput.param1 = px_BcModeCtrl->ul_Param1;
    xInput.param2 = px_BcModeCtrl->ul_Param2;
    xInput.param3 = px_BcModeCtrl->ul_Param3;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_LS_BC_MODE_CTRL, sizeof(xInput), sizeof(xOutput) );

    w_RetVal = API_CMD_STRUCT_LS_BC_MODE_CTRL( ul_Module, &xInput, &xOutput );

    return w_RetVal;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCModeCtrl( AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_BC_MODE_CTRL *px_BcModeCtrl)
{
    AiInt16 w_RetVal = API_OK;

    if( (0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( NULL == px_BcModeCtrl )
        return API_ERR_PARAM3_IS_NULL;
    if( (px_BcModeCtrl->ul_BcMode < MIN_BC_MODE_CTRL) || (px_BcModeCtrl->ul_BcMode > MAX_BC_MODE_CTRL) )
        return API_ERR_PARAM4_NOT_IN_RANGE;
    if(API_BC_MODE_INSERT_MC17_SYNC_CNT == px_BcModeCtrl->ul_BcMode)
    {
        if( (px_BcModeCtrl->ul_Ctrl < API_DIS) || (px_BcModeCtrl->ul_Ctrl > API_ENA) ) 
            return API_ERR_PARAM5_NOT_IN_RANGE;
        else if( (px_BcModeCtrl->ul_Param1 < MIN_XFER_ID) || (px_BcModeCtrl->ul_Param1 > MAX_XFER_ID) ) 
            return API_ERR_PARAM6_NOT_IN_RANGE;
    }
    if(API_BC_MODE_STOP_ON_MC0_DBCA_FLAG == px_BcModeCtrl->ul_BcMode)
    {
        if( (px_BcModeCtrl->ul_Ctrl < API_DIS) || (px_BcModeCtrl->ul_Ctrl > API_ENA) ) 
            return API_ERR_PARAM5_NOT_IN_RANGE;
        else if( (px_BcModeCtrl->ul_Param1 < MIN_XFER_ID) || (px_BcModeCtrl->ul_Param1 > MAX_XFER_ID) ) 
            return API_ERR_PARAM6_NOT_IN_RANGE;
    }
    if(API_BC_MODE_CONFIGURED_DYTAGS == px_BcModeCtrl->ul_BcMode)
    {
        if( (px_BcModeCtrl->ul_Ctrl < API_DIS) || (px_BcModeCtrl->ul_Ctrl > API_ENA) ) 
            return API_ERR_PARAM5_NOT_IN_RANGE;
    }


    w_RetVal = ApiCmdBCModeCtrl_( ul_Module, uc_Biu, px_BcModeCtrl);


    v_ExamineRetVal( "ApiCmdBCModeCtrl", w_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BC_MODE_CTRL x_BcModeCtrl;\n");
    TRACE1("        x_BcModeCtrl.ul_BcMode = 0x%x;", px_BcModeCtrl->ul_BcMode);
    TRACE1("        x_BcModeCtrl.ul_Ctrl   = 0x%x;", px_BcModeCtrl->ul_Ctrl);
    TRACE1("        x_BcModeCtrl.ul_Param1 = 0x%x;", px_BcModeCtrl->ul_Param1);
    TRACE1("        x_BcModeCtrl.ul_Param2 = 0x%x;", px_BcModeCtrl->ul_Param2);
    TRACE1("        x_BcModeCtrl.ul_Param3 = 0x%x;", px_BcModeCtrl->ul_Param3);
    TRACE_FCTA("ApiCmdBCModeCtrl", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_RPARE("&x_BcModeCtrl");
    TRACE_FCTE;
    TRACE_END

    return w_RetVal;
}




/*! Control the traffic override setting of the BC
*
* This function can be used to enable or disable the traffic override control setting of the BC.
* The value must be set before BC start is called.
* \param con API_ENA to enable or API_DIS to disable the traffic override feature
* \return returns API_OK on success, an appropriate error code otherwise
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCTrafficOverrideControl(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 con)
{
    AiReturn ret = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ulModHandle);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if ((con != API_ENA) && (con != API_DIS))
        return API_ERR_INVALID_CON;

    ret = ApiCmdBCTrafficOverrideControl_(ulModHandle, biu, con);

    v_ExamineRetVal("ApiCmdBCTrafficOverrideControl", ret);

    return ret;
}

AiReturn ApiCmdBCTrafficOverrideControl_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 con)
{
    AiReturn ret = API_OK;
    TY_MIL_COM_WITH_VALUE   xInput;
    TY_MIL_COM_ACK          xOutput;

    xInput.value   = con;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(ulModHandle), 0, MIL_COM_LS_BC_TRAFFIC_OVERRIDE_CONTROL, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_STRUCT_LS_BC_TRAFFIC_OVERRIDE_CONTROL(ulModHandle, &xInput, &xOutput);

    return ret;
}

/*! Read the traffic override setting of the BC
*
* This function can be used to read the traffic override control setting of the BC.
* \param con API_ENA means enabled or API_DIS means the feature is disabled
* \return returns API_OK on success, an appropriate error code otherwise
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCTrafficOverrideGet(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 * con)
{
    AiReturn ret = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(ulModHandle);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if ( con == NULL )
        return API_ERR_PARAM1_IS_NULL;

    ret = ApiCmdBCTrafficOverrideGet_(ulModHandle, biu, con);

    v_ExamineRetVal("ApiCmdBCTrafficOverrideGet", ret);

    return ret;
}
AiReturn ApiCmdBCTrafficOverrideGet_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 * con)
{
    AiReturn ret = API_OK;
    TY_MIL_COM                 xInput;
    TY_MIL_COM_ACK_WITH_VALUE  xOutput;

    /* Set default output in case command is not supported by fw */
    xOutput.value = 0;

    MIL_COM_INIT(&xInput, API_STREAM(ulModHandle), 0, MIL_COM_LS_BC_TRAFFIC_OVERRIDE_GET, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_STRUCT_LS_BC_TRAFFIC_OVERRIDE_GET(ulModHandle, &xInput, &xOutput);

    if (con)
        *con   = xOutput.value;

    return ret;
}

