/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_bc.c
Bus Controller (BC) module of the AYI1553 ASP Driver SW. This module 
comprises the handler routines for the BC instructions.
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START

//#ifdef CMPL4SIM


TY_MIL_COM_TABLE_ENTRY axTargetLsBcCommands[] = {
  {MIL_COM_LS_BC_XFER_ERROR,     (TY_MIL_COM_FUNC_PTR)api_bc_xfer_def_err,      "api_bc_xfer_def_err",      sizeof(TY_MIL_COM_LS_XFER_ERROR_INPUT),        sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_XFER_DESC_GET,  (TY_MIL_COM_FUNC_PTR)api_bc_xfer_desc_get,     "api_bc_xfer_desc_get",     sizeof(TY_MIL_COM_LS_XFER_DESC_GET_INPUT),     sizeof(TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT)     },
  {MIL_COM_LS_BC_XFER_DESC_MOD,  (TY_MIL_COM_FUNC_PTR)api_bc_xfer_desc_mod,     "api_bc_xfer_desc_mod",     sizeof(TY_MIL_COM_LS_XFER_DESC_MOD_INPUT),     sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_INI,            (TY_MIL_COM_FUNC_PTR)api_bc_ini_cmd,           "api_bc_ini_cmd",           sizeof(TY_MIL_COM_LS_BC_INI_INPUT),            sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_BH_DEF,         (TY_MIL_COM_FUNC_PTR)api_bc_bhd_def_cmd,       "api_bc_bhd_def_cmd",       sizeof(TY_MIL_COM_LS_BC_BH_DEF_INPUT),         sizeof(TY_MIL_COM_LS_BC_BH_DEF_OUTPUT)         },
  {MIL_COM_LS_BC_XFER_DEF,       (TY_MIL_COM_FUNC_PTR)api_bc_xfer_def_cmd,      "api_bc_xfer_def_cmd",      sizeof(TY_MIL_COM_LS_BC_XFER_DEF_INPUT),       sizeof(TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT)       },
  {MIL_COM_LS_BC_XFER_CTRL ,     (TY_MIL_COM_FUNC_PTR)api_bc_xfer_ctrl,         "api_bc_xfer_ctrl",         sizeof(TY_MIL_COM_LS_BC_XFER_CTRL_INPUT),      sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_START,          (TY_MIL_COM_FUNC_PTR)api_bc_start_cmd,         "api_bc_start_cmd",         sizeof(TY_MIL_COM_LS_BC_START_INPUT),          sizeof(TY_MIL_COM_LS_BC_START_OUTPUT)          },
  {MIL_COM_LS_BC_HALT,           (TY_MIL_COM_FUNC_PTR)api_bc_halt_cmd,          "api_bc_halt_cmd",          sizeof(TY_MIL_COM),                            sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_XFER_DEF_GET,   (TY_MIL_COM_FUNC_PTR)api_bc_xfer_def_get,      "api_bc_xfer_def_get",      sizeof(TY_MIL_COM_LS_BC_XFER_DEF_GET_INPUT),   sizeof(TY_MIL_COM_LS_BC_XFER_DEF_GET_OUTPUT)   },
  {MIL_COM_LS_BC_BH_READ,        (TY_MIL_COM_FUNC_PTR)api_bc_bhd_read,          "api_bc_bhd_read",          sizeof(TY_MIL_COM_LS_BC_BH_READ_INPUT),        sizeof(TY_MIL_COM_LS_BC_BH_READ_OUTPUT)        },
  {MIL_COM_LS_BC_BH_INFO,        (TY_MIL_COM_FUNC_PTR)api_bc_xfer_bh_info_get,  "api_bc_xfer_bh_info_get",  sizeof(TY_MIL_COM_LS_BC_BH_INFO_INPUT),        sizeof(TY_MIL_COM_LS_BC_BH_INFO_OUTPUT)        },
  {MIL_COM_LS_BC_MAJOR_FRAME_GET,(TY_MIL_COM_FUNC_PTR)api_bc_major_frame_get,   "api_bc_major_frame_get",   sizeof(TY_MIL_COM),                            sizeof(TY_MIL_COM_LS_BC_MAJOR_FRAME_GET_OUTPUT)},
  {MIL_COM_LS_BC_MINOR_FRAME_GET,(TY_MIL_COM_FUNC_PTR)api_bc_minor_frame_get,   "api_bc_minor_frame_get",   sizeof(TY_MIL_COM_LS_BC_MINOR_FRAME_GET_INPUT),sizeof(TY_MIL_COM_LS_BC_MINOR_FRAME_GET_OUTPUT)},
  {MIL_COM_LS_BC_DYTAG_DEF,      (TY_MIL_COM_FUNC_PTR)api_bc_dytag_def,         "api_bc_dytag_def",         sizeof(TY_MIL_COM_LS_BC_DYTAG_DEF_INPUT),      sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_DYTAG_DEF_GET,  (TY_MIL_COM_FUNC_PTR)api_bc_dytag_def_get,     "api_bc_dytag_def_get",     sizeof(TY_MIL_COM_LS_BC_DYTAG_DEF_GET_INPUT),  sizeof(TY_MIL_COM_LS_BC_DYTAG_DEF_GET_OUTPUT)  },
  {MIL_COM_LS_BC_MINOR_FRAME_DEF,(TY_MIL_COM_FUNC_PTR)api_bc_frame_def_cmd,     "api_bc_frame_def_cmd",     MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT_MINIMUM_SIZE, sizeof(TY_MIL_COM_ACK)                      },
  {MIL_COM_LS_BC_MAJOR_FRAME_DEF,(TY_MIL_COM_FUNC_PTR)api_bc_mframe_def_cmd,    "api_bc_mframe_def_cmd",    MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT_MINIMUM_SIZE, sizeof(TY_MIL_COM_ACK)                      },
  {MIL_COM_LS_BC_STATUS_READ,    (TY_MIL_COM_FUNC_PTR)api_bc_status_read_cmd,   "api_bc_status_read_cmd",   sizeof(TY_MIL_COM),                            sizeof(TY_MIL_COM_LS_BC_STATUS_READ_OUTPUT)    },
  {MIL_COM_LS_BC_XFER_READ,      (TY_MIL_COM_FUNC_PTR)api_bc_xfer_read_cmd,     "api_bc_xfer_read_cmd",     sizeof(TY_MIL_COM_LS_BC_XFER_READ_INPUT),      sizeof(TY_MIL_COM_LS_BC_XFER_READ_OUTPUT)      },
  {MIL_COM_LS_BC_SRVW_REQ_READ,  (TY_MIL_COM_FUNC_PTR)api_bc_srvw_req_read,     "api_bc_srvw_req_read",     sizeof(TY_MIL_COM_LS_BC_SRVW_REQ_READ_INPUT),  sizeof(TY_MIL_COM_LS_BC_SRVW_REQ_READ_OUTPUT)  },
  {MIL_COM_LS_BC_SRVW_REQ_CON,   (TY_MIL_COM_FUNC_PTR)api_bc_srvw_req_con,      "api_bc_srvw_req_con",      sizeof(TY_MIL_COM_LS_BC_SRVW_REQ_CON_INPUT),   sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_ACYC_PREP,      (TY_MIL_COM_FUNC_PTR)api_bc_acyc_msg_prep,     "api_bc_acyc_msg_prep",     MIL_COM_LS_BC_ACYC_PREP_INPUT_MINIMUM_SIZE,    sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_ACYC_SEND,      (TY_MIL_COM_FUNC_PTR)api_bc_acyc_msg_send_cmd, "api_bc_acyc_msg_send_cmd", sizeof(TY_MIL_COM_LS_BC_ACYC_SEND_INPUT),      sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_MODE_CTRL,      (TY_MIL_COM_FUNC_PTR)api_bc_mode_ctrl,         "api_bc_mode_ctrl",         sizeof(TY_MIL_COM_LS_BC_MODE_CTRL_INPUT),      sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_LS_BC_MODIFY_DEF,     (TY_MIL_COM_FUNC_PTR)api_bc_modify_def,        "api_bc_modify_def",        sizeof(TY_MIL_COM_LS_BC_MODIFY_DEF_INPUT),     sizeof(TY_MIL_COM_LS_BC_MODIFY_DEF_OUTPUT)     },
  { MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER, (TY_MIL_COM_FUNC_PTR)api_bc_acyc_prep_and_send_transfer, "api_bc_acyc_prep_and_send_transfer", sizeof(MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_INPUT), sizeof(MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_OUTPUT) },
  { MIL_COM_LS_BC_TRAFFIC_OVERRIDE_CONTROL, (TY_MIL_COM_FUNC_PTR)api_bc_traffic_override_control, "api_bc_traffic_override_control", sizeof(TY_MIL_COM_WITH_VALUE), sizeof(TY_MIL_COM_ACK) },
  { MIL_COM_LS_BC_TRAFFIC_OVERRIDE_GET, (TY_MIL_COM_FUNC_PTR)api_bc_traffic_override_get, "api_bc_traffic_override_get", sizeof(TY_MIL_COM), sizeof(TY_MIL_COM_ACK_WITH_VALUE) },
};



void api_ls_bc_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLsBcCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLsBcCommands;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_INI                */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Retry Control [retr],                                      */
/*                Service Request/Vector Word Control [svrq]                 */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_INI' instruction to initialize the   */
/*    API1553-DS BC mode.                                                    */
/*                                                                           */
/*****************************************************************************/

BYTE api_bc_ini(TY_API_DEV *p_api_dev, BYTE retr, BYTE svrq, BYTE tbm, BYTE gsb)
{
  short i, j, k, l;

  api_io_mode_con(p_api_dev, API_BC_MODE, API_OFF);
  api_io_bc_ini(p_api_dev, retr, svrq, tbm, gsb);

  /* Clear BC ISR Flag */
  api_io_bc_ir_clr(p_api_dev);

  p_api_dev->bc_bh_def.bid = 0;
  p_api_dev->bc_bh_def.sid = 0;
  p_api_dev->bc_bh_def.eid = 0;
  p_api_dev->bc_bh_def.nbufs = 0;
  p_api_dev->bc_bh_def.hid_addr = 0L;
  p_api_dev->bc_bh_def.bid_addr = 0L;
  p_api_dev->bc_bh_def.sid_addr = 0L;
  p_api_dev->bc_bh_def.eid_addr = 0L;

  p_api_dev->bc_xfer_dsp.cw1 = 0x0000;
  p_api_dev->bc_xfer_dsp.st1 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cw2 = 0x0000;
  p_api_dev->bc_xfer_dsp.st2 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cur_bid = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_brw = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_buf = 0L;
  p_api_dev->bc_xfer_dsp.cur_ttag = 0L;
  p_api_dev->bc_xfer_dsp.msg_cnt = 0L;
  p_api_dev->bc_xfer_dsp.err_cnt = 0L;

  p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
  p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
  p_api_dev->bc_dbc_stop_status[ p_api_dev->biu ] = API_HALT;
  p_api_dev->bc_dbc_stop_xfctrl_save[ p_api_dev->biu ] = 0;
  p_api_dev->bc_dbc_stop_xfswm_save[ p_api_dev->biu ] = 0;
  p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
  p_api_dev->bc_traffic_override_enabled[p_api_dev->biu] = API_OFF;

  p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_HALT;
  p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;

  p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res1 = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res2 = 0L;

  p_api_dev->bc_setup[ p_api_dev->biu ].smod = 0;
  p_api_dev->bc_setup[ p_api_dev->biu ].cnt = 0;
  p_api_dev->bc_setup[ p_api_dev->biu ].ftime = 0L;
  p_api_dev->bc_setup[ p_api_dev->biu ].sinst = 0L;
  p_api_dev->bc_setup[ p_api_dev->biu ].bc_triggered_inst = 0L;

  p_api_dev->bc_acyc_ttag_start[ p_api_dev->biu ].ena = 0L; /* AJR 25APR2003 Added for Ttag Acyc Cmd*/
  /* Minor Frame Setup */
  for(i = 0; i < MAX_API_BC_FRAME; i++)
  {
    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ i ].cnt = 0x0000;
    for(j = 0; j < MAX_API_BC_XFER_IN_FRAME; j++)
      p_api_dev->bc_setup[ p_api_dev->biu ].frame[ i ].instr[ j ] = 0L;
  }

  /* Major Frame Setup */
  p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt = 0x0000;
  for(j = 0; j < MAX_API_BC_FRAME_IN_MFRAME; j++)
    p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ j ] = 0x00;

  /* Support only Firmware BC Dynamic Data */
  for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area; j++)
  {
    p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ j ].ena = API_OFF;
    p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ j ].mode = 0;
  }

  /* Service Request and Vector Word handling */
  p_api_dev->bc_srvw_con[ p_api_dev->biu ] = svrq;
  for(j = 0; j < MAX_API_BC_SRVW_RT; j++)
  {
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].xid = 0x0000;
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].lvw = 0x0000;
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].src = 0L;
  }

  /* Xfer Interrupt Enable Setup */
  for(j = 0; j < (MAX_API_BC_XFER *MAX_MEM_MUL); j++)
  {
    p_api_dev->bc_setup[ p_api_dev->biu ].xfer_interrupt[ j ] = API_OFF;
  }

  for(j = 0; j < MAX_API_BC_SRVW_ID; j++)
  {
    for(k = 0; k < MAX_API_BC_SRVW_RT; k++)
    {
      for(l = 0; l < MAX_API_BC_SRVW_SAMID; l++)
      {
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ j ].ta[ k ].sa_mid[ l ].xid = 0;
      }
    }
  }

  p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt = 0;
  for(j = 0; j < MAX_API_BC_XFER_IN_FRAME; j++)
    p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ j ] = 0L;

  p_api_dev->ul_XferCntForAcyclic = 0;
  return 0;

} /* end: api_bc_ini */

L_WORD api_x_bc_ini(TY_API_DEV *p_api_dev, BYTE uc_retr, BYTE uc_svrq, BYTE uc_tbm, BYTE uc_gsb)
{
    L_WORD retVal = 0;

    retVal = api_bc_ini(p_api_dev, uc_retr, uc_svrq, uc_tbm, uc_gsb);
    if (retVal)
        return retVal;

    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        retVal = api39_bc_ini(p_api_dev);

    return retVal;

} /* end: api_x_bc_ini */


L_WORD api_bc_ini_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_INI_INPUT * in, TY_MIL_COM_ACK * out)
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_SCW_RETRY_TRANSFER_BUS_MODE))
        if (in->tbm != MIL_FW_RESERVED) return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_SCW_RETRY_GLOBAL_START_BUS))
        if (in->gsb != MIL_FW_RESERVED) return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    return api_x_bc_ini( p_api_dev, (BYTE)in->retries, (BYTE)in->svrq, (BYTE)in->tbm, (BYTE)in->gsb );
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_BHD_DEF            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 04.06.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Buffer Header ID [hid],                                 */
/*                BC Data Buffer ID [bid],                                   */
/*                BC Status Queue ID [sid],                                  */
/*                BC Event Queue ID [eid],                                   */
/*                Queue Size definition [qsize],                             */
/*                Buffer Queue Mode [bqm],                                   */
/*                Buffer Store Mode [bsm],                                   */
/*                Status Queue Mode [sts],                                   */
/*                Event Queue Mode [evs],                                    */
/*                Reserved for Double Buffer Mode [dbm]                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_BH_DEF' instruction to define a      */
/*    BC Buffer Header structure.                                            */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_bhd_def(TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE dbm)
{
    BYTE sts_ena;
    WORD nbufs, sq_id;

    if( (hid == 0) || (hid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area) )
    {
        return API_ERR_INVALID_HID;
    }

    sq_id = hid; 
    sts_ena = API_BC_BH_STS_ONE_SQ;
    if (p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_sq_area == p_api_dev->glb_mem.sim_buf_count[ p_api_dev->biu ])
    {
        sq_id = bid; /* use corresponding Buffer ID for StatusQ */
        if ((qsize != 0) && (sts == API_BC_BH_STS_QSIZE_SQ))
            sts_ena = API_BC_BH_STS_QSIZE_SQ;
    }

    nbufs = api_io_bc_bhd_def(p_api_dev, hid, 
                                         bid,                        /* Abs. Buffer ID */
                                         /*sid*/sq_id,               /* StatusQ ID */
                                         /*eid*/hid,                 /* EventQ ID */
                                         qsize,                      /* Queue Size */ 
                                         bqm,                        /* Buffer Queue Mode */
                                         bsm,                        /* Buffer Store Mode */
                                         /*sts*/sts_ena,             /* StatusQ Size */
                                         /*evs*/API_BC_BH_EVS_NO_EQ, /* EventQ Size */ 
                                         /*dbm*/0);

    /* write output into p_api_dev->bc_bh_def struct */
    p_api_dev->bc_bh_def.bid   = bid;
    p_api_dev->bc_bh_def.sid   = sq_id;
    p_api_dev->bc_bh_def.eid   = hid;
    p_api_dev->bc_bh_def.nbufs = nbufs;

    p_api_dev->bc_bh_def.hid_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area + (hid * MAX_API_BC_BH_DESC_SIZE);
    p_api_dev->bc_bh_def.bid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr );
    p_api_dev->bc_bh_def.sid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr );
    p_api_dev->bc_bh_def.eid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr );


    return 0;

} /* end: api_bc_bhd_def */



L_WORD api_bc_bhd_def_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_DEF_INPUT * in, TY_MIL_COM_LS_BC_BH_DEF_OUTPUT * out )
{
    L_WORD retval = api_bc_bhd_def(p_api_dev, (WORD)in->hid, (WORD)in->bid, (WORD)in->sid, (WORD)in->eid, (BYTE)in->qsize, (BYTE)in->bqm, (BYTE)in->bsm, (BYTE)in->sqm, (BYTE)in->eqm, (BYTE)in->dbm);

    out->bid       = p_api_dev->bc_bh_def.bid;
    out->sid       = p_api_dev->bc_bh_def.sid;
    out->eid       = p_api_dev->bc_bh_def.eid;
    out->nbufs     = p_api_dev->bc_bh_def.nbufs;
    out->hid_addr  = p_api_dev->bc_bh_def.hid_addr;
    out->bq_addr   = p_api_dev->bc_bh_def.bid_addr;
    out->sq_addr   = p_api_dev->bc_bh_def.sid_addr;
    out->eq_addr   = p_api_dev->bc_bh_def.eid_addr;

    return retval;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_XFER_DEF           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 21.10.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                BC Buffer Header ID [hid],                                 */
/*                Transfer Type [xfer_type],                                 */
/*                Transfer Channel [chn],                                    */
/*                RT Address of transmitting terminal [xmt_rt],              */
/*                RT Address of receiving terminal [rcv_rt],                 */
/*                Subaddress of transmitting terminal [xmt_sa],              */
/*                Subaddress of receiving terminal [rcv_sa],                 */
/*                Word Count/Modecode field [wcnt],                          */
/*                Transfer Interrupt Control [xfer_ir],                      */
/*                Transfer Halt Control [xfer_hlt],                          */
/*                Retry enable/disable [rte],                                */
/*                SW Exception Handling [sxh],                               */
/*                Response control [rsp],                                    */
/*                Reserved [res],                                            */
/*                Status Word Exception Mask [swxm],                         */
/*                Transfer error injection type [type],                      */
/*                Transfer error word position [wpos],                       */
/*                Transfer error bit position [bpos],                        */
/*                Sync field error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits],                  */
/*                Transfer Delay Mode [delay_mode],                          */
/*                Transfer Wait Time [gap]                                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_XFER_DEF' instruction to define a    */
/*    complete BC transfer including error injection from the in-parameters. */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_xfer_def(TY_API_DEV *p_api_dev, WORD xid, WORD hid, BYTE xfer_type, BYTE chn, BYTE xmt_rt, BYTE rcv_rt, BYTE xmt_sa, BYTE rcv_sa, BYTE wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, BYTE rsp, BYTE res, WORD swxm, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap, L_WORD *addr)
{
    if( addr != NULL )
        *addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc + (xid *MAX_API_BC_XFER_DESC_SIZE);

    if( (xid == 0) || (xid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) )
        return API_ERR_XID_NOT_IN_RANGE;

    if( (hid == 0) || (hid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area) )
        return API_ERR_HID_NOT_IN_RANGE;

    if( (xmt_rt >= MAX_API_RT_MC_SA) || (rcv_rt >= MAX_API_RT_MC_SA) )
        return API_ERR_INVALID_RT;

    if( (xmt_sa >= MAX_API_RT_MC_SA) || (rcv_sa >= MAX_API_RT_MC_SA) )
        return API_ERR_INVALID_RT_SA;

    if( wcnt >= MAX_API_RT_MC_SA )
        return API_ERR_PARAMETER_OUT_OF_RANGE;

    if (delay_mode == API_BC_XFER_IMG_FAST) /* fast gap mode is not supported for embedded */
    {
        /* Do not return an error in case of unsupported fast gap in standard fw for compatibility reasons. */
        if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    if (type != 0)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_ERR_INJECTION))
            return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

        if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
            return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

        if (API_DEVICE_MODE_IS_NOREPLAY_NOERROR(p_api_dev->drv_mode[p_api_dev->biu]))
            return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;
    }

    p_api_dev->errorInjection[ xid ].type    = type;
    p_api_dev->errorInjection[ xid ].wpos    = wpos;
    p_api_dev->errorInjection[ xid ].bpos    = bpos;
    p_api_dev->errorInjection[ xid ].sync    = sync;
    p_api_dev->errorInjection[ xid ].contig  = contig;
    p_api_dev->errorInjection[ xid ].bc_bits = bc_bits;
    p_api_dev->bcXferGap[ xid ]              = gap;



    if( ( (xfer_type == API_BC_XFER_TYPE_BCRT) || 
          (xfer_type == API_BC_XFER_TYPE_RTBC) || 
          (xfer_type == API_BC_XFER_TYPE_RTRT) ) &&
        ( (chn == API_BC_XFER_MIL_CHN_A) || 
          (chn == API_BC_XFER_MIL_CHN_B) ) )
    {
        if(xfer_ir > 0)
            p_api_dev->bc_setup[ p_api_dev->biu ].xfer_interrupt[ xid ] = API_ON;
        else
            p_api_dev->bc_setup[ p_api_dev->biu ].xfer_interrupt[ xid ] = API_OFF;

        /* Setup Transfer Descriptor */
        api_io_bc_xfer_def(p_api_dev, xid, hid, xfer_type, chn, xmt_rt, rcv_rt, xmt_sa, rcv_sa, wcnt, xfer_ir, xfer_hlt, rte, sxh, rsp, res, swxm, type, wpos, bpos, sync, contig, bc_bits, delay_mode, gap);

        /* set output parameter, if required */
        if( addr != NULL )
            *addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc + (xid *MAX_API_BC_XFER_DESC_SIZE);

        return 0;
    }
    else
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

} /* end: api_bc_xfer_def */



L_WORD api_bc_xfer_def_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_DEF_INPUT * in, TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT * out )
{
    L_WORD ret;
    L_WORD addr = 0;

    ret = api_bc_xfer_def( p_api_dev, (WORD)in->xid, (WORD)in->hid, (BYTE)in->xfer_type, (BYTE)in->chn, (BYTE)in->xmt_rt, (BYTE)in->rcv_rt, (BYTE)in->xmt_sa, (BYTE)in->rcv_sa, (BYTE)in->wcnt, (BYTE)in->xfer_ir, (BYTE)in->xfer_hlt, (BYTE)in->rte, (BYTE)in->sxh, (BYTE)in->rsp, (BYTE)in->res, (WORD)in->swxm, (BYTE)in->err_type, (BYTE)in->err_wpos, (BYTE)in->err_bpos, (BYTE)in->err_sync, (BYTE)in->err_contig, (BYTE)in->err_bc_bits, (BYTE)in->delay_mode, (WORD)in->gap, &addr );

    out->addr = addr;
    return ret;
}

/*---------------------------------------------------------------------------*/
/*   Create : 01.08.12   Update : 01.08.12                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                Transfer error injection type [type],                      */
/*                Transfer error word position [wpos],                       */
/*                Transfer error bit position [bpos],                        */
/*                Sync field error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits],                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function can be used to manipulate a BC error injection on the fly */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_xfer_def_err(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_ERROR_INPUT * pxInput, TY_MIL_COM_ACK * pxOutput )
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_ERR_INJECTION))
        return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

    if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
        return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

    if (API_DEVICE_MODE_IS_NOREPLAY_NOERROR(p_api_dev->drv_mode[p_api_dev->biu]))
        return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

    if (pxInput->ulXid == 0)
        return API_ERR_XID_NOT_IN_RANGE;

    if (pxInput->ulXid > p_api_dev->glb_mem.biu[p_api_dev->biu].count.bc_xfer_desc)
        return API_ERR_XID_NOT_IN_RANGE;

    p_api_dev->errorInjection[pxInput->ulXid].type    = (WORD)pxInput->xError.ul_Type;
    p_api_dev->errorInjection[ pxInput->ulXid ].sync    = (WORD)pxInput->xError.ul_Sync;
    p_api_dev->errorInjection[ pxInput->ulXid ].contig  = (WORD)pxInput->xError.ul_Contig;

    p_api_dev->errorInjection[ pxInput->ulXid ].wpos    = (WORD)(pxInput->xError.ul_ErrSpec >> 16) & 0xFF;
    p_api_dev->errorInjection[ pxInput->ulXid ].bpos    = (WORD)(pxInput->xError.ul_ErrSpec >>  8) & 0xFF;
    p_api_dev->errorInjection[ pxInput->ulXid ].bc_bits = (WORD)(pxInput->xError.ul_ErrSpec      ) & 0xFF;


    /* Error Injection */
    api_io_bc_xfer_err_con( p_api_dev, 
                            (WORD)pxInput->ulXid, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].type, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].wpos, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].bpos, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].sync, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].contig, 
                            (BYTE)p_api_dev->errorInjection[ pxInput->ulXid ].bc_bits );

    return 0;

} /* end: api_bc_xfer_def_err */



L_WORD api_bc_xfer_desc_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_DESC_GET_INPUT * pxInput, TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT * pxOutput )
{
    /* -- set ack size --- */
    pxOutput->xAck.ulSize = sizeof(TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT);

    /* -- check input parameters --- */

    if( pxInput->ulXid == 0 )
        return API_ERR_XID_NOT_IN_RANGE;

    if( pxInput->ulXid > p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc )
        return API_ERR_XID_NOT_IN_RANGE;

    /* -- call io layer for global RAM access --- */
    return api_io_bc_xfer_desc_get( p_api_dev, pxInput->ulXid, pxInput->ulMode, pxOutput->aulValue );

} /* end: api_bc_xfer_desc_get */



L_WORD api_bc_xfer_desc_mod(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_XFER_DESC_MOD_INPUT * pxInput, TY_MIL_COM_ACK * pxOutput )
{
    /* -- check input parameters --- */

    if( pxInput->ulXid == 0 )
      return API_ERR_XID_NOT_IN_RANGE;

    if( pxInput->ulXid > p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc )
      return API_ERR_XID_NOT_IN_RANGE;

    /* -- call io layer for global RAM access --- */

    return api_io_bc_xfer_desc_mod( p_api_dev, pxInput->ulXid, pxInput->ulMode, pxInput->aulValue );

} /* end: api_bc_xfer_desc_mod */




/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_XFER_CTRL          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                Enable/Disable BC Transfer [mode]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_XFER_CTRL' instruction to control    */
/*    the specified BC transfer.                                             */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_xfer_ctrl(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_CTRL_INPUT * in, TY_MIL_COM_ACK * out )
{
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        return api39_bc_xfer_ctrl( p_api_dev, (WORD)in->xid, (BYTE)in->mode);


    if( (in->xid == 0) || (in->xid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) )
    {
        return API_ERR_XID_NOT_IN_RANGE;
    }

    api_io_bc_xfer_ctrl(p_api_dev, (WORD)in->xid, (BYTE)in->mode);

    return 0;
} /* end: api_bc_xfer_ctrl */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_DYTAG_DEF          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_dytag_def(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_DYTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out )
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_EVENT_QUEUE))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    if( in->bc_hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area)
    {
        if(in->con == API_BC_DYTAG_ENA)
        {
            /* Enable Dynamic Data Function */
            if(p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ in->bc_hid ].ena == API_OFF)
            {
                p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ in->bc_hid ].ena = API_ON;

                /* Enable Event Queue processing */
                api_io_bc_bh_eq_con(p_api_dev, API_ON, in->bc_hid);

                /* Init Event Queue Descriptor */
                api_io_bc_dytag_def( p_api_dev, (WORD)in->bc_hid, (WORD)in->mode, (WORD)in->tag_fct[0], (WORD)in->min[0], (WORD)in->max[0], (WORD)in->step[0], (WORD)in->wpos[0],
                                                                                  (WORD)in->tag_fct[1], (WORD)in->min[1], (WORD)in->max[1], (WORD)in->step[1], (WORD)in->wpos[1],
                                                                                  (WORD)in->tag_fct[2], (WORD)in->min[2], (WORD)in->max[2], (WORD)in->step[2], (WORD)in->wpos[2],
                                                                                  (WORD)in->tag_fct[3], (WORD)in->min[3], (WORD)in->max[3], (WORD)in->step[3], (WORD)in->wpos[3]);
            }
        }
        else
        {
            /* Disable Dynamic Data Function */
            if(p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ in->bc_hid ].ena == API_ON)
            {
                p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ in->bc_hid ].ena = API_OFF;

                /* Disable Event Queue processing */
                api_io_bc_bh_eq_con(p_api_dev, API_OFF, in->bc_hid);

                /* Reset Event Queue Descriptor */
                api_io_bc_eq_ini(p_api_dev, (WORD)in->bc_hid);
            }
        }

        return 0;
    }


    return API_NAK;

} /* end: api_bc_dytag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_DYTAG_DEF_GET      */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_dytag_def_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_DYTAG_DEF_GET_INPUT * in, TY_MIL_COM_LS_BC_DYTAG_DEF_GET_OUTPUT * out )
{
    L_WORD w[4];
    WORD mode;
    WORD opc, func, wpos, fdw;
    int i;

    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_EVENT_QUEUE))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    /* Read BC Event Queue Descriptor */
    w[0] = BSWAP32( (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ in->bc_hid ].con1);
    w[1] = BSWAP32( (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ in->bc_hid ].addr1);
    w[2] = BSWAP32( (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ in->bc_hid ].con2);
    w[3] = BSWAP32( (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ in->bc_hid ].addr2);

    mode = (WORD) ( (w[0] & 0xc0000000) >> 30);
    out->mode = mode;

    if(mode == 0) /* 0 == API_DYTAG_STD_MODE*/
    {
        out->tag_fct[0]    = ((w[0] & 0x38000000) >> 27);
        out->min[0]        =  (w[1] & 0x0000FFFF);
        out->max[0]        = ((w[1] & 0xFFFF0000) >> 16);
        out->step[0]       =  (w[0] & 0x0000FFFF);
        out->wpos[0]       = ((w[0] & 0x001F0000) >> 16);

        out->tag_fct[1]    = ((w[2] & 0x38000000) >> 27);
        out->min[1]        =  (w[3] & 0x0000FFFF);
        out->max[1]        = ((w[3] & 0xFFFF0000) >> 16);
        out->step[1]       =  (w[2] & 0x0000FFFF);
        out->wpos[1]       = ((w[2] & 0x001F0000) >> 16);

        /* clear other entries */
        for( i=2; i<4; i++ )
        {
            out->tag_fct[i]    = 0;
            out->min[i]        = 0;
            out->max[i]        = 0;
            out->step[i]       = 0;
            out->wpos[i]       = 0;
        }
    }
    else
    {
        for( i=0; i<4; i++ )
        {
            opc  = ((w[i] & 0x30000000) >> 28);
            func = ((w[i] & 0x07000000) >> 24);
            wpos = ((w[i] & 0x001F0000) >> 16);
            fdw  =   w[i] & 0x0000ffff;

            out->tag_fct[i]    = func + 1+(opc << 7);
            out->min[i]        = fdw;
            out->max[i]        = 0;
            out->step[i]       = 0;
            out->wpos[i]       = wpos;
        }

    }

    return 0;

} /* end: api_bc_dytag_def_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_FRAME_INST         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Minor Frame Identifier [frame_id],                      */
/*                Amount of Transfers in Minor Frame [xfer_cnt],             */
/*                Sequence of Transfer Instructions [data]                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_FRAME_DEF' instruction to define a   */
/*    sequence of BC transfers identified by their Transfer Identifer within */
/*    a frame specified by the Frame Identifier.                             */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_frame_def(TY_API_DEV *p_api_dev, BYTE frame_id, BYTE xfer_cnt, TY_BC_INSTRUCTION *data)
{
    short  i, j;
    L_WORD instr, addr;
    WORD   op, dw, dw1;

    if( (frame_id == 0) || (frame_id > MAX_API_BC_FRAME) )
        return API_ERR_FRAME_ID_NOT_IN_RANGE;

    if( (xfer_cnt == 0) || (xfer_cnt > MAX_API_BC_XFER_IN_FRAME) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;




    /* Clear */
    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt = 0x0000;
    for(i = 0; i < MAX_API_BC_XFER_IN_FRAME; i++)
        p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = 0L;

    /* Copy framing info to p_api_dev->apiBcMinorFrameInfo */
    p_api_dev->apiBcMinorFrameInfo[ frame_id - 1 ].cnt = xfer_cnt;
    for(i = 0; i < xfer_cnt; i++)
    {
        p_api_dev->apiBcMinorFrameInfo[ frame_id - 1 ].instr[ i ] = data[i].instr;
        p_api_dev->apiBcMinorFrameInfo[ frame_id - 1 ].xid[ i ]   = data[i].xid;
    }

    /* Copy and check BC Transfer IDs */
    for(i = 0, j = 0; i < xfer_cnt; i++,j++)
    {

        op = (WORD)data[j].instr;
        dw = (WORD)data[j].xid;

        if( (op & 0xC000) == 0x0000)
        {

            switch(op)
            {
            case API_BC_INSTR_XFER_CODE:
                if( ul_StreamIs3910(p_api_dev, p_api_dev->biu) )
                {
                    /* check Board is 3910 */
                    /* source code of HS FrameDef removed and inserted here in order to reduce redundant code */
                    if( (dw > 0) && (dw < Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area) )
                    {

                        /* Pointer to BC Transfer Descriptor */
                        addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                            (dw * MAX_API_BC_XFER_DESC_SIZE);
                        addr = API_GLB_MEM_ADDR_REL(addr);

                        /* Opcode */
                        instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                        /* Write to temporary Instruction List */
                        p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                        p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;

                        if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == API39_BC_XFER_TYPE_HSRTRT) || ( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == API39_BC_XFER_TYPE_HSRTBR) ) )
                        {

                            i++;        /* increment index for instruction list */
                            xfer_cnt++; /* but increment also transfer counter because a HSRTRT or HSRTBR are two LS Transfers */

                            /* Pointer to BC Transfer Descriptor */
                            addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                                ((Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area + 1 + dw) * MAX_API_BC_XFER_DESC_SIZE);
                            addr = API_GLB_MEM_ADDR_REL(addr);

                            /* Opcode */
                            instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                            /* Write to temporary Instruction List */
                            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                        }
                    }
                    else
                    {
                        PRINTF2("ERROR: api_bc_frame_def dw 0..%d..%d\n\r", dw, Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area);
                        return API_ERR;
                    }
                }
                else
                {
                    if( ul_StreamIsEFEX(p_api_dev, p_api_dev->biu) )
                    {
                        /* check Board is EFEX */
                        /* source code of HS FrameDef removed and inserted here in order to reduce redundant code */
                        if( (dw > 0) && (dw < Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area) )
                        {
                            if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_XXRTRT) ||
                                ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_MCRT) ||
                                ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_MCBR) ||
                                ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_X_RTBR) )
                            {
                                /* Opcode */
                                instr = MAKE_INSTR(API_BC_INSTR_XFER_EFEX, dw);

                                /* Write to temporary Instruction List */
                                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                            }
                            else
                            {

                                /* Pointer to BC Transfer Descriptor */
                                addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                                    (dw * MAX_API_BC_XFER_DESC_SIZE);
                                addr = API_GLB_MEM_ADDR_REL(addr);

                                /* Opcode */
                                instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                                /* Write to temporary Instruction List */
                                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;

                                if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_EERTRT) ||
                                    ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_E_RTBR) ||
                                    ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_E_RTBRMIXED) )
                                {

                                    i++;        /* increment index for instruction list */
                                    xfer_cnt++; /* but increment also transfer counter because a HSRTRT or HSRTBR are two LS Transfers */

                                    /* Pointer to BC Transfer Descriptor */
                                    addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                                        ((Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area + 1 + dw) * MAX_API_BC_XFER_DESC_SIZE);
                                    addr = API_GLB_MEM_ADDR_REL(addr);

                                    /* Opcode */
                                    instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                                    /* Write to temporary Instruction List */
                                    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                                    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                                }
                            }
                        }
                        else
                        {
                            PRINTF2("ERROR: api_bc_frame_def dw 0..%d..%d\n\r", dw, Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area);
                            return API_ERR;
                        }
                    }
                    else
                    {
                        /* API1553 only */
                        if( (dw > 0) && (dw < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) )
                        {

                            /* Pointer to BC Transfer Descriptor */
                            addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                                (dw * MAX_API_BC_XFER_DESC_SIZE);
                            addr = API_GLB_MEM_ADDR_REL(addr);

                            /* Opcode */
                            instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                            /* Write to temporary Instruction List */
                            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                        }
                        else
                        {
                            PRINTF2("ERROR: api_bc_frame_def dw 0..%d..%d\n\r", dw, Api39_glb_mem.stream[p_api_dev->hs_stream].count.bc_xfer_area);
                            return API_ERR;
                        }
                    }
                }
                break;

            case API_BC_INSTR_SKIP_CODE:
                dw1 = dw & 0x8000;
                dw &= 0x00FF;
                if( (dw > 0) && (dw <= MAX_API_BC_SKIP_CNT) )
                {

                    /* Opcode */
                    instr = MAKE_INSTR(API_BC_INSTR_SKIP, dw);

                    /* Enable Interrupt */
                    if(dw1 != 0)
                        instr |= 0x02000000L;

                    /* Write to temporary Instruction List */
                    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                    p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                }
                else
                    return API_ERR;
                break;

            case API_BC_INSTR_WAIT_CODE:
                /* Opcode */
                instr = MAKE_INSTR(API_BC_INSTR_WAIT, dw);

                /* Write to temporary Instruction List */
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                break;

            case API_BC_INSTR_TSTR_CODE:
                /* Opcode */
                instr = MAKE_INSTR(API_BC_INSTR_STR, 0);

                /* Write to temporary Instruction List */
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                break;

            case API_BC_INSTR_RESMF_CODE:
                /* Opcode */
                instr = MAKE_INSTR(API_BC_INSTR_STR, 0);

                if(0 != dw)
                    instr |= (1 << 24);

                /* Write to temporary Instruction List */
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
                p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
                break;

            default:
                return API_ERR;
            }
        }
        else
        {
            instr = MAKE_W2LWORD(op, dw);

            /* Write to temporary Instruction List */
            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].cnt++;
            p_api_dev->bc_setup[ p_api_dev->biu ].frame[ frame_id - 1 ].instr[ i ] = instr;
        }
    }

    return 0;

} /* end: api_bc_frame_def */

L_WORD api_bc_frame_def_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT * in, TY_MIL_COM_ACK *out )
{
    return api_bc_frame_def( p_api_dev, (BYTE)in->frame_id, (BYTE)in->xfer_cnt, in->data );
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MFRAME_DEF         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Amount of Transfer Frames in the Major Frame [frame_cnt],  */
/*                Sequence of Frame Identifier [frame_ids]                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_MFRAME_INST' instruction to define   */
/*    a sequence of BC Transfer Frames identified by their Frame Identifer   */
/*    within the Major Frame.                                                */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_mframe_def_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT * in, TY_MIL_COM_ACK * out )
{
    if( in->frame_cnt > MAX_API_BC_FRAME_IN_MFRAME )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;

    return api_bc_mframe_def( p_api_dev, in->frame_cnt, in->frame_ids);
}


L_WORD api_bc_mframe_def(TY_API_DEV *p_api_dev, L_WORD frame_cnt, L_WORD *frame_ids)
{
    AiUInt32  i   = 0;
    BYTE      fid = 0;

    if( (frame_cnt == 0) || (frame_cnt > MAX_API_BC_FRAME_IN_MFRAME) )
        return API_ERR_FRAME_CNT_NOT_IN_RANGE;


    /* Clear */
    p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt = 0x0000;
    for(i = 0; i < MAX_API_BC_FRAME_IN_MFRAME; i++)
        p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ i ] = 0x00;


    /* Copy and check Transfer IDs */
    for(i = 0; i < frame_cnt; i++)
    {
        fid = (BYTE)frame_ids[i];
        if( (fid > 0) && (fid <= MAX_API_BC_FRAME) )
        {
            p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt++;
            p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ i ] = fid;
        }
        else
        {
            /* abort in case of any error */
            return API_ERR_FRAME_ID_NOT_IN_RANGE;
        }
    }

    return 0;

} /* end: api_bc_mframe_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MODE               */
/*                                                                           */
/*   Author : Rolf Heitzmann           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create :09.03.00   Update : 28.08.02                                    */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : API1553T BC Start Mode (Immed./Ext Trg./DBA) [smod],       */
/*                API1553T BC mode (Cyclic/n times execution) [cnt],         */
/*                API1553T BC minor frame time in usec [ftime]               */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the the BIU programming for 'API_BC_START'       */
/*    instruction.                                                           */
/*                                                                           */
/*****************************************************************************/

static BYTE api_bc_program_biu(TY_API_DEV *p_api_dev, BYTE smod, L_WORD cnt, L_WORD ftime, L_WORD *major_frame, L_WORD *minor_frames)
{
  L_WORD addr, offs, instr, tmpl, frame_cnt;
  short i, j, k, l, m, flag;
  WORD tmpw;
  BYTE fid;

  /* Init Globals */
  p_api_dev->bc_bh_def.bid = 0;
  p_api_dev->bc_bh_def.sid = 0;
  p_api_dev->bc_bh_def.eid = 0;
  p_api_dev->bc_bh_def.nbufs = 0;
  p_api_dev->bc_bh_def.hid_addr = 0L;
  p_api_dev->bc_bh_def.bid_addr = 0L;
  p_api_dev->bc_bh_def.sid_addr = 0L;
  p_api_dev->bc_bh_def.eid_addr = 0L;

  /* Init Globals */
  p_api_dev->bc_xfer_dsp.cw1 = 0x0000;
  p_api_dev->bc_xfer_dsp.st1 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cw2 = 0x0000;
  p_api_dev->bc_xfer_dsp.st2 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cur_bid = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_brw = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_buf = 0L;
  p_api_dev->bc_xfer_dsp.cur_ttag = 0L;
  p_api_dev->bc_xfer_dsp.msg_cnt = 0L;
  p_api_dev->bc_xfer_dsp.err_cnt = 0L;

  /* Clear Global BC Message and Error Counter */
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res1 = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res2 = 0L;
  api_io_bc_glb_cnt_ini(p_api_dev);


  /* Service Request and Vector Word handling */
  for(j = 0; j < MAX_API_BC_SRVW_RT; j++)
  {
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].xid = 0x0000;
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].lvw = 0x0000;
    p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].src = 0L;
  }

  p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt = 0;
  for(j = 0; j < MAX_API_BC_XFER_IN_FRAME; j++)
    p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ j ] = 0L;

  if(p_api_dev->bc_srvw_con[ p_api_dev->biu ] == API_ON)
  {
    /* Init first subtable instruction to RETURN */
    *(p_api_dev->bc_lip_instr_p[ p_api_dev->biu ] + 0) = BSWAP32(MAKE_INSTR(API_BC_INSTR_RET, 0) );

    /* Seach on multiple Transaction types */
    for(k = 0; k < MAX_API_BC_SRVW_RT; k++)
    {
      for(l = 0; l < MAX_API_BC_SRVW_SAMID; l++)
      {

        /* Put all TX Transactions in subtable */
        tmpw = p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_TX ].ta[ k ].sa_mid[ l ].xid;
        if(tmpw != 0)
        {
          p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_TX ].ta[ k ].sa_mid[ l ].xid &= 0x7FFF;
          p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_DELETE_TX ].ta[ k ].sa_mid[ l ].xid &= 0x7FFF;
          if(p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt < MAX_API_BC_XFER_IN_FRAME)
          {
            addr = (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                   ((tmpw & 0x7FFF) * MAX_API_BC_XFER_DESC_SIZE);
            instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);
            for(m = 0, flag = 0; m < p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt; m++)
            {
              if(p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ m ] == instr)
              {
                flag = 1;
                break;
              }
            }
            if(flag == 0)
            {
              p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt ] = instr;
              p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt++;
            }
          }
        }

        /* Put all RX Transactions in subtable */
        tmpw = p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_RX ].ta[ k ].sa_mid[ l ].xid;
        if(tmpw != 0)
        {
          p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_RX ].ta[ k ].sa_mid[ l ].xid &= 0x7FFF;
          p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_DELETE_RX ].ta[ k ].sa_mid[ l ].xid &= 0x7FFF;
          if(p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt < MAX_API_BC_XFER_IN_FRAME)
          {
            addr = (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                   ((tmpw & 0x7FFF) * MAX_API_BC_XFER_DESC_SIZE);
            instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);
            for(m = 0, flag = 0; m < p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt; m++)
            {
              if(p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ m ] == instr)
              {
                flag = 1;
                break;
              }
            }
            if(flag == 0)
            {
              p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt ] = instr;
              p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt++;
            }
          }
        }
      }
    }
  }

  /* Clear BC Transfer Message and Error Counter locations */
  for(i = 0; (unsigned)i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; i++)
    api_io_bc_xfer_desc_tc_ini(p_api_dev, (WORD)i);

  offs = 0L;

  /* Store Address of Stop Instruction */
  p_api_dev->bc_setup[ p_api_dev->biu ].sinst = 0L;

  /* Insert Wait for External Trigger Event instruction */
  if(smod == API_BC_XFER_RUN_ETR)
  {
    instr = MAKE_INSTR(API_BC_INSTR_WTRG, 0);
    *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ]) = BSWAP32(instr | 0x02); /* Sync Start */
    offs++;

    /* Insert a BC Branch Instruction that raises an interrupt, when BC is triggered */
    instr = MAKE_INSTR(API_BC_INSTR_SKIP, 0x02000001L);
    *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);

    /* Store address of BC Trigger Instr */
    p_api_dev->bc_setup[p_api_dev->biu].bc_triggered_inst = API_GLB_TSWPTR_TO_PBIREL((uintptr_t)(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs));
    
    offs++;
  }

  /* Clear return value for  Major Frame Start Address */
  *major_frame = API_GLB_TSWPTR_TO_GLOBREL(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ]);

  /* Pre-Check if amount of instructions fit in available BC Instr. List area */
  frame_cnt = 0L;
  for(i = 0; i < p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt; i++)
  {
    fid = p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ i ] - 1; /* Minor Frame ID */
    if(fid < MAX_API_BC_FRAME)
    {
      /* Count the Instructions */
      frame_cnt += p_api_dev->bc_setup[ p_api_dev->biu ].frame[ fid ].cnt;
      frame_cnt += 1; /* add instruction for WMFT */
      if(p_api_dev->bc_srvw_con[ p_api_dev->biu ] == API_ON)
        frame_cnt += 1;
       /* add instruction if SVR-REQUEST handling was enabled */
    }
    else
    {
      /* error */
      frame_cnt = 0xFFFFFFFFL;
      break;
    }
  }

  /* Check on available size */
  if(frame_cnt >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bc_hip_instr / 4)
  {
    *major_frame = 0xFFFFFFFFL; /* indicate ERRROR */
    return API_ERR;
  }


  /*** Major Frame ***/
  for(i = 0; i < p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt; i++)
  {

    /* Store Minor Frame address values */
    if(smod == API_BC_XFER_RUN_SETUP)
      minor_frames[ i ] = API_GLB_TSWPTR_TO_GLOBREL((ptrdiff_t) p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + (ptrdiff_t) offs);

    fid = p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ i ] - 1; /* Minor Frame ID */
    if(fid < MAX_API_BC_FRAME)
    {
      if(smod == API_BC_XFER_RUN_EXT)
      {
        instr = MAKE_INSTR(API_BC_INSTR_WTRG, 0);
        *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
        offs++;
      }

      /* Minor Frames */
      for(j = 0; j < p_api_dev->bc_setup[ p_api_dev->biu ].frame[ fid ].cnt; j++)
      {
        *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(p_api_dev->bc_setup[ p_api_dev->biu ].frame[ fid ].instr[ j ]);
        offs++;
      }

      /* Service Request and Vector Word handling */
      if(p_api_dev->bc_srvw_con[ p_api_dev->biu ] == API_ON)
      {
        /* Insert subtable CALL at the end of each minor frame */
        instr = MAKE_INSTR(API_BC_INSTR_CALL, 
                           (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_lip_instr)) );
        *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
        offs++;
      }

      /* Append Instructions to last Minor Frame */
      if(i == (p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt - 1) )
      {

        /* Decrement and Jump on Zero if non-cyclic */
        if(cnt != 0)
        {
          addr = (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr)) +
                 offs * 4L + 3 * 4L; /* Jump to Skip+Interrupt */
          instr = MAKE_INSTR(API_BC_INSTR_DJZ, addr);
          *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
          offs++;
        }

        /* Wait for Next Minor Frame Time slot */
        if(smod != API_BC_XFER_RUN_EXT)
        {
          instr = MAKE_INSTR(API_BC_INSTR_WMFT, 0);
          *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
          offs++;
        }

        /* Unconditional Jump */
        if(smod == API_BC_XFER_RUN_ETR)
        {
          /* Leave out 'wait for external trigger' and 'bc start interrupt' instruction = 8 Byte */
          addr = (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr)) + 8;
        }
        else
          addr = (API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr));
        instr = MAKE_INSTR(API_BC_INSTR_JMP, addr);
        *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
        offs++;

        /* Skip + Interrupt if non-cyclic */
        if(cnt != 0)
        {
          tmpl = 0x02000001L;
          instr = MAKE_INSTR(API_BC_INSTR_SKIP, tmpl);
          *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
          p_api_dev->bc_setup[ p_api_dev->biu ].sinst = offs * 4+ API_GLB_TSWPTR_TO_GLOBREL(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ]);
          offs++;
        }

        /* HALT */
        instr = MAKE_INSTR(API_BC_INSTR_HALT, 0);
        *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
        offs++;
      }
      else
      {
        if(smod != API_BC_XFER_RUN_EXT)
        {
          /* Wait for Next Minor Frame Time slot */
          instr = MAKE_INSTR(API_BC_INSTR_WMFT, 0);
          *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + offs) = BSWAP32(instr);
          offs++;
        }
      }
    }
  }

  p_api_dev->bc_setup[ p_api_dev->biu ].smod = smod;
  p_api_dev->bc_setup[ p_api_dev->biu ].cnt = cnt;
  p_api_dev->bc_setup[ p_api_dev->biu ].ftime = ftime;

  /* Major Frame Counter */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = BSWAP32(cnt);

  /* Minor Frame Time */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mft = BSWAP32(ftime);
  /* (*p_api_dev->cb_p[p_api_dev->biu]).mft *= 100; */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mft &= BSWAP32(0x000FFFFFUL);

  /* Set High Priority BC Instruction List Pointer */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = BSWAP32(API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr));
    
  /* Check on available size */
  if(offs >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bc_hip_instr / 4)
  {
    *major_frame = 0xFFFFFFFFL; /* indicate ERRROR */
    return API_ERR;
  }

  return API_OK;

} /* end: api_bc_program_biu */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_PROGRAM_BIU_TBL    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create :28.08.02   Update : 28.08.02                                    */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

static void api_bc_program_biu_tbl(TY_API_DEV *p_api_dev, BYTE smod, L_WORD saddr, L_WORD cnt, L_WORD ftime)
{
  short i;

  /* Init Globals */
  p_api_dev->bc_bh_def.bid = 0;
  p_api_dev->bc_bh_def.sid = 0;
  p_api_dev->bc_bh_def.eid = 0;
  p_api_dev->bc_bh_def.nbufs = 0;
  p_api_dev->bc_bh_def.hid_addr = 0L;
  p_api_dev->bc_bh_def.bid_addr = 0L;
  p_api_dev->bc_bh_def.sid_addr = 0L;
  p_api_dev->bc_bh_def.eid_addr = 0L;

  /* Init Globals */
  p_api_dev->bc_xfer_dsp.cw1 = 0x0000;
  p_api_dev->bc_xfer_dsp.st1 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cw2 = 0x0000;
  p_api_dev->bc_xfer_dsp.st2 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cur_bid = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_brw = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_buf = 0L;
  p_api_dev->bc_xfer_dsp.cur_ttag = 0L;
  p_api_dev->bc_xfer_dsp.msg_cnt = 0L;
  p_api_dev->bc_xfer_dsp.err_cnt = 0L;

  /* Clear Global BC Message and Error Counter */
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res1 = 0L;
  p_api_dev->bc_status_cnt[ p_api_dev->biu ].res2 = 0L;
  api_io_bc_glb_cnt_ini(p_api_dev);

  /* Clear BC Transfer Message and Error Counter locations */
  for(i = 0; (unsigned)i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; i++)
    api_io_bc_xfer_desc_tc_ini(p_api_dev, (WORD)i);

  /* Store Address of Stop Instruction */
  p_api_dev->bc_setup[ p_api_dev->biu ].sinst = 0L; /* tbd */

  p_api_dev->bc_setup[ p_api_dev->biu ].smod = smod;
  p_api_dev->bc_setup[ p_api_dev->biu ].cnt = cnt;
  p_api_dev->bc_setup[ p_api_dev->biu ].ftime = ftime;

  /* Major Frame Counter */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = BSWAP32(cnt);

  /* Minor Frame Time */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mft = BSWAP32(ftime);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mft &= BSWAP32(0x000FFFFFUL);

  /* Set High Priority BC Instruction List Pointer */
  if(saddr == 0L)
    (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr);
  else
    (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, saddr);
   /* via address value */

  wbflush();

} /* end: api_bc_program_biu_tbl */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                 Submodule : API_BC_PROGRAM_BIU_CONTINUE */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create :28.08.02   Update : 28.08.02                                    */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

static void api_bc_program_biu_continue(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD cnt, L_WORD ftime)
{
  L_WORD instr, addr, lval;

  /* Reset the Major Frame Counter for DJZ */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = BSWAP32(cnt);

  if(ftime != 0L)
  {
    /* we must use the following to resync the ftime for WMFTs */
    /* use the lip area since it is spare and not used by other function */
    instr = MAKE_INSTR(API_BC_INSTR_CMFT, ftime &0x000FFFFFL); /* resync MFT */
    *(p_api_dev->bc_lip_instr_p[ p_api_dev->biu ] + 0) = BSWAP32(instr);
    instr = MAKE_INSTR(API_BC_INSTR_JMP, BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, saddr))); /* then jump to specified address */
    *(p_api_dev->bc_lip_instr_p[ p_api_dev->biu ] + 1) = BSWAP32(instr);
    addr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_lip_instr);
  }
  else
  {
    /* we feed the hip directly with the specified address if ftime is zero */
    addr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, saddr);
  }

  wbflush();

  /* Set High Priority BC Instruction List Pointer to above sequence or directly */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = addr;
  wbflush();
  lval = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).hip);
  wbflush(); /* dummy read */

  /* Continue BC after Halt */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw &= BSWAP32(0x07FFFFFFUL);
  wbflush(); /* clear */ 
  lval = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  wbflush();
  lval &= 0x07FFFFFFL;
  lval |= 0x98000000L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(lval);
  wbflush(); /* continue */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw &= BSWAP32(0x07FFFFFFUL);
  wbflush(); /* clear */

} /* end: api_bc_program_biu_continue */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MODE               */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : API1553T BC control (START/HALT) [con],                    */
/*                API1553T BC Start Mode (Immed./Ext Trg./DBA) [smod],       */
/*                API1553T BC mode (Cyclic/n times execution) [cnt],         */
/*                API1553T BC minor frame time in usec [ftime]               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_START' and 'API_BC_HALT' instruction */
/*    to control the dynamic operations (START and STOP) of the BC.          */
/*                                                                           */
/*****************************************************************************/


static L_WORD api_bc_start(TY_API_DEV *p_api_dev, L_WORD smod, L_WORD cnt, L_WORD ftime, L_WORD saddr, L_WORD *major_frame, L_WORD *minor_frames )
{
    WORD   i = 0;
    L_WORD j = 0;

    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        api39_bc_start(p_api_dev, (BYTE)smod);

    if(p_api_dev->bc_status[ p_api_dev->biu ] != API_BUSY)
    {
        if(ftime < API_BC_XFER_RUN_MAX_FTIME)
        {
            switch(smod)
            {

            case API_BC_XFER_RUN_IMM:
                /* Start immediately */
                if(api_bc_program_biu(p_api_dev, (BYTE)smod, cnt, ftime, major_frame, minor_frames) == API_OK)
                {
                    p_api_dev->bc_status[ p_api_dev->biu ] = API_BUSY;
                    p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
                    api_io_mode_con(p_api_dev, API_BC_MODE, API_ON);
                }
                break;

            case API_BC_XFER_RUN_ETR:
                /* External Trigger */
                if(api_bc_program_biu(p_api_dev, (BYTE)smod, cnt, ftime, major_frame, minor_frames) == API_OK)
                {
                    p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_BUSY;
                    p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
                    api_io_mode_con(p_api_dev, API_BC_MODE, API_ON);
                }
                break;

            case API_BC_XFER_RUN_DBA:
                /* Dynamic Bus Control */
                if(api_bc_program_biu(p_api_dev, (BYTE)smod, cnt, ftime, major_frame, minor_frames) == API_OK)
                {
                    p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_BUSY;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;

                    UsbSynchronizeBC(p_api_dev, p_api_dev->biu, Out);
                }
                break;

            case API_BC_XFER_RUN_EXT:
                /* External Trigger */
                if(api_bc_program_biu(p_api_dev, (BYTE)smod, cnt, ftime, major_frame, minor_frames) == API_OK)
                {
                    p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                    p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
                    api_io_mode_con(p_api_dev, API_BC_MODE, API_ON);
                }
                break;

            case API_BC_XFER_RUN_SETUP:
                for(i = 0; i < (MAX_API_BC_FRAME_IN_MFRAME + 1); i++)
                    p_api_dev->lword_data_arr[ i ] = 0;
                api_bc_program_biu(p_api_dev, (BYTE)smod, cnt, ftime, major_frame, minor_frames);

                UsbSynchronizeBC(p_api_dev, p_api_dev->biu, Out);

                break;

            case API_BC_XFER_RUN_FAST_IMM:
                p_api_dev->bc_status[ p_api_dev->biu ] = API_BUSY;
                p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
                api_io_mode_con(p_api_dev, API_BC_MODE, API_ON);
                break;

            case API_BC_XFER_RUN_TBL:
                api_bc_program_biu_tbl(p_api_dev, (BYTE)smod, saddr, cnt, ftime);
                p_api_dev->bc_status[ p_api_dev->biu ] = API_BUSY;
                p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
                api_io_mode_con(p_api_dev, API_BC_MODE, API_ON);
                break;

            case API_BC_XFER_RUN_TBL_DBA:
                api_bc_program_biu_tbl(p_api_dev, (BYTE)smod, saddr, cnt, ftime);
                p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
                p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_BUSY;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;
                p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;

                UsbSynchronizeBC(p_api_dev, p_api_dev->biu, Out);

                break;
            default:
                return API_ERR_INVALID_MODE;
            }

            /* clear transfer related srvw counter */
            if( p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid )
            {
                for(j=0; j<p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; j++)
                {
                    p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].lvw = 0;
                    p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].xid = 0;
                    p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].src = 0;
                }
            }
        }
        else
            return API_NAK;
    }
    else
    {
        if(smod == API_BC_XFER_RUN_CONT)
        {
            /* BC must be already started when using this command */

            if(saddr != 0)
            {
                /* continue BC operation at specified address value */
                /* when BC stopped on HALT instruction */
                if(saddr >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr)
                {
                    api_bc_program_biu_continue(p_api_dev, saddr, cnt, ftime);
                }
            }
        }
    }

    return 0;

} /* end: api_bc_start */


static L_WORD api_bc_halt(TY_API_DEV *p_api_dev)
{
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        api39_bc_halt(p_api_dev);

    /* Halt BC operation */
    api_io_mode_con(p_api_dev, API_BC_MODE, API_OFF);
    p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_HALT;
    p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;
    p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
    p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
    p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;

    return 0;
} /* end: api_bc_halt */

L_WORD api_bc_mode(TY_API_DEV *p_api_dev, BYTE con, BYTE smod, L_WORD cnt, L_WORD ftime, L_WORD saddr)
{
    AiUInt32    major_frame_addr;
    AiUInt32    minor_frame_addr[64];

    if(con == API_BUSY)
        return api_bc_start( p_api_dev, smod, cnt, ftime, saddr, &major_frame_addr, minor_frame_addr);
    else
        return api_bc_halt( p_api_dev );
}

L_WORD api_bc_start_cmd( TY_API_DEV * p_api_dev, TY_MIL_COM_LS_BC_START_INPUT * in, TY_MIL_COM_LS_BC_START_OUTPUT * out )
{
    if (API_DEVICE_MODE_IS_DISABLE_BC(p_api_dev->drv_mode[p_api_dev->biu]))
        return API_ERR_BC_NOT_AVAILABLE;

    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if (api_opr_sf(p_api_dev, MIL_COM_LS_BC_START, 0, 0) != 0)
            return API_ERR_BC_NOT_AVAILABLE;

    return api_bc_start( p_api_dev, in->smod, in->cnt, in->frame_time_in_ms, in->saddr, &out->major_frame_addr, &out->minor_frame_addr[0] );
}

L_WORD api_bc_halt_cmd( TY_API_DEV * p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if (api_opr_sf(p_api_dev, MIL_COM_LS_BC_HALT, 0, 0) != 0)
            return API_ERR_BC_NOT_AVAILABLE;

    return api_bc_halt( p_api_dev );
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_STATUS_READ        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_STATUS_READ' instruction to read the */
/*    API1553-DS BC mode status and message/error counter information.       */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_status_read_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BC_STATUS_READ_OUTPUT * out )
{
    L_WORD retval = api_bc_status_read( p_api_dev );

    /* p_api_dev->bc_status_dsp and p_api_dev->bc_status_cnt are set within api_bc_status_read */
    out->status = p_api_dev->bc_status_dsp[ p_api_dev->biu ].status;
    out->hxfer  = p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer;
    out->bc_mc  = p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt;
    out->bc_ec  = p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt;
    out->hip    = p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip;
    out->mfc    = p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc;

    return retval;
}



L_WORD api_bc_status_read(TY_API_DEV *p_api_dev)
{
    L_WORD msg_cnt, err_cnt, hip, mfc;

    if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->bc_mc), In);
        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->bc_ec), In);
        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->mfc), In);
        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->hip), In);
    }

    /* BC Message Counter */
    msg_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_mc);

    /* BC Error Counter */
    err_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_ec);


    hip = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->cb_p[ p_api_dev->biu ]).hip );
    mfc = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc);

    p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt = msg_cnt;
    p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt = err_cnt;
    p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip = hip;
    p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc = mfc;
    p_api_dev->bc_status_cnt[ p_api_dev->biu ].res1 = 0L;
    p_api_dev->bc_status_cnt[ p_api_dev->biu ].res2 = 0L;

    return 0;

} /* end: api_bc_status_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_XFER_READ          */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 04.06.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer Identifier [xid],                              */
/*                Reset Buffer Status Bits after Command call [clr_mode]     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_XFER_READ' instruction to read the   */
/*    BC Transfer status information of the specified API1553-DS BC Transfer */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_xfer_read_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_READ_INPUT * in, TY_MIL_COM_LS_BC_XFER_READ_OUTPUT * out )
{
    L_WORD retval = api_bc_xfer_read( p_api_dev, (WORD)in->xid, (WORD)in->clr );

    out->bid         = p_api_dev->bc_xfer_dsp.cur_bid;
    out->brw         = p_api_dev->bc_xfer_dsp.cur_brw;
    out->bufp        = p_api_dev->bc_xfer_dsp.cur_buf;
    out->cw1         = p_api_dev->bc_xfer_dsp.cw1;
    out->cw2         = p_api_dev->bc_xfer_dsp.cw2;
    out->err_cnt     = p_api_dev->bc_xfer_dsp.err_cnt;
    out->lvw         = p_api_dev->bc_xfer_dsp.lvw;
    out->msg_cnt     = p_api_dev->bc_xfer_dsp.msg_cnt;
    out->srvreq_cnt  = p_api_dev->bc_xfer_dsp.srvreq_cnt;
    out->st1         = p_api_dev->bc_xfer_dsp.st1;
    out->st2         = p_api_dev->bc_xfer_dsp.st2;
    out->ttag        = p_api_dev->bc_xfer_dsp.cur_ttag;

    return retval;
}


L_WORD api_bc_xfer_read(TY_API_DEV *p_api_dev, WORD xid, WORD clr_mode)
{
    L_WORD hid, addr, tmpl, sqid;
    L_WORD ul_SqAddr;
    L_WORD ul_CurrentBufIndex = 0;
    L_WORD sq_mode; /* Status Queue Mode */
    L_WORD bq_mode; /* Buffer Queue Mode */
    L_WORD ul_qsize; /* Queue Size */

    if( (xid == 0) || (xid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) )
    {
        PRINTF2("ERROR: XID NOT IN RANGE (0..%d..%d)\n\r", xid, p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc);
        return API_ERR_INVALID_ID;
    }


    /* Calculate Buffer Header ID */
    addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr & BSWAP32(0x03ffffffUL) );
    addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area;
    hid = addr / MAX_API_BC_BH_DESC_SIZE;


    if(hid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area)
    {
        PRINTF2("ERROR: HID NOT IN RANGE (0..%d..%d)\n\r", hid, p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area);
        return API_ERR_INVALID_HID;
    }


    if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeTransferByID(p_api_dev, p_api_dev->biu, xid, FALSE, In);
    }

    /* BC Transfer Counter */
    p_api_dev->bc_xfer_dsp.msg_cnt = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].msg_cnt);

    /* BC Transfer Error Counter */
    p_api_dev->bc_xfer_dsp.err_cnt = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].err_cnt);


    /* Read from Buffer Header */
    tmpl = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl);

    addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
    addr -= p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
    addr /= MAX_API_BUF_SIZE;

    p_api_dev->bc_xfer_dsp.cur_bid = tmpl & 0xFFL; /* Current Index */
    p_api_dev->bc_xfer_dsp.cur_bid += (WORD) (addr);

    /* Read from Transfer Descriptor */
    tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[xid].cw2_cw1);
    p_api_dev->bc_xfer_dsp.cw1 = LO_WORD(tmpl);
    p_api_dev->bc_xfer_dsp.cw2 = HI_WORD(tmpl);

    /* Read from Status Queue Entry */
    tmpl = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl);
    ul_qsize = ((tmpl >> 14) & 0xF); /* Buffer Queue Size, multiples of 2 */
    if( 0 == ul_qsize )
        ul_qsize = 1;
    else
        ul_qsize = 1 << ul_qsize;
    sq_mode = (tmpl >> 12) & 0x3L; /* check if status queue enabled */
    bq_mode  = (tmpl >> 28) & 0x3L; /* Buffer Queue Mode */
    ul_SqAddr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);
    if (sq_mode == API_BC_BH_STS_QSIZE_SQ)
    {
        ul_CurrentBufIndex = BSWAP32((*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl) & 0x000000FF;

        /* Set Pointer to Status Entry */
        switch(bq_mode)
        {
        case API_BC_BH_BQM_CYCLIC:
        case API_BC_BH_BQM_ONCE:
            /* Set Pointer to last used Status Entry */
            if (ul_CurrentBufIndex == 0L) 
                ul_CurrentBufIndex = ul_qsize - 1; 
            else    
                ul_CurrentBufIndex--;
            break;    
        default:
        case API_BC_BH_BQM_HOST:
            /* Use Index as is */
            break;
        }        
        sqid = ((ul_SqAddr + ul_CurrentBufIndex*MAX_API_BC_SQ_DESC_SIZE) - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_sq_area) / MAX_API_BC_SQ_DESC_SIZE;
    }
    else /* buffer id = status queue id */
        sqid = (ul_SqAddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_sq_area) / MAX_API_BC_SQ_DESC_SIZE;

    p_api_dev->bc_xfer_dsp.st1 = LO_WORD(BSWAP32( (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].lsw2_lsw1) );
    p_api_dev->bc_xfer_dsp.st2 = HI_WORD(BSWAP32( (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].lsw2_lsw1) );
    p_api_dev->bc_xfer_dsp.cur_buf = BSWAP32( (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].act_buf_ptr);
    p_api_dev->bc_xfer_dsp.cur_ttag = BSWAP32( (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].time_tag);
    p_api_dev->bc_xfer_dsp.cur_brw = HI_WORD(BSWAP32( (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl) );

    /* Read SrvReq values */
    p_api_dev->bc_xfer_dsp.lvw        = 0;
    p_api_dev->bc_xfer_dsp.srvreq_cnt = 0;
    if(p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid)
    {
        p_api_dev->bc_xfer_dsp.lvw        = p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[ xid ].lvw;
        p_api_dev->bc_xfer_dsp.srvreq_cnt = p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[ xid ].src;
    }

    /* Set Buffer Status Bits to Buffer Empty state */
    if(clr_mode != API_DONT_MODIFY_STATUS_BITS)
    {
        BYTE bResetErr = 0;
        BYTE bResetStat = 0;
        BYTE bResetSrvReq = 0;

        if(API_RESET_STATUS_BITS == (clr_mode & API_RESET_STATUS_BITS) )
            bResetStat = 1;
        if(API_RESET_ERROR_BITS == (clr_mode & API_RESET_ERROR_BITS) )
            bResetErr = 1;
        if(API_RESET_SRVREQ_CNT == (clr_mode & API_RESET_SRVREQ_CNT) )
            bResetSrvReq = 1;

        if(bResetStat)
        {
            if( (p_api_dev->bc_xfer_dsp.cur_brw &0x1000) == 0x1000)
                /* clear Buffer Full Bit */
                (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xEFFFFFFFL);
            if( (p_api_dev->bc_xfer_dsp.cur_brw &0x2000) == 0x2000)
                /* clear Buffer Empty Bit */
                (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xDFFFFFFFL);
        }
        if(bResetErr)
        {
            if( (p_api_dev->bc_xfer_dsp.cur_brw &0xF) != 0)
                (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xFFF0FFFF);

        }
        if(bResetSrvReq)
        {
            if(p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid)
            {
                p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[ xid ].lvw = 0;
                p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[ xid ].src = 0;
            }
        }

        if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev,
                API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_sq_desc_p[p_api_dev->biu])[sqid].sq_ctrl), Out);
        }
    }

    return 0;

} /* end: api_bc_xfer_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_SRVW_REQ_READ      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt]                                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_srvw_req_read(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_SRVW_REQ_READ_INPUT * in, TY_MIL_COM_LS_BC_SRVW_REQ_READ_OUTPUT * out )
{
    if( in->rt >= MAX_API_BC_SRVW_RT)
        return API_ERR_INVALID_RT;

    out->xid = p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ in->rt ].xid;
    out->lvw = p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ in->rt ].lvw;
    out->src = p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ in->rt ].src;

    return 0;

} /* end: api_bc_srvw_req_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_SRVW_REQ_CON       */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 21.02.05   Update : 21.02.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : con [con]                                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_srvw_req_con(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_SRVW_REQ_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
    if( in->rt >= MAX_API_BC_SRVW_RT)
        return API_ERR_INVALID_RT;


    switch(in->con)
    {
    case API_SEND_SRVW_ON_SA31:
        (*p_api_dev->cb_p[p_api_dev->biu]).xf_instr[in->rt] = BSWAP32(0x4000019cL + 
                                                              BSWAP32( ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr) ));
        (*p_api_dev->cb_p[p_api_dev->biu]).xf_instr_rtrt[in->rt] = BSWAP32(0x4000029cL + 
                                                                   BSWAP32( ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr) ));
        break;
    case API_SEND_SRVW_ON_SA0:
    default:
        (*p_api_dev->cb_p[p_api_dev->biu]).xf_instr[in->rt] = BSWAP32(0x40000180L + 
                                                              BSWAP32( ul_ConvertTSWPtrToGRAMPtr( p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr) ));
        (*p_api_dev->cb_p[p_api_dev->biu]).xf_instr_rtrt[in->rt] = BSWAP32(0x40000280L + 
                                                                   BSWAP32( ul_ConvertTSWPtrToGRAMPtr( p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr) ));
        break;
    }

    return 0;

} /* end: api_bc_srvw_req_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_ACYC_MSG_PREP      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Reserved [res],                                            */
/*                Acyclic transfers in Minor Frame [xfer_cnt],               */
/*                Instruction Code and Data [instr]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_acyc_msg_prep(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_ACYC_PREP_INPUT * in, TY_MIL_COM_ACK *out )
{
  L_WORD i, input_index, flag;
  L_WORD instr, addr;
  WORD op, dw, dw1;
  TY_BC_INSTRUCTION *data = in->data;
  L_WORD xfer_cnt = in->xfer_cnt;

  if( (xfer_cnt == 0) || (xfer_cnt >= MAX_API_BC_ACYC_INSTR) )
      return API_ERR_FRAME_CNT_NOT_IN_RANGE;

    /* Clear BC Acyclic Instruction List */
    instr = MAKE_INSTR(API_BC_INSTR_HALT, 0);
    for(i = 0; i < MAX_API_BC_ACYC_INSTR; i++)
    {
      *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
    }

    /* Copy and check BC Transfer IDs */
    for(i = 0, input_index = 0, flag = API_OK; (i < xfer_cnt) && (flag == API_OK); i++, input_index++)
    {
        op = (WORD)data[input_index].instr;
        dw = (WORD)data[input_index].xid;

      if( (op &0xC000) == 0x0000)
      {

        switch(op)
        {
          case API_BC_INSTR_XFER_CODE:
            if( ul_StreamIs3910( p_api_dev, p_api_dev->biu))
            {
               /* check Board is 3910 */
              /* source code of HS FrameDef removed and inserted here in order to reduce redundant code */
              if( (dw > 0) && (dw < Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.bc_xfer_area) )
              {
                /* Pointer to BC Transfer Descriptor */
                addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                       (dw *MAX_API_BC_XFER_DESC_SIZE);
                addr = API_GLB_MEM_ADDR_REL(addr);

                /* Opcode */
                instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                /* Write to Instruction List */
                *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);

                if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == API39_BC_XFER_TYPE_HSRTRT) || ( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == API39_BC_XFER_TYPE_HSRTBR) ) )
                {

                  i++;        /* increment index for instruction list */
                  xfer_cnt++; /* but increment also transfer counter because a HSRTRT or HSRTBR are two LS Transfers */

                  /* Pointer to BC Transfer Descriptor */
                  addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                         ( (Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.bc_xfer_area + 1 + dw) * MAX_API_BC_XFER_DESC_SIZE);
                  addr = API_GLB_MEM_ADDR_REL(addr);

                  /* Opcode */
                  instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                  /* Write to Instruction List */
                  *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
                }
              }
              else
                flag = API_ERR_XID_NOT_IN_RANGE;

            }
            else
            if( ul_StreamIsEFEX( p_api_dev, p_api_dev->biu) )
            {
               /* check Board is EFEX */
              /* source code of HS FrameDef removed and inserted here in order to reduce redundant code */
              if( (dw > 0) && (dw < Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.bc_xfer_area) )
              {

                  if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_XXRTRT) ||
                      ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_MCRT) ||
                      ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_MCBR) ||
                      ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_X_RTBR) )
                  {
                    /* Opcode */
                    instr = MAKE_INSTR(API_BC_INSTR_XFER_EFEX, dw);

                    /* Write to Instruction List */
                    *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
                  }
                  else
                  {

                    /* Pointer to BC Transfer Descriptor */
                    addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                           (dw *MAX_API_BC_XFER_DESC_SIZE);
                    addr = API_GLB_MEM_ADDR_REL(addr);

                    /* Opcode */
                    instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                    /* Write to Instruction List */
                    *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);

                    if( ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type & API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_EERTRT) ||
                        ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_E_RTBR) ||
                        ( (x_api39_bc_xfer_desc[ p_api_dev->hs_stream ][ dw ].uw_type &API39_BC_XFER_TYPE_MASK) == APIEF_BC_XFER_TYPE_E_RTBRMIXED) )
                    {
                      i++;        /* increment index for instruction list */
                      xfer_cnt++; /* but increment also transfer counter because a HSRTRT or HSRTBR are two LS Transfers */

                      /* Pointer to BC Transfer Descriptor */
                      addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                             ( (Api39_glb_mem.stream[ p_api_dev->hs_stream ].count.bc_xfer_area + 1+dw) *MAX_API_BC_XFER_DESC_SIZE);
                      addr = API_GLB_MEM_ADDR_REL(addr);

                      /* Opcode */
                      instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);
                      /* Write to Instruction List */
                      *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);

                    }
                  }
                }
                else
                  flag = API_ERR_XID_NOT_IN_RANGE;

            }
            else
            {
              /* API1553 only */
              if( (dw > 0) && (dw < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) )
              {

                /* Pointer to BC Transfer Descriptor */
                addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_xfer_desc)) +
                       (dw *MAX_API_BC_XFER_DESC_SIZE);
                addr = API_GLB_MEM_ADDR_REL(addr);

                /* Opcode */
                instr = MAKE_INSTR(API_BC_INSTR_XFER, addr);

                /* Write to Instruction List */
                *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
              }
              else
                flag = API_ERR_XID_NOT_IN_RANGE;
            }
            break;

          case API_BC_INSTR_SKIP_CODE:
            dw1 = dw &0x8000;
            dw &= 0x00FF;
            if( (dw > 0) && (dw <= MAX_API_BC_SKIP_CNT) )
            {

              /* Opcode */
              instr = MAKE_INSTR(API_BC_INSTR_SKIP, dw);

              /* Enable Interrupt */
              if(dw1 != 0)
                instr |= 0x02000000L;

              /* Write to Instruction List */
              *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
            }
            else
              flag = API_ERR_XID_NOT_IN_RANGE;
            break;

          case API_BC_INSTR_WAIT_CODE:
            /* Opcode */
            instr = MAKE_INSTR(API_BC_INSTR_WAIT, dw);

            /* Write to temporary Instruction List */
            *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
            break;

          case API_BC_INSTR_TSTR_CODE:
             /* !%rh */
            /* Opcode */
            instr = MAKE_INSTR(API_BC_INSTR_STR, 0);

            /* Write to Instruction List */
            *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
            break;


          default:
            flag = API_ERR_FRAME_INSTR_NOT_IN_RANGE;
            break;
        }
      }
      else
      {
        instr = MAKE_W2LWORD(op, dw);

        /* Write to Instruction List */
        *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
      }
    }

    /* Append RET instruction */
    if(flag == API_OK)
    {

      /* Opcode */
      instr = MAKE_INSTR(API_BC_INSTR_RET, 0);

      /* Write to Instruction List */
      *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);

      p_api_dev->ul_XferCntForAcyclic = xfer_cnt;

      if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
      {
          UsbSynchronizeAcyclicInstructionList(p_api_dev, p_api_dev->biu, Out);
      }

    }


    return flag;

} /* end: api_bc_acyc_msg_prep */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_ACYC_MSG_SEND      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/


static L_WORD api_bc_acyc_msg_send_on_timetag(TY_API_DEV * p_api_dev, L_WORD timetag_high, L_WORD timetag_low) {
    L_WORD addr   = 0;

    /* Remove minute from ttlow as it was Reserved in old BSPs  */
    timetag_low &= ~(0x3F << 26);


    /* -- Send on ttag handled by TSW main loop --- */
    if (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYX)
    {
    	if( p_api_dev->bc_acyc_ttag_start[ p_api_dev->biu ].ena == 1L )
    		/* Acyc ttag transfer still in process. Try again later */
			return API_ERR_AGAIN;

    	p_api_dev->bc_acyc_ttag_start[ p_api_dev->biu ].ena = 1L;
        p_api_dev->bc_acyc_ttag_start[ p_api_dev->biu ].ttag_hi = timetag_high;
        p_api_dev->bc_acyc_ttag_start[ p_api_dev->biu ].ttag_lo = timetag_low;

        return 0;
    }

    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_ACYC_SEND_ON_TTAG))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    if (p_api_dev->ul_DeviceGroup == AI_DEVICE_USB)
    	/* @AIM_FIXME: TODO: APU support */
    	return API_ERR_PLATTFORM_NOT_SUPPORTED;

    /* -- Send on ttag handled by FW instruction --- */

	if( ((p_api_dev->cb_p[p_api_dev->biu]->ssw >> 18) & 0x1) == 0x1 )
		/* Acyc ttag transfer still in process. Try again later */
		return API_ERR_AGAIN;

	if( ((p_api_dev->cb_p[p_api_dev->biu]->ssw >> 20) & 0x1) == 0x1 )
		/* Replay not allowed, replay ttag register used for acyclic */
        return API_ERR_BC_NOT_AVAILABLE;

	p_api_dev->cb_p[p_api_dev->biu]->ramto = timetag_high;
	p_api_dev->cb_p[p_api_dev->biu]->ramio = timetag_low;

	addr = API_GLB_TSWPTR_TO_GLOBREL(p_api_dev->bc_acyc_instr_p[p_api_dev->biu]);

	*(p_api_dev->bc_acyc_instr_p[p_api_dev->biu] + (MAX_API_BC_ACYC_INSTR-1)) = MAKE_INSTR(API_BC_INSTR_ACYC_TTAG, addr);

	addr = API_GLB_TSWPTR_TO_GLOBREL((p_api_dev->bc_acyc_instr_p[p_api_dev->biu] + (MAX_API_BC_ACYC_INSTR-1)));

	p_api_dev->cb_p[p_api_dev->biu]->aip = addr;

    return 0;
}


static L_WORD api_bc_acyc_msg_send(TY_API_DEV *p_api_dev, L_WORD mode, L_WORD timetag_high, L_WORD timetag_low)
{
  L_WORD instr = 0;
  L_WORD ret = 0;

  /* Added Mode for Acyclic Message sent on Time Tag */
  switch(mode)
  {
    case API_BC_ACYC_RUN_IMM:
      instr = MAKE_INSTR(API_BC_INSTR_RET, 0); /* Opcode */
      *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + p_api_dev->ul_XferCntForAcyclic) = BSWAP32(instr); /* Write to Instruction List */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).aip = ul_ConvertTSWPtrToGRAMPtr( p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_acyc_instr);

      if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
      {
        UsbSynchronizeLWord(p_api_dev, 
                            API_GLB_MEM_ADDR_REL(p_api_dev->bc_acyc_instr_p[p_api_dev->biu] + p_api_dev->ul_XferCntForAcyclic),
                            Out);
        UsbSynchronizeLWord(p_api_dev, 
                            API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->aip), Out);
      }
      break;

    case API_BC_ACYC_SEND_TTAG:
        ret = api_bc_acyc_msg_send_on_timetag(p_api_dev, timetag_high, timetag_low);
      break;

    case API_BC_ACYC_RUN_AT_END_OF_FRAME:
      if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_LOW_PRIORITY_LIST))
          ret = API_ERR_CMD_NOT_SUPPORTED_BY_FW;
      else
      {
          instr = MAKE_INSTR(API_BC_INSTR_JMP, 0); /* Opcode */
          *(p_api_dev->bc_acyc_instr_p[p_api_dev->biu] + p_api_dev->ul_XferCntForAcyclic) = BSWAP32(instr); /* Write to Instruction List */
          (*p_api_dev->cb_p[p_api_dev->biu]).lip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_acyc_instr);

          if (p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
          {
              UsbSynchronizeLWord(p_api_dev,
                  API_GLB_MEM_ADDR_REL(p_api_dev->bc_acyc_instr_p[p_api_dev->biu] + p_api_dev->ul_XferCntForAcyclic),
                  Out);
              UsbSynchronizeLWord(p_api_dev,
                  API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->lip), Out);
          }
      }
      break;

    default:
      ret = API_ERR_INVALID_MODE;
  }

  return ret;
} /* end: api_bc_acyc_msg_send */



L_WORD api_bc_acyc_msg_send_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_ACYC_SEND_INPUT * in, TY_MIL_COM_ACK *out )
{
    return api_bc_acyc_msg_send( p_api_dev, in->mode, in->ttag_high, in->ttag_low );
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_ACYC_CHECK_TTAT    */
/*                                                                           */
/*   Author : Tony Ricker              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_bc_acyc_check_ttag(TY_API_DEV *p_api_dev)
{
  L_WORD ttHi = 0, ttLo = 0;
  BYTE tempBiu, thisBiu = 0;
  L_WORD t_day, t_hour, t_min, t_sec, t_usec, sth, stl;

  /* Check to see if time tag has been supassed */
  for(thisBiu = 0; thisBiu < p_api_dev->chns; thisBiu++)
  {
    if(p_api_dev->bc_acyc_ttag_start[ thisBiu ].ena)
    {
      mil_hw_irig_get_time(p_api_dev, &t_day, &t_hour, &t_min, &t_sec, &t_usec, &sth, &stl);

      ttHi = sth & 0x000FFFFF;
      ttLo = stl & 0x03FFFFFF;

      if(ttHi > p_api_dev->bc_acyc_ttag_start[ thisBiu ].ttag_hi)
      {
        tempBiu = (BYTE)p_api_dev->biu;
        p_api_dev->biu = thisBiu;
        p_api_dev->bc_acyc_ttag_start[ thisBiu ].ena = 0;
        api_bc_acyc_msg_send(p_api_dev, 0, 0, 0);
        p_api_dev->biu = tempBiu;
      }
      if( (ttHi == p_api_dev->bc_acyc_ttag_start[ thisBiu ].ttag_hi) && ttLo >= p_api_dev->bc_acyc_ttag_start[ thisBiu ].ttag_lo)
      {
        tempBiu = (BYTE)p_api_dev->biu;
        p_api_dev->biu = thisBiu;
        p_api_dev->bc_acyc_ttag_start[ thisBiu ].ena = 0;
        api_bc_acyc_msg_send(p_api_dev, 0, 0, 0);
        p_api_dev->biu = tempBiu;
      }
    }
  }
} /* end: api_bc_acyc_check_ttag */



L_WORD api_bc_acyc_prep_and_send_transfer(TY_API_DEV *p_api_dev, MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_INPUT * in, MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_OUTPUT * out)
{
    TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT bc_xfer_def_output;
    TY_MIL_COM_LS_BUF_DEF_OUTPUT buf_def_output;
    TY_MIL_COM_LS_BUF_READ_INPUT buf_read_input;
    TY_MIL_COM_LS_BC_ACYC_PREP_INPUT acyc_prep_input;
    TY_MIL_COM_LS_BC_XFER_READ_INPUT xfer_read_input;
    TY_MIL_COM_ACK ack;
    L_WORD ret = API_OK;
    L_WORD timeout = 200;
    L_WORD i = 0;

    // -- overwrite transfer descriptor --
    ret = api_bc_xfer_def_cmd(p_api_dev, &in->bc_xfer_def, &bc_xfer_def_output);

    if( ret != API_OK )
        return ret;

    // -- overwrite data buffer --
    ret = api_buf_def(p_api_dev, &in->buf_def, &buf_def_output);

    if (ret != API_OK)
        return ret;

    // -- prepare acyclic frame --
    acyc_prep_input.xfer_cnt = 1;
    acyc_prep_input.data[0].instr  = API_BC_INSTR_XFER_CODE;
    acyc_prep_input.data[0].xid    = in->bc_xfer_def.xid;
    ret = api_bc_acyc_msg_prep(p_api_dev, &acyc_prep_input, &ack);

    if (ret != API_OK)
        return ret;

    // -- clear transfer status so we can use it to check if the transfer has been sent --
    xfer_read_input.xid = in->bc_xfer_def.xid;
    xfer_read_input.clr = 0x7;
    api_bc_xfer_read_cmd(p_api_dev, &xfer_read_input, &out->bc_xfer_read);

    // -- send transfer --
    ret = api_bc_acyc_msg_send(p_api_dev, API_BC_ACYC_RUN_IMM, 0, 0);

    if (ret != API_OK)
        return ret;

    // -- wait for acyc transfer to be transmitted to be able to return transfer status and data (blocking) --
    ret = API_ERR_TIMEOUT;
    for (i=0; i<timeout; i++)
    {
        xfer_read_input.xid = in->bc_xfer_def.xid;
        xfer_read_input.clr = 0x0;
        api_bc_xfer_read_cmd(p_api_dev, &xfer_read_input, &out->bc_xfer_read);

        if ((out->bc_xfer_read.brw & 0x3<<12) != 0)
        {
            /* msg_cnt was set to 0 with api_bc_xfer_read_cmd */
            /* FW updates status infos after brw, so we wait until msg or err counter was updated*/
            if ((out->bc_xfer_read.msg_cnt + out->bc_xfer_read.err_cnt) > 0)
            {
                // buffer state changed transfer has been received
                ret = API_OK;
                break;
            }
        }
        else
        {
            // not yet received wait
            WAIT_CLOCKS(p_api_dev->clk_1us *10);
        }
    }

    if (ret != API_OK)
        return ret;


    // -- get buffer data --
    buf_read_input.bt = API_BC_BUF;
    buf_read_input.hid = in->bc_xfer_def.hid;
    buf_read_input.bid = 0;
    buf_read_input.count = 32;
    ret = api_buf_read(p_api_dev, &buf_read_input, &out->buf_read);

    if (ret != API_OK)
        return ret;


    return API_OK;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_BHD_READ           */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 19.11.04   Update : 04.06.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Buffer Header ID [hid]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_FIFO_ASSIGN' instruction to control  */
/*    the Dynamic Tagging within a BC Transfer.                              */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_bhd_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_READ_INPUT * in, TY_MIL_COM_LS_BC_BH_READ_OUTPUT * out )
{
    L_WORD tmpl = 0;

    if( (in->hid == 0) || (in->hid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area) )
        return API_ERR_HID_NOT_IN_RANGE;


    p_api_dev->bc_bh_def.hid_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area + (in->hid *MAX_API_BC_BH_DESC_SIZE);
    p_api_dev->bc_bh_def.bid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bp_addr );
    p_api_dev->bc_bh_def.sid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].sq_addr );
    p_api_dev->bc_bh_def.eid_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].eq_addr );
    tmpl = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl );
    p_api_dev->bc_bh_def.bid = (WORD)( p_api_dev->bc_bh_def.bid_addr - p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ] ) / MAX_API_BUF_SIZE;
    p_api_dev->bc_bh_def.sid = (WORD)( p_api_dev->bc_bh_def.sid_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_sq_area ) / MAX_API_BC_SQ_DESC_SIZE;
    p_api_dev->bc_bh_def.eid = (WORD)( p_api_dev->bc_bh_def.eid_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_eq_area ) / MAX_API_BC_EQ_DESC_SIZE;
    p_api_dev->bc_bh_def.nbufs = 0x1L << ((tmpl >> 14) & 0xF); /* Buffer Queue Size, multiples of 2 */

    out->bid       = p_api_dev->bc_bh_def.bid;
    out->sid       = p_api_dev->bc_bh_def.sid;
    out->eid       = p_api_dev->bc_bh_def.eid;
    out->nbufs     = p_api_dev->bc_bh_def.nbufs;
    out->hid_addr  = p_api_dev->bc_bh_def.hid_addr;
    out->bq_addr   = p_api_dev->bc_bh_def.bid_addr;
    out->sq_addr   = p_api_dev->bc_bh_def.sid_addr;
    out->eq_addr   = p_api_dev->bc_bh_def.eid_addr;


    return 0;

} /* end: api_bc_bhd_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_XFER_DEF_GET       */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_xfer_def_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_XFER_DEF_GET_INPUT * in, TY_MIL_COM_LS_BC_XFER_DEF_GET_OUTPUT * out)
{
    L_WORD xf_ctrl, bhd_ptr, cw2_cw1, img_swm, bid;
    BYTE wcnt, TrType;
    WORD cw_tx, cw_rx;


    xf_ctrl = (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ in->xid ].xf_ctrl;
    bhd_ptr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ in->xid ].bhd_ptr & BSWAP32(0x03ffffffUL) );
    cw2_cw1 = (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ in->xid ].cw2_cw1;
    img_swm = (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ in->xid ].img_swm;

    wcnt = (BYTE) (cw2_cw1 & 0x001f);
    bid = (WORD) ( (bhd_ptr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area) / MAX_API_BC_BH_DESC_SIZE);
    TrType = (BYTE) ( ( (xf_ctrl) &0x30000000) >> 28); /*transfer type*/
    switch(TrType)
    {
    case 0:
        cw_tx = 0;
        cw_rx = (WORD) (cw2_cw1 &0x0000ffff);
        break;
    case 1:
        cw_tx = (WORD) (cw2_cw1 &0x0000ffff);
        cw_rx = 0;
        break;
    case 2:
        cw_tx = (WORD) ((cw2_cw1 &0xffff0000) >> 16);
        cw_rx = (WORD) (cw2_cw1 &0x0000ffff);
        break;
    default:
        cw_tx = cw_rx = 0;
    }

    out->xfer_type  = TrType;
    out->chn        = ( ( (xf_ctrl) &0x02000000) >> 25); /*channel select*/
    out->xmt_rt     = (cw_tx &0xF800) >> 11; /* XMT RT */
    out->rcv_rt     = (cw_rx &0xF800) >> 11; /* RCV RT */
    out->xmt_sa     = (cw_tx &0x03e0) >> 5; /* XMT SA */
    out->rcv_sa     = (cw_rx &0x03e0) >> 5; /* RCV SA */
    out->wcnt       = wcnt; /* WCNT */
    out->xfer_ir    = (xf_ctrl &0x00700000) >> 20; /*interrupt control*/
    out->xfer_hlt   = (xf_ctrl &0x000E0000) >> 17; /*BC operation halt control*/

    out->rte        = (xf_ctrl &0x00800000) >> 23; /*retry enable bit*/
    out->res        = 0; /*reserved*/
    out->sxh        = (xf_ctrl &0x00018000) >> 15; /*status word exception handling control*/
    out->rsp        = (xf_ctrl &0x0C000000) >> 26; /*response control*/
    out->err_type   = p_api_dev->errorInjection[ in->xid ].type; /*err type */
    out->err_sync   = p_api_dev->errorInjection[ in->xid ].sync; /* err sync */
    out->err_contig = p_api_dev->errorInjection[ in->xid ].contig; /* err contig */
    out->err_padding = 0; /* err padding */
    out->gap_mode   = (img_swm &0xC0000000) >> 30; /* gap_mode */

    out->xid        = in->xid;
    out->hid        = bid;
    out->swxm       = img_swm & 0x00007fff; /* swxm */
    out->gap        = (img_swm &0x3fff0000) >> 16; /* gap */
    out->err_wpos   = p_api_dev->errorInjection[ in->xid ].wpos;
    out->err_bpos   = p_api_dev->errorInjection[ in->xid ].bpos;
    out->err_bc_bits = p_api_dev->errorInjection[ in->xid ].bc_bits;

    return 0;

} /* end: api_bc_xfer_def_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_XFER_BH_INFO_GET   */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
 
L_WORD api_bc_xfer_bh_info_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_BH_INFO_INPUT * in, TY_MIL_COM_LS_BC_BH_INFO_OUTPUT * out)
{
	out->buf_addr = ul_ConvertGRAMPtrToTSWPtr( p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ in->xid ].bhd_ptr & BSWAP32(0x03ffffffUL) );
	out->bid      = (out->buf_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area) / MAX_API_BC_BH_DESC_SIZE;

	return 0;

} /* end: api_bc_xfer_bh_info_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MAJOR_FRAME_GET    */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_major_frame_get( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BC_MAJOR_FRAME_GET_OUTPUT *out)
{
	BYTE cnt;
	WORD i;

	cnt = (BYTE)p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt;
	out->cnt     = cnt;

	for(i = 0; i < 64; i++)
		out->minor_frame[ i ] = p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ i ];

	if( (p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt > MAX_API_BC_FRAME_IN_MFRAME) )
		return API_NAK;

	return 0;

} /* end: api_bc_major_frame_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MINOR_FRAME_GET    */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_minor_frame_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MINOR_FRAME_GET_INPUT * in, TY_MIL_COM_LS_BC_MINOR_FRAME_GET_OUTPUT *out)
{
    AiUInt32 cnt = 0;
    AiUInt32 i   = 0;

    if( (in->frame_id==0) || (in->frame_id > MAX_API_BC_FRAME_IN_MFRAME_DEF))
        return API_ERR_FRAME_ID_NOT_IN_RANGE;


    cnt = p_api_dev->apiBcMinorFrameInfo[ in->frame_id - 1 ].cnt;
    out->cnt   = cnt;
    out->id    = in->frame_id;

    for(i=0; i < 128; i++)
    {
        out->instr[ i ] = p_api_dev->apiBcMinorFrameInfo[ in->frame_id - 1 ].instr[ i ];
        out->xid[ i ]   = p_api_dev->apiBcMinorFrameInfo[ in->frame_id - 1 ].xid[ i ];
    }

    if( (p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt > MAX_API_BC_FRAME_IN_MFRAME) )
        return API_NAK;

    return 0;

} /* end: api_bc_minor_frame_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_MODE_CTRL          */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.02.07   Update : -                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Mode [ul_BcMode]                                        */
/*                BC Mode Ctrl [ul_Ctrl]                                     */
/*                Param1 [ul_Param1]                                         */
/*                Param2 [ul_Param2]                                         */
/*                Param3 [ul_Param3]                                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_MODE_CTRL' instruction to control    */
/*    some BC modes on the fly.                                              */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_mode_ctrl( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MODE_CTRL_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ul_XfCtrl;
  L_WORD ul_XfCW;
  L_WORD ul_XfSWM;
  L_WORD ul_Hid;

  switch(in->mode)
  {
    case API_BC_MODE_INSERT_MC17_SYNC_CNT: /* Param1 = Xid */
      if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_MC17_SUPPORT))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

      /* read Transfer descriptor control word */
      ul_XfCtrl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[in->param1].xf_ctrl);
      LWORD_RESET_BITS(ul_XfCtrl, 0, 15);

      if (API_ON == in->ctrl)
        LWORD_SET_BITS(ul_XfCtrl, 0x05, 9, 6);
    
      /* write Transfer descriptor control word */
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[in->param1].xf_ctrl = BSWAP32(ul_XfCtrl);

      if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
      {
          UsbSynchronizeLWord(p_api_dev, 
              API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].xf_ctrl), 
              Out);
      }

      break;

    case API_BC_MODE_STOP_ON_MC0_DBCA_FLAG:
      /* read Transfer descriptor control word */
      ul_XfCtrl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].xf_ctrl);
      /* read Transfer descriptor command word */
      ul_XfCW   = BSWAP32( (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].cw2_cw1);
      /* read Transfer descriptor status word mask word */
      ul_XfSWM  = BSWAP32( (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].img_swm);

      /* check if transfer is a MC0 */
      if( ((ul_XfCW & 0x000003ff) == 0x000003E0) || ((ul_XfCW & 0x000003ff) == 0x00000000) )
      {
        if (API_ON == in->ctrl)
        {
          /* only modify bits again, */
          /* 1. if we are not already enabled */
          /* 2. if we are already enabled AND the bits have been changed since the last enabling */
          if( (API_HALT == p_api_dev->bc_dbc_stop_status[ p_api_dev->biu ]) ||
              ((API_BUSY == p_api_dev->bc_dbc_stop_status[ p_api_dev->biu ]) && ((ul_XfCtrl & 0x007E0000) != 0x00340000) ) )
          {
            p_api_dev->bc_dbc_stop_xfctrl_save[ p_api_dev->biu ] = ul_XfCtrl & 0x007E0000;
            p_api_dev->bc_dbc_stop_xfswm_save[ p_api_dev->biu ]  = ul_XfSWM & 0x000007FF;

            LWORD_RESET_BITS(ul_XfCtrl, 17, 6);
            ul_XfCtrl |= 0x00340000; /* set HLT=2 and INT=3 */
            ul_XfSWM  |= 0x00000002; /* set DBCA bit of status word exception mask */

            /* write back Transfer descriptor control word */
            (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].xf_ctrl = BSWAP32(ul_XfCtrl);
            /* write back Transfer descriptor status word mask word */
            (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].img_swm = BSWAP32(ul_XfSWM);

            p_api_dev->bc_dbc_stop_status[p_api_dev->biu] = API_BUSY;
          }
        }
        else
        {
          if( API_BUSY == p_api_dev->bc_dbc_stop_status[p_api_dev->biu] )
          {
            LWORD_RESET_BITS(ul_XfCtrl, 17, 6);
            ul_XfCtrl |= p_api_dev->bc_dbc_stop_xfctrl_save[p_api_dev->biu]; /* restore HLT and INT */
            p_api_dev->bc_dbc_stop_xfctrl_save[p_api_dev->biu] = 0;

            /* write back Transfer descriptor control word */
            (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].xf_ctrl = BSWAP32(ul_XfCtrl);

            LWORD_RESET_BITS(ul_XfSWM, 0, 11);
            ul_XfSWM  |= p_api_dev->bc_dbc_stop_xfswm_save[p_api_dev->biu]; /* restore status word exception mask */
            p_api_dev->bc_dbc_stop_xfswm_save[p_api_dev->biu] = 0;

            /* write back Transfer descriptor status word mask word */
            (*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].img_swm = BSWAP32(ul_XfSWM);

            p_api_dev->bc_dbc_stop_status[p_api_dev->biu] = API_HALT;
          }
        }

        if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev,
                API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].xf_ctrl), 
                Out);
            UsbSynchronizeLWord(p_api_dev, 
                API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[p_api_dev->biu])[in->param1].img_swm),
                Out);
        }
      }
      break;

    case API_BC_MODE_CONFIGURED_DYTAGS:
      if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_EVENT_QUEUE))
          return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

      for (ul_Hid = 0; ul_Hid<(MAX_API_BC_HID * MAX_MEM_MUL); ul_Hid++)
      {
        if( API_ON == p_api_dev->bc_dytag_status[p_api_dev->biu][ul_Hid] )
        {
          if (API_ON == in->ctrl) /* Enable Event Queue processing */
            api_io_bc_bh_eq_con(p_api_dev, API_ON, ul_Hid);
          else /* Disable Event Queue processing */
            api_io_bc_bh_eq_con(p_api_dev, API_OFF, ul_Hid);
        }
      }
      break;

    default:
        return API_ERR_INVALID_MODE;
  }

  return 0;

} /* end: api_bc_mode_ctrl */





L_WORD api_bc_modify_def(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BC_MODIFY_DEF_INPUT * in, TY_MIL_COM_LS_BC_MODIFY_DEF_OUTPUT * out)
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_MODIFY_INSTRUCTION))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    return api_io_bc_modify_def(p_api_dev, in->id, &in->modify, &out->offset);
}




L_WORD api_bc_traffic_override_control(TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE * in, TY_MIL_COM_ACK * out)
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_TRAFFIC_OVERRIDE))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    p_api_dev->bc_traffic_override_enabled[p_api_dev->biu] = (short)in->value;

    return API_OK;
}

L_WORD api_bc_traffic_override_get(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK_WITH_VALUE * out)
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_TRAFFIC_OVERRIDE))
        return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

    out->value = p_api_dev->bc_traffic_override_enabled[p_api_dev->biu];

    return API_OK;
}




//#endif

NAMESPACE1553_END

