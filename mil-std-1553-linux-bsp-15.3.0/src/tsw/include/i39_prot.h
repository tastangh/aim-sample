/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i39_prot.h
 *
 *  Prototype information of the API3910  ASP Driver Software.
 *
 */

#ifndef __I39_PROT_H__
  #define __I39_PROT_H__

#define NAMESPACE1553_START 
#define NAMESPACE1553_END   
/*
* The following lines shall be uncommented, if namespaces shall be used.
* Please note, that namespaces do not work with plain C.
* Please make sure Avc_io.cpp is also adapted, if namespaces shall be used.

#define NAMESPACE1553_START namespace Mil1553{
#define NAMESPACE1553_END   }
*/

NAMESPACE1553_START



#include "AiMilCom.h"
/********* Prototypes / New Host to Target Communication **********************/

L_WORD api39_bc_xfer_def_err( TY_API_DEV *p_api_dev, TY_MIL_COM_HS_XFER_ERROR_INPUT * pxInput, TY_MIL_COM_ACK * pxOutput );





/* prototypes module api39_main */
void enter_drv3910_calc_mem_layout_size (TY_API_DEV *p_api_dev, TY_MEM_DEFINITION x_MemDef, L_WORD b_IncludeSimBuf, L_WORD *mem_used);
L_WORD enter_drv3910_check_mem_layout   (TY_API_DEV *p_api_dev, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD mem_used[2]);
void enter_drv_set_mem_layout_hs        (TY_API_DEV *p_api_dev, L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] );
void enter_drv3910                      (TY_API_DEV *p_api_dev);


/*****************************************************************************/
/***                  Genereral IO Functions                               ***/
/*****************************************************************************/

L_WORD api39_io_mode_con(TY_API_DEV *p_api_dev, BYTE mode, BYTE con);
void api39_io_cb_ini                  (TY_API_DEV *p_api_dev, short mode);
void api39_io_rx_tout                 (TY_API_DEV *p_api_dev, WORD uw_RxTout);
void api39_io_islp_ini                (TY_API_DEV *p_api_dev);
void api39_io_sim_buf_ini             (TY_API_DEV *p_api_dev);

/*****************************************************************************/
/***                   Memory Commands                                     ***/
/*****************************************************************************/

/* prototyped of api39_mem  */
void api39_mem_sys_init               (TY_API_DEV *p_api_dev);
void api39_mem_sys_set                (TY_API_DEV *p_api_dev, L_WORD addr);
void api39_mem_ini_ptr                (TY_API_DEV *p_api_dev);
L_WORD api39_mem_txrx_mem_alloc       (TY_API_DEV *p_api_dev, L_WORD size);
BYTE api39_mem_txrx_mem_free          (TY_API_DEV *p_api_dev, L_WORD size, L_WORD addr);


/*****************************************************************************/
/***                    Initialise Commands                                ***/
/*****************************************************************************/

/* prototypes module api_x_ini */
BYTE api_x_ini(                          TY_API_DEV *p_api_dev, BYTE uc_mode);

/* prototypes module i39_ini.c */
BYTE api39_ini_legacy(                   TY_API_DEV *p_api_dev, BYTE uc_mode);

L_WORD api39_ini(                        TY_API_DEV *p_api_dev, BYTE uc_mode);
void   api39_ini_bc_glb_vars(            TY_API_DEV *p_api_dev);
void   api39_ini_rt_glb_vars(            TY_API_DEV *p_api_dev);
void   api39_ini_hs_systag_glb_vars(     TY_API_DEV *p_api_dev);
void   api39_ini_rep_glb_vars(           TY_API_DEV *p_api_dev);
void   api39_ini_bm_hs_dyntagm_glb_vars( TY_API_DEV *p_api_dev);




/*****************************************************************************/
/***                     Selftest Commands                                 ***/
/*****************************************************************************/

/* i39_bite.c */
BYTE api39_bite_bc_verify             (TY_API_DEV *p_api_dev, BYTE api_biu, BYTE msg_cnt, BYTE err_cnt);
BYTE api39_bite_rt_verify             (TY_API_DEV *p_api_dev, BYTE api_biu, BYTE msg_cnt, BYTE err_cnt);
BYTE api39_bite_bc_trans_verify       (TY_API_DEV *p_api_dev, BYTE i, BYTE LSChn, BYTE HSChn, BYTE uc_mid_type, WORD exp_rep_w, BYTE msg_cnt, BYTE err_cnt);
BYTE api39_bite_rt_trans_verify       (TY_API_DEV *p_api_dev, BYTE i, BYTE LSChn, BYTE HSChn, BYTE uc_mid_type, WORD exp_rep_w, BYTE msg_cnt, BYTE err_cnt);
BYTE api39_bite_cpl_con               (TY_API_DEV *p_api_dev, BYTE mode);
BYTE api39_bite_bc_setup              (TY_API_DEV *p_api_dev, WORD xid, WORD hid, BYTE XferType, BYTE LSChn, BYTE xmt_rt, BYTE rcv_rt, BYTE xmt_mid, BYTE rcv_mid, BYTE HSChn, WORD HsXmtStartDel, BYTE HsXmtNoPre);
BYTE api39_bite_int_selftest          (TY_API_DEV *p_api_dev);
BYTE api39_bite_bc_br_test            (TY_API_DEV *p_api_dev, BYTE mode, BYTE LSChn, BYTE HSChn, BYTE con);
BYTE api39_bite_bc_rt_test            (TY_API_DEV *p_api_dev, BYTE mode, BYTE LSChn, BYTE HSChn, BYTE con);
BYTE api39_bite_rt_bc_test            (TY_API_DEV *p_api_dev, BYTE mode, BYTE LSChn, BYTE HSChn, BYTE con);
BYTE api39_bite_rt_rt_test            (TY_API_DEV *p_api_dev, BYTE mode, BYTE LSChn, BYTE HSChn, BYTE con);
BYTE api39_bite_hs_timer_test         (TY_API_DEV *p_api_dev, BYTE mode, BYTE LSChn, BYTE HSChn, BYTE con);
BYTE api39_bite_transfer_test         (TY_API_DEV *p_api_dev);
BYTE api39_bite_timing_test           (TY_API_DEV *p_api_dev);
WORD api39_bite                       (TY_API_DEV *p_api_dev, BYTE mode);
BYTE api39_bite_int_selftest          (TY_API_DEV *p_api_dev);


/*****************************************************************************/
/***                     System Commands                                   ***/
/*****************************************************************************/

/* prototypes module api_x_sys */
BYTE api_x_sys_biu_switch             (TY_API_DEV *p_api_dev, BYTE uc_LsHsId);
BYTE api_x_sys_board_type             (TY_API_DEV *p_api_dev);
L_WORD api_x_sys_read_board_config    (TY_API_DEV *p_api_dev);
BYTE api_x_sys_reset                  (TY_API_DEV *p_api_dev, BYTE uc_rc, BYTE uc_mbfs);
BYTE api_x_sys_read_sw_vers           (TY_API_DEV *p_api_dev);
BYTE api_x_sys                        (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);


/* Module API39_SYS */
BYTE api39_sys_read_sw_vers_legacy(      TY_API_DEV *p_api_dev);                        /* required for up/downgrade */
BYTE api39_sys_reset_legacy(             TY_API_DEV *p_api_dev, BYTE rc, BYTE uc_mbfs); /* required for up/downgrade */
BYTE api39_sys_get_mem_partition_legacy( TY_API_DEV *p_api_dev, BYTE uc_mode);          /* required for up/downgrade */

L_WORD api39_sys_foddl(                TY_API_DEV *p_api_dev, BYTE con);


L_WORD api39_sys_hs_rx_tout(           TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_RX_TIMEOUT_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_sys_hs_systag_def(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_SYSTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_sys_hs_systag_con(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_SYSTAG_CON_INPUT * in, TY_MIL_COM_ACK * out );

L_WORD api39_sys_track_def_cmd(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_TRACK_DEF_INPUT  * in, TY_MIL_COM_HS_SYS_TRACK_DEF_OUTPUT * out );
L_WORD api39_sys_track_read_cmd(       TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_TRACK_READ_INPUT * in, TY_MIL_COM_HS_SYS_TRACK_READ_OUTPUT * out );
L_WORD api39_sys_track_scan_cmd(       TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_TRACK_SCAN_INPUT * in, TY_MIL_COM_HS_SYS_TRACK_SCAN_OUTPUT * out );
L_WORD api39_sys_track_alloc_cmd(      TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_TRACK_ALLOC_INPUT* in, TY_MIL_COM_HS_SYS_TRACK_ALLOC_OUTPUT * out );


/* TSW internal */
void api_hs_sys_track_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

L_WORD api39_sys_reset(                TY_API_DEV *p_api_dev, BYTE rc, BYTE uc_mbfs);
L_WORD api39_sys_set_mem_partition(    TY_API_DEV *p_api_dev, L_WORD mode, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD * ret_status, L_WORD ret_mem_used[2] );
L_WORD api39_sys_get_mem_partition(    TY_API_DEV *p_api_dev, L_WORD mode, TY_API_GET_MEM_INFO * mem_info );
void api39_sys_ini_memory(             TY_API_DEV *p_api_dev, BYTE uc_mbfs);

void api_hs_sys_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/*****************************************************************************/
/***                     Buffer Commands                                   ***/
/*****************************************************************************/

/* prototypes of module api_x_buf.c  */
BYTE api_x_buf                        (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);

/* prototypes of module api39_buf.c  */
L_WORD api39_buf_write(               TY_API_DEV * p_api_dev, TY_MIL_COM_HS_BUF_WRITE_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_buf_write_block(TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BUF_WRITE_BLOCK_INPUT *in, TY_MIL_COM_ACK *out);
L_WORD api39_buf_host_control(TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BUF_HOST_CONTROL_INPUT *in, TY_MIL_COM_ACK *out);
L_WORD api39_buf_read_cmd(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BUF_READ_INPUT *in, TY_MIL_COM_HS_BUF_READ_OUTPUT * out );
L_WORD api39_buf_def_cmd(             TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BUF_DEF_INPUT * in, TY_MIL_COM_HS_BUF_DEF_OUTPUT * out );

/* TSW internal */
L_WORD api39_buf_def(                 TY_API_DEV *p_api_dev, BYTE uc_AllocMode, BYTE uc_BufType, WORD uw_HeaderId, WORD uw_BufferId, WORD uw_BufSize, WORD uw_WSizeIndex, WORD uw_WriteSize, WORD *data_p, AiUInt32 *puw_AllocBufNr, AiUInt32 *puw_AllocInfo, AiUInt32 *pul_BufAddr);
L_WORD api39_buf_read(                TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD uw_HeaderId, WORD uw_BufferId, WORD uw_RSizeIndex, WORD uw_BufSize, WORD *out_buffer, L_WORD * out_qsize, L_WORD *out_addr);
L_WORD api39_io_buf_set_mem_addr(     TY_API_DEV *p_api_dev, BYTE uc_BufType, L_WORD ul_AllocSize, WORD uw_size, WORD uw_BufferId, WORD uw_HeaderId, AiUInt32 *pul_BufAddr);
void api39_io_buf_fcpa_get            (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD uw_BufferId, WORD uw_HeaderId, WORD *uw_fcpa, WORD *uw_da, WORD *uw_size);
void api39_io_buf_fcpa_set            (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD uw_BufferId, WORD uw_HeaderId, WORD uw_fcpa, WORD uw_da, WORD uw_size);
void api39_io_buf_def_fcpa_field_set  (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD uw_BufferId, WORD uw_HeaderId, WORD *data_p);


void api_hs_buf_command_table(         TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/*****************************************************************************/
/***                     Bus Controller                                    ***/
/*****************************************************************************/

/* Prototypes module api_x_bc  */
BYTE api_x_bc_bhd_def_read            (TY_API_DEV *p_api_dev, WORD uw_hid);
L_WORD api39_bc_bhd_def_cmd(          TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_BH_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bc_bhd_def_read(         TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_BH_READ_INPUT * in, TY_MIL_COM_HS_BC_BH_READ_OUTPUT * out);
L_WORD api39_bc_xfer_def_cmd(         TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_XFER_DEF_INPUT * in, TY_MIL_COM_HS_BC_XFER_DEF_OUTPUT * out );
L_WORD api39_bc_xfer_def_get(         TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_XFER_DEF_GET_INPUT * in, TY_MIL_COM_HS_BC_XFER_DEF_GET_OUTPUT * out);
L_WORD api39_bc_dytag_def(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_DYTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api39_bc_dytag_def_get(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_DYTAG_GET_INPUT * in, TY_MIL_COM_HS_BC_DYTAG_GET_OUTPUT * out);
L_WORD api39_bc_xfer_read(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_XFER_READ_INPUT * in, TY_MIL_COM_HS_BC_XFER_READ_OUTPUT * out);
L_WORD api39_bc_status_read(          TY_API_DEV *p_api_dev, TY_MIL_COM *in, TY_MIL_COM_HS_BC_STATUS_READ_OUTPUT *out );
L_WORD api39_bc_mode_ctrl(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BC_MODE_CTRL_INPUT * in, TY_MIL_COM_ACK * out);




/* prototypes for api39_bc.c */
L_WORD api39_bc_ini(TY_API_DEV *p_api_dev);
L_WORD api39_bc_ini_xfer_dsp(TY_API_DEV *p_api_dev);
void api39_bc_ini_dytag               (TY_API_DEV *p_api_dev);
void api39_bc_ini_systag              (TY_API_DEV *p_api_dev); 
BYTE api39_bc_xfer_def_ex             (TY_API_DEV *p_api_dev, WORD uw_XferId, WORD uw_Hid1, BYTE uc_XferType, BYTE uc_XferLsIr, BYTE uc_XferHsIr, BYTE uc_XferHlt, BYTE uc_MsgSize, BYTE uc_LsChn, BYTE uc_XmtRt, BYTE uc_RcvRt, BYTE uc_XmtMid, BYTE uc_RcvMid, BYTE uc_HsChn, WORD uw_HsXmtStartDel, BYTE uc_HsXmtNoPre, BYTE uc_LsErrType, BYTE uc_LsErrWpos, BYTE uc_LsErrBpos, BYTE uc_LsErrSync, BYTE uc_LsErrContig, BYTE uc_LsErrBcBits, BYTE uc_HsErrType, BYTE uc_HsErrBpos, BYTE uc_HsErrBcBits, WORD uw_HsErrWpos, BYTE uc_GapMode1, BYTE uc_GapMode2, WORD uw_Gap1, WORD uw_Gap2, BYTE uc_Sxh, BYTE uc_Rte, L_WORD ul_Swxm1, L_WORD ul_Swxm2);
L_WORD api39_bc_xfer_ctrl               (TY_API_DEV *p_api_dev, WORD uw_xid, BYTE uc_mode);




L_WORD api39_bc_start(                TY_API_DEV *p_api_dev, BYTE uc_smod);
L_WORD api39_bc_halt(                 TY_API_DEV *p_api_dev);
L_WORD api39_bc_bhd_def(              TY_API_DEV *p_api_dev, WORD uw_HeaderId, WORD uw_Res, WORD uw_Sid, WORD uw_Eid, BYTE uc_Qsize, BYTE uc_Bqm, BYTE uc_Bsm, BYTE uc_Sts, BYTE uc_Evs, BYTE uc_Dbl, BYTE uc_Wcm);
L_WORD api39_bc_xfer_def(             TY_API_DEV *p_api_dev, WORD uw_XferId, WORD uw_Hid1, BYTE uc_XferType, BYTE uc_XferLsIr, BYTE uc_XferHsIr, BYTE uc_XferHlt, BYTE uc_MsgSize, BYTE uc_LsChn, BYTE uc_XmtRt, BYTE uc_RcvRt, BYTE uc_XmtMid, BYTE uc_RcvMid, BYTE uc_HsChn, WORD uw_HsXmtStartDel, BYTE uc_HsXmtNoPre, BYTE uc_LsErrType, BYTE uc_LsErrWpos, BYTE uc_LsErrBpos, BYTE uc_LsErrSync, BYTE uc_LsErrContig, BYTE uc_LsErrBcBits, BYTE uc_HsErrType, BYTE uc_HsErrBpos, BYTE uc_HsErrBcBits, WORD uw_HsErrWpos, BYTE uc_GapMode1, BYTE uc_GapMode2, WORD uw_Gap1, WORD uw_Gap2, BYTE uc_Sxh, BYTE uc_Rte, L_WORD ul_Swxm1, L_WORD ul_Swxm2);

void api_hs_bc_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );




/* prototypes for apief_bc.c */
L_WORD apief_bc_resp_sim_def(         TY_API_DEV *p_api_dev, TY_MIL_COM_EF_BC_RESP_SIM_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD apief_bc_internal_def(         TY_API_DEV *p_api_dev, TY_MIL_COM_EF_BC_INTERNAL_DEF_INPUT * in, TY_MIL_COM_ACK * out );

void api_ef_bc_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );


/* prototypes module api39_io.c */
void api39_io_bc_xfer_desc_tc_ini     (TY_API_DEV *p_api_dev, L_WORD xid);
void api39_io_bc_ena_lshs_link        (TY_API_DEV *p_api_dev, BYTE uc_XferType, WORD uw_XferIdLs, WORD uw_XferIdHs);
WORD api39_io_bc_header_bsize_get     (BYTE uc_BufType, WORD hid);
WORD api39_io_bc_header_bsize_set     (BYTE uc_BufType, WORD hid, WORD ul_BSize);
WORD api39_io_bc_header_qsize_get     (BYTE uc_BufType, WORD hid);
void api39_io_bc_glb_cnt_ini          (TY_API_DEV *p_api_dev);
void api39_io_bc_xfer_desc_ini        (TY_API_DEV *p_api_dev, WORD uw_XferId);
L_WORD api39_io_bc_ini(TY_API_DEV *p_api_dev);
void api39_io_bc_bh_bp_addr_ini       (TY_API_DEV *p_api_dev, WORD uw_index);
void api39_io_bc_bh_ini               (TY_API_DEV *p_api_dev, WORD uw_index);
void api39_io_bc_sq_ini               (TY_API_DEV *p_api_dev, WORD index);
void api39_io_bc_eq_ini               (TY_API_DEV *p_api_dev, WORD index);
BYTE api39_io_bc_xfer_inst            (TY_API_DEV *p_api_dev, WORD uw_XferId, WORD uw_Hid, BYTE uc_XferType, BYTE uc_XferIr, WORD uw_HsXmtStartDel, BYTE uc_HsXmtNoPre, BYTE uc_Type, BYTE res, BYTE uc_BitPos, BYTE uc_BitCnt, WORD uw_WPos);

BYTE api39_io_bc_xfer_aw_inst         (TY_API_DEV *p_api_dev, WORD uw_XferId, BYTE uc_XferType, BYTE uc_MsgSize, BYTE uc_AwType, BYTE uc_XmtMid, BYTE uc_RcvMid, BYTE uc_HsChn);

WORD api39_io_bc_bhd_def              (TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res, BYTE wcm);
L_WORD api39_io_header_field_get      (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD hid, WORD bpos); 
WORD api39_io_header_bsize_get        (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD hid);
WORD api39_io_header_bsize_set        (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD hid, WORD ul_BSize);
WORD api39_io_header_qsize_get        (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD hid);
void api39_io_bc_xfer_err_con         (TY_API_DEV *p_api_dev, WORD uw_XferId, BYTE uc_Type, BYTE uc_BitPos, BYTE uc_BitCnt, WORD uw_WPos);
BYTE api39_io_bh_free_alloc           (TY_API_DEV *p_api_dev, BYTE uc_BufType, WORD uw_index, BYTE uc_Mode);
BYTE api39_io_bc_xfer_dytag_con       (WORD uw_HeaderId, BYTE dmm, BYTE dmm1, BYTE dmm2, BYTE dmm3, BYTE mode, BYTE mode1, BYTE mode2, BYTE mode3, WORD fct_word, WORD wpos, WORD fct_word1, WORD wpos1, WORD fct_word2, WORD wpos2, WORD fct_word3, WORD wpos3);
void api39_io_bc_xfer_def_ext(TY_API_DEV *p_api_dev, BYTE uc_AwId, WORD uw_XferId, WORD uw_Hid, BYTE uc_XferType, BYTE uc_XmtRt, BYTE uc_RcvRt, BYTE uc_XmtSa, BYTE uc_RcvSa, BYTE uc_Wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, BYTE uc_Rsp, BYTE uc_Res,  /* 11.12.00 */
BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap); /* 11.12.00 */

void apief_io_bc_xfer_def_ext         (TY_API_DEV *p_api_dev, BYTE uc_AwId, WORD uw_XferId, WORD uw_Hid, BYTE uc_XferType, BYTE uc_XmtRt, BYTE uc_RcvRt, BYTE uc_XmtSa, BYTE uc_RcvSa, BYTE uc_Wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, BYTE uc_Rsp, BYTE uc_HsChn, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap, WORD gap2); /* 11.12.00 */

void api39_io_bc_bh_eq_con            (TY_API_DEV *p_api_dev, WORD con, L_WORD bc_hid);
void api39_io_bc_fw_dytag_dis         (TY_API_DEV *p_api_dev, WORD bc_hid);
void api39_io_bc_dytag_def            (TY_API_DEV *p_api_dev, WORD uw_BCHeaderId, BYTE dmm1, BYTE dmm2, BYTE dmm3, BYTE dmm4, BYTE mode1, BYTE mode2, BYTE mode3, BYTE mode4, WORD fct_word1, WORD wpos1, WORD fct_word2, WORD wpos2, WORD fct_word3, WORD wpos3, WORD fct_word4, WORD wpos4);
void api39_io_bc_xfer_ctrl            (TY_API_DEV *p_api_dev, WORD xid, BYTE mode);
void api39_io_bc_systag_free          (TY_API_DEV *p_api_dev, short mode, WORD xid);
void api39_io_bc_systag_add           (TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix);
void api39_io_bc_systag_sub           (TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix);
void api39_io_bc_systag_init_dw       (TY_API_DEV *p_api_dev, WORD xid);
void api39_io_bc_systag_con           (TY_API_DEV *p_api_dev, BYTE tag_ix, WORD xid);


/*******************************************************************************/
/*              Replay Commands  / i39_rep.c                                   */
/*******************************************************************************/

L_WORD api39_rep_ini(                  TY_API_DEV *p_api_dev, TY_MIL_COM_HS_REP_INI_INPUT    * in, TY_MIL_COM_ACK                  * out );
L_WORD api39_rep_status_read(          TY_API_DEV *p_api_dev, TY_MIL_COM                     * in, TY_MIL_COM_HS_REP_STATUS_OUTPUT * out );
L_WORD api39_rep_rt_con(               TY_API_DEV *p_api_dev, TY_MIL_COM_HS_REP_RT_CON_INPUT * in, TY_MIL_COM_ACK                  * out );

/* TSW internal */
void api_hs_rep_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

L_WORD api39_rep_start(                TY_API_DEV *p_api_dev);
L_WORD api39_rep_halt(                 TY_API_DEV *p_api_dev);


/* replay commands i39_io.c */

void api39_io_rep_rt_con              (TY_API_DEV *p_api_dev, BYTE uc_Con, BYTE uc_Mode, BYTE uc_Rt);
void api39_io_rep_buf_ini             (TY_API_DEV *p_api_dev);
void api39_io_rep_ini                 (TY_API_DEV *p_api_dev, BYTE uc_Nct, BYTE uc_Cet, BYTE uc_Rint, L_WORD ul_HsMsgCnt);


/* replay commands api_x_rep.c */

int api_x_rep_calc_tthigh            (L_WORD ttag_high);
int api_x_rep_calc_ttlow             (L_WORD ttag_low);
BYTE api_x_rep_get_ls_only_info       (TY_API_DEV *p_api_dev);
BYTE api_x_rep_mode                   (TY_API_DEV *p_api_dev, BYTE uc_Con);
BYTE api_x_rep                        (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);


/*****************************************************************************/
/***                     Remote Terminal                                   ***/
/*****************************************************************************/


/* module api39_rt.c */
L_WORD api39_rt_ini_start_delay(       TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_INI_START_DELAY_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_ini_preambles(         TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_INI_PREAMBLES_INPUT   * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_ini_lsw(               TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_INI_LSW_INPUT         * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_ini_law(               TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_INI_LAW_INPUT         * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_bhd_def(               TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_BH_DEF_INPUT       * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_bhd_def_read(          TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_BH_GET_INPUT       * in, TY_MIL_COM_HS_RT_BH_GET_OUTPUT * out );
L_WORD api39_rt_mid_def(               TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MID_DEF_INPUT      * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_mid_err_def(           TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MID_ERR_DEF_INPUT  * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_mid_err_get(           TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MID_ERR_GET_INPUT  * in, TY_MIL_COM_HS_RT_MID_ERR_GET_OUTPUT  * out );
L_WORD api39_rt_dytag_def(             TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_DYTAG_DEF_INPUT    * in, TY_MIL_COM_ACK * out );
L_WORD api39_rt_dytag_get(             TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_DYTAG_GET_INPUT    * in, TY_MIL_COM_HS_RT_DYTAG_GET_OUTPUT  * out );
L_WORD api39_rt_status_get(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_STATUS_GET_INPUT   * in, TY_MIL_COM_HS_RT_STATUS_GET_OUTPUT * out );
L_WORD api39_rt_mid_msg_get(           TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MSG_GET_INPUT      * in, TY_MIL_COM_HS_RT_MSG_GET_OUTPUT    * out );
L_WORD api39_rt_mid_bh_get(            TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MID_BH_GET_INPUT   * in, TY_MIL_COM_HS_RT_MID_BH_GET_OUTPUT * out );
L_WORD api39_rt_mode_control(          TY_API_DEV *p_api_dev, TY_MIL_COM_HS_RT_MODE_CONTROL_INPUT * in, TY_MIL_COM_ACK * out );


/* TSW  Internal RT functions */
L_WORD api39_rt_ini(                  TY_API_DEV *p_api_dev, BYTE uc_RtAddr, BYTE uc_Con);
L_WORD api39_rt_con(                  TY_API_DEV *p_api_dev, BYTE uc_RtAddr, BYTE uc_Con);
void api39_rt_ini_systag(             TY_API_DEV *p_api_dev);
void api39_rt_ini_dytag(              TY_API_DEV *p_api_dev); 
L_WORD api39_rt_sa_con(               TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm);
L_WORD api39_rt_mode(                 TY_API_DEV *p_api_dev, BYTE uc_con);

void api_hs_rt_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* module api39_io.c */

BYTE api39_io_rt_desc_ini_cnt         (TY_API_DEV *p_api_dev, BYTE RtAddr);
void api39_io_rt_desc_ini             (TY_API_DEV *p_api_dev, BYTE RtAddr);
void api39_io_rt_desc_ptr_ini         (TY_API_DEV *p_api_dev, BYTE RtAddr);
void apief_io_rt_desc_ptr_ini         (TY_API_DEV *p_api_dev, BYTE RtAddr); /* EFEX */
void api39_io_rt_sq_ini               (TY_API_DEV *p_api_dev, WORD index);
void api39_io_rt_eq_ini               (TY_API_DEV *p_api_dev, WORD index);
void api39_io_rt_glb_cnt_ini          (TY_API_DEV *p_api_dev);
void api39_io_rt_ini                  (TY_API_DEV *p_api_dev);
void api39_io_rt_con                  (TY_API_DEV *p_api_dev, BYTE RtAddr, BYTE con);
void api39_io_rt_ena_dis              (TY_API_DEV *p_api_dev, BYTE RtAddr, BYTE con);
void api39_io_rt_ena_dis_set_operation(TY_API_DEV *p_api_dev, BYTE RtAddr, BYTE con, BYTE new_mode);
void api39_io_rt_bh_ini               (TY_API_DEV *p_api_dev, WORD uw_index);
void api39_io_rt_bh_bp_addr_ini       (TY_API_DEV *p_api_dev, WORD uw_index);
WORD api39_io_rt_bhd_def              (TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res, BYTE wcm);
void api39_io_rt_mid_con              (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE mid_mc, WORD hid, BYTE mid_type, BYTE con, BYTE con_int);
void apief_io_rt_mid_con              (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE mid_mc, WORD hid, BYTE mid_type, BYTE con, BYTE con_int);
void api39_io_rt_xfer_err_con         (TY_API_DEV *p_api_dev, BYTE rt, BYTE mid, BYTE uc_Type, BYTE uc_BitPos, BYTE uc_BitCnt, WORD uw_WPos);
BYTE api39_io_rt_xfer_dytag_con       (WORD uw_HeaderId, BYTE dmm, BYTE dmm1, BYTE dmm2, BYTE dmm3, BYTE mode, BYTE mode1, BYTE mode2, BYTE mode3, WORD fct_word, WORD wpos, WORD fct_word1, WORD wpos1, WORD fct_word2, WORD wpos2, WORD fct_word3, WORD wpos3);
void api39_io_rt_sa_con               (TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm);
void api39_io_rt_check_deallocation   (TY_API_DEV *p_api_dev, WORD uw_Hid);

void api39_io_rt_bh_eq_con            (TY_API_DEV *p_api_dev, WORD con, L_WORD rt_hid);
void api39_io_rt_fw_dytag_dis         (TY_API_DEV *p_api_dev, WORD rt_hid);
void api39_io_rt_dytag_def            (TY_API_DEV *p_api_dev, WORD uw_RTHeaderId, BYTE dmm1, BYTE dmm2, BYTE dmm3, BYTE dmm4, BYTE mode1, BYTE mode2, BYTE mode3, BYTE mode4, WORD fct_word1, WORD wpos1, WORD fct_word2, WORD wpos2, WORD fct_word3, WORD wpos3, WORD fct_word4, WORD wpos4);

void api39_io_rt_systag_free          (TY_API_DEV *p_api_dev, short mode, BYTE rt, WORD mid);
void api39_io_rt_systag_add           (TY_API_DEV *p_api_dev, BYTE rt, WORD mid, BYTE tag_ix);
void api39_io_rt_systag_sub           (TY_API_DEV *p_api_dev, BYTE rt, WORD mid, BYTE tag_ix);
void api39_io_rt_mid_systag_init_dw   (TY_API_DEV *p_api_dev, BYTE rt, BYTE mid);
void api39_io_rt_mid_systag_con       (TY_API_DEV *p_api_dev, BYTE tag_ix, BYTE rt_addr, BYTE mid);

BYTE api39_io_hs_dytag_def            (TY_API_DEV *p_api_dev, BYTE bc_rt, WORD uw_HeaderId, BYTE dmm1, BYTE dmm2, BYTE dmm3, BYTE dmm4, BYTE mode1, BYTE mode2, BYTE mode3, BYTE mode4, WORD fct_word1, WORD wpos1, WORD fct_word2, WORD wpos2, WORD fct_word3, WORD wpos3, WORD fct_word4, WORD wpos4, L_WORD *w1, L_WORD *w2, L_WORD *w3, L_WORD *w4);
void api39_io_hs_systag_dis           (TY_API_DEV *p_api_dev, BYTE mode, WORD par1, WORD par2);
void api39_io_hs_systag_init_dw       (TY_API_DEV *p_api_dev, BYTE dytag_i, BYTE bc_rt_mode, WORD xid_rtmid, L_WORD dbp);
void api39_io_hs_systag_entry_init    (TY_API_DEV *p_api_dev, BYTE tag_ix);


/*****************************************************************************/
/***                    Calibration Functions                              ***/
/*****************************************************************************/

BYTE api_x_cal                        (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);


/* module api39_cal.c */
L_WORD api39_cal_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_CAL_CON_INPUT        * in, TY_MIL_COM_ACK * out );
L_WORD api39_cal_cpl_con_cmd(         TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_CAL_CPL_CON_INPUT    * in, TY_MIL_COM_ACK * out );
L_WORD api39_fop_con(                 TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_CAL_CON_INPUT        * in, TY_MIL_COM_ACK * out );
L_WORD api39_cpl_fofe_discret(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_SYS_FOFE_DISCRETES_INPUT * in, TY_MIL_COM_ACK * out );

/* TSW internal functions */
L_WORD api39_cpl_con(                  TY_API_DEV *p_api_dev, BYTE cpl );
void api_hs_sys_cal_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

/* module api39_io.c */
void api39_io_cal_ini                 (TY_API_DEV *p_api_dev);
void api39_io_cal_con                 (TY_API_DEV *p_api_dev, BYTE bus, BYTE con);
void api39_io_cpl_con                 (TY_API_DEV *p_api_dev, BYTE cpl);
void api39_io_fop_con                 (TY_API_DEV *p_api_dev, BYTE bus, BYTE amp);
void api39_io_fofe_discret_con        (TY_API_DEV *p_api_dev, BYTE fofe_discrit);


void api39_ir_rep                     (TY_API_DEV *p_api_dev, L_WORD w1, L_WORD w2);
void api39_ir_calculate_checksum      (TY_API_DEV *p_api_dev, WORD systag_id, L_WORD dbp );
void api39_ir_systag                  (TY_API_DEV *p_api_dev, BYTE bc_rt_mode, L_WORD xid_rtmid, L_WORD dbp);
void api39_ir_bc                      (TY_API_DEV *p_api_dev, L_WORD w1, L_WORD w2);
void api39_ir_rt                      (TY_API_DEV *p_api_dev, L_WORD w1, L_WORD w2);
void api39_ir_bm_rec                  (TY_API_DEV *p_api_dev, L_WORD ul_addr);
void api39_ir_bm                      (TY_API_DEV *p_api_dev, L_WORD w1, L_WORD w2);
void api39_ir                         (TY_API_DEV *p_api_dev);
void api39_ir_transp                  (TY_API_DEV *p_api_dev); 
  #ifndef _AMC1553
/* #error "muss noch fuer NUCLEUS definiert werden!!!" */
    #ifndef _NUCLEUS
void api39_isr                        (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api39_isr_transp                 (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc); 
void api39_pci0_isr                   (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
void api39_pci1_isr                   (L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc);
    #endif 

  #endif 

/*****************************************************************************/
/***                   Bus Monitor Functions                               ***/
/*****************************************************************************/

/* module i39_bm.c */
void api_hs_bm_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size );

L_WORD api39_bm_ini_intr(             TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_INI_INTR_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bm_ini_cap_cmd(          TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_INI_CAP_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bm_ini_flt(              TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_INI_FLT_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bm_status_read(          TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_HS_BM_STATUS_READ_OUTPUT * out );
L_WORD api39_bm_trg_con(              TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_TRG_CON_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bm_act_read(             TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_ACT_READ_INPUT * in, TY_MIL_COM_HS_BM_ACT_READ_OUTPUT * out);
L_WORD api39_bm_rt_act_read(          TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_RT_ACT_READ_INPUT * in, TY_MIL_COM_HS_BM_RT_ACT_READ_OUTPUT * out);
L_WORD api39_bm_rt_mid_act_read(      TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_RT_MID_ACT_READ_INPUT * in, TY_MIL_COM_HS_BM_RT_MID_ACT_READ_OUTPUT * out);
L_WORD api39_bm_stack_ptr_read(       TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_HS_BM_PTR_READ_OUTPUT * out );
L_WORD api39_bm_hs_stack_read(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_STACK_READ_INPUT * in, TY_MIL_COM_HS_BM_STACK_READ_OUTPUT * out );
L_WORD api39_bm_hs_stack_find(        TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_STACK_FIND_INPUT * in, TY_MIL_COM_HS_BM_STACK_FIND_OUTPUT * out );
L_WORD api39_bm_hs_rec_status_read(   TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_HS_BM_REC_STATUS_OUTPUT * out);
L_WORD api39_bm_hs_copy_rec_dat(      TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_COPY_REC_DAT_INPUT * in, TY_MIL_COM_ACK * out);
L_WORD api39_bm_hs_dyntag_mon_def(    TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_DYNTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out );
L_WORD api39_bm_hs_dyntag_mon_read(   TY_API_DEV *p_api_dev, TY_MIL_COM_HS_BM_DYNTAG_READ_INPUT * in, TY_MIL_COM_HS_BM_DYNTAG_READ_OUTPUT * out );


/* tsw internal */
L_WORD api_x_bm_mode(                 TY_API_DEV *p_api_dev, BYTE uc_Con);
L_WORD api39_bm_ini(                  TY_API_DEV *p_api_dev);
L_WORD api39_bm_mode(                 TY_API_DEV *p_api_dev, BYTE con);
L_WORD api39_bm_ini_cap(              TY_API_DEV *p_api_dev, BYTE uc_HsCapMode, L_WORD ul_Tat);
L_WORD apief_bm_rt_act_read(          TY_API_DEV *p_api_dev, BYTE uc_RtAddr, L_WORD *cc, L_WORD *ec, L_WORD *et);
L_WORD apief_bm_rt_mid_act_read(      TY_API_DEV *p_api_dev, BYTE uc_RtAddr, BYTE uc_Mid, BYTE uc_MidType, L_WORD *cc, L_WORD *ec, L_WORD *et );
L_WORD api39_bm_stack_ptr_read_ir(    TY_API_DEV *p_api_dev, BYTE biu, BYTE hs_stream, TY_MIL_COM_HS_BM_PTR_READ_OUTPUT * out );





/* module i_x_dque.c */
BYTE api_x_dque                       (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p);

/* module api39_io.c */
void api39_io_bm_cap_con              (TY_API_DEV *p_api_dev, BYTE uc_HsCapMode, L_WORD ul_Tat);
BYTE api39_io_bm_ini_msg_flt          (TY_API_DEV *p_api_dev, BYTE uc_RtAddr, L_WORD ul_MceRxMid1, L_WORD ul_MceRxMid2, L_WORD ul_MceRxMid3, L_WORD ul_MceRxMid4, L_WORD ul_MceTxMid1, L_WORD ul_MceTxMid2, L_WORD ul_MceTxMid3, L_WORD ul_MceTxMid4);
BYTE apief_io_bm_ini_msg_flt          (TY_API_DEV *p_api_dev, BYTE uc_RtAddr, L_WORD ul_MceRxMid1, L_WORD ul_MceRxMid2, L_WORD ul_MceRxMid3, L_WORD ul_MceRxMid4, L_WORD ul_MceRxMid5, L_WORD ul_MceRxMid6, L_WORD ul_MceRxMid7, L_WORD ul_MceRxMid8, L_WORD ul_MceTxMid1, L_WORD ul_MceTxMid2, L_WORD ul_MceTxMid3, L_WORD ul_MceTxMid4, L_WORD ul_MceTxMid5, L_WORD ul_MceTxMid6, L_WORD ul_MceTxMid7, L_WORD ul_MceTxMid8);
BYTE api39_io_bm_trg_con              (TY_API_DEV *p_api_dev, WORD uw_WordLocation, WORD uw_TrgCon, L_WORD ul_ErrTrgWord, L_WORD ul_MonDataWord, L_WORD ul_MonCWActWord, L_WORD ul_MonCWActMask, L_WORD ul_MonFcPaWord, L_WORD ul_MonFcPaMask);
void api39_io_bm_imod_set             (TY_API_DEV *p_api_dev, BYTE uc_Imod);
void api39_io_bm_smod_set             (TY_API_DEV *p_api_dev, BYTE uc_Smod);
void api39_io_bm_intr_con             (TY_API_DEV *p_api_dev, BYTE uc_Imod, BYTE uc_Smod);
void api39_io_bm_glb_cnt_ini          (TY_API_DEV *p_api_dev);
void api39_io_bm_rt_act_ini_cnt       (TY_API_DEV *p_api_dev, BYTE uc_RtAddr);
void apief_io_bm_rt_act_ini_cnt       (TY_API_DEV *p_api_dev, BYTE uc_RtAddr); /* EFEX */
void api39_io_bm_rt_act_ini           (TY_API_DEV *p_api_dev, BYTE uc_RtAddr);
void apief_io_bm_rt_act_ini           (TY_API_DEV *p_api_dev, BYTE uc_RtAddr); /* EFEX */
void api39_io_bm_buf_ini              (TY_API_DEV *p_api_dev);
void api39_io_bm_ini                  (TY_API_DEV *p_api_dev);
void api39_io_bm_ir_clr               (TY_API_DEV *p_api_dev);
BYTE api39_io_bm_ir_get               (TY_API_DEV *p_api_dev);
L_WORD api39_io_bm_read_mbfp_ir       (TY_API_DEV *p_api_dev, short biu, short hs_stream);
L_WORD api39_io_bm_read_bm_get_ir     (TY_API_DEV *p_api_dev, short hs_stream);
void api39_io_bm_set_bm_get_ir       (TY_API_DEV *p_api_dev, short hs_stream, L_WORD ul_bm_get);
void api39_io_bm_rt_act_ini_ls        (TY_API_DEV *p_api_dev, BYTE uc_RtAddr);


BYTE api39_io_gpio_read               (TY_API_DEV *p_api_dev, BYTE uc_comDat); 
void api39_io_gpio_write              (TY_API_DEV *p_api_dev, BYTE uc_comDat, BYTE uc_value);

void api39_io_bm_hs_dyntagm_entry_init     (TY_API_DEV *p_api_dev, L_WORD tag_ix);
void api39_io_bm_hs_dyntagm_entry_init_cnt (TY_API_DEV *p_api_dev, L_WORD tag_ix);
void api39_io_bm_hs_dyntagm_lut_free       (TY_API_DEV *p_api_dev, L_WORD mode, L_WORD tag_ix);
void api39_io_bm_hs_dyntagm_add            (TY_API_DEV *p_api_dev, L_WORD rt, L_WORD mid, L_WORD tag_ix);
void api39_io_bm_hs_dyntagm_sub            (TY_API_DEV *p_api_dev, L_WORD rt, L_WORD mid, L_WORD tag_ix);
void api39_io_bm_hs_dyntagm_con            (TY_API_DEV *p_api_dev, L_WORD tag_ix, L_WORD rt, L_WORD mid, L_WORD mid_type);

/* module i39_gpio.c */
BYTE api_gpio                         (TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p); 

#ifdef _NUCLEUS
void API39_HISR_Interrupt(void);
void API39_ISR2_Interrupt(INT vector_number);
void API39_ISR6_Interrupt(INT vector_number);
#endif



L_WORD ExecuteHsFirmwareCommand(TY_API_DEV *p_api_dev, short hs_stream, BYTE uc_FirmwareCmd);

NAMESPACE1553_END

#endif /* __I39_PROT_H__ */

/* end: i39_prot */
