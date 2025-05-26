/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i39_defv.h
 *
 *  Header information of the HS Driver Software.Definition of variables, arrays and structures, external references.  
 *
 */

#ifndef __I39_DEFV_H__
  #define __I39_DEFV_H__


  #include "api_defs.h"
  #include "api_defv.h"   

#ifdef __cplusplus
  extern "C" {
#endif


/***************************************/
/* API System Definition               */
/***************************************/

/* API System Control Block */
typedef struct
{
  volatile L_WORD ul_scw;      /* System Control Word */
  volatile L_WORD ul_ssw;      /* System Status Word */
  volatile L_WORD ul_cid;      /* Configuration Identification Register */
  volatile L_WORD ul_ilp;      /* Interrupt Loglist Pointer */
  volatile L_WORD ul_cor;      /* Coupling and Output Power Register */
  volatile L_WORD ul_fofe;     /* FOFE GPIO Data Register */
  volatile L_WORD ul_rt_type;  /* RT Type Register (EFEX) */
  volatile L_WORD ul_res1;/* Reserved */ 
  volatile L_WORD ul_bm_get;   /* BM get pointer */ 
  volatile L_WORD ul_lca_id;   /* LCA-Version */ 
  volatile L_WORD ul_hs_res;   /* Reserved */ 
  volatile L_WORD ul_lca_chks; /* LCA-Checksum */

  volatile L_WORD ul_bcbp;     /* BC Transfer Descriptor Base Pointer */
  volatile L_WORD ul_bcrsbp;   /* BC Response Simulator Descriptor Base Pointer */
  volatile L_WORD ul_build;    /* FW Build No */
  volatile L_WORD ul_git_hash; /* FW Git Hash */
  volatile L_WORD ul_res2[2];  /* Reserved */
  volatile L_WORD ul_inout;    /* 0x48 FW Command input/output register */
  volatile L_WORD ul_rt_mc;    /* RT Transfer Counter */
  volatile L_WORD ul_rt_ec;    /* RT Error Counter */
  volatile L_WORD ul_bc_mc;    /* BC Transfer Counter */
  volatile L_WORD ul_bc_ec;    /* BC Error Counter */
  volatile L_WORD ul_res3[ 5 ];/* Reserved */
  volatile L_WORD ul_rtlsaw;   /* RT-LS Command and Action Word */
  volatile L_WORD ul_rtlscon;  /* RT-LS Control */
  volatile L_WORD ul_bclsaw;   /* BC-LS Command and Action Word */
  volatile L_WORD ul_bclscon;  /* BC-LS Control / for EFEX BC - Transfer Index*/


  volatile L_WORD ul_mbba;     /* Monitor Buffer Base Address */
  volatile L_WORD ul_mbfp;     /* Monitor Buffer Fill Pointer */
  volatile L_WORD ul_msw;      /* Monitor Status and Trigger Control Word */
  volatile L_WORD ul_mstp;     /* Monitor Start Trigger Pointer */
  volatile L_WORD ul_mtat;     /* Monitor Trace after Trigger Size */
  volatile L_WORD ul_bm_et;    /* Monitor Error Trigger Register */
  volatile L_WORD ul_mdwt;     /* Monitor Data Word Trigger register */
  volatile L_WORD ul_mcawt;    /* Monitor Command and Action Word Trigger */
  volatile L_WORD ul_mcawtm;   /* Monitor Command and Action Word Trigger Mask */
  volatile L_WORD ul_mfhwt;    /* Monitor Frame Header Word Trigger Mask */
  volatile L_WORD ul_mfhtm;    /* Monitor Frame Header Trigger Mask */
  volatile L_WORD ul_bm_mc;    /* Monitor Transfer Counter */
  volatile L_WORD ul_bm_ec;    /* Monitor Error Counter */
  volatile L_WORD ul_bm_lsaw;  /* Monitor LS Command and Action Word */
  volatile L_WORD ul_bm_lsc;   /* Monitor LS Control  */
  volatile L_WORD ul_res4[ 1 ];/* Reserved */

  volatile L_WORD ul_rcw;      /* Replay Control Word */
  volatile L_WORD ul_rrew;     /* Replay RT Enable Word */
  volatile L_WORD ul_rbb;      /* Replay Buffer Base Pointer */
  volatile L_WORD ul_rbp;      /* Replay Buffer Pointer */
  volatile L_WORD ul_rec;      /* Replay Entry Count */
  volatile L_WORD ul_raec;     /* Replay Actual Entry Count */
  volatile L_WORD ul_ramto;    /* Replay Absolute Minute Offset Register */
  volatile L_WORD ul_ramio;    /* Replay Absolute Microsecond Offset Register */
  volatile L_WORD ul_res5[ 2 ];/* Reserved */
  volatile L_WORD bm_wcnt_bus_a;  /* Monitor Word Counter, bus A -> only valid for HS */
  volatile L_WORD bm_wcnt_bus_b;  /* Monitor Word Counter, bus B -> only valid for HS */
  volatile L_WORD bm_xfer_cnt_sec;/* Monitor Transfer Counter, secondary bus -> for HS bus A */
  volatile L_WORD bm_xfer_cnt_pri;/* Monitor Transfer Counter, primary bus -> for HS bus B */
  volatile L_WORD bm_err_cnt_sec; /* Monitor Errorr Counter, secondary bus -> for HS bus A */
  volatile L_WORD bm_err_cnt_pri; /* Monitor Errror Counter, primary bus -> for HS bus B */
} TY_API39_SYS_CB;
extern TY_API39_SYS_CB *px_api39_cb_p[ API39_MAX_STREAM ];


/* API Interrupt Status LogList */
struct ty_api39_islp
{
  volatile L_WORD ul_ir_status; /* Interrupt Status Word */
  volatile L_WORD ul_ir_ptr; /* Current Descriptor Pointer */
};
typedef struct ty_api39_islp TY_API39_ISLP[ MAX_API_ISLP_SIZE ];
extern TY_API39_ISLP *px_api39_islp_p[ API39_MAX_STREAM ];


/***************************************/
/*** 3910 Bus Controller structures  ***/
/***************************************/
  #ifdef CMPL4SIM

/* API BC Transfer Descriptor */
struct ty_api39_bc_xfer_desc
{
  volatile L_WORD ul_XferCtrl; /* Transfer Control Word 	    */
  volatile L_WORD ul_BhdPtr; /* Buffer Header Pointer 	    */
  volatile L_WORD ul_ErrSpec; /* Error Specification Word     */
  volatile L_WORD ul_XferCnt; /* Transfer Counter 		    */
  volatile L_WORD ul_ErrCnt; /* Error Counter 			    */
  volatile L_WORD ul_Rsv1; /* Reserved (used by Traget-SW /?) */
};
typedef struct ty_api39_bc_xfer_desc TY_API39_BC_XFER_DESC[ MAX_API39_BC_XFER *MAX_MEM_MUL ];

extern TY_API39_BC_XFER_DESC *api39_bc_xfer_desc_p[ API39_MAX_STREAM ];

/* EFEX */
struct ty_apief_bc_xfer_desc
{
  volatile L_WORD ul_XferCtrl; /* Transfer Control Word 	    */
  volatile L_WORD ul_XferStatus; /* Transfer Status Word 	    */
  volatile L_WORD ul_ErrSpec; /* Error Specification Word     */
  volatile L_WORD ul_XferCnt; /* Transfer Counter 		    */
  volatile L_WORD ul_ErrCnt; /* Error Counter 			    */
  volatile L_WORD ul_FrameFcPaDA; /* CC/MC FCPA / CC/MC DA */
  volatile L_WORD ul_FrameWordCnt; /* res / Word Count */
  volatile L_WORD ul_SFrameMsgTagW; /*  Message Tag Word (S-Frame)   */
  volatile L_WORD ul_SFrameFcPaDA; /* HS - Status ADW Word          | HS Message Tag Word (S-Frame)   */
  volatile L_WORD ul_SDFrameWcwMsgW; /* SD Frame Word Count Word | HS Message Tag Word (SD-Frame)   */
  volatile L_WORD ul_SDFrameFcPaDA; /*  SD Frame Control / Physical Address Word / SD Frame Destination Address Word*/
  volatile L_WORD ul_TimeTag; /* Time Tag Word */
  volatile L_WORD ul_SDFrameSEMSFameSEM; /* SD - Frame Status Exception Mask | S - Frame Status Exception Word Mask */
  volatile L_WORD ul_XferWaitTime; /* res | BC Transfer Wait Time */
  volatile L_WORD ul_BCIntFcPaDA; /* BC Internal */
  volatile L_WORD ul_BCIntMCWords; /* BC Internal */
};
typedef struct ty_apief_bc_xfer_desc TY_APIEF_BC_XFER_DESC[ MAX_API39_BC_XFER *MAX_MEM_MUL ];

extern TY_APIEF_BC_XFER_DESC *apief_bc_xfer_desc_p[ API39_MAX_STREAM ];

struct ty_apief_bc_resp_sim_desc
{
  volatile L_WORD ul_SDFrameRespSimCtrl; /* SD Response Simulator Control Word 	    */
  volatile L_WORD ul_SDFrameFcPaDA; /* SD Frame FCPA / DA Word 	    */
  volatile L_WORD ul_SDFrameWordCnt; /* SD Frame Word Count    */
  volatile L_WORD ul_SDFrameDataBuf; /* SD Frame Data Buffer Pointer 		    */
  volatile L_WORD ul_SFrameRespSimCtrl; /* SD Response Simulator Control Word 	    */
  volatile L_WORD ul_SFrameFcPaADW; /* SD Frame FCPA / ADW Word 	    */
  volatile L_WORD ul_Res[2]; /* reserved    */
};
typedef struct ty_apief_bc_resp_sim_desc TY_APIEF_BC_RESP_SIM_DESC[ MAX_API39_BC_XFER *MAX_MEM_MUL ];

extern TY_APIEF_BC_RESP_SIM_DESC *apief_bc_resp_sim_desc_p[ API39_MAX_STREAM ];

/* API BC Buffer Header */
struct ty_api39_bc_bh_desc
{
  volatile L_WORD ul_BhCtrl; /* Buffer Header Control Word */
  volatile L_WORD ul_SqAddr; /* Status Queue Base Pointer */
  volatile L_WORD ul_EqAddr; /* Event Queue Base Pointer */
  volatile L_WORD ul_BpAddr; /* Data Buffer Queue Base Pointer */
};
typedef struct ty_api39_bc_bh_desc TY_API39_BC_BH_DESC[ MAX_API_BC_HID *MAX_MEM_MUL ];
extern TY_API39_BC_BH_DESC *api39_bc_bh_desc_p[ API39_MAX_STREAM ];

/* API BC Status Queue Entry */
struct ty_api39_bc_sq_desc
{
  volatile L_WORD ul_SqCtrl; /* Status Queue Control Word */
  volatile L_WORD ul_LcwLsw; /* LS Command Word and LS Action Word */
  volatile L_WORD ul_ActBufPtr; /* Actual Data Buffer Pointer */
  volatile L_WORD ul_FcPaDestAddr; /* Frame Control/Physical Address and Destination Address */
  volatile L_WORD ul_WcwHsmsgw; /* Word Count Word and HS Message Tag Word */
  volatile L_WORD ul_TimeTag; /* Time Tag Word */
};
typedef struct ty_api39_bc_sq_desc TY_API39_BC_SQ_DESC[ MAX_API_BC_SQ ];
extern TY_API39_BC_SQ_DESC *api39_bc_sq_desc_p[ API39_MAX_STREAM ];

/* API BC Event Queue Entry */
struct ty_api39_bc_eq_desc
{
  volatile L_WORD ul_Dyn1; /* Event Queue Control Word 1 for dynamic data */
  volatile L_WORD ul_Dyn2; /* Event Queue Control Word 2 for dynamic data */
  volatile L_WORD ul_Dyn3; /* Event Queue Control Word 3 for dynamic data */
  volatile L_WORD ul_Dyn4; /* Event Queue Control Word 4 for dynamic data */
};
typedef struct ty_api39_bc_eq_desc TY_API39_BC_EQ_DESC[ MAX_API_BC_EQ ];
extern TY_API39_BC_EQ_DESC *api39_bc_eq_desc_p[ API39_MAX_STREAM ];

struct ty_api39_tx_bc_aw
{
  volatile L_WORD ul_rsv_aw; /* Action Word for Low Speed set up 2 * 256 required */
};
typedef struct ty_api39_tx_bc_aw TY_API39_TX_BC_AW_AREA[ MAX_API_BC_XFER * MAX_MEM_MUL ];
extern TY_API39_TX_BC_AW_AREA *api39_tx_bc_aw_p[ API39_MAX_STREAM ];
extern L_WORD aul_api39_bc_xfer_type[ API39_MAX_STREAM ][ MAX_API_BC_XFER * MAX_MEM_MUL ];


/**********************************************************************************/
/*** 						Remote Terminal              					    ***/
/**********************************************************************************/

/* API3910 RT Descriptor */
struct ty_api39_rt_desc
{
  volatile L_WORD ul_RtCtrl;
  volatile L_WORD ul_MidDesc;
  volatile L_WORD ul_RtMsgCnt;
  volatile L_WORD ul_RtErrCnt;
  volatile L_WORD ul_Res[ 12 ];
  volatile L_WORD ul_LawLsw;
  volatile L_WORD ul_Res1[ 15 ];
};
typedef struct ty_api39_rt_desc TY_API39_RT_DESC[ MAX_API39_RT ];
extern TY_API39_RT_DESC *api39_rt_desc_p[ API39_MAX_STREAM ];


/* API3910 RT MID + MC Descriptor */
struct ty_api39_rt_mid_desc
{
  volatile L_WORD ul_MidCtrl;
  volatile L_WORD ul_BhdPtr;
};
struct ty_api39_rt_mid_desc_ary
{
  struct ty_api39_rt_mid_desc mid_mc[ MAX_API39_RT_RCV_MID + MAX_API39_RT_XMT_MID ];
  struct ty_api39_rt_mid_desc mc[ MAX_API39_RT_MC ];
};
typedef struct ty_api39_rt_mid_desc_ary TY_API39_RT_MID_DESC[ MAX_API39_RT ];
extern TY_API39_RT_MID_DESC *api39_rt_mid_desc_p[ API39_MAX_STREAM ];

struct ty_apief_rt_mid_desc_ary
{
  struct ty_api39_rt_mid_desc mid_mc[ MAX_APIEF_RT_RCV_MID + MAX_APIEF_RT_XMT_MID ];
  struct ty_api39_rt_mid_desc mc[ MAX_API39_RT_MC ];
};
typedef struct ty_apief_rt_mid_desc_ary TY_APIEF_RT_MID_DESC[ MAX_API39_RT ];
extern TY_APIEF_RT_MID_DESC *apief_rt_mid_desc_p[ API39_MAX_STREAM ];


/* API3910 HS-RT Buffer Header */
struct ty_api39_rt_bh_desc
{
  volatile L_WORD ul_BhCtrl; /* Buffer Header Control Word */
  volatile L_WORD ul_SqAddr; /* Status Queue Base Pointer */
  volatile L_WORD ul_EqAddr; /* Event Queue Base Pointer */
  volatile L_WORD ul_BpAddr; /* Data Buffer Queue Base Pointer */
};
typedef struct ty_api39_rt_bh_desc TY_API39_RT_BH_DESC[ MAX_API39_RT_HID *MAX_MEM_MUL ];
extern TY_API39_RT_BH_DESC *api39_rt_bh_desc_p[ API39_MAX_STREAM ];

/* API3910 HS-RT Status Queue Entry */
struct ty_api39_rt_sq_desc
{
  volatile L_WORD ul_SqCtrl; /* Status Queue Control Word */
  volatile L_WORD ul_LcwLsw; /* LS Command Word and LS Action Word */
  volatile L_WORD ul_ActBufPtr; /* Actual Data Buffer Pointer */
  volatile L_WORD ul_FcPaDestAddr; /* Frame Control/Physical Address and Destination Address */
  volatile L_WORD ul_WcwHsmsgw; /* Word Count Word and HS Message Tag Word */
  volatile L_WORD ul_TimeTag; /* Time Tag Word */
};
typedef struct ty_api39_rt_sq_desc TY_API39_RT_SQ_DESC[ MAX_API39_RT_SQ ];
extern TY_API39_RT_SQ_DESC *api39_rt_sq_desc_p[ API39_MAX_STREAM ];

/* API3910 HS-RT Event Queue Entry */
struct ty_api39_rt_eq_desc
{
  volatile L_WORD ul_Dyn1; /* Event Queue Control Word 1 for dynamic data */
  volatile L_WORD ul_Dyn2; /* Event Queue Control Word 2 for dynamic data */
  volatile L_WORD ul_Dyn3; /* Event Queue Control Word 3 for dynamic data */
  volatile L_WORD ul_Dyn4; /* Event Queue Control Word 4 for dynamic data */
};
typedef struct ty_api39_rt_eq_desc TY_API39_RT_EQ_DESC[ MAX_API39_RT_EQ ];
extern TY_API39_RT_EQ_DESC *api39_rt_eq_desc_p[ API39_MAX_STREAM ];

/********************************/
/****    RT globales      *******/
/********************************/


/* Information Dynamic Data  */
struct ty_api39_rt_dytag_setup
{
  WORD uw_ena;
  BYTE uc_mode;
};

struct ty_api39_rt_hs_systag_setup
{
  BYTE id;
  BYTE cnt;
  BYTE *p; /* allocated array of Systag Ids */
};

typedef struct
{
  struct ty_api39_rt_hs_systag_setup mid[ MAX_APIEF_RT_XMT_MID ];
} TY_API39_RT_HS_SYSTAG_SETUP[ MAX_API39_RT ];

typedef struct
{
    WORD type[3];   /* mid type ( receive, transmit or modecode ) */
} TY_API39_RT_HS_MID_INT_ALLOC;

typedef struct
{
    TY_API39_RT_HS_MID_INT_ALLOC mid[MAX_APIEF_RT_XMT_MID];
} TY_API39_RT_HS_INT_ALLOC[ MAX_API39_RT ];

struct ty_api39_rt_setup
{
  struct ty_api39_rt_dytag_setup hs_dytag[ MAX_API39_RT_EQ *MAX_MEM_MUL ];
  TY_API39_RT_HS_SYSTAG_SETUP rt_systag; 
  TY_API39_RT_HS_INT_ALLOC rt_int;
  BYTE rt_con[ MAX_API39_RT ];
};
extern struct ty_api39_rt_setup ax_api39_rt_setup[ API39_MAX_STREAM ];
extern L_WORD api39_rt_dytag_status[ API39_MAX_STREAM ][MAX_API39_RT_HID * MAX_MEM_MUL];

  #endif 

/***************************************/
/* API BC Variables                    */
/***************************************/
  #ifdef CMPL4SIM

/* has been defined as global to keep compatibility between 3910 and 1553 old target Software */
struct ty_api39_bc_xfer_desc_info
{
  WORD uw_aw; /* global Action Word for HS-RTRT Xfers */
  WORD uw_type; /* global Transfer type */
};
extern struct ty_api39_bc_xfer_desc_info x_api39_bc_xfer_desc[ API39_MAX_STREAM ][ MAX_API_BC_XFER *MAX_MEM_MUL ]; /* MAX_API_BC_XFER */


struct ty_api39_bc_xfer_dsp
{
  WORD uw_cw1; /* Command Word */
  WORD uw_st1; /* Status Word */
  WORD uw_brw1; /* Status Entry Control Word */
  WORD uw_aw1; /* Action Word */
  WORD uw_cw2; /* Command Word 2*/
  WORD uw_st2; /* Status Word 2*/
  WORD uw_brw2; /* Status Entry Control Word 2*/
  WORD uw_aw2; /* Action Word 2 */
  WORD uw_SqDa; /* Destination Address Word */
  WORD uw_SqFcPa; /* Frame Control Physical Address */
  WORD uw_SqWCnt; /* Word Count word */
  WORD uw_SqHsMsgW; /* 	HS message Tag Word */
  WORD uw_SqCtrl; /* Sq Control Word */
  L_WORD ul_SqActBufptr; /* current Data Buffer Pointer */
  L_WORD ul_SqTimeTag; /* Time Tag */
  L_WORD ul_ttag1; /* LS Time Tag 1*/
  L_WORD ul_xfers1; /* Transfer Counter */
  L_WORD ul_xfer_err1; /* Error Counter */
  L_WORD ul_ttag2; /* LS Time Tag 2*/
  L_WORD ul_xfers2; /* Transfer Counter 2 */
  L_WORD ul_xfer_err2; /* Error Counter 2 */
  L_WORD ul_ServiceRequestCnt1; /* Service Request Counter 1 */
  L_WORD ul_LastVectorWord1; /* Last Vector Word 1 */
  L_WORD ul_ServiceRequestCnt2; /* Service Request Counter 2 */
  L_WORD ul_LastVectorWord2; /* Last Vector Word 2 */
};
extern struct ty_api39_bc_xfer_dsp x_api39_bc_hs_xfer_dsp[ API39_MAX_STREAM ];

struct ty_api39_bc_bh_def
{

  WORD sid;
  WORD eid;
  WORD nbufs;
  L_WORD ul_Mem; /* remaining memory */
  L_WORD hid_addr;
  L_WORD bid_addr;
  L_WORD sid_addr;
  L_WORD eid_addr;
};
extern struct ty_api39_bc_bh_def api39_bc_bh_def[ API39_MAX_STREAM ];


struct ty_api39_bc_dytag_setup
{
  WORD uw_ena;
  BYTE uc_mode;
};

typedef struct
{
  BYTE id;
  BYTE cnt;
  BYTE *p; /* allocated array of Systag Ids */
} TY_API39_BC_HS_SYSTAG_SETUP[ MAX_API39_BC_XFER *MAX_MEM_MUL ];

struct ty_api39_bc_setup
{
  struct ty_api39_bc_dytag_setup hs_dytag[ MAX_API39_BC_EQ *MAX_MEM_MUL ];
  TY_API39_BC_HS_SYSTAG_SETUP bc_systag; 
  BYTE xfer_interrupt[ MAX_API39_BC_XFER *MAX_MEM_MUL ];
};
extern struct ty_api39_bc_setup ax_api39_bc_setup[ API39_MAX_STREAM ];

struct ty_api39_bh_cnt
{
  BYTE rt;
  BYTE bc;
  BYTE uc_XferType;
};
extern struct ty_api39_bh_cnt api39_bh_cnt[ API39_MAX_STREAM ][ MAX_API39_RT_HID *MAX_MEM_MUL ];


struct ty_api39_sys_mem
{
  BYTE uc_mbufs;
  BYTE uc_sbufs;

  BYTE uc_m39_bufs;
  BYTE uc_s39_bufs;

  L_WORD ul_maddr; /* LS Monitor Address */
  L_WORD ul_saddr; /* LS Simulator Address */

  L_WORD ul_m39_offset; /* HS Monitor Offset in GRAM   @AIMFIXME: should be replaced with value from mem partition*/
  L_WORD ul_s39_addr;   /* HS Simulator Address        @AIMFIXME: unused, should be removed */

  WORD uw_fw_vers;
  L_WORD ul_sw_vers;
  L_WORD ul_lca_id; 
  L_WORD ul_lca_chks;
  L_WORD ul_build;
  L_WORD ul_serialNr; /* Serial Number of the Board */

};
extern struct ty_api39_sys_mem api39_sys_mem[ API39_MAX_STREAM ];
extern BYTE api39_bc_dytag_status[ API39_MAX_STREAM ][MAX_API39_BC_HID * MAX_MEM_MUL];

/***************************************/
/*** Bus Monitor structures          ***/
/***************************************/

/* extern BYTE api_bm_ir_status[]; */
extern BYTE uc_api39_bm_stack_dsp[ API39_MAX_STREAM ];
extern L_WORD aul_api39_buf_data[ API39_MAX_STREAM ][ MAX_API39_BM_FRAME_SIZE ];

struct ty_api39_bm_status_cnt
{
  L_WORD ul_Msw;
  L_WORD ul_MsgCnt;
  L_WORD ul_ErrCnt;
};
extern struct ty_api39_bm_status_cnt x_api39_bm_status_cnt[ API39_MAX_STREAM ];

struct ty_api39_bm_rec
{
  volatile BYTE uc_Mode;
  volatile L_WORD ul_HfiOrg;
  volatile L_WORD ul_HfiCnt;
  volatile L_WORD ul_Saddr;
  volatile L_WORD ul_Size;
  volatile L_WORD ul_PreAddr; 
};
extern struct ty_api39_bm_rec x_api39_bm_rec[ API39_MAX_STREAM ];

/* Monitor Activity and Message Filter Control Page */
struct ty_api39_bm_rt_act_flt_entry
{
  volatile L_WORD ul_Ctrl; /* Control Word */
  volatile L_WORD ul_Msg;  /* Message Counter */
  volatile L_WORD ul_Err;  /* Error Counter */
};
struct ty_api39_bm_rt_act_flt_page
{
  struct ty_api39_bm_rt_act_flt_entry ax_RcvMid[ MAX_API39_RT_RCV_MID ];
  struct ty_api39_bm_rt_act_flt_entry ax_XmtMid[ MAX_API39_RT_XMT_MID ];
};
typedef struct ty_api39_bm_rt_act_flt_page TY_API39_BM_RT_ACT_FLT_PAGE[ MAX_API_RT ];

struct ty_apief_bm_rt_act_flt_page
{
  struct ty_api39_bm_rt_act_flt_entry ax_RcvMid[ MAX_APIEF_RT_RCV_MID ];
  struct ty_api39_bm_rt_act_flt_entry ax_XmtMid[ MAX_APIEF_RT_XMT_MID ];
  struct ty_api39_bm_rt_act_flt_entry ax_MCMid[ MAX_API39_RT_MC ];
};
typedef struct ty_apief_bm_rt_act_flt_page TY_APIEF_BM_RT_ACT_FLT_PAGE[ MAX_API_RT ];

extern TY_API39_BM_RT_ACT_FLT_PAGE *px_api39_bm_rt_act_flt_p[ API39_MAX_STREAM ];
extern TY_APIEF_BM_RT_ACT_FLT_PAGE *px_apief_bm_rt_act_flt_p[ API39_MAX_STREAM ];



/*************************************************/
/* 			 API MEM Variables                   */
/* 		 --- Memory Management ----	 			 */
/*************************************************/

/* variable to switch two different memory allocation systems */
extern BYTE start_ptr_look[ API39_MAX_STREAM ];

/* API3910 Buffer Memory Allocation Table */
typedef struct 
{
  BYTE*  start; /*  Buffer Start Address  */
  L_WORD addr;  /*  Heap Pointer Address  */
  L_WORD size;  /*  Size left in memory   */
  L_WORD end;   /*  Buffer End Address    */
} TY_API_BUF_MEMORY;
extern TY_API_BUF_MEMORY *api39_buf_mem_new[ API39_MAX_STREAM ];

extern WORD auw_api39_word_data_arr[ ];
extern L_WORD debug_mode;



/*  */
typedef struct
{
  struct stream_related39
  {
    L_WORD cb_addr; /* System Control Block */
    L_WORD ir_log_addr; /* Interrupt Loglist */
    L_WORD bm_rt_act_flt;
    struct _base39
    {
      L_WORD rt_desc_area;
      L_WORD rt_mid_area;
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_area;
      L_WORD bc_resp_sim_area;
      L_WORD tx_bc_aw_area;
      L_WORD sim_buf;
      L_WORD mon_buf;
      L_WORD rep_buf;
    } base;
    struct _size39
    {
      L_WORD rt_desc_area;
      L_WORD rt_mid_area;
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_area;
      L_WORD bc_resp_sim_area;
      L_WORD tx_bc_aw_area;
      L_WORD sim_buf;
      L_WORD mon_buf;
      L_WORD rep_buf;
    } size;
    struct _count39
    {
      L_WORD rt_desc_area;
      L_WORD rt_mid_area;
      L_WORD rt_mid_rcv;
      L_WORD rt_mid_xmt;
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_area;
      L_WORD bc_resp_sim_area;
      L_WORD tx_bc_aw_area;
    } count;
  } stream[ API39_MAX_STREAM ];
} TY_API39_GLOB_MEMORY;

extern TY_API39_GLOB_MEMORY Api39_glb_mem;

/***************************************/
/* System Dynamic Data Words           */
/***************************************/
struct ty_api39_hs_systag_entry
{
  BYTE mode;
  BYTE run_mode;
  BYTE bc_rt;
  WORD xid_rtmid;
  WORD fct;
  WORD min;
  WORD max;
  WORD step;
  WORD wpos;
  WORD bpos;
  WORD nbit;
  WORD mask;
  BYTE init;
};
struct ty_api39_hs_systag_setup
{
  struct ty_api39_hs_systag_entry dytag[ MAX_API39_SYS_HS_SYSTAGS ];
};
extern struct ty_api39_hs_systag_setup api39_hs_systag_setup[ API39_MAX_STREAM ];

/***************************************/
/* Dyntag Monitor definitions          */
/***************************************/

/* 11.11.05 */
/* Dyntag Monitor RS-SA Lookup Table */
struct ty_api39_bm_hs_dyntagm_mid
{
  L_WORD cnt; /* Dyntag Monitor ID count */
  L_WORD *p;  /* Allocated array of Dyntag Monitor IDs */
};
struct ty_api39_bm_hs_dyntagm_rt
{
  struct ty_api39_bm_hs_dyntagm_mid mid[ MAX_APIEF_RT_RCV_MID + MAX_APIEF_RT_XMT_MID ];
};
struct ty_api39_bm_hs_dyntagm_table
{
  struct ty_api39_bm_hs_dyntagm_rt rt[ MAX_API39_RT ];
}; 
extern struct ty_api39_bm_hs_dyntagm_table api39_hs_dyntagm_lut[ API39_MAX_STREAM ]; /* 11.11.05 */

/* 11.11.05 */
/* Dyntag Monitor Setup Table */
struct ty_api39_bm_hs_dyntagm_entry
{
  BYTE con;
  BYTE rt;
  BYTE mid;
  BYTE mid_type;
 
  BYTE res;
  BYTE dyntag_type;
  WORD dyntag_wpos;
/* BYTE dyntag_bpos; */
/* BYTE dyntag_blen; */
/* BYTE dyntag_step; */

  L_WORD stale_cnt;
  L_WORD bad_cnt;
  L_WORD good_cnt;
  L_WORD dw;
  L_WORD state;
  L_WORD stale_cnt_contig;
  L_WORD good_cnt_contig;
};

struct ty_api39_bm_hs_dyntagm_setup
{
  struct ty_api39_bm_hs_dyntagm_entry dyntag[MAX_API39_BM_HS_DYNTAG_MON_ID];
};
extern struct ty_api39_bm_hs_dyntagm_setup api39_hs_dyntagm_set[ API39_MAX_STREAM ]; /* 11.11.05 */



/***************************************/
/***      Replay structures          ***/
/***************************************/
struct ty_api39_sim_rep
{
  volatile BYTE uc_Mode;
  volatile L_WORD ul_rpi_cnt;
  volatile L_WORD ul_HsMsgCnt;
  volatile L_WORD ul_Saddr;
  volatile L_WORD ul_Size;
  volatile L_WORD ul_NextAddr;
  volatile BYTE uc_Flag;
};
extern struct ty_api39_sim_rep x_api39_sim_rep[ API39_MAX_STREAM ];
extern BYTE api39_rep_status[ API39_MAX_STREAM ];
  #endif 

/* rh Special modification for Rafale */
extern BYTE uc_HsSubAddress;

/* HS Buffer Header Structure */
typedef struct ty_api_hs_bm_fw_mon_buffer_entry
{
  L_WORD ul_BufHeadPointer;
  L_WORD ul_NextBufPointer;
  WORD uw_HsBufTagWord;
  WORD uw_TxInitTime;
  L_WORD ul_HsTimetagHigh;
  L_WORD ul_HsTimetagLow;
  WORD uw_LsActWord;
  WORD uw_LsCmdWord;
  L_WORD ul_LsCtrlWord;
  WORD uw_LsActWord2;
  WORD uw_LsCmdWord2;
  L_WORD ul_LsCtrlWord2;
  L_WORD aul_reserved[ 5 ];
  WORD uw_Da;
  WORD uw_FcPa;
  WORD uw_EntryAmount;
  WORD uw_WCntWord;
  WORD auw_Data[ MAX_API39_BM_BUF_SIZE ];
} TY_API_HS_BM_FW_MON_BUFFER_ENTRY;


/***************************************/
/* API Macros                          */
/***************************************/
  #define MAKE_AW(HsChn,tr,sa,bc) ((WORD)(((HsChn & 0x1F) << 15) + ((tr & 0x01) << 14) + ((sa & 0x7F) << 7) + (bc & 0x1F)))
  #define MAKE_CCX(RxIsEfa,TxIsEfa,Mode,RX,TX) ((WORD)(((RxIsEfa & 0x01) << 12) + ((TxIsEfa & 0x01) << 11) + ((Mode & 0x01) << 10) + ((RX & 0x1F) << 5) + (TX & 0x1F)))

#ifdef __cplusplus
  }
#endif

#endif /* __I39_DEFV_H__ */

 /* end: i39_defv */
