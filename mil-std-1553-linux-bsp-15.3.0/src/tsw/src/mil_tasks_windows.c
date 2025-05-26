/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_tasks_windows.c
 *
 *  This File contains the mil tasks implementation for Windows based systems
 */

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <ntstatus.h>
#include <ntstrsafe.h>

#include "mil_tsw_includes.h"

#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));

EVT_WDF_WORKITEM mil_tasks_dataqueue_worker;
EVT_WDF_TIMER mil_tasks_dataqueue_handler;
EVT_WDF_TIMER mil_tasks_busload_handler;


struct mil_tasks_info
{
    WDFMEMORY                this_memory;
    // Data queue processing
    WDFTIMER                 DataQueueTimer;
    struct ai_tsw_os_lock *  DataQueueSpinLock;
    WDFWORKITEM              DataQueueWorkItem;
    BOOLEAN                  DataQueueRunning;

    // Bus load calculation
    WDFTIMER                 BusLoadTimer;
    BOOLEAN                  BusLoadStopRequest;
    BOOLEAN                  BusLoadRunning;

    DWORD uiLastWordCountPrimary[MAX_BIU];
    DWORD uiLastWordCountSecondary[MAX_BIU];
    DWORD uiTotalBusloadPrimary[MAX_BIU];
    DWORD uiTotalBusloadSecondary[MAX_BIU];
    DWORD uiBusloadSampleCountPrimary[MAX_BIU];
    DWORD uiBusloadSampleCountSecondary[MAX_BIU];
};


typedef struct __MIL_TASKS_CONTEXT
{
    TY_API_DEV* p_api_dev;
} MIL_TASKS_CONTEXT, *PMIL_TASKS_CONTEXT;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(MIL_TASKS_CONTEXT, mil_tasks_get_context);

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev)
{
    WDFMEMORY                   mem_buf;
    WDF_TIMER_CONFIG            TimerConfig;
    WDF_OBJECT_ATTRIBUTES       Attributes;
    WDF_WORKITEM_CONFIG         WorkItemConfig;
    NTSTATUS                    Status           = STATUS_UNSUCCESSFUL;
    PMIL_TASKS_CONTEXT          task_context;

    /* Check if struct does already exist */
    if (p_api_dev->px_mil_tasks_info != 0)
        return API_OK;


    WDF_OBJECT_ATTRIBUTES_INIT(&Attributes);
    Attributes.ParentObject = p_api_dev->p_Device;

    Status = WdfMemoryCreate(&Attributes, NonPagedPool, 'KSAT', sizeof(struct mil_tasks_info), &mem_buf, &p_api_dev->px_mil_tasks_info);

    if (!NT_SUCCESS(Status))
    {
        p_api_dev->px_mil_tasks_info = NULL;

        return API_ERR;
    }

    RtlZeroMemory(p_api_dev->px_mil_tasks_info, sizeof(struct mil_tasks_info));

    /* store the wdf memory object for deletion */
    p_api_dev->px_mil_tasks_info->this_memory = mem_buf;


    /* -- Create a queue processing spin lock --- */
    p_api_dev->px_mil_tasks_info->DataQueueSpinLock = ai_tsw_os_lock_create();

    if (!p_api_dev->px_mil_tasks_info->DataQueueSpinLock)
    {
        WdfObjectDelete(mem_buf);
        p_api_dev->px_mil_tasks_info = NULL;
        return API_ERR;
    }

    /* -- Create a timer object to trigger the data queue processing --- */
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&Attributes, MIL_TASKS_CONTEXT);
    Attributes.ParentObject = p_api_dev->p_Device;
	
    WDF_TIMER_CONFIG_INIT(&TimerConfig, mil_tasks_dataqueue_handler);
    TimerConfig.AutomaticSerialization = FALSE;
    
    Status = WdfTimerCreate(&TimerConfig, &Attributes, &p_api_dev->px_mil_tasks_info->DataQueueTimer);

    if (!NT_SUCCESS(Status))
    {
        WdfObjectDelete(mem_buf);
        p_api_dev->px_mil_tasks_info = NULL;
        return API_ERR;
    }

    task_context = mil_tasks_get_context(p_api_dev->px_mil_tasks_info->DataQueueTimer);
    task_context->p_api_dev = p_api_dev;

    p_api_dev->px_mil_tasks_info->DataQueueRunning = FALSE;

    /* -- Create a work item to perform the data queue processing --- */

    WDF_WORKITEM_CONFIG_INIT(&WorkItemConfig, mil_tasks_dataqueue_worker);

    Status = WdfWorkItemCreate(&WorkItemConfig,
        &Attributes,
        &p_api_dev->px_mil_tasks_info->DataQueueWorkItem);

    if (!NT_SUCCESS(Status))
    {
        WdfObjectDelete(mem_buf);
        p_api_dev->px_mil_tasks_info = NULL;
        return API_ERR;
    }

    task_context = mil_tasks_get_context(p_api_dev->px_mil_tasks_info->DataQueueWorkItem);
    task_context->p_api_dev = p_api_dev;


    /* -- Initialize Bus Load processing --- */


    /* -- Create a timer object to trigger the bus load processing --- */

    WDF_TIMER_CONFIG_INIT(&TimerConfig, mil_tasks_busload_handler);
    TimerConfig.AutomaticSerialization = FALSE;

    Status = WdfTimerCreate(&TimerConfig, &Attributes, &p_api_dev->px_mil_tasks_info->BusLoadTimer);

    if (!NT_SUCCESS(Status))
    {
        WdfObjectDelete(mem_buf);
        p_api_dev->px_mil_tasks_info = NULL;
        return API_ERR;
    }

    task_context = mil_tasks_get_context(p_api_dev->px_mil_tasks_info->BusLoadTimer);
    task_context->p_api_dev = p_api_dev;
    
    p_api_dev->px_mil_tasks_info->BusLoadRunning = FALSE;

    return 0;}

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev)
{
    WDFMEMORY mem_buf;

    WdfTimerStop(p_api_dev->px_mil_tasks_info->DataQueueTimer, TRUE);
    WdfTimerStop(p_api_dev->px_mil_tasks_info->BusLoadTimer, TRUE);

    WdfWorkItemFlush(p_api_dev->px_mil_tasks_info->DataQueueWorkItem);

    ai_tsw_os_lock_free(p_api_dev->px_mil_tasks_info->DataQueueSpinLock);
    p_api_dev->px_mil_tasks_info->DataQueueSpinLock = NULL;

    mem_buf = p_api_dev->px_mil_tasks_info->this_memory;

    WdfObjectDelete(mem_buf);
    p_api_dev->px_mil_tasks_info = NULL;

    return 0;
}






AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev)
{
    if (!p_api_dev->px_mil_tasks_info)
    {
        return API_ERR;
    }

    if (p_api_dev->px_mil_tasks_info->DataQueueRunning == FALSE)
    {
        p_api_dev->px_mil_tasks_info->DataQueueRunning = TRUE;

        WdfTimerStart(p_api_dev->px_mil_tasks_info->DataQueueTimer, WDF_REL_TIMEOUT_IN_MS(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
    }

    return API_OK;
}


AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev)
{
    int i = 0;
    BOOLEAN queue_active = FALSE;
    NTSTATUS Status           = STATUS_UNSUCCESSFUL;
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

    if (!p_api_dev->px_mil_tasks_info)
    {
        return API_ERR;
    }

    /* Check if any of the BIU recording queues is still active */
    for (i = 0; i <= API_DATA_QUEUE_ID_BM_REC_BIU4; i++)
    {
        pDataQueueHeader = API_DATAQUEUE_HEADER(i);
        if (pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START)
        {
            queue_active = TRUE;
        }
    }

    if (!queue_active)
    {
        p_api_dev->px_mil_tasks_info->DataQueueRunning = FALSE;
        WdfTimerStop(p_api_dev->px_mil_tasks_info->DataQueueTimer, TRUE);
    }

    return API_OK;
}

void mil_tasks_dataqueue_worker(WDFWORKITEM WorkItem)
{
    PMIL_TASKS_CONTEXT task_context = mil_tasks_get_context(WorkItem);
    mil_tasks_dataqueue_process(task_context->p_api_dev, -1);
}

AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID)
{
    int i = 0;

    if(!p_api_dev->px_mil_tasks_info)
    {
        return API_ERR;
    }

    ai_tsw_os_lock_aquire(p_api_dev->px_mil_tasks_info->DataQueueSpinLock);

    if(dataqueueID == -1)
    {
        /* Process all BIU Recording queues */
        for(i=0; i <= API_DATA_QUEUE_ID_BM_REC_BIU4; i++)
        {
            api_ir_data_queue(p_api_dev, (AiUInt8)i);
        }
    }
    else
    {
        api_ir_data_queue(p_api_dev, (AiUInt8)dataqueueID);
    }

    ai_tsw_os_lock_release(p_api_dev->px_mil_tasks_info->DataQueueSpinLock);

    return 0;
}



void mil_tasks_dataqueue_handler(WDFTIMER Timer)
{
    PMIL_TASKS_CONTEXT task_context = mil_tasks_get_context(Timer);

    /* queue a work item to do the work */
    WdfWorkItemEnqueue(task_context->p_api_dev->px_mil_tasks_info->DataQueueWorkItem);

    WdfTimerStart(task_context->p_api_dev->px_mil_tasks_info->DataQueueTimer, WDF_REL_TIMEOUT_IN_MS(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
}


AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    if (p_api_dev->px_mil_tasks_info == NULL)
    {
        return API_ERR;
    }

    p_api_dev->bm_status_cnt[stream].bus_load_pri_avg = 0L;
    p_api_dev->bm_status_cnt[stream].bus_load_sec_avg = 0L;

    if( p_api_dev->px_mil_tasks_info->BusLoadRunning == FALSE )
    {
        p_api_dev->px_mil_tasks_info->BusLoadRunning = TRUE;
        WdfTimerStart(p_api_dev->px_mil_tasks_info->BusLoadTimer, WDF_REL_TIMEOUT_IN_MS(AIM_MIL_BUSLOAD_PERIOD_MS));
    }

    return API_OK;
}


AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    int i = 0;
    BOOLEAN monitorActive = FALSE;

    if (!p_api_dev->px_mil_tasks_info)
    {
        return API_ERR;
    }

    /* Reset all bus load specific calculations */
    p_api_dev->px_mil_tasks_info->uiTotalBusloadPrimary[stream] = 0;
    p_api_dev->px_mil_tasks_info->uiTotalBusloadSecondary[stream] = 0;
    p_api_dev->px_mil_tasks_info->uiLastWordCountPrimary[stream] = 0;
    p_api_dev->px_mil_tasks_info->uiLastWordCountSecondary[stream] = 0;
    p_api_dev->px_mil_tasks_info->uiBusloadSampleCountPrimary[stream] = 0;
    p_api_dev->px_mil_tasks_info->uiBusloadSampleCountSecondary[stream] = 0;

    p_api_dev->bm_status_cnt[stream].bus_load_pri = 0L;
    p_api_dev->bm_status_cnt[stream].bus_load_sec = 0L;

    /* Check if monitoring on at least one stream is still active.
    * If not, stop bus load timer
    */
    for (i = 0; i < MAX_BIU; i++)
    {
        if (p_api_dev->bm_status[i] == API_BUSY)
        {
            monitorActive = TRUE;
        }
    }

    if (!monitorActive)
    {
        p_api_dev->px_mil_tasks_info->BusLoadRunning = FALSE;
        WdfTimerStop(p_api_dev->px_mil_tasks_info->BusLoadTimer, TRUE);
    }

    return 0;
}







/* Work item handler function that performs busload calculation
for a specific device */
void mil_tasks_busload_handler(WDFTIMER Timer)
{
    PMIL_TASKS_CONTEXT task_context = mil_tasks_get_context(Timer);
    TY_API_DEV       * pxTswData       = task_context->p_api_dev;

    int   i                           = 0;
    DWORD uiCurrentWordCountPrimary   = 0;
    DWORD uiCurrentWordCountSecondary = 0;

    if (pxTswData == 0)
    {
        return;
    }

    for (i = 0; i < pxTswData->chns; i++)
    {
        if (pxTswData->bm_status[i] == API_BUSY)
        {
            /* Monitor is active, calculate busload */

            uiCurrentWordCountPrimary   = pxTswData->cb_p[i]->bm_wcnt_pri;
            uiCurrentWordCountSecondary = pxTswData->cb_p[i]->bm_wcnt_sec;

            /* calculate bus load in percent*100 (with 2 digits resolution) */
            pxTswData->bm_status_cnt[i].bus_load_pri = (((uiCurrentWordCountPrimary
                - pxTswData->px_mil_tasks_info->uiLastWordCountPrimary[i])
                * 100) / 125);

            pxTswData->bm_status_cnt[i].bus_load_sec = (((uiCurrentWordCountSecondary
                - pxTswData->px_mil_tasks_info->uiLastWordCountSecondary[i])
                * 100) / 125);

            /* Calculate Average Busload of primary bus */
            if (uiCurrentWordCountPrimary  >= pxTswData->px_mil_tasks_info->uiLastWordCountPrimary[i])
            {
                pxTswData->px_mil_tasks_info->uiTotalBusloadPrimary[i] += pxTswData->bm_status_cnt[i].bus_load_pri;
                pxTswData->px_mil_tasks_info->uiBusloadSampleCountPrimary[i]++;

                if (pxTswData->px_mil_tasks_info->uiBusloadSampleCountPrimary[i] != 0)
                {
                    pxTswData->bm_status_cnt[i].bus_load_pri_avg =   pxTswData->px_mil_tasks_info->uiTotalBusloadPrimary[i]
                        / pxTswData->px_mil_tasks_info->uiBusloadSampleCountPrimary[i];
                }
            }
            else
            {
                /* Reset because of word count overflow */
                pxTswData->px_mil_tasks_info->uiTotalBusloadPrimary[i]       = 0;
                pxTswData->px_mil_tasks_info->uiBusloadSampleCountPrimary[i] = 0;

                pxTswData->bm_status_cnt[i].bus_load_pri_avg = 0;
            }


            /* Calculate Average Busload of secondary bus */
            if (uiCurrentWordCountSecondary  >= pxTswData->px_mil_tasks_info->uiLastWordCountSecondary[i])
            {
                pxTswData->px_mil_tasks_info->uiTotalBusloadSecondary[i] += pxTswData->bm_status_cnt[i].bus_load_sec;
                pxTswData->px_mil_tasks_info->uiBusloadSampleCountSecondary[i]++;

                if (pxTswData->px_mil_tasks_info->uiBusloadSampleCountSecondary[i] != 0)
                {
                    pxTswData->bm_status_cnt[i].bus_load_sec_avg =   pxTswData->px_mil_tasks_info->uiTotalBusloadSecondary[i]
                        / pxTswData->px_mil_tasks_info->uiBusloadSampleCountSecondary[i];
                }
            }
            else
            {
                /* Reset because of word count overflow */
                pxTswData->px_mil_tasks_info->uiTotalBusloadSecondary[i]       = 0;
                pxTswData->px_mil_tasks_info->uiBusloadSampleCountSecondary[i] = 0;

                pxTswData->bm_status_cnt[i].bus_load_sec_avg      = 0;
            }

            pxTswData->px_mil_tasks_info->uiLastWordCountPrimary[i]   = uiCurrentWordCountPrimary;
            pxTswData->px_mil_tasks_info->uiLastWordCountSecondary[i] = uiCurrentWordCountSecondary;

        }
        else
        {
            /* set all busload relevant variables to 0 */
            pxTswData->px_mil_tasks_info->uiLastWordCountPrimary[i]        = 0;
            pxTswData->px_mil_tasks_info->uiLastWordCountSecondary[i]      = 0;
            pxTswData->px_mil_tasks_info->uiTotalBusloadPrimary[i]         = 0;
            pxTswData->px_mil_tasks_info->uiTotalBusloadSecondary[i]       = 0;
            pxTswData->px_mil_tasks_info->uiBusloadSampleCountPrimary[i]   = 0;
            pxTswData->px_mil_tasks_info->uiBusloadSampleCountSecondary[i] = 0;

            pxTswData->bm_status_cnt[i].bus_load_pri     = 0;
            pxTswData->bm_status_cnt[i].bus_load_sec     = 0;
            pxTswData->bm_status_cnt[i].bus_load_pri_avg = 0;
            pxTswData->bm_status_cnt[i].bus_load_sec_avg = 0;
        }
    }

    /* start the busload timer again with a specific delay */
    WdfTimerStart(pxTswData->px_mil_tasks_info->BusLoadTimer, WDF_REL_TIMEOUT_IN_MS(AIM_MIL_BUSLOAD_PERIOD_MS));
}










AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev)
{
    /* FODDL not required in Windows Driver context */
    return API_ERR_OS_NOT_SUPPORTED;
}


AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev )
{
    /* FODDL not required in Windows Driver context */
    return API_ERR_OS_NOT_SUPPORTED;
}
