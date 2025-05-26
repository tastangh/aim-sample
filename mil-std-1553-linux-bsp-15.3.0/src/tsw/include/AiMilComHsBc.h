/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsBc.h
*/

#ifndef __MIL_COM_HS_BC_H__
#define __MIL_COM_HS_BC_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


#define MIL_COM_HS_BC_XFER_ERROR     MIL_COM_BC_HS_OFFSET  + 0
#define MIL_COM_HS_BC_BH_DEF         MIL_COM_BC_HS_OFFSET  + 1
#define MIL_COM_HS_BC_BH_READ        MIL_COM_BC_HS_OFFSET  + 2
#define MIL_COM_HS_BC_XFER_DEF       MIL_COM_BC_HS_OFFSET  + 3
#define MIL_COM_HS_BC_XFER_DEF_GET   MIL_COM_BC_HS_OFFSET  + 4
#define MIL_COM_HS_BC_DYTAG_DEF      MIL_COM_BC_HS_OFFSET  + 5
#define MIL_COM_HS_BC_DYTAG_GET      MIL_COM_BC_HS_OFFSET  + 6
#define MIL_COM_HS_BC_XFER_READ      MIL_COM_BC_HS_OFFSET  + 7
#define MIL_COM_HS_BC_STATUS_READ    MIL_COM_BC_HS_OFFSET  + 8
#define MIL_COM_HS_BC_MODE_CTRL      MIL_COM_BC_HS_OFFSET  + 9



/*! Api3910CmdHsBCXferDefErr */

typedef struct
{
    TY_MIL_COM         xCommand;     /*!< default command structure */
    AiUInt32           ulXid;        /*!< The transfer id to modify */
    TY_API_HS_BC_ERROR xError;       /*!< The error injection setup */
}TY_MIL_COM_HS_XFER_ERROR_INPUT;



/* -- MIL_COM_HS_BC_BH_DEF -- */

typedef struct
{
    TY_MIL_COM         cmd;     /*!< default command structure */
    AiUInt32           hid;
    AiUInt32           res;
    AiUInt32           sid;
    AiUInt32           eid;
    AiUInt32           qsize;
    AiUInt32           bqm;
    AiUInt32           bsm;
    AiUInt32           sts;
    AiUInt32           evs;
    AiUInt32           dbm;
    AiUInt32           wcm;
}TY_MIL_COM_HS_BC_BH_DEF_INPUT;



/* -- MIL_COM_HS_BC_BH_READ -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        hid;
}TY_MIL_COM_HS_BC_BH_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    nbufs;
    AiUInt32    sid;
    AiUInt32    eid;
    AiUInt32    mem;
    AiUInt32    hid_addr;
    AiUInt32    bid_addr;
    AiUInt32    sid_addr;
    AiUInt32    eid_addr;
    AiUInt32    hid2;
    AiUInt32    bh1;
    AiUInt32    bp1;
    AiUInt32    sq1;
    AiUInt32    eq1;
    AiUInt32    bh2;
    AiUInt32    bp2;
    AiUInt32    sq2;
    AiUInt32    eq2;
} TY_MIL_COM_HS_BC_BH_READ_OUTPUT;



/* -- MIL_COM_HS_BC_XFER_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xfer_id;
    AiUInt32        hid1;
    AiUInt32        xfer_type;
    AiUInt32        xfer_ls_ir;
    AiUInt32        xfer_hs_ir;
    AiUInt32        xfer_hlt;
    AiUInt32        msg_size;
    AiUInt32        ls_chn;
    AiUInt32        xmt_rt;
    AiUInt32        rcv_rt;
    AiUInt32        xmt_mid;
    AiUInt32        rcv_mid;
    AiUInt32        hs_chn;
    AiUInt32        hs_xmt_start_del;
    AiUInt32        hs_xmt_no_pre;
    AiUInt32        ls_err_type;
    AiUInt32        ls_err_wpos;
    AiUInt32        ls_err_bpos;
    AiUInt32        ls_err_sync;
    AiUInt32        ls_err_contig;
    AiUInt32        ls_err_bc_bits;
    AiUInt32        hs_err_type;
    AiUInt32        hs_err_bpos;
    AiUInt32        hs_err_bc_bits;
    AiUInt32        hs_err_wpos;
    AiUInt32        gap_mode[2];
    AiUInt32        gap[2];
    AiUInt32        sxh;
    AiUInt32        rte;
    AiUInt32        swxm[2];
}TY_MIL_COM_HS_BC_XFER_DEF_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    ls_xfer_desc_addr1;
    AiUInt32    ls_xfer_desc_addr2;
    AiUInt32    hs_xfer_desc_addr;
} TY_MIL_COM_HS_BC_XFER_DEF_OUTPUT;




/* -- MIL_COM_HS_BC_XFER_DEF_GET -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xfer_id;
}TY_MIL_COM_HS_BC_XFER_DEF_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;

    /* as TY_API_HS_BC_XFER */
    AiUInt32        xfer_id;
    AiUInt32        hid1;
    AiUInt32        xfer_type;
    AiUInt32        xfer_ls_ir;
    AiUInt32        xfer_hs_ir;
    AiUInt32        xfer_hlt;
    AiUInt32        msg_size;
    AiUInt32        ls_chn;
    AiUInt32        xmt_rt;
    AiUInt32        rcv_rt;
    AiUInt32        xmt_mid;
    AiUInt32        rcv_mid;
    AiUInt32        hs_chn;
    AiUInt32        hs_xmt_start_del;
    AiUInt32        hs_xmt_no_pre;
    AiUInt32        ls_err_type;
    AiUInt32        ls_err_wpos;
    AiUInt32        ls_err_bpos;
    AiUInt32        ls_err_sync;
    AiUInt32        ls_err_contig;
    AiUInt32        ls_err_bc_bits;
    AiUInt32        hs_err_type;
    AiUInt32        hs_err_bpos;
    AiUInt32        hs_err_bc_bits;
    AiUInt32        hs_err_wpos;
    AiUInt32        gap_mode[2];
    AiUInt32        gap[2];
    AiUInt32        sxh;
    AiUInt32        rte;
    AiUInt32        swxm[2];

    /* as TY_API_HS_BC_XFER_INFO */
    AiUInt32    ls_xfer_desc_addr1;
    AiUInt32    ls_xfer_desc_addr2;
    AiUInt32    hs_xfer_desc_addr;

} TY_MIL_COM_HS_BC_XFER_DEF_GET_OUTPUT;



/* -- MIL_COM_HS_BC_DYTAG_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        con;
    AiUInt32        hid;
    AiUInt32        dmm[4];
    AiUInt32        mode[4];
    AiUInt32        fct_word[4];
    AiUInt32        wpos[4];
}TY_MIL_COM_HS_BC_DYTAG_DEF_INPUT;




/* -- MIL_COM_HS_BC_DYTAG_GET -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        hid;
}TY_MIL_COM_HS_BC_DYTAG_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        dmm[4];
    AiUInt32        mode[4];
    AiUInt32        fct_word[4];
    AiUInt32        wpos[4];
} TY_MIL_COM_HS_BC_DYTAG_GET_OUTPUT;

/* -- MIL_COM_HS_BC_XFER_READ -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        clr;
}TY_MIL_COM_HS_BC_XFER_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32 cmd_word[2];
    AiUInt32 status_word[2];
    AiUInt32 xfer_report_word[2];
    AiUInt32 action_word[2];
    AiUInt32 ttag[2];
    AiUInt32 xfer_cnt[2];
    AiUInt32 err_cnt[2];
    AiUInt32 last_vector_word[2];
    AiUInt32 service_request_cnt[2];
    AiUInt32 sq_dest_addr_word;
    AiUInt32 sq_frm_ctrl_word;
    AiUInt32 sq_wcnt_word;
    AiUInt32 sq_msg_tag_word;
    AiUInt32 sq_entry_ctrl_word;
    AiUInt32 sq_act_buf_pPtr;
    AiUInt32 sq_timetag;
} TY_MIL_COM_HS_BC_XFER_READ_OUTPUT;



/* -- MIL_COM_HS_BC_STATUS_READ -- */

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        msg_cnt;
    AiUInt32        err_cnt;
} TY_MIL_COM_HS_BC_STATUS_READ_OUTPUT;



/* -- MIL_COM_HS_BC_MODE_CTRL -- */
typedef struct
{
    TY_MIL_COM        cmd;
    AiUInt32          mode;
    AiUInt32          ctrl;
    AiUInt32          param1;
    AiUInt32          param2;
    AiUInt32          param3;
}TY_MIL_COM_HS_BC_MODE_CTRL_INPUT;

#endif /* __MIL_COM_HS_BC_H__ */
