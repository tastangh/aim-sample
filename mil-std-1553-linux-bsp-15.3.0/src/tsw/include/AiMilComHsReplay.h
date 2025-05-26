/* SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComReplay.h
*/

#ifndef __MIL_COM_REP_HS_H__
#define __MIL_COM_REP_HS_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"



/* -- Generic SYS command --- */
#define MIL_COM_HS_REP_INI              MIL_COM_REP_HS_OFFSET +  0
#define MIL_COM_HS_REP_RT_CON           MIL_COM_REP_HS_OFFSET +  1
#define MIL_COM_HS_REP_STATUS           MIL_COM_REP_HS_OFFSET +  2




/*****************************************************************************/
/*                                                                           */
/*                               HS REP                                      */
/*                                                                           */
/*****************************************************************************/


/* -- MIL_COM_HS_REP_INI --- */

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    hs_msg_count;
    AiUInt32    nct;
    AiUInt32    cet;
    AiUInt32    rint;
} TY_MIL_COM_HS_REP_INI_INPUT;

/* -- MIL_COM_HS_REP_RT_CON --- */

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    rt;
    AiUInt32    mode;
    AiUInt32    con;
} TY_MIL_COM_HS_REP_RT_CON_INPUT;

/* -- MIL_COM_HS_REP_STATUS --- */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       rpi_cnt;
    AiUInt32       saddr;
    AiUInt32       size;
    AiUInt32       raec;
    AiUInt32       status;
} TY_MIL_COM_HS_REP_STATUS_OUTPUT;










#endif /* __MIL_COM_REP_HS_H__ */
