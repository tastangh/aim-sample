/* SPDX-FileCopyrightText: 2020-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_pci_ays_embedded.c
    This file contains hw layer implementation for ANET3910, AYXX3910, ASC1553, ANET-MxAy

    mil_hw.h 
    mil_hw_ays_embedded.c => ANET3910, AYXX3910, ASC1553, ANET-MxAy
*/

#if defined(_AIM_SYSDRV)
/* PCI system driver and VME driver library only */

/* MIL Driver */
#include "aim_device.h"
#include "aim_gpio.h"
#include "aim_irig.h"

/* Linux system includes */
#include <aim_ays_novram.h>
#include <aim_platform.h>
#include <aim_platform_versions.h>


#include <dt-bindings/aim/aim_platform_dtb.h>


/* TSW */
#include "mil_tsw_includes.h"
#include "Ai_def.h"
#include "hw/AiHwAys.h"
#include "hw/AiHwAnetMa.h"
#include "hw/AiHwZynqMP.h"
#include "mil_novram.h"


#ifdef CONFIG_ARCH_ZYNQMP
#define FW3910 "BIP_3330.elf"
#define FWEFEX "BIP_3370.elf"
#else
#define FW3910 AIM_PLATFORM_3910_FW_DESC
#define FWEFEX AIM_PLATFORM_EFEX_FW_DESC
#endif


/*! \def TARGET_CONTROL_OFFSET
 * Byte offset of target-communication area in shared RAM
 */
#define TARGET_CONTROL_OFFSET 0x1F00

static L_WORD NVR_BOARD_TYPE = 0;

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

    p_api_dev->ul_DeviceGroup       = px_tsw_config->PlatformGroup;

    p_api_dev->BoardSerial     = px_tsw_config->BoardSerial;
    p_api_dev->BoardConfig     = px_tsw_config->BoardConfig;
    p_api_dev->BoardPartNo     = px_tsw_config->BoardPartNo;
    p_api_dev->CPUClk          = px_tsw_config->CPUClk;
    p_api_dev->SYSClk          = px_tsw_config->SYSClk;
    p_api_dev->HwVariant       = px_tsw_config->HwVariant;

    p_api_dev->BoardType             = px_tsw_config->BoardType;
    p_api_dev->BoardSubType          = px_tsw_config->BoardSubType;
    p_api_dev->BoardSubType2         = px_tsw_config->BoardSubType2;
    p_api_dev->BoardApplicationType  = px_tsw_config->BoardAppType;
    p_api_dev->BoardApplicationType2 = px_tsw_config->BoardAppType2;

    /*** Init Target Communication pointers and area ***/
    p_api_dev->targ_com_reg_p  = NULL; /* Unused */
    p_api_dev->cmd_p           = NULL; /* Unused */
    p_api_dev->ack_p           = NULL; /* Unused */

    p_api_dev->clk_1us         = 1;


    return 0;
}




 static AiInt16 mil_hw_internal_tcp_command(  TY_API_DEV *  p_api_dev,
                                       AiUInt32 ulCommand,
                                       AiUInt32 ulInputCount,  AiUInt32 ulInput[],
                                       AiUInt32 ulOutputCount, AiUInt32 ulOutput[] )
 {
     AiUInt32  i             = 0;
     AiUInt32  ulValue       = 0;


     /* check if no other process is running */
     /* multiple processes are not supported */


     /* copy input data into registers */
     for( i=0; i<ulInputCount; i++ )
         mil_hw_io_register_write( p_api_dev, AYS_REG_TCP_INPUT_0 + i, ulInput[i] );

     /* execute command */

     ulCommand = ulCommand << 24;

     mil_hw_io_register_write( p_api_dev, AYS_REG_TCP_CONTROL, ulCommand );

     ai_tsw_os_wait_usec( 100 );

     /* wait until command is finished */

     for( i=0; i<10000; i++ )
     {
         ulValue = mil_hw_io_register_read( p_api_dev, AYS_REG_TCP_STATUS );

         if(    ( (ulValue & MYMON_STATUS_MASK) == MYMON_STATUS_FREE )
             && ( (ulValue & MYMON_STATUS_CMD)  == ulCommand         ) )
             /* finished */
             break;

         ai_tsw_os_wait_usec( 100 );
     }

     if( i == 10000 )
         /* timeout occured */
         return MYMON_ERR_TIMEOUT;

     /* copy return data */

     if( ulOutputCount > 0 )
         for( i=0; i<ulOutputCount; i++ )
             ulOutput[i] = mil_hw_io_register_read( p_api_dev, AYS_REG_TCP_OUTPUT_0 + i );

     /* -- execute command 0 to reset the status --- */
     mil_hw_io_register_write( p_api_dev, AYS_REG_TCP_CONTROL, 0 );


     return MYMON_OK;
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
    int       ret;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    BUG_ON( !aimDevice );

    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA )
        return 1;


    p_api_dev->repeat = API_OFF;

    mil_hw_signal_asp_stopped(p_api_dev);


    /* -- reboot new firmware --- */
    if( (NVR_BOARD_TYPE & 0xF0F0) == 0x3010 )
        ret = aim_platform_firmware_boot(aimDevice->platformDevice, FW3910, AIM_PLATFORM_FW_PU2 );
    else
        ret = aim_platform_firmware_boot(aimDevice->platformDevice, FWEFEX, AIM_PLATFORM_FW_PU2 );

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
    if(    (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_ZYNQMP_ASP))
        if( ul_StreamIsHS(p_api_dev,BIU_1) )
            return 1;

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
                            BYTE ucPermanent,
                            AiUInt32 ulBoardType )
{
    NVR_BOARD_TYPE = ulBoardType;

    /* Write the new BoardType into the NOVRAM mirror */
    mil_hw_novram_write( p_api_dev, AIM_AYS_NVR_BOARDTYPE, ulBoardType );

    if( ucPermanent )
        /* Write the novram mirror to the device */
    	mil_hw_novram_finalize( p_api_dev );

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









/*! \brief Get boot sectors for requested protocol */
/*! 
    This function will return the boot sectors required
    for this hardware and protocol.

    The input parameter ulProtocol might be:

    0x30 -> 3910
    0x70 -> EFEx

    \param  p_api_dev        TY_API_DEV*    The global variable container.
    \param  ulProtocol       AiUInt32       Input variable for the protocol type
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
    *pulLsBiuBootSec = 0; /* N/A */
    *pulHsBiuBootSec = 0; /* N/A */
    *pulLcaBootSec   = 0; /* N/A */
    *pulLsBiuFreq    = 0; /* N/A */
    *pulHsBiuFreq    = 0; /* N/A */
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
    int       retval;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    BUG_ON( !aimDevice );

    retval = aim_platform_nvr_set( aimDevice->platformDevice, ulOffset, ulValue );

    if( retval != 0 )
    {
        return API_ERR_NOVRAM_ERROR;
    }
    else
    {
        return 0;
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
    int       retval;
    aim_nvr_t value;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    BUG_ON( !aimDevice );

    retval = aim_platform_nvr_get( aimDevice->platformDevice, ulOffset, &value );

    if( retval != 0 )
    {
        *pulValue = 0;
        return API_ERR_NOVRAM_ERROR;
    }
    else
    {
        *pulValue = value;
        return 0;
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
    int       retval;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    BUG_ON( !aimDevice );

    retval = aim_platform_nvr_prog( aimDevice->platformDevice );

    if( retval != 0 )
    {
        return API_ERR_NOVRAM_ERROR;
    }
    else
    {
        return 0;
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
    return !MIL_NOVRAM_HW_VARIANT_IRIG_EXTERNALLY_ABSENT(p_api_dev->HwVariant);
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


#ifndef CONFIG_AIM_IRIG
AiReturn aim_irig_source_set(struct aim_irig * dev, enum AIM_IRIG_SRC source)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED; 
}
AiReturn aim_irig_time_set(struct aim_irig * dev, int day, int hour, int minute, int second)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED; 
}
#endif

 
/*! \brief This function sets the hw irig time source the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_set_external(TY_API_DEV *p_api_dev, BYTE con)
{
    L_WORD value;

    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA )
    {
        struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;
        if( con & 0x01 )
        {
            aim_irig_source_set(aimDevice->irig_dev,AIM_IRIG_SRC_EXTERNAL);  
        }
        else
        {
            aim_irig_source_set(aimDevice->irig_dev,AIM_IRIG_SRC_INTERNAL);  
        }
    }
    else
    {
        if( con & 0x01 )
        {
            value = mil_hw_io_register_read(p_api_dev,AYS_REG_TIME_SOURCE_CONTROL);
            value |= BSWAP32(0x00000100L); /* Bit 8=1 is irig external*/
            mil_hw_io_register_write(p_api_dev,AYS_REG_TIME_SOURCE_CONTROL,value);
        }
        else
        {
            value = mil_hw_io_register_read(p_api_dev,AYS_REG_TIME_SOURCE_CONTROL);
            value &= BSWAP32(0xFFFFFEFFL); /* Bit 8=0 is irig internal*/
            mil_hw_io_register_write(p_api_dev,AYS_REG_TIME_SOURCE_CONTROL,value);
        }
    }
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
                        L_WORD         us )
{
    L_WORD ret_val  = 0;
    AiUInt32 ulInput[3];

    if( sec  >  59 ) return API_ERR_VALUE_NOT_IN_RANGE;
    if( min  >  59 ) return API_ERR_VALUE_NOT_IN_RANGE;
    if( hour >  23 ) return API_ERR_VALUE_NOT_IN_RANGE;
    if( day  > 366 ) return API_ERR_VALUE_NOT_IN_RANGE;
    
    /* -- set irig time if commanded --- */
    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA )
    {  
        struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;
        /* aim_irig_time_set is a kernel function that returns negative error codes*/
        if( aim_irig_time_set(aimDevice->irig_dev, day, hour, min, sec) != 0 )
            ret_val = API_ERR_MYMON_ERROR;
    }
    else
    {
        ulInput[0] = 0;     /* time                */
        ulInput[1] = 0;     /* IEEE1344 (reserved) */
        ulInput[2] = 0;     /* update + leap Year  */

        ulInput[0] |= sec;
        ulInput[0] |= min   <<  6;
        ulInput[0] |= hour  << 12;
        ulInput[0] |= day   << 17;

        ulInput[2]  = 1ul<<31;

        /* mil_hw_internal_tcp_command is a MyMon function that returns MYMON error codes*/
        if( mil_hw_internal_tcp_command( p_api_dev, MYMON_CMD_IRIG_SET, 3, ulInput, 0, 0 ) != 0 )
            ret_val = API_ERR_MYMON_ERROR;
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
    AiUInt32 irig_status = 0;

    *irig_sync = API_IRIG_SYNC;
    *irig_src  = API_IRIG_ONBOARD;

    irig_status = mil_hw_io_register_read(p_api_dev,AYS_REG_TIME_SOURCE_CONTROL);

    *irig_src    = (irig_status >> 8) & 0x1;

    if(*irig_src == 0) /* intern IRIG is set */
        *irig_src  = API_IRIG_ONBOARD;
    else
    {
        irig_status = mil_hw_io_register_read(p_api_dev,AYS_REG_TT_STATUS);
        *irig_src = API_IRIG_EXTERN;
        if( 0 == (irig_status & (1<<27)) )
            *irig_sync = API_IRIG_NOT_SYNC;
    }

    return 0;
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
    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA )
    {
        *sth = mil_hw_io_register_read(p_api_dev, ANET_MA_REG_TT_HIGH(ANET_MA_REG_TT_SET_1553) );
        *stl  = mil_hw_io_register_read(p_api_dev, ANET_MA_REG_TT_LOW(ANET_MA_REG_TT_SET_1553) );
    }
    else
    {
        *sth = mil_hw_io_register_read(p_api_dev, AYS_REG_TT_HIGH );
        *stl  = mil_hw_io_register_read(p_api_dev, AYS_REG_TT_LOW  );
    }

    *day   = (*sth >> 17) & 0x001FF;
    *hour  = (*sth >> 12) & 0x0001F;
    *min   = (*sth >>  6) & 0x0003F;
    *sec   = (*sth      ) & 0x0003F;
    *usec  = (*stl      ) & 0xfffff;

    /* Return compatible irig time tag for TSW internal use only */
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
AiReturn mil_hw_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 new_config )
{
    int    ret = 0;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    ret = aim_gpio_config_set(aimDevice->gpio_dev, AIM_GPIO_SET_LOW, new_config );

    if( ret != 0 )
        ret = API_ERR_VALUE_NOT_IN_RANGE;

    return ret;
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
    int    ret = 0;
    int    masked_value = 0;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    masked_value = aim_gpio_value_get(aimDevice->gpio_dev, AIM_GPIO_SET_LOW );
    masked_value &= ( Value | ~Mask ); /* Set all related bits to low  */
    masked_value |= ( Value &  Mask ); /* Set all related bits to high */

    ret = aim_gpio_value_set(aimDevice->gpio_dev, AIM_GPIO_SET_LOW, masked_value );

    if( ret != 0 )
        ret = API_ERR_VALUE_NOT_IN_RANGE;

    return ret;
}

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * value)
{

    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;

    *value = aim_gpio_value_get(aimDevice->gpio_dev, AIM_GPIO_SET_LOW );

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
    short logic_biu = 0;

    logic_biu = ulStream;

    switch(p_api_dev->uc_DeviceType)
    {
        case TYPE_ANET3910:
        case TYPE_ANET3910Xp:
        case TYPE_APXX3910:
        case TYPE_APXX3910Xp:
        case TYPE_APEX3910:
        case TYPE_APEX3910Xp:
        case TYPE_ACE3910:
        case TYPE_ACE3910Xp:
        case TYPE_AXC3910:
        case TYPE_AXC3910Xp:
          if( ulStream == 0 )
          {
            if( ulHs == 0 )
              logic_biu = 0;
            else
              logic_biu = 1;
          }
          break;
        case TYPE_ASC1553_1:
        case TYPE_ASC1553_2:
        default:
            break;
    }

    if( pwLogicalBiu ) *pwLogicalBiu = logic_biu;
    if( pwMemBank    ) *pwMemBank    = 0;

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
    if (ul_StreamIsHS(p_api_dev, ulBiu) && ulBiu > 0)
    {
        /* HS BIU has no variable amplitude */
        *pulCanChangeAmplitude = AiFalse;
        *pulPacked             = AiFalse; /* Don't care */
        return 0;
    }

    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_ZYNQMP_ASP )
    {
        /* AYXX3910, ASC1553-2 */
        /* Check BIU0 */
        *pulCanChangeAmplitude = MIL_NOVRAM_FRONTEND_CAPABILITIES_VA(p_api_dev->BoardSubType);
        *pulPacked             = MIL_NOVRAM_FRONTEND_CAPABILITIES_PA(p_api_dev->BoardSubType);
        return 0;
    }
    else
    {
        /* ANET-MxAy, ANET3910, ASC1553-1 */
        /* Check BIU0 */
        *pulCanChangeAmplitude =  mil_novram_interpret_subtype_amplitude_variable(p_api_dev->BoardSubType);
        *pulPacked             =  mil_novram_interpret_subtype_amplitude_packed(p_api_dev->BoardSubType);
        return 0;
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
    if (ul_StreamIsHS(p_api_dev, ulBiu) && ulBiu > 0)
    {
        /* HS BIU has no variable amplitude */
        *pulCanChangeAmplitudeHighRes = AiFalse;
        return 0;
    }

    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_ZYNQMP_ASP )
    {
        /* AYXX3910, ASC1553-2 */
        /* Check BIU0 */
        *pulCanChangeAmplitudeHighRes = MIL_NOVRAM_FRONTEND_CAPABILITIES_HR(p_api_dev->BoardSubType);
        return 0;
    }
    else
    {
        /* ANET-MxAy, ANET3910, ASC1553-1 */
        /* Check BIU0 */
        *pulCanChangeAmplitudeHighRes = mil_novram_interpret_subtype_amplitude_high_res(p_api_dev->BoardSubType);
        return 0;
    }
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




AiUInt32 mil_hw_init_coupling_capabilities_zynqmp( TY_API_DEV * p_api_dev )
{
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;

    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = MIL_NOVRAM_FRONTEND_CAPABILITIES_ISO(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = MIL_NOVRAM_FRONTEND_CAPABILITIES_DIR(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Transformer = MIL_NOVRAM_FRONTEND_CAPABILITIES_XFM(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Network     = MIL_NOVRAM_FRONTEND_CAPABILITIES_NET(p_api_dev->BoardSubType);
    p_api_dev->x_CouplingCapabilities.b.b_Packed      = MIL_NOVRAM_FRONTEND_CAPABILITIES_PC( p_api_dev->BoardSubType);

    return 0;
}

AiUInt32 mil_hw_init_coupling_capabilities_zynq( TY_API_DEV * p_api_dev )
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
    if( p_api_dev->ul_DeviceGroup == AI_DEVICE_ZYNQMP_ASP )
        return mil_hw_init_coupling_capabilities_zynqmp(p_api_dev);
    else
        return mil_hw_init_coupling_capabilities_zynq(p_api_dev);
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
    int i;
    int count;
    int ret = 0;
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;


    if( MIL_NOVRAM_HW_VARIANT_DISCRETES_EXTERNALLY_ABSENT(p_api_dev->HwVariant) )
    {
        count = 0;
    }
    else
    {
        switch (p_api_dev->BoardConfig & 0x000000FF)
        {
        case AI_PLATFORM_CPCIE_3U_ZYNQMP:
            count = 3;
            break;
        default:
            count = aim_gpio_count_get(aimDevice->gpio_dev, AIM_GPIO_SET_LOW);
            break;
        }
        if( count < 0 )
        {
            count = 0;
            ret = API_ERR_VALUE_NOT_IN_RANGE;
        }
    }

    *pulDiscreteCount = count;

    for (i=0; i < *pulDiscreteCount; i++)
        *pulDiscreteConfig += 1<<i;

    return ret;
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
    AiUInt32 ulDiscreteConfig;

    mil_hw_get_discretes( p_api_dev, pul_Channel, &ulDiscreteConfig );
    
    *pul_Canin   = 0; /* default value */
    *pul_Canout  = 0; /* default value */

    for (i = 0; i < *pul_Channel; i++)
    {
        *pul_Canin  += 1 << i;
        *pul_Canout += 1 << i;
    }
    
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
    switch (p_api_dev->ul_DeviceGroup)
    {
    case AI_DEVICE_AYS_ASP_MA:
        if ( ul_StreamIsMultiChannel(p_api_dev,ulBiu) )
            return MIL_FW_MCH_AT91;     // ANET-M2Ay
        else
            return MIL_FW_STD_AT91;     // ANET-M1Ay
    case AI_DEVICE_ZYNQMP_ASP:
        if (ul_StreamIsHS(p_api_dev, ulBiu))
            return MIL_FW_STG3838_CORTEXR5; // AYXX3910
        else
            return MIL_FW_STD_CORTEXR5;     // ASC1553-2
    case AI_DEVICE_AYS_ASP:
        if (ul_StreamIsHS(p_api_dev, ulBiu))
            return MIL_FW_STG3838_AT91; // ANET3910 (LS-FW->AT91, HS-FW->Zynq)
        else
            return MIL_FW_STD_CORTEXA9; // ASC1553-1
    }

    return MIL_FW_STD_AT91; // Fallback, allow most features.
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


/*! \brief Check if high res zero crossing error injection is supported.

    Check if this HW platform supports the High Resolution Zero
    Crossing Deviation error injection.

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
    return 0x00200000L; /* 2 MB */
}


/*! \brief Update the system state */
void mil_hw_update_system_state(  TY_API_DEV *p_api_dev, L_WORD ulState )
{
    p_api_dev->ulGlobalError = ulState;
}



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public versions access                        *
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
    \param AiUInt32* ulAslLca        ASL/SOC/PL version
    \param AiUInt32* ulLsIoLcaPbi2   LS IO LCA (second PBI)
    \param AiUInt32* ulHsIoLcaPbi2   HS IO LCA (second PBI)
    \param AiUInt32* ulFwBiu3Biu4    FW BIU3/BIU4
    \param AiUInt32* ulMonitor       Monitor

    \return returns 0 on success
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
    volatile L_WORD *pul_GlobMem = (L_WORD*)p_api_dev->GlobalRAMBase;

    /* -- PCI LCA --- */
    *ulPciLca = mil_hw_io_register_read(p_api_dev, AYS_REG_PL_VERSION );
    *ulPciLca = *ulPciLca>>16;/* No build supported here */

    /* Firmware BIU1 / BIU2 */
    *ulFwBiu1Biu2 = ((BSWAP32(pul_GlobMem[ 0x10008 / 4 ]) &0x0000ffff) << 16) |
                     (BSWAP32(pul_GlobMem[ 0x8     / 4 ]) &0x0000ffff);

    /* -- TSW version and build --- */
    *ulTSW = MIL_TSW_LEGACY_VERSION;
    *ulTSWBuild = MIL_TSW_LEGACY_BUILD;

    /* -- Board serial number --- */
    *ulSN = p_api_dev->BoardSerial;

    /* -- ASL LCA or SOC or PL Version --- */
    mil_hw_novram_read( p_api_dev, AIM_AYS_NVR_PL_VERSION, ulAslLca );
    *ulAslLca = *ulAslLca>>16;/* No build supported here */

    /* -- Not available --- */
    *ulLsIoLcaPbi2 = 0;
    *ulHsIoLcaPbi2 = 0;
    *ulFwBiu3Biu4  = 0;

    /* -- Return TCP as Monitor --- */

    *ulMonitor = mil_hw_io_register_read(p_api_dev, AYS_REG_TCP_SW_VERSION );
    *ulMonitor = *ulMonitor>>16;/* No build supported here */

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
    struct aim_device* aimDevice = (struct aim_device*)p_api_dev->p_Device;
    struct aim_platform_version  version;

    switch( eId )
    {
    /* Hardware components */
    case AI_FIRMWARE_VER_BIU1:
    case AI_FIRMWARE_VER_BIU2:
    case AI_TCP_VER:
    case AI_MAIN_LCA_VER:
    case AI_FPGA_VER:
    case AI_MONITOR_VER:
    case AI_BOOTSTRAP_VER:
    case AI_TARGET_OS_VER:
        retval = aim_platform_get_version( aimDevice->platformDevice, eId, &version );

        if( retval == 0 )
        {
            pxVersionValue->ul_VersionType  = version.ID;
            pxVersionValue->ul_MajorVer     = version.major;
            pxVersionValue->ul_MinorVer     = version.minor;
            pxVersionValue->ul_PatchVersion = version.patch;
            pxVersionValue->ul_BuildNr      = version.build;

            strncpy( pxVersionString->ac_Description, version.name,           AI_DESCRIPTION_STRINGL );
            strncpy( pxVersionString->ac_FullVersion, version.version_string, AI_DESCRIPTION_STRINGL );
        }
        else
        {
            retval = API_ERR_INVALID_MODE;
        }

        break;
    case AI_TARGET_VER:
        retval = api_sys_read_version_tsw(  p_api_dev, eId, pxVersionValue, pxVersionString );
        break;
    default:
        retval = API_ERR_INVALID_MODE;
    }

    return retval;
}






/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public Memory access                          *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

L_WORD mil_hw_io_register_read( TY_API_DEV * p_api_dev, L_WORD register_index )
{
    return BSWAP32(p_api_dev->IoRAMBase[register_index]);
}

L_WORD mil_hw_io_register_write( TY_API_DEV * p_api_dev, L_WORD register_index, L_WORD value )
{
    p_api_dev->IoRAMBase[register_index] = BSWAP32(value);
    return 0;
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
 *                              PXI Access                                   *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
typedef struct ty_api_pxi_mapping
{
    AiInt8  cOutputPBI2;
    AiInt8  cOutputPBI4;
    AiInt8  cInputPBI2;
    AiInt8  cInputPBI4;
    AiInt8  cInputRegPBI4;
} TY_API_PXI_MAPPING;


#define MAP_API_PXI_TRIGGER_TABLE_SIZE 21

static const TY_API_PXI_MAPPING map_api_pxi_trigger_to_hw_def[] = {
    /*        {cOutputPBI2, cOutputPBI4, cInputPBI2, cInputPBI4, cInputRegPBI4}*/
    /*PXI0 */{ -1, -1, -1, -1, 0 },
    /*PXI1 */{ -1, -1, -1, -1, 0 },
    /*PXI2 */{ -1, -1, -1, -1, 0 },
    /*PXI3 */{ -1, -1, -1, -1, 0 },
    /*PXI4 */{ -1, -1, -1, -1, 0 },
    /*PXI5 */{ -1, -1, -1, -1, 0 },
    /*PXI6 */{ -1, -1, -1, -1, 0 },
    /*PXI7 */{ -1, -1, -1, -1, 0 },
    /*BC1  */{ 4, 4, 4, 4, 1 },
    /*BC2  */{ 5, 5, 5, 5, 1 },
    /*BC3  */{ -1, 12, -1, 2, 2 },
    /*BC4  */{ -1, 13, -1, 3, 2 },
    /*RT1  */{ 2, 6, 2, 2, 1 },
    /*RT2  */{ 3, 7, 3, 3, 1 },
    /*RT3  */{ -1, 8, -1, 0, 2 },
    /*RT4  */{ -1, 9, -1, 1, 2 },
    /*BM1  */{ 0, 0, 0, 0, 1 },
    /*BM2  */{ 1, 1, 1, 1, 1 },
    /*BM3  */{ -1, 10, -1, 6, 1 },
    /*BM4  */{ -1, 11, -1, 7, 1 },
    /*BM HS*/{ 14, -1, -1, -1, 0 }
};

/*! \brief Check if PXI is supported on this device  */
/*!
    This function returns true if PXI is supported and false in any other case.

    \param  p_api_dev      TY_API_DEV*             The global variable container.

    \return                AiBoolean               True on PXI support
*/
AiBoolean mil_hw_pxi_is_supported(        TY_API_DEV* p_api_dev )
{
    if (p_api_dev->HwVariant & (1 << 8))
        return AiTrue;
    else
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
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;

    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN1_SELECT] = BSWAP32(0x00000000); /* Clear all Bits to default settings in PXI Trigger Input Select Register 1 */
    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN2_SELECT] = BSWAP32(0x00000000); /* Clear all Bits to default settings in PXI Trigger Input Select Register 2 */
    return 0;

}

/*! \brief Disable all PXI output triggers  */
/*!
    This function disables all output triggers and resets the register to the default values.

    \param  p_api_dev      TY_API_DEV*             The global variable container.

    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_disable( TY_API_DEV* p_api_dev )
{
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;

    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_SELECT] = BSWAP32(0x00);
    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_ENABLE] = BSWAP32(0xFF);

    return 0;
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
    /* PXI_TTAGSrc_Ctrl(UINT32 TTAGSrc, UINT32 TTClrEn) */
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;
    AiUInt32   TTCtrl = 0;

    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if (TTAGSrc == 1)
        TTCtrl = TTCtrl | 0x00000001;

    if (TTClrEn == 1)
        TTCtrl = TTCtrl | 0x00000002;

    aulIo[AI_HW_ZYNQMP_REG_PXI_IRIG_SOURCE_CONTROL] = BSWAP32(TTCtrl);

    return 0;
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
    /* PXI_TrgIn_Sel(UINT8 PBIType, UINT8 TrgInLine, UINT32 PXITrgLine) */
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;
    AiUInt32 OEReg = 0;
    AiInt32  LineNr = 0;
    AiUInt32 PXITrgInSelReg1, PXITrgInSelReg2;


    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    if (TrgInLine > MAP_API_PXI_TRIGGER_TABLE_SIZE)
        return API_ERR_VALUE_NOT_IN_RANGE;


    PXITrgInSelReg1 = BSWAP32(aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN1_SELECT]);
    PXITrgInSelReg2 = BSWAP32(aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN2_SELECT]);

    if (p_api_dev->uc_DeviceType != TYPE_APE1553_4)
    {
        /* ------------------------------ 1553-1(DS)/-2(DS) PBI ----------------------------------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgInLine].cInputPBI2;

        if (LineNr < 0)
            return API_ERR_VALUE_NOT_IN_RANGE;

        PXITrgInSelReg1 = (PXITrgInSelReg1 | (PXITrgLine = (PXITrgLine << (LineNr * 4))));
    }
    else
    {
        /* ------------------------------ 1553-4 PBI ----------------------------------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgInLine].cInputPBI4;

        if (LineNr < 0)
            return API_ERR_VALUE_NOT_IN_RANGE;

        if (map_api_pxi_trigger_to_hw_def[TrgInLine].cInputRegPBI4 == 2)
        {
            PXITrgInSelReg2 = (PXITrgInSelReg2 | (PXITrgLine = (PXITrgLine << (LineNr * 4))));
        }
        else
        {
            PXITrgInSelReg1 = (PXITrgInSelReg1 | (PXITrgLine = (PXITrgLine << (LineNr * 4))));
        }
    }

    // Determine which PXI Trigger Input Buffer must be enabled
    if (TrgInLine == API_TRG_BM_CHN1)  OEReg = (OEReg | 0x00100000);
    if (TrgInLine == API_TRG_BM_CHN2)  OEReg = (OEReg | 0x00200000);
    if (TrgInLine == API_TRG_RT_CHN1)  OEReg = (OEReg | 0x00400000);
    if (TrgInLine == API_TRG_RT_CHN2)  OEReg = (OEReg | 0x00800000);
    if (TrgInLine == API_TRG_BC_CHN1)  OEReg = (OEReg | 0x01000000);
    if (TrgInLine == API_TRG_BC_CHN2)  OEReg = (OEReg | 0x02000000);
    if (TrgInLine == API_TRG_BM_CHN3)  OEReg = (OEReg | 0x04000000);
    if (TrgInLine == API_TRG_BM_CHN4)  OEReg = (OEReg | 0x08000000);
    if (TrgInLine == API_TRG_RT_CHN3)  OEReg = (OEReg | 0x10000000);
    if (TrgInLine == API_TRG_RT_CHN4)  OEReg = (OEReg | 0x20000000);
    if (TrgInLine == API_TRG_BC_CHN3)  OEReg = (OEReg | 0x40000000);
    if (TrgInLine == API_TRG_BC_CHN4)  OEReg = (OEReg | 0x80000000);


    OEReg = OEReg & 0xFFF00000;
    PXITrgInSelReg2 = PXITrgInSelReg2 | OEReg;

    /* Assign the PXI-Trigger Lines to the BIU-Triggers and enable the PXI Trigger Input Buffers */
    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN1_SELECT] = BSWAP32(PXITrgInSelReg1);
    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN2_SELECT] = BSWAP32(PXITrgInSelReg2);

    return 0;
}


/*! \brief Connect a PXI trigger line with PBI input trigger line */
/*

    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgOutLine     AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger input line

    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_select(  TY_API_DEV* p_api_dev, AiUInt8 TrgOutLine, AiUInt8 PXITrgLine )
{
    /* PXI_TrgOut_Sel(UINT8 PBIType, UINT8 TrgOutLine, UINT8 PXITrgLine) */
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;
    AiUInt32 OEReg;
    AiUInt8  LineNr = 0;
    AiUInt32 PXITrgOutSelReg, PXITrgOutEnaReg;


    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;


    if (TrgOutLine > MAP_API_PXI_TRIGGER_TABLE_SIZE)
        return API_ERR_VALUE_NOT_IN_RANGE;


    PXITrgOutSelReg = BSWAP32(aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_SELECT]);
    PXITrgOutEnaReg = BSWAP32(aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_ENABLE]);


    if (p_api_dev->uc_DeviceType == TYPE_APE1553_4)
    {
        /* -------- If 1553-4 PBI -------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgOutLine].cOutputPBI4;
    }
    else
    {
        /* -------- If 1553-2/-1 PBI -------- */
        LineNr = map_api_pxi_trigger_to_hw_def[TrgOutLine].cOutputPBI2;
    }

    //if( LineNr < 0 )
    //return API_ERR_VALUE_NOT_IN_RANGE;

    PXITrgOutSelReg = (PXITrgOutSelReg | (LineNr << (PXITrgLine * 4)));


    // Determine which PXI Trigger Input Buffer must be enabled
    OEReg = 0xFF;
    if (PXITrgLine == API_TRG_PXI0)   OEReg = (OEReg & 0xFE);
    if (PXITrgLine == API_TRG_PXI1)   OEReg = (OEReg & 0xFD);
    if (PXITrgLine == API_TRG_PXI2)   OEReg = (OEReg & 0xFB);
    if (PXITrgLine == API_TRG_PXI3)   OEReg = (OEReg & 0xF7);
    if (PXITrgLine == API_TRG_PXI4)   OEReg = (OEReg & 0xEF);
    if (PXITrgLine == API_TRG_PXI5)   OEReg = (OEReg & 0xDF);
    if (PXITrgLine == API_TRG_PXI6)   OEReg = (OEReg & 0xBF);
    if (PXITrgLine == API_TRG_PXI7)   OEReg = (OEReg & 0x7F);

    OEReg = OEReg & 0x000000FF;
    PXITrgOutEnaReg = (PXITrgOutEnaReg & OEReg);


    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_SELECT] = BSWAP32(PXITrgOutSelReg); /* Assign the PXI-Trigger Lines to the BIU-Triggers */
    aulIo[AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_ENABLE] = BSWAP32(PXITrgOutEnaReg); /* Set PXI Trigger Output Enable Register */

    return 0;
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

    if (!mil_hw_pxi_is_supported(p_api_dev))
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    *pxiGeographicalAddress = p_api_dev->IoRAMBase[AI_HW_ZYNQMP_REG_PXI_GEOGRAPHICAL_ADDR] & AI_HW_ZYNQMP_REG_PXI_GEOGRAPHICAL_ADDR_MASK;

    return API_OK;

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
    return !MIL_NOVRAM_HW_VARIANT_TRIGGER_EXTERNALLY_ABSENT(p_api_dev->HwVariant);
}

/*! \brief Check if trigger digital loop is available in this FPGA */
/*!
This function returns true if digital trigger loop is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_digital_loop(TY_API_DEV* p_api_dev)
{
    /* ANET3910, ASC1553-1 (AyS-ASP) */
    /* ANET-MxAy           (AyS-ASP) */
    /* APEX3910, ASC1553-2 (ZynqMP-ASP) */
    return AiTrue;
}


/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    p_api_dev->IoRAMBase[AYS_REG_TRIGGER_DIGITAL_LOOP] = con;
    return 0;
}

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con)
{
    *con = p_api_dev->IoRAMBase[AYS_REG_TRIGGER_DIGITAL_LOOP];
    return 0;
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
    return API_ERR_PLATTFORM_NOT_SUPPORTED;}


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
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_get_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
}


AiUInt32 mil_hw_scope_calibrate( TY_API_DEV * p_api_dev, AiUInt8 ucMode )
{
    /* For Boards without ASP the calibration is done within the DLL */
    return API_ERR_FUNCTION_NOT_SUPPORTED;
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
    AiUInt32 power_class = 0;
    AiUInt32 * aulIo = p_api_dev->IoRAMBase;

    switch(p_api_dev->BoardConfig & 0x000000FF)
    {
        case AI_PLATFORM_USB3:
            /* Report the power class */
            power_class = aulIo[AYS_REG_IO_USB_C_STATUS] & 0x7; /*Bit 2:0 */
            break;
        default:
            /* For non USB devices simply report full power mode (No restrictions) */
            power_class = 0;
            break;
    }
    return power_class;
}


#endif /* defined(_AIM_SYSDRV) */

