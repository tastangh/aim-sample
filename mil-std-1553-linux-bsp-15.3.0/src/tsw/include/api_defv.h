/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_defv.h
 *
 *  Header information of the 1553 Driver Software.Definition of variables, arrays and structures, external references.  
 *
 */

#ifndef __API_DEFV_H__
#define __API_DEFV_H__

#ifdef __cplusplus
  extern "C" {
#endif

/* Basic Typedefs */

#include "Ai_cdef.h"
#include "Ai_container.h"
#include "api_defs.h"
#include "Ai1553Com.h"
#include "i39_defs.h"

#if !defined(_AIM_VME) && !defined(_AIM_LINUX)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int L_WORD;
#endif

#if defined(_AIM_VMEWINDOWS)
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long L_WORD;
#endif

/***************************************/
/* Type Definitions                    */
/***************************************/

/* void function pointer */
typedef void V_FNPTR(void);

#if defined(_AIM_SYSDRV) || defined (_AIM_VME)
    /* Pointer to memory location on target. Not to use in system driver environment */
    typedef L_WORD TARGET_POINTER;
#else
    /* Pointer to memory location on target. Not to use in system driver environment */
    typedef BYTE*   TARGET_POINTER;
#endif

/****************************************************************************************/
/* Pointer to API Target Communication locations relative to the shared RAM start       */
/****************************************************************************************/

typedef struct
{
  TARGET_POINTER trg_ctl; /* 0x00 Target Command Array */
  TARGET_POINTER cmd_array; /* 0x04 Target Command Array */
  TARGET_POINTER ack_array; /* 0x08 Target Acknowledge Array */
  TARGET_POINTER spare_1;
  TARGET_POINTER biu_ill; /* 0x10 ASP Interrupt Loglist */
  TARGET_POINTER spare_2[ 3 ];
  TARGET_POINTER ls_rec_area; /* 0x20 Low Speed Recording area */
  TARGET_POINTER ls_rep_area; /* 0x24 Low Speed Replay area */
  TARGET_POINTER spare_3[ 2 ];
  TARGET_POINTER hs_rec_area; /* 0x30 High Speed Recording area */
  TARGET_POINTER hs_rep_area; /* 0x34 High Speed Replay area */
  TARGET_POINTER spare_4[ 9 ];
  /* The following Pointers are only related in VME and VXI Environment */
  TARGET_POINTER local_ram_base; /* 0x5C Pointer to the Local RAM in Memory */
  TARGET_POINTER universe_base; /* 0x60 Pointer to the Register Base of the UNIVERSE II Chip in Memory */
  TARGET_POINTER galileo_base; /* 0x64 Pointer to the Register Base of the Galileo Chip in Memory */
  TARGET_POINTER glob_ram_base; /* 0x68 Pointer to the Global RAM in Memory*/
} TY_TRG_COM_POINTER;

/***************************************/
/* API Target Communication area       */
/***************************************/

/* Target Communication Control Register Block */
typedef struct
{
  volatile L_WORD ul_OsBit;        /* Host Os Bit */
  volatile L_WORD res;             /* was Target Subsystem ID from PCI Header */
  volatile L_WORD tg_control;      /* Target Communication Control Register */
  volatile L_WORD tg_status;       /* Target Communication Status Register */
  volatile L_WORD ul_DmaControl;   /* bit 0: start dma (1 = start dma, 0 = no dma issued) */
  volatile L_WORD ul_DmaStatus;    /* bit 0: start dma (1 = start dma, 0 = no dma issued) */
  volatile L_WORD ul_DmaMemType;   /* 0 = GLOBAL, 1 = SHARED, 2 = LOCAL */
  volatile L_WORD ul_DmaSourcePtr; /* relative (memory dependent) address to read from */
  volatile L_WORD ul_DmaDestinationPtr; /* physical PCI address to write to */
  volatile L_WORD ul_DmaSize;      /* size in bytes to read */
  volatile L_WORD ul_Time;         /* time in us to set up DMA */
  volatile L_WORD ul_LoadTimeMin;  /* Minimum time in us for one loop */
  volatile L_WORD ul_LoadTimeAct;  /* Actual time in us for one loop */
  volatile L_WORD ul_LoadTimeMax;  /* Maximum time in us for one loop */
  volatile L_WORD ul_LoadTimeMax1; /* Maximum time in us for one loop (last second) */
  volatile L_WORD ul_IRCounter;
} TY_API_TARG_COM_REG;

/* Target Communication Command and Acknowledge array */

typedef struct
{
  volatile L_WORD xf_ctrl; /* Transfer Control Word */
  volatile L_WORD bhd_ptr; /* Buffer Header Pointer */
  volatile L_WORD cw2_cw1; /* Command Word1 and Command Word 2 */
  volatile L_WORD img_swm; /* Gap Time Word and Status Word Exception */
  volatile L_WORD msg_cnt; /* Transfer Counter */
  volatile L_WORD err_cnt; /* Error Counter */
  volatile L_WORD xf_last; /* Descriptor Pointer to Transfer causing Vector Word MC */
} SPECIAL_XFER;

typedef struct
{
  /* Buffer Header */
  volatile L_WORD bh_ctrl; /* Buffer Header Control Word */
  volatile L_WORD sq_addr; /* Status Queue Base Pointer */
  volatile L_WORD eq_addr; /* Event Queue Base Pointer */
  volatile L_WORD bp_addr; /* Data Buffer Base Pointer */

  /* Status Queue */
  volatile L_WORD sq_ctrl; /* Status Queue Control Word */
  volatile L_WORD lcw_lsw; /* Last Command Word and Last Status Word */
  volatile L_WORD act_bptr; /* Actual Data Buffer Pointer */
  volatile L_WORD time_tag; /* Time Tag Word */

  /* Event Queue */
  volatile L_WORD con1; /* Event Queue Control Word and Step Size or Mask Word 1 */
  volatile L_WORD addr1; /* Address or Lower/Upper Limit values 1 */
  volatile L_WORD con2; /* Event Queue Control Word and Step Size or Mask Word 2 */
  volatile L_WORD addr2; /* Address or Lower/Upper Limit values 2 */

  volatile L_WORD data; /* Data Buffer to store received Vector Word */
} SPECIAL_BH;

/***************************************/
/* API Memory Definition               */
/***************************************/

/* API System Control Block */
typedef struct
{
  volatile L_WORD scw;      /* 0x0:  System Control Word */
  volatile L_WORD ssw;      /*       System Status Word */
  volatile L_WORD cid;      /*       Configuration Identification Register */
  volatile L_WORD ilp;      /*       Interrupt Loglist Pointer */
  volatile L_WORD car;      /* 0x10: Coupling and Amplitude Register */
  volatile L_WORD rtpd1;    /*       RT Protocol Definition Register 1 */
  volatile L_WORD rtpd2;    /*       RT Protocol Definition Register 2 */
  volatile L_WORD res;     /*       reserved */
  volatile L_WORD bm_get;   /* 0x20: Monitor Get Offset */
  volatile L_WORD lca_id;   /*       LCA Version / Revision */
  volatile L_WORD glb_mem;  /*       Global Memory Size detected */
  volatile L_WORD biu_core_freq; /*  BIU Core Frequency */
  volatile L_WORD mfc;      /* 0x30: Major Frame Counter */
  volatile L_WORD mft;      /*       Minor Frame Time */
  volatile L_WORD hip;      /*       High Priority BC Instruction List Pointer */
  volatile L_WORD lip;      /*       Low Priority BC Instruction List Pointer */
  volatile L_WORD aip;      /* 0x40: Acyclic Event BC Instruction List Pointer */
  volatile L_WORD res1;     /*       reserved */
  volatile L_WORD cmd_inout;/*       System Command Input / Output Word */
  volatile L_WORD rt_mc;    /*       RT Transfer Counter */
  volatile L_WORD rt_ec;    /* 0x50: RT Error Counter */
  volatile L_WORD bc_mc;    /*       BC Transfer Counter */
  volatile L_WORD bc_ec;    /*       BC Error Counter */
  volatile L_WORD syscnt_init;/*     Reserved */
  volatile L_WORD syscnt_ttl;/* 0x60: Reserved */
  volatile L_WORD syscnt_tth;/*      Reserved */
  volatile L_WORD res2[ 3 ];/*       Reserved */
  volatile L_WORD git_hash; /* 0x74: FW Git-Hash*/
  volatile L_WORD fw_build; /*       FW build word */
  volatile L_WORD glob_id;  /*       Global RAM identification */
  volatile L_WORD mbba;     /* 0x80: Monitor Buffer Base Address */
  volatile L_WORD mbfp;     /*       Monitor Buffer Fill Pointer */
  volatile L_WORD msw;      /*       Monitor Status Word */
  volatile L_WORD mftw;     /*       Monitor Function Trigger Word */
  volatile L_WORD mtiw;     /* 0x90: Monitor Trigger Index Word */
  volatile L_WORD mstp;     /*       Monitor Start Trigger Pointer */
  volatile L_WORD mcc;      /*       Monitor Message Capture Count */
  volatile L_WORD tat;      /*       Monitor Trace after Trigger Count */
  volatile L_WORD swxm;     /* 0xA0: Monitor Status Word Exception Mask */
  volatile L_WORD bm_msg_cnt;/*      Monitor Message Counter */
  volatile L_WORD bm_msg_err;/*      Monitor Message Error Counter */
  volatile L_WORD bm_mc;    /*       Monitor Transfer Counter */
  volatile L_WORD bm_ec;    /* 0xB0: Monitor Error Counter */
  volatile L_WORD bm_wcnt_sec; /*    Monitor Word Counter, secondary bus */
  volatile L_WORD bm_wcnt_pri; /*    Monitor Word Counter, primary bus */
  volatile L_WORD bm_act_addr; /*    Monitor activity page address*/
  volatile L_WORD rcw;      /* 0xC0: Replay Control Word */
  volatile L_WORD rrew;     /*       Replay RT Enable Word */
  volatile L_WORD rbb;      /*       Replay Buffer Base Pointer */
  volatile L_WORD rbp;      /*       Replay Buffer Pointer */
  volatile L_WORD rec;      /* 0xD0: Replay Entry Count */
  volatile L_WORD raec;     /*       Replay Actual Entry Count */
  volatile L_WORD ramto;    /*       Replay Absolute Minute Offset Register */
  volatile L_WORD ramio;    /*       Replay Absolute Microsecond Offset Register */
  volatile L_WORD res4[ 4 ];/* 0xE0: Reserved */
  volatile L_WORD bm_xfer_cnt_sec; /* 0xF0: Monitor Transfer Counter, secondary bus -> for HS bus A */
  volatile L_WORD bm_xfer_cnt_pri; /*       Monitor Transfer Counter, primary bus -> for HS bus B */
  volatile L_WORD bm_err_cnt_sec;  /*       Monitor Errorr Counter, secondary bus -> for HS bus A */
  volatile L_WORD bm_err_cnt_pri;  /*       Monitor Errror Counter, primary bus -> for HS bus B */

  /* BC Miscellaneous Block extensions (starts at Byte addr 0x100) */
  volatile L_WORD xf_instr[ 32 ]; /* Transfer Instruction */

  /* Special Transfer Descriptor  */
  volatile SPECIAL_XFER xf_desc[ 2 ];
  volatile L_WORD rsv1[ 4 ];  /* Reserved */
  volatile L_WORD rsv2[ 14 ]; /* Reserved for further expansion */

  /* BC Miscellaneous Block extensions for rtrt transfers (starts at Byte addr 0x200) */
  volatile L_WORD xf_instr_rtrt[ 32 ];     /* Transfer Instruction for Rx in RTRT transfers*/
  volatile SPECIAL_XFER xf_desc_rtrt[ 2 ]; /* 0x0280 */

  volatile SPECIAL_BH xf_bh[ 2 ];      /* 0x02B8 */
  volatile SPECIAL_BH xf_bh_rtrt[ 2 ]; /* 0x0320 */

} TY_API_SYS_CB;

/* API Interrupt Status LogList */
struct ty_api_islp
{
  volatile L_WORD ir_status; /* Interrupt Status Word */
  volatile L_WORD ir_ptr;    /* Current Descriptor Pointer */
};
typedef struct ty_api_islp TY_API_ISLP[ MAX_API_ISLP_SIZE ];

/* Global System IRIG Time */
typedef struct ty_api_isr_irig_time
{
  volatile L_WORD tm_lo;
  volatile L_WORD tm_hi;
} TY_API_ISR_IRIG_TIME;

/***************************************/
/*** Remote Terminal structures      ***/
/***************************************/
  #ifdef CMPL4SIM

/* API RT Descriptor */
struct ty_api_rt_desc
{
  volatile L_WORD rt_ctrl;   /* RT Descriptor Control Word */
  volatile L_WORD sa_desc;   /* RT SA Descriptor Base Pointer */
  volatile L_WORD lcw_lsw;   /* Last Command Word and Last Status Word */
  volatile L_WORD msg_cnt;   /* Transfer Counter */
  volatile L_WORD err_cnt;   /* Error Counter */
  volatile L_WORD res[ 11 ]; /* Reserved */
};
typedef struct ty_api_rt_desc TY_API_RT_DESC[ MAX_API_RT ];

/* API RT SA/MC Descriptor Table */
struct ty_api_rt_sa_desc
{
  volatile L_WORD sa_ctrl; /* Control Word */
  volatile L_WORD bhd_ptr; /* Buffer Header Pointer */
};
struct ty_api_rt_sa_desc_ary
{
  struct ty_api_rt_sa_desc sa_mc[ MAX_API_RT_RCV_SA + MAX_API_RT_XMT_SA + MAX_API_RT_RCV_MC + MAX_API_RT_XMT_MC ];
};
typedef struct ty_api_rt_sa_desc_ary TY_API_RT_SA_DESC[ MAX_API_RT ];

/* API RT Buffer Header */
struct ty_api_rt_bh_desc
{
  volatile L_WORD bh_ctrl; /* Buffer Header Control Word */
  volatile L_WORD sq_addr; /* Status Queue Base Pointer */
  volatile L_WORD eq_addr; /* Event Queue Base Pointer */
  volatile L_WORD bp_addr; /* Data Buffer Queue Base Pointer */
};
typedef struct ty_api_rt_bh_desc TY_API_RT_BH_DESC[ MAX_API_RT_HID * MAX_MEM_MUL ];

/* API RT Status Queue Entry */
struct ty_api_rt_sq_desc
{
  volatile L_WORD sq_ctrl; /* Status Queue Control Word */
  volatile L_WORD lcw_lsw; /* Last Command Word and Last Status Word */
  volatile L_WORD act_buf_ptr; /* Actual Data Buffer Pointer */
  volatile L_WORD time_tag; /* Time Tag Word */
};
typedef struct ty_api_rt_sq_desc TY_API_RT_SQ_DESC[ MAX_API_RT_SQ * MAX_MEM_MUL * 4 ];

/* API RT Event Queue Entry */
struct ty_api_rt_eq_desc
{
  volatile L_WORD con1; /* Event Queue Control Word and Step Size or Mask Word 1 */
  volatile L_WORD addr1; /* Address or Lower/Upper Limit values 1 */
  volatile L_WORD con2; /* Event Queue Control Word and Step Size or Mask Word 2 */
  volatile L_WORD addr2; /* Address or Lower/Upper Limit values 2 */
};
typedef struct ty_api_rt_eq_desc TY_API_RT_EQ_DESC[ MAX_API_RT_EQ * MAX_MEM_MUL ];

  #endif 


/***************************************/
/*** Bus Controller structures       ***/
/***************************************/
  #ifdef CMPL4SIM

/* API BC Transfer Descriptor */
struct ty_api_bc_xfer_desc
{
  volatile L_WORD xf_ctrl; /* Transfer Control Word */
  volatile L_WORD bhd_ptr; /* Buffer Header Pointer */
  volatile L_WORD cw2_cw1; /* Command Word1 and Command Word 2 */
  volatile L_WORD img_swm; /* Gap Time Word and Status Word Exception */
  volatile L_WORD msg_cnt; /* Transfer Counter */
  volatile L_WORD err_cnt; /* Error Counter */
  volatile L_WORD fifo_id; /* Fifo Tagging Index and Transfer ID */ 
  volatile L_WORD dytg_id; /* Dynamic Tagging Index and Transfer ID */
};
typedef struct ty_api_bc_xfer_desc TY_API_BC_XFER_DESC[ MAX_API_BC_XFER * MAX_MEM_MUL ];

/* API BC Buffer Header */
struct ty_api_bc_bh_desc
{
  volatile L_WORD bh_ctrl; /* Buffer Header Control Word */
  volatile L_WORD sq_addr; /* Status Queue Base Pointer */
  volatile L_WORD eq_addr; /* Event Queue Base Pointer */
  volatile L_WORD bp_addr; /* Data Buffer Queue Base Pointer */
};
typedef struct ty_api_bc_bh_desc TY_API_BC_BH_DESC[ MAX_API_BC_HID * MAX_MEM_MUL ];

/* API BC Status Queue Entry */
struct ty_api_bc_sq_desc
{
  volatile L_WORD sq_ctrl; /* Status Queue Control Word */
  volatile L_WORD lsw2_lsw1; /* Last Command Word and Last Status Word */
  volatile L_WORD act_buf_ptr; /* Actual Data Buffer Pointer */
  volatile L_WORD time_tag; /* Time Tag Word */
};
typedef struct ty_api_bc_sq_desc TY_API_BC_SQ_DESC[ MAX_API_BC_SQ * MAX_MEM_MUL * 4 ];

/* API BC Event Queue Entry */
struct ty_api_bc_eq_desc
{
  volatile L_WORD con1; /* Event Queue Control Word and Step Size or Mask Word 1 */
  volatile L_WORD addr1; /* Address or Lower/Upper Limit values 1 */
  volatile L_WORD con2; /* Event Queue Control Word and Step Size or Mask Word 2 */
  volatile L_WORD addr2; /* Address or Lower/Upper Limit values 2 */
};
typedef struct ty_api_bc_eq_desc TY_API_BC_EQ_DESC[ MAX_API_BC_EQ * MAX_MEM_MUL ];

  #endif 

/*****************************/
/*** Replay structures     ***/
/*****************************/
  #ifdef CMPL4SIM
struct ty_api_sim_rep
{
  volatile BYTE mode;
  volatile L_WORD rpi_cnt;
  volatile L_WORD saddr;
  volatile L_WORD size;
  volatile L_WORD entry_cnt;
  volatile BYTE flag;

};
  #endif 


/*****************************/
/*** Fifo structures       ***/
/*****************************/
  #ifdef CMPL4SIM
struct ty_api_queue_dsp
{
  volatile BYTE mode;
  volatile L_WORD saddr;
  volatile L_WORD cur_buf_addr;
  volatile L_WORD half_buf_addr;
  volatile WORD xid;
  volatile BYTE rt;
  volatile BYTE sa;
  volatile BYTE flag;
  volatile WORD buf_cnt;
};
struct ty_api_queue_setup
{
  BYTE max_queue;
  WORD max_size;
  struct ty_api_queue_dsp queue[ MAX_API_QUEUE_SIZE ];
};

  #endif 

/***************************************/
/*** Bus Monitor structures          ***/
/***************************************/
  #ifdef CMPL4MON

/* Monitor Trigger Control Block area */
struct ty_api_bm_tcb_desc
{
  volatile L_WORD trg_ctrl; /* Trigger Control Word */
  volatile L_WORD next_eom; /* Next Index, EOM Index */
  volatile L_WORD wmask;    /* Word Mask */
  volatile L_WORD uli_lli;  /* Upper and Lower Limit */
  volatile L_WORD res[ 4 ]; /* Reserved */
};
typedef struct ty_api_bm_tcb_desc TY_API_BM_TCB_DESC[ MAX_API_TCB ];


/* Monitor Activity and Message Filter Descriptor */
struct ty_api_bm_activity_and_msg_filter_desc
{
    AiUInt16 status;
    AiUInt16 control;
    AiUInt32 transfer_count;
    AiUInt32 error_count;
};
/* Enhanced Monitor Activity and Message Filter Descriptor */
struct ty_api_bm_activity_and_msg_filter_desc_enhanced
{
    AiUInt16 status;
    AiUInt16 control;
    AiUInt32 transfer_count;
    AiUInt32 error_count;
    AiUInt32 transfer_count_2; /* only for enhanced activity and message filter page */
    AiUInt32 error_count_2; /* only for enhanced activity and message filter page */
};
typedef struct ty_api_bm_activity_and_msg_filter_desc_enhanced TY_API_BM_ACTIVITY;

  #endif 


/***************************************/
/* API RT Variables                    */
/***************************************/
  #ifdef CMPL4SIM

struct ty_api_rt_dytag_setup
{
  BYTE ta;
  BYTE sa;
  BYTE mode;
  BYTE mode1;
  WORD min;
  WORD max;
  WORD step;
  WORD wpos;
  WORD min1;
  WORD max1;
  WORD step1;
  WORD wpos1;
};

struct ty_api_rt_ls_dytag_setup
{
  BYTE ena;
  BYTE mode;
};

struct ty_api_rt_ls_systag_setup
{
  BYTE id;
  BYTE cnt;
  BYTE *p; /* allocated array of Systag Ids */
};

typedef struct
{
  struct ty_api_rt_ls_systag_setup sa[ MAX_API_RT_XMT_SA ];
} TY_API_RT_LS_SYSTAG_SETUP[ MAX_API_RT ];

typedef struct
{
  WORD sa[ MAX_API_RT_XMT_SA ];
} TY_API_RT_DYTAG_ALLOC[ MAX_API_RT ];

typedef struct
{
  WORD sa[ MAX_API_RT_XMT_SA ];
} TY_API_RT_QUEUE_ALLOC[ MAX_API_RT ];

typedef struct
{
  WORD sa[ MAX_API_RT_XMT_SA ];
} TY_API_RT_EVENT_ALLOC[ MAX_API_RT ];

typedef struct
{
  WORD sa[ MAX_API_RT_XMT_SA ];
} TY_API_RT_ACTION_ALLOC[ MAX_API_RT ];

typedef struct
{
  WORD sa_tx[ MAX_API_RT_XMT_SA ];
  WORD sa_rx[ MAX_API_RT_XMT_SA ];
} TY_API_RT_INT_ALLOC[ MAX_API_RT ];

struct ty_api_rt_setup
{
  TY_API_RT_DYTAG_ALLOC rt_dytag;
  TY_API_RT_QUEUE_ALLOC rt_queue; 
  TY_API_RT_EVENT_ALLOC rt_event; 
  TY_API_RT_ACTION_ALLOC rt_action; 
  struct ty_api_rt_ls_dytag_setup ls_dytag[ MAX_API_RT_EQ *MAX_MEM_MUL ]; 
  TY_API_RT_LS_SYSTAG_SETUP rt_systag;
  TY_API_RT_INT_ALLOC rt_int;
  BYTE rt_con[ MAX_API_RT ];
};

struct ty_api_rt_sa_dsp
{
  WORD cur_bid;
  WORD cur_brw;
  WORD cur_lcw;
  WORD cur_lsw;
  L_WORD cur_buf;
  L_WORD cur_ttag;
};

  #endif 


/***************************************/
/* API BC Variables                    */
/***************************************/
  #ifdef CMPL4SIM

struct ty_api_bc_dytag_setup
{
  WORD xid;
  BYTE mode;
  BYTE mode1;
  WORD min;
  WORD max;
  WORD step;
  WORD wpos;
  WORD min1;
  WORD max1;
  WORD step1;
  WORD wpos1;
};

struct ty_api_bc_ls_dytag_setup
{
  BYTE ena;
  BYTE mode;
};

typedef struct
{
  BYTE id;
  BYTE cnt;
  BYTE *p; /* allocated array of Systag Ids */
} TY_API_BC_LS_SYSTAG_SETUP[ MAX_API_BC_XFER * MAX_MEM_MUL ];

struct ty_api_bc_frame_setup
{
  WORD cnt;
  L_WORD instr[ MAX_API_BC_XFER_IN_FRAME ];
};

struct ty_api_bc_mframe_setup
{
  WORD cnt;
  BYTE fr[ MAX_API_BC_FRAME_IN_MFRAME ];
};

struct ty_api_bc_acyc_ttag
{
  BYTE ena;
  L_WORD ttag_hi;
  L_WORD ttag_lo;
};

typedef struct
{
  WORD xid[ MAX_API_BC_XFER *MAX_MEM_MUL ];
} TY_API_BC_ACTION_ALLOC;

struct ty_api_bc_setup
{
  WORD smod;    /* BC Start Mode */
  L_WORD cnt;   /* n-times or cyclic */ 
  L_WORD ftime; /* Minor Frame Time */ 
  L_WORD sinst; /* Address of BC Stop Instruction */ 
  L_WORD bc_triggered_inst; /* Address of BC Branch instruction, that indicates when BC was triggered */
  TY_API_BC_ACTION_ALLOC bc_action; 
  struct ty_api_bc_mframe_setup mframe;
  struct ty_api_bc_frame_setup frame[ MAX_API_BC_FRAME ];
  struct ty_api_bc_ls_dytag_setup ls_dytag[ MAX_API_BC_EQ *MAX_MEM_MUL ];
  TY_API_BC_LS_SYSTAG_SETUP bc_systag;
  BYTE xfer_interrupt[ MAX_API_BC_XFER *MAX_MEM_MUL ];
};

struct ty_api_bc_status_dsp
{
  BYTE status;
  WORD hxfer;
};

struct ty_api_bc_status_cnt
{
  L_WORD msg_cnt;
  L_WORD err_cnt;
  L_WORD hip; 
  L_WORD mfc; 
  L_WORD res1; 
  L_WORD res2; 
};

struct ty_api_bc_xfer_dsp
{
  WORD cw1;
  WORD st1;
  WORD cw2;
  WORD st2;
  WORD cur_bid;
  WORD cur_brw;
  L_WORD cur_buf;
  L_WORD cur_ttag;
  L_WORD msg_cnt;
  L_WORD err_cnt;
  L_WORD lvw;
  L_WORD srvreq_cnt;
};

struct ty_api_bc_bh_def
{
  WORD bid;
  WORD sid;
  WORD eid;
  WORD nbufs;
  L_WORD hid_addr;
  L_WORD bid_addr;
  L_WORD sid_addr;
  L_WORD eid_addr;
};

/* Service Request and Vector Word handling */
struct ty_api_bc_srvw_dsp
{
  WORD xid;
  WORD lvw;
  L_WORD src;
};
struct ty_api_bc_srvw_xid_dsp
{
  struct ty_api_bc_srvw_dsp *px_Xid;
};

struct ty_api_bc_srvw_rt_dsp
{
  struct ty_api_bc_srvw_dsp rt[ MAX_API_BC_SRVW_RT ];
};

struct ty_api_bc_srvw_info
{
  WORD xid;
};
struct ty_api_bc_srvw_sa_mid
{
  struct ty_api_bc_srvw_info sa_mid[ MAX_API_BC_SRVW_SAMID ];
};
struct ty_api_bc_srvw_ta
{
  struct ty_api_bc_srvw_sa_mid ta[ MAX_API_BC_SRVW_RT ];
};
struct ty_api_bc_srvw_t
{
  struct ty_api_bc_srvw_ta t[ MAX_API_BC_SRVW_ID ];
};
  #endif 


/***************************************/
/* API BM Variables                    */
/***************************************/
  #ifdef CMPL4MON

struct ty_api_bm_status_cnt
{
  L_WORD msg_cnt;
  L_WORD err_cnt;
  L_WORD bus_load_pri;
  L_WORD bus_load_sec;
  L_WORD bus_load_pri_avg;
  L_WORD bus_load_sec_avg;
};

struct ty_api_bm_rec
{
  volatile BYTE mode;
  volatile L_WORD hfi_org;
  volatile L_WORD hfi_cnt;
  volatile L_WORD saddr;
  volatile L_WORD size;
};

struct ty_api_bm_rt_act_dsp
{
  BYTE fnd;
  BYTE rt;
  BYTE tr;
  BYTE sa;
  L_WORD cc;
  L_WORD ec;
  L_WORD et;
};

struct ty_api_bm_stackp_dsp
{
  BYTE status;
  L_WORD stp;
  L_WORD ctp;
  L_WORD etp;
};

struct ty_api_bm_msg_flt_dsp
{
  BYTE cnt;
  WORD cw[ MAX_API_BM_MSG_FLT_CMD ];
  BYTE pos1[ MAX_API_BM_MSG_FLT_CMD ];
  BYTE pos2[ MAX_API_BM_MSG_FLT_CMD ];
  BYTE ovfl;
  L_WORD out_size;
  L_WORD old_mbfp;
  L_WORD cur_mbfp;
  L_WORD cur_entry_ptr;
  L_WORD bm_entry[ 80 ];
  WORD bm_entry_cnt;
  WORD data_cnt;
  L_WORD data[ 32 ];
  WORD cur_sw_errw;
  L_WORD cur_ttag_high;
  L_WORD cur_ttag_low;
  BYTE trigd; 
  L_WORD cur_cw[2]; /* info for RTRT transfers, first/second command word */
  L_WORD cur_sw[2]; /* info for RTRT transfers, first/second status word */
};

/* Dyntag Monitor RS-SA Lookup Table */
struct ty_api_bm_ls_dyntagm_sa
{
  L_WORD cnt; /* Dyntag Monitor ID count */
  L_WORD *p;  /* Allocated array of Dyntag Monitor IDs */
};
struct ty_api_bm_ls_dyntagm_rt
{
  struct ty_api_bm_ls_dyntagm_sa sa[ MAX_API_RT_SA ];
};
struct ty_api_bm_ls_dyntagm_table
{
  struct ty_api_bm_ls_dyntagm_rt rt[ MAX_API_RT ];
}; 

/* Dyntag Monitor Setup Table */
struct ty_api_bm_ls_dyntagm_entry
{
  BYTE con;
  BYTE rt;
  BYTE sa;
  BYTE sa_type;

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

struct ty_api_bm_ls_dyntagm_setup
{
  struct ty_api_bm_ls_dyntagm_entry dyntag[MAX_API_BM_DYNTAG_MON_ID];
};

  #endif 

/***************************************/
/* API 1760 Protocol Variables         */
/***************************************/
  #ifdef CMPL4SIM
struct ty_api_c1760_setup_buf
{
  WORD buf_id;
  BYTE c01; /* Checksum Word Position */
  BYTE c02[ MAX_API_C1760_C02 ]; /* Critical Authority Checksum Positions */
};
struct ty_api_c1760_setup
{
  BYTE used;
  struct ty_api_c1760_setup_buf c1760_buf[ MAX_API_C1760_BUF ];
};
  #endif 

/***************************************/
/* System Dynamic Data Words           */
/***************************************/
  #ifdef CMPL4SIM
struct ty_api_ls_systag_entry
{
  BYTE mode;
  BYTE run_mode;
  BYTE bc_rt;
  WORD xid_rtsa;
  WORD fct;
  WORD min;
  WORD max;
  WORD step;
  WORD wpos;
  WORD bpos;
  WORD nbit;
  WORD mask;
  BYTE init;
  WORD index;
};
struct ty_api_ls_systag_setup
{
  struct ty_api_ls_systag_entry dytag[ MAX_API_SYS_LS_SYSTAGS ];
};
  #endif 

typedef struct
{
  L_WORD ul_Size;
  L_WORD ul_Source;
  L_WORD ul_Destination;
  void *v_CallBack;
  BYTE uc_Type;
} ty_api_dma_setup;

/***************************************/
/* API System Variables                */
/***************************************/
struct ty_api_ini_info
{
  BYTE bt[ 2 ]; 
  BYTE streams;
  BYTE prot;
  BYTE emod;
  BYTE irg;
  BYTE res1;
  WORD pbi_id_biu1;
  WORD asp_mon_id;
  WORD asp_bite_id;
  WORD pbi_id_biu2;
  L_WORD board_config;
  L_WORD glb_mem_size;
  L_WORD glb_mem_addr;
  L_WORD loc_dram_size;
  L_WORD loc_dram_addr;
  L_WORD shared_dram_size;
  L_WORD shared_dram_addr;
  L_WORD flash_ram_size;
  L_WORD flash_ram_addr;
  L_WORD pci[ 16 ];
  L_WORD bt2[ 2 ]; /* to store board type for AMC1553-4 (third and fourth channel) */
  L_WORD board_type;
  L_WORD board_sub_type;
  L_WORD hardware_variant;

};

struct mil_tsw_discretes
{
    AiUInt32 setup;         /* remember the discrete setup (required for ApiReadDiscretesConfig of some boards) */
};

struct ty_api_timer_intr_list
{
  L_WORD tcnt;
  L_WORD tena;
  L_WORD predev;
  L_WORD isr_cnt;
};


struct ty_api_sys_mem
{
  BYTE mbufs;
  BYTE sbufs;

  L_WORD maddr;
  L_WORD saddr;

  WORD fw_vers;
  L_WORD sw_vers;
  L_WORD lca_id; 
  L_WORD lca_chks;
  L_WORD ul_build;
  L_WORD ul_serialNr;
  L_WORD pci_lca_id;
  L_WORD res[ 4 ];
};


/***************************************/
/* API Macros                          */
/***************************************/

  #define HI_WORD(lw)               ((WORD)(lw >> 16))
  #define LO_WORD(lw)               ((WORD)(lw))
  #define MAKE_B2LWORD(b1,b2,b3,b4) ((L_WORD)(((L_WORD)b1<<24)+((L_WORD)b2<<16)+((L_WORD)b3<<8)+(L_WORD)b4))
  #define MAKE_W2LWORD(b1,b2)       ((L_WORD)(((L_WORD)b1<<16)+ (L_WORD)b2))
  #define MAKE_WORD(b1,b2)          ((WORD)(((WORD)b1 << 8) + b2))
  #define MAKE_INSTR(b1,b2)         ((L_WORD)(((L_WORD)(b1) << 26) + ((L_WORD)(b2) & 0x03FFFFFFL)))
  #define MAKE_CW(rt,tr,sa,wc)      ((WORD)(((rt & 0x1F) << 11) + ((tr & 0x01) << 10) + ((sa & 0x1F) << 5) + (wc & 0x1F)))
  #define MAKE_SW(rt)               ((WORD) (rt << 11))
  #define LWORD_RESET_BITS(lw,pos,len) (lw &= (~(((1L<<len)-1)<<pos)))
  #define LWORD_SET_BITS(lw,b,pos,len) (lw |= (((L_WORD)b&((1L<<len)-1))<<pos))
  #define WORD_RESET_BITS(w,pos,len) (w &= (~(((1<<len)-1)<<pos)))
  #define WORD_SET_BITS(w,b,pos,len) (w |= (((WORD)b&((1<<len)-1))<<pos))
  #define API_INC_LOLI_CNT(idx) {(idx++);idx%=(MAX_API_IR_EVENTS);} 

  #define BCD2BIN_B(x)        ( (x & 0xF) + ( ((x>>4) & 0xF) * 10) )
  #define BCD2BIN_W(x)        ( BCD2BIN_B(x) + BCD2BIN_B(x>>8)*100 )
  #define EXTRACT_BITS(x,o,m) ((x>>o)&m)


typedef struct
{
  WORD nbufs;
  WORD bid;
} TY_API_BH_INFO;

typedef struct
{
  AiUInt32 cnt;
  AiUInt32 instr[ MAX_API_BC_XFER_IN_FRAME ];
  AiUInt32 xid[ MAX_API_BC_XFER_IN_FRAME ];
} TY_MINOR_FRAME_INFO;



typedef struct
{
  WORD type;
  WORD wpos;
  WORD bpos;
  WORD sync;
  WORD contig;
  WORD bc_bits;
  WORD deviation;
} TY_ERROR_INJECTION;

typedef struct
{
  WORD uw_XferId;
  WORD uw_Hid1;
  BYTE uc_XferType;
  BYTE uc_XferLsIr;
  BYTE uc_XferHsIr;
  BYTE uc_XferHlt;
  BYTE uc_MsgSize;
  BYTE uc_LsChn;
  BYTE uc_XmtRt;
  BYTE uc_RcvRt;
  BYTE uc_XmtMid;
  BYTE uc_RcvMid;
  BYTE uc_HsChn;
  WORD uw_HsXmtStartDel;
  BYTE uc_HsXmtNoPre;
  BYTE uc_LsErrType;
  BYTE uc_LsErrWpos;
  BYTE uc_LsErrBpos;
  BYTE uc_LsErrSync;
  BYTE uc_LsErrContig;
  BYTE uc_LsErrBcBits;
  BYTE uc_HsErrType;
  BYTE uc_HsErrBpos;
  BYTE uc_HsErrBcBits;
  WORD uw_HsErrWpos;
  BYTE uc_GapMode1;
  BYTE uc_GapMode2;
  WORD uw_Gap1;
  WORD uw_Gap2;
  BYTE uc_sxh;
  BYTE uc_rte;
  L_WORD ul_LsXferDescAddr1;
  L_WORD ul_LsXferDescAddr2;
  L_WORD ul_HsXferDescAddr;
  L_WORD ul_Swxm1;
  L_WORD ul_Swxm2;
} TY_HS_BC_XFER_DEFINITIONS;

/* Memory Layout */
/* Changed for using more global Memory */
typedef struct
{
  struct biu_related
  {
    L_WORD cb_addr;      /* System Control Block */
    L_WORD ir_log_addr;  /* Interrupt Loglist */
    L_WORD rt_desc_addr; /* RT Descriptor area */
    L_WORD bm_tcb_addr;  /* Monitor Trigger Block area */
    L_WORD bm_act_addr;  /* Monitor Trigger Block area */
    struct _base
    {
      L_WORD rt_sa_desc;
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_desc;
      L_WORD bc_hip_instr;
      L_WORD bc_lip_instr;
      L_WORD bc_acyc_instr;
      L_WORD bm_buf;
      L_WORD rep_buf;
    } base;
    struct _size
    {
      L_WORD rt_sa_desc;
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_desc;
      L_WORD bc_hip_instr;
      L_WORD bc_lip_instr;
      L_WORD bc_acyc_instr;
      L_WORD bm_buf;
      L_WORD rep_buf;
    } size;
    struct _count
    {
      L_WORD rt_bh_area;
      L_WORD rt_sq_area;
      L_WORD rt_eq_area;
      L_WORD bc_bh_area;
      L_WORD bc_sq_area;
      L_WORD bc_eq_area;
      L_WORD bc_xfer_desc;
      L_WORD bc_hip_instr;
      L_WORD bc_lip_instr;
      L_WORD bc_acyc_instr;
    } count;
  } biu[ MAX_BIU ];
  L_WORD sim_buf_base_addr[ MAX_BIU ];
  L_WORD sim_buf_size[ MAX_BIU ];
  L_WORD sim_buf_count[ MAX_BIU ];
  
  /* Variables for USB scatterlist used in USB drivers */
  
  L_WORD UsbScatterListBase;    /* Base of scatterlist in global memory */
  L_WORD UsbScatterListSize;    /* complete size in bytes of global memory */
  L_WORD UsbScatterListEntries; /* number of entries, the scatterlist consists of */
}

TY_API_GLOB_MEMORY;

/* structure for MUX/TRACK Buffer Data */
typedef struct
{
  L_WORD *px_Prev;
  L_WORD *px_Next;
  WORD   *puw_Dest;
  BYTE   uc_Flags;
  WORD   uw_TrackState;
} TY_API_TRACK_LIST_ELEMENT;

typedef struct
{
  BYTE bEnable;
  WORD *pDest;  /* Dest Data */
  BYTE *pFlags; /* Access Flags */
  WORD uTrackSize;
  WORD uTrackStates;
  WORD uTrackFileCount;
  WORD uStartPos;
  WORD uXfer;
  BYTE uRT;
  BYTE uSA;
  BYTE bTX;
  BYTE uBpos;
  BYTE uBlen;
  BYTE uType;
  BYTE uType2;
  BYTE uRelCount;
  int l_Offset;
  L_WORD ul_LastTimeTag;
  BYTE bPreAllocated;
  TY_API_TRACK_LIST_ELEMENT *px_TrackStateList;
} TY_API_TRACK_DEF;


/* structure for Data Queue */
typedef volatile struct
{
  L_WORD status;
  L_WORD put;        /* relative offset to SHARED start */
  L_WORD get;        /* relative offset to SHARED start */
  L_WORD data_start; /* relative offset to SHARED start */
  L_WORD data_size;
  L_WORD reserved1;
  L_WORD reserved2;
  L_WORD reserved3;
  L_WORD reserved4;
  L_WORD reserved5;
  L_WORD bm_get;     /* relative offset to GLOB start */
  BYTE   hs_stream;
  BYTE   reserved6;
  BYTE   reserved7;
  BYTE   reserved8;
  BYTE   id;
  BYTE   biu;
  BYTE   triggered;
  BYTE   first_read;
  L_WORD reserved9;
} TY_API_DATA_QUEUE_HEADER;



typedef struct TY_API_DEV
{

  /***************************************/
  /* API Target Communication area       */
  /***************************************/

  TY_API_TARG_COM_REG *targ_com_reg_p;
  BYTE *cmd_p; /* Target Command area */
  BYTE *ack_p; /* Target Acknowledge area */


  /***************************************/
  /* API Memory Definition               */
  /***************************************/

  TY_API_SYS_CB *cb_p[ MAX_BIU ]; /* API System Control Block */
  TY_API_ISLP *islp_p[ MAX_BIU ]; /* API Interrupt Status LogList */


  /***************************************/
  /*** Remote Terminal structures      ***/
  /***************************************/

  #ifdef CMPL4SIM

  TY_API_RT_DESC *rt_desc_p[ MAX_BIU ];       /* API RT Descriptor */
  TY_API_RT_SA_DESC *rt_sa_desc_p[ MAX_BIU ]; /* API RT SA/MC Descriptor Table */
  TY_API_RT_BH_DESC *rt_bh_desc_p[ MAX_BIU ]; /* API RT SA/MC Descriptor Table */
  TY_API_RT_SQ_DESC *rt_sq_desc_p[ MAX_BIU ]; /* API RT Status Queue Entry */
  TY_API_RT_EQ_DESC *rt_eq_desc_p[ MAX_BIU ]; /* API RT Event Queue Entry */

  #endif 


  /***************************************/
  /*** Bus Controller structures       ***/
  /***************************************/

  #ifdef CMPL4SIM

  TY_API_BC_XFER_DESC *bc_xfer_desc_p[ MAX_BIU ]; /* API BC Transfer Descriptor */

  L_WORD *bc_hip_instr_p[ MAX_BIU ]; /* API BC Instruction Lists */
  L_WORD *bc_lip_instr_p[ MAX_BIU ];
  L_WORD *bc_acyc_instr_p[ MAX_BIU ];


  TY_API_BC_BH_DESC *bc_bh_desc_p[ MAX_BIU ]; /* API BC Buffer Header */
  TY_API_BC_SQ_DESC *bc_sq_desc_p[ MAX_BIU ]; /* API BC Status Queue Entry */

  TY_API_BC_EQ_DESC *bc_eq_desc_p[ MAX_BIU ]; /* API BC Event Queue Entry */

  #endif 


  /***************************************/
  /*** Bus Monitor structures          ***/
  /***************************************/

  #ifdef CMPL4MON

  TY_API_BM_TCB_DESC *bm_tcb_desc_p[ MAX_BIU ];          /* Monitor Trigger Control Block area */

  #endif 


  /***************************************/
  /* API RT Variables                    */
  /***************************************/

  #ifdef CMPL4SIM
  short rt_status[ MAX_BIU ];
  BYTE rt_ir_status[ MAX_BIU][ MAX_API_RT ];
  struct ty_api_rt_setup rt_setup[ MAX_BIU ];
  L_WORD rt_dytag_status[ MAX_BIU ][MAX_API_RT_HID * MAX_MEM_MUL];
  BYTE single_rt_addr[MAX_BIU];
#endif 

  /***************************************/
  /* API BC Variables                    */
  /***************************************/

  #ifdef CMPL4SIM
  short bc_status[ MAX_BIU ];
  short bc_trg_status[ MAX_BIU ];
  short bc_dbc_start_status[ MAX_BIU ];
  short bc_dbc_stop_status[ MAX_BIU ];
  L_WORD bc_dbc_stop_xfctrl_save[ MAX_BIU ];
  L_WORD bc_dbc_stop_xfswm_save[ MAX_BIU ];
  BYTE bc_ir_status[ MAX_BIU ];
  struct ty_api_bc_setup bc_setup[ MAX_BIU ];
  struct ty_api_bc_status_dsp bc_status_dsp[ MAX_BIU ];
  struct ty_api_bc_status_cnt bc_status_cnt[ MAX_BIU ];
  struct ty_api_bc_xfer_dsp bc_xfer_dsp;
  struct ty_api_bc_bh_def bc_bh_def;
  BYTE bc_dytag_status[ MAX_BIU ][MAX_API_BC_HID * MAX_MEM_MUL];
  short bc_traffic_override_enabled[MAX_BIU];

  /* Service Request and Vector Word handling */
  BYTE bc_srvw_con[ MAX_BIU ];
  struct ty_api_bc_srvw_xid_dsp bc_srvw_xid_dsp[ MAX_BIU ];
  struct ty_api_bc_srvw_dsp bc_srvw_dsp[ MAX_BIU ];
  struct ty_api_bc_srvw_rt_dsp bc_srvw_rt_dsp[ MAX_BIU ];
  struct ty_api_bc_srvw_t bc_srvw_tab[ MAX_BIU ];
  struct ty_api_bc_frame_setup bc_srvw_subframe[ MAX_BIU ];
  struct ty_api_bc_acyc_ttag bc_acyc_ttag_start[ MAX_BIU ];

  #endif 

  /***************************************/
  /* API BM Variables                    */
  /***************************************/

  #ifdef CMPL4MON
  short bm_status[ MAX_BIU ];
  BYTE bm_status_dsp[ MAX_BIU ];
  BYTE bm_ir_status[ MAX_BIU ];
  struct ty_api_bm_status_cnt bm_status_cnt[ MAX_BIU ];
  struct ty_api_bm_rec bm_rec[ MAX_BIU ];
  struct ty_api_bm_rt_act_dsp bm_rt_act_dsp;
  //struct ty_api_bm_stackp_dsp bm_stackp_dsp;

  struct ty_api_bm_msg_flt_dsp bm_msg_flt_dsp[ MAX_BIU ];
  BYTE bm_msg_flt_mode[ MAX_BIU ]; 
  struct ty_api_bm_ls_dyntagm_table bm_ls_dyntagm_lut[ MAX_BIU ];
  struct ty_api_bm_ls_dyntagm_setup bm_ls_dyntagm_set[ MAX_BIU ];

  BYTE uc_BmIniClearBuffer;  /* initializing the BM buffer may take some time. */

  AiUInt32 ulUseEnhancedBmActivityPage;
  #endif 

  /***************************************/
  /* API 1760 Protocol Variables         */
  /***************************************/
  #ifdef CMPL4SIM
  struct ty_api_c1760_setup c1760_setup[ MAX_BIU ];
  #endif 

  /***************************************/
  /* System Dynamic Data Words           */
  /***************************************/
  #ifdef CMPL4SIM
  struct ty_api_ls_systag_setup ls_systag_setup[ MAX_BIU ];
  #endif 

  /***************************************/
  /* API System Variables                */
  /***************************************/
  struct ty_api_ini_info ini_info;
  struct mil_tsw_discretes discretes;

  struct ty_api_intr_event_list *intr_list_p;

  struct ty_api_timer_intr_list timer_intr[ MAX_API_TIMER ];
  struct ty_api_sys_mem sys_mem[ MAX_BIU ];
  BYTE reset_can_be_done;
  WORD bite_stream[ MAX_BIU ];
  short biu_active[ MAX_BIU ];
  AiBoolean b_BiuEnabled[MAX_BIU];
  L_WORD islp[ MAX_BIU ];

  /***************************************/
  /* API Communication Variables         */
  /***************************************/
  short b_length;
  short w_length;
  short lw_length;
  BYTE *ack_b_ptr;
  WORD *ack_w_ptr;
  L_WORD *ack_lw_ptr;
  short biu;
  short hs_stream;
                             /* ini_info.streams           */
  short physical_bius;       /* physical bius              */
  short chns;                /* logical bius               */
  short chns1;               /* logical bius on PBI1       */
  short chns2;               /* logical bius on PBI2       */
  short conf1; 
  short conf2; 


  /***************************************/
  /* SIM Only / MON Only / SIM+MON       */
  /***************************************/
  L_WORD ApiRestartAddress;
  BYTE ApiRestartCyclicFlag;
  BYTE byte_data_arr[ MAX_API_BDATA_SIZE ];
  WORD word_data_arr[ MAX_API_WDATA_SIZE ];
  L_WORD lword_data_arr[ MAX_API_LDATA_SIZE ];
  WORD drv_mode[ MAX_BIU ];
  /* Added to support single mode function */
  WORD drv_exe[ MAX_BIU ];
  L_WORD drv_rt_status[ MAX_BIU ];



  /***************************************/
  /* Memory Layout                       */
  /***************************************/
  
  TY_API_GLOB_MEMORY glb_mem;



  /***************************************/
  /* API Replay Variables         */
  /***************************************/
  #ifdef CMPL4SIM
  BYTE rep_status[ MAX_BIU ];
  struct ty_api_sim_rep sim_rep[ MAX_BIU ];
  #endif 

  /******************************/
  /* API Fifi Variables         */
  /******************************/
  #ifdef CMPL4SIM
  struct ty_api_queue_setup queue_setup[ MAX_BIU ];
  #endif 


  /******************************************/
  /*  Variables from MONITOR functions      */
  /******************************************/

  L_WORD selftest_result; /* result of selftest PASS/FAIL     */
  L_WORD boot_width;
  L_WORD biu1_present;
  L_WORD biu2_present;
  L_WORD biu3_present;
  L_WORD biu4_present;
  L_WORD clk_1us;
  BYTE * GlobalRAMBase;         /* Global RAM base address          */
  BYTE * GlobalRAMBase2;        /* Global RAM base address          */
  L_WORD GlobalRAMSize;         /* Global RAM size in bytes         */
  L_WORD GlobalRAMSize2;        /* Global RAM size in bytes         */
  BYTE * GlobalRAMExtBase;      /* Global RAM base address          */
  L_WORD GlobalRAMExtSize;      /* Global RAM size in bytes         */
  BYTE *  SharedRAMBase;        /* Shared RAM base address          */
  L_WORD * IoRAMBase;           /* IO RAM base address              */
  L_WORD * HostIoRAMBase;       /* Host IO RAM base address         */
  L_WORD SharedRAMSize;         /* Shared RAM size in bytes         */
  L_WORD LocalRAMBase;          /* Local RAM base address           */
  L_WORD LocalRAMSize;          /* Local RAM size in bytes          */
  L_WORD BoardSerial;           /* board serial number              */
  L_WORD BoardConfig;           /* board configuration              */
  L_WORD BoardType;             /* board type information           */
  L_WORD BoardApplicationType;  /* board application type (BIU 1/2) */
  L_WORD BoardApplicationType2; /* board application type (BIU 3/4 ArtixUS) */
  L_WORD BoardSubType;          /* extended board information       */
  L_WORD BoardSubType2;         /* extended board information (BIU 3/4 ArtixUS) */
  L_WORD HwVariant;             /* Hardware Variant word            */
  L_WORD BoardPartNo;           /* board part number                */
  L_WORD CPUClk;                /* CPU Clock Frequency MHz          */
  L_WORD SYSClk;                /* System Clock Frequency MHz       */
  L_WORD biu1BootSec;           /* boot sector for BIU1             */
  L_WORD biu2BootSec;           /* boot sector for BIU2             */
  L_WORD biu3BootSec;           /* boot sector for BIU3             */
  L_WORD biu4BootSec;           /* boot sector for BIU4             */
  L_WORD lcaBootSec;            /* LCA boot sector (1...31) 32= NO  */

  L_WORD api_3910_sim_mem_map[ API39_MAX_STREAM ][ API_HS_SIM_MEM_MAP_SIZE ];
  L_WORD uApi3910SimMemFree[ API39_MAX_STREAM ];
  L_WORD uSimMemMapSize[ API39_MAX_STREAM ];

  TY_MINOR_FRAME_INFO apiBcMinorFrameInfo[ MAX_API_BC_FRAME ];

  TY_ERROR_INJECTION        errorInjection[        MAX_API_BC_XFER * MAX_MEM_MUL ];
  WORD                      bcXferGap[             MAX_API_BC_XFER * MAX_MEM_MUL ];
  TY_HS_BC_XFER_DEFINITIONS apiHsBcXferDefinition[ MAX_API_BC_XFER * MAX_MEM_MUL ]; /* @TODO: two arrays for 2 PBIs */

  /******************************************/
  /*  Variables from Fibre Optical Digital  */
  /*  Data Link FODDL                       */
  /******************************************/
  BYTE   bFoddlMode[ API39_MAX_STREAM ];         /* On/OFF                           */
  BYTE   bFoddlTrigger[ API39_MAX_STREAM ];      /* HS BM Trigger flag               */
  WORD   *pFoddlStoreBuf[ API39_MAX_STREAM ];    /* Pointer to the RT Data Buffer    */
  L_WORD pFoddlStoreAddr[ API39_MAX_STREAM ];
  L_WORD *pFoddlStatusBuf[ API39_MAX_STREAM ];   /* Pointer to the RT Satus Queue    */
  L_WORD pFoddlStatusAddr[ API39_MAX_STREAM ];
  void* ApiFoddlFunc[ API39_MAX_STREAM ];



  /***************************************/
  /* GenIo variables                     */
  /***************************************/
  L_WORD ul_GenIo_TestSeq_Running;      /* 0: not active, 1: active */
  BYTE   auc_SequenceActive[4];         /* 0: not triggered, 1: active, 2: finished */
  L_WORD aul_CurrentSequenceEvent[4];   /* just a counter */
  L_WORD ul_LastDiscrStat[4];           /* the result of the last discrete read */
  L_WORD ul_GenIoTimer[4];
  TY_API_GENIO_TEST_SEQ_TBL xGenIoSeqTbl;


  /***************************************/
  /* Other variables                     */
  /***************************************/

  TY_API_TRACK_DEF ls_track[ MAX_BIU][ API_MAX_MUX_BUFFER ];
  TY_API_TRACK_DEF hs_track[ API39_MAX_STREAM ][ API_MAX_MUX_BUFFER ];

  L_WORD dataqueue_header_offsets[ API_MAX_DATA_QUEUE_ID+1 ]; /* API Data Queue LogList */

  TY_API_ISR_IRIG_TIME api_irg_time;
  L_WORD SrecCheckSum;

  L_WORD ul_XferCntForAcyclic;

  #ifdef _AMC1553
    #ifndef _NUCLEUS
  TyMyMonContext         * pxMyMonContext;
  int bModule;
  BYTE *shared;
    #endif 
  #endif 

  #ifndef _AMC1553 /* in case of APX1553 (for MILScope) */
    #ifdef _NUCLEUS
  L_WORD ul_SamplingRate;
  L_WORD ul_OperationMode;
  L_WORD ul_StartMode;
  L_WORD ul_MilScopeRunning;
  long l_ChannelA_ADC1_50MHz;
  long l_ChannelB_ADC2_50MHz;
  long l_ChannelA_ADC1_100MHz;
  long l_ChannelA_ADC2_100MHz;
  long l_ChannelB_ADC1_100MHz;
  long l_ChannelB_ADC2_100MHz;
  L_WORD ul_CouplingPri;
  L_WORD ul_CouplingSec;
    #endif
  #endif

  BYTE   uc_DeviceType;
  L_WORD ul_DeviceGroup;
  BYTE   auc_StreamType[MAX_BIU];
  short ul_NbOfStreams1;
  short ul_NbOfStreams2;
  L_WORD ul_DbgLevel;
  L_WORD ul_InstrLevel[MAX_INSTR_LEVEL];
  L_WORD ul_CntBCInt;
  L_WORD ul_CntRTInt;

#if defined(_NUCLEUS)
  NU_HISR hisr;
  L_WORD  *vp_HISRStackOverflowBuffer;
  void    *vp_HISRStack;

  NU_HISR hisr39;
  L_WORD  *vp_HISRStack39OverflowBuffer;
  void    *vp_HISRStack39;

  #ifndef _AMC1553 /* MilScope */
  L_WORD  *vp_HISRStackMscopeOverflowBuffer;
  void    *vp_HISRStackMscope;
  NU_HISR milscope_hisr;
  #endif
#endif

  BYTE uc_SRecMarkFound;

  L_WORD ul_TCP_FileNameExt;  
  L_WORD ul_ASP_FileNameExt;  
  L_WORD ul_BIP1_FileNameExt; 
  L_WORD ul_BIP2_FileNameExt; 
  L_WORD ul_BIP3_FileNameExt; 
  L_WORD ul_BIP4_FileNameExt; 
  L_WORD ul_ILCA1_FileNameExt;
  L_WORD ul_ILCA2_FileNameExt;
  L_WORD ul_ILCA3_FileNameExt;
  L_WORD ul_ILCA4_FileNameExt;
  L_WORD ul_PLCA_FileNameExt; 

  L_WORD ulGlobalError;
  L_WORD repeat;
#if defined(_AIM_SYSDRV) || defined(_AIM_VME) || defined(_AIM_LVRT) || defined(_AIM_RTX64)
  void* p_DeviceContext;
  void* p_Device;  /*!< WDF device in Windows driver */

  /* only used for USB devices */
  struct ai_list_head     TransferList[MAX_BIU];
  struct ai_tsw_os_lock * TransferListLock[MAX_BIU];
  struct ai_list_head     RTBufferList[MAX_BIU];
  struct ai_tsw_os_lock * RTBufferListLock[MAX_BIU];
#endif


  /* Device capabilities */
  TY_IRIG_CAPABILITIES           x_IrigCapabilities;
  TY_COUPLING_CAPABILITIES       x_CouplingCapabilities;

  struct mil_tasks_info * px_mil_tasks_info;

} TY_API_DEV;

  #if defined(_NUCLEUS)
extern TY_API_DEV *px_api_dev;
  #endif 

typedef void V_FNPTR_DEV(TY_API_DEV*, BYTE biu, L_WORD hs_stream );

/* Must be finally defined by the SW-Engineer who makes the 1553 Target-Driver !!!!!!!!!!!! */
typedef enum _biu_stat
{
  BIU_DOWN, BIU_DISABLED, BIU_BOOTED, BIU_STARTED
} TY_E_BIU_STAT;


typedef struct
{
  L_WORD ul_DriverMemId;                 /* 0x00        */ /* Softie identification                        */
  L_WORD ul_SubSysVenId;                 /* 0x04        */ /* Subsystem and Subvendor ID from PCI Header   */
  L_WORD ul_LocalRamSize;                /* 0x08        */ /* Base Address of Local Memory                 */
  L_WORD ul_LocalRamBase;                /* 0x0C        */ /* Size of Local Memory in Bytes                */
  L_WORD ul_SharedRamSize;               /* 0x10        */ /* Base Address of Shared Memory                */
  L_WORD ul_SharedRamBase;               /* 0x14        */ /* Size of Shared Memory in Bytes               */
  L_WORD ul_GlobalRamSize;               /* 0x18        */ /* Base Address of Global Memory                */
  L_WORD ul_GlobalRamBase;               /* 0x1C        */ /* Size of Global Memory in Bytes               */
  L_WORD ul_BoardType;                   /* 0x20        */ /* NOVRam */ /* Board Interface Type            */
  L_WORD ul_BoardConfig;                 /* 0x24        */ /* NOVRam */ /* Board HW version, Platform Type */
  L_WORD ul_BoardPartNo;                 /* 0x28        */ /* NOVRam */ /* AIM Partnumber for the Board    */
  L_WORD ul_BoardSerialNo;               /* 0x2C        */ /* NOVRam */ /* Board Serial Number             */
  L_WORD ul_SysClk;                      /* 0x30        */ /* NOVRam */ /* System/Bus Clock in MHz         */
  L_WORD ul_CpuClk;                      /* 0x34        */ /* NOVRam */ /* Processor Clock Frequency in MHz */
  L_WORD ul_DevVendId;                   /* 0x38        */ /* Device and Vendor ID from PCI Header         */
  L_WORD ul_Interrupt;                   /* 0x3C        */ /* Host Interrupt                               */
  L_WORD ul_LcaBootSector;               /* 0x44        */ /* NOVRam */
  L_WORD ul_LcaStatus;                   /* 0x48        */
  L_WORD aul_BiuBootSector[ MAX_BIU ];   /* 0x4C - 0x68 */ /* NOVRam */
  TY_E_BIU_STAT ae_BiuStatus[ MAX_BIU ]; /* 0x6C - 0x86 */
  L_WORD ul_NumOfPorts;                  /* 0x88        */
  L_WORD ul_Reserved1[8];                /* 0x8C - 0xA8 */
  L_WORD ul_IrigTimeHi;                  /* 0xAC        */
  L_WORD ul_IrigTimeLo;                  /* 0xB0        */
  L_WORD ul_GlobalRamSize2;              /* 0xB4        */
  L_WORD ul_GlobalRamBase2;              /* 0xB8        */
  L_WORD ul_AspRunningMagic;             /* 0xBC        */ /* value set by when TSW is ready. And cleared during reboot */
  L_WORD ul_Reserved2[ 3 ];              /* 0xC0 - 0xC2 */
} TY_HW_INFORMATION;


typedef union
{
  L_WORD all;
  struct
  {
    L_WORD Data: 8;
    L_WORD Command: 5;
    L_WORD res15_13: 3;
    L_WORD Ready: 1;
    L_WORD CSx: 1;
    L_WORD RdWr: 1;
    L_WORD res31_19: 13;
  } bit;
}

TY_TCP_COM_REG;

#ifdef _NUCLEUS
typedef	union
	{
			L_WORD  ul_All;
			struct
			{
				L_WORD  sec	    	: 6   ;	  /* seconds of minute*/
				L_WORD  minutes     : 6   ;     /* minutes of hour*/
				L_WORD  hour        : 5   ;     /* hour of day*/
				L_WORD  day         : 9   ;     /* day of year*/
				L_WORD              : 6   ;     /* reserved*/
				L_WORD		      	: 0   ;	  /* alignment     */
			}Reg;
	}PMC_TTHIGH_Reg;

typedef	union
	{
			L_WORD  ul_All;
			struct
			{
				L_WORD  usec		: 20  ;     /* us of seconds*/
				L_WORD  sec		    : 6   ;     /* seconds of minute*/
				L_WORD  minutes     : 6   ;     /* minutes of hour*/
				L_WORD		    	: 0	  ;	   /* alignment     */
			}Reg;
	}PMC_TTLOW_Reg;
#endif

typedef struct 
{
  L_WORD rt_bh_area;
  L_WORD rt_sq_area;
  L_WORD rt_eq_area;
  L_WORD bc_bh_area;
  L_WORD bc_sq_area;
  L_WORD bc_eq_area; 
  L_WORD bc_xfer_desc;
  L_WORD bc_hip_instr;
  L_WORD bc_lip_instr; 
  L_WORD bc_acyc_instr;
  L_WORD rep_buf; 
  L_WORD bm_buf; 
  L_WORD sim_buf;
  L_WORD UsbScatterListEntries;  /* number of entries the scatterlist shall contain as maximum */
} TY_MEM_DEFINITION;


/***********************************************/
/*  Structs for ASP Realtime Driver extension  */
/***********************************************/
#ifdef _CMPL4DTS
typedef int (*P_FNC_PTR)();

/* IP Hardware Access Setup */
typedef struct
{
     volatile L_WORD model;
     volatile L_WORD ip_addr;
     volatile L_WORD sh_addr;
     volatile L_WORD data;

} TY_API_ASP_DRV_IP_CHN_SETUP_TYPE;

typedef struct
{
     TY_API_ASP_DRV_IP_CHN_SETUP_TYPE din;
     TY_API_ASP_DRV_IP_CHN_SETUP_TYPE dout;

} TY_API_ASP_DRV_IP_CHN_SETUP;

/* IP Sampling Task Setup */
typedef struct
{
     volatile L_WORD ena;
     volatile L_WORD isr_cnt;
     volatile L_WORD rate;
     volatile L_WORD res;
     volatile L_WORD ustep;

} TY_API_ASP_DRV_IP_STASK_SETUP;

/* IP Recording setup */
typedef struct 
{
     volatile L_WORD rec_con;
     volatile L_WORD rec_status;
     volatile L_WORD status;
     volatile L_WORD wr_addr;
     volatile L_WORD rd_addr;
     volatile L_WORD entry_cnt;
} TY_API_ASP_DRV_IP_REC_STATUS;

/* ASP Driver Channel structure */ 
typedef struct
{
     L_WORD used;
     L_WORD ip_type;
	 P_FNC_PTR p_dhw_init_func;
	 P_FNC_PTR p_dout_write_func;
	 P_FNC_PTR p_dout_read_func;
	 P_FNC_PTR p_din_read_func;
     TY_API_ASP_DRV_IP_CHN_SETUP   ip_setup;
     TY_API_ASP_DRV_IP_STASK_SETUP stask_setup;
	 TY_API_ASP_DRV_IP_REC_STATUS  rec_status;
     volatile L_WORD in_data[16];
     volatile L_WORD act_data[16];
     volatile L_WORD out_data[16];
     volatile WORD rec_data[16];
} TY_API_ASP_DRV;

 #if defined(_NUCLEUS)
  extern TY_API_ASP_DRV ip_drv[];
 #endif

#endif /* _CMPL4DTS */

/********************************************************/







/*****************************************************************************/
/*                                                                           */
/*                    Target command table definition                        */
/*                                                                           */
/*****************************************************************************/

typedef L_WORD (*TY_MIL_COM_FUNC_PTR)(TY_API_DEV *, TY_MIL_COM *, TY_MIL_COM_ACK * ack_p );


typedef struct 
{
    L_WORD               ulFuncCode;
    TY_MIL_COM_FUNC_PTR  pFunc;
    const char *         pszFuncName;
    L_WORD               ulMinimumSize;
    L_WORD               ulMaximumAckSize;
} TY_MIL_COM_TABLE_ENTRY;



/* Types from delete Windows System Driver api_defv.h */


typedef union
{
  L_WORD ul_All;
  struct
  {
    AiUInt ul_Reserved:24;
    AiUInt uc_IntType:8;
  }t;

  struct
  {
    AiUInt ul_Reserved:24;
    AiUInt uc_Biu1:1;
    AiUInt uc_Biu2:1;
    AiUInt uc_Dma:1;
    AiUInt uc_Target:1;
    AiUInt uc_Cmd:1;
    AiUInt uc_Biu3:1;
    AiUInt uc_Biu4:1;
    AiUInt res:1;
  }b;
} TY_LOGLIST_LLC_ENTRY;

typedef union
{
  L_WORD ul_All;
  struct
  {
    AiUInt ul_Index:16;
    AiUInt uc_Reserved:8;
    AiUInt uc_IntSrc:8;
  }t;
} TY_LOGLIST_LLD_ENTRY;

typedef struct
{
  volatile AiUInt32 ul_Lla;
  volatile AiUInt32 ul_Llb;
  volatile TY_LOGLIST_LLC_ENTRY x_Llc;
  volatile TY_LOGLIST_LLC_ENTRY x_Lld;
} TY_API_INT_LOGLIST_ENTRY;



typedef struct 
{
  volatile AiUInt32 put_cnt;
  volatile AiUInt32 get_cnt;
  volatile TY_API_INT_LOGLIST_ENTRY entry[ MAX_API_IR_EVENTS ];
} TY_API_INT_LOGLIST;


#ifdef __cplusplus
  }
#endif

#endif /* __API_DEFV_H__ */


/* end: api_defv */
