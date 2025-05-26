/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsBm.h
*/

#ifndef __MIL_COM_LS_BM_H__
#define __MIL_COM_LS_BM_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"

/* -- LS BM commands --- */
#define MIL_COM_LS_BM_INI              MIL_COM_BM_LS_OFFSET + 0
#define MIL_COM_LS_BM_INI_INTR         MIL_COM_BM_LS_OFFSET + 1
#define MIL_COM_LS_BM_INI_CAP          MIL_COM_BM_LS_OFFSET + 2
#define MIL_COM_LS_BM_INI_SWXM         MIL_COM_BM_LS_OFFSET + 3
#define MIL_COM_LS_BM_START            MIL_COM_BM_LS_OFFSET + 4
#define MIL_COM_LS_BM_HALT             MIL_COM_BM_LS_OFFSET + 5
#define MIL_COM_LS_BM_STATUS_READ      MIL_COM_BM_LS_OFFSET + 6
#define MIL_COM_LS_BM_ACT_READ         MIL_COM_BM_LS_OFFSET + 7
#define MIL_COM_LS_BM_STACK_PTR_READ   MIL_COM_BM_LS_OFFSET + 8
#define MIL_COM_LS_BM_STACK_ENTRY_READ MIL_COM_BM_LS_OFFSET + 9
#define MIL_COM_LS_BM_STACK_ENTRY_FIND MIL_COM_BM_LS_OFFSET + 10
#define MIL_COM_LS_BM_REC_STATUS_READ  MIL_COM_BM_LS_OFFSET + 11
#define MIL_COM_LS_BM_COPY_REC_DAT     MIL_COM_BM_LS_OFFSET + 12
#define MIL_COM_LS_BM_INI_MSG_FLT      MIL_COM_BM_LS_OFFSET + 13
#define MIL_COM_LS_BM_INI_MSG_ILL      MIL_COM_BM_LS_OFFSET + 14
#define MIL_COM_LS_BM_INI_FTW          MIL_COM_BM_LS_OFFSET + 15
#define MIL_COM_LS_BM_INI_TIW          MIL_COM_BM_LS_OFFSET + 16
#define MIL_COM_LS_BM_INI_TCI          MIL_COM_BM_LS_OFFSET + 17
#define MIL_COM_LS_BM_INI_TCB          MIL_COM_BM_LS_OFFSET + 18
#define MIL_COM_LS_BM_RT_ACT_READ      MIL_COM_BM_LS_OFFSET + 19
#define MIL_COM_LS_BM_RT_SA_ACT_READ   MIL_COM_BM_LS_OFFSET + 20
#define MIL_COM_LS_BM_INI_MSG_REC_FLT  MIL_COM_BM_LS_OFFSET + 21
#define MIL_COM_LS_BM_MSG_REC_FLT      MIL_COM_BM_LS_OFFSET + 22
#define MIL_COM_LS_BM_DYNTAG_MON_DEF   MIL_COM_BM_LS_OFFSET + 23
#define MIL_COM_LS_BM_DYNTAG_MON_READ  MIL_COM_BM_LS_OFFSET + 24


/* -- Data Queue commands --- */
#define MIL_COM_LS_BM_DATAQUEUE_OPEN    MIL_COM_BM_LS_DQUEUE_OFFSET + 0
#define MIL_COM_LS_BM_DATAQUEUE_CONTROL MIL_COM_BM_LS_DQUEUE_OFFSET + 1
#define MIL_COM_LS_BM_DATAQUEUE_CLOSE   MIL_COM_BM_LS_DQUEUE_OFFSET + 2




/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/





/*****************************************************************************/
/*                                                                           */
/*                                  BM                                       */
/*                                                                           */
/*****************************************************************************/


/* MIL_COM_LS_BM_INI_INTR */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     imod;
    AiUInt32     smod;
} TY_MIL_COM_LS_BM_INI_INTR_INPUT;


/* MIL_COM_LS_BM_INI_CAP */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     cmod;
    AiUInt32     tat;
    AiUInt32     mcc;
    AiUInt32     fsize;
} TY_MIL_COM_LS_BM_INI_CAP_INPUT;


/* MIL_COM_LS_BM_INI_SWXM */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     swxm;
} TY_MIL_COM_LS_BM_INI_SWXM_INPUT;


/* MIL_COM_LS_BM_STATUS_READ */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 men;
    AiUInt32 msw;
    AiUInt32 msp;
    AiUInt32 glb_msg_cnt;
    AiUInt32 glb_err_cnt;
    AiUInt32 glb_word_cnt_sec;
    AiUInt32 glb_word_cnt_pri;
    AiUInt32 glb_msg_cnt_sec;
    AiUInt32 glb_msg_cnt_pri;
    AiUInt32 glb_err_cnt_sec;
    AiUInt32 glb_err_cnt_pri;
    AiUInt32 bus_load_sec;
    AiUInt32 bus_load_pri;
    AiUInt32 bus_load_sec_avg;
    AiUInt32 bus_load_pri_avg;
} TY_MIL_COM_LS_BM_STATUS_OUTPUT;




/* MIL_COM_LS_BM_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     entry_no;
} TY_MIL_COM_LS_BM_ACT_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 fnd;
    AiUInt32 rt;
    AiUInt32 tr;
    AiUInt32 sa;
    AiUInt32 cc;
    AiUInt32 ec;
    AiUInt32 et;
} TY_MIL_COM_LS_BM_ACT_READ_OUTPUT;



/* MIL_COM_LS_BM_STACK_PTR_READ */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 status;
    AiUInt32 stp;
    AiUInt32 ctp;
    AiUInt32 etp;
} TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT;



/* MIL_COM_LS_BM_STACK_ENTRY_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     mode;
    AiUInt32     sgn;
    AiUInt32     offset;
} TY_MIL_COM_LS_BM_STACK_ENTRY_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 bm_stack_entry;
} TY_MIL_COM_LS_BM_STACK_ENTRY_READ_OUTPUT;



/* MIL_COM_LS_BM_STACK_ENTRY_FIND */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     mode;
    AiUInt32     sgn;
    AiUInt32     offset;
    AiUInt32     spec;
    AiUInt32     etw;
    AiUInt32     etm;
} TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 fnd;
    AiUInt32 eptr;
    AiUInt32 entry;
} TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_OUTPUT;


/* MIL_COM_LS_BM_REC_STATUS */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 status;
    AiUInt32 hfi_cnt;
    AiUInt32 saddr;
    AiUInt32 size;
} TY_MIL_COM_LS_BM_REC_STATUS_OUTPUT;



/* MIL_COM_LS_BM_COPY_REC_DAT */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     res;
    AiUInt32     start_offset;
    AiUInt32     size;
} TY_MIL_COM_LS_BM_COPY_REC_DAT_INPUT;




/* MIL_COM_LS_BM_INI_MSG_FLT */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     rcv_sa;
    AiUInt32     xmt_sa;
    AiUInt32     rcv_mc;
    AiUInt32     xmt_mc;
} TY_MIL_COM_LS_BM_INI_MSG_FLT_INPUT;




/* MIL_COM_LS_BM_INI_MSG_ILL */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     rcv_sa;
    AiUInt32     xmt_sa;
    AiUInt32     rcv_mc;
    AiUInt32     xmt_mc;
} TY_MIL_COM_LS_BM_INI_MSG_ILL_INPUT;




/* MIL_COM_LS_BM_INI_FTW */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     con;
    AiUInt32     htm;
    AiUInt32     htc;
    AiUInt32     stm;
    AiUInt32     stc;
} TY_MIL_COM_LS_BM_INI_FTW_INPUT;



/* MIL_COM_LS_BM_INI_TIW */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     con;
    AiUInt32     eti;
    AiUInt32     aei;
    AiUInt32     ti0;
    AiUInt32     ti1;
} TY_MIL_COM_LS_BM_INI_TIW_INPUT;


/* MIL_COM_LS_BM_INI_TCI */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     sa_mc;
    AiUInt32     sa_type;
    AiUInt32     tagm;
    AiUInt32     tfct;
    AiUInt32     tid;
} TY_MIL_COM_LS_BM_INI_TCI_INPUT;


/* MIL_COM_LS_BM_INI_TCB */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     tid;
    AiUInt32     ten;
    AiUInt32     tt;
    AiUInt32     sot;
    AiUInt32     tri;
    AiUInt32     inv;
    AiUInt32     tres;
    AiUInt32     tset;
    AiUInt32     tsp;
    AiUInt32     next;
    AiUInt32     eom;
    AiUInt32     tdw;
    AiUInt32     tmw;
    AiUInt32     tuli;
    AiUInt32     tlli;
} TY_MIL_COM_LS_BM_INI_TCB_INPUT;




/* MIL_COM_LS_BM_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
} TY_MIL_COM_LS_BM_RT_ACT_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 cc;
    AiUInt32 ec;
    AiUInt32 et;
} TY_MIL_COM_LS_BM_RT_ACT_READ_OUTPUT;


/* MIL_COM_LS_BM_RT_SA_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     sa;
    AiUInt32     sa_type;
} TY_MIL_COM_LS_BM_RT_SA_ACT_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 cc;
    AiUInt32 ec;
    AiUInt32 et;
} TY_MIL_COM_LS_BM_RT_SA_ACT_READ_OUTPUT;



/* MIL_COM_LS_BM_INI_MSG_REC_FLT */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     cnt;
    AiUInt8      mrec[1];
} TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT;


/* MIL_COM_LS_BM_MSG_FLT_REC */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     mode;
    AiUInt32     con;
    AiUInt32     max_size;
    AiUInt32     max_msg;
} TY_MIL_COM_LS_BM_MSG_FLT_REC_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 ovfl;
    AiUInt32 out_size;
} TY_MIL_COM_LS_BM_MSG_FLT_REC_OUTPUT;



/* MIL_COM_LS_BM_DYNTAG_DEF */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
    AiUInt32     con;
    AiUInt32     rt;
    AiUInt32     sa;
    AiUInt32     sa_type;
    AiUInt32     dyntag_type;
    AiUInt32     dyntag_wpos;
    AiUInt32     dyntag_bpos;
    AiUInt32     dyntag_blen;
    AiUInt32     dyntag_step;
} TY_MIL_COM_LS_BM_DYNTAG_DEF_INPUT;



/* MIL_COM_LS_BM_DYNTAG_MON_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
} TY_MIL_COM_LS_BM_DYNTAG_MON_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 stale_cnt;
    AiUInt32 bad_cnt;
    AiUInt32 good_cnt;
} TY_MIL_COM_LS_BM_DYNTAG_MON_READ_OUTPUT;




/*****************************************************************************/
/*                                                                           */
/*                             DATA QUEUE                                    */
/*                                                                           */
/*****************************************************************************/

/* MIL_COM_LS_BM_DATAQUEUE_OPEN */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
    AiUInt32     asp_queue_size;
    AiUInt32     execution_key;
} TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 retval;
    AiUInt32 dataqueue_offset;
    AiUInt32 dataqueue_size;
} TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_OUTPUT;


/* MIL_COM_LS_BM_DATAQUEUE_CLOSE */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
} TY_MIL_COM_LS_BM_DATAQUEUE_CLOSE_INPUT;


/* MIL_COM_LS_BM_DATAQUEUE_CONTROL */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
    AiUInt32     mode;
} TY_MIL_COM_LS_BM_DATAQUEUE_CONTROL_INPUT;



#endif /* __MIL_COM_LS_BC_H__ */
