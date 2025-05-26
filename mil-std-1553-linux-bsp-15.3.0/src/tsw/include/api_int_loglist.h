/* SPDX-FileCopyrightText: 2018-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_int_loglist.h
 *
 *  Interrupt loglist types for TSW
 *
 */

#ifndef __API_INT_LOGLIST_H__
#define __API_INT_LOGLIST_H__

#ifdef __cplusplus
  extern "C" {
#endif

/* Basic Typedefs */


/* new Type for interrupt Log List */
/*
The interrupt loglist is now organized as Ring-Buffer with a
'put_counter' and a 'get_counter'.
Each time when a interrupt entry is copied from the BIU Interrupt 
Loglist to the high level Loglist, the put_counter must be incremented after. 
The increment must be done modulo to the MAX_API_IR_EVENTS (see Macro
API_INC_LOLI_CNT).

After reading an interrupt event from the get_counter, the get_counter must 
be incremented to the next location. If the counters are equal, the Buffer 
is empty.

 */

typedef union
{
  L_WORD ul_All;
  struct
  {
    L_WORD ul_Info: 24;
    L_WORD uc_IntType: 8;
  } t;

  struct
  {
    L_WORD ul_Info: 24;
    L_WORD uc_Biu1: 1;
    L_WORD uc_Biu2: 1;
    L_WORD uc_Dma: 1;
    L_WORD uc_Target: 1;
    L_WORD uc_Cmd: 1;
    L_WORD uc_Biu3: 1;
    L_WORD uc_Biu4: 1;
    L_WORD res: 1;
  } b;
}
TY_ASP_LOGLIST_LLC_ENTRY;

typedef union
{
  L_WORD ul_All;
  struct
  {
    L_WORD ul_Index: 16;
    L_WORD ul_Res: 8;
    L_WORD uc_IntSrc: 8;
  } t;
}
TY_ASP_LOGLIST_LLD_ENTRY;

struct ty_api_intr_loglist_entry
{
  L_WORD lla;
  L_WORD llb;
  TY_ASP_LOGLIST_LLC_ENTRY llc;
  TY_ASP_LOGLIST_LLD_ENTRY lld;
};

struct ty_api_intr_event_list
{
  L_WORD put_cnt;
  L_WORD get_cnt;
  struct ty_api_intr_loglist_entry entry[ MAX_API_IR_EVENTS ];
};

#endif
/* end: __API_INT_LOGLIST_H__ */
