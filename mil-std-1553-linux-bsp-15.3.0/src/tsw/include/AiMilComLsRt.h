/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsRt.h

*/

#ifndef __MIL_COM_LS_RT_H__
#define __MIL_COM_LS_RT_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


/*****************************************************************************/
/*                                                                           */
/*                              command definitions                          */
/*                                                                           */
/*****************************************************************************/

#define MIL_COM_LS_RT_INI          MIL_COM_RT_LS_OFFSET +  0
#define MIL_COM_LS_RT_CON          MIL_COM_RT_LS_OFFSET +  1
#define MIL_COM_LS_RT_SA_CON       MIL_COM_RT_LS_OFFSET +  2
#define MIL_COM_LS_RT_INI_RESP     MIL_COM_RT_LS_OFFSET +  3
#define MIL_COM_LS_RT_INI_NXW      MIL_COM_RT_LS_OFFSET +  4
#define MIL_COM_LS_RT_INI_LCW      MIL_COM_RT_LS_OFFSET +  5
#define MIL_COM_LS_RT_INI_LSW      MIL_COM_RT_LS_OFFSET +  6
#define MIL_COM_LS_RT_BH_DEF       MIL_COM_RT_LS_OFFSET +  7
#define MIL_COM_LS_RT_BH_GET       MIL_COM_RT_LS_OFFSET +  8
#define MIL_COM_LS_RT_SA_ERR_CON   MIL_COM_RT_LS_OFFSET +  9
#define MIL_COM_LS_RT_SA_ERR_GET   MIL_COM_RT_LS_OFFSET + 10
#define MIL_COM_LS_RT_DYTAG_DEF    MIL_COM_RT_LS_OFFSET + 11
#define MIL_COM_LS_RT_DYTAG_GET    MIL_COM_RT_LS_OFFSET + 12
#define MIL_COM_LS_RT_START        MIL_COM_RT_LS_OFFSET + 13
#define MIL_COM_LS_RT_HALT         MIL_COM_RT_LS_OFFSET + 14
#define MIL_COM_LS_RT_STATUS_READ  MIL_COM_RT_LS_OFFSET + 15
#define MIL_COM_LS_RT_MSG_READ     MIL_COM_RT_LS_OFFSET + 16
#define MIL_COM_LS_RT_MSG_READ_ALL MIL_COM_RT_LS_OFFSET + 17
#define MIL_COM_LS_RT_SA_MSG_READ  MIL_COM_RT_LS_OFFSET + 18
#define MIL_COM_LS_RT_GLOBAL_CON   MIL_COM_RT_LS_OFFSET + 19
#define MIL_COM_LS_RT_GET_SIM_INFO MIL_COM_RT_LS_OFFSET + 20
#define MIL_COM_LS_RT_SA_BH_GET    MIL_COM_RT_LS_OFFSET + 21
#define MIL_COM_LS_RT_MODE_CTRL    MIL_COM_RT_LS_OFFSET + 22
#define MIL_COM_LS_RT_DWC_SET      MIL_COM_RT_LS_OFFSET + 23
#define MIL_COM_LS_RT_DWC_GET      MIL_COM_RT_LS_OFFSET + 24



/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/



typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     con;
    AiUInt32     bus;
    AiUInt32     resp_time;
    AiUInt32     nxw;
} TY_MIL_COM_LS_RT_INI_INPUT;




typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     con;
} TY_MIL_COM_LS_RT_CON_INPUT;




typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     sa_mc; 
    AiUInt32     sa_hid;
    AiUInt32     sa_type; 
    AiUInt32     con;
    AiUInt32     rmod;
    AiUInt32     smod;
    AiUInt32     swm;
} TY_MIL_COM_LS_RT_SA_CON_INPUT;



typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     resp_time;
} TY_MIL_COM_LS_RT_INI_RESP_INPUT;



typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     nxw;
} TY_MIL_COM_LS_RT_INI_NXW_INPUT;



typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     lcw;
} TY_MIL_COM_LS_RT_INI_LCW_INPUT;



typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     lsw;
} TY_MIL_COM_LS_RT_INI_LSW_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   hid;
    AiUInt32   bid;
    AiUInt32   sid;
    AiUInt32   eid;
    AiUInt32   qsize;
    AiUInt32   bqm;
    AiUInt32   bsm;
    AiUInt32   sqm;
    AiUInt32   eqm;
    AiUInt32   res;
} TY_MIL_COM_LS_RT_BH_DEF_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       bid;
    AiUInt32       sid;
    AiUInt32       eid;
    AiUInt32       nbufs;
    AiUInt32       hid_addr;
    AiUInt32       bid_addr;
    AiUInt32       sid_addr;
    AiUInt32       eid_addr;
} TY_MIL_COM_LS_RT_BH_DEF_OUTPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   hid;
} TY_MIL_COM_LS_RT_BH_GET_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   sa_mc;
    AiUInt32   sa_type;
    AiUInt32   type;
    AiUInt32   wpos;
    AiUInt32   bpos;
    AiUInt32   sync;
    AiUInt32   contig;
    AiUInt32   bc_bits;
} TY_MIL_COM_LS_RT_SA_ERR_CON_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   sa_mc;
    AiUInt32   sa_type;
} TY_MIL_COM_LS_RT_SA_ERR_GET_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       type;
    AiUInt32       wpos;
    AiUInt32       bpos;
    AiUInt32       sync;
    AiUInt32       contig;
    AiUInt32       bc_bits;
} TY_MIL_COM_LS_RT_SA_ERR_GET_OUTPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_hid;
    AiUInt32   con;
    AiUInt32   mode;
    AiUInt32   tag_fct[4];
    AiUInt32   min[4];
    AiUInt32   max[4];
    AiUInt32   step[4];
    AiUInt32   wpos[4];
} TY_MIL_COM_LS_RT_DYTAG_DEF_INPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_hid;
} TY_MIL_COM_LS_RT_DYTAG_GET_INPUT;

 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       mode;
    AiUInt32       tag_fct[4];
    AiUInt32       min[4];
    AiUInt32       max[4];
    AiUInt32       step[4];
    AiUInt32       wpos[4];
} TY_MIL_COM_LS_RT_DYTAG_GET_OUTPUT;



 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       msg_cnt;
    AiUInt32       err_cnt;
    AiUInt32       dsp;
} TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
} TY_MIL_COM_LS_RT_MSG_READ_INPUT;



 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       msg_cnt;
    AiUInt32       err_cnt;
    AiUInt32       nxw;
    AiUInt32       lsw;
    AiUInt32       lcw;
} TY_MIL_COM_LS_RT_MSG_READ_OUTPUT;




 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       msg_cnt[32]; /* 32 RTs */
    AiUInt32       err_cnt[32]; /* 32 RTs */
} TY_MIL_COM_LS_RT_MSG_READ_ALL_OUTPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   sa_mc;
    AiUInt32   sa_type;
    AiUInt32   clr_mode;
} TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       bid;
    AiUInt32       brw;
    AiUInt32       lcw;
    AiUInt32       lsw;
    AiUInt32       buf;
    AiUInt32       ttag;
} TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   count;
} TY_MIL_COM_LS_RT_GLOBAL_CON_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32 rt;
} TY_MIL_COM_LS_RT_GET_SIM_INFO_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 mode;
    AiUInt32 rx_sa;
    AiUInt32 tx_sa;
    AiUInt32 rx_mc;
    AiUInt32 tx_mc;
    AiUInt32 hs_mc;
    AiUInt32 rx_mid[8];
    AiUInt32 tx_mid[8];
} TY_MIL_COM_LS_RT_GET_SIM_INFO_OUTPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt;
    AiUInt32   sa_type;
    AiUInt32   sa_mc;
} TY_MIL_COM_LS_RT_SA_BH_GET_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       hid;
    AiUInt32       hid_addr;
} TY_MIL_COM_LS_RT_SA_BH_GET_OUTPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_mode;
    AiUInt32   rt_ctrl;
    AiUInt32   param1;
    AiUInt32   param2;
    AiUInt32   param3;
} TY_MIL_COM_LS_RT_MODE_CTRL_INPUT;




typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     sa_mc;
    AiUInt32     sa_type;
    AiUInt32     word_cnt;
} TY_MIL_COM_LS_RT_DWC_SET_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     rt_addr;
    AiUInt32     sa_mc;
    AiUInt32     sa_type;
} TY_MIL_COM_LS_RT_DWC_GET_INPUT;

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       word_cnt;
} TY_MIL_COM_LS_RT_DWC_GET_OUTPUT;

#endif /* __MIL_COM_LS_RT_H__ */
