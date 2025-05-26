/* SPDX-FileCopyrightText: 2019-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiRtx64.h
 *
 *  This header file contains declarations for
 *  RTX64 realted API functionality
 *
 */

#ifndef AI1553_RTX64_H
#define AI1553_RTX64_H 

#ifdef _AIM_RTX64

#include "AiOs.h"

/**************************/
/* structs and typedefs   */
/**************************/
typedef struct ty_api_rtx64_interrupt_thread_config
{
    ULONG     Enabled;
    ULONG     Priority;
    KAFFINITY ProcessorEnableMask;
} TY_API_RTX64_INTERRUPT_THREAD_CONFIG;

/***********************/
/* interface functions */
/***********************/

// ApiRtx64InterruptConfigGet can be used to read the configure the current active creation of the interrupt thread.
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiRtx64InterruptConfigGet(TY_API_RTX64_INTERRUPT_THREAD_CONFIG * config);
// ApiRtx64InterruptConfigSet can be used to configure the creation of the interrupt thread. Must be called in between ApiInit and ApiOpenEx
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiRtx64InterruptConfigSet(TY_API_RTX64_INTERRUPT_THREAD_CONFIG * config);

#endif // _AIM_RTX64
#endif // _AI1553_RTX64_H
