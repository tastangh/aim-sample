/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComEfBc.h
*/

#ifndef __MIL_COM_EF_BC_H__
#define __MIL_COM_EF_BC_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


#define MIL_COM_EF_BC_RESP_SIM_DEF       MIL_COM_BC_EF_OFFSET  + 0
#define MIL_COM_EF_BC_INTERNAL_DEF       MIL_COM_BC_EF_OFFSET  + 1



/* -- MIL_COM_EF_BC_RESP_SIM_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        hid;
    AiUInt32        sd_bus;
    AiUInt32        sd_err_word_pos;
    AiUInt32        sd_err_type;
    AiUInt32        sd_bit_cnt_or_pos;
    AiUInt32        sd_response_time;
    AiUInt32        sd_fcpa;
    AiUInt32        sd_dest_addr;
    AiUInt32        sd_wordcount;
    AiUInt32        s_bus;
    AiUInt32        s_err_word_pos;
    AiUInt32        s_err_type;
    AiUInt32        s_bit_cnt_or_pos;
    AiUInt32        s_response_time;
    AiUInt32        s_fcpa;
    AiUInt32        s_adw;
}TY_MIL_COM_EF_BC_RESP_SIM_DEF_INPUT;


/* -- MIL_COM_EF_BC_INTERNAL_DEF -- */

typedef struct
{
    TY_MIL_COM      cmd;
    AiUInt32        xid;
    AiUInt32        hid;
    AiUInt32        bc_fpca;
    AiUInt32        bc_dest_addr;
    AiUInt32        bc_wordcount;
    AiUInt32        bc_modecode_words;
}TY_MIL_COM_EF_BC_INTERNAL_DEF_INPUT;

#endif /* __MIL_COM_EF_BC_H__ */
