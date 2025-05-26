/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsBc.h
*/

#ifndef __MIL_COM_LS_BC_H__
#define __MIL_COM_LS_BC_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"
#include "AiMilComLsBuf.h"

#define MIL_COM_LS_BC_XFER_ERROR      MIL_COM_BC_LS_OFFSET  + 0
#define MIL_COM_LS_BC_XFER_DESC_GET   MIL_COM_BC_LS_OFFSET  + 1
#define MIL_COM_LS_BC_XFER_DESC_MOD   MIL_COM_BC_LS_OFFSET  + 2
#define MIL_COM_LS_BC_INI             MIL_COM_BC_LS_OFFSET  + 3
#define MIL_COM_LS_BC_BH_DEF          MIL_COM_BC_LS_OFFSET  + 4
#define MIL_COM_LS_BC_XFER_DEF        MIL_COM_BC_LS_OFFSET  + 5
#define MIL_COM_LS_BC_XFER_CTRL       MIL_COM_BC_LS_OFFSET  + 6
#define MIL_COM_LS_BC_START           MIL_COM_BC_LS_OFFSET  + 7
#define MIL_COM_LS_BC_HALT            MIL_COM_BC_LS_OFFSET  + 8
#define MIL_COM_LS_BC_XFER_DEF_GET    MIL_COM_BC_LS_OFFSET  + 9
#define MIL_COM_LS_BC_BH_READ         MIL_COM_BC_LS_OFFSET  + 10
#define MIL_COM_LS_BC_BH_INFO         MIL_COM_BC_LS_OFFSET  + 11
#define MIL_COM_LS_BC_MAJOR_FRAME_GET MIL_COM_BC_LS_OFFSET  + 12
#define MIL_COM_LS_BC_MINOR_FRAME_GET MIL_COM_BC_LS_OFFSET  + 13
#define MIL_COM_LS_BC_DYTAG_DEF       MIL_COM_BC_LS_OFFSET  + 14
#define MIL_COM_LS_BC_DYTAG_DEF_GET   MIL_COM_BC_LS_OFFSET  + 15
#define MIL_COM_LS_BC_MINOR_FRAME_DEF MIL_COM_BC_LS_OFFSET  + 16
#define MIL_COM_LS_BC_MAJOR_FRAME_DEF MIL_COM_BC_LS_OFFSET  + 17
#define MIL_COM_LS_BC_STATUS_READ     MIL_COM_BC_LS_OFFSET  + 18
#define MIL_COM_LS_BC_XFER_READ       MIL_COM_BC_LS_OFFSET  + 19
#define MIL_COM_LS_BC_SRVW_REQ_READ   MIL_COM_BC_LS_OFFSET  + 20
#define MIL_COM_LS_BC_SRVW_REQ_CON    MIL_COM_BC_LS_OFFSET  + 21
#define MIL_COM_LS_BC_ACYC_PREP       MIL_COM_BC_LS_OFFSET  + 22
#define MIL_COM_LS_BC_ACYC_SEND       MIL_COM_BC_LS_OFFSET  + 23
#define MIL_COM_LS_BC_MODE_CTRL       MIL_COM_BC_LS_OFFSET  + 24
#define MIL_COM_LS_BC_MODIFY_DEF      MIL_COM_BC_LS_OFFSET  + 25
#define MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER      MIL_COM_BC_LS_OFFSET  + 26
#define MIL_COM_LS_BC_TRAFFIC_OVERRIDE_CONTROL        MIL_COM_BC_LS_OFFSET  + 27
#define MIL_COM_LS_BC_TRAFFIC_OVERRIDE_GET            MIL_COM_BC_LS_OFFSET  + 28





/* -- MIL_COM_LS_XFER_DESC_GET --- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulXid;
    AiUInt32    ulMode;
} TY_MIL_COM_LS_XFER_DESC_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    aulValue[4];
} TY_MIL_COM_LS_XFER_DESC_GET_OUTPUT;

/* -- MIL_COM_LS_XFER_DESC_MOD --- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulXid;
    AiUInt32    ulMode;
    AiUInt32    aulValue[4];
} TY_MIL_COM_LS_XFER_DESC_MOD_INPUT;






/* -- MIL_COM_LS_XFER_ERRROR -- */

/*! ApiCmdBCXferDefErr */
typedef struct
{
    TY_MIL_COM      xCommand;     /*!< default command structure */
    AiUInt32        ulXid;        /*!< The transfer id to modify */
    TY_API_BC_ERROR xError;       /*!< The error injection setup */
}TY_MIL_COM_LS_XFER_ERROR_INPUT;





/* -- MIL_COM_LS_BC_INI -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        retries;
    AiUInt32        svrq;
    AiUInt32        tbm;
    AiUInt32        gsb;
}TY_MIL_COM_LS_BC_INI_INPUT;



/* -- MIL_COM_LS_BC_BH_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        hid;
    AiUInt32        bid;
    AiUInt32        sid;
    AiUInt32        eid;
    AiUInt32        qsize;
    AiUInt32        bqm;
    AiUInt32        bsm;
    AiUInt32        sqm;
    AiUInt32        eqm;
    AiUInt32        dbm;
}TY_MIL_COM_LS_BC_BH_DEF_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    bid;
    AiUInt32    sid;
    AiUInt32    eid;
    AiUInt32    nbufs;
    AiUInt32    hid_addr;
    AiUInt32    bq_addr;
    AiUInt32    sq_addr;
    AiUInt32    eq_addr;
} TY_MIL_COM_LS_BC_BH_DEF_OUTPUT;




/* -- MIL_COM_LS_BC_XFER_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        hid;
    AiUInt32        xfer_type;
    AiUInt32        chn;
    AiUInt32        xmt_rt;
    AiUInt32        rcv_rt;
    AiUInt32        xmt_sa;
    AiUInt32        rcv_sa;
    AiUInt32        wcnt;
    AiUInt32        xfer_ir;
    AiUInt32        xfer_hlt;
    AiUInt32        rte;
    AiUInt32        sxh;
    AiUInt32        rsp;
    AiUInt32        res;
    AiUInt32        swxm;
    AiUInt32        err_type;
    AiUInt32        err_wpos;
    AiUInt32        err_bpos;
    AiUInt32        err_sync;
    AiUInt32        err_contig;
    AiUInt32        err_bc_bits;
    AiUInt32        delay_mode;
    AiUInt32        gap;
}TY_MIL_COM_LS_BC_XFER_DEF_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    addr;
} TY_MIL_COM_LS_BC_XFER_DEF_OUTPUT;




/* -- MIL_COM_LS_BC_XFER_CTRL -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        mode;
}TY_MIL_COM_LS_BC_XFER_CTRL_INPUT;


/* -- MIL_COM_LS_BC_START -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        smod;
    AiUInt32        cnt;
    AiUInt32        frame_time_in_ms;
    AiUInt32        saddr;
}TY_MIL_COM_LS_BC_START_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32    major_frame_addr;
    AiUInt32    minor_frame_addr[64];
} TY_MIL_COM_LS_BC_START_OUTPUT;


/* -- MIL_COM_LS_BC_XFER_DEF_GET -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
}TY_MIL_COM_LS_BC_XFER_DEF_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        xid;
    AiUInt32        hid;
    AiUInt32        xfer_type;
    AiUInt32        chn;
    AiUInt32        xmt_rt;
    AiUInt32        rcv_rt;
    AiUInt32        xmt_sa;
    AiUInt32        rcv_sa;
    AiUInt32        wcnt;
    AiUInt32        xfer_ir;
    AiUInt32        xfer_hlt;
    AiUInt32        rte;
    AiUInt32        sxh;
    AiUInt32        rsp;
    AiUInt32        res;
    AiUInt32        swxm;
    AiUInt32        err_type;
    AiUInt32        err_wpos;
    AiUInt32        err_bpos;
    AiUInt32        err_sync;
    AiUInt32        err_padding;
    AiUInt32        err_contig;
    AiUInt32        err_bc_bits;
    AiUInt32        gap_mode;
    AiUInt32        gap;
} TY_MIL_COM_LS_BC_XFER_DEF_GET_OUTPUT;




/* -- MIL_COM_LS_BC_BH_READ -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        hid;
}TY_MIL_COM_LS_BC_BH_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        bid;
    AiUInt32        sid;
    AiUInt32        eid;
    AiUInt32        nbufs;
    AiUInt32        hid_addr;
    AiUInt32        bq_addr;
    AiUInt32        sq_addr;
    AiUInt32        eq_addr;
} TY_MIL_COM_LS_BC_BH_READ_OUTPUT;


/* -- MIL_COM_LS_BC_BH_INFO -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
}TY_MIL_COM_LS_BC_BH_INFO_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        bid;
    AiUInt32        buf_addr;
} TY_MIL_COM_LS_BC_BH_INFO_OUTPUT;



/* -- MIL_COM_LS_BC_MAJOR_FRAME_GET -- */
typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        cnt;
    AiUInt32        minor_frame[64];
} TY_MIL_COM_LS_BC_MAJOR_FRAME_GET_OUTPUT;


/* -- MIL_COM_LS_BC_MINOR_FRAME_GET -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        frame_id;
}TY_MIL_COM_LS_BC_MINOR_FRAME_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        id;
    AiUInt32        cnt;
    AiUInt32        instr[128]; /*MAX_API_BC_XFRAME*/
    AiUInt32        xid[128]; /*MAX_API_BC_XFRAME*/
} TY_MIL_COM_LS_BC_MINOR_FRAME_GET_OUTPUT;



/* -- MIL_COM_LS_BC_DYTAG_DEF -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        con;
    AiUInt32        bc_hid;
    AiUInt32        mode;
    AiUInt32        tag_fct[4];
    AiUInt32        min[4];
    AiUInt32        max[4];
    AiUInt32        step[4];
    AiUInt32        wpos[4];
}TY_MIL_COM_LS_BC_DYTAG_DEF_INPUT;

/* -- MIL_COM_LS_BC_DYTAG_DEF_GET -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        bc_hid;
}TY_MIL_COM_LS_BC_DYTAG_DEF_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        mode;
    AiUInt32        tag_fct[4];
    AiUInt32        min[4];
    AiUInt32        max[4];
    AiUInt32        step[4];
    AiUInt32        wpos[4];
} TY_MIL_COM_LS_BC_DYTAG_DEF_GET_OUTPUT;

/* -- MIL_COM_LS_BC_MINOR_FRAME_DEF -- */

typedef struct
{
    AiUInt32 instr;
    AiUInt32 xid;
} TY_BC_INSTRUCTION;


#define MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT_MINIMUM_SIZE (sizeof(TY_MIL_COM) + 2 * sizeof(AiUInt32))

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32          frame_id;
    AiUInt32          xfer_cnt;
    TY_BC_INSTRUCTION data[128];
}TY_MIL_COM_LS_BC_MINOR_FRAME_DEF_INPUT;



/* -- MIL_COM_LS_BC_MAJOR_FRAME_DEF -- */

#define MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT_MINIMUM_SIZE (sizeof(TY_MIL_COM) + sizeof(AiUInt32))

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        frame_cnt;
    AiUInt32        frame_ids[512];
}TY_MIL_COM_LS_BC_MAJOR_FRAME_DEF_INPUT;






/* -- MIL_COM_LS_BC_STATUS_READ -- */

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        status;
    AiUInt32        hxfer;
    AiUInt32        bc_mc;
    AiUInt32        bc_ec;
    AiUInt32        mfc;
    AiUInt32        hip;
} TY_MIL_COM_LS_BC_STATUS_READ_OUTPUT;



/* -- MIL_COM_LS_BC_XFER_READ -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        clr;
}TY_MIL_COM_LS_BC_XFER_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32 cw1;
    AiUInt32 st1;
    AiUInt32 cw2;
    AiUInt32 st2;
    AiUInt32 bid;
    AiUInt32 brw;
    AiUInt32 bufp;
    AiUInt32 ttag;
    AiUInt32 msg_cnt;
    AiUInt32 err_cnt;
    AiUInt32 lvw;
    AiUInt32 srvreq_cnt;
} TY_MIL_COM_LS_BC_XFER_READ_OUTPUT;


/* -- MIL_COM_LS_BC_SRVW_REQ_READ -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        rt;
}TY_MIL_COM_LS_BC_SRVW_REQ_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32 xid;
    AiUInt32 lvw;
    AiUInt32 src;
} TY_MIL_COM_LS_BC_SRVW_REQ_READ_OUTPUT;


/* -- MIL_COM_LS_BC_SRVW_REQ_CON -- */
typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        rt;
    AiUInt32        con;
}TY_MIL_COM_LS_BC_SRVW_REQ_CON_INPUT;

/* -- MIL_COM_LS_BC_ACYC_PREP -- */
#define MIL_COM_LS_BC_ACYC_PREP_INPUT_MINIMUM_SIZE (sizeof(TY_MIL_COM) + sizeof(AiUInt32))

typedef struct
{
    TY_MIL_COM        cmd;
    AiUInt32          xfer_cnt;
    TY_BC_INSTRUCTION data[127];
}TY_MIL_COM_LS_BC_ACYC_PREP_INPUT;


/* -- MIL_COM_LS_BC_ACYC_SEND -- */
typedef struct
{
    TY_MIL_COM        cmd;
    AiUInt32          mode;
    AiUInt32          ttag_high;
    AiUInt32          ttag_low;
}TY_MIL_COM_LS_BC_ACYC_SEND_INPUT;

/* -- MIL_COM_LS_BC_MODE_CTRL -- */
typedef struct
{
    TY_MIL_COM        cmd;
    AiUInt32          mode;
    AiUInt32          ctrl;
    AiUInt32          param1;
    AiUInt32          param2;
    AiUInt32          param3;
}TY_MIL_COM_LS_BC_MODE_CTRL_INPUT;

/* -- MIL_COM_LS_BC_MODIFY_DEF -- */
typedef struct
{
    TY_MIL_COM                   cmd;
    AiUInt32                     id;
    TY_API_BC_MODIFY_INSTRUCTION modify;
}TY_MIL_COM_LS_BC_MODIFY_DEF_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        offset;
} TY_MIL_COM_LS_BC_MODIFY_DEF_OUTPUT;


/* -- MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER -- */
typedef struct
{
    TY_MIL_COM                       cmd;
    TY_MIL_COM_LS_BC_XFER_DEF_INPUT  bc_xfer_def;
    TY_MIL_COM_LS_BUF_DEF_INPUT      buf_def;
} MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    TY_MIL_COM_LS_BC_XFER_READ_OUTPUT bc_xfer_read;
    TY_MIL_COM_LS_BUF_READ_OUTPUT     buf_read;

} MIL_COM_LS_BC_ACYC_PREP_AND_SEND_TRANSFER_OUTPUT;


#endif /* __MIL_COM_LS_BC_H__ */
