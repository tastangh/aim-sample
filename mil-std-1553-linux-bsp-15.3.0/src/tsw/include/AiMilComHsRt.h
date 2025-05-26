/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsRt.h

*/

#ifndef __MIL_COM_HS_RT_H__
#define __MIL_COM_HS_RT_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


/*****************************************************************************/
/*                                                                           */
/*                              command definitions                          */
/*                                                                           */
/*****************************************************************************/

#define MIL_COM_HS_RT_BH_DEF           MIL_COM_RT_HS_OFFSET +  0
#define MIL_COM_HS_RT_BH_GET           MIL_COM_RT_HS_OFFSET +  1
#define MIL_COM_HS_RT_MID_DEF          MIL_COM_RT_HS_OFFSET +  2
#define MIL_COM_HS_RT_MID_ERR_DEF      MIL_COM_RT_HS_OFFSET +  3
#define MIL_COM_HS_RT_MID_ERR_GET      MIL_COM_RT_HS_OFFSET +  4
#define MIL_COM_HS_RT_DYTAG_DEF        MIL_COM_RT_HS_OFFSET +  5
#define MIL_COM_HS_RT_DYTAG_GET        MIL_COM_RT_HS_OFFSET +  6
#define MIL_COM_HS_RT_STATUS_GET       MIL_COM_RT_HS_OFFSET +  7
#define MIL_COM_HS_RT_MSG_GET          MIL_COM_RT_HS_OFFSET +  8
#define MIL_COM_HS_RT_MID_BH_GET       MIL_COM_RT_HS_OFFSET +  9
#define MIL_COM_HS_RT_INI_START_DELAY  MIL_COM_RT_HS_OFFSET + 10
#define MIL_COM_HS_RT_INI_PREAMBLES    MIL_COM_RT_HS_OFFSET + 11
#define MIL_COM_HS_RT_INI_LSW          MIL_COM_RT_HS_OFFSET + 12
#define MIL_COM_HS_RT_INI_LAW          MIL_COM_RT_HS_OFFSET + 13
#define MIL_COM_HS_RT_MODE_CONTROL     MIL_COM_RT_HS_OFFSET + 14


/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/



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
    AiUInt32   dbm; 
    AiUInt32   wcm;
} TY_MIL_COM_HS_RT_BH_DEF_INPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   hid;
} TY_MIL_COM_HS_RT_BH_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       nbufs;
    AiUInt32       sid;
    AiUInt32       eid;
    AiUInt32       mem_available;
    AiUInt32       hid_addr;
    AiUInt32       bid_addr;
    AiUInt32       sid_addr;
    AiUInt32       eid_addr;

} TY_MIL_COM_HS_RT_BH_GET_OUTPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   mid_mc;
    AiUInt32   hid;
    AiUInt32   mid_type;
    AiUInt32   mid_con;
    AiUInt32   int_mid_con;
} TY_MIL_COM_HS_RT_MID_DEF_INPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   mid;
    AiUInt32   hs_err_type;
    AiUInt32   bit_pos;
    AiUInt32   bit_count;
    AiUInt32   wpos;
} TY_MIL_COM_HS_RT_MID_ERR_DEF_INPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   mid;
} TY_MIL_COM_HS_RT_MID_ERR_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32   hs_err_type;
    AiUInt32   bit_pos;
    AiUInt32   bit_count;
    AiUInt32   wpos;
} TY_MIL_COM_HS_RT_MID_ERR_GET_OUTPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   con;
    AiUInt32   hid;
    AiUInt32   fct_word[4];
    AiUInt32   mode[4];
    AiUInt32   wpos[4];
    AiUInt32   dmm[4];
} TY_MIL_COM_HS_RT_DYTAG_DEF_INPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   hid;
} TY_MIL_COM_HS_RT_DYTAG_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32   fct_word[4];
    AiUInt32   mode[4];
    AiUInt32   wpos[4];
    AiUInt32   dmm[4];
} TY_MIL_COM_HS_RT_DYTAG_GET_OUTPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
} TY_MIL_COM_HS_RT_STATUS_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       msg_cnt;
    AiUInt32       err_cnt;
    AiUInt32       last_sw;
    AiUInt32       last_aw;
} TY_MIL_COM_HS_RT_STATUS_GET_OUTPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   mid;
    AiUInt32   type;
    AiUInt32   clear;
} TY_MIL_COM_HS_RT_MSG_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       sq_ctrl;
    AiUInt32       da;
    AiUInt32       fc_pa;
    AiUInt32       wcnt;
    AiUInt32       msg_tag;
    AiUInt32       action_word;
    AiUInt32       data_buffer;
    AiUInt32       time_tag;
} TY_MIL_COM_HS_RT_MSG_GET_OUTPUT;







typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   type;
    AiUInt32   mc;
} TY_MIL_COM_HS_RT_MID_BH_GET_INPUT;




typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       bid;
    AiUInt32       buf_addr;
} TY_MIL_COM_HS_RT_MID_BH_GET_OUTPUT;







typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   mode;
    AiUInt32   delay;
} TY_MIL_COM_HS_RT_INI_START_DELAY_INPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   count;
} TY_MIL_COM_HS_RT_INI_PREAMBLES_INPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   lsw;
} TY_MIL_COM_HS_RT_INI_LSW_INPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_addr;
    AiUInt32   law;
} TY_MIL_COM_HS_RT_INI_LAW_INPUT;






typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   rt_mode;
    AiUInt32   control;
    AiUInt32   param[3];
} TY_MIL_COM_HS_RT_MODE_CONTROL_INPUT;






#endif /* __MIL_COM_HS_RT_H__ */
