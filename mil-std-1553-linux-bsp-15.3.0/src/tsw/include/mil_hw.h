/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*********************************************************************************/
/*! \file mil_hw.h 

          This file contains hw layer prototypes. Each of this prototypes must be
          implemented in all of the hw specific implementation files.

          mil_hw.h 
          mil_hw_ayx.c          => APX, ACX, AVX
          mil_hw_usb_driver.c   => APU
          mil_hw_aye_embedded.c => ANET-AT91
          mil_hw_ays_embedded.c => ASC1553, AYXX3910, ANET3910, ANET-MxAy

          mil_hw_pci_driver.c: (devices without ASP)
                mil_hw_pci_driver_aye.c    => AEC, AXC, AXE, ASE, AMCX, AMCE, APE, ACE, ACXX, APXX
                mil_hw_pci_driver_ays.c    => AME
                mil_hw_pci_driver_artix7.c => AMEE
*/
/*********************************************************************************/



#ifndef __MIL_HW_H__
#define __MIL_HW_H__





#include "api_defv.h"



#define MIL_HW_IS_MIXED(p) ((p->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA)?AiTrue:AiFalse)
#define MIL_HW_BOARD_TYPE_MASK(p)   (p->BoardType|0x0000FF00)
#define MIL_HW_BOARD_TYPE_NOMASK(p) (p->BoardType)

#define MIL_BOARD_TYPE(p) (MIL_HW_IS_MIXED(p)?MIL_HW_BOARD_TYPE_MASK(p):MIL_HW_BOARD_TYPE_NOMASK(p))


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *            HW Layer public TSW structures and definitions                 *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

typedef struct ty_api_hw_init_params
{
    void   * mon_data;          // Reserved/Unused
    void   * pxMyMonContext;
    void   * pDevice;
    void   * pDeviceContext;
    
    BYTE   * SharedRAMBase;
    BYTE   * GlobalRAMBase;
    BYTE   * GlobalRAMBase2;
    L_WORD * IORAMBase;
    L_WORD   SharedRAMSize;
    L_WORD   GlobalRAMSize;
    L_WORD   GlobalRAMSize2;
    BYTE   * HostIoRAMBase;
    BYTE   * GlobalRAMExtBase;
    L_WORD   GlobalRAMExtSize;

    L_WORD   PlatformGroup;

    L_WORD   BoardSerial;
    L_WORD   BoardConfig;
    L_WORD   BoardType;
    L_WORD   BoardSubType;
    L_WORD   BoardSubType2;   // BIU 3/4 on ArtixUS (Frontent Capabilities)
    L_WORD   BoardPartNo;
    L_WORD   CPUClk;          // Reserved/Unused
    L_WORD   SYSClk;          // Reserved/Unused
    L_WORD   BoardAppType;
    L_WORD   BoardAppType2;   // BIU 3/4 on ArtixUS
    L_WORD   HwVariant;
}TY_API_HW_INIT_PARAMS;







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
BYTE mil_hw_initialize_tsw( TY_API_DEV * p_api_dev, TY_API_HW_INIT_PARAMS * px_tsw_config );




/*! \brief This function shuts the TSW down and frees the allocated memory */
/*!
    This function shuts the TSW down and frees the allocated memory 
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    
    \return                BYTE                    Zero on normal termination
*/
BYTE mil_hw_shutdown_tsw( TY_API_DEV * p_api_dev );



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
BYTE mil_hw_reboot_tsw( TY_API_DEV * p_api_dev );



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
void mil_hw_signal_asp_running( TY_API_DEV * p_api_dev );





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
void mil_hw_signal_asp_stopped( TY_API_DEV * p_api_dev );











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
BYTE mil_hw_switch_available( TY_API_DEV * p_api_dev );




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
                            AiUInt32     ulBoardType );




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
                              AiUInt32     ulBiu2Freq );



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
                              AiUInt32     ulBiu2Freq );



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
                              AiUInt32   * pulHsBiuFreq );











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
                            AiUInt32     ulValue );


/*! \brief Read from the given NOVRAM offset. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ulOffset           AiUInt32         The NOVRAM offset
    \param  pulValue           AiUInt32*        The NOVRAM value
    
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_read( TY_API_DEV * p_api_dev,
                           AiUInt32     ulOffset,
                           AiUInt32*    pulValue );

/*! \brief Start the NOVRAM write. */
/*!
    Reserved for future use.

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_start( TY_API_DEV * p_api_dev );


/*! \brief Finalize the NOVRAM write. */
/*!
    This is command is unknown by older StreamUtils. To stay compatible this
    command does different things on different plattforms.

    AyX:  This command programs the LCA clocks according to the NOVRAM settings.
          The AyX does not need an additional flush command.

    AMC-T This command does nothing.
    

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_finalize( TY_API_DEV * p_api_dev );










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
AiBoolean mil_hw_irig_has_external_input(TY_API_DEV* p_api_dev);



/*! \brief This function sets the hw irig time through the TCP. */
/*!
    This function handles the 'API_EXEC_SYS' instruction 1

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  con        BYTE             System Command control
    
    \return ackfl      BYTE             Instruction acknowledge type.
*/
L_WORD mil_hw_irig_set_external(TY_API_DEV *p_api_dev, BYTE con);

/*! \brief This function sets the hw irig time through the TCP. */
/*!
    This function handles the 'API_SET_IRIG_TIME' instruction to set the
    IRIG encoder of the TCP (Time Code Processor)

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \param  day        L_WORD             IRIG time encode day
    \param  hour       L_WORD             IRIG time encode hour
    \param  min        L_WORD             IRIG time encode minute
    \param  sec        L_WORD             IRIG time encode second
    \param  us         L_WORD             IRIG time encode micro seconds

    \return ackfl      L_WORD           Return code
*/
L_WORD mil_hw_irig_set( TY_API_DEV * p_api_dev, 
                        L_WORD         day, 
                        L_WORD         hour, 
                        L_WORD         min, 
                        L_WORD         sec,
                        L_WORD         us);



/*! \brief This function reads the hw irig source through the TCP. */
/*!
    \param  p_api_dev  TY_API_DEV*      The global variable container.
    \return ackfl      L_WORD           Return code
*/
L_WORD mil_hw_irig_get_status(TY_API_DEV *p_api_dev, AiUInt32 * irig_sync, AiUInt32 * irig_src );


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
    
    \return ackfl       L_WORD           Return code
*/
L_WORD mil_hw_irig_get_time( TY_API_DEV * p_api_dev, 
                           L_WORD     * day, 
                           L_WORD     * hour, 
                           L_WORD     * min, 
                           L_WORD     * sec,
                           L_WORD     * usec, 
                           L_WORD     * sth, 
                           L_WORD     * stl );






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
AiReturn mil_hw_discretes_fifo_read(TY_API_DEV *p_api_dev, AiUInt32 int_count);

/*! \brief Setup Discrete interrupt and enable loglist */
/*!

\param  p_api_dev          TY_API_DEV*      The global variable container.
\param  ul_DiscreteSetup   AiUInt32         Setup Discrete interrupt and loglist enable

\return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_fifo(TY_API_DEV *p_api_dev, AiUInt32 new_config);

/*! \brief Initialize discrete input/output */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \param  ul_DiscreteSetup   AiUInt32         The discrete setup

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_init(TY_API_DEV *p_api_dev, AiUInt32 ul_DiscreteSetup );

/*! \brief Write discrete output values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Mask         AiUInt32         Output mask
    \param  Value        AiUInt32         Output value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_write(TY_API_DEV *p_api_dev, AiUInt32 Mask, AiUInt32 Value);

/*! \brief Read discrete input values */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.
    \param  Value        AiUInt32         input value

    \return AiReturn The return value is 0 on success or non zero on failure.
*/
AiReturn mil_hw_discretes_read(TY_API_DEV *p_api_dev, AiUInt32 * Value);

AiReturn mil_hw_discretes_info_read(TY_API_DEV * p_api_dev, AiUInt32 *pul_Channel, AiUInt32 *pul_Canin, AiUInt32 *pul_Canout);



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

AiUInt32 mil_hw_convert_stream_to_LogicalBiu( TY_API_DEV * p_api_dev, AiUInt32 ulStream, AiUInt32 ulHs, AiInt16 * pwLogicalBiu, AiInt16 * pwMemBank );


/*! \brief Get the start offset of the global Memory bank */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulGlobalRamStartOffset AiUInt32*      Output variable for the Offset of the start of the Global Ram for this stream.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_GlobalRamStartOffset( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 *pulGlobalRamStartOffset );




/*! \brief Get some information whether the amplitude can be changed for this stream */
/*!
\param  p_api_dev               TY_API_DEV*    The global variable container.
\param  ulBiu                   AiUInt32       Input variable for the logical BIU.
\param  pulCanChangeAmplitude   AiUInt32*      Output variable showing if the amplitude can be modified.
\param  pulPacked               AiUInt32*      Output variable showing which is true if Bus A and B can not be changed independently.

\return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude(TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulCanChangeAmplitude, AiUInt32 * pulPacked);

/*! \brief Get some information whether the amplitude can be changed in high resolution mode for this stream */
/*!
\param  p_api_dev               TY_API_DEV*    The global variable container.
\param  ulBiu                   AiUInt32       Input variable for the logical BIU.
\param  pulCanChangeAmplitudeHighRes   AiUInt32*      Output variable showing if the amplitude can be modified in high resolution.

\return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude_high_res(TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulCanChangeAmplitudeHighRes );



/*! \brief Initialize irig capabilities within p_api_dev */
/*! 
    \param  p_api_dev             TY_API_DEV*            The global variable container.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_irig_capabilities( TY_API_DEV * p_api_dev );



/*! \brief Initialize the coupling capabilities struct within p_api_dev to the initial values */
/*! 
    \param  p_api_dev               TY_API_DEV*               The global variable container.
    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_init_coupling_capabilities( TY_API_DEV * p_api_dev );


/*! \brief Get some information about the discrete channels */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  pulDiscreteCount        AiUInt32*      Output variable for the ammount of discrete channels.
    \param  pulDiscreteConfig       AiUInt32*      Output variable for the configuration of the discrete channel. 
                                                   The bits of this field are one if this discrete can send data or zero if not.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_discretes( TY_API_DEV * p_api_dev, AiUInt32 * pulDiscreteCount, AiUInt32 * pulDiscreteConfig );


/*! \brief Get some information about the interrupt capabilities */
/*! 
    \param  p_api_dev               TY_API_DEV*          The global variable container.
    \param  ulBiu                   AiUInt32             Input variable for the logical BIU.
    \param  pulIRCapabilities       TY_IR_CAPABILITIES*  Output variable for the interrupt capabilities.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_ir_capabilities( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, TY_IR_CAPABILITIES * pulIRCapabilities );



/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure
\return returns The type of the FW 
*/
enum MIL_FW mil_hw_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu);


/*! \brief Checks whether the hs biu is redundant or not */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulIsHsRedundant        AiUInt32*      Output variable for the result (zero or one).

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_get_is_hs_redundant( TY_API_DEV * p_api_dev, AiUInt32 ulBiu, AiUInt32 * pulIsHsRedundant );



/*! \brief Check if high res zero crossing error injectio is supported.

    Check if this HW plattform supports the High Resolution Zero
    Crossing Deviatio error injection.

    \param p_api_dev pointer to target administration structure
    \return returns 1 if High Resolution Zero Crossing is supported 
*/
AiUInt32 mil_hw_is_high_res_zero_crossing_supported( TY_API_DEV* p_api_dev );



/*! \brief Return the default size of the monitor buffer in the global RAM */
AiUInt32 mil_hw_get_bm_size( TY_API_DEV* p_api_dev, AiUInt32 ucHs );





/*! \brief Update the system state */
void mil_hw_update_system_state(  TY_API_DEV *p_api_dev, L_WORD ulState );



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
                             AiUInt32* ulMonitor );



/*! \brief Read a specific versions for ApiReadVersions

    \param p_api_dev        Global variable container
    \param eId              The version to read
    \param pxVersionValue   The numeric version representation
    \param pxVersionString  The string version representation

    \return returns 1 if High Resolution Zero Crossing is supported
*/
L_WORD mil_hw_read_version(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString );



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public Memory access                          *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

L_WORD mil_hw_io_register_read( TY_API_DEV * p_api_dev, L_WORD register_index );

L_WORD mil_hw_io_register_write( TY_API_DEV * p_api_dev, L_WORD register_index, L_WORD value );


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public PCI Bus Master access                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


/*! \brief Read a 32 bit long word from a physical PCI address */
L_WORD mil_hw_pci_readlw( L_WORD ulPciAddress );

/*! \brief Write a 32 bit long word to a physical PCI address */
void mil_hw_pci_writelw( L_WORD ulPciAddress, L_WORD ulValue );

/*! \brief Read a 32 bit long word from the FPGA */
L_WORD mil_hw_ioreg_read(TY_API_DEV *p_api_dev, L_WORD IO_Reg, L_WORD *pulValue);

/*! \brief Write a 32 bit long word to the FPGA */
L_WORD mil_hw_ioreg_write(TY_API_DEV *p_api_dev, L_WORD IO_Reg, L_WORD ulValue);




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
AiBoolean mil_hw_pxi_is_supported(        TY_API_DEV* p_api_dev );



/*! \brief Disable all PXI input triggers  */
/*!
    This function disables all input triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_disable(  TY_API_DEV* p_api_dev );

/*! \brief Disable all PXI output triggers  */
/*!
    This function disables all output triggers and resets the register to the default values.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_disable( TY_API_DEV* p_api_dev );


/*! \brief Control the PXI irig source */
/*!
    This function enables, disables or clears the PXI irig signal.
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TTAGSrc        AiUInt32                Enable PXI irig if value is 1
    \param  TTAGClrEn       AiUInt32               Clear PXI Irig if value is 1
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_timetag_src_control( TY_API_DEV* p_api_dev, AiUInt32 TTAGSrc, AiUInt32 TTClrEn );


/*! \brief Connect a PBI trigger line with PXI output trigger line */
/*!
   
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgInLine      AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger output line
   
    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_select(   TY_API_DEV* p_api_dev, AiUInt8 TrgInLine, AiUInt32 PXITrgLine );


/*! \brief Connect a PXI trigger line with PBI input trigger line */
/*!
    
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  TrgOutLine     AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger input line
   
    \return                AiReturn                Zero on success
*/

AiReturn  mil_hw_pxi_trigger_out_select(  TY_API_DEV* p_api_dev, AiUInt8 TrgOutLine, AiUInt8 PXITrgLine );

/*! \brief Get PXI Geographic Address  */
/*!
This function returns the PXI Geographic Address.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  geo_addr       AiUInt32*               The PXI Geographic Address

\return                AiReturn                Zero on success
*/
AiReturn mil_hw_pxi_geo_addr(TY_API_DEV* p_api_dev, AiUInt32 *pxiGeographicalAddress);

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
AiBoolean mil_hw_trigger_has_external_input(TY_API_DEV* p_api_dev);

/*! \brief Check if trigger digital loop is available in this FPGA */
/*!
This function returns true if digital trigger loop is supported and false in any other case.

\param  p_api_dev      TY_API_DEV*             The global variable container.

\return                AiBoolean               True on trigger support
*/
AiBoolean mil_hw_trigger_has_digital_loop(TY_API_DEV* p_api_dev);

/*! \brief Configure digital trigger loop */
/*!
This function configures the digital trigger loop.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32                Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_con(TY_API_DEV* p_api_dev, AiUInt32 con);

/*! \brief Read digital trigger loop Configuration */
/*!
This function reads the digital trigger loop configures.

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  con            AiUInt32*               Configuration value.

*/
AiReturn mil_hw_trigger_digital_loop_get(TY_API_DEV* p_api_dev, AiUInt32 * con);


/*! \brief Configure the trigger input edge for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32                The trigger edges

\return                AiReturn                Zero on success
*/
AiReturn  mil_hw_trigger_edge_input_set(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 edge_flags);


/*! \brief Get the trigger input edge configuration for this biu */
/*!

\param  p_api_dev      TY_API_DEV*             The global variable container.
\param  logical_biu    AiUInt32                The logical biu
\param  edge_flags     AiUInt32*               The trigger edges output

\return                AiReturn                Zero on success
*/

AiReturn  mil_hw_trigger_edge_input_get(TY_API_DEV* p_api_dev, AiUInt32 logical_biu, AiUInt32 * edge_flags);

/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                      HW Layer public TSW MILScope                         *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/



/*! \brief Platform specific function for setting up scope functionality

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param p_scope_setup pointer to \ref TY_API_SCOPE_SETUP structure which contains the scope settings
    \return returns 0 on success, an appropriate error code on failure */
AiUInt32 mil_hw_scope_setup(TY_API_DEV* p_api_dev, TY_API_SCOPE_SETUP* p_scope_setup);



/*! \brief Platform specific function for setting up scope trigger functionality

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param p_scope_trigger pointer to \ref TY_API_SCOPE_TRG_EX structure which contains the scope trigger settings
    \return returns 0 on success, an appropriate error code on failure */
AiUInt32 mil_hw_scope_trigger(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger);



/*! \brief Platform specific function to start milscope 

    \param p_api_dev pointer to \ref TY_API_DEV target administration structure
    \param ulMode mode the milscope shall run. '0' means single shot, '1' means continuous capturing
    \param ulNoOfCycles ammount of cycles to run (only used in single shot mode)
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_start(TY_API_DEV* p_api_dev, AiUInt32 ulMode, AiUInt32 ulNoOfCycles );


/*! \brief Platform specific function to get scope status of a board

    This function currently only supports AYE boards
    \param[in] p_api_dev pointer to target administration structure
    \param[out] p_Status pointer that receives scope status
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_status(TY_API_DEV* p_api_dev, TY_API_SCOPE_STATUS* p_Status);


/*! \brief Platform specific function to stop scope of a board

    \param[in] p_api_dev pointer to target administration structure
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_stop(TY_API_DEV* p_api_dev);



/*! \brief Platform specific function to set scope trigger 

    \param[in] p_api_dev pointer to target administration structure
    \param[in] p_scope_trigger_strobe pointer to \ref TY_API_SCOPE_TRG_STROBE structure that holds strobe settings
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_STROBE* p_scope_trigger_strobe);


/*! \brief Platform specific function to reset scope functionality

    \param[in] p_api_dev pointer to target administration structure
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_reset(TY_API_DEV* p_api_dev);


/*! \brief Platform specific function to set scope offsets

    This function has to set the offset values to compensate the 
    input pitch from zero-line.
    \param p_api_dev pointer to target administration structure
    \param p_scope_offsets offset values to set
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_set_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets);


/*! \brief Platform specific function to get scope offsets

    This function has to read back the offset values which are used
    to compensate the input pitch from zero-line.
    \param p_api_dev pointer to target administration structure
    \param p_scope_offsets stores the read back offset values
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_get_offsets(TY_API_DEV* p_api_dev, TY_API_SCOPE_OFFSETS* p_scope_offsets);


/*! \brief Milscope calibrate funciton for devices with ASP

    \param p_api_dev pointer to target administration structure
    \param ucMode The calibration mode
    \return returns 0 on success, an appropriate error code otherwise */
AiUInt32 mil_hw_scope_calibrate( TY_API_DEV * p_api_dev, AiUInt8 ucMode );


/*! \brief Shows which type of Milscope implementation is supported

\param p_api_dev pointer to target administration structure
\return returns API_MILSCOPE_TYPE_NONE, API_MILSCOPE_TYPE_AYX or API_MILSCOPE_TYPE_AYE */

AiUInt32 mil_hw_scope_type_get(TY_API_DEV *p_api_dev);


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
    \return returns The power class */
AiUInt32 mil_hw_usb_aim_power_class(TY_API_DEV* p_api_dev);


#endif /* __MIL_HW_H__ */

/* end: mil_hw.h */
