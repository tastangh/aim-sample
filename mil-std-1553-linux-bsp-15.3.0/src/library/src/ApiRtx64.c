/* SPDX-FileCopyrightText: 2019-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiRtx64.c
 *
 * RTX64 Related interface functions
 * 
 */

//****************************************************
// my own includes
//****************************************************
#include "Ai_cdef.h"
#include "Ai_def.h"

#ifdef _AIM_RTX64
#include "ApiRtx64.h"
#include "AiMilErrors.h"

static TY_API_RTX64_INTERRUPT_THREAD_CONFIG global_interrupt_thread_config;



// ApiRtx64InterruptConfigGet can be used to read the configure the current active creation of the interrupt thread.
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiRtx64InterruptConfigGet(TY_API_RTX64_INTERRUPT_THREAD_CONFIG * config)
{
    if (!config)
        return API_ERR_PARAM1_IS_NULL;

    memcpy(config, &global_interrupt_thread_config, sizeof(TY_API_RTX64_INTERRUPT_THREAD_CONFIG));

    return API_OK;
}


// ApiRtx64InterruptConfigSet can be used to configure the creation of the interrupt thread. Must be called in between ApiInit and ApiOpenEx
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiRtx64InterruptConfigSet(TY_API_RTX64_INTERRUPT_THREAD_CONFIG * config)
{
    if (!config)
        return API_ERR_PARAM1_IS_NULL;

    memcpy(&global_interrupt_thread_config, config, sizeof(TY_API_RTX64_INTERRUPT_THREAD_CONFIG));

    return API_OK;
}



#endif // _AIM_RTX64
/* EOF */
