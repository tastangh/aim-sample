/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*! \file mil_tasks.c
 *
 *  This File contains the mil tasks dummy implementation for systems that do not need any tasks
 * 
 */


#include "mil_tasks.h"


AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev){ return 0; }
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev){ return 0; }
AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev){ return 0; }
AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev){  return 0; }
AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID){ return 0; }
AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream){     return 0; }
AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream){      return 0; }

AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev){ return 0; }
AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev ){ return 0; }
