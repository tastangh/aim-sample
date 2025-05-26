/* SPDX-FileCopyrightText: 2018-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_driver.c
    This file contains hw layer implementations for AyE without ASP, Artix7 without ASP, Ays without ASP.

    mil_hw_pci_driver.c               => Generic functions
    mil_hw_pci_driver_aye.c           => AyE related
    mil_hw_pci_driver_aye_pxi.c       => AyE PXI and Trigger related
    mil_hw_pci_driver_aye_milscope.c  => AyE Milscope related
    mil_hw_pci_driver_ays.c           => AyS related
    mil_hw_pci_driver_artix7.c        => Artix7 related
*/

#if defined(_AIM_SYSDRV) || defined (_AIM_VME) || defined (_AIM_LVRT)
/* PCI system driver and VME driver library only */

/* TSW */
#include "mil_tsw_includes.h"
#include "hw/AiHwAye.h"

#include "mil_hw_pci_driver.h"
#include "mil_novram.h"

/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                      HW Layer internal functions                          *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
 
 
static BYTE mil_hw_initialize_memory( TY_API_DEV * p_api_dev )
{
  memset( p_api_dev, 0, sizeof(TY_API_DEV) );
  return 0;
}



/*! \brief This function initializes the the global variable container during bootup/reboot */
/*!
    This function copies global monitor variables into the 
    according p_api_dev variable. 

    The global monitor parameters contain :
    
      1. The NOVRAM values during cold boot
      
      2. The new board settings during reboot.
         (E.g. efa efex switch )

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
 static BYTE mil_hw_initialize_api_dev( TY_API_DEV *  p_api_dev, TY_API_HW_INIT_PARAMS * px_tsw_config  )
{

    p_api_dev->pxMyMonContext  = (TyMyMonContext*)px_tsw_config->pxMyMonContext;
    p_api_dev->p_DeviceContext = px_tsw_config->pDeviceContext;
    p_api_dev->p_Device        = px_tsw_config->pDevice;


    p_api_dev->SharedRAMBase   = px_tsw_config->SharedRAMBase;
    p_api_dev->shared          = px_tsw_config->SharedRAMBase;

    p_api_dev->GlobalRAMBase   = px_tsw_config->GlobalRAMBase;
    p_api_dev->GlobalRAMSize   = px_tsw_config->GlobalRAMSize;

    p_api_dev->GlobalRAMBase2  = px_tsw_config->GlobalRAMBase2;
    p_api_dev->GlobalRAMSize2  = px_tsw_config->GlobalRAMSize2;

    p_api_dev->IoRAMBase       = px_tsw_config->IORAMBase;

    p_api_dev->HostIoRAMBase   = (L_WORD *)px_tsw_config->HostIoRAMBase;

    p_api_dev->GlobalRAMExtBase = px_tsw_config->GlobalRAMExtBase;
    p_api_dev->GlobalRAMExtSize = px_tsw_config->GlobalRAMExtSize;

    p_api_dev->BoardApplicationType  = px_tsw_config->BoardAppType;
    p_api_dev->BoardApplicationType2 = px_tsw_config->BoardAppType2;
    p_api_dev->ul_DeviceGroup        = px_tsw_config->PlatformGroup;

    p_api_dev->BoardSerial     = px_tsw_config->BoardSerial;
    p_api_dev->BoardConfig     = px_tsw_config->BoardConfig;
    p_api_dev->BoardType       = px_tsw_config->BoardType;
    p_api_dev->BoardSubType    = px_tsw_config->BoardSubType;
    p_api_dev->BoardSubType2   = px_tsw_config->BoardSubType2;
    p_api_dev->BoardPartNo     = px_tsw_config->BoardPartNo;
    p_api_dev->CPUClk          = px_tsw_config->CPUClk;
    p_api_dev->SYSClk          = px_tsw_config->SYSClk;
    p_api_dev->HwVariant       = px_tsw_config->HwVariant;

    /*** Init Target Communication pointers and area ***/
    p_api_dev->targ_com_reg_p  = NULL; /* Unused */
    p_api_dev->cmd_p           = NULL; /* Unused */
    p_api_dev->ack_p           = NULL; /* Unused */

    p_api_dev->clk_1us         = 1;


    return 0;
}




/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *            HW Layer public TSW bootup/initialization                      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief This function does the complete initialization of the TSW */
/*!
    This function does the complete initialization of the TSW
    \callgraph
   
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  px_tsw_config  TY_API_HW_INIT_PARAMS*  The initial values required to start the TSW
    
    \return                BYTE                    Zero on normal termination
*/
BYTE mil_hw_initialize_tsw( TY_API_DEV *  p_api_dev, TY_API_HW_INIT_PARAMS * px_tsw_config )
{
    mil_hw_initialize_memory( p_api_dev );
    mil_hw_initialize_api_dev( p_api_dev, px_tsw_config );

    api_main_initialize_stream_type( p_api_dev );

    mil_hw_init_irig_capabilities(     p_api_dev );
    mil_hw_init_coupling_capabilities( p_api_dev );

    api_main_setup_memory_layout( p_api_dev );
    api_main_init_and_reset(      p_api_dev );



    /* Last call in initialization / api_opr can be called now */
    p_api_dev->repeat = API_ON;
    mil_hw_signal_asp_running( p_api_dev );
    return 0;
}


/*! \brief This function shuts the TSW down and frees the allocated memory */
/*!
    This function shuts the TSW down and frees the allocated memory 
    
    \callgraph

    \param  p_api_dev      TY_API_DEV*             The global variable container.
    
    \return                BYTE                    Zero on normal termination
*/
BYTE mil_hw_shutdown_tsw( TY_API_DEV * p_api_dev )
{
    /* -- free allocated memory that is stored in p_api_dev pointers --- */
    api_main_free_allocated_memory( p_api_dev );
    return 0;
}


/*! \brief Soft-Reboot TSW */
/*!
    This function sets the repeat parameter of p_api_dev to
    API_OFF. The current command is properly acknowleged to the
    host but no new driver commands are accessed until the reboot
    has finished. \see mil_hw_signal_asp_running

    This function will call \see mil_hw_signal_asp_stopped

    After the reboot the device will be initialized and reset.
    All old setup information will be lost and the device will
    be operational. No additional init/reset required.

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_reboot_tsw( TY_API_DEV *p_api_dev )
{
    p_api_dev->repeat = API_OFF;

    mil_hw_signal_asp_stopped(p_api_dev);
    return 0;
}


/*! \brief This function signals that the TSW is running. */
/*!
    This is required in two cases. 
    
      1. The driver is loaded:
         The interrupts must not be installed before the TSW is ready.
         The host to target area must not be used.before the TSW is ready.
      
      2. The TSW is rebooted.
         When the TSW is rebooted e.g. after a efa efex switch the TSW commands
         returns immediately. The DLL can then poll the shared RAM location 0xBC
         (ul_AspRunningMagic) until the MIL_COM_ASP_RUNNING magic is found.

    
    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return void
*/
void mil_hw_signal_asp_running( TY_API_DEV * p_api_dev )
{
    TY_HW_INFORMATION * pHwInfo = (TY_HW_INFORMATION *)p_api_dev->SharedRAMBase;

    pHwInfo->ul_AspRunningMagic = MIL_COM_ASP_RUNNING;
}


/*! \brief This function signals that the TSW is stopped. */
/*!
    This is required in one cases. 
    
      1. The TSW is rebooted.
         When the reboot starts the TSW will clear the shared RAM 
         location 0xBC (ul_AspRunningMagic). This location well be
         reset when the reboot is finished.
    
    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return void
*/
void mil_hw_signal_asp_stopped( TY_API_DEV * p_api_dev )
{
    TY_HW_INFORMATION * pHwInfo = (TY_HW_INFORMATION *)p_api_dev->SharedRAMBase;

    pHwInfo->ul_AspRunningMagic = 0;

}






/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW EFA/EFEx switch                       *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


/*! \brief Check if this board type supports efa efex switching */
/*! 
    \param  p_api_dev        TY_API_DEV*    The global variable container.
    
    \return BYTE  One if switch is available and zero if not.
*/
BYTE mil_hw_switch_available( TY_API_DEV *p_api_dev )
{
  return 0;
}






/*! \brief Store board type parameter in global variables and NOVRAM for reboot. */
/*!
    During the reboot the global variable container p_api_dev 
    is cleared with zeros. We need store the configuration
    that we want to load after the reboot in the global
    monitor variables. This is possible because the global 
    monitor variables are not re-read from the NOVRAM during
    the reboot.

    \param  p_api_dev     TY_API_DEV*      The global variable container.
    \param  ucPermanent   BYTE             Update the NOVRAM is value is true.
    \param  ulBoardType   AiUInt32         The board type value to set.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_set_globals_bt( TY_API_DEV * p_api_dev,
                            BYTE         ucPermanent,
                            AiUInt32     ulBoardType )
{
    return 0;
}



 /*! \brief Store PBI1 parameters in global variables for reboot. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ucPermanent        BYTE             Update the NOVRAM is value is true.
    \param  ulLcaBootSector    AiUInt32         The LCA boot sector.
    \param  ulBiu1BootSector   AiUInt32         The BIU1 boot sector.
    \param  ulBiu2BootSector   AiUInt32         The BIU2 boot sector.
    \param  ulBiu1Freq         AiUInt32         The BIU1 frequency
    \param  ulBiu2Freq         AiUInt32         The BIU2 frequency
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_set_globals_pbi1( TY_API_DEV * p_api_dev,
                              BYTE         ucPermanent,
                              AiUInt32     ulLcaBootSector,
                              AiUInt32     ulBiu1BootSec,
                              AiUInt32     ulBiu2BootSec,
                              AiUInt32     ulBiu1Freq,
                              AiUInt32     ulBiu2Freq )
{
    return 0;
}



/*! \brief Store PBI2 parameters in global variables for reboot. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ucPermanent        BYTE             Update the NOVRAM is value is true.
    \param  ulLcaBootSector    AiUInt32         The LCA boot sector.
    \param  ulBiu1BootSector   AiUInt32         The BIU1 boot sector.
    \param  ulBiu2BootSector   AiUInt32         The BIU2 boot sector.
    \param  ulBiu1Freq         AiUInt32         The BIU1 frequency
    \param  ulBiu2Freq         AiUInt32         The BIU2 frequency
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_set_globals_pbi2( TY_API_DEV * p_api_dev,
                              BYTE         ucPermanent,
                              AiUInt32     ulLcaBootSector,
                              AiUInt32     ulBiu1BootSec,
                              AiUInt32     ulBiu2BootSec,
                              AiUInt32     ulBiu1Freq,
                              AiUInt32     ulBiu2Freq )
{
    return 0;
}









/*! \brief Get boot sectors for requested protocoll */
/*! 
    This function will return the boot secotors required
    for this hardware and protocoll.

    The input parameter ulProtocol might be:

    0x30 -> 3910
    0x70 -> EFEx

    \param  p_api_dev        TY_API_DEV*    The global variable container.
    \param  ulProtocol       AiUInt32       Input variable for the protocoll type
    \param  pulLsBiuBootSec  AiUInt32*      Output variable for the LS BIU boot sector.
    \param  pulHsBiuBootSec  AiUInt32*      Output variable for the HS BIU boot sector.
    \param  pulLcaBootSec    AiUInt32*      Output variable for the LCA    boot sector.
    \param  pulLsBiuFreq     AiUInt32*      Output variable for the LS BIU frequency.
    \param  pulHsBiuFreq     AiUInt32*      Output variable for the HS BIU frequency.
    
    \return void
*/
void mil_hw_switch_get_param( TY_API_DEV * p_api_dev,
                              AiUInt32     ulProtocol,
                              AiUInt32   * pulLsBiuBootSec,
                              AiUInt32   * pulHsBiuBootSec,
                              AiUInt32   * pulLcaBootSec,
                              AiUInt32   * pulLsBiuFreq,
                              AiUInt32   * pulHsBiuFreq  )
{
 
}












/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW NOVRAM access                         *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/




/*! \brief Write the given value to the NOVRAM offset. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ulOffset           AiUInt32         The NOVRAM offset
    \param  ulValue            AiUInt32         The NOVRAM value
    
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_write( TY_API_DEV * p_api_dev,
                            AiUInt32     ulOffset,
                            AiUInt32     ulValue )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_novram_write(p_api_dev, ulOffset, ulValue);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_novram_write(p_api_dev, ulOffset, ulValue);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_novram_write(p_api_dev, ulOffset, ulValue);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_novram_write(p_api_dev, ulOffset, ulValue);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}


/*! \brief Read from the given NOVRAM offset. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ulOffset           AiUInt32         The NOVRAM offset
    \param  pulValue           AiUInt32*        The NOVRAM value
    
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_read( TY_API_DEV * p_api_dev,
                           AiUInt32     ulOffset,
                           AiUInt32*    pulValue )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_novram_read(p_api_dev, ulOffset, pulValue);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_novram_read(p_api_dev, ulOffset, pulValue);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_novram_read(p_api_dev, ulOffset, pulValue);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_novram_read(p_api_dev, ulOffset, pulValue);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}

/*! \brief Start the NOVRAM write. */
/*!
    Reserved for future use.

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_start( TY_API_DEV * p_api_dev )
{
    return 0;
}

/*! \brief Flush the NOVRAM mirror. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_finalize( TY_API_DEV * p_api_dev )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_novram_finalize(p_api_dev);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_novram_finalize(p_api_dev);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_novram_finalize(p_api_dev);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_novram_finalize(p_api_dev);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}












/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW IRIG access                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


/*! \brief Check if external Irig is available on this device  */
/*!
This function returns true if external Irig signal is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on Irig support
*/
AiBoolean mil_hw_irig_has_external_input(TY_API_DEV* p_api_dev)
{
    return AiTrue;
}

 
/*! \brief This function sets the hw irig time source the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_irig_set_external(p_api_dev, con);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_irig_set_external(p_api_dev, con);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_irig_set_external(p_api_dev, con);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_irig_set_external(p_api_dev, con);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}




/*! \brief This function sets the hw irig time through the TCP. */
/*!
    This function handles the 'API_SET_IRIG_TIME' instruction to set the
    IRIG encoder of the TCP (Time Code Processor)

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  day_hi     BYTE             IRIG time encode day high
    \param  day_low    BYTE             IRIG time encode day low
    \param  hour       BYTE             IRIG time encode hour
    \param  min        BYTE             IRIG time encode minute
    \param  sec        BYTE             IRIG time encode second
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_set( TY_API_DEV * p_api_dev, 
                        L_WORD         day, 
                        L_WORD         hour, 
                        L_WORD         min, 
                        L_WORD         sec, 
                        L_WORD         us )
{
    if( (day > 366L) || (hour > 24) || (min >= 60) || (sec >= 60) || (us >= 1000000))
        return API_ERR_INVALID_TIME;

    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_irig_set(p_api_dev, day, hour, min, sec);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_irig_set(p_api_dev, day, hour, min, sec);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_irig_set(p_api_dev, day, hour, min, sec, us);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_irig_set(p_api_dev, day, hour, min, sec, us);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}



/*! \brief This function reads the hw irig time and source through the TCP. */
/*!
    This function handles the 'API_GET_IRIG_TIME' instruction to read the
    IRIG time decode information

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src )
{
    *irig_sync = API_IRIG_SYNC;
    *irig_src  = API_IRIG_ONBOARD;

    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_irig_get_status(p_api_dev, irig_sync, irig_src);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_irig_get_status(p_api_dev, irig_sync, irig_src);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_irig_get_status(p_api_dev, irig_sync, irig_src);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_irig_get_status(p_api_dev, irig_sync, irig_src);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}


/*! \brief This function reads the hw irig time through the TCP. */
/*!
    This function reads the hw irig time through the TCP.

    \param  p_api_dev   TY_API_DEV*      The global variable container.
    \param  day_hi      L_WORD *         IRIG time encode day high
    \param  day_low     L_WORD *         IRIG time encode day low
    \param  hour        L_WORD *         IRIG time encode hour
    \param  min         L_WORD *         IRIG time encode minute
    \param  sec         L_WORD *         IRIG time encode second
    \param  msec_hi     L_WORD *         IRIG time encode micro second high
    \param  msec_lo     L_WORD *         IRIG time encode micro second low
    \param  sth         L_WORD *         IRIG time encode Time Tag High
    \param  stl         L_WORD *         IRIG time encode Time Tag Low
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_get_time( TY_API_DEV * p_api_dev, 
                           L_WORD     * day, 
                           L_WORD     * hour, 
                           L_WORD     * min, 
                           L_WORD     * sec,
                           L_WORD     * usec, 
                           L_WORD     * sth, 
                           L_WORD     * stl )
{
    AiReturn ret = 0;

    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        ret = mil_hw_pci_driver_aye_irig_get_time(p_api_dev, sth, stl);
        break;
    case AI_DEVICE_AYS:
        ret = mil_hw_pci_driver_ays_irig_get_time(p_api_dev, sth, stl);
        break;
    case AI_DEVICE_ARTIX7:
        ret = mil_hw_pci_driver_artix7_irig_get_time(p_api_dev, sth, stl);
        break;
    case AI_DEVICE_ARTIXUS:
        ret = mil_hw_pci_driver_artixus_irig_get_time(p_api_dev, sth, stl);
        break;
    default:
        ret = API_ERR_PLATTFORM_NOT_SUPPORTED;
    }

    *day  = (*sth >> 17) & 0x001FF;
    *hour = (*sth >> 12) & 0x0001F;
    *min  = (*sth >>  6) & 0x0003F;
    *sec  = (*sth)       & 0x0003F;
    *usec = (*stl)       & 0xfffff;

    /* Return compatible irig time tag for TSW internal use only */
    *stl = ((*min & 0x0000003F) << 26) + ((*sec & 0x0000003F) << 20) + *usec;
    *sth = (*day << 11) + ((*hour & 0x0000001F) << 6) + (*min & 0x0000003F);

    return ret;
}





/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW Discretes access                      *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Read Discrete interrupt entries from Loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  int_count          AiUInt32         Interrupt count

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_fifo_read(TY_API_DEV *p_api_dev, AiUInt32 int_count)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_discretes_fifo_read(p_api_dev, int_count);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}

/*! \brief Setup Discrete interrupt and enable loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ul_DiscreteSetup   AiUInt32         Setup Discrete interrupt and loglist enable

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_fifo(TY_API_DEV *p_api_dev, AiUInt32 new_config)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_discretes_fifo(p_api_dev, new_config);
    default:
        return 0;
    }
}

/*! \brief Initialize discrete input/output */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ul_DiscreteSetup   AiUInt32         The discrete setup

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_discretes_init(p_api_dev, new_config);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_discretes_init(p_api_dev, new_config);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_discretes_init(p_api_dev, new_config);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_discretes_init(p_api_dev, new_config);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Mask         AiUInt32         Output mask
    \param  Value        AiUInt32         Output value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value)
{
  switch (p_api_dev->ul_DeviceGroup)
  {
  case AI_DEVICE_AYE:
      return mil_hw_pci_driver_aye_discretes_write(p_api_dev, Mask, Value);
  case AI_DEVICE_AYS:
      return mil_hw_pci_driver_ays_discretes_write(p_api_dev, Mask, Value);
  case AI_DEVICE_ARTIX7:
      return mil_hw_pci_driver_artix7_discretes_write(p_api_dev, Mask, Value);
  case AI_DEVICE_ARTIXUS:
      return mil_hw_pci_driver_artixus_discretes_write(p_api_dev, Mask, Value);
  default:
      return API_ERR_PLATTFORM_NOT_SUPPORTED;
  }
}


/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_discretes_read(p_api_dev, Value);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_discretes_read(p_api_dev, Value);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_discretes_read(p_api_dev, Value);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_discretes_read(p_api_dev, Value);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}









/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW system information                    *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
 

/*! \brief converts a zero based stream to a zero based logical biu */
/*! 
    Converts a stream into logical biu.
	
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulStream                AiUInt32       Input variable for the stream.
    \param  ulHS                    AiUInt32       1=Return HS BIUT, 0=Return LS BIU
    \param  pwLogicalBiu            AiInt16*       Output variable for the logical biu.
    \param  pwMemBank               AiInt16*       Output variable for the memory bank (0=first,1=second).

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/

AiUInt32 mil_hw_convert_stream_to_LogicalBiu( TY_API_DEV * p_api_dev, AiUInt32 ulStream, AiUInt32 ulHs, AiInt16 * pwLogicalBiu, AiInt16 * pwMemBank )
{
    if( pwMemBank != NULL )
        *pwMemBank = 0;

    if( pwLogicalBiu != NULL )
        *pwLogicalBiu = (AiInt16)ulStream;

    return 0;
}


/*! \brief Get the start offset of the global Memory bank */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulGlobalRamStartOffset AiUInt32*      Output variable for the Offset of the start of the Global Ram for this stream.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_GlobalRamStartOffset( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 *pulGlobalRamStartOffset )
{
  if( pulGlobalRamStartOffset == NULL )
    return 1;

  *pulGlobalRamStartOffset = 0x00000000;

  return 0;
}


/*! \brief Get some information whether the amplitude can be changed for this stream */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulCanChangeAmplitude   AiUInt32*      Output variable showing if the amplitude can be modified.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude(TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulCanChangeAmplitude, AiUInt32 * pulPacked)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
    case AI_DEVICE_AYS:
        *pulCanChangeAmplitude =  mil_novram_interpret_subtype_amplitude_variable(p_api_dev->BoardSubType);
        *pulPacked             =  mil_novram_interpret_subtype_amplitude_packed(p_api_dev->BoardSubType);
        return 0;
    case AI_DEVICE_ARTIX7:
    case AI_DEVICE_ARTIXUS:
        *pulCanChangeAmplitude = MIL_NOVRAM_FRONTEND_CAPABILITIES_VA(p_api_dev->BoardSubType);
        *pulPacked             = MIL_NOVRAM_FRONTEND_CAPABILITIES_PA(p_api_dev->BoardSubType);
        return 0;
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}


/*! \brief Get some information whether the amplitude can be changed in high resolution mode for this stream */
/*!
\param  p_api_dev               TY_API_DEV*    The global variable container.
\param  ulBiu                   AiUInt32       Input variable for the logical BIU.
\param  pulCanChangeAmplitudeHighRes   AiUInt32*      Output variable showing if the amplitude can be modified in high resolution.

\return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude_high_res(TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulCanChangeAmplitudeHighRes)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
    case AI_DEVICE_AYS:
        *pulCanChangeAmplitudeHighRes = mil_novram_interpret_subtype_amplitude_high_res(p_api_dev->BoardSubType);
        return 0;
    case AI_DEVICE_ARTIX7:
    case AI_DEVICE_ARTIXUS:
        *pulCanChangeAmplitudeHighRes = MIL_NOVRAM_FRONTEND_CAPABILITIES_HR(p_api_dev->BoardSubType);
        return 0;
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}

/*! \brief Initialize irig capabilities within p_api_dev */
/*! 
    \param  p_api_dev             TY_API_DEV*            The global variable container.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_irig_capabilities( TY_API_DEV * p_api_dev )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_ARTIX7:
        mil_hw_pci_driver_artix7_irig_set_capabilities(p_api_dev);
        break;
    case AI_DEVICE_ARTIXUS:
        mil_hw_pci_driver_artixus_irig_set_capabilities(p_api_dev);
        break;
    default:
        /* Set default values */
        p_api_dev->x_IrigCapabilities.b.ul_Res = 0;
        p_api_dev->x_IrigCapabilities.b.b_InstantIrigSetOnInternalMode = AiFalse;
        p_api_dev->x_IrigCapabilities.b.b_NoExternalSignalOnInternalMode = AiFalse;
        p_api_dev->x_IrigCapabilities.b.b_IrigSwitch   = AiTrue;
        p_api_dev->x_IrigCapabilities.b.b_FreeWheeling = AiTrue;
        p_api_dev->x_IrigCapabilities.b.b_Sinusoidal   = AiTrue;
        break;
    }

    return 0;
}


static AiUInt32 mil_hw_init_coupling_capabilities_board_capabilities( TY_API_DEV * p_api_dev )
{
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;

    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = MIL_NOVRAM_FRONTEND_CAPABILITIES_ISO(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = MIL_NOVRAM_FRONTEND_CAPABILITIES_DIR(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Transformer = MIL_NOVRAM_FRONTEND_CAPABILITIES_XFM(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Network     = MIL_NOVRAM_FRONTEND_CAPABILITIES_NET(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = MIL_NOVRAM_FRONTEND_CAPABILITIES_PC( p_api_dev->BoardSubType);

    return 0;
}

static AiUInt32 mil_hw_init_coupling_capabilities_board_subtype( TY_API_DEV * p_api_dev )
{
  /* Check Bit 5-4: "Coupling Network" for AyE boards
   * For older boards this bit is always 0. This is OK, as they don't support direct or network coupling. */

  if( (p_api_dev->BoardSubType & 0x30) == 0x20 )
  {
    /* AyE device with onboard coupling network and combined coupling of channel A and B */
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Transformer = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiTrue;
  }
  else if( (p_api_dev->BoardSubType & 0x30) == 0x10 )
  {
    /* AyE device with onboard coupling network and independent coupling of channel A and B */
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Transformer = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiFalse;
  }
  else /* if( (p_api_dev->BoardSubType & 0x30) == 0x00 ) */
  {
    /* AMC or AyE device without onboard coupling network */
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Transformer = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiFalse;
  }

  return 0;
}

/*! \brief Initialize the coupling capabilities struct within p_api_dev to the initial values */
/*! 
    \param  p_api_dev               TY_API_DEV*               The global variable container.
    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_coupling_capabilities( TY_API_DEV * p_api_dev )
{
  /* Check Bit 5-4: "Coupling Network" for AyE boards
   * For older boards this bit is always 0. This is OK, as they don't support direct or network coupling. */

    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
    case AI_DEVICE_AYS:
        return mil_hw_init_coupling_capabilities_board_subtype(p_api_dev);
    case AI_DEVICE_ARTIX7:
    case AI_DEVICE_ARTIXUS:
        return mil_hw_init_coupling_capabilities_board_capabilities(p_api_dev);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}



/*! \brief Get some information about the discrete channels */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  pulDiscreteCount        AiUInt32*      Output variable for the ammount of discrete channels.
    \param  pulDiscreteConfig       AiUInt32*      Output variable for the configuration of the discrete channel. 
                                                   The bits of this field are one if this discrete can send data or zero if not.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_discretes( TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig )
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_get_discretes(p_api_dev, pulDiscreteCount, pulDiscreteConfig);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_get_discretes(p_api_dev, pulDiscreteCount, pulDiscreteConfig);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_get_discretes(p_api_dev, pulDiscreteCount, pulDiscreteConfig);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_get_discretes(p_api_dev, pulDiscreteCount, pulDiscreteConfig);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}


/*! \brief Get some information about the discrete channels */
/*!
\param  p_api_dev               TY_API_DEV*    The global variable container.
\param  pul_Channel             AiUInt32*      Number of discrete channels
\param  pul_Canin               AiUInt32*      bit field that tells, which channels can be set to In 
\param  pul_Canout              AiUInt32*      bit field that tells, which channels can be set to Out
\return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiReturn  mil_hw_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_discretes_info_read(p_api_dev, pul_Channel, pul_Canin, pul_Canout);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_discretes_info_read(p_api_dev, pul_Channel, pul_Canin, pul_Canout);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_discretes_info_read(p_api_dev, pul_Channel, pul_Canin, pul_Canout);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_discretes_info_read(p_api_dev, pul_Channel, pul_Canin, pul_Canout);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}


/*! \brief Get some information about the interrupt capabilities */
/*! 
    \param  p_api_dev               TY_API_DEV*          The global variable container.
    \param  ulBiu                   AiUInt32             Input variable for the logical BIU.
    \param  pulIRCapabilities       TY_IR_CAPABILITIES*  Output variable for the interrupt capabilities.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_ir_capabilities( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, TY_IR_CAPABILITIES * pxIRCapabilities )
{
  (*pxIRCapabilities).ul_All = 0;
  return 0;
}


/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure

\return returns The type of the FW 
*/
enum MIL_FW mil_hw_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_get_fw_type(p_api_dev, ulBiu);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_get_fw_type(p_api_dev, ulBiu);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_get_fw_type(p_api_dev, ulBiu);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_get_fw_type(p_api_dev, ulBiu);
    default:
        return MIL_FW_UNKNOWN;
    }
}




/*! \brief Checks whether the hs biu is redundant or not */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulIsHsRedundant        AiUInt32*      Output variable for the result (zero or one).

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_is_hs_redundant( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulIsHsRedundant )
{
  *pulIsHsRedundant = 0;
  return 0;
}


/*! \brief Check if high res zero crossing error injectio is supported.

    Check if this HW plattform supports the High Resolution Zero
    Crossing Deviatio error injection.

    \param p_api_dev pointer to target administration structure
    \return returns 1 if High Resolution Zero Crossing is supported 
*/
AiUInt32 mil_hw_is_high_res_zero_crossing_supported( TY_API_DEV* p_api_dev )
{
    return 1;
}

/*! \brief Return the default size of the monitor buffer in the global RAM */
AiUInt32 mil_hw_get_bm_size( TY_API_DEV* p_api_dev, AiUInt32 ucHs )
{
    if(    (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIXUS))
        {
        /* HS not available for AyE, AyS, ArtixUS platforms */
        return 0x00200000L; /* 2 MB */
    }
    else
    {
        /* HS not available for AMC, Artix7 platform */
        return 0x00080000L; /* 0.5 MB */
    }
}



/*****************************************************************************
*                                                                           *
*                                                                           *
*                 HW Layer public TSW version information                   *
*                                                                           *
*                                                                           *
*****************************************************************************/

/*! \brief Read versions for ApiReadBSPVersionEx

    \param p_api_dev pointer to target administration structure
    \param AiUInt32* ulLsIoLcaPbi1   LS IO LCA (first PBI)
    \param AiUInt32* ulPciLca        PCI LCA
    \param AiUInt32* ulFwBiu1Biu2    FW BIU1/BIU2
    \param AiUInt32* ulTSW           TSW version
    \param AiUInt32* ulTSW build     TSW build
    \param AiUInt32* ulSN            Board Serial Number
    \param AiUInt32* ulHsIoLcaPbi1   HS IO LCA (first PBI)
    \param AiUInt32* ulAslLca        ASL LCA
    \param AiUInt32* ulLsIoLcaPbi2   LS IO LCA (second PBI)
    \param AiUInt32* ulHsIoLcaPbi2   HS IO LCA (second PBI)
    \param AiUInt32* ulFwBiu3Biu4    FW BIU3/BIU4
    \param AiUInt32* ulMonitor       Monitor

    \return returns 1 if High Resolution Zero Crossing is supported
*/
AiUInt32 mil_hw_get_bsp_version_ex( TY_API_DEV* p_api_dev,
                             AiUInt32* ulLsIoLcaPbi1,
                             AiUInt32* ulPciLca,
                             AiUInt32* ulFwBiu1Biu2,
                             AiUInt32* ulTSW,
                             AiUInt32* ulTSWBuild,
                             AiUInt32* ulSN,
                             AiUInt32* ulHsIoLcaPbi1,
                             AiUInt32* ulAslLca,
                             AiUInt32* ulLsIoLcaPbi2,
                             AiUInt32* ulHsIoLcaPbi2,
                             AiUInt32* ulFwBiu3Biu4,
                             AiUInt32* ulMonitor )
{
    /*
    Note: This function does not return all of the AYE versions. For 
          AyE the versions are read directly by the DLL from the uMon 
          version table.
    */


    volatile L_WORD *pul_GlobMem  = (L_WORD*)p_api_dev->GlobalRAMBase;

    /* Initialize versions which are not available for all boards */

    *ulPciLca      = 0;
    *ulHsIoLcaPbi1 = 0;
    *ulAslLca      = 0;
    *ulLsIoLcaPbi2 = 0;
    *ulHsIoLcaPbi2 = 0;
    *ulFwBiu3Biu4  = 0;
    *ulMonitor     = 0;

    /* Target software version */
    *ulTSW      = MIL_TSW_LEGACY_VERSION;
    *ulTSWBuild = MIL_TSW_LEGACY_BUILD;

    /* Board serial number */
    *ulSN = p_api_dev->BoardSerial;

    /* FW BIU1 */
    *ulFwBiu1Biu2   = BSWAP32(pul_GlobMem[ 0x8  / 4 ]) & 0x0000ffff; /* FW BIU1/BIU2 */

    return 0;
}



/*! \brief Read a specific FW version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_fw(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }
}

/*! \brief Read a specific TCP version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_tcp(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }
}

/*! \brief Read a specific MLCA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_mlca(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_version_mlca(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_version_mlca(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_version_mlca(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_version_mlca(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }
}


/*! \brief Read a specific FPGA version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_fpga(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_version_fpga(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_version_fpga(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }
}


/*! \brief Read a specific Monitor version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_monitor(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_version_monitor(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_version_monitor(p_api_dev, eId, pxVersionValue, pxVersionString);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_version_monitor(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }

}


/*! \brief Read a specific OS version

\param p_api_dev        Global variable container
\param eId              The version to read
\param pxVersionValue   The numeric version representation
\param pxVersionString  The string version representation

\return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
static L_WORD mil_hw_pci_driver_version_os(TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_version_os(p_api_dev, eId, pxVersionValue, pxVersionString);
    default:
        return API_ERR_INVALID_MODE;
    }
}


/*! \brief Read a specific versions for ApiReadVersions

    \param p_api_dev        Global variable container
    \param eId              The version to read
    \param pxVersionValue   The numeric version representation
    \param pxVersionString  The string version representation

    \return returns         0 on success or API_ERR_INVALID_MODE if version not found.
*/
L_WORD mil_hw_read_version(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD retval = 0;

    switch( eId )
    {
    /* Hardware components */
    case AI_TCP_VER:
        retval = mil_hw_pci_driver_version_tcp(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;

    case AI_MAIN_LCA_VER:
        retval = mil_hw_pci_driver_version_mlca(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;

    case AI_FPGA_VER:
        retval = mil_hw_pci_driver_version_fpga(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;

    /* Firmware */
    case AI_FIRMWARE_VER_BIU1:
    case AI_FIRMWARE_VER_BIU2:
    case AI_FIRMWARE_VER_BIU3:
    case AI_FIRMWARE_VER_BIU4:
        retval = mil_hw_pci_driver_version_fw(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;

    /* TSW */
    case AI_TARGET_VER:
        retval = api_sys_read_version_tsw( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    /* TSW environment */
    case AI_MONITOR_VER:
        retval = mil_hw_pci_driver_version_monitor(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;


    /* OS environment */
    case AI_TARGET_OS_VER:
        retval =  mil_hw_pci_driver_version_os(p_api_dev, eId, pxVersionValue, pxVersionString);
        break;

    default:
        retval = API_ERR_INVALID_MODE;
    }

    return retval;
}

/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public PCI Bus Master access                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Read a 32 bit long word from a physical PCI address */
L_WORD mil_hw_pci_readlw( L_WORD ulPciAddress )
{
    return 0;
}

/*! \brief Write a 32 bit long word to a physical PCI address */
void mil_hw_pci_writelw( L_WORD ulPciAddress, L_WORD ulValue )
{
}

/*****************************************************************************
*                                                                           *
*                                                                           *
*                      HW Layer public TSW Trigger                          *
*                                                                           *
*                                                                           *
*****************************************************************************/

/*! \brief Check if trigger digital loop is available in this FPGA */
/*!
This function returns true if digital trigger loop is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_digital_loop(TY_API_DEV* p_api_dev)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_has_digital_loop(p_api_dev);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_has_digital_loop(p_api_dev);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_has_digital_loop(p_api_dev);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_has_digital_loop(p_api_dev);
    default:
        return AiFalse;
    }
}

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_trigger_digital_loop_con(p_api_dev, con);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_trigger_digital_loop_con(p_api_dev, con);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_trigger_digital_loop_con(p_api_dev, con);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_trigger_digital_loop_con(p_api_dev, con);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYE:
        return mil_hw_pci_driver_aye_trigger_digital_loop_get(p_api_dev, con);
    case AI_DEVICE_AYS:
        return mil_hw_pci_driver_ays_trigger_digital_loop_get(p_api_dev, con);
    case AI_DEVICE_ARTIX7:
        return mil_hw_pci_driver_artix7_trigger_digital_loop_get(p_api_dev, con);
    case AI_DEVICE_ARTIXUS:
        return mil_hw_pci_driver_artixus_trigger_digital_loop_get(p_api_dev, con);
    default:
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }
}



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                          HW Layer USB Specific                            *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Get the currect power class for the device
           This function is only applicable on devices with power delivery.

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \return returns The power class 
*/
AiUInt32 mil_hw_usb_aim_power_class(TY_API_DEV* p_api_dev)
{
    /* For non USB devices simply report full power mode (No restrictions) */
    return 0;
}

#endif /* defined(_AIM_SYSDRV) */
