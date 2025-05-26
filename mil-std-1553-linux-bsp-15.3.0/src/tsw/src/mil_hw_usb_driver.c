/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_usb_driver.c
    This file contains hw layer implementation for USB without ASP devices.

    mil_hw.h 
    mil_hw_usb_driver.c => APU1553-1/2
*/
/*********************************************************************************/


#if defined(_AIM_1553_SYSDRV_USB)
/* USB system driver only */

/* TSW */
#include "mil_tsw_includes.h"


/* Old PMC Monitor defines */
#define TC_ISTAT           0xb           /* IRIG status register      */
#define TC_ICTRL           0xc           /* IRIG control register      */
#define TC_IEDAYHI         0xd           /* IRIG time encode day high  */
#define TC_IEDAYLO         0xe           /* IRIG time encode day low   */
#define TC_IEHOUR          0xf           /* IRIG time encode hour      */
#define TC_IEMIN           0x10          /* IRIG time encode minute    */
#define TC_IESEC           0x11          /* IRIG time encode seconds   */
#define TC_I_ELATCH_B      (1 << 0)      /* Latch IRIG Encoder Data */
#define PMC_TTHIGH_Reg_Adr 0x50
#define PMC_TTLOW_Reg_Adr  0x51
#define PMC_IRIGSTATUS_Adr 0x54

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
  return 1;
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

    p_api_dev->pxMyMonContext  = NULL;
    p_api_dev->p_DeviceContext = px_tsw_config->pDeviceContext;
    p_api_dev->p_Device        = px_tsw_config->pDevice;

    p_api_dev->SharedRAMBase   = px_tsw_config->SharedRAMBase;
    p_api_dev->shared          = px_tsw_config->SharedRAMBase;

    p_api_dev->GlobalRAMBase   = px_tsw_config->GlobalRAMBase;
    p_api_dev->GlobalRAMSize   = px_tsw_config->GlobalRAMSize;

    p_api_dev->GlobalRAMBase2  = px_tsw_config->GlobalRAMBase2;
    p_api_dev->GlobalRAMSize2  = px_tsw_config->GlobalRAMSize2;

    p_api_dev->IoRAMBase       = NULL;
    p_api_dev->HostIoRAMBase   = NULL;

    p_api_dev->BoardApplicationType  = px_tsw_config->BoardAppType;
    p_api_dev->BoardApplicationType2 = 0;
    p_api_dev->ul_DeviceGroup        = px_tsw_config->PlatformGroup;

    p_api_dev->BoardSerial     = px_tsw_config->BoardSerial;
    p_api_dev->BoardConfig     = px_tsw_config->BoardConfig;
    p_api_dev->BoardType       = px_tsw_config->BoardType;
    p_api_dev->BoardSubType    = px_tsw_config->BoardSubType;
    p_api_dev->BoardSubType2   = 0;
    p_api_dev->BoardPartNo     = px_tsw_config->BoardPartNo;
    p_api_dev->CPUClk          = px_tsw_config->CPUClk;
    p_api_dev->SYSClk          = px_tsw_config->SYSClk;

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
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  px_tsw_config  TY_API_HW_INIT_PARAMS*  The initial values required to start the TSW
    
    \return                BYTE                    Zero on normal termination
*/
BYTE mil_hw_initialize_tsw( TY_API_DEV *  p_api_dev, TY_API_HW_INIT_PARAMS * px_tsw_config )
{
    mil_hw_initialize_memory( p_api_dev );
    mil_hw_initialize_api_dev( p_api_dev, px_tsw_config );

    CreateTransferLists(p_api_dev);
    CreateRTBufferLists(p_api_dev);

    api_main_initialize_stream_type(p_api_dev);
    mil_hw_init_irig_capabilities(     p_api_dev );
    mil_hw_init_coupling_capabilities( p_api_dev );

    api_main_setup_memory_layout( p_api_dev );
    api_main_init_and_reset(      p_api_dev );

    return 0;
}


/*! \brief This function shuts the TSW down and frees the allocated memory */
/*!
    This function shuts the TSW down and frees the allocated memory 
    
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
    
    \return AiUInt32 The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_novram_write( TY_API_DEV * p_api_dev,
                            AiUInt32     ulOffset,
                            AiUInt32     ulValue )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


/*! \brief Read from the given NOVRAM offset. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ulOffset           AiUInt32         The NOVRAM offset
    \param  pulValue           AiUInt32*        The NOVRAM value
    
    \return AiUInt32 The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_novram_read( TY_API_DEV * p_api_dev,
                           AiUInt32     ulOffset,
                           AiUInt32*    pulValue )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

/*! \brief Start the NOVRAM write. */
/*!
    Reserved for future use.

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return AiUInt32 The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_novram_start( TY_API_DEV * p_api_dev )
{
    return API_OK;
}


/*! \brief Flush the NOVRAM mirror. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return AiUInt32 The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_novram_finalize( TY_API_DEV * p_api_dev )
{
    /* Is a flush command required for USB ? */
    return API_OK;
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


/*! \brief Configure the trigger input edge for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32                The trigger edges

\return                AiReturn                Zero on success
*/
AiReturn  mil_hw_trigger_edge_input_set(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 edge_flags)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


/*! \brief Get the trigger input edge configuration for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32*               The trigger edges output

\return                AiReturn                Zero on success
*/
AiReturn  mil_hw_trigger_edge_input_get(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 * edge_flags)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}



 
/*! \brief This function sets the hw irig time through the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    unsigned int ui_IrigSwitchRegister;

    mil_hw_ioreg_read(p_api_dev, 0, &ui_IrigSwitchRegister );

    if( con & 0x01 )
        ui_IrigSwitchRegister &= BSWAP32(0xFFFFFFDFL);
    else
        ui_IrigSwitchRegister |= BSWAP32(0x00000020L);

    mil_hw_ioreg_write(p_api_dev, 0, ui_IrigSwitchRegister);

    return 0; 
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
                        L_WORD         us)
{
    L_WORD   ret_val  = 0;

    /* -- set irig time if commanded --- */

    if( (day > 0) && (day <= 366L) && (hour < 24) && (min < 60) && (sec < 60) )
    {
        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_IEDAYHI, (day / 256) ); /* IRIG time encode day high */
        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_IEDAYLO, (day % 256) ); /* IRIG time encode day low */
        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_IEHOUR, (L_WORD) (hour) ); /* IRIG time encode hour */
        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_IEMIN, (L_WORD) (min) ); /* IRIG time encode minute */
        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_IESEC, (L_WORD) (sec) ); /* IRIG time encode seconds */

        usb_tcp_port_write(p_api_dev->p_DeviceContext, TC_ICTRL, TC_I_ELATCH_B);
    }
    else
    {
        ret_val = API_ERR_INVALID_TIME;
    }

    return ret_val;

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
    L_WORD irig_status = 0;

    *irig_sync = API_IRIG_SYNC;
    *irig_src  = API_IRIG_ONBOARD;

    if(p_api_dev->sys_mem[0].pci_lca_id < 0x74)
    {
        /* With PCI-LCA versions below 7R4, IRIG status has to be read via TCP */
        usb_tcp_port_read(p_api_dev->p_DeviceContext, TC_ISTAT, &irig_status);

        mil_hw_ioreg_read(p_api_dev, 0, irig_src);
        *irig_src =  *irig_src & 0x00000020;
    }
    else
    {
        mil_hw_ioreg_read(p_api_dev, PMC_IRIGSTATUS_Adr, &irig_status);

        *irig_src = (irig_status >> 15) & 0x1;
    }

    if(*irig_src) /* intern IRIG is set */
        *irig_src  = API_IRIG_ONBOARD;
    else
    {
        *irig_src = API_IRIG_EXTERN;
        if( 0 == (irig_status & 0x01) )
            *irig_sync = API_IRIG_NOT_SYNC;
    }
    return 0;
}


typedef union
{
    L_WORD  ul_All;
    struct
    {
        L_WORD  sec         : 6   ;     /* seconds of minute*/
        L_WORD  minutes     : 6   ;     /* minutes of hour*/
        L_WORD  hour        : 5   ;     /* hour of day*/
        L_WORD  day         : 9   ;     /* day of year*/
        L_WORD              : 6   ;     /* reserved*/
        L_WORD              : 0   ;     /* alignment     */
    }Reg;
} PMC_TTHIGH_Reg;

typedef union
{
    L_WORD  ul_All;
    struct
    {
        L_WORD  usec       : 20  ;     /* us of seconds*/
        L_WORD  sec        : 6   ;     /* seconds of minute*/
        L_WORD  minutes    : 6   ;     /* minutes of hour*/
        L_WORD             : 0   ;     /* alignment     */
    }Reg;
} PMC_TTLOW_Reg;


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
    PMC_TTHIGH_Reg tt_high;
    PMC_TTLOW_Reg tt_low;

    mil_hw_ioreg_read(p_api_dev, PMC_TTHIGH_Reg_Adr, &tt_high.ul_All);
    mil_hw_ioreg_read(p_api_dev, PMC_TTLOW_Reg_Adr, &tt_low.ul_All);

    *day     = tt_high.Reg.day;
    *hour    = tt_high.Reg.hour;
    *min     = tt_high.Reg.minutes;
    *sec     = tt_high.Reg.sec;
    *usec    = tt_low.Reg.usec;

    *stl = ((*min & 0x0000003F) << 26) + ((*sec & 0x0000003F) << 20) + *usec;
    *sth = (*day << 11) + ((*hour & 0x0000001F) << 6) + (*min & 0x0000003F);

    return 0;
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
	return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

/*! \brief Setup Discrete interrupt and enable loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ul_DiscreteSetup   AiUInt32         Setup Discrete interrupt and loglist enable

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_fifo(TY_API_DEV *p_api_dev, AiUInt32 new_config)
{
	return API_ERR_PLATTFORM_NOT_SUPPORTED;
}




/*! \brief Initialize discrete input/output */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ul_DiscreteSetup   AiUInt32         The discrete setup

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 ul_DiscreteSetup )
{
  L_WORD ul_Temp;

  mil_hw_ioreg_read(p_api_dev, 0x380 / 4, &ul_Temp);

  ul_Temp &= 0xff00ffff;
  ul_Temp |= ( (ul_DiscreteSetup << 16) &0x00ff0000);

  mil_hw_ioreg_write(p_api_dev, 0x380 / 4, ul_Temp);

  return 0;
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
  L_WORD ul_Tmp;

  Mask  &= 0x000000ff;  /* only the lowest 8 bit are valid */
  Value &= 0x000000ff;  /* only the lowest 8 bit are valid */

  mil_hw_ioreg_read(p_api_dev, 0x380 / 4, &ul_Tmp);

  ul_Tmp &= ~Mask; /* reset all bits referenced in the mask */
  ul_Tmp |= (Value &Mask); /* setup all bits referenced in value together with mask */

  mil_hw_ioreg_write(p_api_dev, 0x380 / 4, ul_Tmp);

  return 0;
}


/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value)
{
    mil_hw_ioreg_read(p_api_dev, 0x380 / 4, Value);

    *Value &= 0x000000ff;

    return 0;
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

/*! \brief Get number of bius of the board */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  pulNumberOfBiu          AiUInt32*      Output variable for the number of BIUs.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_number_of_biu( TY_API_DEV * p_api_dev, AiUInt32 * pulNumberOfBiu )
{
  switch(p_api_dev->uc_DeviceType)
  {
  case TYPE_APU1553_1:
  case TYPE_APU1553_2:
  default:
    *pulNumberOfBiu                           = 1;
    break;
  }
  return 0;
}

/*! \brief Get some information whether the amplitude can be changed for this stream */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulCanChangeAmplitude   AiUInt32*      Output variable showing if the amplitude can be modified.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulCanChangeAmplitude, AiUInt32 * pulPacked )
{
    *pulCanChangeAmplitude = AiFalse;
    *pulPacked             = AiFalse; /*Don't care*/

    return 0;
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
    *pulCanChangeAmplitudeHighRes = AiFalse;

    return 0;
}


/*! \brief Initialize irig capabilities within p_api_dev */
/*! 
    \param  p_api_dev             TY_API_DEV*            The global variable container.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_irig_capabilities( TY_API_DEV * p_api_dev )
{
    /* Set default values */
    p_api_dev->x_IrigCapabilities.b.ul_Res = 0;
    p_api_dev->x_IrigCapabilities.b.b_InstantIrigSetOnInternalMode   = AiFalse;
    p_api_dev->x_IrigCapabilities.b.b_NoExternalSignalOnInternalMode = AiFalse;
    p_api_dev->x_IrigCapabilities.b.b_IrigSwitch      = AiTrue;
    p_api_dev->x_IrigCapabilities.b.b_FreeWheeling    = AiTrue;
    p_api_dev->x_IrigCapabilities.b.b_Sinusoidal      = AiTrue;

    return 0;
}


/*! \brief Initialize the coupling capabilities struct within p_api_dev to the initial values */
/*! 
    \param  p_api_dev               TY_API_DEV*               The global variable container.
    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_coupling_capabilities( TY_API_DEV * p_api_dev )
{
  /* Set default values */
  p_api_dev->x_CouplingCapabilities.b.b_Transformer = AiTrue;


  /* Check Bit 5-4: "Coupling Network" for AyE boards
   * For older boards this bit is always 0. This is OK, as they don't support direct or network coupling. */
  if( (p_api_dev->BoardSubType & 0x30) == 0x10 )
  {
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiTrue;
  }
  else
  {
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiFalse;
  }



  /* Check board sub type for packed coupling check */
  if( (p_api_dev->BoardSubType & 0xF0) == 0x20 )
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiTrue;
  else
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiFalse;

  return 0;
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
  AiUInt32 i;

  *pulDiscreteCount = 8; /* default value */

  /* all discrete channels are read/write */
  *pulDiscreteConfig = 0;
  for (i=0; i < *pulDiscreteCount; i++)
    *pulDiscreteConfig += 1<<i;

  return 0;
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
    AiUInt32 i;


    *pul_Channel = 8; /* default value */
    *pul_Canin   = 0xFF; /* default value */
    *pul_Canout  = 0xFF; /* default value */

    return 0;
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
\return returns The type of the FW */
enum MIL_FW mil_hw_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    int multi_channel;

    multi_channel = ul_StreamIsMultiChannel(p_api_dev, ulBiu);

    return multi_channel ? MIL_FW_MCH_XSCALE : MIL_FW_STD_XSCALE;
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
    /* Supported on APU */
    return 1;
}

/*! \brief Return the default size of the monitor buffer in the global RAM */
AiUInt32 mil_hw_get_bm_size( TY_API_DEV* p_api_dev, AiUInt32 ucHs )
{
    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE )
    {
        /* HS not available vor AYE platform */
        return 0x00200000L; /* 2 MB */
    }
    else
    {
        /* HS not available vor AMC platform */
        return 0x00080000L; /* 0.5 MB */
    }
}



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
    volatile L_WORD *pul_GlobMem  = (L_WORD*)p_api_dev->GlobalRAMBase;

    /* Read Targetsoftware version */
    *ulTSW      = MIL_TSW_LEGACY_VERSION;
    *ulTSWBuild = MIL_TSW_LEGACY_BUILD;

    /* Read Board Serialnumber */
    *ulSN = p_api_dev->BoardSerial;

    /* Read monitor version */
    *ulMonitor = 0;

    /* read IO-LCA version                               */
    *ulLsIoLcaPbi1 = BSWAP32(pul_GlobMem[ 0x24 / 4 ]) & 0x0FFF0FFF;

    /* read PCI-LCA version                              */
    mil_hw_ioreg_read(p_api_dev, 0x80, ulPciLca);

    /* read firmware version                             */
    *ulFwBiu1Biu2 = (BSWAP32(pul_GlobMem[ 0x8 / 4 ]) &0x0000ffff);

    /* Clear unavailable versions */
    *ulHsIoLcaPbi1 = 0;
    *ulAslLca      = 0;
    *ulLsIoLcaPbi2 = 0;
    *ulHsIoLcaPbi2 = 0;
    *ulFwBiu3Biu4  = 0;

    return 0;
}



L_WORD mil_hw_read_version_lca(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD  version = 0;

    mil_hw_ioreg_read(p_api_dev, 0x80, &version);

    pxVersionValue->ul_VersionType  = AI_PCI_LCA_VER;
    pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,4,0xFF  ) );
    pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,0,0xF   ) );
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr      = 0;

    AI_TSW_OS_SPRINTF(pxVersionString->ac_Description, AI_DESCRIPTION_STRINGL, "PCI LCA");
    AI_TSW_OS_SPRINTF(pxVersionString->ac_FullVersion, AI_VERSION_STRINGL, "%d.%d", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer);

    return 0;
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
    case AI_PCI_LCA_VER:
        retval = mil_hw_read_version_lca( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    /* IO interface */
    case AI_IO_LCA_VER_BIU1:
    case AI_IO_LCA_VER_BIU2:
    case AI_IO_LCA_VER_BIU3:
    case AI_IO_LCA_VER_BIU4:
        retval = api_sys_read_version_io( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    /* Firmware */
    case AI_FIRMWARE_VER_BIU1:
    case AI_FIRMWARE_VER_BIU2:
    case AI_FIRMWARE_VER_BIU3:
    case AI_FIRMWARE_VER_BIU4:
        retval = api_sys_read_version_fw( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    /* TSW */
    case AI_TARGET_VER:
        retval = api_sys_read_version_tsw( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    /* TSW environment */
    case AI_MONITOR_VER:
    case AI_TCP_VER:
    case AI_FPGA_VER:
    case AI_MAIN_LCA_VER:
    case AI_BOOTSTRAP_VER:
    case AI_TARGET_OS_VER:
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




L_WORD mil_hw_ioreg_read(TY_API_DEV *p_api_dev, L_WORD IO_Reg, L_WORD *pulValue)
{
    if (pulValue)
    {
        return usb_io_read(p_api_dev->p_DeviceContext, IO_Reg * 4, pulValue, sizeof(L_WORD));
    }
    else
    {
        return API_ERR_PARAM3_IS_NULL;
    }
}


L_WORD mil_hw_ioreg_write(TY_API_DEV *p_api_dev, L_WORD IO_Reg, L_WORD ulValue)
{
    return usb_io_write(p_api_dev->p_DeviceContext, IO_Reg * 4, &ulValue, sizeof(L_WORD));
}



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                              PXI Access                                   *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/*! \brief Check if PXI is supported on this device  */
/*!
    This function returns true if PXI is supported and false in any other case.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiBoolean               True on PXI support
*/
AiBoolean mil_hw_pxi_is_supported(        TY_API_DEV* p_api_dev )
{
    return AiFalse;
}



/*! \brief Disable all PXI input triggers  */
/*!
    This function disables all input triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_disable(  TY_API_DEV* p_api_dev )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

/*! \brief Disable all PXI output triggers  */
/*!
    This function disables all output triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_disable( TY_API_DEV* p_api_dev )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}



/*! \brief Control the PXI irig source */
/*!
    This function enables, disables or clears the PXI irig signal.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TTAGSrc        AiUInt32                Enable PXI irig if value is 1
    \param  TTAGClrEn       AiUInt32               Clear PXI Irig if value is 1
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_timetag_src_control( TY_API_DEV* p_api_dev, AiUInt32 TTAGSrc, AiUInt32 TTClrEn )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}



/*! \brief Connect a PBI trigger line with PXI output trigger line */
/*!
   
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgInLine      AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger output line
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_select(   TY_API_DEV* p_api_dev, AiUInt8 TrgInLine, AiUInt32 PXITrgLine )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}



/*! \brief Connect a PXI trigger line with PBI input trigger line */
/*!
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgOutLine     AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger input line
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_select(  TY_API_DEV* p_api_dev, AiUInt8 TrgOutLine, AiUInt8 PXITrgLine )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

/*! \brief Get PXI Geographic Address  */
/*!
This function returns the PXI Geographic Address.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  geo_addr       AiUInt32*               The PXI Geographic Address

\return                AiReturn                Zero on success
*/
AiReturn mil_hw_pxi_geo_addr(TY_API_DEV* p_api_dev, AiUInt32 *pxiGeographicalAddress)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}





/*****************************************************************************
*                                                                           *
*                                                                           *
*                      HW Layer public TSW Trigger                          *
*                                                                           *
*                                                                           *
*****************************************************************************/

/*! \brief Check if external trigger is available on this device  */
/*!
This function returns true if external trigger signal is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_external_input(TY_API_DEV* p_api_dev)
{
    return AiTrue;
}

/*! \brief Check if trigger digital loop is available in this FPGA */
/*!
This function returns true if digital trigger loop is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_digital_loop(TY_API_DEV* p_api_dev)
{
    return AiFalse;
}

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}















/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                      HW Layer public TSW MILScope                         *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


AiUInt32 mil_hw_scope_setup(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_trigger(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_setup_coupling(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_start(TY_API_DEV* p_api_dev, AiUInt32 ulMode, AiUInt32 ulNoOfCycles )
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_status(TY_API_DEV* p_api_dev, TY_API_SCOPE_STATUS* p_Status)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_stop(TY_API_DEV* p_api_dev)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_trigger_discretes(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_STROBE* p_scope_trigger_strobe)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_trigger_strobe_mon(TY_API_DEV* p_api_dev, AiUInt32 destination_biu)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_trigger_strobe_bc(TY_API_DEV* p_api_dev, AiUInt32 destination_biu)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_trigger_strobe_discretes(TY_API_DEV* p_api_dev, AiUInt32 discrete_channel)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_reset(TY_API_DEV* p_api_dev)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_set_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_get_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_calibrate( TY_API_DEV * p_api_dev, AiUInt8 ucMode )
{
    /* For Boards without ASP the calibration is done within the DLL */
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_type_get( TY_API_DEV *p_api_dev )
{
    return API_MILSCOPE_TYPE_NONE;
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
