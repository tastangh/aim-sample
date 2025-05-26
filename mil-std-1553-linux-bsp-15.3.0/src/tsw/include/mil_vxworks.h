/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */



typedef struct ty_bus_load {
            AiUInt32 uiLastWordCountPrimary[MAX_BIU];
            AiUInt32 uiLastWordCountSecondary[MAX_BIU];
            AiUInt32 uiTotalBusloadPrimary[MAX_BIU];
            AiUInt32 uiTotalBusloadSecondary[MAX_BIU];
            AiUInt32 uiBusloadSampleCountPrimary[MAX_BIU];
            AiUInt32 uiBusloadSampleCountSecondary[MAX_BIU];
} TY_BUS_LOAD;



struct aim_mil_device
{
#ifndef __RTP__	
	spinlockIsr_t pSpinLock;
#endif
	
	int dataQueueTimerStop;

	int busloadCalcStop;
	TY_BUS_LOAD xBusLoadContext;
	
  SEM_ID FirmwareExecutionSemId;  /* firmware execution mutex ID*/
  SEM_ID eSmartSemId;  			  /* eSmart command mutex ID*/
};

AiUInt32 mil_tasks_dataqueue_process_thread(TY_API_DEV* p_api_dev);
AiUInt32 mil_tasks_dataqueue_timer_start(TY_API_DEV* p_api_dev);
AiUInt32 mil_tasks_dataqueue_timer_stop(TY_API_DEV* p_api_dev);

AiInt16 _AmcVmeTswPollingThread(TY_API_DEV* p_api_dev);
AiUInt32 mil_tasks_busload_calc_start(TY_API_DEV* p_api_dev, AiUInt8 stream);
AiUInt32 mil_tasks_busload_calc_stop(TY_API_DEV* p_api_dev, AiUInt8 stream);
