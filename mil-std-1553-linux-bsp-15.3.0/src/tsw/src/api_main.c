/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_main.c
    Main module of 1553 Target Software.
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START

#if !defined(_AMC1553)
static void instrument_sys_config(TY_API_DEV * p_api_dev )
{
    int i;

    PRINTF1("physical_bius:  %d\r\n", p_api_dev->physical_bius  );
    PRINTF1("chns:           %d\r\n", p_api_dev->chns  );
    PRINTF1("chns1:          %d\r\n", p_api_dev->chns1 );
    PRINTF1("chns2:          %d\r\n", p_api_dev->chns2 );

    for( i=0; i<MAX_BIU; i++ )
        PRINTF2("biu_active[%d]: %d\r\n", i, p_api_dev->biu_active[i] );

    for( i=0; i<MAX_BIU; i++ )
        PRINTF2("b_BiuEnabled[%d]: %d\r\n", i, p_api_dev->b_BiuEnabled[i] );


    PRINTF1("init/chns: %d\r\n", p_api_dev->ini_info.streams );
    PRINTF1("init/prot: %d\r\n", p_api_dev->ini_info.prot );


    PRINTF1("biu1_present: %d\r\n", p_api_dev->biu1_present );
    PRINTF1("biu2_present: %d\r\n", p_api_dev->biu2_present );
    PRINTF1("biu3_present: %d\r\n", p_api_dev->biu3_present );
    PRINTF1("biu4_present: %d\r\n", p_api_dev->biu4_present );

    PRINTF1("GlobalRAMBase: %p\r\n", p_api_dev->GlobalRAMBase );
    PRINTF1("GlobalRAMBase2: %p\r\n", p_api_dev->GlobalRAMBase2 );


    for( i=0; i<MAX_BIU; i++ )
        PRINTF2("auc_StreamType[%d]: %d\r\n", i, p_api_dev->auc_StreamType[i] );

    PRINTF1("ul_NbOfStreams1: %d\r\n", p_api_dev->ul_NbOfStreams1 );
    PRINTF1("ul_NbOfStreams2: %d\r\n", p_api_dev->ul_NbOfStreams2 );


    for( i=0; i<MAX_BIU; i++ )
        PRINTF2("cb_p[%d]: %p\r\n", i, p_api_dev->cb_p[i] );

    for( i=0; i<2; i++ )
        PRINTF2("px_api39_cb_p[%d]: %p\r\n", i, px_api39_cb_p[i] );

}
#endif /*_AMC1553*/



extern TY_HW_INFORMATION *px_HwInfo; /* mil_main.c */



#define MIL_HW_AYE_APPLICATION_FLAGS_IS_EMBEDDED(x)           ((x) & (1<<4))
#define MIL_HW_AYE_APPLICATION_FLAGS_IS_NOREPLAY_NOERROR(x)   ((x) & (1<<5))

static void v_InitializeDrvModes_AyE(TY_API_DEV *p_api_dev)
{
    L_WORD bt;
    L_WORD ul_Index;
    L_WORD ul_NbOfInvestigatedBius = 0;
    L_WORD ul_CheckValue;
    L_WORD ul_ApplicationType;
    L_WORD ul_Embedded, ul_NoReplayNoError;
    L_WORD ul_ApplicationFlags;
    WORD   uw_AYE_NVR2BT[] ={ API_DEVICE_MODE_FULL, 0xFFFF, API_DEVICE_MODE_SIM, 0xFFFF, API_DEVICE_MODE_SF, 0xFFFF, API_DEVICE_MODE_DISABLE_BC };

    bt = MIL_BOARD_TYPE(p_api_dev);

    /* Check NOVRAM settings */
    for( ul_Index=0; ul_Index<4; ul_Index++ )
    {
        ul_CheckValue       = (bt                              >> (ul_Index*8)) & 0x000000F0;
        ul_ApplicationType  = (p_api_dev->BoardApplicationType >> (ul_Index*8)) & 0x0000000F;
        ul_ApplicationFlags = (p_api_dev->BoardApplicationType >> (ul_Index*8)) & 0x000000F0;

        if (MIL_HW_AYE_APPLICATION_FLAGS_IS_EMBEDDED(ul_ApplicationFlags))
        {
            ul_Embedded = API_DEVICE_MODE_EMBEDDED;
        }
        else
        {
            ul_Embedded = 0;
        }


        if (MIL_HW_AYE_APPLICATION_FLAGS_IS_NOREPLAY_NOERROR(ul_ApplicationFlags))
        {
            ul_NoReplayNoError = API_DEVICE_MODE_NOREPLAY_NOERROR;
        }
        else
        {
            ul_NoReplayNoError = 0;
        }


        switch(ul_CheckValue)
        {
          case 0x00000000:
          case 0x00000010:
          case 0x00000030:
          case 0x00000070:
            /* single channel firmware */
            p_api_dev->drv_mode[ul_NbOfInvestigatedBius] = uw_AYE_NVR2BT[ ul_ApplicationType % 7 ];
            p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_Embedded;
            p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_NoReplayNoError;
            ul_NbOfInvestigatedBius++;
            break;
          case 0x000000A0:
            /* multi channel firmware */
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] = uw_AYE_NVR2BT[ul_ApplicationType % 7];
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_Embedded;
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_NoReplayNoError;
              ul_NbOfInvestigatedBius++;

              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] = uw_AYE_NVR2BT[ul_ApplicationType % 7];
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_Embedded;
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius] |= ul_NoReplayNoError;
              ul_NbOfInvestigatedBius++;

            break;
          case 0x000000F0:
            /* disabled */
          default:
            break;
        }

    }
}


#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SIM(x)           ((x) & (1<<0))
#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SF(x)            ((x) & (1<<1))
#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_EBD(x)           ((x) & (1<<2))
#define MIL_NOVRAM_APPLICATION_TYPE_BITMASK_DBC(x)           ((x) & (1<<3))

static void v_InitializeDrvModes_ARTIX( TY_API_DEV * p_api_dev )
{
    L_WORD bt;
    L_WORD ul_Index;
    L_WORD ul_NbOfInvestigatedBius = 0;
    L_WORD ul_CheckValue;
    L_WORD ul_ApplicationType;
    WORD   mode = 0, embedded = 0;

    bt = MIL_BOARD_TYPE(p_api_dev);

    /* Check NOVRAM settings */
    for( ul_Index=0; ul_Index<4; ul_Index++ )
    {
        ul_CheckValue      = (bt                              >> (ul_Index*8))  & 0x000000F0;

        if(ul_Index <2)
          ul_ApplicationType = (p_api_dev->BoardApplicationType  >> ((ul_Index  )*16)) & 0x0000FFFF;
        else
          ul_ApplicationType = (p_api_dev->BoardApplicationType2 >> ((ul_Index-2)*16)) & 0x0000FFFF;

        if(MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SIM(ul_ApplicationType))
            mode = API_DEVICE_MODE_SIM;
        else if (MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SF(ul_ApplicationType))
            mode = API_DEVICE_MODE_SF;
        else if (MIL_NOVRAM_APPLICATION_TYPE_BITMASK_DBC(ul_ApplicationType))
            mode = API_DEVICE_MODE_DISABLE_BC;
        else
            mode = API_DEVICE_MODE_FULL;

        if(MIL_NOVRAM_APPLICATION_TYPE_BITMASK_EBD(ul_ApplicationType))
            embedded = API_DEVICE_MODE_EMBEDDED;


        switch(ul_CheckValue)
        {
          case 0x00000000:
          case 0x00000010:
            /* single channel firmware */
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius   ]  = mode;
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius++ ] |= embedded;
            break;
          case 0x000000F0:
            /* disabled */
          default:
            break;
        }
    }
}

static void v_InitializeDrvModes_ZynqMP( TY_API_DEV * p_api_dev )
{
    L_WORD bt;
    L_WORD ul_Index;
    L_WORD ul_NbOfInvestigatedBius = 0;
    L_WORD ul_CheckValue;
    L_WORD ul_ApplicationType;
    WORD   mode = 0, embedded = 0;

    bt = MIL_BOARD_TYPE(p_api_dev);

    /* Check NOVRAM settings */
    for( ul_Index=0; ul_Index<2; ul_Index++ )
    {
        ul_CheckValue      = (bt                              >> (ul_Index*8))  & 0x000000F0;
        ul_ApplicationType = (p_api_dev->BoardApplicationType >> (ul_Index*16)) & 0x0000FFFF;

        if(MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SIM(ul_ApplicationType))
            mode = API_DEVICE_MODE_SIM;
        else if (MIL_NOVRAM_APPLICATION_TYPE_BITMASK_SF(ul_ApplicationType))
            mode = API_DEVICE_MODE_SF;
        else if (MIL_NOVRAM_APPLICATION_TYPE_BITMASK_DBC(ul_ApplicationType))
            mode = API_DEVICE_MODE_DISABLE_BC;
        else
            mode = API_DEVICE_MODE_FULL;

        if(MIL_NOVRAM_APPLICATION_TYPE_BITMASK_EBD(ul_ApplicationType))
            embedded = API_DEVICE_MODE_EMBEDDED;


        switch(ul_CheckValue)
        {
          case 0x00000000:
          case 0x00000010:
          case 0x00000030:
          case 0x00000070:
            /* single channel firmware */
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius   ]  = mode;
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius++ ] |= embedded;
            break;
          case 0x000000A0:
            /* multi channel firmware */
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius   ]  = mode;
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius++ ] |= embedded;
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius   ]  = mode;
              p_api_dev->drv_mode[ ul_NbOfInvestigatedBius++ ] |= embedded;
            break;
          case 0x000000F0:
            /* disabled */
          default:
            break;
        }
    }
}

static void v_InitializeDrvModes_AyX(TY_API_DEV *p_api_dev)
{
    L_WORD ul_Index;
    L_WORD ul_NbOfInvestigatedBius = 0;
    L_WORD ul_CheckValue;

    /* Check NOVRAM settings */
    for( ul_Index=0; ul_Index<4; ul_Index++ )
    {
        ul_CheckValue = (p_api_dev->BoardType >> (ul_Index*8)) & 0x000000FF;

        switch(ul_CheckValue)
        {
          case 0x00000010:
          case 0x00000011:
          case 0x00000016:
          case 0x00000030:
          case 0x00000031:
          case 0x00000070:
          case 0x00000071:
          case 0x00000000:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_FULL; /* BIU1: SIM + MON */
            break;
          case 0x000000A0:
          case 0x000000A1:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_FULL; /* BIU1: SIM + MON */
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_FULL; /* BIU1: SIM + MON */
            break;
          case 0x00000012:
          case 0x00000013:
          case 0x00000032:
          case 0x00000033:
          case 0x00000072:
          case 0x00000073:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SIM; /* BIU1: SIM only */
            break;
          case 0x000000A2:
          case 0x000000A3:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SIM; /* BIU1: SIM only */
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SIM; /* BIU1: SIM only */
            break;
          case 0x00000014:
          case 0x00000015:
          case 0x00000018:
          case 0x00000034:
          case 0x00000035:
          case 0x00000074:
          case 0x00000075:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SF; /* BIU1: Single Function only */
            break;
          case 0x000000A4:
          case 0x000000A5:
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SF; /* BIU1: Single Function only */
              p_api_dev->drv_mode[ul_NbOfInvestigatedBius++] = API_DEVICE_MODE_SF; /* BIU1: Single Function only */
            break;
          case 0x000000FF:
          default:
            /* This case may happen in between in case of a EFA-1 board.*/
            break;
        }
    }
}

void v_InitializeDrvModes(TY_API_DEV *p_api_dev)
{
    L_WORD ul_Index;

    /* Set default values for Single Function Mode Parameters
    * drv_exe - Indicates which mode are currently executing
    *    BIT 0:  Set indicates Bus Controller is running
    *    BIT 1:  Set indicates Bus Monitor is running
    *    BIT 2:  Set indicates RT running in emulation mode
    *    Bit 3:  Set indicates RT running in Mailbox mode
    * drv_rt_status - Indicates which RT's are Initialized for emulation mode
    *    BIT 0:  Set indicates RT0 Initialized in Emulation Mode
    *    BIT 1:  Set indicates RT0 Initialized in Emulation Mode
    *     ...................................................
    *     ...................................................
    *     ...................................................
    *    BIT 31: Set indicates RT31 Initialized in Emulation Mode
    */

    for( ul_Index=0; ul_Index<MAX_BIU; ul_Index++ )
    {
        p_api_dev->drv_exe[       ul_Index ] = 0;
        p_api_dev->drv_rt_status[ ul_Index ] = 0;
        p_api_dev->drv_mode[      ul_Index ] = 0xFFFF;
    }

    if(    (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE    )
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE_ASP)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP)
        || (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYS_ASP_MA) )
    {
        v_InitializeDrvModes_AyE(p_api_dev);
    }
    else if( (p_api_dev->ul_DeviceGroup == AI_DEVICE_ZYNQMP_ASP) )
        {
        v_InitializeDrvModes_ZynqMP(p_api_dev);
    }
    else if( (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIX7)
             || (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIXUS) )
        {
        v_InitializeDrvModes_ARTIX(p_api_dev);
    }
    else
    {
        v_InitializeDrvModes_AyX(p_api_dev);
    }
}



#if !defined(_AMC1553)

void v_ReadDebugLevel( TY_API_DEV *p_api_dev )
{
  L_WORD ulLevel = 0;

  PRINTF0("\r\nAvailable debug levels in HEX:\r\n");
  PRINTF0("       0: no debug output\r\n");
  PRINTF0("       1: debug function calls\r\n");
  PRINTF0("       2: debug mux track\r\n");
  PRINTF0("       4: analyze recording data with word count check on-the-fly\r\n");
  PRINTF0("       8: return BC/RT interrupt count in LLC field of user int struct\r\n");
  PRINTF0("      10: debug data queues (all except int conext)\r\n");
  PRINTF0("      20: debug data queues (interrupt context)\r\n");
  PRINTF0("      40: debug parameter recording (interrupt context)\r\n");
  PRINTF0("      80: debug replay\r\n");
  PRINTF0("     100: debug MilScope\r\n");
  PRINTF0("     200: trace MilScope\r\n");
  PRINTF0("    1000: debug DMA\r\n");
  PRINTF0("    2000: debug memory\r\n");
  PRINTF0("    4000: debug genio\r\n");
  PRINTF0("80000000: reset BC/RT interrupt counter to 0\r\n");
  PRINTF0("\r\n");
  PRINTF1("Previous debug level is 0x%X\r\n", p_api_dev->ul_DbgLevel);
  PRINTF0("Enter new debug level in HEX: ");

  sscanf(gets(InBuf), "%x", &ulLevel);
  PRINTF0("\r\n");
  if( ulLevel & DBG_RESET_INTERRUPT_CNT )
  {
    PRINTF2("reset interrupt counters BC=%08d, RT=%08d\r\n", p_api_dev->ul_CntBCInt, p_api_dev->ul_CntRTInt);
    p_api_dev->ul_CntBCInt = 0;
    p_api_dev->ul_CntRTInt = 0;
  }
  else
    p_api_dev->ul_DbgLevel = ulLevel;
}



void v_ReadInstrLevel( TY_API_DEV *p_api_dev )
{
  /* Read Instrumentation level */
  L_WORD ulGroup = 0;
  L_WORD ulLevel = 0;

  PRINTF2("Group %08X: Instrument MilScope. => %08X\r\n",                INSTR_MILSCOPE, p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] );
  PRINTF1("Level %08X: Generate saw tooth sample data\r\n",              INSTR_MILSCOPE_GENERATE_SAW_TOOTH );
  PRINTF1("Level %08X: Do not copy data into correct order\r\n",         INSTR_MILSCOPE_NO_COPY            );
  PRINTF1("Level %08X: Do not reorder samples\r\n",                      INSTR_MILSCOPE_NO_SAMPLE_REORDER  );
  PRINTF1("Level %08X: Do not add calibration offset\r\n",               INSTR_MILSCOPE_NO_SAMPLE_OFFSET   );
  PRINTF1("Level %08X: Print  MILScope data in HISR\r\n",                INSTR_MILSCOPE_PRINT              );
  PRINTF1("Level %08X: Backup MILScope data in HISR\r\n",                INSTR_MILSCOPE_BACKUP_DATA        );
  PRINTF1("Level %08X: Backup MILScope print\r\n",                       INSTR_MILSCOPE_BACKUP_PRINT       );
  PRINTF1("Level %08X: Backup MILSCOPE DataQueue in HISR\r\n",           INSTR_MILSCOPE_BACKUP_DQUEUE      );
  PRINTF1("Level %08X: Backup MILSCOPE DataQueue print  \r\n",           INSTR_MILSCOPE_BACKUP_DQUEUE_PRINT);
  PRINTF0("\r\n");
  PRINTF2("Group %08X: Instrument IRQ. => %08X\r\n",                     INSTR_SYS, p_api_dev->ul_InstrLevel[INSTR_SYS] );
  PRINTF1("Level %08X: Check for HISR stack overflow\r\n",               INSTR_IRQ_STACK_OVERFLOW );
  PRINTF1("Level %08X: Print board configuration\r\n",                   INSTR_SYS_CONFIG );

  PRINTF0("\r\n");
  PRINTF0("Enter group to change (hex): ");
  sscanf(gets(InBuf), "%x", &ulGroup);

  PRINTF0("\r\n");
  PRINTF0("Enter level for group (hex): ");
  sscanf(gets(InBuf), "%x", &ulLevel);

  PRINTF0("\r\n");

  if( ulGroup < MAX_INSTR_LEVEL )
  {
    if( ulGroup == INSTR_MILSCOPE )
    {
#if defined(_NUCLEUS)
    /* AyX : Milscope functions available */
    if( ulLevel == INSTR_MILSCOPE_BACKUP_PRINT )
    {
      mscope_post_analyse_data( p_api_dev );
    }
    else if( ulLevel == INSTR_MILSCOPE_BACKUP_DQUEUE_PRINT )
    {
      mscope_post_analyse_dqueue( p_api_dev );
    }
    else
    {
      p_api_dev->ul_InstrLevel[ulGroup] = ulLevel;
    }
#else
    /* Milscope functions not available */
    p_api_dev->ul_InstrLevel[ulGroup] = ulLevel;
#endif
    }
    else if( ulGroup == INSTR_SYS )
    {
        if( ulLevel == INSTR_SYS_CONFIG )
        {
          instrument_sys_config( p_api_dev );
        }
        else
        {
            p_api_dev->ul_InstrLevel[ulGroup] = ulLevel;
        }
    }
  }
  else
    PRINTF0("Error group is not in range.\r\n");
}



void v_ShowIntUsage(TY_API_DEV *p_api_dev)
{
  L_WORD ul_DestBiu;
  L_WORD ul_NbOfItemFound;
  L_WORD ul_Index;
  L_WORD ul_RTIndex;
  L_WORD ul_SAIndex;
  TY_API_TRACK_DEF *pTrack;

  for(ul_DestBiu=0; ul_DestBiu<p_api_dev->ul_NbOfStreams1+p_api_dev->ul_NbOfStreams2; ul_DestBiu++)
  {
    PRINTF1("Current Interrupt Usage List [BIU %d]\r\n", ul_DestBiu);
    PRINTF0("===============================================================================================\r\n\r\n");

/***************************************************************************************/
    PRINTF0("Enabled BC Xfer Interrupts on XferIds:  ");
    ul_NbOfItemFound = 0;
    for( ul_Index=0; ul_Index<p_api_dev->glb_mem.biu[ ul_DestBiu ].count.bc_xfer_desc; ul_Index++ )
    {
      if( API_ON == p_api_dev->bc_setup[ ul_DestBiu ].xfer_interrupt[ ul_Index ] )
      {
        PRINTF1("%03d ", ul_Index);
        ul_NbOfItemFound++;
      }
      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");

/***************************************************************************************/
    PRINTF0("Enabled BC Systag Interrupts on XferIds:");
    ul_NbOfItemFound = 0;
    for( ul_Index=0; ul_Index<p_api_dev->glb_mem.biu[ ul_DestBiu ].count.bc_xfer_desc; ul_Index++ )
    {
      if( p_api_dev->bc_setup[ ul_DestBiu ].bc_systag[ ul_Index ].cnt != 0)
      {
        PRINTF1("%03d ", ul_Index);
        ul_NbOfItemFound++;
      }

      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");

/***************************************************************************************/
    PRINTF0("Enabled BC Track Interrupts on XferIds: ");
    ul_NbOfItemFound = 0;
    if(ul_DestBiu < p_api_dev->ul_NbOfStreams1)
      pTrack = &p_api_dev->ls_track[ ul_DestBiu][ 0 ];
    else
      pTrack = &p_api_dev->hs_track[ ul_DestBiu][ 0 ];
    for(ul_Index=0; ul_Index<API_MAX_MUX_BUFFER; ul_Index++, pTrack++)
    {
      if(pTrack->bEnable && (pTrack->uType == API_TRACK_TYPE_BC) )
      {
          PRINTF1("%03d ", pTrack->uXfer);
          ul_NbOfItemFound++;
      }

      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");

/***************************************************************************************/
    PRINTF0("Enabled RT SA Interrupts:               ");
    ul_NbOfItemFound = 0;
    for( ul_RTIndex=0; ul_RTIndex<MAX_API_RT; ul_RTIndex++ )
    {
      for( ul_SAIndex=0; ul_SAIndex<MAX_API_RT_MC_SA; ul_SAIndex++ )
      {
        if(p_api_dev->rt_setup[ ul_DestBiu ].rt_int[ ul_RTIndex ].sa_rx[ ul_SAIndex ] == API_ON)
        {
          PRINTF2("RT%02dSA%02d ", ul_RTIndex, ul_SAIndex);
          ul_NbOfItemFound++;
        }

      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
      }
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");

/***************************************************************************************/
    PRINTF0("Enabled RT Systag Interrupts:           ");
    ul_NbOfItemFound = 0;
    for( ul_RTIndex=0; ul_RTIndex<MAX_API_RT; ul_RTIndex++ )
    {
      for( ul_SAIndex=0; ul_SAIndex<MAX_API_RT_MC_SA; ul_SAIndex++ )
      {
        if(p_api_dev->rt_setup[ ul_DestBiu ].rt_systag[ ul_RTIndex ].sa[ ul_SAIndex ].cnt != 0)
        {
          PRINTF2("RT%02dSA%02d ", ul_RTIndex, ul_SAIndex);
          ul_NbOfItemFound++;
        }

      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
      }
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");

/***************************************************************************************/
    PRINTF0("Enabled RT Track Interrupts:            ");
    ul_NbOfItemFound = 0;
    if(ul_DestBiu < p_api_dev->ul_NbOfStreams1)
      pTrack = &p_api_dev->ls_track[ ul_DestBiu][ 0 ];
    else
      pTrack = &p_api_dev->hs_track[ ul_DestBiu][ 0 ];
    for(ul_Index=0; ul_Index<API_MAX_MUX_BUFFER; ul_Index++, pTrack++)
    {
      if(pTrack->bEnable && (pTrack->uType == API_TRACK_TYPE_RT) )
      {
          PRINTF2("RT%02dSA%02d ", pTrack->uRT, pTrack->uSA);
          ul_NbOfItemFound++;
      }

      if( (ul_NbOfItemFound>0) && (0 == (ul_NbOfItemFound % 4)) )
        PRINTF0("\r\n                                        ");
    }
    if(ul_NbOfItemFound)
      PRINTF0("\r\n");
  }


}

void v_PrintAspMenu(TY_API_DEV *p_api_dev)
{
  PRINTF0("\n\r===============================================================================================\n\r");

  switch(p_api_dev->uc_DeviceType)
  {
    case TYPE_APX1553_1:                PRINTF0("APX1553-1 "); break;
    case TYPE_APX1553_2:                PRINTF0("APX1553-2 "); break;
    case TYPE_APX1553_4:                PRINTF0("APX1553-4 "); break;
    case TYPE_APX3910  :                PRINTF0("APX3910 "); break;
    case TYPE_APX3910Xp:                PRINTF0("APX3910Xp "); break;
    case TYPE_ACX1553_1:                PRINTF0("ACX1553-1 "); break;
    case TYPE_ACX1553_2:                PRINTF0("ACX1553-2 "); break;
    case TYPE_ACX1553_2_TWO_PBIS:       PRINTF0("ACX1553-2 (2 PBIs) "); break;
    case TYPE_ACX1553_4:                PRINTF0("ACX1553-4 "); break;
    case TYPE_ACX1553_4_TWO_PBIS:       PRINTF0("ACX1553-4 (2 PBIs) "); break;
    case TYPE_ACX1553_8:                PRINTF0("ACX1553-8 "); break;
    case TYPE_ACX3910  :                PRINTF0("ACX3910 "); break;
    case TYPE_ACX3910Xp:                PRINTF0("ACX3910Xp "); break;
    case TYPE_ACX3910_2  :              PRINTF0("ACX3910-2 "); break;
    case TYPE_ACX3910Xp_2:              PRINTF0("ACX3910Xp-2 "); break;

    case TYPE_ACX1553_4_DS_TWO_PBIS:    PRINTF0("ACX1553-4 "); break;   
    case TYPE_ACX_EFA_1_TWO_PBIS:       PRINTF0("ACX-EFA-1 "); break;      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:     PRINTF0("ACX-EFAXp-1 "); break;    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:    PRINTF0("ACX-EFA-1 "); break;   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:  PRINTF0("ACX-EFAXp-1 "); break; 
    case TYPE_ACX_EFA_2_TWO_PBIS:       PRINTF0("ACX-EFA-2 "); break;      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:     PRINTF0("ACX-EFAXp-2 "); break;   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:    PRINTF0("ACX-EFA-2 "); break;   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:  PRINTF0("ACX-EFAXp-2 "); break; 
    case TYPE_ACX_EFA_4_TWO_PBIS:       PRINTF0("ACX-EFA-4 "); break;      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:     PRINTF0("ACX-EFAXp-4 "); break;   

    case TYPE_AVX1553_1:                PRINTF0("AVX1553-1 "); break;
    case TYPE_AVX1553_2:                PRINTF0("AVX1553-2 "); break;
    case TYPE_AVX1553_2_TWO_PBIS:       PRINTF0("AVX1553-2 (2 PBIs) "); break;
    case TYPE_AVX1553_4:                PRINTF0("AVX1553-4 "); break;
    case TYPE_AVX1553_4_TWO_PBIS:       PRINTF0("AVX1553-4 (2 PBIs) "); break;
    case TYPE_AVX1553_8:                PRINTF0("AVX1553-8 "); break;
    case TYPE_AVX3910  :                PRINTF0("AVX3910 "); break;
    case TYPE_AVX3910Xp:                PRINTF0("AVX3910Xp "); break;
    case TYPE_AVX3910_2  :              PRINTF0("AVX3910-2 "); break;
    case TYPE_AVX3910Xp_2:              PRINTF0("AVX3910Xp-2 "); break;
    case TYPE_AVX1553_4_DS_TWO_PBIS:    PRINTF0("AVX1553-4 "); break;   
    case TYPE_AVX_EFA_1_TWO_PBIS:       PRINTF0("AVX-EFA-1 "); break;  
    case TYPE_AVX_EFAXp_1_TWO_PBIS:     PRINTF0("AVX-EFAXp-1 "); break;
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:    PRINTF0("AVX-EFA-1 "); break;  
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:  PRINTF0("AVX-EFAXp-1 "); break;
    case TYPE_AVX_EFA_2_TWO_PBIS:       PRINTF0("AVX-EFA-2 "); break;  
    case TYPE_AVX_EFAXp_2_TWO_PBIS:     PRINTF0("AVX-EFAXp-2 "); break;
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:    PRINTF0("AVX-EFA-2 "); break;  
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:  PRINTF0("AVX-EFAXp-2 "); break;
    case TYPE_AVX_EFA_4_TWO_PBIS:       PRINTF0("AVX-EFA-4 "); break;  
    case TYPE_AVX_EFAXp_4_TWO_PBIS:     PRINTF0("AVX-EFAXp-4 "); break;
  }

  if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_NONE)
    PRINTF0("MilScope TARGET-SW\n\r");
  else
    PRINTF0("TARGET-SW\n\r");


  PRINTF0(MIL_TSW_VERSION_STRING_FULL "\n\r");
  PRINTF0(MIL_TSW_BUILD_DATE "\n\r");

  PRINTF2(" MON V%02X.%02X\n\r", (MON_VERSION &0xff00) >> 8, MON_VERSION &0x00ff);
  PRINTF2("(c)AIM GmbH <%s %s>\n\r", __DATE__, __TIME__);

  PRINTF0("===============================================================================================\n\r\n\r");
  PRINTF0("D      <DBG_LEVEL>   set the TSW debug level\n\r");
  PRINTF0("X      <INSTR_LEVEL> set the TSW instrumentation level\n\r");
  PRINTF0("I      <INT USAGE>   show current interrupt usage list\n\r");
  PRINTF0("J      <JUMP>        to a specified address (GOTO)\n\r");
  PRINTF0("L      <LOAD>        a File (S1/S2/S3 Records)\n\r");
  PRINTF0("P      <PROG>        Program FLASH from Memory\n\r");
  PRINTF0("T      <TIMER>       Timer tests\n\r");
  PRINTF0("U      <UPDATE>      Modify and Update NOV RAM settings\n\r");
  PRINTF0("V      <VERSION>     Show the version numbers of the components\n\r");
  PRINTF0("RETURN <MEM_INFO>    show memory partition setting\n\r");
  PRINTF0("Q      <QUIT>        quit TSW menu\n\r");
  PRINTF0("\n\r");
}


void v_MonitorMenu(TY_API_DEV *p_api_dev)
{
  UINT32 ul_ExitMenu,
	       ul_FirstCall;
	INT8   c_Choice;
  L_WORD status = 0;

  ul_ExitMenu  = FALSE;
	ul_FirstCall = 1;

	/* ----------------- MONITOR COMMAND LOOP ------------------ */
  while(FALSE == ul_ExitMenu)
  {
    if (ul_FirstCall==1)
    { 
      c_Choice='?';
      ul_FirstCall=0;
    }
    else 
    {	
      c_Choice = toupper(GetChar());  
		  PutChar(toupper(c_Choice));  /* Output character on terminal */
    }
	
    while (KeyPressed()==TRUE)
      GetChar();  /* Remove all remaining characters from Input FIFO */

    PRINTF0("\n\r");

    switch (c_Choice)
    {
      case 'A':
        do_access();
        break;
      case 'C':
        PRINTF0("   > Interrupt COM  < ");
        api_write_int_loli_gen_int(p_api_dev, GT_ICAUSE_CMD__INDEX, 0, 0, 0, 0);
        break;
      case 'D':
        dump_buf();
        break;
      case 'E':
#ifndef _NUCLEUS
        exec_selftest();
        break;
#endif 
      case 'F':
        do_fill();
        break;
      case 'G':
        print_s3recs();
        break;
      case 'I':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        init_hw(status);
        break;
      case 'J':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        do_jump();
        break;
      case 'L':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
        LoadSREC();
#else
        LoadSREC(300000);
#endif 
        break;
      case 'M':
        do_move();
        break;
      case 'O':
        mem_open();
        break;
      case 'P':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
        do_prog(status);
#else 
        do_flashprog(status);
#endif 
        break;
      case 'U':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        update_novram(status);
        break;
   		case 'Q':
        return;
      case 'R':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        do_novread(status);
        break;
      case 'S':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
        LoadSREC();
#else
        LoadSREC(300000);
#endif 
        break;
      case 'T':
        print_textbuf();
        break;
      case 'V':
        PRINTF0("   > Interrupt BIU1 < ");
        api1553_gen_int2host(p_api_dev, GT_ICAUSE_BIU1_INDEX);
        break;
      case 'W':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        do_novwrite(status);
        break;
      case 'X':
        MonIntTest();
        break;
      case 'Y':
        PRINTF0("   > Interrupt BIU2 < ");
        api1553_gen_int2host(p_api_dev, GT_ICAUSE_BIU2_INDEX);
        break;
      case 'Z':
        PRINTF0("   > Interrupt DMA0  < ");
        api_write_int_loli_gen_int(p_api_dev, GT_ICAUSE_DMA0_INDEX, 0, 0, 0, 0);
        break;
   		case '?':
#ifdef _NUCLEUS
        print_menue(1);
#else
        print_menue();
#endif
        break;
   	}
   	PRINTF0("\n\rMon-Command (? for Help) :> ");
 	}  
}

void v_TSWMenu(TY_API_DEV *p_api_dev)
{
#if defined(_NUCLEUS)
  L_WORD tmr_clk1us;
  L_WORD tm;
  L_WORD test_timer;
  L_WORD test_timer2;
#endif

#if defined(_NUCLEUS)
  UINT32 StartAddress;
#endif

  UINT32 ul_ExitMenu,
	       ul_FirstCall;
	INT8   c_Choice;
  L_WORD status = 0;

#ifndef _NUCLEUS
  tmr_clk1us = p_api_dev->clk_1us / 2;
#else
  #ifdef _AMC1553
    tmr_clk1us = 1;
  #else
    tmr_clk1us = p_api_dev->clk_1us;
  #endif
#endif

  ul_ExitMenu  = FALSE;
	ul_FirstCall = 1;

	/* ----------------- TSW COMMAND LOOP ------------------ */
  while(FALSE == ul_ExitMenu)
  {
    if (ul_FirstCall==1)
    { 
      c_Choice='?';
      ul_FirstCall=0;
    }
    else 
    {	
      c_Choice = toupper(GetChar());  
		  PutChar(toupper(c_Choice));  /* Output character on terminal */
    }
	
    while (KeyPressed()==TRUE)
      GetChar();  /* Remove all remaining characters from Input FIFO */

    PRINTF0("\n\r");

    switch (c_Choice)
    {
      case 'D':
        v_ReadDebugLevel(p_api_dev);
        break;
      case 'X':
        v_ReadInstrLevel(p_api_dev);
        break;
      case 'I':
        v_ShowIntUsage(p_api_dev);
        break;
      case 'J':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        do_jump();
        break;
      case 'L':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
        LoadSREC();
#else
        PRINTF0("Enter Start address (HEX) : "); sscanf(gets(InBuf), "%x", &StartAddress);
        LoadSREC(StartAddress);
#endif 
        break;
      case 'P':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
        do_prog(status);
#else 
        do_flashprog(status);
#endif 
        break;
   		case 'Q':
        return;
      case 'T':
        set_time_mark((UINT32*)&test_timer);
        WAIT_CLOCKS(p_api_dev->clk_1us *10)tm = time_since_mark((UINT32*)&test_timer);
        PRINTF1("time (10us?): %d \n\r", tm / tmr_clk1us);
        set_time_mark((UINT32*)&test_timer2);
        WAIT_CLOCKS(p_api_dev->clk_1us *100)tm = time_since_mark((UINT32*)&test_timer2);
        PRINTF1("time (100us?): %d \n\r", tm / tmr_clk1us);
        set_time_mark((UINT32*)&test_timer2);
        WAIT_CLOCKS(p_api_dev->clk_1us *1000)tm = time_since_mark((UINT32*)&test_timer2);
        PRINTF1("time (1000us?): %d \n\r", tm / tmr_clk1us);
        set_time_mark((UINT32*)&test_timer2);
        WAIT_CLOCKS(p_api_dev->clk_1us *10000)tm = time_since_mark((UINT32*)&test_timer2);
        PRINTF1("time (10000us?): %d \n\r", tm / tmr_clk1us);
        tm = time_since_mark((UINT32*)&test_timer);
        PRINTF1("time (all us?): %d \n\r", tm / tmr_clk1us);
        break;
      case 'U':
#ifndef _NUCLEUS
        disable_int(~ALL_INTS);
#endif 
        update_novram(status);
        break;
      case 'V':
        PRINTF0("\n\rBoard Info:\n\r");
#ifdef _NUCLEUS
        PRINTF1(" SN             : %04d \n\r", TCP_Std_NOVRam_read(NVR_SERIAL_A) );
        PRINTF2(" BOARDTYPE      : %08x (TSW-DeviceType = %d)\n\r", TCP_Std_NOVRam_read(NVR_BOARDTYPE_A), p_api_dev->uc_DeviceType );
        PRINTF1(" ASP    SW      : %s\n", MIL_TSW_VERSION_STRING_FULL );
        PRINTF3(" ASL-LCA        : V%xR%x (Build %05x)\n\r", (BSWAP32(p_ASL_Lca_IoRegs->ul_VersionReg) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_ASL_Lca_IoRegs->ul_VersionReg) & 0x0000000F),
                                                            (BSWAP32(p_ASL_Lca_IoRegs->ul_VersionReg) & 0xFFFFF000) >> 12);


        switch(p_api_dev->uc_DeviceType)
        {
          case TYPE_APX1553_1:
          case TYPE_ACX1553_1:
          case TYPE_AVX1553_1:
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
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
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 2 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 2  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 2  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_APX1553_4:
          case TYPE_ACX1553_4:
          case TYPE_AVX1553_4:
            PRINTF1(" BIU 1/2 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1/2 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1/2 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3/4 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 3/4 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3/4 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX1553_2_TWO_PBIS:
          case TYPE_AVX1553_2_TWO_PBIS:
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 2 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 2  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 2  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX1553_4_TWO_PBIS:
          case TYPE_ACX1553_4_DS_TWO_PBIS:   
          case TYPE_ACX3910_2:
          case TYPE_ACX3910Xp_2:
          case TYPE_AVX1553_4_TWO_PBIS:
          case TYPE_AVX1553_4_DS_TWO_PBIS:   
          case TYPE_AVX3910_2:
          case TYPE_AVX3910Xp_2:
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 2 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 2  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 2  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 3  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 4 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU4BOOTSEC_A) );
            PRINTF2(" BIU 4  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 4  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX1553_8:
          case TYPE_AVX1553_8:
            PRINTF1(" BIU 1/2 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1/2 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1/2 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3/4 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 3/4 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3/4 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 5/6 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 5/6 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 5/6 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 7/8 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU4BOOTSEC_A) );
            PRINTF2(" BIU 7/8 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 7/8 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_7-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX_EFA_1_TWO_PBIS:      
          case TYPE_ACX_EFAXp_1_TWO_PBIS:    
          case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
          case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
          case TYPE_AVX_EFA_1_TWO_PBIS:      
          case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
          case TYPE_AVX_EFAXp_1_TWO_PBIS:    
          case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 2 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 2  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 2  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU4BOOTSEC_A) );
            PRINTF2(" BIU 3  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX_EFA_2_TWO_PBIS:      
          case TYPE_ACX_EFAXp_2_TWO_PBIS:   
          case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
          case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
          case TYPE_AVX_EFA_2_TWO_PBIS:      
          case TYPE_AVX_EFAXp_2_TWO_PBIS:   
          case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
          case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
            PRINTF1(" BIU 1 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 2 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 2  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 2  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_2-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 3  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 4 BOOTSEC  : %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU4BOOTSEC_A) );
            PRINTF2(" BIU 4  FW      : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 4  LCA     : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_4-1 ]->lca_id) & 0x0000000F) );
            break;

          case TYPE_ACX_EFA_4_TWO_PBIS:      
          case TYPE_ACX_EFAXp_4_TWO_PBIS:   
          case TYPE_AVX_EFA_4_TWO_PBIS:      
          case TYPE_AVX_EFAXp_4_TWO_PBIS:   
            PRINTF1(" BIU 1/2 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU1BOOTSEC_A) );
            PRINTF2(" BIU 1/2 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 1/2 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_1-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 3/4 BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU2BOOTSEC_A) );
            PRINTF2(" BIU 3/4 FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 3/4 LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_3-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 5   BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU3BOOTSEC_A) );
            PRINTF2(" BIU 5   FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 5   LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_5-1 ]->lca_id) & 0x0000000F) );
            PRINTF1(" BIU 6   BOOTSEC: %02d \n\r", TCP_Std_NOVRam_read(NVR_BIU4BOOTSEC_A) );
            PRINTF2(" BIU 6   FW     : V%02X.%02X\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->cid) & 0x0000FF00) >> 8, BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->cid) & 0x000000FF );
            PRINTF4(" BIU 6   LCA    : V%xR%x - V%xR%x\n\r", (BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->lca_id) & 0x0FF00000) >> 20,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->lca_id) & 0x000F0000) >> 16,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->lca_id) & 0x00000FF0) >> 4,
                                                            (BSWAP32(p_api_dev->cb_p[ BIU_6-1 ]->lca_id) & 0x0000000F) );
            break;
          default:
            break;
        }
#else 
        PRINTF0("\n\rBoard Info:\n\r");
        PRINTF1(" SN             : %04d \n\r", NOVRam_read(NVR_SERIAL_A) );
        PRINTF1(" BOARDTYPE      : %08x \n\r", NOVRam_read(NVR_BOARDTYPE_A) );
        PRINTF0(" ASP    SW      : ");
        PRINTF0(MIL_TSW_VERSION_STRING_FULL "\n");
        PRINTF0(MIL_TSW_BUILD_DATE "\n");
        PRINTF0("\r\n");
#endif 
        PRINTF2(" MON            : V%02X.%02X\n\r", (MON_VERSION &0xff00) >> 8, MON_VERSION &0x00ff);
        break;
      case 13:
        /*	<CTRL><M> */
        PRINTF0("\n\rMemory Layout:\n\r");
        enter_drv_dump_mem_layout(p_api_dev);
        break;
   		case '?':
        v_PrintAspMenu(p_api_dev);
        break;
   	}
   	PRINTF0("\n\rTSW-Command (? for Help) :> ");
 	}  
}
#endif  /*!defined(_AMC1553)*/

/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : API_DRV                   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : In  parameter [in]                                         */
/*                                                                           */
/*    Outputs   : Out parameter [out]                                        */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_drv(TY_API_DEV *p_api_dev, L_WORD mode)
{
  L_WORD status;
#if defined(_NUCLEUS)
  L_WORD tmr_clk1us;
#endif

#if !defined(_AMC1553)
  L_WORD state_timer;
  static L_WORD ul_counter;
  char uc_Ch;
#endif 

#if defined(_NUCLEUS)
  L_WORD tmpl, state, i;
  L_WORD dataqueue_timer;
  L_WORD busload_timer_rel;
  L_WORD busload_timer_abs_started[ MAX_BIU ];
  L_WORD busload_timer_abs_counter[ MAX_BIU ];
  L_WORD busload_timer_reset;
  L_WORD word_cnt_pri_old[ MAX_BIU ];
  L_WORD word_cnt_sec_old[ MAX_BIU ];
  L_WORD word_cnt_pri_abs_old[ MAX_BIU ];
  L_WORD word_cnt_sec_abs_old[ MAX_BIU ];
  L_WORD temp;
  L_WORD time_in_2msec;
  V_FNPTR *pv_Func;
#ifdef DEBUG_TEST
  BYTE *puc_Shared = (BYTE *)p_api_dev->SharedRAMBase;
  L_WORD *pul_Shared;
#endif
  L_WORD load_timer;
  static L_WORD load_timer_min;
  static L_WORD load_timer_max;
  static L_WORD load_timer_max1;
  static L_WORD load_timer_act;
  char uc_EnterMonitorMenuWithNextChar = 0;
  char uc_EnterTSWMenuWithNextChar = 0;

  p_api_dev->ul_DbgLevel = 0;

  for( i=0; i<MAX_INSTR_LEVEL; i++ )
    p_api_dev->ul_InstrLevel[i] = 0;

  load_timer_min = 0xFFFF;
  load_timer_max = 0;
  load_timer_max1 = 0;
  load_timer_act = 0;
  p_api_dev->targ_com_reg_p->ul_IRCounter = 0;

  for(i=0; i<MAX_BIU; i++)
  {
    busload_timer_abs_started[ i ] = 0;
    busload_timer_abs_counter[ i ] = 0;
    word_cnt_pri_old[ i ] = 0;
    word_cnt_sec_old[ i ] = 0;
    word_cnt_pri_abs_old[ i ] = 0;
    word_cnt_sec_abs_old[ i ] = 0;
  }
  busload_timer_reset = 0;
#endif 
  status = 0L;
#if defined(_NUCLEUS)

#ifndef _NUCLEUS
  tmr_clk1us = p_api_dev->clk_1us / 2;
#else
  #ifdef _AMC1553
    tmr_clk1us = 1;
  #else
    tmr_clk1us = p_api_dev->clk_1us;
  #endif
#endif





    state = 0L;
    p_api_dev->ApiRestartAddress = 0;



    set_time_mark( (UINT32*) &state_timer);
    set_time_mark( (UINT32*) &dataqueue_timer);
    set_time_mark( (UINT32*) &busload_timer_rel);

    /* Perform polling loop */
    PRINTF0("ASP Driver Processing loop ...\n\r");

    /*
    		printf("sys_stack_top %08X\n\r",sys_stack_top);
    		printf("exc_stack_top %08X\n\r",exc_stack_top);
    		printf("sys_mem_size %08X\n\r",sys_mem_size);
    		printf("mem_top_addr %08X\n\r",mem_top_addr);
    		printf("icache_size %08X\n\r",icache_size);
    		printf("dcache_size %08X\n\r",dcache_size);
    		printf("icache_line_size %08X\n\r",icache_line_size);
    		printf("dcache_line_size %08X\n\r",dcache_line_size);
    
    		printf("prog_start %08X\n\r",prog_start);
    		printf("prog_end %08X\n\r",prog_end);
    		printf("data_start %08X\n\r",data_start);
    		printf("data_end %08X\n\r",data_end);
    */

    set_time_mark( (UINT32*) &load_timer);

    while(p_api_dev->repeat == API_ON)
    {

#if !defined(_AMC1553) /* API, APX */
      api_dma_Host2TargetCom(p_api_dev);
#endif

      switch(state)
      {

        case 0:
           /* Process Driver Command */

          load_timer_act = time_since_mark( (UINT32*) &load_timer) / (tmr_clk1us);

          if(load_timer_act < load_timer_min)
          {
            load_timer_min = load_timer_act;
            /*printf("MIN %dus ACT %dus MAX %dus (%dms) \n\r", load_timer_min, load_timer_act, load_timer_max, load_timer_max/1000);*/
          }
          if(load_timer_act > load_timer_max)
          {
            load_timer_max = load_timer_act;
            /*printf("MIN %dus ACT %dus MAX %dus (%dms) \n\r", load_timer_min, load_timer_act, load_timer_max, load_timer_max/1000);*/
          }
          if(load_timer_act > load_timer_max1)
          {
            load_timer_max1 = load_timer_act;
          }
          set_time_mark( (UINT32*) &load_timer);

          if(p_api_dev->ApiRestartAddress != 0)
          {
            pv_Func = (V_FNPTR*)p_api_dev->ApiRestartAddress;
            if(p_api_dev->ApiRestartCyclicFlag == FALSE)
              p_api_dev->ApiRestartAddress = 0;
            /* call the user restart function */
            (*pv_Func) ();
          }

          api_bc_acyc_check_ttag(p_api_dev);

          /* mmo FODDL Rucksack */
          if(p_api_dev->ApiFoddlFunc[0] != 0)
          {
            V_FNPTR_DEV *pv_FoddlFunc;

            pv_FoddlFunc = (V_FNPTR_DEV*)p_api_dev->ApiFoddlFunc[0];
            (*pv_FoddlFunc) (p_api_dev, 0, 0);
          }


          if(p_api_dev->ApiFoddlFunc[1] != 0)
          {
            V_FNPTR_DEV *pv_FoddlFunc;

            pv_FoddlFunc = (V_FNPTR_DEV*)p_api_dev->ApiFoddlFunc[1];
            (*pv_FoddlFunc) (p_api_dev, 2, 1);
          }




          if(time_since_mark( (UINT32*) &dataqueue_timer) >= (50000 *(tmr_clk1us) ) )
          {
              set_time_mark( (UINT32*) &dataqueue_timer);
              mil_tasks_dataqueue_process(p_api_dev, -1);

              api_dma_Host2TargetCom(p_api_dev);
          }

          for(i=0; i<MAX_BIU; i++)
          {
            if( (0 == busload_timer_abs_started[ i ]) && (API_BUSY == p_api_dev->bm_status[ i ]) )
             /* monitor is enabled */
            {
              busload_timer_abs_started[ i ] = 1;
              busload_timer_abs_counter[ i ] = 0;
            }
          }

#ifdef DEBUG_TEST
          pul_Shared = (L_WORD *)&puc_Shared[0x1e00];
          pul_Shared[14] = tmr_clk1us;
#endif
          /* calculate bus load every 500ms */
          if(time_since_mark( (UINT32*) &busload_timer_rel) >= (500000 *(tmr_clk1us) ) )
          {
            /* since MILBus is 1MHz, 25000 words (2 Bytes) can be sent at maximum in 500 milli-seconds -> 100 words in 2ms */
            /* since EfaBus is 20MHz, 625000 words (2 Bytes) can be sent at maximum in 500 milli-seconds -> 2500 words in 2ms */

            time_in_2msec = time_since_mark( (UINT32*) &busload_timer_rel) / (2000 *(tmr_clk1us) );
            set_time_mark( (UINT32*) &busload_timer_rel);
            for(i=0; i<MAX_BIU; i++)
            {
              if(API_BUSY == p_api_dev->bm_status[ i ])
              {
                if( ((BIU_2 - 1) == i) && ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
                {
                  temp = BSWAP32( (*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_a); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                  p_api_dev->bm_status_cnt[ i ].bus_load_pri = ( ( (temp - word_cnt_pri_old[ i ]) *100) / (25 *time_in_2msec) ); /* calculate bus load in percent*100 (with 2 digits resolution) */
                  word_cnt_pri_old[ i ] = temp;
                  temp = BSWAP32( (*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_b); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                  p_api_dev->bm_status_cnt[ i ].bus_load_sec = ( ( (temp - word_cnt_sec_old[ i ]) *100) / (25 *time_in_2msec) );
                  word_cnt_sec_old[ i ] = temp;
                }
                else if( (((BIU_4 - 1) == i) || ((BIU_6 - 1) == i)) && ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
                {
                  temp = BSWAP32( (*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_a); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                  p_api_dev->bm_status_cnt[ i ].bus_load_pri = ( ( (temp - word_cnt_pri_old[ i ]) *100) / (25 *time_in_2msec) ); /* calculate bus load in percent*100 (with 2 digits resolution) */
                  word_cnt_pri_old[ i ] = temp;
                  temp = BSWAP32( (*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_b); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                  p_api_dev->bm_status_cnt[ i ].bus_load_sec = ( ( (temp - word_cnt_sec_old[ i ]) *100) / (25 *time_in_2msec) );
                  word_cnt_sec_old[ i ] = temp;
                }
                else
                {
                  temp = BSWAP32( (*p_api_dev->cb_p[ i ]).bm_wcnt_pri);
                  p_api_dev->bm_status_cnt[ i ].bus_load_pri = ( ( (temp - word_cnt_pri_old[ i ]) *10000) / (100 *time_in_2msec) ); /* calculate bus load in percent*100 (with 2 digits resolution) */
                  word_cnt_pri_old[ i ] = temp;
                  temp = BSWAP32( (*p_api_dev->cb_p[ i ]).bm_wcnt_sec);
                  p_api_dev->bm_status_cnt[ i ].bus_load_sec = ( ( (temp - word_cnt_sec_old[ i ]) *10000) / (100 *time_in_2msec) );
                  word_cnt_sec_old[ i ] = temp;
                }
              }
              else
              {
                word_cnt_pri_old[ i ] = 0;
                word_cnt_sec_old[ i ] = 0;
                p_api_dev->bm_status_cnt[ i ].bus_load_pri = 0;
                p_api_dev->bm_status_cnt[ i ].bus_load_sec = 0;
                p_api_dev->bm_status_cnt[ i ].bus_load_pri_avg = 0;
                p_api_dev->bm_status_cnt[ i ].bus_load_sec_avg = 0;
              }
            }

            if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_PRISEC_COUNTER))
            {
                for (i = 0; i < MAX_BIU; i++)
                {
                    if (1 == busload_timer_abs_started[i])
                    {
                        if (0 == busload_timer_abs_counter[i])
                        {
                            if (((BIU_2 - 1) == i) && ul_StreamIsHS(p_api_dev, p_api_dev->biu))
                            {
                                word_cnt_pri_abs_old[i] = BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_a); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                                word_cnt_sec_abs_old[i] = BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_b); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                            }
                            else if ((((BIU_4 - 1) == i) || ((BIU_6 - 1) == i)) && ul_StreamIsHS(p_api_dev, p_api_dev->biu))
                            {
                                word_cnt_pri_abs_old[i] = BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_a); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                                word_cnt_sec_abs_old[i] = BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_b); /* hier mu der richtige offset fuer 3910 bzw. efex gelesen werden */
                            }
                            else
                            {
                                word_cnt_pri_abs_old[i] = BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_pri);
                                word_cnt_sec_abs_old[i] = BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_sec);
                            }
                            if (word_cnt_pri_abs_old[i] || word_cnt_sec_abs_old[i])
                                busload_timer_abs_counter[i]++;
                        }
                        else
                        {
                            if (((BIU_2 - 1) == i) && ul_StreamIsHS(p_api_dev, p_api_dev->biu))
                            {
                                temp = BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_a) - word_cnt_pri_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_pri_avg = (temp * 10) / (625 * busload_timer_abs_counter[i]);
                                temp = BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_b) - word_cnt_sec_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_sec_avg = (temp * 10) / (625 * busload_timer_abs_counter[i]);
                                if ((BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_a) <= word_cnt_pri_abs_old[i]) ||
                                    (BSWAP32((*px_api39_cb_p[API39_HS_BIU_1]).bm_wcnt_bus_b) <= word_cnt_sec_abs_old[i]))
                                    busload_timer_reset = 1;
                            }
                            else if ((((BIU_4 - 1) == i) || ((BIU_6 - 1) == i)) && ul_StreamIsHS(p_api_dev, p_api_dev->biu))
                            {
                                temp = BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_a) - word_cnt_pri_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_pri_avg = (temp * 10) / (625 * busload_timer_abs_counter[i]);
                                temp = BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_b) - word_cnt_sec_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_sec_avg = (temp * 10) / (625 * busload_timer_abs_counter[i]);
                                if ((BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_a) <= word_cnt_pri_abs_old[i]) ||
                                    (BSWAP32((*px_api39_cb_p[API39_HS_BIU_2]).bm_wcnt_bus_b) <= word_cnt_sec_abs_old[i]))
                                    busload_timer_reset = 1;
                            }
                            else
                            {
                                temp = BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_pri) - word_cnt_pri_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_pri_avg = (temp * 10) / (25 * busload_timer_abs_counter[i]); /* 50 words in one milli-second */
                                temp = BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_sec) - word_cnt_sec_abs_old[i];
                                p_api_dev->bm_status_cnt[i].bus_load_sec_avg = (temp * 10) / (25 * busload_timer_abs_counter[i]);
                                if ((BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_pri) < word_cnt_pri_abs_old[i]) ||
                                    (BSWAP32((*p_api_dev->cb_p[i]).bm_wcnt_sec) < word_cnt_sec_abs_old[i]))
                                    busload_timer_reset = 1;
                            }
                            if (busload_timer_reset) /* restart calculation in case of wrap around */
                            {
                                busload_timer_reset = 0;
                                for (i = 0; i < MAX_BIU; i++)
                                {
                                    busload_timer_abs_counter[i] = 0;
                                    p_api_dev->bm_status_cnt[i].bus_load_pri_avg = 0;
                                    p_api_dev->bm_status_cnt[i].bus_load_sec_avg = 0;
                                }
                            }
                            else
                                busload_timer_abs_counter[i]++;
                        }
                    }
                }
            }
          }

          for(i=0; i<MAX_BIU; i++)
          {
            if(API_HALT == p_api_dev->bm_status[ i ])
             /* monitor is disabled */
              busload_timer_abs_started[ i ] = 0;
#ifdef DEBUG_TEST
pul_Shared[17+i*2] = p_api_dev->bm_status_cnt[ i ].bus_load_pri;
pul_Shared[18+i*2] = p_api_dev->bm_status_cnt[ i ].bus_load_sec;
pul_Shared[26+i*2] = p_api_dev->bm_status_cnt[ i ].bus_load_pri_avg;
pul_Shared[27+i*2] = p_api_dev->bm_status_cnt[ i ].bus_load_sec_avg;
#endif
          }

          /* Check Target Control Register if Host ready */
          tmpl = p_api_dev->targ_com_reg_p->tg_control;
          wbflush();
          tmpl &= API_TG_COM_CONTROL_EXE_BIT_SET;

          if(tmpl == API_TG_COM_CONTROL_EXE_BIT_SET)
          {
  #ifdef _TIMING_TEST_
            MEM(GPIO_REG3) = (0xffff);
            wbflush();
  #endif 

            /* Target Communication Status Register */
            /* Clear Completed Bit, set Execution Bit */
            tmpl = 0L;
            tmpl |= API_TG_COM_STATUS_EXE_BIT_SET;
            tmpl &= API_TG_COM_STATUS_CPL_BIT_RES;
            p_api_dev->targ_com_reg_p->tg_status = tmpl;
            wbflush();

            /*******************************/
            /*** Call Driver instruction ***/
            /*******************************/
            api_opr(p_api_dev, (BYTE*)p_api_dev->cmd_p, (BYTE*)p_api_dev->ack_p);
            wbflush();


            switch( *(BYTE*)(p_api_dev->cmd_p) )
            {
            case API_INI:
            case API_RESET:
              load_timer_min = 0xFFFF;
              load_timer_max = 0;
              load_timer_max1 = 0;
              load_timer_act = 0;
              p_api_dev->targ_com_reg_p->ul_IRCounter = 0;
              break;
            default:
              break;
            }

            /* Target Communication Control Register */
            /* Clear Execution Bit */
            tmpl = p_api_dev->targ_com_reg_p->tg_control;
            wbflush();
            tmpl &= API_TG_COM_CONTROL_EXE_BIT_RES;
            p_api_dev->targ_com_reg_p->tg_control = tmpl;
            wbflush(); /* 24.11.98 */

            /* Target Communication Status Register */
            /* Set Completed Bit, clear Execution Bit */
            tmpl = 0L;
            tmpl |= API_TG_COM_STATUS_CPL_BIT_SET;
            tmpl &= API_TG_COM_STATUS_EXE_BIT_RES;
            p_api_dev->targ_com_reg_p->tg_status = tmpl;
            wbflush(); /* 24.11.98 */

  #ifndef _NUCLEUS

            api1553_gen_int2host(p_api_dev, GT_ICAUSE_CMD__INDEX); /* generate Interrupt at end of communication. */

  #endif 


  #ifdef _TIMING_TEST_
            MEM(GPIO_REG3) = (0x0);
            wbflush();
  #endif 
          }

          if(time_since_mark( (UINT32*) &state_timer) >= (100000 *(tmr_clk1us) ) )
          {
            state++;
          }
          break;


        case 1:
           /* Process Monitor Command and update load in the shared memory*/

          p_api_dev->targ_com_reg_p->ul_LoadTimeAct = load_timer_act;
          p_api_dev->targ_com_reg_p->ul_LoadTimeMin = load_timer_min;
          p_api_dev->targ_com_reg_p->ul_LoadTimeMax = load_timer_max;
          if(ul_counter > 100)
          {
            load_timer_max1 = 0;
            ul_counter = 0;
          }
          else
            ul_counter++;

          p_api_dev->targ_com_reg_p->ul_LoadTimeMax1 = load_timer_max1;

          /* 14.04.99 */
          if(KeyPressed() )
          {
            uc_Ch = toupper(ReadKey());  

            if( (uc_Ch != 'M') && (1 == uc_EnterMonitorMenuWithNextChar) )
            {
              printf("\n\rSequence to enter monitor commands has been reset!\n\r");
              uc_EnterMonitorMenuWithNextChar = 0; /* reset entry character sequence to monitor commands */
            }
            if( (uc_Ch != 'T') && (1 == uc_EnterTSWMenuWithNextChar) )
            {
              printf("\n\rSequence to enter TSW commands has been reset!\n\r");
              uc_EnterTSWMenuWithNextChar = 0; /* reset entry character sequence to monitor commands */
            }

            switch(uc_Ch)
            {
            case 'M':
              if( 1 == uc_EnterMonitorMenuWithNextChar )
              {
                v_MonitorMenu(p_api_dev);
                uc_EnterTSWMenuWithNextChar = 0; /* reset entry character sequence to monitor commands */
              }
              break;
            case 'T':
              if( 1 == uc_EnterTSWMenuWithNextChar )
              {
                v_TSWMenu(p_api_dev);
                uc_EnterMonitorMenuWithNextChar = 0; /* reset entry character sequence to monitor commands */
              }
              break;
            case 32:
              uc_EnterMonitorMenuWithNextChar++; /* start entry character sequence to monitor commands */
              uc_EnterTSWMenuWithNextChar++; /* start entry character sequence to tsw commands */
              printf("\n\rPress <M> to enter monitor commands\n\r");
              printf("Press <T> to enter TSW commands\n\r");
              break;
            case '?':
              printf("\n\rPress <SPACE> followed by <M> to enter monitor commands\n\r");
              printf("Press <SPACE> followed by <T> to enter TSW commands\n\r");
              break;
            case 'P': /* Ping Target */
              printf("Pong!");
              break;
            
            default:
              break;
            }
          }

          set_time_mark( (UINT32*) &state_timer);
          state = 0;
          break;
      }
    } /* end: while */
#endif /* _AMC1553 */

  
  PRINTF0("Rebooting TSW ...\r\n");
  return (status);

} /* end: api_drv */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : ENTER_DRV_CHECK_MEM_LAYOUT*/
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void enter_drv_calc_mem_layout_size(TY_API_DEV *p_api_dev, TY_MEM_DEFINITION x_MemDef, L_WORD b_IncludeSimBuf, L_WORD *mem_used)
{
  L_WORD ul_MemSize = 0;

  ul_MemSize += (MAX_API_RT *MAX_API_RT_SA_AREA_SIZE);        /* rt sa desc */
  ul_MemSize += MAX_API_RT_BH_AREA_SIZE(x_MemDef.rt_bh_area); /* rt bh area */
  ul_MemSize += MAX_API_RT_SQ_AREA_SIZE(x_MemDef.rt_sq_area); /* rt sq area */
  ul_MemSize += MAX_API_RT_EQ_AREA_SIZE(x_MemDef.rt_eq_area); /* rt eq area */
  ul_MemSize += MAX_API_BC_BH_AREA_SIZE(x_MemDef.bc_bh_area); /* bc bh area */
  ul_MemSize += MAX_API_BC_SQ_AREA_SIZE(x_MemDef.bc_sq_area); /* bc sq area */
  ul_MemSize += MAX_API_BC_EQ_AREA_SIZE(x_MemDef.bc_eq_area); /* bc eq area */
  ul_MemSize += MAX_API_BC_XFER_DESC_AREA_SIZE(x_MemDef.bc_xfer_desc);  /* bc xfer desc */
  ul_MemSize += MAX_API_BC_HIP_INST_AREA_SIZE(x_MemDef.bc_hip_instr);   /* bc hip instr */
  ul_MemSize += MAX_API_BC_LIP_INST_AREA_SIZE(x_MemDef.bc_lip_instr);   /* bc lip instr */
  ul_MemSize += MAX_API_BC_ACYC_INST_AREA_SIZE(x_MemDef.bc_acyc_instr); /* bc acyc instr */
  if( 1 == b_IncludeSimBuf )
    ul_MemSize += x_MemDef.sim_buf;
  ul_MemSize += x_MemDef.bm_buf;

  if( p_api_dev->ulUseEnhancedBmActivityPage == 1 )
  {
      /* the BM activity page is now 0x14000 instead of 0xC000, however the original area cannot be reused for ApiCmdSetMemPartition */
      ul_MemSize = ul_MemSize + API_BM_ACT_ENH_PAGE_SIZE;
  }


  *mem_used = ul_MemSize;
}

L_WORD enter_drv_check_mem_layout(TY_API_DEV *p_api_dev, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD mem_used[2])
{
  L_WORD ul_MemUsedbyBiu       = 0;
  L_WORD ul_MemUsedbyBiuTotal1 = 0;
  L_WORD ul_MemUsedbyBiuTotal2 = 0;
  L_WORD st                    = API_OK;

  switch(p_api_dev->uc_DeviceType)
  {
    case TYPE_APX1553_1:
    case TYPE_ACX1553_1:
    case TYPE_AVX1553_1:
    case TYPE_APU1553_1:
    case TYPE_AEC1553_1:
    case TYPE_AXC1553_1:
    case TYPE_AXE1553_1:
    case TYPE_ASE1553_1:
    case TYPE_AMCX1553_1:
    case TYPE_AMCE1553_1:
    case TYPE_APE1553_1:
    case TYPE_ANET1553_1:
    case TYPE_ASC1553_1:
    case TYPE_AME1553_1:
    case TYPE_AMEE1553_1:
    case TYPE_AM2E1553_1:
    case TYPE_AXCX1553_1:
        ul_MemUsedbyBiuTotal1 = 0x10000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;
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
    case TYPE_APU1553_2:
    case TYPE_AEC1553_2:
    case TYPE_AXC1553_2:
    case TYPE_AXE1553_2:
    case TYPE_ASE1553_2:
    case TYPE_AMCX1553_2:
    case TYPE_AMCE1553_2:
    case TYPE_APE1553_2:
    case TYPE_ANET1553_2:
    case TYPE_AMEE1553_2:
    case TYPE_AM2E1553_2:
    case TYPE_AXCX1553_2:
    case TYPE_ASC1553_2:
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
      ul_MemUsedbyBiuTotal1 = 0x20000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;
      break;

    case TYPE_APX1553_4:
    case TYPE_ACX1553_4:
    case TYPE_AVX1553_4:
    case TYPE_AXC1553_4:
    case TYPE_AXE1553_4:
    case TYPE_ASE1553_4:
    case TYPE_AMCX1553_4:
    case TYPE_AMCE1553_4:
    case TYPE_APE1553_4:
    case TYPE_AXCX1553_4:
      ul_MemUsedbyBiuTotal1 = 0x40000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU3 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU4 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_4-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX1553_2_TWO_PBIS:
    case TYPE_AVX1553_2_TWO_PBIS:
      ul_MemUsedbyBiuTotal1 = 0x10000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x10000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX1553_4_TWO_PBIS:
    case TYPE_ACX1553_4_DS_TWO_PBIS:   
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX1553_4_TWO_PBIS:
    case TYPE_AVX1553_4_DS_TWO_PBIS:   
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
      ul_MemUsedbyBiuTotal1 = 0x20000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x20000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU6 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_4-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX1553_8:
    case TYPE_AVX1553_8:
      ul_MemUsedbyBiuTotal1 = 0x40000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x40000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU3 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU4 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_4-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_5-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU6 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_6-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU7 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_7-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU8 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_8-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX_EFA_1_TWO_PBIS:      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_1_TWO_PBIS:      
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_1_TWO_PBIS:    
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
      ul_MemUsedbyBiuTotal1 = 0x10000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x20000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU6 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX_EFA_2_TWO_PBIS:      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_2_TWO_PBIS:      
    case TYPE_AVX_EFAXp_2_TWO_PBIS:   
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
      ul_MemUsedbyBiuTotal1 = 0x20000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x20000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU6 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_4-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    case TYPE_ACX_EFA_4_TWO_PBIS:      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:   
    case TYPE_AVX_EFA_4_TWO_PBIS:      
    case TYPE_AVX_EFAXp_4_TWO_PBIS:   
      ul_MemUsedbyBiuTotal1 = 0x40000; /* size of all BIU control blocks */
      ul_MemUsedbyBiuTotal2 = 0x20000; /* size of all BIU control blocks */

      /* calculate size needed for BIU1 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_1-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU2 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_2-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU3 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_3-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU4 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_4-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal1 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU5 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_5-1], 1, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;

      /* calculate size needed for BIU6 */
      enter_drv_calc_mem_layout_size( p_api_dev, ax_MemDef[BIU_6-1], 0, &ul_MemUsedbyBiu);
      ul_MemUsedbyBiuTotal2 += ul_MemUsedbyBiu;
      break;

    default:
      break;
  }

  if( (ul_MemUsedbyBiuTotal1 > p_api_dev->GlobalRAMSize) || (ul_MemUsedbyBiuTotal2 > p_api_dev->GlobalRAMSize2))
    st = API_ERR;

  mem_used[0] = ul_MemUsedbyBiuTotal1;
  mem_used[1] = ul_MemUsedbyBiuTotal2;

  return (st);
} /* end: enter_drv_check_mem_layout */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : ENTER_DRV_SET_MEM_LAYOUT  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 09.10.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
#ifdef _NUCLEUS
  /* AyX Boards can have two PBIs. 
     The board type is separated into the upper 16bits for PBI2 and the lower 16bits for PBI1 
  */
  Error: no longer implemented. _NUCLEUS support will be removed in the future.
#else
/* All board_type BIUs are expected to be on the same PBI */
void enter_drv_set_mem_layout(TY_API_DEV *p_api_dev, L_WORD mode, L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] )
{
#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
  PRINTF0("***enter_drv_set_mem_layout ***\n\r");
  PRINTF1("  Memory Bank                = %08lx\n\r", ul_MemoryBank);
  PRINTF1("  Number of Streams on Mem1  = %08lx\n\r", p_api_dev->ul_NbOfStreams1);
  PRINTF1("  Number of Streams on Mem2  = %08lx\n\r", p_api_dev->ul_NbOfStreams2);
#endif /*#ifndef _AIM_VME*/



  p_api_dev->ulUseEnhancedBmActivityPage = 0;
  if (mode == 2)
  {
      if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_ENH_ACTIVITY_PAGE))
          p_api_dev->ulUseEnhancedBmActivityPage = 1;
  }

  if( 0 == ul_MemoryBank )
  {
    if( ((p_api_dev->BoardType & 0x0000F000) == 0x00003000) ||
        ((p_api_dev->BoardType & 0x0000F000) == 0x00007000) )
    { /* initialize 3910/EFEX on memory bank 0 */
      enter_drv_set_mem_layout_hs( p_api_dev, ul_MemoryBank, ax_MemDef);
    }
    else
    { /* initialize 1553 on memory bank 0 */
      enter_drv_set_mem_layout_ls( p_api_dev, ul_MemoryBank, ax_MemDef);
    }
  }
  else
  {
    if( ((p_api_dev->BoardType & 0xF0000000) == 0x30000000) ||
        ((p_api_dev->BoardType & 0xF0000000) == 0x70000000) )
    { /* initialize 3910/EFEX on memory bank 1 */
      enter_drv_set_mem_layout_hs( p_api_dev, ul_MemoryBank, ax_MemDef );
    }
    else
    { /* initialize 1553 on memory bank 1 */
      enter_drv_set_mem_layout_ls( p_api_dev, ul_MemoryBank, ax_MemDef );
    }
  }
}
#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : ENTER_DRV_SET_MEM_LAYOUT_LS */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void enter_drv_set_mem_layout_ls(TY_API_DEV *p_api_dev,   L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] )
{
  L_WORD i;
  L_WORD ul_NbOfStreams;
  L_WORD ulStreamOffs; 
  
  ulStreamOffs  = (p_api_dev->ul_NbOfStreams1*ul_MemoryBank);

  if( 0 == ul_MemoryBank )
    ul_NbOfStreams = p_api_dev->ul_NbOfStreams1;
  else
    ul_NbOfStreams = p_api_dev->ul_NbOfStreams2;

  if(ul_NbOfStreams == 0)
  {
      /* We have no streams (probably firmware is not loaded),
       * so we don't need to set up memory layout
       */
      return;
  }

  /***********************/
  /* Init Address Values */
  /***********************/
  for( i=0; i<ul_NbOfStreams; i++ )
  {
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr            = 0L; /* BIU1 System Control Block */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].ir_log_addr        = 0L; /* BIU1 Interrupt Loglist */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].rt_desc_addr       = 0L; /* BIU1 RT Descriptor area */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_tcb_addr        = 0L; /* BIU1 Monitor Trigger Block area */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_act_addr        = 0L; /* BIU1 Monitor Trigger Block area */

    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sa_desc    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_bh_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sq_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_eq_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_bh_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_sq_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_eq_area    = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_xfer_desc  = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_hip_instr  = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_lip_instr  = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_acyc_instr = 0L;

    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bm_buf        = 0L;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rep_buf       = 0L;

    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sa_desc    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_bh_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sq_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_eq_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_bh_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_sq_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_eq_area    = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_xfer_desc  = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_hip_instr  = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_lip_instr  = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_acyc_instr = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rep_buf       = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bm_buf        = 0;

    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_bh_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_sq_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_eq_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_bh_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_sq_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_eq_area   = 0;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_xfer_desc = 0;

    p_api_dev->glb_mem.sim_buf_base_addr[ ulStreamOffs + i ]      = 0L;
  }

  /*******************/
  /* Calculate Sizes */
  /*******************/
  for( i=0; i<ul_NbOfStreams; i++ )
  {
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sa_desc   = (MAX_API_RT *MAX_API_RT_SA_AREA_SIZE);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_bh_area   = MAX_API_RT_BH_AREA_SIZE(ax_MemDef[ulStreamOffs + i].rt_bh_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sq_area   = MAX_API_RT_SQ_AREA_SIZE(ax_MemDef[ulStreamOffs + i].rt_sq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_eq_area   = MAX_API_RT_EQ_AREA_SIZE(ax_MemDef[ulStreamOffs + i].rt_eq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_bh_area   = MAX_API_BC_BH_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_bh_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_sq_area   = MAX_API_BC_SQ_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_sq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_eq_area   = MAX_API_BC_EQ_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_eq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_xfer_desc = MAX_API_BC_XFER_DESC_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_xfer_desc);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_hip_instr = MAX_API_BC_HIP_INST_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_hip_instr);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_lip_instr = MAX_API_BC_LIP_INST_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_lip_instr);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_acyc_instr = MAX_API_BC_ACYC_INST_AREA_SIZE(ax_MemDef[ulStreamOffs + i].bc_acyc_instr);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rep_buf      = ax_MemDef[ulStreamOffs + i].rep_buf;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bm_buf       = ax_MemDef[ulStreamOffs + i].bm_buf;
  }

  for( i=0; i<ul_NbOfStreams; i++ )
  {
    p_api_dev->glb_mem.sim_buf_size[ ulStreamOffs + i ] = ax_MemDef[ulStreamOffs + i].sim_buf;
    p_api_dev->glb_mem.sim_buf_count[ ulStreamOffs + i ] = (p_api_dev->glb_mem.sim_buf_size[ ulStreamOffs + i ] / MAX_API_BUF_SIZE);
  }

  /****************/
  /* Store Counts */
  /****************/
  for( i=0; i<ul_NbOfStreams; i++ )
  {
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_bh_area = ax_MemDef[ulStreamOffs + i].rt_bh_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_sq_area = ax_MemDef[ulStreamOffs + i].rt_sq_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.rt_eq_area = ax_MemDef[ulStreamOffs + i].rt_eq_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_bh_area = ax_MemDef[ulStreamOffs + i].bc_bh_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_sq_area = ax_MemDef[ulStreamOffs + i].bc_sq_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_eq_area = ax_MemDef[ulStreamOffs + i].bc_eq_area;
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].count.bc_xfer_desc = ax_MemDef[ulStreamOffs + i].bc_xfer_desc;
  }


  /******************************/
  /* Define Control Block Areas */
  /******************************/
  for( i=0; i<ul_NbOfStreams; i++ )
  {
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr      = API_GRAM2_OFFSET*ul_MemoryBank + 0x10000L*i; /* BIU1 System Control Block */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].ir_log_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr + 0x400L); /* BIU1 Interrupt Loglist */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].rt_desc_addr = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr + 0x800L); /* BIU1 RT Descriptor area */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_tcb_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr + 0x1000L); /* BIU1 Monitor Trigger Block area */
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_act_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].cb_addr + 0x4000L); /* BIU1 Monitor Trigger Block area */
  }

  if( 1 == ul_NbOfStreams )
  { /* Required for Reading of Firmware / LCA versions of API1553-1 */
    p_api_dev->glb_mem.biu[ ul_MemoryBank + BIU_2 - 1 ].cb_addr      = API_GRAM2_OFFSET*ul_MemoryBank + 0x10000L; /* BIU1 System Control Block */
    p_api_dev->glb_mem.biu[ ul_MemoryBank + BIU_2 - 1 ].ir_log_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].cb_addr + 0x400L); /* BIU1 Interrupt Loglist */
    p_api_dev->glb_mem.biu[ ul_MemoryBank + BIU_2 - 1 ].rt_desc_addr = (p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].cb_addr + 0x800L); /* BIU1 RT Descriptor area */
    p_api_dev->glb_mem.biu[ ul_MemoryBank + BIU_2 - 1 ].bm_tcb_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].cb_addr + 0x1000L); /* BIU1 Monitor Trigger Block area */
    p_api_dev->glb_mem.biu[ ul_MemoryBank + BIU_2 - 1 ].bm_act_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].cb_addr + 0x4000L); /* BIU1 Monitor Trigger Block area */
  }

  /***************************/
  /* Calculate Base Addresses*/
  /***************************/
  switch( ul_NbOfStreams )
  {
    case 1:
    default:
      p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].base.rt_sa_desc = API_GRAM2_OFFSET*ul_MemoryBank + 0x10000L;
      break;
    case 2:
      p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].base.rt_sa_desc = API_GRAM2_OFFSET*ul_MemoryBank + 0x20000L;
      break;
    case 3:
      p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].base.rt_sa_desc = API_GRAM2_OFFSET*ul_MemoryBank + 0x30000L;
      break;
    case 4:
      p_api_dev->glb_mem.biu[ ulStreamOffs + BIU_1 - 1 ].base.rt_sa_desc = API_GRAM2_OFFSET*ul_MemoryBank + 0x40000L;
      break;
  }

  for( i=0; i<ul_NbOfStreams; i++ )
  {
    if( i>0 )
      p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sa_desc = (p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].base.bc_acyc_instr + p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].size.bc_acyc_instr);

    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_bh_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sa_desc + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sa_desc);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sq_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_bh_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_bh_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_eq_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_sq_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_sq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_bh_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rt_eq_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.rt_eq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_sq_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_bh_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_bh_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_eq_area    = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_sq_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_sq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_xfer_desc  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_eq_area + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_eq_area);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_hip_instr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_xfer_desc + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_xfer_desc);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_lip_instr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_hip_instr + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_hip_instr);
    p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_acyc_instr = (p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bc_lip_instr + p_api_dev->glb_mem.biu[ ulStreamOffs + i ].size.bc_lip_instr);
  }

  for( i=0; i<ul_NbOfStreams; i++ )
    p_api_dev->glb_mem.sim_buf_base_addr[ ulStreamOffs + i ] = p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].base.bc_acyc_instr + p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].size.bc_acyc_instr;

  for( i=0; i<ul_NbOfStreams; i++ )
  {
    if( 0 == i )
      p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rep_buf  = (p_api_dev->glb_mem.sim_buf_base_addr[ ulStreamOffs + ul_NbOfStreams - 1 ] + p_api_dev->glb_mem.sim_buf_size[ ulStreamOffs + ul_NbOfStreams - 1 ]);
    else
      p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.rep_buf  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].base.rep_buf + p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].size.rep_buf);
  }

  for( i=0; i<ul_NbOfStreams; i++ )
  {
    if( 0 == i )
      p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bm_buf  = (p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].base.rep_buf + p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].size.rep_buf);
    else
      p_api_dev->glb_mem.biu[ ulStreamOffs + i ].base.bm_buf  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].base.bm_buf + p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1 ].size.bm_buf);
  }






  /********************************************************************/
  /* Set different bm_act_addr, if enhanced activity page is required */
  /********************************************************************/
  if( p_api_dev->ulUseEnhancedBmActivityPage == 1 )
  {
    for( i=0; i<ul_NbOfStreams; i++ )
    {
        /* note: this will override settings made above */
        if( 0 == i )
            p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_act_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].base.bm_buf +
                                                                        p_api_dev->glb_mem.biu[ ulStreamOffs + ul_NbOfStreams - 1 ].size.bm_buf);
        else
            p_api_dev->glb_mem.biu[ ulStreamOffs + i ].bm_act_addr  = (p_api_dev->glb_mem.biu[ ulStreamOffs + i - 1].bm_act_addr + 
                                                                          API_BM_ACT_ENH_PAGE_SIZE );
    }
  }





  /* Allocate space for USB scatter list (Not BIU specific, just one list for all) */
  p_api_dev->glb_mem.UsbScatterListSize = USB_SCATTERLIST_SIZE(ax_MemDef[0].UsbScatterListEntries);
  p_api_dev->glb_mem.UsbScatterListEntries = ax_MemDef[0].UsbScatterListEntries;

  /* USB scatter list is laid out after the monitor buffers at end of global memory */
  p_api_dev->glb_mem.UsbScatterListBase = p_api_dev->glb_mem.biu[ul_NbOfStreams - 1].base.bm_buf + 
                                          p_api_dev->glb_mem.biu[ul_NbOfStreams -1].size.bm_buf;

} /* end: enter_drv_set_mem_layout_ls */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : ENTER_DRV_DUMP_MEM_LAYOUT */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void enter_drv_dump_mem_layout(TY_API_DEV *p_api_dev)
{
  WORD   uw_Index;
  L_WORD ul_HsIndex;

  PRINTF0(" *********** Memory mapping *******************\n\r");
  ul_HsIndex = 0;
  p_api_dev->biu = 0;

  for( uw_Index=0; uw_Index<MAX_BIU; uw_Index++ )
  {
    p_api_dev->biu = uw_Index;
    if( API_TYPE_1553 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      PRINTF0("\n\r");
      PRINTF2("[BIU%d] System Control Block                               %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].cb_addr));
      PRINTF2("[BIU%d] Interrupt Loglist                                  %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].ir_log_addr));
      PRINTF2("[BIU%d] RT Descriptor Area                                 %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].rt_desc_addr));
      PRINTF2("[BIU%d] BM Trigger Control Block Area                      %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].bm_tcb_addr));
      PRINTF2("[BIU%d] Monitor Activity Recording/Message Filtering Page  %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].bm_act_addr));
    }
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      if( uw_Index == (BIU_2-1) )
        ul_HsIndex = API39_HS_BIU_1;
      else
        ul_HsIndex = API39_HS_BIU_2;
      PRINTF0("\n\r");
      PRINTF2("[BIU%d] System Control Block Area                          %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].cb_addr));
      PRINTF2("[BIU%d] Interrupt Loglist                                  %p\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].ir_log_addr));
      PRINTF2("[BIU%d] RT Descriptor Area                                 %08X\n\r", uw_Index, (L_WORD)api39_rt_desc_p[ ul_HsIndex ]);
      PRINTF2("[BIU%d] bm_rt_act_flt                                      %08X\n\r", uw_Index, (L_WORD)px_api39_bm_rt_act_flt_p[ ul_HsIndex ]);
    }
  }

  ul_HsIndex = 0;
  for( uw_Index=0; uw_Index<MAX_BIU; uw_Index++ )
  {
    p_api_dev->biu = uw_Index;

    if( API_TYPE_1553 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      PRINTF0("\n\r");
      PRINTF4("[BIU%d] RT Subaddress Descriptor Area                      %p..%p [%08X]\n\r", uw_Index, API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_sa_desc),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_sa_desc) + p_api_dev->glb_mem.biu[ uw_Index ].size.rt_sa_desc-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.rt_sa_desc);
      PRINTF5("[BIU%d] RT Buffer Header Area               [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.rt_bh_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_bh_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_bh_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.rt_bh_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.rt_bh_area);
      PRINTF5("[BIU%d] RT Status Queue Area                [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.rt_sq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_sq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_sq_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.rt_sq_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.rt_sq_area);
      PRINTF5("[BIU%d] RT Event Queue Area                 [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.rt_eq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_eq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rt_eq_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.rt_eq_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.rt_eq_area);
      PRINTF5("[BIU%d] BC Buffer Header Area               [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.bc_bh_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_bh_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_bh_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_bh_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_bh_area);
      PRINTF5("[BIU%d] BC Status Queue Area                [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.bc_sq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_sq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_sq_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_sq_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_sq_area);
      PRINTF5("[BIU%d] BC Event Queue Area                 [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.bc_eq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_eq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_eq_area) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_eq_area-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_eq_area);
      PRINTF5("[BIU%d] BC Transfer Descriptor Area         [%5d]        %p..%p [%08X]\n\r", uw_Index, p_api_dev->glb_mem.biu[ uw_Index ].count.bc_xfer_desc,
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_xfer_desc),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_xfer_desc) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_xfer_desc-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_xfer_desc);
      PRINTF4("[BIU%d] BC High Instruction List Area                      %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_hip_instr),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_hip_instr) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_hip_instr-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_hip_instr);
      PRINTF4("[BIU%d] BC Low Instruction List Area                       %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_lip_instr),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_lip_instr) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_lip_instr-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_lip_instr);
      PRINTF4("[BIU%d] BC Acyclic Instruction List Area                   %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_acyc_instr),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bc_acyc_instr) + p_api_dev->glb_mem.biu[ uw_Index ].size.bc_acyc_instr-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bc_acyc_instr);
    }
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      if( uw_Index == (BIU_2-1) )
        ul_HsIndex = API39_HS_BIU_1;
      else
        ul_HsIndex = API39_HS_BIU_2;

      PRINTF0("\n\r");
      PRINTF4("[BIU%d] RT MID Descriptor Area                             %p..%p [%08X]\n\r", uw_Index,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_mid_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_mid_area) + Api39_glb_mem.stream[ul_HsIndex].size.rt_mid_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.rt_mid_area);
      PRINTF5("[BIU%d] RT Buffer Header Area               [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.rt_bh_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_bh_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_bh_area) + Api39_glb_mem.stream[ul_HsIndex].size.rt_bh_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.rt_bh_area);
      PRINTF5("[BIU%d] RT Status Queue Area                [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.rt_sq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_sq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_sq_area) + Api39_glb_mem.stream[ul_HsIndex].size.rt_sq_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.rt_sq_area);
      PRINTF5("[BIU%d] RT Event Queue Area                 [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.rt_eq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_eq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.rt_eq_area) + Api39_glb_mem.stream[ul_HsIndex].size.rt_eq_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.rt_eq_area);
      PRINTF5("[BIU%d] BC Buffer Header Area               [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.bc_bh_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_bh_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_bh_area) + Api39_glb_mem.stream[ul_HsIndex].size.bc_bh_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.bc_bh_area);
      PRINTF5("[BIU%d] BC Status Queue Area                [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.bc_sq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_sq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_sq_area) + Api39_glb_mem.stream[ul_HsIndex].size.bc_sq_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.bc_sq_area);
      PRINTF5("[BIU%d] BC Event Queue Area                 [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.bc_eq_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_eq_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_eq_area) + Api39_glb_mem.stream[ul_HsIndex].size.bc_eq_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.bc_eq_area);
      PRINTF5("[BIU%d] BC Transfer Descriptor Area         [%5d]        %p..%p [%08X]\n\r", uw_Index,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.bc_xfer_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_xfer_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_xfer_area) + Api39_glb_mem.stream[ul_HsIndex].size.bc_xfer_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.bc_xfer_area);
      PRINTF5("[BIU%d] BC Response Simulator Area          [%5d]        %p..%p [%08X]\n\r", uw_Index, 
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.bc_resp_sim_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_resp_sim_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.bc_resp_sim_area) + Api39_glb_mem.stream[ul_HsIndex].size.bc_resp_sim_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.bc_resp_sim_area);
      PRINTF5("[BIU%d] BC Action Word Area                 [%5d]        %p..%p [%08X]\n\r", uw_Index, 
                                                                                             Api39_glb_mem.stream[ul_HsIndex].count.tx_bc_aw_area,
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.tx_bc_aw_area),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ul_HsIndex].base.tx_bc_aw_area) + Api39_glb_mem.stream[ul_HsIndex].size.tx_bc_aw_area-1,
                                                                                             Api39_glb_mem.stream[ul_HsIndex].size.tx_bc_aw_area);
    }
  }

  PRINTF0("\n\r");
  ul_HsIndex = 0;
  for( uw_Index=0; uw_Index<MAX_BIU; uw_Index++ )
  {
    p_api_dev->biu = uw_Index;

    if( API_TYPE_1553 == p_api_dev->auc_StreamType[ uw_Index ] )
      PRINTF5("[BIU%d] Simulator Buffer Area               [%5d]        %p..%p [%08X]\n\r", uw_Index, 
                                                                                             p_api_dev->glb_mem.sim_buf_count[ uw_Index ],
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.sim_buf_base_addr[ uw_Index ]),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.sim_buf_base_addr[ uw_Index ]) + p_api_dev->glb_mem.sim_buf_size[ uw_Index ]-1,
                                                                                             p_api_dev->glb_mem.sim_buf_size[ uw_Index ]);
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      if( uw_Index == (BIU_2-1) )
        ul_HsIndex = API39_HS_BIU_1;
      else
        ul_HsIndex = API39_HS_BIU_2;
      PRINTF4("[BIU%d] Simulator Buffer Area                              %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ ul_HsIndex ].base.sim_buf),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ ul_HsIndex ].base.sim_buf) + api39_sys_mem[ ul_HsIndex ].uc_s39_bufs * 0x10000-1,
                                                                                             api39_sys_mem[ ul_HsIndex ].uc_s39_bufs * 0x10000);
    }
  }

  PRINTF0("\n\r");
  ul_HsIndex = 0;
  for( uw_Index=0; uw_Index<MAX_BIU; uw_Index++ )
  {
    p_api_dev->biu = uw_Index;

    if( API_TYPE_1553 == p_api_dev->auc_StreamType[ uw_Index ] )
      PRINTF4("[BIU%d] Replay Buffer Area                                 %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rep_buf),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.rep_buf) + p_api_dev->glb_mem.biu[ uw_Index ].size.rep_buf-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.rep_buf);
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      if( uw_Index == (BIU_2-1) )
        ul_HsIndex = API39_HS_BIU_1;
      else
        ul_HsIndex = API39_HS_BIU_2;
      PRINTF4("[BIU%d] Replay Buffer Area                                 %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ ul_HsIndex ].base.rep_buf),
                                                                                             API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ ul_HsIndex ].base.rep_buf) + Api39_glb_mem.stream[ ul_HsIndex ].size.rep_buf-1,
                                                                                             Api39_glb_mem.stream[ ul_HsIndex ].size.rep_buf);
    }
  }

  PRINTF0("\n\r");
  ul_HsIndex = 0;
  for( uw_Index=0; uw_Index<MAX_BIU; uw_Index++ )
  {
    p_api_dev->biu = uw_Index;

    if( API_TYPE_1553 == p_api_dev->auc_StreamType[ uw_Index ] )
      PRINTF4("[BIU%d] BM Buffer Area                                     %p..%p [%08X]\n\r", uw_Index, 
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bm_buf),
                                                                                             API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ uw_Index ].base.bm_buf) + p_api_dev->glb_mem.biu[ uw_Index ].size.bm_buf-1,
                                                                                             p_api_dev->glb_mem.biu[ uw_Index ].size.bm_buf);
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[ uw_Index ] )
    {
      if( uw_Index == (BIU_2-1) )
        ul_HsIndex = API39_HS_BIU_1;
      else
        ul_HsIndex = API39_HS_BIU_2;
      PRINTF4("[BIU%d] BM Buffer Area                                     %08X..%08X [%08X]\n\r", uw_Index, 
                                                                                             api39_sys_mem[ ul_HsIndex ].ul_m39_offset,
                                                                                             api39_sys_mem[ ul_HsIndex ].ul_m39_offset + api39_sys_mem[ ul_HsIndex ].uc_m39_bufs * 0x10000-1,
                                                                                             api39_sys_mem[ ul_HsIndex ].uc_m39_bufs * 0x10000);
    }
  }

  PRINTF0(" *********** Memory mapping *******************\n\r");

  p_api_dev->biu = 0;
} /* end: enter_drv_dump_mem_layout */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : ENTER_DRV                 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void enter_drv(TY_API_DEV *p_api_dev)
{
  L_WORD map_multiplier1 = 1;
  L_WORD map_multiplier2 = 1;
  volatile TY_HW_INFORMATION *lptr;
  TY_MEM_DEFINITION ax_MemDef[MAX_BIU];
  L_WORD ul_Index;

  /* Set Pointer to Local Memory */
  lptr = (TY_HW_INFORMATION*) (p_api_dev->SharedRAMBase);

#if defined(_AMC1553)
  /* Write board info required by SYS drivers into Local RAM */
  lptr->ul_DriverMemId = BSWAP32(AIM_SH_RAM_ID); /* Softie identification */
  lptr->ul_LocalRamSize = BSWAP32(p_api_dev->LocalRAMSize); /* Size of Local Memory in Bytes */
  lptr->ul_LocalRamBase = BSWAP32(p_api_dev->LocalRAMBase); /* Base Address of Local Memory */
  lptr->ul_SharedRamSize = BSWAP32(p_api_dev->SharedRAMSize); /* Size of Shared Memory in Bytes */
  lptr->ul_SharedRamBase = 0L; /* Base Address of Shared Memory */
  lptr->ul_GlobalRamSize = BSWAP32((AiUInt32)p_api_dev->GlobalRAMSize); /* Size of Global Memory in Bytes */
  lptr->ul_GlobalRamBase = 0L; /* Base Address of Global Memory */
  lptr->ul_BoardType = BSWAP32(p_api_dev->BoardType); /* Board Interface Type */
  lptr->ul_BoardConfig = BSWAP32(p_api_dev->BoardConfig); /* Board HW version, Platform Type */
  lptr->ul_BoardPartNo = BSWAP32(p_api_dev->BoardPartNo); /* AIM Partnumber for the Board */
  lptr->ul_BoardSerialNo = BSWAP32(p_api_dev->BoardSerial); /* Board Serial Number */
  lptr->ul_SysClk = BSWAP32(p_api_dev->SYSClk); /* System/Bus Clock in MHz */
  lptr->ul_CpuClk = BSWAP32(p_api_dev->CPUClk); /* Processor Clock Frequency in MHz */

#else
  /* Write board info required by SYS drivers into Local RAM */
  lptr->ul_DriverMemId = AIM_SH_RAM_ID; /* Softie identification */
  lptr->ul_LocalRamSize = p_api_dev->LocalRAMSize; /* Size of Local Memory in Bytes */
  lptr->ul_LocalRamBase = (L_WORD)p_api_dev->LocalRAMBase; /* Base Address of Local Memory */
  lptr->ul_SharedRamSize = p_api_dev->SharedRAMSize; /* Size of Shared Memory in Bytes */
  lptr->ul_SharedRamBase = (L_WORD)p_api_dev->SharedRAMBase; /* Base Address of Shared Memory */
  lptr->ul_GlobalRamSize = p_api_dev->GlobalRAMSize; /* Size of Global Memory in Bytes */
#ifdef _NUCLEUS
  lptr->ul_GlobalRamSize2= p_api_dev->GlobalRAMSize2; /* Size of Global Memory in Bytes */
#endif
  lptr->ul_GlobalRamBase = (L_WORD)p_api_dev->GlobalRAMBase; /* Base Address of Global Memory */
#ifdef _NUCLEUS
  lptr->ul_GlobalRamBase2= (L_WORD)p_api_dev->GlobalRAMBase2; /* Base Address of Global Memory */
#endif
  lptr->ul_BoardType = p_api_dev->BoardType; /* Board Interface Type */
  lptr->ul_BoardConfig = p_api_dev->BoardConfig; /* Board HW version, Platform Type */
  lptr->ul_BoardPartNo = p_api_dev->BoardPartNo; /* AIM Partnumber for the Board */
  lptr->ul_BoardSerialNo = p_api_dev->BoardSerial; /* Board Serial Number */
  lptr->ul_SysClk = p_api_dev->SYSClk; /* System/Bus Clock in MHz */
  lptr->ul_CpuClk = p_api_dev->CPUClk; /* Processor Clock Frequency in MHz */
  lptr->ul_LcaBootSector = lcaBootSec; /* LCA Boot Sector */
  lptr->aul_BiuBootSector[ 0 ] = biu1BootSec; /* Biu1 Boot Sector */
  lptr->aul_BiuBootSector[ 1 ] = biu2BootSec; /* Biu2 Boot Sector */
  lptr->ae_BiuStatus[ 0 ] = (biu1_present ? BIU_STARTED : BIU_DOWN); /* Biu1 Status */
  lptr->ae_BiuStatus[ 1 ] = (biu2_present ? BIU_STARTED : BIU_DOWN); /* Biu2 Status */
#ifdef _NUCLEUS
  lptr->aul_BiuBootSector[ 2 ] = biu3BootSec; /* Biu1 Boot Sector */
  lptr->aul_BiuBootSector[ 3 ] = biu4BootSec; /* Biu2 Boot Sector */
  lptr->ae_BiuStatus[ 2 ] = (biu3_present ? BIU_STARTED : BIU_DOWN); /* Biu1 Status */
  lptr->ae_BiuStatus[ 3 ] = (biu4_present ? BIU_STARTED : BIU_DOWN); /* Biu2 Status */
#endif
  lptr->ul_IrigTimeHi = 0; /* IRIG Time Hi */
  lptr->ul_IrigTimeLo = 0; /* IRIG Time Lo */
  wbflush(); /* table to be continued */
#endif 

  /* Check NOVRAM settings */
#if defined(_NUCLEUS)
  p_api_dev->conf1 = (short) ( p_api_dev->GlobalRAMSize / 0x100000L);
  p_api_dev->conf2 = (short) ( p_api_dev->GlobalRAMSize2 / 0x100000L);
#else
  p_api_dev->conf1 = (short) (p_api_dev->GlobalRAMSize / 0x100000L);
#endif


  p_api_dev->physical_bius      = uc_CalcNumberOfPhysicalBius(p_api_dev);

#if defined(_NUCLEUS)
  /* AyX Boards can have two PBIs. 
     The board type is separated into the upper 16bits for PBI2 and the lower 16bits for PBI1 
  */
  Error: no longer implemented. _NUCLEUS support will be removed in the future.
#else
  /* All board_type BIUs are expected to be on the same PBI */
  p_api_dev->chns1              = uc_CalcNumberOfChannels(p_api_dev);
  p_api_dev->chns2              = 0;
#endif

  p_api_dev->chns               = p_api_dev->chns1 + p_api_dev->chns2;
  p_api_dev->uc_DeviceType      = uc_GetDeviceType(p_api_dev);

  v_InitializeDrvModes( p_api_dev );

#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
  /* Debug info */
  PRINTF0("\n\renter_drv:\n\r");
#if defined(_NUCLEUS)
  PRINTF1("SoftieID      : %08lX\n\r", lptr->ul_DriverMemId);
  PRINTF1("Sub ID        : %08lX\n\r", lptr->ul_SubSysVenId);
  PRINTF1("LocalRAMSize  : %08lX\n\r", lptr->ul_LocalRamSize);
  PRINTF1("LocalRAMBase  : %08lX\n\r", lptr->ul_LocalRamBase);
  PRINTF1("SharedRAMSize : %08lX\n\r", lptr->ul_SharedRamSize);
  PRINTF1("SharedRAMBase : %08lX\n\r", lptr->ul_SharedRamBase);
  PRINTF1("GlobalRAMSize : %08lX\n\r", lptr->ul_GlobalRamSize);
#ifdef _NUCLEUS
  PRINTF1("GlobalRAMSize2: %08lX\n\r", lptr->ul_GlobalRamSize2);
#endif
  PRINTF1("GlobalRAMBase : %08lX\n\r", lptr->ul_GlobalRamBase);
#ifdef _NUCLEUS
  PRINTF1("GlobalRAMBase2: %08lX\n\r", lptr->ul_GlobalRamBase2);
#endif
  PRINTF1("BoardType     : %08lX\n\r", lptr->ul_BoardType);
  PRINTF1("BoardConfig   : %08lX\n\r", lptr->ul_BoardConfig);
  PRINTF1("BoardPartNo   : %08lX\n\r", lptr->ul_BoardPartNo);
  PRINTF1("BoardSerial   : %08lX\n\r", lptr->ul_BoardSerialNo);
  PRINTF1("SYSClk        : %08lX\n\r", lptr->ul_SysClk);
  PRINTF1("CPUClk        : %08lX\n\r", lptr->ul_CpuClk);
  PRINTF1("Dev/Vendor ID : %08lX\n\r", lptr->ul_DevVendId);
  PRINTF1("IRQ           : %08lX\n\r", lptr->ul_Interrupt);
  PRINTF1("LCA Boot Sec  : %08lX\n\r", lptr->ul_LcaBootSector);
  PRINTF1("LCA Status    : %08lX\n\r", lptr->ul_LcaStatus);
  PRINTF1("BIU1 Boot Sec : %08lX\n\r", lptr->aul_BiuBootSector[ 0 ]);
  PRINTF1("BIU2 Boot Sec : %08lX\n\r", lptr->aul_BiuBootSector[ 1 ]);
  PRINTF1("BIU1 Status   : %08lX\n\r", lptr->ae_BiuStatus[ 0 ]);
  PRINTF1("BIU2 Status   : %08lX\n\r", lptr->ae_BiuStatus[ 1 ]);
  PRINTF1("IRIG High     : %08lX\n\r", lptr->ul_IrigTimeHi);
  PRINTF1("IRIG Low      : %08lX\n\r", lptr->ul_IrigTimeLo);
#else 
  PRINTF1("SoftieID      : %08X\n\r", AIM_SH_RAM_ID);
  PRINTF1("GlobalRAMSize : %08lX\n\r", p_api_dev->GlobalRAMSize);
  PRINTF1("GlobalRAMBase : %08lX\n\r", p_api_dev->GlobalRAMBase);
  PRINTF1("BoardType     : %08lX\n\r", p_api_dev->BoardType);
  PRINTF1("BoardConfig   : %08lX\n\r", p_api_dev->BoardConfig);
  PRINTF1("BoardPartNo   : %08lX\n\r", p_api_dev->BoardPartNo);
  PRINTF1("BoardSerial   : %08lX\n\r", p_api_dev->BoardSerial);
  PRINTF1("SYSClk        : %08lX\n\r", p_api_dev->SYSClk);
  PRINTF1("CPUClk        : %08lX\n\r", p_api_dev->CPUClk);
#endif 

  PRINTF1("p_api_dev->chns=%d\n\r", p_api_dev->chns);
  PRINTF2("p_api_dev->chns1=%d p_api_dev->conf1=%d\n\r", p_api_dev->chns1, p_api_dev->conf1);
  PRINTF2("p_api_dev->chns2=%d p_api_dev->conf2=%d\n\r", p_api_dev->chns2, p_api_dev->conf2);
#endif /*#ifndef _AIM_VME*/

  /* calculate GRAM per channel */
  if(p_api_dev->chns1 != 0)
    map_multiplier1 = p_api_dev->conf1 / p_api_dev->chns1;
  if(p_api_dev->chns2 != 0)
    map_multiplier2 = p_api_dev->conf2 / p_api_dev->chns2;

  /* calculate real map multiplier */

  /* Note: analysis (23.11.2010) showed that a board with 1 channel and 1MB global memory
   *       needs 1008KB. So using a map multiplier of (p_api_dev->conf1 / p_api_dev->chns1)
   *       seems a good baseline. However the Monitor needs 512KB, Replay area needs 128KB.
   *       Both are not modified by map_multiplier. In other words a big part is not scalable.
   *       This means that if more than 1MB global memory is available per BIU,
   *       a higher map_multiplier can be used, than (p_api_dev->conf1 / p_api_dev->chns1)
   */
   /*       This seems to be sole the reason for the follwing four lines of code */
  if(map_multiplier1 > 1)
    map_multiplier1 *= 2;
  if(map_multiplier2 > 1)
    map_multiplier2 *= 2;


  /* restrict map_multiplier to max allowed value (e.g. for AMC this is 1!!) */
  /* Using a higher map_multiplier than MAX_MEM_MUL makes no sense, as the 
   * corresponding arrays use MAX_MEM_MUL, to calculate their sizes. */
  if(map_multiplier1 > MAX_MEM_MUL)
    map_multiplier1 = MAX_MEM_MUL;
  if(map_multiplier2 > MAX_MEM_MUL)
    map_multiplier2 = MAX_MEM_MUL;

  if (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIXUS)
  {
      /* We want 2MB Bus Monitor memory for AXCX1553. However the default mem multiplier produces a memory layout slightly to big. */
      if (map_multiplier1 > 1) map_multiplier1--;
      if (map_multiplier2 > 1) map_multiplier2--;
  }

#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
  PRINTF2("map_multiplier1=%ld  map_multiplier2=%ld\n\r", map_multiplier1, map_multiplier2);
#endif

  /* initialize memory mapping of memory bank 0, if 1553 */
  if( (0x00003000 != (p_api_dev->BoardType & 0x0000F000)) &&
      (0x00007000 != (p_api_dev->BoardType & 0x0000F000)) )
  {
    for( ul_Index=0; ul_Index<MAX_BIU; ul_Index++ )
    {
      ax_MemDef[ul_Index].rt_bh_area    = MAX_API_RT_HID * map_multiplier1;
      ax_MemDef[ul_Index].rt_sq_area    = MAX_API_DEFAULT_BUF_ID * map_multiplier1;
      ax_MemDef[ul_Index].rt_eq_area    = MAX_API_RT_EQ * map_multiplier1;
      ax_MemDef[ul_Index].bc_bh_area    = MAX_API_BC_HID * map_multiplier1;
      ax_MemDef[ul_Index].bc_sq_area    = MAX_API_DEFAULT_BUF_ID * map_multiplier1;
      ax_MemDef[ul_Index].bc_eq_area    = MAX_API_BC_EQ * map_multiplier1;
      ax_MemDef[ul_Index].bc_xfer_desc  = MAX_API_BC_XFER * map_multiplier1;
      ax_MemDef[ul_Index].bc_hip_instr  = MAX_API_BC_HIP_INSTR;
      ax_MemDef[ul_Index].bc_lip_instr  = MAX_API_BC_LIP_INSTR;
      ax_MemDef[ul_Index].bc_acyc_instr = MAX_API_BC_ACYC_INSTR;
      ax_MemDef[ul_Index].rep_buf       = API_REP_BUF_AREA_BYTE_SIZE;
      ax_MemDef[ul_Index].bm_buf        = mil_hw_get_bm_size(p_api_dev, 0 );
      ax_MemDef[ul_Index].sim_buf       = (MAX_API_DEFAULT_BUF_ID * MAX_API_BUF_SIZE) * map_multiplier1;
    }

    /* Define size of USB scatter list */
    ax_MemDef[0].UsbScatterListEntries = MAX_USB_SCATTERLIST_ENTRIES;

    enter_drv_set_mem_layout(p_api_dev, 0, 0, ax_MemDef );
  }

#ifdef _NUCLEUS
  /* Second memory bank is only applicable on AyX boards  */
  /* initialize memory mapping of memory bank 1, if 1553  */
  if( (0x30000000 != (p_api_dev->BoardType & 0xF0000000)) &&
      (0x70000000 != (p_api_dev->BoardType & 0xF0000000)) &&
      (0x00FF0000 != (p_api_dev->BoardType & 0x00FF0000)) )
  {
    for( ul_Index=0; ul_Index<MAX_BIU; ul_Index++ )
    {
      ax_MemDef[ul_Index].rt_bh_area    = MAX_API_RT_HID * map_multiplier2;
      ax_MemDef[ul_Index].rt_sq_area    = MAX_API_DEFAULT_BUF_ID * map_multiplier2;
      ax_MemDef[ul_Index].rt_eq_area    = MAX_API_RT_EQ * map_multiplier2;
      ax_MemDef[ul_Index].bc_bh_area    = MAX_API_BC_HID * map_multiplier2;
      ax_MemDef[ul_Index].bc_sq_area    = MAX_API_DEFAULT_BUF_ID * map_multiplier2;
      ax_MemDef[ul_Index].bc_eq_area    = MAX_API_BC_EQ * map_multiplier2;
      ax_MemDef[ul_Index].bc_xfer_desc  = MAX_API_BC_XFER * map_multiplier2;
      ax_MemDef[ul_Index].bc_hip_instr  = MAX_API_BC_HIP_INSTR;
      ax_MemDef[ul_Index].bc_lip_instr  = MAX_API_BC_LIP_INSTR;
      ax_MemDef[ul_Index].bc_acyc_instr = MAX_API_BC_ACYC_INSTR;
      ax_MemDef[ul_Index].rep_buf       = API_REP_BUF_AREA_BYTE_SIZE;
      ax_MemDef[ul_Index].bm_buf        =  mil_hw_get_bm_size( p_api_dev, 0 );
      ax_MemDef[ul_Index].sim_buf       = (MAX_API_DEFAULT_BUF_ID * MAX_API_BUF_SIZE) * map_multiplier2;
    }
    enter_drv_set_mem_layout(p_api_dev, 0, 1, ax_MemDef );
  }
#endif

#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
  enter_drv_dump_mem_layout(p_api_dev);
#endif

} /* end: enter_drv */


#if defined(_NUCLEUS)
static void v_PrintWelcome(TY_API_DEV *p_api_dev)
{
  PRINTF0("\n\r===============================================================================================\n\r");

  switch(p_api_dev->uc_DeviceType)
  {
    case TYPE_APX1553_1:                PRINTF0("APX1553-1 "); break;
    case TYPE_APX1553_2:                PRINTF0("APX1553-2 "); break;
    case TYPE_APX1553_4:                PRINTF0("APX1553-4 "); break;
    case TYPE_APX3910  :                PRINTF0("APX3910 "); break;
    case TYPE_APX3910Xp:                PRINTF0("APX3910Xp "); break;
    case TYPE_ACX1553_1:                PRINTF0("ACX1553-1 "); break;
    case TYPE_ACX1553_2:                PRINTF0("ACX1553-2 "); break;
    case TYPE_ACX1553_2_TWO_PBIS:       PRINTF0("ACX1553-2 (2 PBIs) "); break;
    case TYPE_ACX1553_4:                PRINTF0("ACX1553-4 "); break;
    case TYPE_ACX1553_4_TWO_PBIS:       PRINTF0("ACX1553-4 (2 PBIs) "); break;
    case TYPE_ACX1553_8:                PRINTF0("ACX1553-8 "); break;
    case TYPE_ACX3910  :                PRINTF0("ACX3910 "); break;
    case TYPE_ACX3910Xp:                PRINTF0("ACX3910Xp "); break;
    case TYPE_ACX3910_2  :              PRINTF0("ACX3910-2 "); break;
    case TYPE_ACX3910Xp_2:              PRINTF0("ACX3910Xp-2 "); break;

    case TYPE_ACX1553_4_DS_TWO_PBIS:    PRINTF0("ACX1553-4 "); break;   
    case TYPE_ACX_EFA_1_TWO_PBIS:       PRINTF0("ACX-EFA-1 "); break;      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:     PRINTF0("ACX-EFAXp-1 "); break;    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:    PRINTF0("ACX-EFA-1 "); break;   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:  PRINTF0("ACX-EFAXp-1 "); break; 
    case TYPE_ACX_EFA_2_TWO_PBIS:       PRINTF0("ACX-EFA-2 "); break;      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:     PRINTF0("ACX-EFAXp-2 "); break;   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:    PRINTF0("ACX-EFA-2 "); break;   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:  PRINTF0("ACX-EFAXp-2 "); break; 
    case TYPE_ACX_EFA_4_TWO_PBIS:       PRINTF0("ACX-EFA-4 "); break;      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:     PRINTF0("ACX-EFAXp-4 "); break;   

    case TYPE_AVX1553_1:                PRINTF0("AVX1553-1 "); break;
    case TYPE_AVX1553_2:                PRINTF0("AVX1553-2 "); break;
    case TYPE_AVX1553_2_TWO_PBIS:       PRINTF0("AVX1553-2 (2 PBIs) "); break;
    case TYPE_AVX1553_4:                PRINTF0("AVX1553-4 "); break;
    case TYPE_AVX1553_4_TWO_PBIS:       PRINTF0("AVX1553-4 (2 PBIs) "); break;
    case TYPE_AVX1553_8:                PRINTF0("AVX1553-8 "); break;
    case TYPE_AVX3910  :                PRINTF0("AVX3910 "); break;
    case TYPE_AVX3910Xp:                PRINTF0("AVX3910Xp "); break;
    case TYPE_AVX3910_2  :              PRINTF0("AVX3910-2 "); break;
    case TYPE_AVX3910Xp_2:              PRINTF0("AVX3910Xp-2 "); break;
    case TYPE_AVX1553_4_DS_TWO_PBIS:    PRINTF0("AVX1553-4 "); break;   
    case TYPE_AVX_EFA_1_TWO_PBIS:       PRINTF0("AVX-EFA-1 "); break;  
    case TYPE_AVX_EFAXp_1_TWO_PBIS:     PRINTF0("AVX-EFAXp-1 "); break;
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:    PRINTF0("AVX-EFA-1 "); break;  
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:  PRINTF0("AVX-EFAXp-1 "); break;
    case TYPE_AVX_EFA_2_TWO_PBIS:       PRINTF0("AVX-EFA-2 "); break;  
    case TYPE_AVX_EFAXp_2_TWO_PBIS:     PRINTF0("AVX-EFAXp-2 "); break;
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:    PRINTF0("AVX-EFA-2 "); break;  
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:  PRINTF0("AVX-EFAXp-2 "); break;
    case TYPE_AVX_EFA_4_TWO_PBIS:       PRINTF0("AVX-EFA-4 "); break;  
    case TYPE_AVX_EFAXp_4_TWO_PBIS:     PRINTF0("AVX-EFAXp-4 "); break;
  }

  if (mil_hw_scope_type_get(p_api_dev) != API_MILSCOPE_TYPE_NONE)
    PRINTF0("MilScope TARGET-SW ");
  else
    PRINTF0("TARGET-SW ");

  PRINTF0(MIL_TSW_VERSION_STRING_FULL);
  PRINTF0(MIL_TSW_BUILD_DATE);

  PRINTF2("(c)AIM GmbH <%s %s>\n\r", __DATE__, __TIME__);
  PRINTF0("===============================================================================================\n\r\n\r");
  PRINTF0("Press <SPACE> followed by <M> to list the monitor commands.\n\r");
  PRINTF0("Press <SPACE> followed by <T> to list the TSW commands.\n\r");
}
#endif /*_NUCLEUS*/

#ifdef _NUCLEUS
  /* AyX Boards can have two PBIs. 
     The board type is separated into the upper 16bits for PBI2 and the lower 16bits for PBI1 
  */
  Error: no longer implemented. _NUCLEUS support will be removed in the future.
#else
/* All board_type BIUs are expected to be on the same PBI */
void api_main_initialize_stream_type( TY_API_DEV * p_api_dev )
{
    p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_3 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_4 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_5 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_6 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_7 - 1] = API_TYPE_NONE;
    p_api_dev->auc_StreamType[BIU_8 - 1] = API_TYPE_NONE;

    p_api_dev->ul_NbOfStreams1           = 0;
    p_api_dev->ul_NbOfStreams2           = 0;


    switch( MIL_BOARD_TYPE(p_api_dev) & 0x0000F0F0 )
    {
    case 0x0000F010:
        p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1           = 1;
        break;
    case 0x0000F0A0:
    case 0x00001010:
        p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1           = 2;
        break;
    case 0x0000A0A0:
        p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_3 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_4 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1           = 4;
        break;
    case 0x000010A0:
    case 0x0000A010:
        p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[BIU_3 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1           = 3;
        break;
    case 0x00003010:
    case 0x00007010:
        p_api_dev->auc_StreamType[BIU_1 - 1] = API_TYPE_1553; 
        p_api_dev->auc_StreamType[BIU_2 - 1] = API_TYPE_3910; 
        p_api_dev->ul_NbOfStreams1           = 2;
        break;
    default:
        break;
    }

    switch( MIL_BOARD_TYPE(p_api_dev) & 0xF0F00000 )
    {
    case 0xF0100000: 
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1 += 1; 
        break;
    case 0xF0A00000:  
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1 += 2; 
        break;
    case 0x10100000:
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1 += 2; 
        break;
    case 0xA0A00000: 
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_3 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_4 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1 += 4; 
        break;
    case 0x10A00000: 
    case 0xA0100000: 
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_2 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_3 - 1] = API_TYPE_1553;
        p_api_dev->ul_NbOfStreams1 += 3; 
        break;
    case 0x30100000: 
    case 0x70100000: 
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_1 - 1] = API_TYPE_1553;
        p_api_dev->auc_StreamType[p_api_dev->ul_NbOfStreams1 + BIU_2 - 1] = API_TYPE_3910;
        p_api_dev->ul_NbOfStreams1 += 2; 
        break;
    default: 
        break;
    }

}
#endif

void api_main_setup_memory_layout( TY_API_DEV * p_api_dev )
{
    /* if at least one stream is 3910/EFEX, we have to initialize it */
    if( (API_TYPE_3910 == p_api_dev->auc_StreamType[BIU_2 - 1]) ||
        (API_TYPE_3910 == p_api_dev->auc_StreamType[BIU_3 - 1]) ||
        (API_TYPE_3910 == p_api_dev->auc_StreamType[BIU_4 - 1]) ||
        (API_TYPE_3910 == p_api_dev->auc_StreamType[BIU_6 - 1]) )
     /* 3910/EFEX */
      enter_drv3910(p_api_dev);

    /* if at least one stream is pure 1553, we have to initialize it */
    if( ((API_TYPE_3910 != p_api_dev->auc_StreamType[BIU_2 - 1]) &&
         (API_TYPE_1553 == p_api_dev->auc_StreamType[BIU_1 - 1])) ||
        ((API_TYPE_3910 != p_api_dev->auc_StreamType[BIU_3 - 1]) &&
         (API_TYPE_1553 == p_api_dev->auc_StreamType[BIU_2 - 1])) ||
        ((API_TYPE_3910 != p_api_dev->auc_StreamType[BIU_4 - 1]) &&
         (API_TYPE_1553 == p_api_dev->auc_StreamType[BIU_3 - 1])) ||
        ((API_TYPE_3910 != p_api_dev->auc_StreamType[BIU_6 - 1]) &&
         (API_TYPE_1553 == p_api_dev->auc_StreamType[BIU_5 - 1])) )
      enter_drv(p_api_dev);
}


void api_main_init_and_reset( TY_API_DEV * p_api_dev )
{
    short i;
    L_WORD retVal;
    short ulPossibleHsBiu[]    = { BIU_2-1, BIU_4-1, BIU_6-1 };
    short ulPossibleHsStream[] = { 0,       1,       1       };
    short ulPossibleHsCount    = 3;


    api_ini_startup(p_api_dev);


    PRINTF0("Initialize LS area (all BIUs)...");

    /* -- Set all BIUs initially to an enabled state --- */

    for( i = 0; i < p_api_dev->chns; i++ )
    {
        p_api_dev->b_BiuEnabled[i] = TRUE;
    }

    /* -- Initialize LS part --- */

    p_api_dev->biu       = 0;
    p_api_dev->hs_stream = 0;

    retVal = api_ini(p_api_dev, 2);

    if(retVal != 0)
    {
        PRINTF0("FAILED\n\r");
        return;
    }

    PRINTF0("OK\n\r");


    /* -- Initialize HS part --- */

    for( i=0; i<ulPossibleHsCount; i++ )
    {
        p_api_dev->biu       = ulPossibleHsBiu[i];
        p_api_dev->hs_stream = ulPossibleHsStream[i];

        if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
        {
            PRINTF2("Initialize HS area (BIU%d, HS-STREAM%d)...",p_api_dev->biu, p_api_dev->hs_stream);
            
            retVal = api39_ini(p_api_dev, 2);
            
            (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, API39_IR_LOG_ADDR); /* Interrupt Loglist Pointer */
            p_api_dev->islp[ p_api_dev->biu ]               = API39_IR_LOG_ADDR;
            
            PRINTF2("islp(%d)=%08X...",p_api_dev->biu,p_api_dev->islp[ p_api_dev->biu ]);

            if(retVal == 0)
                PRINTF0("OK\n\r");
            else
                PRINTF0("FAILED\n\r");
        }

        p_api_dev->biu = 0;
    }



    /*
    Reset all the streams to initialize the CB areas
    This ensures that all the streams are operational after bootup.
    */
    for( i=0; i<p_api_dev->chns; i++ )
    {
        p_api_dev->biu = i;

        if( (p_api_dev->biu + 1) > p_api_dev->ul_NbOfStreams1 )
            p_api_dev->hs_stream = API39_HS_BIU_2;
        else
            p_api_dev->hs_stream = API39_HS_BIU_1;

        api_sys_reset(p_api_dev, 0);

        if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
        {
            api39_sys_reset(p_api_dev, 0, 0);
            /* HS biu already reset skip it */
            i++; 
        }
    }



    mil_fw_initialize_lookup();
}


void api_main_free_allocated_memory( TY_API_DEV * p_api_dev )
{
    short i, j, k = 0;

    mil_tasks_info_free(p_api_dev);

    /* -- clean up the malloc mess --- */

    /* Free RT setup systags */
    for( i=0; i<MAX_BIU; i++ )
    {
        for( j=0; j<MAX_API_RT; j++ )
        {
            for( k=0; k<MAX_API_RT_XMT_SA; k++ )
            {
                if( p_api_dev->rt_setup[i].rt_systag[j].sa[k].p != NULL )
                {
                    FREE( p_api_dev->rt_setup[i].rt_systag[j].sa[k].p );
                    p_api_dev->rt_setup[i].rt_systag[j].sa[k].p = NULL;
                }
            }
        }
    }


    /* Free BC setup systags */
    for( i=0; i<MAX_BIU; i++ )
    {
        for( j=0; j<(MAX_API_BC_XFER * MAX_MEM_MUL); j++ )
        {
            if( p_api_dev->bc_setup[i].bc_systag[j].p != NULL )
            {
                FREE( p_api_dev->bc_setup[i].bc_systag[j].p );
                p_api_dev->bc_setup[i].bc_systag[j].p = NULL;
            }
        }
    }

    /* Free BM dyntag lookup table */
    for( i=0; i<MAX_BIU; i++ )
    {
        for( j=0; j<MAX_API_RT; j++ )
        {
            for( k=0; k<MAX_API_RT_XMT_SA; k++ )
            {
                if( p_api_dev->bm_ls_dyntagm_lut[i].rt[j].sa[k].p != NULL )
                {
                    FREE( p_api_dev->bm_ls_dyntagm_lut[i].rt[j].sa[k].p );
                    p_api_dev->bm_ls_dyntagm_lut[i].rt[j].sa[k].p = NULL;
                }
            }
        }
    }


    /* Free BC service request vector word descriptor  */
    for( i=0; i<MAX_BIU; i++ )
    {
        if( p_api_dev->bc_srvw_xid_dsp[i].px_Xid != NULL )
        {
            FREE( p_api_dev->bc_srvw_xid_dsp[i].px_Xid );
            p_api_dev->bc_srvw_xid_dsp[i].px_Xid = NULL;
        }
    }

    /* Free HS RT setup systags */
    for( i=0; i<API39_MAX_STREAM; i++ )
    {
        for( j=0; j<MAX_API39_RT; j++ )
        {
            for( k=0; k<MAX_APIEF_RT_XMT_MID; k++ )
            {
                if( ax_api39_rt_setup[i].rt_systag[j].mid[k].p != NULL )
                {
                    FREE( ax_api39_rt_setup[i].rt_systag[j].mid[k].p );
                    ax_api39_rt_setup[i].rt_systag[j].mid[k].p = NULL;
                }
            }
        }
    }


    /* Free HS BC setup systags */
    for( i=0; i<API39_MAX_STREAM; i++ )
    {
        for( j=0; j<(MAX_API39_BC_XFER *MAX_MEM_MUL); j++ )
        {
            if( ax_api39_bc_setup[i].bc_systag[j].p != NULL )
            {
                FREE( ax_api39_bc_setup[i].bc_systag[j].p );
                ax_api39_bc_setup[i].bc_systag[j].p = NULL;
            }
        }
    }

    /* Free HS dyntag monitor lookup table */
    for( i=0; i<API39_MAX_STREAM; i++ )
    {
        for( j=0; j<MAX_API39_RT; j++ )
        {
            for( k=0; k<(MAX_APIEF_RT_RCV_MID + MAX_APIEF_RT_XMT_MID); k++ )
            {
                if( api39_hs_dyntagm_lut[i].rt[j].mid[k].p != NULL )
                {
                    FREE( api39_hs_dyntagm_lut[i].rt[j].mid[k].p );
                    api39_hs_dyntagm_lut[i].rt[j].mid[k].p = NULL;
                }
            }
        }
    }



    /* Free Track */
    for( i=0; i<MAX_BIU; i++ )
    {
        p_api_dev->biu       = i;
        p_api_dev->hs_stream = 0;
        api_sys_track_init( p_api_dev, 0, 1, 1 ); /* Disable LS track for BC and RT */
        api_sys_track_init( p_api_dev, 1, 1, 1 ); /* Disable HS track for BC and RT */
    }

    for (i=0; i<API39_MAX_STREAM; i++ )
    {
        p_api_dev->hs_stream = i;
        api_sys_track_init(p_api_dev, 1, 1, 1); /* Disable HS track for BC and RT */
    }


#ifdef API_LOCAL
    /* Free data queue pointers */
    /* Dataqueues are only allocated on AyI                */
    /* AyX uses fixed memory locations for the data queues */
    for( i=0; i<(API_MAX_DATA_QUEUE_ID+1); i++ )
    {
        if( p_api_dev->dataqueue[i].DestOffset != NULL )
        {
            FREE( p_api_dev->dataqueue[i].DestOffset );
            p_api_dev->dataqueue[i].DestOffset = NULL;
        }
    }

    ClearHeap(p_api_dev);
#endif /* API_LOCAL */




}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_MAIN                 Submodule : MAIN                      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.02.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
int Mymain(void);

#if defined(_NUCLEUS)
/* AyX, AMC-T => Mymain() called by vt_Mil() */
int Mymain(void)
{
  L_WORD st;
  TY_API_HW_INIT_PARAMS tsw_config;

  memset( &tsw_config, 0, sizeof(TY_API_HW_INIT_PARAMS) );

  px_api_dev = MALLOC(sizeof(TY_API_DEV));

  /*
  The NOVRAM values are read within enter_hw()
  AyX              => Application_Initialize(void*)
  AMC without ASP  => system driver
  USB              => system driver
  AyE              => SOC MyMon

  The NOVRAM values are read within v_GetBoardHardwareInfo()
  AMC-ASP          => Application_Initialize(void*)

  */

  do
  {
    /* -- Initialize TSW based on blobal monitor variables --- */
    mil_hw_initialize_tsw( px_api_dev, &tsw_config /* unused */ );
   
    
    /* -- Print info if RS232 is available --- */

    PRINTF0("BIU Boot finished. \n\r");

    v_PrintWelcome(px_api_dev);

    /* -- start TSW command processing loop --- */
    
    /* This function will only return if 
       p_api_dev->repeat is set to API_OFF */

    st = api_drv(px_api_dev, 0);

    /* -- Free allocated memory and prepare for restart --- */
    mil_hw_shutdown_tsw( px_api_dev );

  }while( st==0 );



  FREE(px_api_dev);
  px_api_dev = NULL;
 
  
  return (0);
} /* end: main */
#endif



#ifdef _AIM_1553_SYSDRV_USB
    int isTswLibUsbDefinePresent(void){ return 1; }
#else
    int isTswLibUsbDefineNotPresent(void) { return 1; }
#endif



NAMESPACE1553_END

