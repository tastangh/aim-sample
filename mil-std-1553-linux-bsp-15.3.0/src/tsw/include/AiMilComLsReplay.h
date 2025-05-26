/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComReplay.h
*/

#ifndef __MIL_COM_REP_H__
#define __MIL_COM_REP_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"



/* -- Generic SYS command --- */
#define MIL_COM_REP_INI                MIL_COM_REP_OFFSET +  0
#define MIL_COM_REP_START              MIL_COM_REP_OFFSET +  1
#define MIL_COM_REP_HALT               MIL_COM_REP_OFFSET +  2
#define MIL_COM_REP_RT_CON             MIL_COM_REP_OFFSET +  3
#define MIL_COM_REP_STATUS             MIL_COM_REP_OFFSET +  4




/*****************************************************************************/
/*                                                                           */
/*                                  REP                                      */
/*                                                                           */
/*****************************************************************************/


/* -- MIL_COM_REP_INI --- */

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    cet;
    AiUInt32    nct;
    AiUInt32    cyc;
    AiUInt32    nimg;
    AiUInt32    alt;
    AiUInt32    rlt;
    AiUInt32    rint;
    AiUInt32    min;
    AiUInt32    msec;
    AiUInt32    sign;
    AiUInt32    fsize;
} TY_MIL_COM_REP_INI_INPUT;


/* -- MIL_COM_REP_RT_CON --- */

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    rt;
    AiUInt32    mode;
    AiUInt32    con;
} TY_MIL_COM_REP_RT_CON_INPUT;



/* -- MIL_COM_REP_STATUS --- */

typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       rpi_cnt;
    AiUInt32       saddr;
    AiUInt32       size;
    AiUInt32       raec;
    AiUInt32       status;
} TY_MIL_COM_REP_STATUS_OUTPUT;




#endif /* __MIL_COM_REP_H__ */
