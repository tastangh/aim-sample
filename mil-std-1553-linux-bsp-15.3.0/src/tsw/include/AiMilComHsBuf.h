/* SPDX-FileCopyrightText: 2011-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComHsBm.h
*/

#ifndef __MIL_COM_HS_BUF_H__
#define __MIL_COM_HS_BUF_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"


#define MIL_COM_HS_BUF_DEF             MIL_COM_BUF_HS_OFFSET + 0
#define MIL_COM_HS_BUF_READ            MIL_COM_BUF_HS_OFFSET + 1
#define MIL_COM_HS_BUF_WRITE           MIL_COM_BUF_HS_OFFSET + 2
#define MIL_COM_HS_BUF_WRITE_BLOCK     MIL_COM_BUF_HS_OFFSET + 3
#define MIL_COM_HS_BUF_HOST_CONTROL    MIL_COM_BUF_HS_OFFSET + 4

/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/

/* MIL_COM_HS_BUF_DEF */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     alloc_mode;
    AiUInt32     buf_type;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     size;
    AiUInt32     start_index;
    AiUInt32     write_size;
    AiUInt16     data[1];
} TY_MIL_COM_HS_BUF_DEF_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 alloc_buf_nr;
    AiUInt32 alloc_info;
    AiUInt32 buf_addr;
} TY_MIL_COM_HS_BUF_DEF_OUTPUT;






/* MIL_COM_HS_BUF_READ */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     buf_type;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     read_start_index;
    AiUInt32     size;
} TY_MIL_COM_HS_BUF_READ_INPUT;



typedef struct
{
    TY_MIL_COM_ACK xAck;
    AiUInt32 addr;
    AiUInt32 queue_size;
    AiUInt16 buffer[1];
} TY_MIL_COM_HS_BUF_READ_OUTPUT;






/* MIL_COM_HS_BUF_WRITE */

typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     buf_type;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     data_pos;
    AiUInt32     data_word_mask;
    AiUInt32     data_word;
} TY_MIL_COM_HS_BUF_WRITE_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     bid;
    AiUInt32     offset;
    AiUInt32     count;
    AiUInt16     data[1];
} TY_MIL_COM_HS_BUF_WRITE_BLOCK_INPUT;


typedef struct
{
    TY_MIL_COM   cmd;
    AiUInt32     bt;
    AiUInt32     hid;
    AiUInt32     con;
} TY_MIL_COM_HS_BUF_HOST_CONTROL_INPUT;


#endif /* __MIL_COM_HS_BC_H__ */
