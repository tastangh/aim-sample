/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_tasks_vxworks.c
 *
 *  This File contains the mil tasks implementation for VxWorks based systems
 */

#include "mil_tsw_includes.h"
#include "mil_vxworks.h"
#include <hwif/mil1553/vxbMil1553.h>
#include <mil_hw_pci_driver.h>
#include "mil_ioctl.h"

/* Note: I didn't use timers, as the timerLib is not included to all BSP. (E.g. MVME5100 with VxWorks 6.9 */





AiUInt32 mil_tasks_dataqueue_process(TY_API_DEV* p_api_dev, AiInt32 dataqueueID)
{
	AiUInt32 i = 0;
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;

	spinLockTaskTake (&deviceContextVxWorks->pSpinLock);
	if (deviceContextVxWorks->dataQueueTimerStop) {
		spinLockTaskGive (&deviceContextVxWorks->pSpinLock);
		return ERROR;
	}
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
	spinLockTaskGive(&deviceContextVxWorks->pSpinLock);

	return API_OK;	
}

AiUInt32 mil_tasks_dataqueue_process_thread(TY_API_DEV* p_api_dev)
{
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;

	deviceContextVxWorks->dataQueueTimerStop = 0;

	while( AiTrue )
	{
	    mil_tasks_dataqueue_process(p_api_dev, -1);
		if (deviceContextVxWorks->dataQueueTimerStop) break;
		AIM_WAIT(AIM_MIL_DATAQUEUE_PERIOD_MSEC);
	}
	
	return API_OK;	
}

AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev)
{
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;

	spinLockTaskInit(&deviceContextVxWorks->pSpinLock, 0);

	mil1553GivePoolingSem (DATA_QUEUE_TIMER_THREAD, p_api_dev->bModule);
	
	return API_OK;
}

AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev)
{
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;
	
		
	spinLockTaskTake(&deviceContextVxWorks->pSpinLock);
	
	deviceContextVxWorks->dataQueueTimerStop = 1;
	
	spinLockTaskGive(&deviceContextVxWorks->pSpinLock);
	

	return API_OK;
}

AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
    p_api_dev->bm_status_cnt[stream].bus_load_pri_avg = 0;
    p_api_dev->bm_status_cnt[stream].bus_load_sec_avg = 0;

	mil1553GivePoolingSem (BUS_LOAD_THREAD, p_api_dev->bModule);
	
	return API_OK;
}

AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream)
{
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;
    int i = 0;
    AiBoolean monitorActive = AiFalse;

    if(!deviceContextVxWorks)
    {
        printf("No valid MIL device context for bus load calculation\n");
        return 1;
    }

    p_api_dev->bm_status_cnt[stream].bus_load_pri     = 0;
    p_api_dev->bm_status_cnt[stream].bus_load_sec     = 0;

    /* Check if monitoring on at least one stream is still active.
     * If not, stop bus load timer
     */
    for(i = 0; i < MAX_BIU; i++)
    {
        if(p_api_dev->bm_status[i] == API_BUSY)
        {
            monitorActive = AiTrue;
        }
    }

    if(!monitorActive)
    {
		deviceContextVxWorks->busloadCalcStop = 1;
    }

    return 0;
}


AiInt16 _AmcCalcBusLoad( TY_API_DEV *p_api_dev )
{
    AiUInt32 i;
    DWORD uiCurrentWordCountPrimary   = 0;
    DWORD uiCurrentWordCountSecondary = 0;
    struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;
    TY_BUS_LOAD *pBusLoadContext = &deviceContextVxWorks->xBusLoadContext;

    for(i = 0; i < p_api_dev->ini_info.streams; i++)
    {
        if(p_api_dev->bm_status[i] == API_BUSY)
        {
            /* Monitor is active, calculate busload */

            uiCurrentWordCountPrimary   = BSWAP32(p_api_dev->cb_p[i]->bm_wcnt_pri);
            uiCurrentWordCountSecondary = BSWAP32(p_api_dev->cb_p[i]->bm_wcnt_sec);

            /* calculate bus load in percent*100 (with 2 digits resolution) */
            p_api_dev->bm_status_cnt[i].bus_load_pri = (((uiCurrentWordCountPrimary 
                                                          - pBusLoadContext->uiLastWordCountPrimary[i]) 
                                                          * 100) / 125 ); 

            p_api_dev->bm_status_cnt[i].bus_load_sec = (((uiCurrentWordCountSecondary 
                                                          - pBusLoadContext->uiLastWordCountSecondary[i]) 
                                                          * 100) / 125 ); 

            /* Calculate Average Busload of primary bus */
            if(uiCurrentWordCountPrimary  >= pBusLoadContext->uiLastWordCountPrimary[i])
            {
                pBusLoadContext->uiTotalBusloadPrimary[i] += p_api_dev->bm_status_cnt[i].bus_load_pri;
                pBusLoadContext->uiBusloadSampleCountPrimary[i]++;

                if(pBusLoadContext->uiBusloadSampleCountPrimary[i] != 0)
                {
                    p_api_dev->bm_status_cnt[i].bus_load_pri_avg =   pBusLoadContext->uiTotalBusloadPrimary[i]
                                                                   / pBusLoadContext->uiBusloadSampleCountPrimary[i];
                }
            }
            else
            {
                /* Reset because of word count overflow */
                pBusLoadContext->uiTotalBusloadPrimary[i]       = 0;
                pBusLoadContext->uiBusloadSampleCountPrimary[i] = 0;

                p_api_dev->bm_status_cnt[i].bus_load_pri_avg = 0;
            }


             /* Calculate Average Busload of secondary bus */
            if(uiCurrentWordCountSecondary  >= pBusLoadContext->uiLastWordCountSecondary[i])
            {
                pBusLoadContext->uiTotalBusloadSecondary[i] += p_api_dev->bm_status_cnt[i].bus_load_sec;
                pBusLoadContext->uiBusloadSampleCountSecondary[i]++;

                if(pBusLoadContext->uiBusloadSampleCountSecondary[i] != 0)
                {
                    p_api_dev->bm_status_cnt[i].bus_load_sec_avg =   pBusLoadContext->uiTotalBusloadSecondary[i]
                                                                   / pBusLoadContext->uiBusloadSampleCountSecondary[i];
                }
            }
            else
            {
                /* Reset because of word count overflow */
                pBusLoadContext->uiTotalBusloadSecondary[i]       = 0;
                pBusLoadContext->uiBusloadSampleCountSecondary[i] = 0;

                p_api_dev->bm_status_cnt[i].bus_load_sec_avg      = 0;
            }

            pBusLoadContext->uiLastWordCountPrimary[i]   = uiCurrentWordCountPrimary;
            pBusLoadContext->uiLastWordCountSecondary[i] = uiCurrentWordCountSecondary;

        }
        else
        {
            /* set all busload relevant variables to 0 */
            pBusLoadContext->uiLastWordCountPrimary[i]        = 0;
            pBusLoadContext->uiLastWordCountSecondary[i]      = 0;
            pBusLoadContext->uiTotalBusloadPrimary[i]         = 0;
            pBusLoadContext->uiTotalBusloadSecondary[i]       = 0;
            pBusLoadContext->uiBusloadSampleCountPrimary[i]   = 0;
            pBusLoadContext->uiBusloadSampleCountSecondary[i] = 0;
        }
    }
    return 0;
}

AiInt16 _AmcVmeTswPollingThread(TY_API_DEV *p_api_dev )
{
	struct aim_mil_device* deviceContextVxWorks = (struct aim_mil_device*)p_api_dev->p_DeviceContext;
	TY_BUS_LOAD *pBusLoadContext = &deviceContextVxWorks->xBusLoadContext;

	memset( pBusLoadContext, 0, sizeof(TY_BUS_LOAD));

	deviceContextVxWorks->busloadCalcStop = 0;

	/* It will be deleted with taskDelete in _ApiOsClose */
	while(AiTrue)
	{
		_AmcCalcBusLoad( p_api_dev );
		if (deviceContextVxWorks->busloadCalcStop) break;
		AIM_WAIT(AIM_MIL_BUSLOAD_PERIOD_MS);
	}

	return API_OK;
}

AiUInt32 mil_tasks_info_init(TY_API_DEV* p_api_dev)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
AiUInt32 mil_tasks_info_free(TY_API_DEV* p_api_dev)
{
	return API_ERR_OS_NOT_SUPPORTED;
}

AiUInt32 mil_tasks_foddl_start(TY_API_DEV* p_api_dev)
{
	return API_ERR_OS_NOT_SUPPORTED;
}

AiUInt32 mil_tasks_foddl_stop(TY_API_DEV* p_api_dev)
{
	return API_ERR_OS_NOT_SUPPORTED;
}
