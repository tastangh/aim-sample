/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsScopeInternal.h
 *
 * LS Digital Scope Internal functions
 * 
 */
#ifndef API_LSSCOPE_INTERNAL_H
#define API_LSSCOPE_INTERNAL_H

#include "Ai_cdef.h"
#include "Ai1553i_def.h"
#include "Ai1553i_fnc.h"


#if defined(_AIM_VME)
#define OVERLAPPED       void *
#define CRITICAL_SECTION void *
#define RPC_ASYNC_STATE  void*
#endif

#if defined(_AIM_LINUX)
#define OVERLAPPED       void *
#define CRITICAL_SECTION pthread_mutex_t
#define RPC_ASYNC_STATE  void*
#endif

#if defined(_AIM_RTX64)
#define RPC_ASYNC_STATE  void*
#endif


/* only internally used */
typedef struct ty_scope_buffer_list_element
{
    struct ty_scope_buffer_list_element * p_Prev;
    struct ty_scope_buffer_list_element * p_Next;

    TY_API_SCOPE_BUFFER * p_ApiScopeBuffer;
    OVERLAPPED x_Event;
    
    RPC_ASYNC_STATE RPCScopeData_AsyncHandle;
}TY_SCOPE_BUFFER_LIST_ELEMENT; 


#endif
/* end: ApiLsScope.h */
