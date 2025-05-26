/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_tasks_linux.c
 *
 *  This File contains the mil tasks implementation for Linux based systems
 */

#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

#include "mil_tsw_includes.h"

#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));

void mil_tasks_dataqueue_worker(struct work_struct* work);
void mil_tasks_foddl_worker(struct work_struct* work);

/*! Platform specific os layer timer structure */
struct ai_tsw_os_timer
{
    struct timer_list timer;   /*!< Linux Kernel timer object that is used as os layer timer */
    AiBoolean is_elapsed;       /*!< signifies if timer has elapsed */ 
    void* parent;
};

struct mil_tasks_info
{
    struct ai_tsw_os_timer* pDataqueueTimer;               /*!< timer that triggers retrievel of data into data queues */
    struct ai_tsw_os_lock * dataQueueSpinlock;        /*!< spinlock used for synchronizing access to data queues */
    struct work_struct dataQueueWork;                 /*!< work item for handling interrupts */
    TY_API_DEV* parent;

    struct ai_tsw_os_timer* pBusloadTimer;                 /*!< Timer used for periodic calculation of bus load */
    AiUInt32 busload_calculation_counter[ MAX_BIU ];  /*!< Counts how many times bus load has been calculated for a stream, since monitor on this stream was started */
    AiUInt32 busload_wordcount_pri[ MAX_BIU ];        /*!< The last word count on primary bus that was detected during bus load calculation */
    AiUInt32 busload_wordcount_sec[ MAX_BIU ];        /*!< The last word count on secondary bus that was detected during bus load calculation */
    struct ai_tsw_os_timer* pFoddlTimer;                   /*!< Timer used for FODDL monitoring */
    struct work_struct foddlWork;                 /*!< work item for handling foddl work */
};

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev)
{
    p_api_dev->px_mil_tasks_info = kzalloc(sizeof(struct mil_tasks_info), GFP_KERNEL);

    p_api_dev->px_mil_tasks_info->pDataqueueTimer = NULL;
    p_api_dev->px_mil_tasks_info->pBusloadTimer = NULL;
    p_api_dev->px_mil_tasks_info->pFoddlTimer = NULL;

    p_api_dev->px_mil_tasks_info->parent = p_api_dev;
    p_api_dev->px_mil_tasks_info->dataQueueSpinlock = ai_tsw_os_lock_create();

    INIT_WORK(&p_api_dev->px_mil_tasks_info->dataQueueWork, mil_tasks_dataqueue_worker);
    INIT_WORK(&p_api_dev->px_mil_tasks_info->foddlWork, mil_tasks_foddl_worker);

    return 0;
}

/*! \brief Allocate tasks info struct
 *
 * @param p_api_dev pointer to target software administration structure of the device to start timer for
 * @return returns 0 on success, non-zero value otherwise
 */
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev)
{
   if( p_api_dev->px_mil_tasks_info->pDataqueueTimer )
    {
        del_timer_sync(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer);
        cancel_work_sync(&p_api_dev->px_mil_tasks_info->dataQueueWork);
        kfree(p_api_dev->px_mil_tasks_info->pDataqueueTimer);
        p_api_dev->px_mil_tasks_info->pDataqueueTimer = NULL;
    }

    if( p_api_dev->px_mil_tasks_info->pBusloadTimer )
    {
        del_timer_sync(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer);
        kfree(p_api_dev->px_mil_tasks_info->pBusloadTimer);
        p_api_dev->px_mil_tasks_info->pBusloadTimer = NULL;
    }

    if( p_api_dev->px_mil_tasks_info->pFoddlTimer )
    {
        del_timer_sync(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer);
        cancel_work_sync(&p_api_dev->px_mil_tasks_info->foddlWork);
        kfree(p_api_dev->px_mil_tasks_info->pFoddlTimer);        
        p_api_dev->px_mil_tasks_info->pFoddlTimer = NULL;
    }
    

    ai_tsw_os_lock_free(p_api_dev->px_mil_tasks_info->dataQueueSpinlock);

    kfree(p_api_dev->px_mil_tasks_info);
    p_api_dev->px_mil_tasks_info = NULL;

    return 0;
}




/*! \brief Data queue timer handler
 *
 * This function is called every time the periodic data queue timer elapses. \n
 * @param data pointer to \ref TY_API_DEV
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_dataqueue_handler(unsigned long data);
#else
void mil_tasks_dataqueue_handler(struct timer_list *t);
#endif

/*! \brief Bus load calculation timer handler
 *
 * This function is called every time the periodic bus load calculation timer elapses. \n
 * It will calculate the current bus load, as well as average bus load since monitor start
 * on every stream where monitoring is active
 * @param data pointer to \ref TY_API_DEV
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_busload_handler(unsigned long data);
#else
void mil_tasks_busload_handler(struct timer_list *t);
#endif

/*! \brief FODDL monitoring timer handler
 *
 * This function is called every time the periodic FODDL timer elapses. \n
 * It will monitor the current FODDL values from the Bus Monitor stack.
 * @param data pointer to \ref TY_API_DEV
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_foddl_handler(unsigned long data);
#else
void mil_tasks_foddl_handler(struct timer_list *t);
#endif

AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev)
{
    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No tasks info p_api_dev\n");
        return 1;
    }

    if(!p_api_dev->px_mil_tasks_info->pDataqueueTimer)
    {
        /* Timer was not yet allocated. Do this and initialize it */
        p_api_dev->px_mil_tasks_info->pDataqueueTimer = kzalloc(sizeof(struct ai_tsw_os_timer), GFP_KERNEL);
        if(!p_api_dev->px_mil_tasks_info->pDataqueueTimer)
        {
            pr_err("Allocation of data queue timer failed\n");
            return 1;
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
        init_timer(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer);
        p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer.function = mil_tasks_dataqueue_handler;
        p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer.data     = (unsigned long) p_api_dev;
#else
        timer_setup(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer, mil_tasks_dataqueue_handler, 0);	
        p_api_dev->px_mil_tasks_info->pDataqueueTimer->parent = (void*)p_api_dev;
#endif
    }

    if(!timer_pending(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer))
    {
        /* Timer is not started yet, so do initial activation.
         * The timer must be restarted in the timer handler function */
        mod_timer(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
    }
  
    return 0;
}


AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev)
{
    int i = 0;
    bool queue_active = false;
    volatile TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No tasks info in p_api_dev\n");
        return 1;
    }

    /* Check if any of the BIU recording queues is still active */
    for(i = 0; i <= API_DATA_QUEUE_ID_BM_REC_BIU4; i++)
    {
        pDataQueueHeader = API_DATAQUEUE_HEADER(i);
        if(pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START)
        {
            queue_active = true;
        }
    }

    if(!queue_active)
    {
        /* No queue is active, so stop timer */

        del_timer_sync(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer);
    }

    return 0;
}


AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID)
{
    int i = 0;

    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No valid MIL device context for processing data queues\n");
        return 1;
    }

    ai_tsw_os_lock_aquire(p_api_dev->px_mil_tasks_info->dataQueueSpinlock);

    if(dataqueueID == -1)
    {
        /* Process all BIU Recording queues */
        for(i=0; i <= API_DATA_QUEUE_ID_BM_REC_BIU4; i++)
        {
            api_ir_data_queue(p_api_dev, i);
        }
    }
    else
    {
        /* Process specified queue */
        api_ir_data_queue(p_api_dev, dataqueueID);
    }

    ai_tsw_os_lock_release(p_api_dev->px_mil_tasks_info->dataQueueSpinlock);

    return 0;
}

void mil_tasks_dataqueue_worker(struct work_struct* work)
{
    TY_API_DEV* p_api_dev = NULL;
    struct mil_tasks_info * tasks_info;
    AiUInt32 retVal = 0;

    tasks_info = container_of(work, struct mil_tasks_info, dataQueueWork);

    p_api_dev = tasks_info->parent;

    BUG_ON(p_api_dev == NULL);

    /* Process all BIU recording data queues */
    retVal = mil_tasks_dataqueue_process(p_api_dev, -1);

    if(retVal)
    {
        pr_err("Data queue processing failed. Stopping them!\n");
    }
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_dataqueue_handler(unsigned long data)
{
    TY_API_DEV* p_api_dev = (TY_API_DEV*)data;
#else
void mil_tasks_dataqueue_handler(struct timer_list *t)
{
    TY_API_DEV* p_api_dev = NULL;
    TY_AI_TSW_OS_TIMER *p_TimerDQ = from_timer(p_TimerDQ,t,timer);
    BUG_ON(p_TimerDQ == NULL);
    p_api_dev = (TY_API_DEV*)p_TimerDQ->parent;
#endif
    BUG_ON(p_api_dev == NULL);

    /* Process all BIU recording data queues */
    schedule_work(&p_api_dev->px_mil_tasks_info->dataQueueWork);

    /* Restart data queue timer */
    mod_timer(&p_api_dev->px_mil_tasks_info->pDataqueueTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
}

AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No valid tasks info for bus load calculation\n");
        return API_ERR;
    }

    p_api_dev->bm_status_cnt[ stream ].bus_load_pri_avg = 0L;
    p_api_dev->bm_status_cnt[ stream ].bus_load_sec_avg = 0L;

    if(!p_api_dev->px_mil_tasks_info->pBusloadTimer)
    {
        /* Create bus load timer */
        p_api_dev->px_mil_tasks_info->pBusloadTimer = kzalloc(sizeof(struct ai_tsw_os_timer), GFP_KERNEL);
        if(!p_api_dev->px_mil_tasks_info->pBusloadTimer)
        {
            pr_err("Allocation of bus load timer failed\n");
            return API_ERR;
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
        init_timer(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer);
        p_api_dev->px_mil_tasks_info->pBusloadTimer->timer.function = mil_tasks_busload_handler;
        p_api_dev->px_mil_tasks_info->pBusloadTimer->timer.data = (unsigned long) p_api_dev;
#else
        timer_setup(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer,mil_tasks_busload_handler,0);
        p_api_dev->px_mil_tasks_info->pBusloadTimer->parent = (void*)p_api_dev;
#endif
    }

    if(!timer_pending(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer))
    {
        mod_timer(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_BUSLOAD_PERIOD_MS));
    }

    return 0;
}


AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    int i = 0;
    bool monitorActive = false;

    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No valid tasks info for bus load calculation\n");
        return API_ERR;
    }

    /* Reset all bus load specific calculations */
    p_api_dev->px_mil_tasks_info->busload_calculation_counter[stream] = 0;
    p_api_dev->px_mil_tasks_info->busload_wordcount_pri[stream] = 0;
    p_api_dev->px_mil_tasks_info->busload_wordcount_sec[stream] = 0;

    p_api_dev->bm_status_cnt[ stream ].bus_load_pri = 0L;
    p_api_dev->bm_status_cnt[ stream ].bus_load_sec = 0L;

    /* Check if monitoring on at least one stream is still active.
     * If not, stop bus load timer
     */
    for(i = 0; i < MAX_BIU; i++)
    {
        if(p_api_dev->bm_status[i] == API_BUSY)
        {
            monitorActive = true;
        }
    }

    if(!monitorActive)
    {
        del_timer_sync(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer);
    }

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_busload_handler(unsigned long data)
{
    TY_API_DEV* p_api_dev = (TY_API_DEV*)data;
    int i = 0;
    AiUInt32 current_wordcount_pri = 0;
    AiUInt32 current_wordcount_sec = 0;
    const unsigned int ls_words_per_msec =   50; /* data words possible per ms with  1MHz bus speed */
    const unsigned int hs_words_per_msec = 1250; /* data words possible per ms with 20MHz bus speed */
    unsigned int divisor = 0;

    BUG_ON(p_api_dev == NULL);
#else
void mil_tasks_busload_handler(struct timer_list *t)
{
    TY_API_DEV* p_api_dev = NULL;
    TY_AI_TSW_OS_TIMER *p_TimerBL = from_timer(p_TimerBL,t,timer);
    int i = 0;
    AiUInt32 current_wordcount_pri = 0;
    AiUInt32 current_wordcount_sec = 0;
    const unsigned int ls_words_per_msec =   50; /* data words possible per ms with  1MHz bus speed */
    const unsigned int hs_words_per_msec = 1250; /* data words possible per ms with 20MHz bus speed */
    unsigned int divisor = 0;

    BUG_ON(p_TimerBL == NULL);
    p_api_dev = (TY_API_DEV*)p_TimerBL->parent;
    BUG_ON(p_api_dev == NULL);
#endif

    /* Calculate bus load on every stream on which monitor is active */
    for(i = 0; i < MAX_BIU; i++)
    {
        if(p_api_dev->bm_status[i] == API_BUSY)
        {
            p_api_dev->px_mil_tasks_info->busload_calculation_counter[i]++;

            if( p_api_dev->auc_StreamType[i] == API_TYPE_1553 )
            {
                /* calculate bus load in percent*100 (with 2 digits resolution) */
                divisor = (ls_words_per_msec * AIM_MIL_BUSLOAD_PERIOD_MS) / 100;

                current_wordcount_pri = p_api_dev->cb_p[i]->bm_wcnt_pri;
                current_wordcount_sec = p_api_dev->cb_p[i]->bm_wcnt_sec;
            }
            else
            {
                /* calculate bus load in percent*100 (with 2 digits resolution) */
                divisor = (hs_words_per_msec * AIM_MIL_BUSLOAD_PERIOD_MS) / 100;

                current_wordcount_pri = px_api39_cb_p[API39_HS_BIU_1]->bm_wcnt_bus_a;
                current_wordcount_sec = px_api39_cb_p[API39_HS_BIU_1]->bm_wcnt_bus_b;
            }

            /* calculate load on primary bus */
            p_api_dev->bm_status_cnt[i].bus_load_pri = (((current_wordcount_pri - p_api_dev->px_mil_tasks_info->busload_wordcount_pri[i])*100) / divisor);
            p_api_dev->px_mil_tasks_info->busload_wordcount_pri[i] = current_wordcount_pri;
            /* calculate load average on primary bus */
            /* current_wordcount_sec contains all words since bm start. If we divide it by timer occurences we geht the average load */
            p_api_dev->bm_status_cnt[i].bus_load_pri_avg = (AiUInt32) (current_wordcount_pri*100 / (p_api_dev->px_mil_tasks_info->busload_calculation_counter[i] * divisor));

            /* calculate load on secondary bus */
            p_api_dev->bm_status_cnt[i].bus_load_sec = (((current_wordcount_sec - p_api_dev->px_mil_tasks_info->busload_wordcount_sec[i])*100) / divisor);
            p_api_dev->px_mil_tasks_info->busload_wordcount_sec[i] = current_wordcount_sec;
            /* calculate load average on secondary bus */
            /* current_wordcount_sec contains all words since bm start. If we divide it by timer occurences we geht the average load */
            p_api_dev->bm_status_cnt[i].bus_load_sec_avg = (current_wordcount_sec*100) / (p_api_dev->px_mil_tasks_info->busload_calculation_counter[i] * divisor);
        }
    }

    /* Restart periodic bus load calculation timer */
    mod_timer(&p_api_dev->px_mil_tasks_info->pBusloadTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_BUSLOAD_PERIOD_MS));
}










AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev)
{
    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No valid tasks info for bus load calculation\n");
        return API_ERR;
    }

    if(!p_api_dev->px_mil_tasks_info->pFoddlTimer)
    {
        /* Create bus load timer */
        p_api_dev->px_mil_tasks_info->pFoddlTimer = kzalloc(sizeof(struct ai_tsw_os_timer), GFP_KERNEL);
        if(!p_api_dev->px_mil_tasks_info->pFoddlTimer)
        {
            pr_err("Allocation of FODDL timer failed\n");
            return API_ERR;
        }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
        init_timer(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer);
        p_api_dev->px_mil_tasks_info->pFoddlTimer->timer.function = mil_tasks_foddl_handler;
        p_api_dev->px_mil_tasks_info->pFoddlTimer->timer.data = (unsigned long) p_api_dev;
#else
        timer_setup(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer,mil_tasks_foddl_handler,0);
        p_api_dev->px_mil_tasks_info->pFoddlTimer->parent = (void*)p_api_dev;
#endif
    }

    if(!timer_pending(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer))
    {
        mod_timer(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
    }

    return 0;
}


AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev )
{
    if(!p_api_dev->px_mil_tasks_info)
    {
        pr_err("No valid tasks info for bus load calculation\n");
        return API_ERR;
    }

    if( p_api_dev->px_mil_tasks_info->pFoddlTimer )
    {
        del_timer_sync(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer);
        kfree(p_api_dev->px_mil_tasks_info->pFoddlTimer);
        p_api_dev->px_mil_tasks_info->pFoddlTimer = NULL;
    }
    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void mil_tasks_foddl_handler(unsigned long data)
{
    TY_API_DEV* p_api_dev = (TY_API_DEV*)data;
    BUG_ON(p_api_dev == NULL);
#else
void mil_tasks_foddl_handler(struct timer_list *t)
{
    TY_API_DEV* p_api_dev = NULL;
    TY_AI_TSW_OS_TIMER *p_TimerFD = from_timer(p_TimerFD,t,timer);
    BUG_ON(p_TimerFD == NULL);
    p_api_dev = (TY_API_DEV*)p_TimerFD->parent;
    BUG_ON(p_api_dev == NULL);
#endif

    if(p_api_dev->ApiFoddlFunc[0] != 0)
    {
        /* Process all BIU recording data queues */
        schedule_work(&p_api_dev->px_mil_tasks_info->foddlWork);
    }

    /* Restart periodic bus load calculation timer */
    mod_timer(&p_api_dev->px_mil_tasks_info->pFoddlTimer->timer, jiffies + msecs_to_jiffies(AIM_MIL_DATAQUEUE_PERIOD_MSEC));
}


void mil_tasks_foddl_worker(struct work_struct* work)
{
    V_FNPTR_DEV *pv_FoddlFunc;
    TY_API_DEV* p_api_dev = NULL;
    struct mil_tasks_info * tasks_info;

    tasks_info = container_of(work, struct mil_tasks_info, foddlWork);

    p_api_dev = tasks_info->parent;

    BUG_ON(p_api_dev == NULL);

    if(p_api_dev->ApiFoddlFunc[0] != 0)
    {
        pv_FoddlFunc = (V_FNPTR_DEV*)p_api_dev->ApiFoddlFunc[0];
        (*pv_FoddlFunc) (p_api_dev, p_api_dev->biu, p_api_dev->hs_stream);
    }
}


