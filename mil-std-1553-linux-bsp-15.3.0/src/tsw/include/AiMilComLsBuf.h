/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComLsRt.h

*/

#ifndef __MIL_COM_LS_BUF_H__
#define __MIL_COM_LS_BUF_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


/*****************************************************************************/
/*                                                                           */
/*                              command definitions                          */
/*                                                                           */
/*****************************************************************************/

#define MIL_COM_LS_BUF_DEF            MIL_COM_BUF_LS_OFFSET +  0
#define MIL_COM_LS_BUF_READ           MIL_COM_BUF_LS_OFFSET +  1
#define MIL_COM_LS_BUF_WRITE          MIL_COM_BUF_LS_OFFSET +  2
#define MIL_COM_LS_BUF_C1760_CON      MIL_COM_BUF_LS_OFFSET +  3
#define MIL_COM_LS_BUF_DATASET_WRITE  MIL_COM_BUF_LS_OFFSET +  4
#define MIL_COM_LS_BUF_DATASET_READ   MIL_COM_BUF_LS_OFFSET +  5
#define MIL_COM_LS_BUF_WRITE_BLOCK    MIL_COM_BUF_LS_OFFSET +  6
#define MIL_COM_LS_BUF_HOST_CONTROL   MIL_COM_BUF_LS_OFFSET +  7




/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     count;
    AiUInt16     data[32];
} TY_MIL_COM_LS_BUF_DEF_INPUT;



 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       bid;
    AiUInt32       address;
} TY_MIL_COM_LS_BUF_DEF_OUTPUT;




typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     count;
} TY_MIL_COM_LS_BUF_READ_INPUT;


typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32       bid;
    AiUInt32       address;
    AiUInt32       count;
    AiUInt16       data[32];
} TY_MIL_COM_LS_BUF_READ_OUTPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     word_pos;
    AiUInt32     bit_pos;
    AiUInt32     bit_len;
    AiUInt32     data;
} TY_MIL_COM_LS_BUF_WRITE_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     offset;
    AiUInt32     count;
    AiUInt16     data[32];
} TY_MIL_COM_LS_BUF_WRITE_BLOCK_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     con;
} TY_MIL_COM_LS_BUF_HOST_CONTROL_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     mode;
    AiUInt32     buf_id;
    AiUInt32     c01;
    AiUInt8      c02[16];
} TY_MIL_COM_LS_BUF_C1760_CON_INPUT;





typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     dsid;
    AiUInt16     data[32];
} TY_MIL_COM_LS_BUF_DATASET_WRITE_INPUT;





typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     dsid;
} TY_MIL_COM_LS_BUF_DATASET_READ_INPUT;



 
typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt16       data[32];
} TY_MIL_COM_LS_BUF_DATASET_READ_OUTPUT;
 

#endif /* __MIL_COM_LS_BUF_H__ */
