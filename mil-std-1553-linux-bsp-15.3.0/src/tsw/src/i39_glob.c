/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i39_gpio.c
    Global variables of the 3910 ASP Driver Software.
*/

#include "mil_tsw_includes.h"



/***************************************/
/*** 3910 System structures  		  ***/
/***************************************/

TY_API39_TX_BC_AW_AREA *api39_tx_bc_aw_p[ API39_MAX_STREAM ];
TY_API39_SYS_CB *px_api39_cb_p[ API39_MAX_STREAM ];
TY_API39_ISLP *px_api39_islp_p[ API39_MAX_STREAM ];
L_WORD aul_api39_bc_xfer_type[ API39_MAX_STREAM ][MAX_API_BC_XFER * MAX_MEM_MUL];

/***************************************/
/*** 3910 Bus Controller structures  ***/
/***************************************/

struct ty_api39_bc_xfer_dsp x_api39_bc_hs_xfer_dsp[ API39_MAX_STREAM ];
struct ty_api39_bc_setup ax_api39_bc_setup[ API39_MAX_STREAM ];
struct ty_api39_bc_bh_def api39_bc_bh_def[ API39_MAX_STREAM ];
struct ty_api39_bc_xfer_desc_info x_api39_bc_xfer_desc[ API39_MAX_STREAM ][ MAX_API_BC_XFER *MAX_MEM_MUL ]; /* MAX_API_BC_XFER */
TY_API39_BC_BH_DESC *api39_bc_bh_desc_p[ API39_MAX_STREAM ];
TY_API39_BC_SQ_DESC *api39_bc_sq_desc_p[ API39_MAX_STREAM ];
TY_API39_BC_EQ_DESC *api39_bc_eq_desc_p[ API39_MAX_STREAM ];
TY_API39_BC_XFER_DESC *api39_bc_xfer_desc_p[ API39_MAX_STREAM ]; /* 3910 */

TY_APIEF_BC_XFER_DESC *apief_bc_xfer_desc_p[ API39_MAX_STREAM ]; /* EFEX */
TY_APIEF_BC_RESP_SIM_DESC *apief_bc_resp_sim_desc_p[ API39_MAX_STREAM ]; /* EFEX */
BYTE api39_bc_dytag_status[ API39_MAX_STREAM ][MAX_API39_BC_HID * MAX_MEM_MUL];


/****************************************/
/*** 3910 RT structures               ***/
/****************************************/

TY_API39_RT_DESC *api39_rt_desc_p[ API39_MAX_STREAM ];
TY_API39_RT_MID_DESC *api39_rt_mid_desc_p[ API39_MAX_STREAM ];
TY_APIEF_RT_MID_DESC *apief_rt_mid_desc_p[ API39_MAX_STREAM ]; /* EFEX */
TY_API39_RT_BH_DESC *api39_rt_bh_desc_p[ API39_MAX_STREAM ];
TY_API39_RT_SQ_DESC *api39_rt_sq_desc_p[ API39_MAX_STREAM ];
TY_API39_RT_EQ_DESC *api39_rt_eq_desc_p[ API39_MAX_STREAM ];

struct ty_api39_rt_setup ax_api39_rt_setup[ API39_MAX_STREAM ];


struct ty_api39_bh_cnt api39_bh_cnt[ API39_MAX_STREAM ][ MAX_API39_RT_HID *MAX_MEM_MUL ];
L_WORD api39_rt_dytag_status[ API39_MAX_STREAM ][MAX_API39_RT_HID * MAX_MEM_MUL];

/****************************************/
/*** 3910 BM structures               ***/
/****************************************/

TY_APIEF_BM_RT_ACT_FLT_PAGE *px_apief_bm_rt_act_flt_p[ API39_MAX_STREAM ]; /*EFEX*/
TY_API39_BM_RT_ACT_FLT_PAGE *px_api39_bm_rt_act_flt_p[ API39_MAX_STREAM ];

struct ty_api39_bm_rec x_api39_bm_rec[ API39_MAX_STREAM ];
struct ty_api39_bm_status_cnt x_api39_bm_status_cnt[ API39_MAX_STREAM ];
L_WORD aul_api39_buf_data[ API39_MAX_STREAM ][ MAX_API39_BM_FRAME_SIZE ];
BYTE uc_api39_bm_stack_dsp[ API39_MAX_STREAM ];


/****************************************/
/***  Memory structures               ***/
/****************************************/

TY_API39_GLOB_MEMORY Api39_glb_mem;

TY_API_BUF_MEMORY *api39_buf_mem_new[ API39_MAX_STREAM ];
struct ty_api39_sys_mem api39_sys_mem[ API39_MAX_STREAM ];
BYTE start_ptr_look[ API39_MAX_STREAM ];
L_WORD debug_mode;
WORD auw_api39_word_data_arr[ MAX_API39_WDATA_SIZE ];

/***************************************/
/* System Dynamic Data Words           */
/***************************************/
struct ty_api39_hs_systag_setup api39_hs_systag_setup[ API39_MAX_STREAM ];

/***************************************/
/* Dyntag Monitor definitions          */
/***************************************/

struct ty_api39_bm_hs_dyntagm_table api39_hs_dyntagm_lut[ API39_MAX_STREAM ]; /* 11.11.05 */
struct ty_api39_bm_hs_dyntagm_setup api39_hs_dyntagm_set[ API39_MAX_STREAM ]; /* 11.11.05 */

/***************************************/
/***      Replay structures          ***/
/***************************************/
struct ty_api39_sim_rep x_api39_sim_rep[ API39_MAX_STREAM ];
BYTE api39_rep_status[ API39_MAX_STREAM ];

/* rh Special modification for Rafale */
BYTE uc_HsSubAddress = API39_HSSA_DEF;


/* EOF */
