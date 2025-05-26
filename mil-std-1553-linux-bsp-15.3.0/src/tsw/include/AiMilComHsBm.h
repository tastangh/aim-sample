/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComHsBm.h
*/

#ifndef __MIL_COM_HS_BM_H__
#define __MIL_COM_HS_BM_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


#define MIL_COM_HS_BM_INI_INTR         MIL_COM_BM_HS_OFFSET + 0
#define MIL_COM_HS_BM_INI_CAP          MIL_COM_BM_HS_OFFSET + 1
#define MIL_COM_HS_BM_INI_FLT          MIL_COM_BM_HS_OFFSET + 2
#define MIL_COM_HS_BM_STATUS_READ      MIL_COM_BM_HS_OFFSET + 3
#define MIL_COM_HS_BM_TRG_CON          MIL_COM_BM_HS_OFFSET + 4
#define MIL_COM_HS_BM_ACT_READ         MIL_COM_BM_HS_OFFSET + 5
#define MIL_COM_HS_BM_RT_ACT_READ      MIL_COM_BM_HS_OFFSET + 6
#define MIL_COM_HS_BM_RT_MID_ACT_READ  MIL_COM_BM_HS_OFFSET + 7
#define MIL_COM_HS_BM_PTR_READ         MIL_COM_BM_HS_OFFSET + 8
#define MIL_COM_HS_BM_STACK_READ       MIL_COM_BM_HS_OFFSET + 9
#define MIL_COM_HS_BM_STACK_FIND       MIL_COM_BM_HS_OFFSET + 10
#define MIL_COM_HS_BM_REC_STATUS       MIL_COM_BM_HS_OFFSET + 11
#define MIL_COM_HS_BM_COPY_REC_DAT     MIL_COM_BM_HS_OFFSET + 12
#define MIL_COM_HS_BM_DYNTAG_DEF       MIL_COM_BM_HS_OFFSET + 13
#define MIL_COM_HS_BM_DYNTAG_READ      MIL_COM_BM_HS_OFFSET + 14



/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/


/* MIL_COM_HS_BM_INI_INTR */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     imod;
    AiUInt32     smod;
} TY_MIL_COM_HS_BM_INI_INTR_INPUT;



/* MIL_COM_HS_BM_INI_CAP */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     hs_cap_mode;
    AiUInt32     tat;
} TY_MIL_COM_HS_BM_INI_CAP_INPUT;



/* MIL_COM_HS_BM_INI_MSG_FLT */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     mce_rx_mid[8];
    AiUInt32     mce_tx_mid[8];

} TY_MIL_COM_HS_BM_INI_FLT_INPUT;


/* MIL_COM_HS_BM_STATUS */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 status;

    AiUInt32 msw;
    AiUInt32 msg_cnt;
    AiUInt32 err_cnt;

    AiUInt32 wcnt_bus_b;
    AiUInt32 wcnt_bus_a;

    AiUInt32 xfer_cnt_pri;
    AiUInt32 xfer_cnt_sec;
    AiUInt32 xfer_err_pri;
    AiUInt32 xfer_err_sec;

    AiUInt32 bus_load_sec;
    AiUInt32 bus_load_pri;
    AiUInt32 bus_load_avg_sec;
    AiUInt32 bus_load_avg_pri;
} TY_MIL_COM_HS_BM_STATUS_READ_OUTPUT;


/* MIL_COM_HS_BM_TRG_CON */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     word_location;
    AiUInt32     trg_con;
    AiUInt32     err_trg_word;
    AiUInt32     mon_data_word;
    AiUInt32     mon_cw_act_word;
    AiUInt32     mon_cw_act_mask;
    AiUInt32     mon_fcpa_word;
    AiUInt32     mon_fcpa_mask;
} TY_MIL_COM_HS_BM_TRG_CON_INPUT;




/* MIL_COM_HS_BM_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     entry_no;
} TY_MIL_COM_HS_BM_ACT_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 fnd;
    AiUInt32 rt;
    AiUInt32 mid;
    AiUInt32 mid_type;
    AiUInt32 msg_cnt;
    AiUInt32 err_cnt;
    AiUInt32 err_type;
} TY_MIL_COM_HS_BM_ACT_READ_OUTPUT;



/* MIL_COM_HS_BM_RT_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
} TY_MIL_COM_HS_BM_RT_ACT_READ_INPUT;


typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 msg_cnt;
    AiUInt32 err_cnt;
    AiUInt32 err_type;
} TY_MIL_COM_HS_BM_RT_ACT_READ_OUTPUT;



/* MIL_COM_HS_BM_RT_MID_ACT_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt;
    AiUInt32     mid;
    AiUInt32     mid_type;
} TY_MIL_COM_HS_BM_RT_MID_ACT_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 msg_cnt;
    AiUInt32 err_cnt;
    AiUInt32 err_type;
} TY_MIL_COM_HS_BM_RT_MID_ACT_READ_OUTPUT;




/* MIL_COM_HS_BM_PTR_READ */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 status;
    AiUInt32 stp;
    AiUInt32 ctp;
    AiUInt32 etp;
    AiUInt32 scn;
    AiUInt32 hcn;
    AiUInt32 ecn;
} TY_MIL_COM_HS_BM_PTR_READ_OUTPUT;



/* MIL_COM_HS_BM_STACK_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     ptr_type;
    AiUInt32     offset_sign;
    AiUInt32     offset;
    AiUInt32     start_index;
    AiUInt32     entry_size;
} TY_MIL_COM_HS_BM_STACK_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 header[16];
    AiUInt32 data[1];
} TY_MIL_COM_HS_BM_STACK_READ_OUTPUT;



/* MIL_COM_HS_BM_STACK_FIND */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     ptr_type;
    AiUInt32     offset_sign;
    AiUInt32     entry_type;
    AiUInt32     offset;
    AiUInt32     entry_word;
    AiUInt32     entry_mask;
} TY_MIL_COM_HS_BM_STACK_FIND_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32     fnd;
    AiUInt32     eos;
    AiUInt32     eptr;
    AiUInt32     wptr;
} TY_MIL_COM_HS_BM_STACK_FIND_OUTPUT;


/* MIL_COM_HS_BM_REC_STATUS */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32     status;
    AiUInt32     hfi_cnt;
    AiUInt32     saddr;
    AiUInt32     size;
} TY_MIL_COM_HS_BM_REC_STATUS_OUTPUT;



/* MIL_COM_HS_BM_COPY_REC_DAT */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     saddr;
    AiUInt32     size;
} TY_MIL_COM_HS_BM_COPY_REC_DAT_INPUT;


/* MIL_COM_HS_BM_DYNTAG_DEF */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
    AiUInt32     con;
    AiUInt32     rt;
    AiUInt32     mid;
    AiUInt32     mid_type;
    AiUInt32     dyntag_type;
    AiUInt32     dyntag_wpos;
    AiUInt32     dyntag_bpos;
    AiUInt32     dyntag_blen;
    AiUInt32     dyntag_step;
} TY_MIL_COM_HS_BM_DYNTAG_DEF_INPUT;



/* TY_MIL_COM_HS_BM_DYNTAG_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     id;
} TY_MIL_COM_HS_BM_DYNTAG_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32     stale_cnt;
    AiUInt32     bad_cnt;
    AiUInt32     good_cnt;
} TY_MIL_COM_HS_BM_DYNTAG_READ_OUTPUT;




#endif /* __MIL_COM_HS_BC_H__ */
