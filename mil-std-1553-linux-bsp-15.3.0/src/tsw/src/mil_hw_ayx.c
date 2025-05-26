/* SPDX-FileCopyrightText: 2018-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_hw_ayx.c
  This file contains hw layer implementation for AyX devices.

  api_hw_layer.h 
  api_hw_layer_ayx.c => APX, ACX, AVX
*/


#if defined(_NUCLEUS) && !defined(_AMC1553)
/* AyX only */

#include "ini_defs.h"
#include "mil_tsw_includes.h"
#include "mil_novram.h"


L_WORD ulSaveDriverInterruptSetup = 0;


#define MIL_HW_AYX_HISR_STACK_SIZE      800
#define MIL_HW_AYX_HISR_STACK_OVERFLOW  128
#define ATU_OMWTVR1_VIRTUAL_ADDR		0xFB000000




/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                      HW Layer internal functions                          *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
static BYTE mil_hw_initialize_memory( TY_API_DEV * p_api_dev )
{
  memset( px_api_dev, 0, sizeof(TY_API_DEV) );
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
static BYTE mil_hw_initialize_api_dev( TY_API_DEV *  p_api_dev )
{

  p_api_dev->SharedRAMBase = (BYTE*)(SharedRAMBase + 0x4000);/* Shared RAM base address  */

  /* copy all global monitor variables, which are set by function enter_hw,  */
  /* into data structure                                                     */
  p_api_dev->clk_1us        = clk_1us;
  p_api_dev->GlobalRAMBase  = (BYTE*)GlobalRAMBase; /* Global RAM base address          */
  p_api_dev->GlobalRAMSize  = GlobalRAMSize; /* Global RAM size in bytes         */
  p_api_dev->GlobalRAMBase2 = (BYTE*)GlobalRAMBase2; /* Global RAM base address          */
  p_api_dev->GlobalRAMSize2 = GlobalRAMSize2; /* Global RAM size in bytes         */
  p_api_dev->IoRAMBase      = NULL;           /* AMC-T uses global monitor px_PmcIoRegs for FPGA access */
  p_api_dev->HostIoRAMBase  = NULL;
  p_api_dev->BoardSubType   = BOARD_Subtype;  /* extended board information       */
  p_api_dev->BoardSubType2  = 0;
  p_api_dev->HwVariant      = HW_Variant;     /* hardware variant                 */
  p_api_dev->SharedRAMSize  = SharedRAMSize; /* Shared RAM size in bytes         */
  p_api_dev->LocalRAMBase   = LocalRAMBase;  /* Local RAM base address           */
  p_api_dev->LocalRAMSize   = LocalRAMSize;  /* Local RAM size in bytes          */
  p_api_dev->BoardSerial    = BoardSerial;   /* board serial number              */
  p_api_dev->BoardConfig    = BoardConfig;   /* board configuration              */
  p_api_dev->BoardType      = BoardType;     /* board type information           */

  p_api_dev->BoardPartNo    = BoardPartNo;   /* board part number                */
  p_api_dev->CPUClk         = CPUClk;        /* CPU Clock Frequency MHz          */
  p_api_dev->SYSClk         = SYSClk;        /* System Clock Frequency MHz       */
  p_api_dev->biu1BootSec    = biu1BootSec;   /* boot sector for BIU1             */
  p_api_dev->biu2BootSec    = biu2BootSec;   /* boot sector for BIU2             */
  p_api_dev->biu3BootSec    = biu3BootSec;   /* boot sector for BIU3             */
  p_api_dev->biu4BootSec    = biu4BootSec;   /* boot sector for BIU4             */

  p_api_dev->ul_DeviceGroup = AI_DEVICE_AYX;/* device group                     */

  p_api_dev->ul_TCP_FileNameExt   = TCP_FileNameExt; 
  p_api_dev->ul_ASP_FileNameExt   = ASP_FileNameExt; 
  p_api_dev->ul_BIP1_FileNameExt  = BIP1_FileNameExt;
  p_api_dev->ul_BIP2_FileNameExt  = BIP2_FileNameExt;
  p_api_dev->ul_BIP3_FileNameExt  = BIP3_FileNameExt;
  p_api_dev->ul_BIP4_FileNameExt  = BIP4_FileNameExt;
  p_api_dev->ul_ILCA1_FileNameExt = ILCA1_FileNameExt;
  p_api_dev->ul_ILCA2_FileNameExt = ILCA2_FileNameExt;
  p_api_dev->ul_ILCA3_FileNameExt = ILCA3_FileNameExt;
  p_api_dev->ul_ILCA4_FileNameExt = ILCA4_FileNameExt;
  p_api_dev->ul_PLCA_FileNameExt  = PLCA_FileNameExt;
  p_api_dev->lcaBootSec           = lcaBootSec;    /* LCA boot sector (1...31) 32= NO  */

  p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_3 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_4 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_5 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_6 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_7 - 1] = API_TYPE_NONE;
  p_api_dev->auc_StreamType[BIU_8 - 1] = API_TYPE_NONE;

  p_api_dev->ul_GenIo_TestSeq_Running  = 0;


  /*** Init Target Communication pointers and area ***/
  p_api_dev->targ_com_reg_p = (TY_API_TARG_COM_REG*)API_TARG_COM_REG_ADDR;
  p_api_dev->cmd_p = (BYTE*)API_TARG_COM_CMD_ADDR; /* Target Command area */
  p_api_dev->ack_p = (BYTE*)API_TARG_COM_ACK_ADDR; /* Target Acknowledge area */

  return 0;
}




/*! \brief This function can change the bus arbitration for special products. */
/*!
    This function changes the arbitration for the following products:
    
      1. AyX MilScope

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_adjust_arbitration( TY_API_DEV *  p_api_dev )
{
  /* in case of MILScope give more arbitration priority to the ASP */
  /* this should be done by the monitor in future */
  if( mil_hw_scope_type_get( p_api_dev ) != API_MILSCOPE_TYPE_NONE );
    p_ASL_Lca_IoRegs->ul_GRAM_TSlotCtrlReg = BSWAP32(0x00000008L);


  return 0;
}



/*! \brief This function installs the TSW interrupt handlers. */
/*!
    This function installs the ISR required for TSW operation.

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_tsw_isr_install(  TY_API_DEV * p_api_dev  )
{
  VOID(*old_lisr) (INT);
  STATUS status_int;
  L_WORD aul_VectorTable[MAX_BIU];
  BYTE i;


  /* Create HISR Task */
  /* see : mil_hw_tsw_isr_check_stack */
  p_api_dev->vp_HISRStackOverflowBuffer = MALLOC(MIL_HW_AYX_HISR_STACK_OVERFLOW);
  memset(p_api_dev->vp_HISRStackOverflowBuffer, 0xCE, MIL_HW_AYX_HISR_STACK_OVERFLOW );

  p_api_dev->vp_HISRStack               = MALLOC(MIL_HW_AYX_HISR_STACK_SIZE);
  memset(p_api_dev->vp_HISRStack, 5, MIL_HW_AYX_HISR_STACK_SIZE );
  status_int = NU_Create_HISR(&p_api_dev->hisr, "AIM_HISR", API_HISR_Interrupt, 2, p_api_dev->vp_HISRStack, MIL_HW_AYX_HISR_STACK_SIZE);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_HISR_Interrupt_Service ok");
  else
    PRINTF0("\n\r API_HISR_Interrupt_Service error \n\r");

  if( ( (p_api_dev->BoardType &0x0000F000L) == 0x00003000) ||  /* 3910 */
      ( (p_api_dev->BoardType &0x0000F000L) == 0x00007000) ||
      ( (p_api_dev->BoardType &0xF0000000L) == 0x30000000) ||
      ( (p_api_dev->BoardType &0xF0000000L) == 0x70000000) )
  {
    /* Create HISR39 Task */
    
    /* see : mil_hw_tsw_isr_check_stack */
    p_api_dev->vp_HISRStack39OverflowBuffer = MALLOC(MIL_HW_AYX_HISR_STACK_OVERFLOW);
    memset(p_api_dev->vp_HISRStack39OverflowBuffer, 0xCE, MIL_HW_AYX_HISR_STACK_OVERFLOW );
    
    
    p_api_dev->vp_HISRStack39 = MALLOC(MIL_HW_AYX_HISR_STACK_SIZE);
    memset(p_api_dev->vp_HISRStack39, 5, MIL_HW_AYX_HISR_STACK_SIZE );
    status_int = NU_Create_HISR(&p_api_dev->hisr39, "AIM_HISR39", API39_HISR_Interrupt, 2, p_api_dev->vp_HISRStack39, MIL_HW_AYX_HISR_STACK_SIZE);


    if(status_int == NU_SUCCESS)
      PRINTF0("\n\r API_HISR39_Interrupt_Service ok");
    else
      PRINTF0("\n\r API_HISR39_Interrupt_Service error \n\r");
  }

  for( i=0;i<MAX_BIU; i++ )
    aul_VectorTable[i] = 0xFFFFFFFF;

  switch(p_api_dev->uc_DeviceType)
  {
    case TYPE_APX1553_1:
    case TYPE_ACX1553_1:
    case TYPE_AVX1553_1:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      break;

    case TYPE_APX1553_2:
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_ACX1553_2:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX1553_2:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      break;
    case TYPE_APX1553_4:
    case TYPE_ACX1553_4:
    case TYPE_AVX1553_4:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU3_VECTOR;
      aul_VectorTable[BIU_4 - 1] = BIU4_VECTOR;
      break;

    case TYPE_ACX1553_2_TWO_PBIS:
    case TYPE_AVX1553_2_TWO_PBIS:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU5_VECTOR;
      break;

    case TYPE_ACX1553_4_TWO_PBIS:
    case TYPE_ACX1553_4_DS_TWO_PBIS:   
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX1553_4_TWO_PBIS:
    case TYPE_AVX1553_4_DS_TWO_PBIS:   
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU5_VECTOR;
      aul_VectorTable[BIU_4 - 1] = BIU6_VECTOR;
      break;

    case TYPE_ACX1553_8:
    case TYPE_AVX1553_8:
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU3_VECTOR;
      aul_VectorTable[BIU_4 - 1] = BIU4_VECTOR;
      aul_VectorTable[BIU_5 - 1] = BIU5_VECTOR;
      aul_VectorTable[BIU_6 - 1] = BIU6_VECTOR;
      aul_VectorTable[BIU_7 - 1] = BIU7_VECTOR;
      aul_VectorTable[BIU_8 - 1] = BIU8_VECTOR;
      break;

    case TYPE_ACX_EFA_1_TWO_PBIS:      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_1_TWO_PBIS:      
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_1_TWO_PBIS:    
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU5_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU6_VECTOR;
      break;

    case TYPE_ACX_EFA_2_TWO_PBIS:      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_2_TWO_PBIS:      
    case TYPE_AVX_EFAXp_2_TWO_PBIS:   
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU5_VECTOR;
      aul_VectorTable[BIU_4 - 1] = BIU6_VECTOR;
      break;

    case TYPE_ACX_EFA_4_TWO_PBIS:      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:   
    case TYPE_AVX_EFA_4_TWO_PBIS:      
    case TYPE_AVX_EFAXp_4_TWO_PBIS:   
      aul_VectorTable[BIU_1 - 1] = BIU1_VECTOR;
      aul_VectorTable[BIU_2 - 1] = BIU2_VECTOR;
      aul_VectorTable[BIU_3 - 1] = BIU3_VECTOR;
      aul_VectorTable[BIU_4 - 1] = BIU4_VECTOR;
      aul_VectorTable[BIU_5 - 1] = BIU5_VECTOR;
      aul_VectorTable[BIU_6 - 1] = BIU6_VECTOR;
      break;
    default:
      break;
  }

  IntFlags.flags.bip1 = 0;
  IntFlags.flags.bip2 = 0;
  IntFlags.flags.bip3 = 0;
  IntFlags.flags.bip4 = 0;

  status_int = NU_Register_LISR(aul_VectorTable[BIU_1 - 1], API_ISR1_Interrupt, &old_lisr);

  /*  */
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_ISR1_Interrupt_Service ok");
  else
    PRINTF0("\n\r API_ISR1_Interrupt_Service error \n\r");

  IntFlags.flags.bip2 = 0;

  if( aul_VectorTable[BIU_2 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_2 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_2-1]) ? API39_ISR2_Interrupt : API_ISR2_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR2_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_2-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR2_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_2-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_3 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_3 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_3-1]) ? API39_ISR6_Interrupt : API_ISR3_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR3_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_3-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR3_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_3-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_4 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_4 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_4-1]) ? API39_ISR6_Interrupt : API_ISR4_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR4_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_4-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR4_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_4-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_5 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_5 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_5-1]) ? API39_ISR6_Interrupt : API_ISR5_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR5_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_5-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR5_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_5-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_6 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_6 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_6-1]) ? API39_ISR6_Interrupt : API_ISR6_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR6_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_6-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR6_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_6-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_7 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_7 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_7-1]) ? API39_ISR6_Interrupt : API_ISR7_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR7_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_7-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR7_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_7-1]) ? "39" : "");
  }

  if( aul_VectorTable[BIU_8 - 1] != 0xFFFFFFFF )
  {
    status_int = NU_Register_LISR(aul_VectorTable[BIU_8 - 1], (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_8-1]) ? API39_ISR6_Interrupt : API_ISR8_Interrupt, &old_lisr);
    if(status_int == NU_SUCCESS)
      PRINTF1("\n\r API%s_ISR8_Interrupt_Service ok", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_8-1]) ? "39" : "");
    else
      PRINTF1("\n\r API%s_ISR8_Interrupt_Service error", (API_TYPE_3910 == px_api_dev->auc_StreamType[BIU_8-1]) ? "39" : "");
  }

  
  status_int = NU_Register_LISR(RELTI_VECTOR, API_RTI1_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_RTI_Interrupt_Service ok");
  else
    PRINTF0("\n\r API_RTI_Interrupt_Service error \n\r");

  p_api_dev->ul_MilScopeRunning = 0;
  IntFlags.flags.milscope = 0;


  /* see : mil_hw_tsw_isr_check_stack */
  p_api_dev->vp_HISRStackMscopeOverflowBuffer = MALLOC(MIL_HW_AYX_HISR_STACK_OVERFLOW);
  memset(p_api_dev->vp_HISRStackMscopeOverflowBuffer, 0xCE, MIL_HW_AYX_HISR_STACK_OVERFLOW );


  p_api_dev->vp_HISRStackMscope = MALLOC(MIL_HW_AYX_HISR_STACK_SIZE);
  memset(p_api_dev->vp_HISRStackMscope, 0, MIL_HW_AYX_HISR_STACK_SIZE );
  status_int = NU_Create_HISR(&p_api_dev->milscope_hisr, "MSCOPEIR", API_MSCOPE_HISR, 2, p_api_dev->vp_HISRStackMscope, MIL_HW_AYX_HISR_STACK_SIZE);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_MSCOPE_HISR_Service ok");
  else
    PRINTF0("\n\r API_MSCOPE_HISR_Service error \n\r");

  status_int = NU_Register_LISR(MSCOPE_VECTOR, API_MSCOPE_Interrupt, &old_lisr);

  /*  */
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_MSCOPE_Interrupt_Service ok");
  else
    PRINTF0("\n\r API_MSCOPE_Interrupt_Service error \n\r");


  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r API_DMA0_Interrupt_Service ok");
  else
    PRINTF0("\n\r API_DMA0_Interrupt_Service error \n\r");

  PRINTF0("\r\n");



  p_ASL_HOST_IntMaskReg->all = ulSaveDriverInterruptSetup; /* Restore Interrupt settings */

  if(status_int == NU_SUCCESS)
    return 0;
  else
    return 1;
}


/*! \brief Check if a HISR stack overflow occured */
/*!
    This functions checks a safety memory pool allocated before
    each HISR stack. If the pattern in the safety pool is changed
    the HISR stack might be overflown.

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 if no overflow is detected
*/
BYTE mil_hw_tsw_isr_check_stack( TY_API_DEV * p_api_dev )
{
  L_WORD i = 0;
  BYTE   ucStackOverflowHISR         = 0;
  BYTE   ucStackOverflowHISR3910     = 0;
  BYTE   ucStackOverflowHISRMilScope = 0;



  if( p_api_dev->vp_HISRStackOverflowBuffer != NULL )
  {
    for( i=0; i<(MIL_HW_AYX_HISR_STACK_OVERFLOW/4); i++ )
    {
      if( p_api_dev->vp_HISRStackOverflowBuffer[i] != 0xCECECECE )
      {
        if( ucStackOverflowHISR == 0 )
        {
          PRINTF0("Error : Possible p_api_dev->vp_HISRStack stack overflow.\r\n");
          ucStackOverflowHISR = 1;
          break;
        }
      }
    }
  }


  
  if( p_api_dev->vp_HISRStack39OverflowBuffer != NULL )
  {
    for( i=0; i<(MIL_HW_AYX_HISR_STACK_OVERFLOW/4); i++ )
    {
      if( p_api_dev->vp_HISRStack39OverflowBuffer[i] != 0xCECECECE )
      {
        if( ucStackOverflowHISR3910 == 0 )
        {
          PRINTF0("Error : Possible p_api_dev->vp_HISRStack39 stack overflow.\r\n");
          ucStackOverflowHISR3910 = 1;
          break;
        }
      }
    }
  }

  if( p_api_dev->vp_HISRStackMscopeOverflowBuffer != NULL )
  {
    for( i=0; i<(MIL_HW_AYX_HISR_STACK_OVERFLOW/4); i++ )
    {
      if( p_api_dev->vp_HISRStackMscopeOverflowBuffer[i] != 0xCECECECE )
      {
        if( ucStackOverflowHISRMilScope == 0 )
        {
          PRINTF0("Error : Possible p_api_dev->vp_HISRStackMscope stack overflow.\r\n");
          ucStackOverflowHISRMilScope = 1;
          break;
        }
      }
    }
  }



  /* Return 0 => No overflow detected */
  /* Return 1 => overflow detected    */
  return ucStackOverflowHISR | ucStackOverflowHISR3910 | ucStackOverflowHISRMilScope;

}



/*! \brief This function removes the TSW interrupt handlers. */
/*!
    This function removes the ISR required for TSW operation.

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_tsw_isr_remove(  TY_API_DEV * p_api_dev )
{
  ulSaveDriverInterruptSetup = p_ASL_HOST_IntMaskReg->all; /* Backup Interrupt settings */



  /* -- Free LS ISR --- */

  if( p_api_dev->vp_HISRStack != NULL )
  {
    NU_Delete_HISR( &p_api_dev->hisr );

    FREE( p_api_dev->vp_HISRStack );
    p_api_dev->vp_HISRStack = NULL;
  }


  if( p_api_dev->vp_HISRStackOverflowBuffer != NULL )
  {
    FREE( p_api_dev->vp_HISRStackOverflowBuffer );
    p_api_dev->vp_HISRStackOverflowBuffer = NULL;
  }


  /* -- Free HS ISR --- */

  if( p_api_dev->vp_HISRStack39 != NULL )
  {
    NU_Delete_HISR( &p_api_dev->hisr39 );

    FREE( p_api_dev->vp_HISRStack39 );
    p_api_dev->vp_HISRStack39 = NULL;
  }

  if( p_api_dev->vp_HISRStack39OverflowBuffer != NULL )
  {
    FREE( p_api_dev->vp_HISRStack39OverflowBuffer );
    p_api_dev->vp_HISRStack39OverflowBuffer = NULL;
  }

  



  /* -- Free MilScope ISR --- */


  if( p_api_dev->vp_HISRStackMscope != NULL )
  {
    NU_Delete_HISR( &p_api_dev->milscope_hisr );

    FREE( p_api_dev->vp_HISRStackMscope );
    p_api_dev->vp_HISRStackMscope = NULL;
  }

  if( p_api_dev->vp_HISRStackMscopeOverflowBuffer != NULL )
  {
    FREE( p_api_dev->vp_HISRStackMscopeOverflowBuffer );
    p_api_dev->vp_HISRStackMscopeOverflowBuffer = NULL;
  }

  return 0;
}


/*! \brief This function installes the interrup handlser required to boot the BIU */
/*!
    This function installs the ISR required for BIU boot.

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_boot_isr(void)
{
  VOID(*old_lisr) (INT);
  STATUS status_int;



  /* This interrupt hanlders are required for the BIU boot process */
  status_int = NU_Register_LISR(BIU1_VECTOR, BIU1_CHN1_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU1_CHN1_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU1_CHN1_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU2_VECTOR, BIU2_CHN1_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU2_CHN1_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU2_CHN1_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU3_VECTOR, BIU1_CHN2_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU1_CHN2_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU1_CHN2_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU4_VECTOR, BIU2_CHN2_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU2_CHN2_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU2_CHN2_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU5_VECTOR, BIU3_CHN1_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU3_CHN1_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU3_CHN1_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU6_VECTOR, BIU4_CHN1_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU4_CHN1_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU4_CHN1_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU7_VECTOR, BIU3_CHN2_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU3_CHN2_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU3_CHN2_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(BIU8_VECTOR, BIU4_CHN2_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r BIU4_CHN2_Interrupt_Service ok");
  else
    PRINTF0("\n\r BIU4_CHN2_Interrupt_Service error \n\r");

  status_int = NU_Register_LISR(HOST_VECTOR, HOST_Interrupt, &old_lisr);
  if(status_int == NU_SUCCESS)
    PRINTF0("\n\r HOST_Interrupt_Service ok");
  else
    PRINTF0("\n\r HOST_Interrupt_Service error \n\r");


  return 0;
}

/*! \brief This function reboots the BIUs and the IO LCAs */
/*!
    Reboot BIU and IO-LCA on plattforms where the BIU boot is controlled by the ASP

    Required on:

    1. AyX -> Boot BIUs and IO-LCAs from the global monitor sectors.

    2. AyI -> Boot BIUs and IO-LCAs from the sectors in p_api_dev

    \param  p_api_dev  TY_API_DEV*      The global variable container.
    
    \return BYTE  The return value is 0 on success or non zero on failure.
*/
BYTE mil_hw_reboot_biu( TY_API_DEV * p_api_dev )
{
 

  IntFlags.flags.bip1 = 0;
  IntFlags.flags.bip2 = 0;
  IntFlags.flags.bip3 = 0;
  IntFlags.flags.bip4 = 0;
  Boot_All_BIUs();
  IntFlags.flags.bip1 = 0;
  IntFlags.flags.bip2 = 0;
  IntFlags.flags.bip3 = 0;
  IntFlags.flags.bip4 = 0;


  px_api_dev->biu1_present = biu1_present;
  px_api_dev->biu2_present = biu2_present;
  px_api_dev->biu3_present = biu3_present;
  px_api_dev->biu4_present = biu4_present;

  return TRUE;
}


void BIU1_CHN1_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU1_CHN1_Interrupt raised");
  IntFlags.flags.bip1 = 1; /* Set BIP1 Interrupt Flag to '1' */
}

void BIU2_CHN1_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU2_CHN1_Interrupt raised");
  IntFlags.flags.bip2 = 1; /* Set BIP1 Interrupt Flag to '2' */
}

void BIU3_CHN1_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU3_CHN1_Interrupt raised");
  IntFlags.flags.bip3 = 1; /* Set BIP1 Interrupt Flag to '1' */
}

void BIU4_CHN1_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU4_CHN1_Interrupt raised");
  IntFlags.flags.bip4 = 1; /* Set BIP1 Interrupt Flag to '2' */
}

void BIU1_CHN2_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU1_CHN2_Interrupt raised");
  IntFlags.flags.bip1 = 1;
}

void BIU2_CHN2_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU2_CHN2_Interrupt raised");
  IntFlags.flags.bip2 = 1;
}

void BIU3_CHN2_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU3_CHN2_Interrupt raised");
  IntFlags.flags.bip3 = 1;
}

void BIU4_CHN2_Interrupt(INT vector_number)
{
  PRINTF0("\n\r BIU4_CHN2_Interrupt raised");
  IntFlags.flags.bip4 = 1;
}

void HOST_Interrupt(INT vector_number)
{
  PRINTF0("\n\r HOST_Interrupt raised");
}

/* Set board type value in NOVRAM */
void mil_hw_novram_set_boardtype( TY_API_DEV *p_api_dev, AiUInt32 ulBoardType )
{
  TCP_Std_NOVRam_write( NVR_BOARDTYPE_A, ulBoardType );
}



/* If value is not equal to zero set the bootsector (if available) to the specified value */
void mil_hw_novram_set_bootsectors( TY_API_DEV * p_api_dev, 
                                    AiUInt32     ulLca1Sector, 
                                    AiUInt32     ulLca2Sector, 
                                    AiUInt32     ulBiu1Sector,
                                    AiUInt32     ulBiu2Sector,
                                    AiUInt32     ulBiu3Sector,
                                    AiUInt32     ulBiu4Sector, 
                                    AiUInt32     ulBiu1Freq,
                                    AiUInt32     ulBiu2Freq,
                                    AiUInt32     ulBiu3Freq,
                                    AiUInt32     ulBiu4Freq )
{
    if (ulBiu1Sector) TCP_Std_NOVRam_write( NVR_BIU1BOOTSEC_A,  ulBiu1Sector );
    if (ulBiu2Sector) TCP_Std_NOVRam_write( NVR_BIU2BOOTSEC_A,  ulBiu2Sector );
    if (ulBiu3Sector) TCP_Std_NOVRam_write( NVR_BIU3BOOTSEC_A,  ulBiu3Sector );
    if (ulBiu4Sector) TCP_Std_NOVRam_write( NVR_BIU4BOOTSEC_A,  ulBiu4Sector );
    if (ulLca1Sector) TCP_Std_NOVRam_write( NVR_LCABOOTSEC_A,   ulLca1Sector );
    if (ulLca2Sector) TCP_Std_NOVRam_write( NVR_LCABOOTSEC2_A,  ulLca2Sector );


    TCP_Std_NOVRam_write( NVR_BIP1CLK_A,  ulBiu1Freq );
    TCP_Std_NOVRam_write( NVR_BIP2CLK_A,  ulBiu2Freq );
    TCP_Std_NOVRam_write( NVR_BIP3CLK_A,  ulBiu3Freq );
    TCP_Std_NOVRam_write( NVR_BIP4CLK_A,  ulBiu4Freq );
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
    /* -- init boot interrupts --- */

    /* 
        Disable interrupts
        Install BIU-Boot interrupt handlers
    */
    mil_hw_boot_isr();


    /* 
       -- Reboot loop ---
       If api_drv returns with 0 the system is re-initialized.
       This is used for the temporary EFA/EFEx switching.
       This is also used for the ApiCmdUtil reboot command.
       The reboot is commanded by setting p_api_dev->repeat to API_OFF
    */
    mil_hw_initialize_memory( p_api_dev );


    /* -- Initialize the p_api_dev values --- */

    /* The values are initialized from global monitor values
       on AyI and AyX and from the HW-Info in the shared RAM on AMC-ASP.

      The global monitor values are read from the NOVRAM within enter_hw during the bootup (AyI, AyX)
      The HW-Info values are read with v_GetBoardHardwareInfo during the bootup            (AMC-ASP)

      We must not re-read the NOVRAM values here. Because the global 
      variables may differ from the NOVRAM in case of an temporary 
      EFA-EFEx switch.

    */
    mil_hw_initialize_api_dev( p_api_dev );

    /* -- Set stream type and number of streams according to BoardType --- */
    api_main_initialize_stream_type( p_api_dev );

     /* -- Initialize memory layout --- */

    api_main_setup_memory_layout( p_api_dev );


    /* -- Boot the BIU and IO-LCA components --- */
    /* This does overwrite our interrupt settings */
    mil_hw_reboot_biu(p_api_dev);


    /* -- Install TSW interrupt vectors --- */
    /* Booting the BIU required different interrupt vectors 
       so we have to reinstall the TSW vector. 
    */
    mil_hw_tsw_isr_install( p_api_dev );

    /* -- arbitration adjustments --- */
    
    /* Give the MilScope a priority boost */

    mil_hw_adjust_arbitration( p_api_dev );

    /* This values might depend on the BIU control block */

    mil_hw_init_irig_capabilities(     p_api_dev );
    mil_hw_init_coupling_capabilities( p_api_dev );


    api_main_init_and_reset( p_api_dev );




    /* Last call in initialization / api_opr can be called now */
    p_api_dev->repeat = API_ON;
    mil_hw_signal_asp_running( p_api_dev );

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
    /* -- uninstall tsw interrupts --- */
    mil_hw_tsw_isr_remove( p_api_dev );

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

  return TRUE;
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
  TY_HW_INFORMATION * pHwInfo = NULL;

  p_ASL_CtrlReg->bits.board_ready = 1;

  pHwInfo = (TY_HW_INFORMATION*)p_api_dev->SharedRAMBase;

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
  TY_HW_INFORMATION * pHwInfo = NULL;

  p_ASL_CtrlReg->bits.board_ready = 0;

  pHwInfo = (TY_HW_INFORMATION*)p_api_dev->SharedRAMBase;

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
  return TRUE;
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
  
  BoardType   = ulBoardType;

  if( ucPermanent == TRUE )
  {
    /* -- store permanent setting in NOVRAM --- */
    TCP_Std_NOVRam_write( NVR_BOARDTYPE_A, ulBoardType );
  }



  return TRUE;
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
  biu1BootSec = ulBiu1BootSec;
  biu2BootSec = ulBiu2BootSec;

  lcaBootSec  = ulLcaBootSector;

  BIP1Clk = ulBiu1Freq;			
  BIP2Clk = ulBiu2Freq;			


  if( ucPermanent == TRUE )
  {
    /* -- store permanent setting in NOVRAM --- */
    TCP_Std_NOVRam_write( NVR_BIU1BOOTSEC_A,  biu1BootSec );
    TCP_Std_NOVRam_write( NVR_BIU2BOOTSEC_A,  biu2BootSec );
    TCP_Std_NOVRam_write( NVR_LCABOOTSEC_A,   lcaBootSec );


    TCP_Std_NOVRam_write( NVR_BIP1CLK_A,  BIP1Clk );
    TCP_Std_NOVRam_write( NVR_BIP2CLK_A,  BIP2Clk );
  }


  return TRUE;
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
  biu3BootSec = ulBiu1BootSec;
  biu4BootSec = ulBiu2BootSec;

  lcaBootSec2  = ulLcaBootSector;

  BIP3Clk = ulBiu1Freq;			
  BIP4Clk = ulBiu2Freq;			


  if( ucPermanent == TRUE )
  {
    /* -- store permanent setting in NOVRAM --- */
    TCP_Std_NOVRam_write( NVR_BIU3BOOTSEC_A,  biu3BootSec );
    TCP_Std_NOVRam_write( NVR_BIU4BOOTSEC_A,  biu4BootSec );
    TCP_Std_NOVRam_write( NVR_LCABOOTSEC2_A,  lcaBootSec2 );


    TCP_Std_NOVRam_write( NVR_BIP3CLK_A,  BIP3Clk );
    TCP_Std_NOVRam_write( NVR_BIP4CLK_A,  BIP4Clk );
  }


  return TRUE;
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
                              AiUInt32   * pulHsBiuFreq )
{
  switch( ulProtocol )
  { 
    case 0x30 : 
      
      /* 3910 */
      if(pulLsBiuBootSec) *pulLsBiuBootSec = 41;
      if(pulHsBiuBootSec) *pulHsBiuBootSec = 42;
      if(pulLcaBootSec)   *pulLcaBootSec   = 52;
      if(pulLsBiuFreq)    *pulLsBiuFreq    = 400;
      if(pulHsBiuFreq)    *pulHsBiuFreq    = 600;
      break;

    case 0x70 :
      
      /* EFEx */
      if(pulLsBiuBootSec) *pulLsBiuBootSec = 41;
      if(pulHsBiuBootSec) *pulHsBiuBootSec = 43;
      if(pulLcaBootSec)   *pulLcaBootSec   = 52;
      if(pulLsBiuFreq)    *pulLsBiuFreq    = 400;
      if(pulHsBiuFreq)    *pulHsBiuFreq    = 666;
      break;
  }
  
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
  return TCP_Std_NOVRam_write(ulOffset, ulValue);
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
  *pulValue = TCP_Std_NOVRam_read( ulOffset /*nvByteAddr*/);

  return 0;
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


/*! \brief Finalize the NOVRAM write. */
/*!

    \param  p_api_dev          TY_API_DEV*      The global variable container.
    \return L_WORD The return value is 0 on success or non zero on failure.
*/
L_WORD mil_hw_novram_finalize( TY_API_DEV * p_api_dev )
{
  L_WORD ulRet           = 0;
  L_WORD ulErrorCount    = 0;
  BYTE   ucLcaIndex      = 0;
  BYTE   ucAvailableBius = 0;

  L_WORD ulClock     = 0;
  L_WORD ulReference = 0;
  L_WORD ulDivider   = 0;

  L_WORD aulClockOffset[]     = { NVR_LCA1CLK_A,    NVR_LCA2CLK_A,    NVR_LCA3CLK_A,    NVR_LCA4CLK_A    };
  L_WORD aulReferenceOffset[] = { NVR_LCA1CLKREF_A, NVR_LCA2CLKREF_A, NVR_LCA3CLKREF_A, NVR_LCA4CLKREF_A };


  /* If an LCA clock was changed it needs to be programmed with ClkGen_Prog */

  if (p_ASL_BoardtypeIDReg2->bits.BoardType == 0xCAFE0001)
    ucAvailableBius = 4;
  else
    ucAvailableBius = 2;

  for( ucLcaIndex = 1; ucLcaIndex<=ucAvailableBius; ucLcaIndex++ )
  {
    ulClock     = TCP_Std_NOVRam_read( aulClockOffset[    ucLcaIndex-1] );
    ulReference = TCP_Std_NOVRam_read( aulReferenceOffset[ucLcaIndex-1] );

    ulDivider   = (ucLcaIndex%2) ? lca1_clk_div : lca2_clk_div;


    if(    (ulClock      != 0) 
        && (ulReference  != 0) 
        && (ulDivider    != 0) )
    {
      ulRet = ClkGen_Prog( ucLcaIndex, ulReference, ulClock, ulDivider );

      if( ulRet == PASS )
      {
        ulErrorCount += 0;
        printf("BIU%d-LCA clock programmed passed : %6d [kHz] / %6d [kHz] / %d div\n\r", ucLcaIndex, ulClock, ulReference, ulDivider );
      }
      else 
      {
        ulErrorCount += 1;
        printf("BIU%d-LCA clock programming failed : %6d [kHz] / %6d [kHz] / %d div\n\r", ucLcaIndex, ulClock, ulReference, ulDivider );
      }
    }
  }


  if (ulErrorCount > 0)
      return API_ERR_INVALID_DEVICE_STATE;
  else
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
 BYTE biu;

 biu = (BYTE)p_api_dev->biu;
 p_api_dev->biu = 0;

 if( con & 0x01 )
   p_ASL_Lca_IoRegs->ul_CtrlReg &= BSWAP32(0xFFFFFFDFL);
 else
   p_ASL_Lca_IoRegs->ul_CtrlReg |= BSWAP32(0x00000020L);

 p_api_dev->biu = biu;

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
  L_WORD   ret_val  = 0;

  if( (day <= 366L) && (hour < 24) && (min < 60) && (sec < 60) )
  {

    TcpOut(TC_IEDAYHI, (day / 256) ); /* IRIG time encode day high */
    TcpOut(TC_IEDAYLO, (day % 256) ); /* IRIG time encode day low */
    TcpOut(TC_IEHOUR, (L_WORD) (hour) ); /* IRIG time encode hour */
    TcpOut(TC_IEMIN, (L_WORD) (min) ); /* IRIG time encode minute */
    TcpOut(TC_IESEC, (L_WORD) (sec) ); /* IRIG time encode seconds */

    /* Latch IRIG Encoder Data */
    TcpOut(TC_ICTRL, TC_I_ELATCH_B);
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
  AiUInt32 irig_status = 0;

  *irig_sync = API_IRIG_SYNC;
  *irig_src = API_IRIG_ONBOARD;

  irig_status = TcpIn(TC_ISTAT);
  if( BSWAP32(p_ASL_Lca_IoRegs->ul_CtrlReg) & 0x00000020 ) /* intern IRIG is set */
    *irig_src  = API_IRIG_ONBOARD;
  else
  {
    *irig_src = API_IRIG_EXTERN;
    if( 0 == (irig_status & 0x01) )
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
    
    \return ackfl      L_WORD             Return code.
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
    PMC_TTHIGH_Reg x_TTHigh;
    PMC_TTLOW_Reg x_TTLow;

    x_TTHigh.ul_All = BSWAP32(p_ASL_Lca_IoRegs->ul_TTHigh);
    x_TTLow.ul_All  = BSWAP32(p_ASL_Lca_IoRegs->ul_TTLow);

    *day  = x_TTHigh.Reg.day;
    *hour = x_TTHigh.Reg.hour;
    *min  = x_TTHigh.Reg.minutes;
    *sec  = x_TTHigh.Reg.sec;
    *usec = x_TTLow.Reg.usec;

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

  /* the program bits are defined in the location of ul_DiscreteOut in the SysCtrlBlk */

  ul_Temp = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOConfReg);
  ul_Temp &= 0xffffff00;
  ul_Temp |= ( (ul_DiscreteSetup) &0x000000ff);

  p_ASL_Lca_IoRegs->ul_GPIOConfReg = BSWAP32(ul_Temp);

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
  L_WORD ul_Tmp1;

  Mask  &= 0x000000ff;  /* only the lowest 8 bit are valid */
  Value &= 0x000000ff;  /* only the lowest 8 bit are valid */

  ul_Tmp1 = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg);
  ul_Tmp = (ul_Tmp1 &0x00ff0000) >> 16;

  ul_Tmp &= ~Mask;         /* reset all bits referenced in the mask */
  ul_Tmp |= (Value &Mask); /* setup all bits referenced in value together with mask */

  ul_Tmp = ul_Tmp << 16;
  ul_Tmp1 &= 0xff00ffff;
  ul_Tmp1 |= ul_Tmp;

  p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg = BSWAP32(ul_Tmp1);


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
    L_WORD ul_Temp = 0;

    L_WORD ul_Stat;
    L_WORD ul_StatOut;
    L_WORD ul_StatIn;
    L_WORD ul_Conf;

    ul_Conf     = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOConfReg) & 0x000000ff;
    ul_Stat     = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg);
    ul_StatOut  = (ul_Stat &0x00ff0000) >> 16;
    ul_StatIn   = (ul_Stat &0x000000ff);
    ul_StatOut &= ul_Conf; /* read output status */
    ul_StatIn  &= (ul_Conf ^ 0x000000ff); /* read input status */
    ul_Temp     = ul_StatOut | ul_StatIn; /* merge output and input together */

    *Value = ul_Temp & 0x000000ff;

    return 0;
}









/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                 HW Layer public TSW system information                    *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
 









/* converts a zero based stream to a zero based logical biu */
/*


                   |----- input ----|- output --|
    Device         | stream | LS/HS | biu | mem |
    --------------------------------------------|
    1553 Boards    |    0   | ls    |  0  |  0  |
    one memory     |    1   | ls    |  1  |  0  |
                   |    2   | ls    |  2  |  0  |
                   |    3   | ls    |  3  |  0  |
                   |    4   | ls    |  4  |  0  |
                   |    5   | ls    |  5  |  0  |
                   |    6   | ls    |  6  |  0  |
                   |    7   | ls    |  7  |  0  |
    --------------------------------------------|
    ACX1553-2 6U   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  1  |
    --------------------------------------------|
    ACX1553-4 6U   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  0  |
                   |    2   | ls    |  2  |  1  |
                   |    3   | ls    |  3  |  1  |
    --------------------------------------------|
    ACX1553-8 6U   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  0  |
                   |    2   | ls    |  2  |  0  |
                   |    3   | ls    |  3  |  0  |
                   |    4   | ls    |  5  |  1  |
                   |    5   | ls    |  6  |  1  |
                   |    6   | ls    |  7  |  1  |
                   |    7   | ls    |  8  |  1  |
    --------------------------------------------|
    3910-1 Boards  |    0   | ls    |  0  |  0  |
                   |    0   | hs    |  1  |  0  |
    --------------------------------------------|
    3910-2 Boards  |    0   | ls    |  0  |  0  |
                   |    0   | hs    |  1  |  0  |
                   |    1   | ls    |  2  |  1  |
                   |    1   | hs    |  3  |  1  |
    --------------------------------------------|
    EFA-1 Boards   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  1  |
                   |    1   | hs    |  2  |  1  |
    --------------------------------------------|
    EFA-2 Boards   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  0  |
                   |    2   | ls    |  2  |  1  |
                   |    2   | hs    |  3  |  1  |
    --------------------------------------------|
    EFA-4 Boards   |    0   | ls    |  0  |  0  |
                   |    1   | ls    |  1  |  0  |
                   |    2   | ls    |  2  |  0  |
                   |    3   | ls    |  3  |  0  |
                   |    4   | ls    |  4  |  1  |
                   |    4   | hs    |  5  |  1  |
    --------------------------------------------|

*/
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
  short pbi       = 0;
  short logic_biu = 0;

  logic_biu = ulStream;
  pbi       = 0;

  switch(p_api_dev->uc_DeviceType)
  {

  case TYPE_ACX1553_2_TWO_PBIS:
  case TYPE_AVX1553_2_TWO_PBIS:
    if( ulStream >= 1 )
    {
      pbi = 1;
    }
    break;

  case TYPE_ACX1553_4_TWO_PBIS:
  case TYPE_AVX1553_4_TWO_PBIS:
  case TYPE_ACX1553_4_DS_TWO_PBIS:
  case TYPE_AVX1553_4_DS_TWO_PBIS:
    if( ulStream >= 2 )
    {
      pbi = 1;
    }
    break;

  case TYPE_ACX1553_8:
  case TYPE_AVX1553_8:
    if( ulStream >= 4 )
    {
      pbi = 1;
    }
    break;


  case TYPE_APX3910:
  case TYPE_APX3910Xp:
  case TYPE_ACX3910:
  case TYPE_ACX3910Xp:
  case TYPE_AVX3910:
  case TYPE_AVX3910Xp:
    if( ulStream == 0 )
    {
      if( ulHs == 0 )
        logic_biu = 0;
      else
        logic_biu = 1;
    }
    break;
  
  case TYPE_ACX3910_2:
  case TYPE_ACX3910Xp_2:
  case TYPE_AVX3910_2:
  case TYPE_AVX3910Xp_2:
    if( ulStream == 0 )
    {
      if( ulHs == 0 )
        logic_biu = 0;
      else
        logic_biu = 1;
    }
    else if( ulStream == 1 )
    {
      if( ulHs == 0 )
        logic_biu = 2;
      else
        logic_biu = 3;

      pbi = 1;
    }
    break;

  case TYPE_ACX_EFA_1_TWO_PBIS:
  case TYPE_ACX_EFAXp_1_TWO_PBIS:
  case TYPE_ACX_EFA_1_DS_TWO_PBIS:
  case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
  case TYPE_AVX_EFA_1_TWO_PBIS:
  case TYPE_AVX_EFAXp_1_TWO_PBIS:
  case TYPE_AVX_EFA_1_DS_TWO_PBIS:
  case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
    if( ulStream == 1 )
    {
      if( ulHs == 0 )
        logic_biu = 1;
      else
        logic_biu = 2;

      pbi = 1;
    }
    break;

  case TYPE_ACX_EFA_2_TWO_PBIS:
  case TYPE_ACX_EFAXp_2_TWO_PBIS:
  case TYPE_ACX_EFA_2_DS_TWO_PBIS:
  case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
  case TYPE_AVX_EFA_2_TWO_PBIS:
  case TYPE_AVX_EFAXp_2_TWO_PBIS:
  case TYPE_AVX_EFA_2_DS_TWO_PBIS:
  case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
    if( ulStream == 2 )
    {
      if( ulHs == 0 )
        logic_biu = 2;
      else
        logic_biu = 3;

      pbi = 1;
    }
    break;

  case TYPE_ACX_EFA_4_TWO_PBIS:
  case TYPE_ACX_EFAXp_4_TWO_PBIS:
  case TYPE_AVX_EFA_4_TWO_PBIS:
  case TYPE_AVX_EFAXp_4_TWO_PBIS:
    if( ulStream == 4 )
    {
      if( ulHs == 0 )
        logic_biu = 4;
      else
        logic_biu = 5;

      pbi = 1;
    }
    break;

  default:
      break;
  }

  if( pwLogicalBiu ) *pwLogicalBiu = logic_biu;
  if( pwMemBank    ) *pwMemBank    = pbi;


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
  AiUInt32 ulRetVal        = 0;

  if( pulGlobalRamStartOffset == NULL )
    return 1;

  *pulGlobalRamStartOffset = 0x00000000;

  switch(p_api_dev->uc_DeviceType)
  {
    case TYPE_ACX1553_8:            
    case TYPE_AVX1553_8: 
    case TYPE_AVX_EFA_4_TWO_PBIS:     
    case TYPE_AVX_EFAXp_4_TWO_PBIS:   
    case TYPE_ACX_EFA_4_TWO_PBIS:     
    case TYPE_ACX_EFAXp_4_TWO_PBIS:   
      if( ulBiu >= 4 )
        *pulGlobalRamStartOffset     = 0x02000000;
      break;
    case TYPE_ACX1553_4_TWO_PBIS:
    case TYPE_ACX1553_4_DS_TWO_PBIS:  
    case TYPE_AVX1553_4_TWO_PBIS:
    case TYPE_AVX1553_4_DS_TWO_PBIS:  
    case TYPE_ACX_EFA_2_TWO_PBIS:     
    case TYPE_ACX_EFAXp_2_TWO_PBIS:   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:  
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:     
    case TYPE_AVX_EFAXp_2_TWO_PBIS:   
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:  
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
      if( ulBiu >= 2 )
        *pulGlobalRamStartOffset     = 0x02000000;
      break;
    case TYPE_ACX1553_2_TWO_PBIS:   
    case TYPE_AVX1553_2_TWO_PBIS:   
    case TYPE_ACX_EFA_1_TWO_PBIS:     
    case TYPE_ACX_EFAXp_1_TWO_PBIS:   
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:  
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:     
    case TYPE_AVX_EFAXp_1_TWO_PBIS:   
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:  
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
      if( ulBiu >= 1 )
        *pulGlobalRamStartOffset     = 0x02000000;
      break;

    default:
      ulRetVal = 1;
      break;
  }


  return ulRetVal;

}

/*! \brief Get some information whether the amplitude can be changed for this stream */
/*! 
    \param  p_api_dev               TY_API_DEV*    The global variable container.
    \param  ulBiu                   AiUInt32       Input variable for the logical BIU.
    \param  pulCanChangeAmplitude   AiUInt32*      Output variable showing if the amplitude can be modified.

    \return AiUInt32  The return value is 0 on success or non zero on failure.
*/
AiUInt32 mil_hw_can_change_amplitude(TY_API_DEV *p_api_dev, AiUInt32 ulBiu, AiUInt32 *pulCanChangeAmplitude, AiUInt32 *pulPacked)
{
  if (pulCanChangeAmplitude == NULL)
    return 1;

  /* Some very old boards do not have the boardsubtype value set. Even worse they might contain random data.
     So far all transceivers used on AyX CMAC and HOLD have a variable amplitude. Thus, we do not check.

     *pulCanChangeAmplitude =  mil_novram_interpret_subtype_amplitude_variable(p_api_dev->BoardSubType);
  */
  *pulCanChangeAmplitude = AiTrue;

  /* Boards with HOLT transceivers have limitations. A and B can only be modified together */
  /* Used Transceivers: 1,2,4,8,9 */
  *pulPacked             =  mil_novram_interpret_subtype_amplitude_packed(p_api_dev->BoardSubType);

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
    /* Some AyX PBIs have HOLD HI1590 transceivers but switching to high resolution is not supported by the FW */
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
  AiUInt32 ulIsMultiChannel;

  ulIsMultiChannel = ul_StreamIsMultiChannel(p_api_dev, p_api_dev->biu);

  /* default entries */
  p_api_dev->x_CouplingCapabilities.b.b_Packed      = AiFalse;
  p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiTrue;
  p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
  p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiTrue;
  p_api_dev->x_CouplingCapabilities.b.b_Transformer = AiTrue;
  p_api_dev->x_CouplingCapabilities.b.b_Network     = AiTrue;

 
  /* adaptations for DBTE PBI */
  /* check for DBTE PBI
   * This PBI can be identified by checking the upper 16 bits of the version register for the magic number 0x1F */
  if( ( (MEM(GLBRAM_BP + BIU_VERSION_O) &0x00ff0000) >> 16) == 0x1F)
  {
    p_api_dev->x_CouplingCapabilities.b.b_Direct      = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Network     = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_Isolated    = AiFalse;
    p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap = AiTrue;
  }


  /* Set b_Packed if appropriate */
  /* AyX multichannel boards have packed coupling and older boards
   * may not have the correct sub type set in the novram */
  if( ulIsMultiChannel )
  {
    p_api_dev->x_CouplingCapabilities.b.b_Packed    = AiTrue;
  }
  /* Check board sub type for packed coupling check.
   * This is the recommended way to check for packed coupling */
  if( (p_api_dev->BoardSubType & 0xF0) == 0x20 )
    p_api_dev->x_CouplingCapabilities.b.b_Packed    = AiTrue;



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

  if( pulDiscreteCount == NULL )
    return 1;


  *pulDiscreteCount = 8; /* default value */

  if( pulDiscreteCount == NULL )
  {
    /* all discrete channels are read/write */
    *pulDiscreteConfig = 0;
    for (i=0; i < *pulDiscreteCount; i++)
      *pulDiscreteConfig += 1<<i;
  }

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
  if( pxIRCapabilities == NULL )
    return 1;

  (*pxIRCapabilities).ul_All = 0;
  return 0;
}


/*! \brief Returns the type of the firmware

\param p_api_dev pointer to target administration structure
\return returns The type of the FW */
enum MIL_FW mil_hw_get_fw_type(TY_API_DEV * p_api_dev, short ulBiu)
{
    if( ul_StreamIsHS(p_api_dev, ulBiu) )
    {
        // HS Stream
        return MIL_FW_STG3838_XSCALE;
    }
    else
    {
        if( ul_StreamIsMultiChannel(p_api_dev, ulBiu) )
            return MIL_FW_MCH_XSCALE;
        else
            return MIL_FW_STD_XSCALE;
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
  if( pulIsHsRedundant == NULL )
    return 1;

  *pulIsHsRedundant = 1;

  switch( p_api_dev->BoardPartNo )
  {
  case 0x13148: /* APX3910-EL*/
  case 0x131B0: /* APE3910-1 */
      *pulIsHsRedundant = 0;
  }

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
    if( ucHs )
    {
        /* if 4 MByte or less -- not yet used in memory layout */
        return 0x00200000L; /* 2 MB */
    }
    else
    {
        return 0x00080000L; /* 0.5 MB */
    }
}


/*! \brief Update the system state */
void mil_hw_update_system_state(  TY_API_DEV *p_api_dev, L_WORD ulState )
{
    if( ulState != 0 )
    {
        /* Report state */
        p_api_dev->ulGlobalError = ulState;
    }
    else
    {
        /* Poll HW for system state changes */
        /* The function CheckFOFEalert is implemented in the Monitor
         * but is according to FSC completely untested. Uncomment the
         * function call when temperature test results are available. */
        /*
        if( CheckFOFEalert() == TRUE )
        {
            PRINTF0("OT Error\r\n");
            p_api_dev->ulGlobalError = API_ERR_FOFE_OT_SHUTDOWN;
        }
        */
    }
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
    volatile L_WORD *pul_GlobMem2 = (L_WORD*)p_api_dev->GlobalRAMBase2;


    /* Target Software */
    *ulTSW      = MIL_TSW_LEGACY_VERSION;
    *ulTSWBuild = MIL_TSW_LEGACY_BUILD;

    /* Board serial number */
    *ulSN = p_api_dev->BoardSerial;

    /* Monitor */
    *ulMonitor = MON_VERSION;

    /* read IO-LCA version                               */
    *ulLsIoLcaPbi1 = BSWAP32(pul_GlobMem[ 0x24 / 4 ]) & 0x0FFF0FFF;

    /* Read ASL FPGA of APX boards                       */
    *ulAslLca = p_ASL_Lca_IoRegs->ul_VersionReg;

    /* Firmware */
    *ulFwBiu1Biu2 = (BSWAP32(pul_GlobMem[ 0x8 / 4 ]) &0x0000ffff);


    /* Initialize and set values that are not available for all platforms */
    *ulPciLca      = 0;
    *ulHsIoLcaPbi1 = 0;
    *ulLsIoLcaPbi2 = 0;
    *ulHsIoLcaPbi2 = 0;
    *ulFwBiu3Biu4  = 0;

    switch(p_api_dev->uc_DeviceType )
    {
      case TYPE_APX1553_2:
      case TYPE_ACX1553_2:
      case TYPE_AVX1553_2:
          /* PCI LCA BIU2 */
          *ulFwBiu1Biu2 |= (BSWAP32(pul_GlobMem[ 0x10008 / 4 ]) &0x0000ffff) << 16;
          break;

      case TYPE_APX1553_4:
      case TYPE_APX3910:
      case TYPE_APX3910Xp:
      case TYPE_ACX1553_4:
      case TYPE_ACX3910:
      case TYPE_ACX3910Xp:
      case TYPE_AVX1553_4:
      case TYPE_AVX3910:
      case TYPE_AVX3910Xp:
          /* PCI LCA BIU2 */
          *ulFwBiu1Biu2 |= (BSWAP32(pul_GlobMem[ 0x10008 / 4 ]) & 0x0000ffff) << 16;
          *ulHsIoLcaPbi1 =  BSWAP32(pul_GlobMem[ 0x10024 / 4 ]  & 0x0FFF0FFF );
          break;

      case TYPE_ACX1553_4_TWO_PBIS:
      case TYPE_ACX1553_8:
      case TYPE_ACX3910_2:
      case TYPE_ACX3910Xp_2:
      case TYPE_ACX1553_4_DS_TWO_PBIS:
      case TYPE_ACX_EFA_2_TWO_PBIS:
      case TYPE_ACX_EFAXp_2_TWO_PBIS:
      case TYPE_ACX_EFA_2_DS_TWO_PBIS:
      case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
      case TYPE_ACX_EFA_4_TWO_PBIS:
      case TYPE_ACX_EFAXp_4_TWO_PBIS:
      case TYPE_AVX1553_4_TWO_PBIS:
      case TYPE_AVX1553_8:
      case TYPE_AVX3910_2:
      case TYPE_AVX3910Xp_2:
      case TYPE_AVX1553_4_DS_TWO_PBIS:
      case TYPE_AVX_EFA_2_TWO_PBIS:
      case TYPE_AVX_EFAXp_2_TWO_PBIS:
      case TYPE_AVX_EFA_2_DS_TWO_PBIS:
      case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
      case TYPE_AVX_EFA_4_TWO_PBIS:
      case TYPE_AVX_EFAXp_4_TWO_PBIS:
          /* PCI LCA BIU2 */
          *ulFwBiu1Biu2 |= (BSWAP32(pul_GlobMem[  0x10008 / 4 ]) & 0x0000ffff) << 16;
          *ulHsIoLcaPbi1 =  BSWAP32(pul_GlobMem[  0x10024 / 4 ]  & 0x0FFF0FFF);
          *ulLsIoLcaPbi2 =  BSWAP32(pul_GlobMem2[ 0x24    / 4 ]) & 0x0FFF0FFF;
          *ulHsIoLcaPbi2 =  BSWAP32(pul_GlobMem2[ 0x10024 / 4 ]  & 0x0FFF0FFF);
          *ulFwBiu3Biu4 = ((BSWAP32(pul_GlobMem2[ 0x10008 / 4 ]) & 0x0000ffff) << 16) |
                           (BSWAP32(pul_GlobMem2[ 0x8     / 4 ]) & 0x0000ffff);

        break;

      case TYPE_ACX_EFA_1_TWO_PBIS:
      case TYPE_ACX_EFAXp_1_TWO_PBIS:
      case TYPE_ACX_EFA_1_DS_TWO_PBIS:
      case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
      case TYPE_AVX_EFA_1_TWO_PBIS:
      case TYPE_AVX_EFA_1_DS_TWO_PBIS:
      case TYPE_AVX_EFAXp_1_TWO_PBIS:
      case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
          /* PCI LCA BIU2 */
          *ulFwBiu1Biu2 |=  (BSWAP32(pul_GlobMem[  0x10008 / 4 ]) & 0x0000ffff) << 16;
          *ulLsIoLcaPbi2 =   BSWAP32(pul_GlobMem2[ 0x24    / 4 ]) & 0x0FFF0FFF;
          *ulHsIoLcaPbi2 =   BSWAP32(pul_GlobMem2[ 0x10024 / 4 ]  & 0x0FFF0FFF );
          *ulFwBiu3Biu4  = ((BSWAP32(pul_GlobMem2[ 0x10008 / 4 ]) & 0x0000ffff) << 16) |
                            (BSWAP32(pul_GlobMem2[ 0x8     / 4 ]) & 0x0000ffff);
        break;

    }


    return 0;
}

L_WORD mil_hw_read_version_tcp(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD  version = TcpIn(TC_VERSION) & 0xff;

    pxVersionValue->ul_VersionType  = AI_TCP_VER;
    pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,4,0xF  ) );
    pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,0,0xF  ) );
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr      = 0;

    sprintf( pxVersionString->ac_Description, "TCP" );
    sprintf( pxVersionString->ac_FullVersion, "%d.%d", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer );

    return 0;
}


L_WORD mil_hw_read_version_lca(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{
    L_WORD  version = p_ASL_Lca_IoRegs->ul_VersionReg;

    pxVersionValue->ul_VersionType  = AI_FPGA_VER;
    pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(version,4,0xFF  ) );
    pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(version,0,0xF   ) );
    pxVersionValue->ul_PatchVersion = 0;
    pxVersionValue->ul_BuildNr      = 0;

    sprintf( pxVersionString->ac_Description, "ASL LCA" );
    sprintf( pxVersionString->ac_FullVersion, "%d.%d", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer );

    return 0;
}





L_WORD mil_hw_read_version_env(  TY_API_DEV *p_api_dev, TY_E_VERSION_ID eId, TY_API_VER_INFO_VALUES * pxVersionValue, TY_API_VER_INFO_STRINGS * pxVersionString )
{

    if( eId == AI_TARGET_OS_VER )
    {
        pxVersionValue->ul_VersionType  = AI_TARGET_OS_VER;
        pxVersionValue->ul_MajorVer     = PLUS_VERSION_COMP;
        pxVersionValue->ul_MinorVer     = 0;
        pxVersionValue->ul_PatchVersion = 0;
        pxVersionValue->ul_BuildNr      = 0;

        sprintf( pxVersionString->ac_Description, "Nucleus" );
        sprintf( pxVersionString->ac_FullVersion, "%d.%d",  pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer );

        return 0;
    }
    else if( eId == AI_MONITOR_VER )
    {
        pxVersionValue->ul_VersionType  = AI_MONITOR_VER;
        pxVersionValue->ul_MajorVer     = BCD2BIN_B( EXTRACT_BITS(MON_VERSION,8,0xFF  ) );
        pxVersionValue->ul_MinorVer     = BCD2BIN_B( EXTRACT_BITS(MON_VERSION,0,0xFF  ) );
        pxVersionValue->ul_PatchVersion = 0;
        pxVersionValue->ul_BuildNr      = 0;

        sprintf( pxVersionString->ac_Description, "Monitor" );
        sprintf( pxVersionString->ac_FullVersion, "%d.%d", pxVersionValue->ul_MajorVer, pxVersionValue->ul_MinorVer );

        return 0;
    }

    return API_ERR_INVALID_MODE;
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
        retval = mil_hw_read_version_tcp( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    case AI_FPGA_VER:
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
    case AI_BOOTSTRAP_VER:
    case AI_TARGET_OS_VER:
        retval = mil_hw_read_version_env( p_api_dev, eId, pxVersionValue, pxVersionString );
        break;

    case AI_MAIN_LCA_VER:
    case AI_PCI_LCA_VER:
    default:
        retval = API_ERR_INVALID_MODE;
    }


    if( retval == 0 )
        PRINTF3("mil_hw_read_version id=%d desc=%s ver=%s\r\n", eId, pxVersionString->ac_Description, pxVersionString->ac_FullVersion );



    return retval;
}





/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                    HW Layer public PCI Bus Master access                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

/* Read a long word 32 bit from the PCI bus address addr */
L_WORD mil_hw_pci_readlw( L_WORD ulPciAddress )
{
  L_WORD   ulPciBaseAddress    = (ulPciAddress & 0xFC000000); /* 64 MB Part of the address */
  L_WORD   ulOffset            = (ulPciAddress & 0x03FFFFFF); /* Offset within the 64MB ATU outbound 2 window */
  L_WORD * pVirtualAtuAddress  = NULL;


  /* configure the ATU outbound window 2 to address the right PCI memory bar */

  MEM(ATU_OMWTVR1_ADDR) = ulPciBaseAddress;


  /* create a pointer to the virtual 16 MB window of the ATU O2
     each access to this window will be redirected to the PCI address 
     assigned to the register ATU_OMWTVR1_ADDR. See above 
  */

  pVirtualAtuAddress = (L_WORD*)(ulOffset + ATU_OMWTVR1_VIRTUAL_ADDR );


  /* Write the value */

  return *pVirtualAtuAddress;
}

/* Write a long word 32 bit to the PCI bus address addr */
void mil_hw_pci_writelw( L_WORD ulPciAddress, L_WORD ulValue )
{
  L_WORD   ulPciBaseAddress    = (ulPciAddress & 0xFC000000); /* 16 MB Part of the address */
  L_WORD   ulOffset            = (ulPciAddress & 0x03FFFFFF); /* Offset within the 16MB ATU outbound 2 window */
  L_WORD * pVirtualAtuAddress  = NULL;


  /* configure the ATU outbound window 2 to address the right PCI memory bar */

  MEM(ATU_OMWTVR1_ADDR) = ulPciBaseAddress;


  /* create a pointer to the virtual 16 MB window of the ATU O2
     each access to this window will be redirected to the PCI address 
     assigned to the register ATU_OMWTVR1_ADDR. See above 
  */

  pVirtualAtuAddress = (L_WORD*)(ulOffset + ATU_OMWTVR1_VIRTUAL_ADDR );


  /* Write the value */

  *pVirtualAtuAddress = ulValue;
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
AiBoolean mil_hw_pxi_is_supported( TY_API_DEV* p_api_dev )
{
    /* We check the platform type here instead of the HW variant word      */
    /* The HW variant word might not be set on very old boards, who knows? */
    if( (p_api_dev->BoardConfig & 0x000000FF) == AI_PLATFORM_CPCIX_3U )
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
    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    PXI_TrgIn_Disable();
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
    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    PXI_TrgOut_Disable();
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
    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    PXI_TTAGSrc_Ctrl(TTAGSrc, TTClrEn);
    return 0;
}



/*! \brief Connect a PBI trigger line with PXI output trigger line */
/*!

    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  ul_TrgDest     AiUInt8                 The PBI trigger line
    \param  ul_TrgSource   AiUInt8                 The PXI trigger output line

    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_in_select(   TY_API_DEV* p_api_dev, AiUInt8 ul_TrgDest, AiUInt32 ul_TrgSource )
{
    BYTE uc_PbiType = 0;
    BYTE uc_TrgLine = 0;

    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    switch(p_api_dev->uc_DeviceType)
    {
      case TYPE_ACX1553_1:
      case TYPE_ACX1553_2:
        uc_PbiType = PBI1553_2;
        break;
      case TYPE_ACX1553_4:
        uc_PbiType = PBI1553_4;
        break;
      case TYPE_ACX3910:
      case TYPE_ACX3910Xp:
        uc_PbiType = PBI3910_1;
        break;
      default:
        uc_PbiType = 0;
    }

    if( 0 == uc_PbiType )
      return API_ERR_INVALID_DEVICE_STATE;

    switch( ul_TrgDest )
    {
      case API_TRG_BC_CHN1:
        uc_TrgLine = BC_CH1;
        break;
      case API_TRG_BC_CHN2:
        uc_TrgLine = BC_CH2;
        break;
      case API_TRG_BC_CHN3:
        uc_TrgLine = BC_CH3;
        break;
      case API_TRG_BC_CHN4:
        uc_TrgLine = BC_CH4;
        break;
      case API_TRG_RT_CHN1:
        uc_TrgLine = RT_CH1;
        break;
      case API_TRG_RT_CHN2:
        uc_TrgLine = RT_CH2;
        break;
      case API_TRG_RT_CHN3:
        uc_TrgLine = RT_CH3;
        break;
      case API_TRG_RT_CHN4:
        uc_TrgLine = RT_CH4;
        break;
      case API_TRG_BM_CHN1:
        uc_TrgLine = BM_CH1;
        break;
      case API_TRG_BM_CHN2:
        uc_TrgLine = BM_CH2;
        break;
      case API_TRG_BM_CHN3:
        uc_TrgLine = BM_CH3;
        break;
      case API_TRG_BM_CHN4:
        uc_TrgLine = BM_CH4;
        break;
      case API_TRG_BM_HS:
        uc_TrgLine = BM_HS;
        break;
    }
    PXI_TrgIn_Sel(uc_PbiType, uc_TrgLine, ul_TrgSource);

    return 0;

}



/*! \brief Connect a PXI trigger line with PBI input trigger line */
/*!

    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  ul_TrgSource   AiUInt8                 The PBI trigger line
    \param  PXITrgLine     AiUInt8                 The PXI trigger input line

    \return                AiReturn                Zero on success
*/
AiReturn  mil_hw_pxi_trigger_out_select(  TY_API_DEV* p_api_dev, AiUInt8 ul_TrgSource, AiUInt8 ul_TrgDest )
{
    BYTE uc_PbiType = 0;
    BYTE uc_TrgLine = 0;

    if( !mil_hw_pxi_is_supported(p_api_dev) )
        return API_ERR_PLATTFORM_NOT_SUPPORTED;

    switch(p_api_dev->uc_DeviceType)
    {
      case TYPE_ACX1553_1:
      case TYPE_ACX1553_2:
        uc_PbiType = PBI1553_2;
        break;
      case TYPE_ACX1553_4:
        uc_PbiType = PBI1553_4;
        break;
      case TYPE_ACX3910:
      case TYPE_ACX3910Xp:
        uc_PbiType = PBI3910_1;
        break;
      default:
        uc_PbiType = 0;
    }

    if( 0 == uc_PbiType )
      return API_ERR_INVALID_DEVICE_STATE;

    switch( ul_TrgSource )
    {
      case API_TRG_BC_CHN1:
        uc_TrgLine = BC_CH1;
        break;
      case API_TRG_BC_CHN2:
        uc_TrgLine = BC_CH2;
        break;
      case API_TRG_BC_CHN3:
        uc_TrgLine = BC_CH3;
        break;
      case API_TRG_BC_CHN4:
        uc_TrgLine = BC_CH4;
        break;
      case API_TRG_RT_CHN1:
        uc_TrgLine = RT_CH1;
        break;
      case API_TRG_RT_CHN2:
        uc_TrgLine = RT_CH2;
        break;
      case API_TRG_RT_CHN3:
        uc_TrgLine = RT_CH3;
        break;
      case API_TRG_RT_CHN4:
        uc_TrgLine = RT_CH4;
        break;
      case API_TRG_BM_CHN1:
        uc_TrgLine = BM_CH1;
        break;
      case API_TRG_BM_CHN2:
        uc_TrgLine = BM_CH2;
        break;
      case API_TRG_BM_CHN3:
        uc_TrgLine = BM_CH3;
        break;
      case API_TRG_BM_CHN4:
        uc_TrgLine = BM_CH4;
        break;
      case API_TRG_BM_HS:
        uc_TrgLine = BM_HS;
        break;
    }
    PXI_TrgOut_Sel(uc_PbiType, uc_TrgLine, ul_TrgDest);

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
AiBoolean mil_hw_trigger_has_digital_loop(TY_API_DEV* p_api_dev, AiUInt32 con)
{
    return API_ERR_PLATTFORM_NOT_SUPPORTED;
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
  BYTE ackfl;
  
  ackfl = api_scope_setup(p_api_dev,  
                         p_scope_setup->ul_CouplingPri,
                         p_scope_setup->ul_CouplingSec,
                         p_scope_setup->ul_SamplingRate,
                         p_scope_setup->ul_CaptureMode,
                         p_scope_setup->ul_OperationMode,
                         p_scope_setup->ul_SingleShotBuf );

  if( ackfl != API_ACK )
    return API_ERR_INVALID_DEVICE_STATE;
  else
    return 0;

}



AiUInt32 mil_hw_scope_trigger(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_EX* p_scope_trigger)
{
  BYTE ackfl;

  if( p_scope_trigger->ul_TrgNbSamples2 != 0 )
    return API_ERR_PARAM6_NOT_IN_RANGE;

  if( p_scope_trigger->ul_TrgDiscrete != 0 )
    return API_ERR_PARAM10_NOT_IN_RANGE;


  ackfl = api_scope_trigger_def(p_api_dev, 
                                p_scope_trigger->ul_TrgMode,
                                p_scope_trigger->ul_TrgSrc,
                                p_scope_trigger->ul_TrgValue,
                                p_scope_trigger->ul_TrgNbSamples,
                                p_scope_trigger->ul_TrgFlags,
                                p_scope_trigger->ul_TrgDelay,
                                p_scope_trigger->ul_TrgTbt );

  if( ackfl != API_ACK )
    return API_ERR_INVALID_DEVICE_STATE;
  else
    return 0;
}




AiUInt32 mil_hw_scope_start(TY_API_DEV* p_api_dev, AiUInt32 ulMode, AiUInt32 ulNoOfCycles )
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}



AiUInt32 mil_hw_scope_status(TY_API_DEV* p_api_dev, TY_API_SCOPE_STATUS* p_Status)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}



AiUInt32 mil_hw_scope_stop(TY_API_DEV* p_api_dev)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}



AiUInt32 mil_hw_scope_trigger_strobe(TY_API_DEV* p_api_dev, TY_API_SCOPE_TRG_STROBE* p_scope_trigger_strobe)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
}

AiUInt32 mil_hw_scope_reset(TY_API_DEV* p_api_dev)
{
  return API_ERR_FUNCTION_NOT_SUPPORTED;
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
  BYTE ackfl;

  ackfl = api_scope_calibrate( p_api_dev, ucMode );

  if( ackfl != API_ACK2 )
    return API_ERR_INVALID_DEVICE_STATE;
  else
    return 0;
}

AiUInt32 mil_hw_scope_type_get( TY_API_DEV *p_api_dev )
{
    if (((p_api_dev->BoardType & 0x000000FF) == 0x00000016)
     || ((p_api_dev->BoardType & 0x000000FF) == 0x00000018))
    {
        return API_MILSCOPE_TYPE_AYX;
    }

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





#endif /* defined(_NUCLEUS) && !defined(_AMC1553) */
