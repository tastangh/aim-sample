/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_tasks_labviewrt.c
 *
 *  This File contains the mil tasks implementation for LabView RT based systems
 */

#include <initguid.h>
#include "mil_tsw_includes.h"

#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));

/* TODO, NEEDS TO BE IMPLEMENTED */
AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev){ return 0; }       /* not implemented but needs to return 0 (OK) */
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev){ return 0; }       /* not implemented but needs to return 0 (OK) */
AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev){ return API_ERR_OS_NOT_SUPPORTED; }
AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev){  return API_ERR_OS_NOT_SUPPORTED; }
AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID){ return API_ERR_OS_NOT_SUPPORTED; }
AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream){     return 0; }       /* not implemented but needs to return 0 (OK) */
AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream){      return 0; }       /* not implemented but needs to return 0 (OK) */

AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev){ return 0; }       /* not implemented but needs to return 0 (OK) */
AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev ){ return 0; }       /* not implemented but needs to return 0 (OK) */


