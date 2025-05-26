/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_tasks_nucleus.c
 *
 *  This File contains the mil tasks implementation for Nucleus based systems
 */


#include "mil_tsw_includes.h"

#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));

struct mil_tasks_info
{
    TY_API_DEV* parent;
};

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev)
{
    return 0;
}

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev)
{
    return 0;
}


AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev)
{
    /* Data Queues are processed from api_opr */
    return 0;
}


AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev)
{
    /* Data Queues are processed from api_opr */
    return 0;
}


AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID)
{
    int i = 0;

    if(dataqueueID == -1)
    {
        /* Process all BIU Recording queues */
        for(i=0; i <= API_DATA_QUEUE_ID_BM_REC_BIU8; i++)
        {
            api_ir_data_queue(p_api_dev, i);
        }
    }
    else
    {
        /* Process specified queue */
        api_ir_data_queue(p_api_dev, dataqueueID);
    }

    return 0;
}

AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    p_api_dev->bm_status_cnt[stream].bus_load_pri_avg = 0L;
    p_api_dev->bm_status_cnt[stream].bus_load_sec_avg = 0L;

    /* Data Queues are processed from api_opr */
    return 0;
}


AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    p_api_dev->bm_status_cnt[stream].bus_load_pri = 0L;
    p_api_dev->bm_status_cnt[stream].bus_load_sec = 0L;

    /* Data Queues are processed from api_opr */
    return 0;
}


AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev)
{
    /* Data Queues are processed from api_opr */
    return 0;
}


AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev )
{
    /* Data Queues are processed from api_opr */
    return 0;
}

