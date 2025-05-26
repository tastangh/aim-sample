/* SPDX-FileCopyrightText: 2018-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver_aye_milscope.c
    This file contains hw layer implementations for AyE without ASP (Milscope part).

    mil_hw_pci_driver.c               => Generic functions
    mil_hw_pci_driver_aye.c           => AyE related
    mil_hw_pci_driver_aye_pxi.c       => AyE PXI and Trigger related
    mil_hw_pci_driver_aye_milscope.c  => AyE Milscope related
    mil_hw_pci_driver_ays.c           => AyS related
    mil_hw_pci_driver_artix7.c        => Artix7 related
    mil_hw_pci_driver_artixus.c       => Artix UltraScale+ related
 */




#if defined(_AIM_SYSDRV) || defined (_AIM_VME) || defined (_AIM_LVRT)
/* PCI system driver and VME driver library only */

/* TSW */
#include "mil_tsw_includes.h"
#include "hw/AiHwAye.h"

#include "mil_hw_pci_driver.h"


/*! \brief Checks for scope offset compensation values being correct

    This function checks, if all scope offsets are in range
    \param p_ScopeOffsets structure that contains offsets to check
    \return returns TRUE if all offsets are valid, FALSE otherwise */
static AiBoolean INLINE(scope_offset_check(TY_API_SCOPE_OFFSETS* p_ScopeOffsets))
{
    if(!p_ScopeOffsets)
    {
        return FALSE;
    }

    if(abs(p_ScopeOffsets->lOffsetA_3V_stub) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetB_3V_stub) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetA_30V_stub) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetB_30V_stub) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetA_3V_ext) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetB_3V_ext) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetA_30V_ext) > AYE_MS_MAX_OFFSET_COMP
        || abs(p_ScopeOffsets->lOffsetB_30V_ext) > AYE_MS_MAX_OFFSET_COMP)
    {
        return FALSE;
    }

    return TRUE;
}



/***************************************************************************
 ***                                                                     ***
 *** Implementations for all PCI based cards that have the TSW running   ***
 *** in the system driver.                                               ***
 ***                                                                     ***
 *** AEC, AXC, AMCX, APE, AME, AMEE                                      ***
 ***                                                                     ***
 *** Functions interface defined in mil_hw.h                             ***
 *** Functions must start with : mil_hw_xxxxx()                          ***
 ***                                                                     ***
 ***************************************************************************/



/*! \brief Sets coupling mode of scope capable boards

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param p_scope_setup pointer to \ref TY_API_SCOPE_SETUP structure which contains coupling settings
    \return returns 0 on succes an appropriate error code on failure */
AiUInt32 mil_hw_scope_setup_coupling(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup);


/*! \brief Sets the discret input trigger for scope capable boards

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param p_scope_trigger pointer to \ref TY_API_SCOPE_TRG_EX structure that contains the trigger settings 
    \return returns 0 on succes an appropriate error code on failure */
AiUInt32 mil_hw_scope_trigger_discretes(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger);


/*! \brief Sets the monitor trigger strobe for scope capable boards

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param destination_biu BIU to set the monitor strobe to 
    \return returns 0 on succes an appropriate error code on failure */
AiUInt32 mil_hw_scope_trigger_strobe_mon(TY_API_DEV* p_api_dev, AiUInt32 destination_biu);


/*! \brief Sets the bus controller trigger strobe for scope capable boards

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param destination_biu BIU to set the bus controller strobe to 
    \return returns 0 on succes an appropriate error code on failure */
AiUInt32 mil_hw_scope_trigger_strobe_bc(TY_API_DEV* p_api_dev, AiUInt32 destination_biu);


/*! \brief Sets the discrete trigger strobe for scope capable boards

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param discrete_channel discrete channel to issue strobe on
    \return returns 0 on succes an appropriate error code on failure */
AiUInt32 mil_hw_scope_trigger_strobe_discretes(TY_API_DEV* p_api_dev, AiUInt32 discrete_channel);




/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                      HW Layer public TSW MILScope                         *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


AiUInt32 mil_hw_scope_setup(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup)
{
    volatile AiUInt32* p_IoMemory = NULL;
    AiUInt32 return_val = API_ERR_NAK;

    if(!p_api_dev || !p_scope_setup)
    {
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_WRONG_DEVICE;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* set scope acquisition register according to requested operation mode */
    switch(p_scope_setup->ul_OperationMode)
    {
    case API_SCOPE_OPR_CHANNEL_DISABLED:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_DISABLED;
        break;
    case API_SCOPE_OPR_CHANNEL_A:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_PRIMARY_50M;
        break;
    case API_SCOPE_OPR_CHANNEL_B:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_SECONDARY_50M;
        break;
    case API_SCOPE_OPR_CHANNEL_A100:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_PRIMARY_100M;
        break;
    case API_SCOPE_OPR_CHANNEL_B100:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_SECONDARY_100M;
        break;
    case API_SCOPE_OPR_CHANNEL_AB:
        p_IoMemory[AYE_REG_MS_ACQUISITION] = MS_ACQ_DUAL_50M;
        break;
    default:
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* Now set up coupling mode */
    return_val = mil_hw_scope_setup_coupling(p_api_dev, p_scope_setup);
    if(return_val)
    {
        return return_val;
    }

    /* validate sampling rate and set sampling rate register */
    if(p_scope_setup->ul_SamplingRate > 0xFF)
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }
    p_IoMemory[AYE_REG_MS_SAMPLING_RATE] = p_scope_setup->ul_SamplingRate;

    return 0;
}

static AiUInt32 mil_hw_scope_trigger_physical(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    TY_AYE_MS_TRIGGER_SELECT trigger = {0};
    AiUInt32 triggerValue            = 0;
    AiUInt32 triggerValue2           = 0;
    AiUInt32 numberOfSamples1        = 0;
    AiUInt32 numberOfSamples2        = 0;
    volatile AiUInt32* p_IoMemory             = NULL;

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* read trigger register to preserve previous settings */
    trigger.all = p_IoMemory[AYE_REG_MS_TRIGGER];

    trigger.settings.trigger_source         = 1;  /* Physical trigger */

    if(p_scope_trigger->ul_TrgSrc == API_SCOPE_SRC_PRIMARY)
    {
        trigger.settings.physical_trigger = MS_TRIGGER_PRIMARY_CHANNEL;
    }
    else if(p_scope_trigger->ul_TrgSrc == API_SCOPE_SRC_SECONDARY)
    {
        trigger.settings.physical_trigger = MS_TRIGGER_SECONDARY_CHANNEL;
    }
    else
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    triggerValue  = p_scope_trigger->ul_TrgValue;
    triggerValue2 = p_scope_trigger->ul_TrgValue2;

    if(triggerValue > AYE_MS_MAX_SAMPLE_VALUE || triggerValue2 > AYE_MS_MAX_SAMPLE_VALUE)
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* set the parameters for each individual case */

    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_GREATER_THAN )
    {
      trigger.settings.physical_trigger_mode  = MS_PTMS_HIGHER;
    }


    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_LESS_THAN )
    {
      trigger.settings.physical_trigger_mode  = MS_PTMS_LOWER;
    }


    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_GREATER_OR_LESS_THAN )
    {
      trigger.settings.physical_trigger_mode  = MS_PTMS_HIGHER1_LOWER2;
    }


    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_GREATER_THAN_SAMPLES )
    {
      trigger.settings.physical_trigger_mode  = MS_PTMS_HIGH_PULSE;
      numberOfSamples1                        = p_scope_trigger->ul_TrgNbSamples;
    }


    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_LESS_THAN_SAMPLES )
    {
      trigger.settings.physical_trigger_mode  = MS_PTMS_LOW_PULSE;
      numberOfSamples1                        = p_scope_trigger->ul_TrgNbSamples;
    }

    if( p_scope_trigger->ul_TrgMode == API_SCOPE_MODE_COMMAND_SYNC )
    {
        trigger.settings.physical_trigger_mode = MS_PTMS_COMMAND_SYNC;
        numberOfSamples1 = p_scope_trigger->ul_TrgNbSamples;
        numberOfSamples2 = p_scope_trigger->ul_TrgNbSamples2;
    }

    /* write the registers */
    p_IoMemory[AYE_REG_MS_TRIGGER]        = trigger.all;
    p_IoMemory[AYE_REG_MS_TRIGGER_VALUE1] = triggerValue;
    p_IoMemory[AYE_REG_MS_TRIGGER_VALUE2] = triggerValue2;

    if( numberOfSamples1 )
    {
        if(numberOfSamples1 > AYE_MS_MAX_PHYSICAL_PULSE)
        {
            return API_ERR_PARAMETER_OUT_OF_RANGE;
        }
        p_IoMemory[AYE_REG_MS_TRIGGER_NO_OF_SAMPLES1] = numberOfSamples1;
    }

    if(numberOfSamples2)
    {
        if(numberOfSamples2 > AYE_MS_MAX_SYNC_GAP)
        {
            return API_ERR_PARAMETER_OUT_OF_RANGE;
        }
        p_IoMemory[AYE_REG_MS_TRIGGER_NO_OF_SAMPLES2] = numberOfSamples2;
    }

    return 0;
}

static AiUInt32 mil_hw_scope_trigger_protocol(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    TY_AYE_MS_TRIGGER_SELECT trigger = {0};
    volatile AiUInt32* p_IoMemory             = NULL;

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* read the trigger register to preserve previous settings */
    trigger.all = p_IoMemory[AYE_REG_MS_TRIGGER];

    trigger.settings.trigger_source = 2; /* protocol trigger */

    switch(p_scope_trigger->ul_TrgMode)
    {
    case API_SCOPE_MODE_BIU_BM:
        trigger.settings.protocol_trigger = AYE_MS_TRIGGER_BIU_BM;
        break;
    case API_SCOPE_MODE_BIU_BC:
        trigger.settings.protocol_trigger = AYE_MS_TRIGGER_BIU_BC;
        break;
    case API_SCOPE_MODE_BIU2_BM:
        trigger.settings.protocol_trigger = AYE_MS_TRIGGER_BIU2_BM;
        break;
    case API_SCOPE_MODE_BIU2_BC:
        trigger.settings.protocol_trigger = AYE_MS_TRIGGER_BIU2_BC;
        break;
    default:
        return API_ERR_INVALID_MODE;
    }

    /* write the trigger register */
    p_IoMemory[AYE_REG_MS_TRIGGER] = trigger.all;
  
    return 0;
}

AiUInt32 mil_hw_scope_trigger(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    AiUInt32 return_val = API_ERR_NAK;
    TY_AYE_MS_CONTROL control = {0};
    volatile AiUInt32* p_IoMemory = NULL;
    AiUInt32 current_sample_rate = 0;
    AiUInt32 delay_in_samples = 0;

    if(!p_api_dev || !p_scope_trigger)
    {
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_WRONG_DEVICE;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    switch(p_scope_trigger->ul_TrgMode)
    {
    case API_SCOPE_MODE_BIU_BM:
    case API_SCOPE_MODE_BIU_BC:
    case API_SCOPE_MODE_BIU2_BM:
    case API_SCOPE_MODE_BIU2_BC:
        return_val = mil_hw_scope_trigger_protocol(p_api_dev, p_scope_trigger);
        break;
    
    case API_SCOPE_MODE_GREATER_THAN:
    case API_SCOPE_MODE_LESS_THAN:
    case API_SCOPE_MODE_GREATER_OR_LESS_THAN:
    case API_SCOPE_MODE_COMMAND_SYNC:
    case API_SCOPE_MODE_GREATER_THAN_SAMPLES:
    case API_SCOPE_MODE_LESS_THAN_SAMPLES:
        return_val = mil_hw_scope_trigger_physical(p_api_dev, p_scope_trigger);
        break;
    
    case API_SCOPE_MODE_DISCRETES:
        return_val = mil_hw_scope_trigger_discretes(p_api_dev, p_scope_trigger);
        break;

    default:
        return API_ERR_INVALID_MODE;
    }

    if(return_val != 0)
    {
        return return_val;
    }

    control.all = p_IoMemory[AYE_REG_MS_CONTROL];
    control.settings.ffbbt = p_scope_trigger->ul_TrgFlags;
    p_IoMemory[AYE_REG_MS_CONTROL] = control.all;

    /* Check and set trigger delay */
    if(p_scope_trigger->ul_TrgDelay > AYE_MS_MAX_TRIGGER_DELAY)
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* The trigger delay of the command is specified in microseconds,
       hence we compute the delay in samples for the specified time */
    delay_in_samples = p_scope_trigger->ul_TrgDelay * AYE_MS_SAMPLES_PER_USEC;

    /* Get current sampling rate so we can compute actual number
       of delay samples */
    current_sample_rate = p_IoMemory[AYE_REG_MS_SAMPLING_RATE];
    delay_in_samples /= (current_sample_rate + 1);

    p_IoMemory[AYE_REG_MS_TRIGGER_DELAY] = delay_in_samples;

    return return_val;
}


AiUInt32 mil_hw_scope_setup_coupling(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup)
{
    TY_AYE_MS_COUPLING coupling = {0};
    volatile AiUInt32* p_IoMemory = NULL;

    if(!p_api_dev || !p_scope_setup)
    {
        return API_ERR_NAK;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* Check if 100M recording mode is requested. If yes, the second channel
        has to be cross-coupled to the other channel on which 100M recording is requested. 
        return error if this is not the case (Maybe we could also force correct coupling without notifying caller?) */
    if(p_scope_setup->ul_OperationMode == API_SCOPE_OPR_CHANNEL_A100)
    {
        if(p_scope_setup->ul_CouplingPri == API_SCOPE_COUPLING_SECONDARY
            || p_scope_setup->ul_CouplingSec != API_SCOPE_COUPLING_SECONDARY)
        {
            return API_ERR_INVALID_MODE;
        }
    }
    else if(p_scope_setup->ul_OperationMode == API_SCOPE_OPR_CHANNEL_B100)
    {
        if(p_scope_setup->ul_CouplingPri != API_SCOPE_COUPLING_SECONDARY
            || p_scope_setup->ul_CouplingSec == API_SCOPE_COUPLING_SECONDARY)
        {
            return API_ERR_INVALID_MODE;
        }
    }

    /* set coupling on primary channel */
    switch(p_scope_setup->ul_CouplingPri)
    {
    case API_SCOPE_COUPLING_SECONDARY:
        coupling.settings.primary_to_signal_source = FALSE;
        break;
    case API_SCOPE_COUPLING_EXTERN:
        coupling.settings.primary_to_signal_source = TRUE;
        coupling.settings.primary_3V = FALSE;
        coupling.settings.primary_to_stub = FALSE;
        break;
    case API_SCOPE_COUPLING_STUB:
        coupling.settings.primary_3V = FALSE;
        coupling.settings.primary_to_signal_source = TRUE;
        coupling.settings.primary_to_stub = TRUE;
        break;
    case API_SCOPE_COUPLING_STUB_3V:
        coupling.settings.primary_3V = TRUE;
        coupling.settings.primary_to_signal_source = TRUE;
        coupling.settings.primary_to_stub = TRUE;
        break;
    case API_SCOPE_COUPLING_EXTERN_3V:
        coupling.settings.primary_3V = TRUE;
        coupling.settings.primary_to_signal_source = TRUE;
        coupling.settings.primary_to_stub = FALSE;
        break;
    default:
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* set up coupling on secondary channel */
    switch(p_scope_setup->ul_CouplingSec)
    {
    case API_SCOPE_COUPLING_SECONDARY:
        coupling.settings.secondary_to_signal_source = FALSE;
        break;
    case API_SCOPE_COUPLING_EXTERN:
        coupling.settings.secondary_to_signal_source = TRUE;
        coupling.settings.secondary_3V = FALSE;
        coupling.settings.secondary_to_stub = FALSE;
        break;
    case API_SCOPE_COUPLING_STUB:
        coupling.settings.secondary_3V = FALSE;
        coupling.settings.secondary_to_signal_source = TRUE;
        coupling.settings.secondary_to_stub = TRUE;
        break;
    case API_SCOPE_COUPLING_STUB_3V:
        coupling.settings.secondary_3V = TRUE;
        coupling.settings.secondary_to_signal_source = TRUE;
        coupling.settings.secondary_to_stub = TRUE;
        break;
    case API_SCOPE_COUPLING_EXTERN_3V:
        coupling.settings.secondary_3V = TRUE;
        coupling.settings.secondary_to_signal_source = TRUE;
        coupling.settings.secondary_to_stub = FALSE;
        break;
    default:
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* Now set value in scope coupling register */
    p_IoMemory[AYE_REG_MS_COUPLING] = coupling.all;

    return 0;
}


AiUInt32 mil_hw_scope_start(TY_API_DEV* p_api_dev, AiUInt32 ulMode, AiUInt32 ulNoOfCycles )
{
    volatile AiUInt32* p_IoMemory       = NULL;
    TY_AYE_MS_CONTROL xControl = {0};
    TY_AYE_MS_STATUS  xStatus  = {0};

    if(!p_api_dev)
    {
        return API_ERR_NAK;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_NAK;
    }

    if(p_api_dev->uc_DeviceType != TYPE_APE1553_1 &&
        p_api_dev->uc_DeviceType != TYPE_APE1553_2)
    {
        /* Only APE-1 and -2 are scope capable at this time */
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    /* Check if scope is already running or in reset.
        If this is the case return error */
    xStatus.all = p_IoMemory[AYE_REG_MS_STATUS];
    if( xStatus.settings.state == AYE_MS_STATUS_IDLE )
    {
        if( ulMode == API_SCOPE_START_SINGLE )
        {
            p_IoMemory[AYE_REG_MS_NO_OF_CYCLES] = ulNoOfCycles;
        }
        else if(ulMode == API_SCOPE_START_CONTINUOUS)
        {
            p_IoMemory[AYE_REG_MS_NO_OF_CYCLES] = 0;
        }
        else
        {
            return API_ERR_INVALID_MODE;
        }

        xControl.all                        = p_IoMemory[AYE_REG_MS_CONTROL];
        xControl.settings.start             = 1;
        p_IoMemory[AYE_REG_MS_CONTROL]      = xControl.all;
    }
    else if(xStatus.settings.state == AYE_MS_STATUS_RESET)
    {
        return API_ERR_INVALID_DEVICE_STATE;
    }
    else 
    {
        return API_ERR_SCOPE_ALREADY_RUNNING;
    }


    return 0;
}


AiUInt32 mil_hw_scope_status(TY_API_DEV* p_api_dev, TY_API_SCOPE_STATUS* p_Status)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_STATUS scope_status = {0};
    
    if(!p_api_dev || !p_Status)
    {
        return API_ERR;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    scope_status.all = p_IoMemory[AYE_REG_MS_STATUS];

    /* Check if a DMA overflow occured and then
        return at once */
    if(scope_status.settings.dma_fail)
    {
        *p_Status = SCOPE_STATUS_OVERFLOW;
        return 0;
    }

    /*! convert hardware scope states to states available on API level */
    switch(scope_status.settings.state)
    {
    case AYE_MS_STATUS_IDLE:
        *p_Status = SCOPE_STATUS_STOPPED;
        break;
    case AYE_MS_STATUS_START:
    case AYE_MS_STATUS_DELAY_ACQ:
    case AYE_MS_STATUS_WAIT_BLOCK_FULL:
    case AYE_MS_STATUS_WAIT_TRIGGER:
        *p_Status = SCOPE_STATUS_WAIT_FOR_TRIGGER;
        break;
    case AYE_MS_STATUS_ACQ_BLOCK1:
    case AYE_MS_STATUS_ACQ_BLOCK2:
        *p_Status = SCOPE_STATUS_TRIGGERED;
        break;
    default:
        return API_ERR;
    }

    return 0;
}


AiUInt32 mil_hw_scope_stop(TY_API_DEV* p_api_dev)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_CONTROL control_reg = {0};
    TY_API_SCOPE_STATUS scope_status = SCOPE_STATUS_WAIT_FOR_TRIGGER; /* == 0*/
    TY_AYE_MS_DMA_CONTROL scope_dma_control = {0};
    TY_AI_TSW_OS_TIMER * timer = NULL;
    AiUInt32 ret_val = API_ERR_NAK;
    AiBoolean scope_stopped = FALSE;
    AiBoolean scope_dma_reset = FALSE;

    if(!p_api_dev)
    {
        return API_ERR_NAK;
    }

    timer = ai_tsw_os_timer_create( p_api_dev->p_Device );

    if( timer == NULL )
      {
        return API_ERR_NAK;
      }


    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    /* stop scope via control register */
    control_reg.all = p_IoMemory[AYE_REG_MS_CONTROL];
    control_reg.settings.stop = TRUE;
    p_IoMemory[AYE_REG_MS_CONTROL] = control_reg.all;

    /* wait until scope is eventually stopped */
    ret_val = ai_tsw_os_timer_start( timer, 1000);
    if(ret_val)
    {
        ai_tsw_os_timer_destroy(timer);
        return ret_val;
    }

    do
    {
        ret_val = mil_hw_scope_status(p_api_dev,&scope_status);
        if(ret_val != 0)
        {
            break;
        }

        switch(scope_status)
        {
        case SCOPE_STATUS_STOPPED:
        case SCOPE_STATUS_OVERFLOW:
            scope_stopped = TRUE;
            break;
        default:
            break;
        }
    
    }while(!scope_stopped && !ai_tsw_os_timer_elapsed(timer));

    if(ret_val != 0)
    {
        ai_tsw_os_timer_stop(timer);
        ai_tsw_os_timer_destroy(timer);
        return ret_val;
    }

    ret_val = ai_tsw_os_timer_stop(timer);
    if(ret_val)
    {
        ai_tsw_os_timer_destroy(timer);
        return ret_val;
    }
   
    if(scope_stopped)
    {
        /* Now reset scope DMA engine, so all scatter list entries
           in its FIFO are invalidated */
        scope_dma_control.all = p_IoMemory[AYE_REG_DMA_MS_CONTROL];
        scope_dma_control.settings.reset = TRUE;
        p_IoMemory[AYE_REG_DMA_MS_CONTROL] = scope_dma_control.all;

        /* Wait until scope DMA has been reset */
        ret_val = ai_tsw_os_timer_start(timer, 1000);
        if(ret_val)
        {
            ai_tsw_os_timer_destroy(timer);
            return ret_val;
        }

        scope_dma_reset = FALSE;
        while(!scope_dma_reset && !ai_tsw_os_timer_elapsed(timer))
        {
            scope_dma_control.all = p_IoMemory[AYE_REG_DMA_MS_CONTROL];
            if(!scope_dma_control.settings.reset)
            {
                scope_dma_reset = TRUE;
            }
        }
    
        ret_val = ai_tsw_os_timer_stop(timer);
        if(ret_val)
        {
            ai_tsw_os_timer_destroy(timer);
            return ret_val;
        }

        if(!scope_dma_reset)
        {
            ai_tsw_os_timer_destroy(timer);
            return API_ERR_TARGET_RESPONSE_TIMEOUT;
        }
    }
    else
    {
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_TARGET_RESPONSE_TIMEOUT;
    }


    ai_tsw_os_timer_destroy(timer);

    return ret_val;
}


AiUInt32 mil_hw_scope_trigger_discretes(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_TRIGGER_SELECT trigger = {0};

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    if(p_scope_trigger->ul_TrgDiscrete > AYE_MS_MAX_DISCRETES)
    {
        return API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    /* read trigger register. set right values and write it back */
    trigger.all = p_IoMemory[AYE_REG_MS_TRIGGER];

    trigger.settings.trigger_source = 3; /* External Trigger */
    trigger.settings.external_trigger_source = p_scope_trigger->ul_TrgDiscrete;

    p_IoMemory[AYE_REG_MS_TRIGGER] = trigger.all;

    return 0;
}



AiUInt32 mil_hw_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_STROBE* p_scope_trigger_strobe)
{
    AiUInt32 ret_val = API_ERR_NAK;

    if(!p_api_dev || !p_scope_trigger_strobe)
    {
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_WRONG_DEVICE;
    }

    /* branch off into strobe type specific function or return error
       on invalid type */
    switch(p_scope_trigger_strobe->ul_TrgStrobeType)
    {
    case API_SCOPE_TRG_STROBE_MON:
        ret_val = mil_hw_scope_trigger_strobe_mon(p_api_dev, p_scope_trigger_strobe->ul_TrgStrobeDest);
        break;
    case API_SCOPE_TRG_STROBE_BC:
        ret_val = mil_hw_scope_trigger_strobe_bc(p_api_dev, p_scope_trigger_strobe->ul_TrgStrobeDest);
        break;
    case API_SCOPE_TRG_STROBE_DISCRETE:
        ret_val = mil_hw_scope_trigger_strobe_discretes(p_api_dev, p_scope_trigger_strobe->ul_TrgStrobeDest);
        break;
    default:
        return API_ERR_INVALID_MODE;
    }

    return ret_val;
}


AiUInt32 mil_hw_scope_trigger_strobe_mon(TY_API_DEV* p_api_dev, AiUInt32 destination_biu)
{
    TY_AYE_MS_TRIGGER_STROBE trigger_strobe_reg = {0};
    volatile AiUInt32* p_IoMemory = NULL;

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* Read trigger strobe register from target, adjust the BIU specific
       strobe settings and write register back */
    trigger_strobe_reg.all = p_IoMemory[AYE_REG_MS_TRIGGER_STROBE];

    switch(destination_biu)
    {
    case 1:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_BM_1;
        break;
    case 2:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_BM_2;
        break;
    default:
        return API_ERR_BIU_OUT_OF_RANGE;
    }

    p_IoMemory[AYE_REG_MS_TRIGGER_STROBE] = trigger_strobe_reg.all;

    return 0;
}


AiUInt32 mil_hw_scope_trigger_strobe_bc(TY_API_DEV* p_api_dev, AiUInt32 destination_biu)
{
    TY_AYE_MS_TRIGGER_STROBE trigger_strobe_reg = {0};
    volatile AiUInt32* p_IoMemory = NULL;

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* Read trigger strobe register from target, adjust the BIU specific
       strobe settings and write register back */
    trigger_strobe_reg.all = p_IoMemory[AYE_REG_MS_TRIGGER_STROBE];

    switch(destination_biu)
    {
    case 1:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_BC_1;
        break;
    case 2:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_BC_2;
        break;
    default:
        return API_ERR_BIU_OUT_OF_RANGE;
    }

    p_IoMemory[AYE_REG_MS_TRIGGER_STROBE] = trigger_strobe_reg.all;

    return 0;
}


AiUInt32 mil_hw_scope_trigger_strobe_discretes(TY_API_DEV* p_api_dev, AiUInt32 discrete_channel)
{
    TY_AYE_MS_TRIGGER_STROBE trigger_strobe_reg = {0};
    volatile AiUInt32* p_IoMemory = NULL;

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_MEMORY_INVALID;
    }

    /* Read trigger strobe register from target, adjust the discrete channel specific
       strobe settings and write register back */
    trigger_strobe_reg.all = p_IoMemory[AYE_REG_MS_TRIGGER_STROBE];

    switch(discrete_channel)
    {
    case 1:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_1;
        break;
    case 2:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_2;
        break;
    case 3:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_3;
        break;
    case 4:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_4;
        break;
    case 5:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_5;
        break;
    case 6:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_6;
        break;
    case 7:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_7;
        break;
    case 8:
        trigger_strobe_reg.settings.strobe = AYE_MS_TRIGGER_STROBE_DISCRETE_8;
        break;
    default:
        return API_ERR_INVALID_DISCRETE;
    }

    p_IoMemory[AYE_REG_MS_TRIGGER_STROBE] = trigger_strobe_reg.all;

    return 0;
}


AiUInt32 mil_hw_scope_reset(TY_API_DEV* p_api_dev)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_CONTROL xControl = {0};
    AiUInt32 ret_val = API_ERR_NAK;
    TY_AI_TSW_OS_TIMER * timer = NULL;
    AiBoolean scope_reset = FALSE;

    if(!p_api_dev)
    {
        return API_ERR_NAK;
    }

    timer = ai_tsw_os_timer_create( p_api_dev->p_Device );

    if( timer == NULL )
      {
        return API_ERR_NAK;
      }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    /* Reset scope functionality via control register */
    xControl.all = p_IoMemory[AYE_REG_MS_CONTROL];
    xControl.settings.reset = TRUE;
    p_IoMemory[AYE_REG_MS_CONTROL] = xControl.all;

     /* Wait until scope is eventually reseted.
        This is the case if LCA cleared reset flag
        in control register */
    ret_val = ai_tsw_os_timer_start(timer, 1000);
    if(ret_val)
    {
        ai_tsw_os_timer_destroy(timer);
        return ret_val;
    }

    do
    {
        xControl.all = p_IoMemory[AYE_REG_MS_CONTROL];
        if(xControl.settings.reset == FALSE)
        {
            scope_reset = TRUE;
        }

    }while(!scope_reset && !ai_tsw_os_timer_elapsed(timer));

    ret_val = ai_tsw_os_timer_stop(timer);
    if(ret_val)
    {
        ai_tsw_os_timer_destroy(timer);
        return ret_val;
    }

    if(!scope_reset)
    {
        ai_tsw_os_timer_destroy(timer);
        return API_ERR_TARGET_RESPONSE_TIMEOUT;
    }

    ai_tsw_os_timer_destroy(timer);
    return ret_val;
}


AiUInt32 mil_hw_scope_set_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_OFFSETS scope_offset_reg = {0};

    if(!p_api_dev)
    {
        return API_ERR_NAK;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    if(!scope_offset_check(p_scope_offsets))
    {
        return API_ERR_VALUE_NOT_IN_RANGE;
    }

    /* Prepare offset register with offset values for stub coupled mode and
        write the final value to stub offset register in LCA */
    scope_offset_reg.settings.abs_primary_val_3V = abs(p_scope_offsets->lOffsetA_3V_stub);
    p_scope_offsets->lOffsetA_3V_stub < 0 ? scope_offset_reg.settings.primary_negative_offset_3V = 1 : 0;

    scope_offset_reg.settings.abs_primary_val_30V = abs(p_scope_offsets->lOffsetA_30V_stub);
    p_scope_offsets->lOffsetA_30V_stub < 0 ?   scope_offset_reg.settings.primary_negative_offset_30V = 1 : 0;

    scope_offset_reg.settings.abs_secondary_val_3V = abs(p_scope_offsets->lOffsetB_3V_stub);
    p_scope_offsets->lOffsetB_3V_stub < 0 ? scope_offset_reg.settings.secondary_negative_offset_3V = 1 : 0;

    scope_offset_reg.settings.abs_secondary_val_30V = abs(p_scope_offsets->lOffsetB_30V_stub);
    p_scope_offsets->lOffsetB_30V_stub < 0 ? scope_offset_reg.settings.secondary_negative_offset_30V = 1 : 0;

    p_IoMemory[AYE_REG_MS_OFFSETS_STUB] = scope_offset_reg.all;


     /* Prepare offset register with offset values for extern coupled mode and
        write the final value to extern offset register in LCA */
    scope_offset_reg.all = 0;

    scope_offset_reg.settings.abs_primary_val_3V = abs(p_scope_offsets->lOffsetA_3V_ext);
    p_scope_offsets->lOffsetA_3V_ext < 0 ? scope_offset_reg.settings.primary_negative_offset_3V = 1 : 0;

    scope_offset_reg.settings.abs_primary_val_30V = abs(p_scope_offsets->lOffsetA_30V_ext);
    p_scope_offsets->lOffsetA_30V_ext < 0 ?   scope_offset_reg.settings.primary_negative_offset_30V = 1 : 0;

    scope_offset_reg.settings.abs_secondary_val_3V = abs(p_scope_offsets->lOffsetB_3V_ext);
    p_scope_offsets->lOffsetB_3V_ext < 0 ? scope_offset_reg.settings.secondary_negative_offset_3V = 1 : 0;

    scope_offset_reg.settings.abs_secondary_val_30V = abs(p_scope_offsets->lOffsetB_30V_ext);
    p_scope_offsets->lOffsetB_30V_ext < 0 ? scope_offset_reg.settings.secondary_negative_offset_30V = 1 : 0;

    p_IoMemory[AYE_REG_MS_OFFSETS_EXTERN] = scope_offset_reg.all;
    
    return 0;
}


AiUInt32 mil_hw_scope_get_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets)
{
    volatile AiUInt32* p_IoMemory = NULL;
    TY_AYE_MS_OFFSETS scope_offset_reg = {0};

    if(!p_api_dev)
    {
        return API_ERR_NAK;
    }

    p_IoMemory = p_api_dev->IoRAMBase;
    if(!p_IoMemory)
    {
        return API_ERR_NAK;
    }

    if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_AYE)
    {
        /* No DS capable APE board */
        return API_ERR_FUNCTION_NOT_SUPPORTED;
    }

    /* Read LCA offset register for stub coupled mode
       and extract the values to API structure */
    scope_offset_reg.all = p_IoMemory[AYE_REG_MS_OFFSETS_STUB];

    p_scope_offsets->lOffsetA_3V_stub = scope_offset_reg.settings.abs_primary_val_3V;
    scope_offset_reg.settings.primary_negative_offset_3V == 1 ? p_scope_offsets->lOffsetA_3V_stub *= (-1) : 0;
  
    p_scope_offsets->lOffsetA_30V_stub = scope_offset_reg.settings.abs_primary_val_30V;
    scope_offset_reg.settings.primary_negative_offset_30V == 1 ? p_scope_offsets->lOffsetA_30V_stub *= (-1) : 0;

    p_scope_offsets->lOffsetB_3V_stub = scope_offset_reg.settings.abs_secondary_val_3V;
    scope_offset_reg.settings.secondary_negative_offset_3V == 1 ? p_scope_offsets->lOffsetB_3V_stub *= (-1) : 0;

    p_scope_offsets->lOffsetB_30V_stub = scope_offset_reg.settings.abs_secondary_val_30V;
    scope_offset_reg.settings.secondary_negative_offset_30V == 1 ? p_scope_offsets->lOffsetB_30V_stub *= (-1) : 0;


     /* Read LCA offset register for extern coupled mode
       and extract the values to API structure */
    scope_offset_reg.all = p_IoMemory[AYE_REG_MS_OFFSETS_EXTERN];

    p_scope_offsets->lOffsetA_3V_ext = scope_offset_reg.settings.abs_primary_val_3V;
    scope_offset_reg.settings.primary_negative_offset_3V == 1 ? p_scope_offsets->lOffsetA_3V_ext *= (-1) : 0;
  
    p_scope_offsets->lOffsetA_30V_ext = scope_offset_reg.settings.abs_primary_val_30V;
    scope_offset_reg.settings.primary_negative_offset_30V == 1 ? p_scope_offsets->lOffsetA_30V_ext *= (-1) : 0;

    p_scope_offsets->lOffsetB_3V_ext = scope_offset_reg.settings.abs_secondary_val_3V;
    scope_offset_reg.settings.secondary_negative_offset_3V == 1 ? p_scope_offsets->lOffsetB_3V_ext *= (-1) : 0;

    p_scope_offsets->lOffsetB_30V_ext = scope_offset_reg.settings.abs_secondary_val_30V;
    scope_offset_reg.settings.secondary_negative_offset_30V == 1 ? p_scope_offsets->lOffsetB_30V_ext *= (-1) : 0;

    return 0;
}


AiUInt32 mil_hw_scope_calibrate( TY_API_DEV * p_api_dev, AiUInt8 ucMode )
{
    /* For Boards without ASP the calibration is done within the DLL */
    return API_ERR_FUNCTION_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_type_get( TY_API_DEV *p_api_dev )
{
    AiUInt32 ulMilscope = API_MILSCOPE_TYPE_NONE;

    if (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE)
    {
        switch (p_api_dev->BoardSubType & 0xF)
        {
        case 0x1:                             /* HOLT transceiver HI1570PSIF, var. Amplitude    */
        case 0x2:                             /* CMAC FC1553721VSC transceiver, var. Amplitude  */
        case 0x3:                             /* HOLT transceiver HI1573PCT, fixed Amplitude    */
        case 0x5:                             /* CMAC transceiver (FC1553395) fixed Amplitude   */
        case 0x6:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. */
            ulMilscope = API_MILSCOPE_TYPE_NONE;
            break;
        case 0x4:                             /* CMAC FC1553721VSC transceiver var. Amplitude              & MILScope V02.00 PBI (only for BIU1 available) */
        case 0x7:                             /* HOLT HI1590, var.Amplitude with Fine Tuning range/switch. & MILScope V03.xx PBI (only for BIU1 available)*/
            ulMilscope = API_MILSCOPE_TYPE_AYE;
            break;
        default:
            ulMilscope = API_MILSCOPE_TYPE_NONE;
            break;
        }
    }

    return ulMilscope;
}



#endif /* defined(_AIM_SYSDRV) */
