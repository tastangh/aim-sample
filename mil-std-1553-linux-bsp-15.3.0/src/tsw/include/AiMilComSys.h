/* SPDX-FileCopyrightText: 2011-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilComSys.h
*/

#ifndef __MIL_COM_SYS_H__
#define __MIL_COM_SYS_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"



/* -- Generic SYS command --- */
#define MIL_COM_SYS_DUMMY                      MIL_COM_SYS_OFFSET +  0
#define MIL_COM_SYS_PING                       MIL_COM_SYS_OFFSET +  1
#define MIL_COM_SYS_DMA_SETUP_SGL              MIL_COM_SYS_OFFSET +  2
#define MIL_COM_SYS_MALLOC                     MIL_COM_SYS_OFFSET +  3
#define MIL_COM_SYS_FREE                       MIL_COM_SYS_OFFSET +  4
#define MIL_COM_SYS_GET_BOARD_INFO             MIL_COM_SYS_OFFSET +  5
#define MIL_COM_SYS_GET_MEM_LOCATION           MIL_COM_SYS_OFFSET +  6
#define MIL_COM_SYS_MEM_PART_SET               MIL_COM_SYS_OFFSET +  7
#define MIL_COM_SYS_MEM_PART_GET               MIL_COM_SYS_OFFSET +  8
#define MIL_COM_SYS_SYSTAG_CON                 MIL_COM_SYS_OFFSET +  9
#define MIL_COM_SYS_SYSTAG_DEF                 MIL_COM_SYS_OFFSET + 10
#define MIL_COM_SYS_MILBUS_PROT                MIL_COM_SYS_OFFSET + 11
#define MIL_COM_SYS_RESP_TOUT_DEF              MIL_COM_SYS_OFFSET + 12
#define MIL_COM_SYS_PXI_CON                    MIL_COM_SYS_OFFSET + 13
#define MIL_COM_SYS_DISCRETES_READ             MIL_COM_SYS_OFFSET + 14
#define MIL_COM_SYS_DISCRETES_WRITE            MIL_COM_SYS_OFFSET + 15
#define MIL_COM_SYS_DISCRETES_INIT             MIL_COM_SYS_OFFSET + 16
#define MIL_COM_SYS_SYNC_COUNTER_SET           MIL_COM_SYS_OFFSET + 17
#define MIL_COM_SYS_SYNC_COUNTER_GET           MIL_COM_SYS_OFFSET + 18
#define MIL_COM_SYS_IRIG_GET                   MIL_COM_SYS_OFFSET + 19
#define MIL_COM_SYS_IRIG_STATUS_GET            MIL_COM_SYS_OFFSET + 20
#define MIL_COM_SYS_IRIG_SET                   MIL_COM_SYS_OFFSET + 21
#define MIL_COM_SYS_IRIG_SOURCE_SET            MIL_COM_SYS_OFFSET + 22
#define MIL_COM_SYS_RESET                      MIL_COM_SYS_OFFSET + 23
#define MIL_COM_SYS_RESET_INFO                 MIL_COM_SYS_OFFSET + 24
#define MIL_COM_SYS_DISCRETES_REC_INIT         MIL_COM_SYS_OFFSET + 25
#define MIL_COM_SYS_DISCRETES_CONFIG_READ      MIL_COM_SYS_OFFSET + 26
#define MIL_COM_SYS_TRIGGER_EDGE_INPUT_SET     MIL_COM_SYS_OFFSET + 27
#define MIL_COM_SYS_TRIGGER_EDGE_INPUT_GET     MIL_COM_SYS_OFFSET + 28
#define MIL_COM_SYS_DISCRETES_INFO             MIL_COM_SYS_OFFSET + 29
#define MIL_COM_SYS_PXI_GEO_ADDR               MIL_COM_SYS_OFFSET + 30
#define MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_CON   MIL_COM_SYS_OFFSET + 31
#define MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_GET   MIL_COM_SYS_OFFSET + 32


/* -- Generic HS SYS command --- */
#define MIL_COM_HS_SYS_SYSTAG_CON      MIL_COM_SYS_HS_OFFSET +  0
#define MIL_COM_HS_SYS_SYSTAG_DEF      MIL_COM_SYS_HS_OFFSET +  1
#define MIL_COM_HS_SYS_RX_TIMEOUT      MIL_COM_SYS_HS_OFFSET +  2



/* -- CAL LS SYS commands --- */
#define MIL_COM_LS_SYS_CAL_SIG_CON      MIL_COM_SYS_CAL_OFFSET + 0
#define MIL_COM_LS_SYS_CAL_CPL_CON      MIL_COM_SYS_CAL_OFFSET + 1
#define MIL_COM_LS_SYS_CAL_XMT_CON      MIL_COM_SYS_CAL_OFFSET + 2
#define MIL_COM_LS_SYS_CAL_TRANS        MIL_COM_SYS_CAL_OFFSET + 3


/* -- CAL HS SYS commands --- */
#define MIL_COM_HS_SYS_CAL_CON            MIL_COM_SYS_HS_CAL_OFFSET + 0
#define MIL_COM_HS_SYS_CAL_CPL_CON        MIL_COM_SYS_HS_CAL_OFFSET + 1
#define MIL_COM_HS_SYS_CAL_FOP_CON        MIL_COM_SYS_HS_CAL_OFFSET + 2
#define MIL_COM_HS_SYS_CAL_FOFE_DISCRETE  MIL_COM_SYS_HS_CAL_OFFSET + 3


/* -- LS TRACK commands --- */
#define MIL_COM_LS_SYS_TRACK_DEF          MIL_COM_SYS_TRACK_OFFSET + 0
#define MIL_COM_LS_SYS_TRACK_READ         MIL_COM_SYS_TRACK_OFFSET + 1
#define MIL_COM_LS_SYS_TRACK_SCAN         MIL_COM_SYS_TRACK_OFFSET + 2
#define MIL_COM_LS_SYS_TRACK_ALLOC        MIL_COM_SYS_TRACK_OFFSET + 3

/* -- HS TRACK commands --- */
#define MIL_COM_HS_SYS_TRACK_DEF          MIL_COM_SYS_HS_TRACK_OFFSET + 0
#define MIL_COM_HS_SYS_TRACK_READ         MIL_COM_SYS_HS_TRACK_OFFSET + 1
#define MIL_COM_HS_SYS_TRACK_SCAN         MIL_COM_SYS_HS_TRACK_OFFSET + 2
#define MIL_COM_HS_SYS_TRACK_ALLOC        MIL_COM_SYS_HS_TRACK_OFFSET + 3

/* -- FIFO commands --- */
#define MIL_COM_SYS_FIFO_INI              MIL_COM_SYS_FIFO_OFFSET + 0
#define MIL_COM_SYS_FIFO_WRITE            MIL_COM_SYS_FIFO_OFFSET + 1
#define MIL_COM_SYS_FIFO_STATUS           MIL_COM_SYS_FIFO_OFFSET + 2
#define MIL_COM_SYS_FIFO_RT_ASSIGN        MIL_COM_SYS_FIFO_OFFSET + 3
#define MIL_COM_SYS_FIFO_BC_ASSIGN        MIL_COM_SYS_FIFO_OFFSET + 4


/* -- Utility commands --- */
#define MIL_COM_SYS_UTIL                  MIL_COM_SYS_UTIL_OFFSET +  0


/* -- Version commands --- */
#define MIL_COM_READ_VERSION              MIL_COM_SYS_VERSIONS_OFFSET +  0


/* -- Exec sys commands --- */
#define MIL_COM_SYS_EXEC                  MIL_COM_SYS_EXEC_OFFSET +  0


/* -- Bite sys commands --- */
#define MIL_COM_SYS_BITE                  MIL_COM_SYS_BITE_OFFSET +  0


/* -- Bite sys commands --- */
#define MIL_COM_SYS_INI                   MIL_COM_SYS_INI_OFFSET +  0



/*****************************************************************************/
/*                                                                           */
/*                                  SYS                                      */
/*                                                                           */
/*****************************************************************************/


/* -- MIL_COM_SYS_PING --- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulDataCount;
    AiUInt32    aulData[1];
} TY_MIL_COM_SYS_PING_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        ulDataCount;
    AiUInt32        aulData[1];
} TY_MIL_COM_SYS_PING_OUTPUT;


/* -- MIL_COM_SYS_MALLOC --- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulMemory;
    AiUInt32    ulSize;
    AiUInt32    ulTag;
    AiUInt32    ulReserved;
} TY_MIL_COM_SYS_MALLOC_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        ulOffset;
} TY_MIL_COM_SYS_MALLOC_OUTPUT;



/* -- MIL_COM_FREE --- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulMemory;
    AiUInt32    ulOffset;
    AiUInt32    ulTag;
    AiUInt32    ulReserved;
} TY_MIL_COM_SYS_FREE_INPUT;





/* -- MIL_COM_SYS_DMA_SETUP_SGL --- */

typedef struct
{
    AiUInt32             ulPciAddressLow;
    AiUInt32             ulPciAddressHigh;
    AiUInt32             ulLength;
    AiUInt32             ulReserved;
} TY_MIL_COM_DMA_SETUP_SGL_INPUT_ENTRY;



typedef struct
{
    TY_MIL_COM    xCommand;
    
    AiUInt32      ulOffset;
    AiUInt32      ulMemory;
    AiUInt32      ulCopySize;
    AiUInt32      ulDirection;

    AiUInt32      ulReserved1;
    AiUInt32      ulReserved2;
    AiUInt32      ulReserved3;
    AiUInt32      ulReserved4;

    AiUInt32      ulSglEntries;

    TY_MIL_COM_DMA_SETUP_SGL_INPUT_ENTRY xEntry[1];

} TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT;
  


/* -- MIL_COM_GET_BOARD_INFO -- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulDataStart;
    AiUInt32    ulDataCount;
} TY_MIL_COM_SYS_GET_INFO_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        ulDataCount;
    AiUInt32        aulData[1];
} TY_MIL_COM_SYS_GET_INFO_OUTPUT;




/* -- MIL_COM_GET_MEM_LOCATION -- */

typedef struct
{
    TY_MIL_COM  xCommand;
    AiUInt32    ulMode;
    AiUInt32    ulId;
} TY_MIL_COM_SYS_GET_MEM_LOCATION_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        ulMemType;
    AiUInt32        ulOffset;
} TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT;





 
typedef struct
{
    TY_MIL_COM       xCommand;
    TY_E_VERSION_ID  eId;
} TY_MIL_COM_VERSION_INPUT;


 
typedef struct
{
    TY_MIL_COM_ACK          xAck;
    TY_API_VER_INFO_VALUES  xVersionValues;
    TY_API_VER_INFO_STRINGS xVersionStrings;
} TY_MIL_COM_VERSION_OUTPUT;





typedef struct
{
    TY_MIL_COM          cmd;
    AiUInt32            mode;
    TY_API_SET_MEM_INFO mem_info;
} TY_MIL_COM_SYS_MEM_PART_SET_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        status;
    AiUInt32        mem_used[2];
} TY_MIL_COM_SYS_MEM_PART_SET_OUTPUT;


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    mode;
} TY_MIL_COM_SYS_MEM_PART_GET_INPUT;


typedef struct
{
    TY_MIL_COM_ACK      xAck;
    TY_API_GET_MEM_INFO mem_info;
} TY_MIL_COM_SYS_MEM_PART_GET_OUTPUT;





typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
    AiUInt32   con;
} TY_MIL_COM_SYS_SYSTAG_CON_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
    AiUInt32   con;
    AiUInt32   mode;
    AiUInt32   xid_rtsa;
    AiUInt32   fct;
    AiUInt32   min;
    AiUInt32   max;
    AiUInt32   step;
    AiUInt32   wpos;
    AiUInt32   bpos_nbit;
} TY_MIL_COM_SYS_SYSTAG_DEF_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   prot;
    AiUInt32   mode;
    AiUInt32   rt;
} TY_MIL_COM_SYS_MILBUS_PROT_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   timeout;
} TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   mode;
    AiUInt32   source;
    AiUInt32   dest;
    AiUInt32   tt_clear;
} TY_MIL_COM_SYS_PXI_CON_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        value;
} TY_MIL_COM_SYS_DISCRETES_READ_OUTPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        value;
} TY_MIL_COM_SYS_DISCRETES_CONFIG_READ_OUTPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32 discreteChannels;
    AiUInt32 canIn;
    AiUInt32 canOut;
} TY_MIL_COM_SYS_DISCRETES_INFO_READ_OUTPUT;

typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   mask;
    AiUInt32   value;
} TY_MIL_COM_SYS_DISCRETES_WRITE_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   setup;
} TY_MIL_COM_SYS_DISCRETES_INIT_INPUT;


typedef struct
{
	TY_MIL_COM cmd;
	AiUInt32   setup;
} TY_MIL_COM_SYS_DISCRETES_REC_INIT_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   value;
} TY_MIL_COM_SYS_SYNC_COUNTER_SET_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        value;
    AiUInt32        init;
    AiUInt32        ttl;
    AiUInt32        tth;
} TY_MIL_COM_SYS_SYNC_COUNTER_GET_OUTPUT;



typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        day;
    AiUInt32        hour;
    AiUInt32        min;
    AiUInt32        sec;
    AiUInt32        usec;
} TY_MIL_COM_SYS_IRIG_GET_OUTPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        src;
    AiUInt32        sync;
} TY_MIL_COM_SYS_IRIG_STATUS_GET_OUTPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   day;
    AiUInt32   hour;
    AiUInt32   min;
    AiUInt32   sec;
} TY_MIL_COM_SYS_IRIG_SET_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   source;
} TY_MIL_COM_SYS_IRIG_SOURCE_SET_INPUT;

typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   mode;
    AiUInt32   mbufs;
} TY_MIL_COM_SYS_RESET_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        mbufs;
    AiUInt32        sbufs;
    AiUInt32        m39_bufs;
    AiUInt32        s39_bufs;
    AiUInt32        maddr;
    AiUInt32        saddr;
    AiUInt32        m39_addr;
    AiUInt32        s39_addr;
} TY_MIL_COM_SYS_RESET_OUTPUT;



/*****************************************************************************/
/*                                                                           */
/*                               HS SYS                                      */
/*                                                                           */
/*****************************************************************************/

typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
    AiUInt32   con;
} TY_MIL_COM_HS_SYS_SYSTAG_CON_INPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
    AiUInt32   con;
    AiUInt32   mode;
    AiUInt32   xid_rtmid;
    AiUInt32   fct;
    AiUInt32   min;
    AiUInt32   max;
    AiUInt32   step;
    AiUInt32   wpos;
    AiUInt32   bpos_nbit;
} TY_MIL_COM_HS_SYS_SYSTAG_DEF_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   timeout;
} TY_MIL_COM_HS_SYS_RX_TIMEOUT_INPUT;

/*****************************************************************************/
/*                                                                           */
/*                              SYS - LS CAL                                 */
/*                                                                           */
/*****************************************************************************/
typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    bus;
    AiUInt32    con;
} TY_MIL_COM_LS_SYS_CAL_CON_INPUT;

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    con;
} TY_MIL_COM_LS_SYS_CAL_TRANS_INPUT;





/*****************************************************************************/
/*                                                                           */
/*                              SYS - HS CAL                                 */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    bus;
    AiUInt32    con;
} TY_MIL_COM_HS_SYS_CAL_CON_INPUT;

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    con;
} TY_MIL_COM_HS_SYS_CAL_CPL_CON_INPUT;

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    enable;
    AiUInt32    con[2];
} TY_MIL_COM_HS_SYS_FOFE_DISCRETES_INPUT;





/*****************************************************************************/
/*                                                                           */
/*                              SYS - LS TRACK                               */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    track_id;
    AiUInt32    bt;
    AiUInt32    xid_rt_sa;
    AiUInt32    mode;
    AiUInt32    start;
    AiUInt32    bit_pos;
    AiUInt32    bit_len;
    AiUInt32    size;
    AiUInt32    states;
    AiUInt32    file_count;
    AiUInt32    offset;
} TY_MIL_COM_LS_SYS_TRACK_DEF_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        buffer_start;
} TY_MIL_COM_LS_SYS_TRACK_DEF_OUTPUT;

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    index;
    AiUInt32    mode;
} TY_MIL_COM_LS_SYS_TRACK_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        valid;
    AiUInt32        last_time_tag;
    AiUInt16        data[1];
} TY_MIL_COM_LS_SYS_TRACK_READ_OUTPUT;


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    index;
    AiUInt32    size;
} TY_MIL_COM_LS_SYS_TRACK_SCAN_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        more;
    AiUInt16        data[1];
} TY_MIL_COM_LS_SYS_TRACK_SCAN_OUTPUT;


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    count;
    AiUInt16    states[1];
} TY_MIL_COM_LS_SYS_TRACK_ALLOC_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        buffer_start[1];
} TY_MIL_COM_LS_SYS_TRACK_ALLOC_OUTPUT;


/*****************************************************************************/
/*                                                                           */
/*                              SYS - HS TRACK                               */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    track_id;
    AiUInt32    bt;
    AiUInt32    xid_rt_mid;
    AiUInt32    mode;
    AiUInt32    start;
    AiUInt32    bit_pos;
    AiUInt32    bit_len;
    AiUInt32    size;
    AiUInt32    states;
    AiUInt32    file_count;
    AiUInt32    offset;
} TY_MIL_COM_HS_SYS_TRACK_DEF_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        buffer_start;
} TY_MIL_COM_HS_SYS_TRACK_DEF_OUTPUT;

typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    index;
    AiUInt32    mode;
} TY_MIL_COM_HS_SYS_TRACK_READ_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        valid;
    AiUInt32        last_time_tag;
    AiUInt16        data[1];
} TY_MIL_COM_HS_SYS_TRACK_READ_OUTPUT;


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    index;
    AiUInt32    size;
} TY_MIL_COM_HS_SYS_TRACK_SCAN_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        more;
    AiUInt16        data[1];
} TY_MIL_COM_HS_SYS_TRACK_SCAN_OUTPUT;


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    id;
    AiUInt32    count;
    AiUInt16    states[1];
} TY_MIL_COM_HS_SYS_TRACK_ALLOC_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        buffer_start[1];
} TY_MIL_COM_HS_SYS_TRACK_ALLOC_OUTPUT;





/*****************************************************************************/
/*                                                                           */
/*                              SYS - FIFO                                   */
/*                                                                           */
/*****************************************************************************/

typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   f_nbr;
    AiUInt32   b_nbr;
} TY_MIL_COM_SYS_FIFO_INI_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
    AiUInt32   count;
    AiUInt16   data[1];
} TY_MIL_COM_SYS_FIFO_WRITE_INPUT;


typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   id;
} TY_MIL_COM_SYS_FIFO_STATUS_INPUT;


typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        status;
} TY_MIL_COM_SYS_FIFO_STATUS_OUTPUT;




typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   con;
    AiUInt32   f_id;
    AiUInt32   rt;
    AiUInt32   sa;
} TY_MIL_COM_SYS_FIFO_RT_ASSIGN_INPUT;



typedef struct
{
    TY_MIL_COM cmd;
    AiUInt32   con;
    AiUInt32   f_id;
    AiUInt32   xid;
} TY_MIL_COM_SYS_FIFO_BC_ASSIGN_INPUT;



/*****************************************************************************/
/*                                                                           */
/*                              SYS - UTIL                                   */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    count;
    AiUInt32    data[1];
} TY_MIL_COM_SYS_UTIL_INPUT;

typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        count;
    AiUInt32        data[1];
} TY_MIL_COM_SYS_UTIL_OUTPUT;



/*****************************************************************************/
/*                                                                           */
/*                              SYS - EXEC                                   */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    mode;
    AiUInt32    con;
} TY_MIL_COM_SYS_EXEC_INPUT;



/*****************************************************************************/
/*                                                                           */
/*                              SYS - BITE                                   */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    mode;
} TY_MIL_COM_SYS_BITE_INPUT;



typedef struct
{
    TY_MIL_COM_ACK  xAck;
    AiUInt32        result;
} TY_MIL_COM_SYS_BITE_OUTPUT;



/*****************************************************************************/
/*                                                                           */
/*                              SYS - INI                                    */
/*                                                                           */
/*****************************************************************************/


typedef struct
{
    TY_MIL_COM  cmd;
    AiUInt32    mode;
} TY_MIL_COM_SYS_INI_INPUT;



typedef struct
{
    TY_MIL_COM_ACK  xAck;

    AiUInt32    bt[4];
    AiUInt32    streams;
    AiUInt32    prot;
    AiUInt32    pbi_id_biu1;
    AiUInt32    asp_mon_id;
    AiUInt32    asp_bite_id;
    AiUInt32    pbi_id_biu2;
    AiUInt32    glb_mem_size;
    AiUInt32    loc_dram_size;
    AiUInt32    shared_dram_size;
    AiUInt32    board_config;
    AiUInt32    board_type;
    AiUInt32    board_sub_type;
    AiUInt32    hardware_variant;

} TY_MIL_COM_SYS_INI_OUTPUT;





















#endif /* __MIL_COM_SYS_H__ */
