/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_ini.c
    Initialisaion module of the TSW
*/

#include "mil_tsw_includes.h"
#include "api_int_loglist.h"


NAMESPACE1553_START





TY_MIL_COM_TABLE_ENTRY axTargetSysIniCommands[] = {
  {MIL_COM_SYS_INI,      (TY_MIL_COM_FUNC_PTR)api_ini_cmd,        "api_ini_cmd",        sizeof(TY_MIL_COM_SYS_INI_INPUT),   sizeof(TY_MIL_COM_SYS_INI_OUTPUT)   }
};


void api_sys_ini_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysIniCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysIniCommands;
}





#ifdef CMPL4SIM

/* checks if a stream is multichannel */
L_WORD ul_StreamIsMultiChannel( TY_API_DEV *p_api_dev, WORD biu )
{
    L_WORD biu_config;

    L_WORD phy_biu = api_sys_convert_biu_logical_to_physical( p_api_dev, biu );

    biu_config = (p_api_dev->BoardType >> (phy_biu*8) ) & 0x000000F0L;

    if( biu_config == 0xA0L )
        return 1;
    else
        return 0;
}

/* checks if a stream is configured to 3910 */
L_WORD ul_StreamIs3910( TY_API_DEV *p_api_dev, WORD uw_Biu )
{
  L_WORD ul_RetVal = 0;
 
  if( (uw_Biu+1) <= (WORD)p_api_dev->ul_NbOfStreams1 )
  {
    if( (p_api_dev->BoardType & 0x0000F000) == 0x00003000 )
      ul_RetVal = 1;
  }
  else
  {
    if( (p_api_dev->BoardType & 0xF0000000) == 0x30000000 )
      ul_RetVal = 1;
  }

  return( ul_RetVal );
}


/* checks if a stream is configured to 3910Xp */
L_WORD ul_StreamIsEFEX( TY_API_DEV *p_api_dev, WORD uw_Biu )
{
  L_WORD ul_RetVal = 0;
 
  if( (uw_Biu+1) <= (WORD)p_api_dev->ul_NbOfStreams1 )
  {
    if( (p_api_dev->BoardType & 0x0000F000) == 0x00007000 )
      ul_RetVal = 1;
  }
  else
  {
    if( (p_api_dev->BoardType & 0xF0000000) == 0x70000000 )
      ul_RetVal = 1;
  }

  return( ul_RetVal );
}


/* checks if a stream is configured to 3910 or 3910Xp */
L_WORD ul_StreamIsHS( TY_API_DEV *p_api_dev, WORD uw_Biu )
{
  L_WORD ul_RetVal = 0;

  if( ul_StreamIs3910(p_api_dev, uw_Biu) || ul_StreamIsEFEX(p_api_dev, uw_Biu) )
    ul_RetVal = 1;

  return( ul_RetVal );
}


/* translates a relative into an absolute address */
#if defined(_NUCLEUS)
L_WORD get_global_ram_addr_abs (TY_API_DEV *p_api_dev, L_WORD addr, BYTE biu)
{
  L_WORD ul_GlobalRamStartOffset = 0;
  mil_hw_get_GlobalRamStartOffset( p_api_dev, biu, &ul_GlobalRamStartOffset ) ;

  /* The proper way would be to add the GlobalRAMBase and ul_GlobalRamStartOffset. However this might be a different behaviour. 
   * So I left it the old way - to be on the safe side */
  return (addr | ((L_WORD)p_api_dev->GlobalRAMBase + ul_GlobalRamStartOffset) );
}
#endif


/* translates an absolute into an relative address */
L_WORD get_global_ram_addr_rel (TY_API_DEV *p_api_dev, L_WORD addr, BYTE biu)
{
  L_WORD ul_GlobalRamStartOffset = 0;
  mil_hw_get_GlobalRamStartOffset( p_api_dev, biu, &ul_GlobalRamStartOffset ) ;
  
  /* protection against accidental multiple use of this function */
  if( addr >= ((ptrdiff_t)p_api_dev->GlobalRAMBase + (ptrdiff_t)ul_GlobalRamStartOffset) ) 
  {
      addr = TRUNCATE_64BIT_TO32BIT((ptrdiff_t) addr - (ptrdiff_t)p_api_dev->GlobalRAMBase - (ptrdiff_t)ul_GlobalRamStartOffset);
  }
  else
  {
    /* this might be a good place to set a breakpoint, when cleaning up */
  }

  return(addr);
}


/* translates an offset written into the Global RAM into a pointer ready to be used by the TSW */
L_WORD ul_ConvertGRAMPtrToTSWPtr(TY_API_DEV *p_api_dev, WORD uw_Biu, L_WORD ul_Value)
{
  L_WORD ul_GlobalRamStartOffset = 0;
  mil_hw_get_GlobalRamStartOffset( p_api_dev, uw_Biu, &ul_GlobalRamStartOffset ) ;

  return BSWAP32(ul_Value + ul_GlobalRamStartOffset);
}


/* translates a pointer of the TSW into an offset that can written into the Global RAM */
L_WORD ul_ConvertTSWPtrToGRAMPtr(TY_API_DEV *p_api_dev, WORD uw_Biu, L_WORD ul_Value)
{
  L_WORD ul_GlobalRamStartOffset = 0;
  mil_hw_get_GlobalRamStartOffset( p_api_dev, uw_Biu, &ul_GlobalRamStartOffset ) ;

  /* The proper way would be to subtract ul_GlobalRamStartOffset. However this might be a different behaviour. 
   * So I left it the old way - to be on the safe side */
  return BSWAP32(ul_Value & ~ul_GlobalRamStartOffset);
}




BYTE uc_CalcNumberOfPhysicalBius(TY_API_DEV *p_api_dev)
{
    int i;
    BYTE physical_biu_count = 0;

    for( i=0; i<MAX_PHY_BIU; i++ )
    {
      switch( (p_api_dev->BoardType >> (i*8) ) & 0x000000F0L)
      {
        case 0x00000010:
        case 0x00000030:
        case 0x00000070:
        case 0x000000A0:
            physical_biu_count += 1;
            break;
        default:
          break;
      }
    }


    return physical_biu_count;
}


BYTE uc_CalcNumberOfStreams(TY_API_DEV *p_api_dev)
{
    int i;
    BYTE stream_count = 0;

    for( i=0; i<MAX_PHY_BIU; i++ )
    {
      switch( (p_api_dev->BoardType >> (i*8) ) & 0x000000F0L)
      {
        case 0x00000010:
        	stream_count += 1;
            break;
        case 0x00000030:
        case 0x00000070:
            // ls and hs biu form 1 stream
            break;
        case 0x000000A0:
        	stream_count += 2;
            break;
        default:
          break;
      }
    }


    return stream_count;
}

/* calculates the number of streams */
BYTE uc_CalcNumberOfChannels(TY_API_DEV *p_api_dev)
{
  BYTE uc_NumberOfChannels = 0;
  switch(p_api_dev->BoardType & 0x000000F0L)
  {
    case 0x00000010:
      uc_NumberOfChannels = 1;
      break;
    case 0x000000A0:
      uc_NumberOfChannels = 2;
      break;
  }

  switch(p_api_dev->BoardType & 0x0000F000L)
  {
    case 0x00001000:
    case 0x00003000:
    case 0x00007000:
      uc_NumberOfChannels++;
      break;
    case 0x0000A000:
      uc_NumberOfChannels += 2;
      break;
  }

  switch(p_api_dev->BoardType & 0x00F00000L)
  {
    case 0x00100000:
      uc_NumberOfChannels++;
      break;
    case 0x00A00000:
      uc_NumberOfChannels += 2;
      break;
  }

  switch(p_api_dev->BoardType & 0xF0000000L)
  {
    case 0x10000000:
    case 0x30000000:
    case 0x70000000:
      uc_NumberOfChannels++;
      break;
    case 0xA0000000:
      uc_NumberOfChannels += 2;
      break;
  }

  return( uc_NumberOfChannels );
}


/* returns the device type from board type and board config */
BYTE uc_GetDeviceType(TY_API_DEV *p_api_dev)
{
  BYTE uc_DeviceType = 0;
  switch(p_api_dev->BoardConfig & 0x000000FF)
  {
    case AI_PLATFORM_CPCIX_3U:
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFF11 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFF1111 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFFFF6) == 0xFFFFFF16 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF8) == 0xFFFFFF18 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF6F1) == 0xFFFF1611 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F1) == 0xFFFF1811 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF6F6) == 0xFFFF1616 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F8) == 0xFFFF1818 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFFA1 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFFA1A1 ) uc_DeviceType = TYPE_ACX1553_4;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF3011 ) uc_DeviceType = TYPE_ACX3910;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF7011 ) uc_DeviceType = TYPE_ACX3910Xp;
      break;
    case AI_PLATFORM_CPCIX_6U:
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFF11 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFFA1 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFF1111 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFFFF6) == 0xFFFFFF16 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF8) == 0xFFFFFF18 ) uc_DeviceType = TYPE_ACX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF6F1) == 0xFFFF1611 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F1) == 0xFFFF1811 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF6F6) == 0xFFFF1616 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F8) == 0xFFFF1818 ) uc_DeviceType = TYPE_ACX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFFA1A1 ) uc_DeviceType = TYPE_ACX1553_4;
      if( (p_api_dev->BoardType & 0xF1F1F1F1) == 0x11111111 ) uc_DeviceType = TYPE_ACX1553_4_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xF1F1F1F1) == 0xA1A1A1A1 ) uc_DeviceType = TYPE_ACX1553_8;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF3011 ) uc_DeviceType = TYPE_ACX3910;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF7011 ) uc_DeviceType = TYPE_ACX3910Xp;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x30113011 ) uc_DeviceType = TYPE_ACX3910_2;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x70117011 ) uc_DeviceType = TYPE_ACX3910Xp_2;
      if( (p_api_dev->BoardType & 0xF1F1F6F6) == 0x11111616 ) uc_DeviceType = TYPE_ACX1553_4_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xF1F1F8F8) == 0x11111818 ) uc_DeviceType = TYPE_ACX1553_4_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x3011FF11 ) uc_DeviceType = TYPE_ACX_EFA_1_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x7011FF11 ) uc_DeviceType = TYPE_ACX_EFAXp_1_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF6) == 0x3011FF16 ) uc_DeviceType = TYPE_ACX_EFA_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF8) == 0x3011FF18 ) uc_DeviceType = TYPE_ACX_EFA_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF6) == 0x7011FF16 ) uc_DeviceType = TYPE_ACX_EFAXp_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF8) == 0x7011FF18 ) uc_DeviceType = TYPE_ACX_EFAXp_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x30111111 ) uc_DeviceType = TYPE_ACX_EFA_2_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x70111111 ) uc_DeviceType = TYPE_ACX_EFAXp_2_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F6F6) == 0x30111616 ) uc_DeviceType = TYPE_ACX_EFA_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F8F8) == 0x30111818 ) uc_DeviceType = TYPE_ACX_EFA_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F6F6) == 0x70111616 ) uc_DeviceType = TYPE_ACX_EFAXp_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F8F8) == 0x70111818 ) uc_DeviceType = TYPE_ACX_EFAXp_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x3011A1A1 ) uc_DeviceType = TYPE_ACX_EFA_4_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x7011A1A1 ) uc_DeviceType = TYPE_ACX_EFAXp_4_TWO_PBIS;
      break;
    case AI_PLATFORM_VMEX_B:
    case AI_PLATFORM_VMEX_A:
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFF11 ) uc_DeviceType = TYPE_AVX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFFFFA1 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFF1111 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFFFF6) == 0xFFFFFF16 ) uc_DeviceType = TYPE_AVX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF8) == 0xFFFFFF18 ) uc_DeviceType = TYPE_AVX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF6F1) == 0xFFFF1611 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F1) == 0xFFFF1811 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF6F6) == 0xFFFF1616 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F8) == 0xFFFF1818 ) uc_DeviceType = TYPE_AVX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF1F1) == 0xFFFFA1A1 ) uc_DeviceType = TYPE_AVX1553_4;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF3011 ) uc_DeviceType = TYPE_AVX3910;
      if( (p_api_dev->BoardType & 0xFFFFFFF1) == 0xFFFF7011 ) uc_DeviceType = TYPE_AVX3910Xp;
      if( (p_api_dev->BoardType & 0xF1F1F1F1) == 0x11111111 ) uc_DeviceType = TYPE_AVX1553_4_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xF1F1F1F1) == 0xA1A1A1A1 ) uc_DeviceType = TYPE_AVX1553_8;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x30113011 ) uc_DeviceType = TYPE_AVX3910_2;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x70117011 ) uc_DeviceType = TYPE_AVX3910Xp_2;
      if( (p_api_dev->BoardType & 0xF1F1F6F6) == 0x11111616 ) uc_DeviceType = TYPE_AVX1553_4_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xF1F1F8F8) == 0x11111818 ) uc_DeviceType = TYPE_AVX1553_4_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x3011FF11 ) uc_DeviceType = TYPE_AVX_EFA_1_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF1) == 0x7011FF11 ) uc_DeviceType = TYPE_AVX_EFAXp_1_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF6) == 0x3011FF16 ) uc_DeviceType = TYPE_AVX_EFA_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF8) == 0x3011FF18 ) uc_DeviceType = TYPE_AVX_EFA_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF6) == 0x7011FF16 ) uc_DeviceType = TYPE_AVX_EFAXp_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1FFF8) == 0x7011FF18 ) uc_DeviceType = TYPE_AVX_EFAXp_1_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x30111111 ) uc_DeviceType = TYPE_AVX_EFA_2_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x70111111 ) uc_DeviceType = TYPE_AVX_EFAXp_2_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F6F6) == 0x30111616 ) uc_DeviceType = TYPE_AVX_EFA_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F8F8) == 0x30111818 ) uc_DeviceType = TYPE_AVX_EFA_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F6F6) == 0x70111616 ) uc_DeviceType = TYPE_AVX_EFAXp_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F8F8) == 0x70111818 ) uc_DeviceType = TYPE_AVX_EFAXp_2_DS_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x3011A1A1 ) uc_DeviceType = TYPE_AVX_EFA_4_TWO_PBIS;
      if( (p_api_dev->BoardType & 0xFFF1F1F1) == 0x7011A1A1 ) uc_DeviceType = TYPE_AVX_EFAXp_4_TWO_PBIS;
      break;
    case AI_PLATFORM_PCIX:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_APX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_APX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFFFF6) == 0xFFFFFF16 ) uc_DeviceType = TYPE_APX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF8) == 0xFFFFFF18 ) uc_DeviceType = TYPE_APX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF6F1) == 0xFFFF1611 ) uc_DeviceType = TYPE_APX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F1) == 0xFFFF1811 ) uc_DeviceType = TYPE_APX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF6F6) == 0xFFFF1616 ) uc_DeviceType = TYPE_APX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF8F8) == 0xFFFF1818 ) uc_DeviceType = TYPE_APX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_APX1553_4;
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFF3010 ) uc_DeviceType = TYPE_APX3910;
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFF7010 ) uc_DeviceType = TYPE_APX3910Xp;
      break;
    case AI_PLATFORM_PCIE_PCIX_BASED:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFF3010 ) uc_DeviceType = TYPE_APX3910;
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFF7010 ) uc_DeviceType = TYPE_APX3910Xp;
        break;
    case AI_PLATFORM_USB:
        if((p_api_dev->BoardType & 0xFF) == 0x10      /* Full function */
            || (p_api_dev->BoardType & 0xFF) == 0x12  /* Simulator only */
            || (p_api_dev->BoardType & 0xFF) == 0x14) /* Single function */
        {
            uc_DeviceType = TYPE_APU1553_1;
        }
        else if((p_api_dev->BoardType & 0xFF) == 0xA0      /* Full function */
                 || (p_api_dev->BoardType & 0xFF) == 0xA2  /* Simulator only */
                 || (p_api_dev->BoardType & 0xFF) == 0xA4) /* Single function */
        {
            uc_DeviceType = TYPE_APU1553_2;
        }
        break;
    case AI_PLATFORM_PCIE_CARD:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_AEC1553_1;
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFFA0 ) uc_DeviceType = TYPE_AEC1553_2;
      break;
    case AI_PLATFORM_MINI_PCIE_CARD:
    case AI_PLATFORM_MINI_PCIE_CARD_AP:
        if ((p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10) uc_DeviceType = TYPE_AME1553_1;
        break;
    case AI_PLATFORM_MINI_PCIE_ARTIX7:
        if ((p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10) uc_DeviceType = TYPE_AMEE1553_1;
        if ((p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010) uc_DeviceType = TYPE_AMEE1553_2;
        break;
    case AI_PLATFORM_M2_B_M_ARTIX7:
        if ((p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10) uc_DeviceType = TYPE_AM2E1553_1;
        if ((p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010) uc_DeviceType = TYPE_AM2E1553_2;
        break;
    case AI_PLATFORM_XMC_ARTIXUS:
        if ((p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10) uc_DeviceType = TYPE_AXCX1553_1;
        if ((p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010) uc_DeviceType = TYPE_AXCX1553_2;
        if ((p_api_dev->BoardType & 0xF0F0F0F0) == 0x10101010) uc_DeviceType = TYPE_AXCX1553_4;
        break;
    case AI_PLATFORM_XMC:
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_AXC1553_1;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_AXC1553_2;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_AXC1553_4;
        break;
    case AI_PLATFORM_XMC_EBD:
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_AXE1553_1;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_AXE1553_2;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_AXE1553_4;
        break;
    case AI_PLATFORM_XMC_NOREPLAY_NOERROR:
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_ASE1553_1;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_ASE1553_2;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_ASE1553_4;
        break;
    case AI_PLATFORM_PMC_XMC_BASED:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_AMCX1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_AMCX1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_AMCX1553_4;
      break;
    case AI_PLATFORM_PMC_BASED_EBD:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_AMCE1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_AMCE1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_AMCE1553_4;
      break;
    case AI_PLATFORM_PCIE:
    case AI_PLATFORM_PCIX_PCIE_BASED:
    case AI_PLATFORM_CPCIE_3U:
    case AI_PLATFORM_CPCIX_3U_PCIE_BASED:
      if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_APE1553_1;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010 ) uc_DeviceType = TYPE_APE1553_2;
      if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFFA0A0 ) uc_DeviceType = TYPE_APE1553_4;
      break;
    case AI_PLATFORM_ANET:
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_ANET1553_1;
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFFA0 ) uc_DeviceType = TYPE_ANET1553_2;
        break;
    case AI_PLATFORM_ANET_AYS:
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF3010 ) uc_DeviceType = TYPE_ANET3910;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF7010 ) uc_DeviceType = TYPE_ANET3910Xp;
        if( (p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_ANET1553_1;
        break;
    case AI_PLATFORM_ASC:
    case AI_PLATFORM_USB3:
        if ((p_api_dev->BoardType & 0xFFFFFFF0) == 0xFFFFFF10) uc_DeviceType = TYPE_ASC1553_1;
        if ((p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF1010) uc_DeviceType = TYPE_ASC1553_2;
        break;
    case AI_PLATFORM_ANET_AYS_MA:
        if( (MIL_BOARD_TYPE(p_api_dev) & 0xFFFFFFF0) == 0xFFFFFF10 ) uc_DeviceType = TYPE_ANET1553_1;
        if( (MIL_BOARD_TYPE(p_api_dev) & 0xFFFFFFF0) == 0xFFFFFFA0 ) uc_DeviceType = TYPE_ANET1553_2;
        break;
    case AI_PLATFORM_PCI_SHORT_ZYNQMP:
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF3010 ) uc_DeviceType = TYPE_APXX3910;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF7010 ) uc_DeviceType = TYPE_APXX3910Xp;
        break;
    case AI_PLATFORM_PCI_E_1L_ZYNQMP:
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF3010 ) uc_DeviceType = TYPE_APEX3910;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF7010 ) uc_DeviceType = TYPE_APEX3910Xp;
        break;
    case AI_PLATFORM_CPCIE_3U_ZYNQMP:
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF3010 ) uc_DeviceType = TYPE_ACE3910;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF7010 ) uc_DeviceType = TYPE_ACE3910Xp;
        break;
    case AI_PLATFORM_XMC_ZYNQMP:
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF3010 ) uc_DeviceType = TYPE_AXC3910;
        if( (p_api_dev->BoardType & 0xFFFFF0F0) == 0xFFFF7010 ) uc_DeviceType = TYPE_AXC3910Xp;
        break;

    default:
      PRINTF2("Did not recognize Board Type!!! (CFG=%08X, TYP=%08X)\r\n",p_api_dev->BoardConfig,p_api_dev->BoardType);
      break;
  }

#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
  PRINTF3("Device Type is %d! (CFG=%08X, TYP=%08X)\r\n",uc_DeviceType,p_api_dev->BoardConfig,p_api_dev->BoardType);
#endif

  return( uc_DeviceType );
}

AiUInt32 hasElectricalInterface(TY_API_DEV *p_api_dev, short ls_biu)
{
    AiUInt32 ssw;
    short hs_biu = ls_biu + 1;

    if(! ul_StreamIsHS(p_api_dev, hs_biu))
        return 0;

    ssw = BSWAP32( p_api_dev->cb_p[hs_biu]->ssw);

    if( ((ssw >> 16) & 0x7) == 0x02 )
        return 1;
    else
        return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_BC_GLB_VARS       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the BC global variables.                     */
/*                                                                           */
/*****************************************************************************/

void api_ini_bc_glb_vars(TY_API_DEV *p_api_dev)
{
short i, j, k, l;
L_WORD ul_Hid;

  p_api_dev->bc_bh_def.bid = 0;
  p_api_dev->bc_bh_def.sid = 0;
  p_api_dev->bc_bh_def.eid = 0;
  p_api_dev->bc_bh_def.nbufs = 0;
  p_api_dev->bc_bh_def.hid_addr = 0L;
  p_api_dev->bc_bh_def.bid_addr = 0L;
  p_api_dev->bc_bh_def.sid_addr = 0L;
  p_api_dev->bc_bh_def.eid_addr = 0L;

  p_api_dev->bc_xfer_dsp.cw1 = 0x0000;
  p_api_dev->bc_xfer_dsp.st1 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cw2 = 0x0000;
  p_api_dev->bc_xfer_dsp.st2 = 0xFFFF;
  p_api_dev->bc_xfer_dsp.cur_bid = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_brw = 0x0000;
  p_api_dev->bc_xfer_dsp.cur_buf = 0L;
  p_api_dev->bc_xfer_dsp.cur_ttag = 0L;
  p_api_dev->bc_xfer_dsp.msg_cnt = 0L;
  p_api_dev->bc_xfer_dsp.err_cnt = 0L;

  for(i = 0; i < MAX_BIU; i++)
  {
    p_api_dev->bc_trg_status[ i ] = API_HALT;
    p_api_dev->bc_dbc_start_status[ i ] = API_HALT;
    p_api_dev->bc_dbc_stop_status[ i ]  = API_HALT;
    p_api_dev->bc_dbc_stop_xfctrl_save[ i ] = 0;
    p_api_dev->bc_dbc_stop_xfswm_save[ i ] = 0;
    p_api_dev->bc_status[ i ] = API_HALT;
    for( ul_Hid=0; ul_Hid<(MAX_API_BC_HID * MAX_MEM_MUL); ul_Hid++ )
      p_api_dev->bc_dytag_status[i][ul_Hid] = API_OFF;

    /* BC ISR Flag */
    p_api_dev->bc_ir_status[ i ] = API_OFF;

    p_api_dev->bc_status_dsp[ i ].status = API_BC_STATUS_HALT;
    p_api_dev->bc_status_dsp[ i ].hxfer = 0;

    p_api_dev->bc_status_cnt[ i ].msg_cnt = 0L;
    p_api_dev->bc_status_cnt[ i ].err_cnt = 0L;
    p_api_dev->bc_status_cnt[ i ].hip = 0L; 
    p_api_dev->bc_status_cnt[ i ].mfc = 0L; 
    p_api_dev->bc_status_cnt[ i ].res1 = 0L;
    p_api_dev->bc_status_cnt[ i ].res2 = 0L;

    p_api_dev->bc_setup[ i ].smod = 0;
    p_api_dev->bc_setup[ i ].cnt = 0;
    p_api_dev->bc_setup[ i ].ftime = 0L;
    p_api_dev->bc_setup[ i ].sinst = 0L; 
    p_api_dev->bc_setup[ i ].bc_triggered_inst = 0L;

    /* Minor Frame Setup */
    for(j = 0; j < MAX_API_BC_FRAME; j++)
    {
      p_api_dev->bc_setup[ i ].frame[ j ].cnt = 0x0000;
      for(k = 0; k < MAX_API_BC_XFER_IN_FRAME; k++)
        p_api_dev->bc_setup[ i ].frame[ j ].instr[ k ] = 0L;
    }

    /* Major Frame Setup */
    p_api_dev->bc_setup[ i ].mframe.cnt = 0x00;
    for(j = 0; j < MAX_API_BC_FRAME_IN_MFRAME; j++)
      p_api_dev->bc_setup[ i ].mframe.fr[ j ] = 0x00;

    /* Support Firmware BC Dynamic Data */
    for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area; j++)
    {
      p_api_dev->bc_setup[ i ].ls_dytag[ j ].ena = API_OFF;
      p_api_dev->bc_setup[ i ].ls_dytag[ j ].mode = 0;
    }

    /* System Dynamic Data */
    for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; j++)
    {
      p_api_dev->bc_setup[ i ].bc_systag[ j ].id = 0;
      p_api_dev->bc_setup[ i ].bc_systag[ j ].cnt = 0;
      p_api_dev->bc_setup[ i ].bc_systag[ j ].p = NULL;
    }

    /* Service Request and Vector Word handling */
    p_api_dev->bc_srvw_xid_dsp[ i ].px_Xid = NULL;
    p_api_dev->bc_srvw_con[ i ] = API_OFF;
    for(j = 0; j < MAX_API_BC_SRVW_RT; j++)
    {
      p_api_dev->bc_srvw_rt_dsp[ i ].rt[ j ].xid = 0x0000;
      p_api_dev->bc_srvw_rt_dsp[ i ].rt[ j ].lvw = 0x0000;
      p_api_dev->bc_srvw_rt_dsp[ i ].rt[ j ].src = 0L;
    }

    for(j = 0; j < MAX_API_BC_SRVW_ID; j++)
    {
      for(k = 0; k < MAX_API_BC_SRVW_RT; k++)
      {
        for(l = 0; l < MAX_API_BC_SRVW_SAMID; l++)
        {
          p_api_dev->bc_srvw_tab[ i ].t[ j ].ta[ k ].sa_mid[ l ].xid = 0;
        }
      }
    }

    p_api_dev->bc_srvw_subframe[ i ].cnt = 0;
    for(k = 0; k < MAX_API_BC_XFER_IN_FRAME; k++)
      p_api_dev->bc_srvw_subframe[ i ].instr[ k ] = 0L;
  }

} /* end: api_ini_bc_glb_vars */
#endif 

#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_RT_GLB_VARS       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the RT global variables.                     */
/*                                                                           */
/*****************************************************************************/

void api_ini_rt_glb_vars(TY_API_DEV *p_api_dev)
{
short i, j, rt;
L_WORD ul_Hid;

  for(i = 0; i < MAX_BIU; i++)
  {
    p_api_dev->rt_status[ i ] = API_HALT;
    p_api_dev->single_rt_addr[ i ] = 0;

    /* RT ISR Flag */
    for(rt = 0; rt < MAX_API_RT; rt++)
      p_api_dev->rt_ir_status[ i][ rt ] = API_OFF;
  }

  /* Initialize RT Dynamic Tagging Slots */
  for(i = 0; i < MAX_BIU; i++)
  {

    /* RT INIT Mode */
    for(rt = 0; rt < MAX_API_RT; rt++)
    {
      p_api_dev->rt_setup[ i ].rt_con[ rt ] = API_RT_DISABLE;
      for(ul_Hid = 0; ul_Hid < (MAX_API_RT_HID * MAX_MEM_MUL); ul_Hid++)
        p_api_dev->rt_dytag_status[ i ][ ul_Hid ] = API_OFF; /* disable all bits for all 32 RTs */
    }
    /* RT DYTAG SA Descriptor Allocation Area */
    for(rt = 0; rt < MAX_API_RT; rt++)
    {
      for(j = 0; j < MAX_API_RT_XMT_SA; j++)
      {
        p_api_dev->rt_setup[ i ].rt_dytag[ rt ].sa[ j ] = 0;
        p_api_dev->rt_setup[ i ].rt_queue[ rt ].sa[ j ] = 0; 
      }
    }

    /* Support Firmware RT Dynamic Data */
    for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_eq_area; j++)
    {
      p_api_dev->rt_setup[ i ].ls_dytag[ j ].ena = API_OFF;
      p_api_dev->rt_setup[ i ].ls_dytag[ j ].mode = 0;
    }

    /* System Dynamic Data */
    for(rt = 0; rt < MAX_API_RT; rt++)
    {
      for(j = 0; j < MAX_API_RT_XMT_SA; j++)
      {
        p_api_dev->rt_setup[ i ].rt_systag[ rt ].sa[ j ].id = 0;
        p_api_dev->rt_setup[ i ].rt_systag[ rt ].sa[ j ].cnt = 0;
        p_api_dev->rt_setup[ i ].rt_systag[ rt ].sa[ j ].p = NULL;
      }
    }
  }

} /* end: api_ini_rt_glb_vars */
#endif 

#ifdef CMPL4MON
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_BM_GLB_VARS       */
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
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the BM global variables.                     */
/*                                                                           */
/*****************************************************************************/

void api_ini_bm_glb_vars(TY_API_DEV *p_api_dev)
{
short i;

  /* Activity Page */
  p_api_dev->bm_rt_act_dsp.fnd = API_OFF;
  p_api_dev->bm_rt_act_dsp.rt = 0;
  p_api_dev->bm_rt_act_dsp.tr = 0;
  p_api_dev->bm_rt_act_dsp.sa = 0;
  p_api_dev->bm_rt_act_dsp.cc = 0L; /* Message Counter */
  p_api_dev->bm_rt_act_dsp.ec = 0L; /* Error Counter */
  p_api_dev->bm_rt_act_dsp.et = 0L; /* Error Type */



  for(i = 0; i < MAX_BIU; i++)
  {
    p_api_dev->bm_status_cnt[ i ].msg_cnt = 0L;
    p_api_dev->bm_status_cnt[ i ].err_cnt = 0L;

    p_api_dev->bm_status_dsp[ i ] = API_BM_STATUS_HALT;
    p_api_dev->bm_status[ i ] = API_HALT;

    /* BM ISR Flag */
    p_api_dev->bm_ir_status[ i ] = API_OFF;

    /* Recording */
    p_api_dev->bm_rec[ i ].mode = API_OFF;
    p_api_dev->bm_rec[ i ].hfi_org = 0L;
    p_api_dev->bm_rec[ i ].hfi_cnt = 0L;
    p_api_dev->bm_rec[ i ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
    p_api_dev->bm_rec[ i ].size = 0L;
  }

} /* end: api_ini_bm_glb_vars */
#endif 

#ifdef CMPL4MON
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                Submodule : API_INI_BM_DYNTAGM_GLB_VARS */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the LS BM Dyntag Monitor global variables.   */
/*                                                                           */
/*****************************************************************************/

void api_ini_bm_dyntagm_glb_vars(TY_API_DEV *p_api_dev)
{
L_WORD i,j,k;

  for(i=0; i<MAX_BIU; i++) {

    /* Dyntag Monitor RT-SA Lookup Table */
    for(j=0; j<MAX_API_RT; j++) {
      for(k=0; k<MAX_API_RT_SA; k++) {
        p_api_dev->bm_ls_dyntagm_lut[i].rt[j].sa[k].cnt = 0L;
        p_api_dev->bm_ls_dyntagm_lut[i].rt[j].sa[k].p   = NULL;
      }
    }    
    
    /* Dyntag Monitor Setup Table */
    for(j=0; j<MAX_API_BM_DYNTAG_MON_ID; j++) {
    
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].con         = API_BM_DYNTAG_MON_DIS;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].rt          = 0;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].sa          = 0;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].sa_type     = 0;
        
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].res         = 0;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].dyntag_type = 0;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].dyntag_wpos = 0;
        
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].stale_cnt   = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].bad_cnt     = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].good_cnt    = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].dw          = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].state       = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].stale_cnt_contig = 0L;
        p_api_dev->bm_ls_dyntagm_set[i].dyntag[j].good_cnt_contig = 0L;
    }    
  }

} /* end: api_ini_bm_dyntagm_glb_vars */
#endif 

#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_REP_GLB_VARS      */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.08.99   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the REP global variables.                    */
/*                                                                           */
/*****************************************************************************/

void api_ini_rep_glb_vars(TY_API_DEV *p_api_dev)
{

  short i;

  for(i = 0; i < MAX_BIU; i++)
  {
    p_api_dev->rep_status[ i ] = API_REP_STOP;

    p_api_dev->sim_rep[ i ].mode = API_OFF;
    p_api_dev->sim_rep[ i ].rpi_cnt = 0L;
    p_api_dev->sim_rep[ i ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf;
    p_api_dev->sim_rep[ i ].size = 0L;
    p_api_dev->sim_rep[ i ].entry_cnt = 0L;
    p_api_dev->sim_rep[ i ].flag = API_OFF;
  }

} /* end: api_ini_rep_glb_vars */
#endif 

#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_BC_QUEUE_GLB_VARS */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the BC Queue global variables.               */
/*                                                                           */
/*****************************************************************************/

void api_ini_bc_queue_glb_vars(TY_API_DEV *p_api_dev)
{
  short i, j;

  /*** Init FIFO Queue ***/
  for(i = 0; i < MAX_BIU; i++)
  {

    p_api_dev->queue_setup[ i ].max_queue = 0;
    p_api_dev->queue_setup[ i ].max_size = 0;

    for(j = 0; j < MAX_API_QUEUE_SIZE; j++)
    {
      p_api_dev->queue_setup[ i ].queue[ j ].mode = 0;
      p_api_dev->queue_setup[ i ].queue[ j ].saddr = 0L;
      p_api_dev->queue_setup[ i ].queue[ j ].cur_buf_addr = 0L;
      p_api_dev->queue_setup[ i ].queue[ j ].half_buf_addr = 0L;
      p_api_dev->queue_setup[ i ].queue[ j ].xid = 0;
      p_api_dev->queue_setup[ i ].queue[ j ].rt = 0;
      p_api_dev->queue_setup[ i ].queue[ j ].sa = 0;
      p_api_dev->queue_setup[ i ].queue[ j ].flag = 0;
      p_api_dev->queue_setup[ i ].queue[ j ].buf_cnt = 0;
    }
  }
} /* end: api_ini_bc_queue_glb_vars */
#endif 

#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                 Submodule : API_INI_LS_SYSTAG_GLB_VARS */
/*                                                                           */
/*   Author : Marco Motz              Project   : API1553-DS                 */
/*                                                                           */
/*   Source : C                       Tools     : PC/AT; Norton Editor;      */
/*                                                CYGNUS, GNU-C, As, and LD  */
/*                                                IDT-C 5.1 Toolkit          */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function initialises the LS System Dynamic Data structures.       */
/*                                                                           */
/*****************************************************************************/

void api_ini_ls_systag_glb_vars(TY_API_DEV *p_api_dev)
{
  short i, j;

  for(i = 0; i < MAX_BIU; i++)
  {
    for(j = 0; j < MAX_API_SYS_LS_SYSTAGS; j++)
    {
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].mode = API_SYSTAG_DIS;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].run_mode = API_SYSTAG_RUN_DISABLED;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].bc_rt = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].xid_rtsa = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].fct = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].min = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].max = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].step = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].wpos = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].bpos = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].nbit = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].mask = 0;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].init = API_OFF;
      p_api_dev->ls_systag_setup[ i ].dytag[ j ].index = 0;
    }
  }

} /* end: api_ini_ls_systag_glb_vars */
#endif 



/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI_STARTUP           */
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
/*    This function performs the initalization of the Host-to-Target Shared  */
/*    Memory area.                                                           */
/*                                                                           */
/*****************************************************************************/

void api_ini_startup(TY_API_DEV *p_api_dev)
{
    int i;

    mil_tasks_info_init(p_api_dev);

    if (p_api_dev->targ_com_reg_p != NULL)
    {
        p_api_dev->targ_com_reg_p->ul_OsBit             = 0L;
        p_api_dev->targ_com_reg_p->res                  = 0L;
        p_api_dev->targ_com_reg_p->tg_control           = 0L;
        p_api_dev->targ_com_reg_p->tg_status            = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaControl        = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaStatus         = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaMemType        = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaSourcePtr      = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaDestinationPtr = 0L;
        p_api_dev->targ_com_reg_p->ul_DmaSize           = 0L;


        for (i = 0; i < MAX_API_CMD_SIZE; i++)
        {
            *(p_api_dev->cmd_p + i) = 0x00;
            *(p_api_dev->ack_p + i) = 0x00;
        }
    }
} /* end: api_ini_startup */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_INI                  Submodule : API_INI                   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.05.09                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Status Indication [status]                                 */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_INI' instruction to initialize the      */
/*    API1553-DS ASP Driver Software.                                        */
/*                                                                           */
/*****************************************************************************/
L_WORD api_ini_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_INI_INPUT * in, TY_MIL_COM_SYS_INI_OUTPUT * out )
{
#ifdef _API3910    
    WORD   i       = 0;
#endif    
    L_WORD ret_val = 0;

    ret_val = api_ini(p_api_dev, (BYTE)in->mode);

    if( ret_val != 0 )
        return ret_val;

#ifdef _API3910
    p_api_dev->hs_stream = 0;

    for( i=0; i<MAX_BIU; i++ )
    {
        if( ul_StreamIsHS( p_api_dev, i) )
        {
            p_api_dev->biu = i + 1; /* Use HS biu */

            ret_val = api39_ini(p_api_dev, (BYTE)in->mode);

            if( ret_val != 0 )
                break;

            (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, API39_IR_LOG_ADDR); /* Interrupt Loglist Pointer */
            p_api_dev->islp[ p_api_dev->biu ] = API39_IR_LOG_ADDR;

            /* Skip one BIU because ul_StreamIsHS returns true for LS and HS part */
            i++; 
            /* We have a maximum of two HS streams / Do not use ++ to avoid array index out of range */
            p_api_dev->hs_stream = 1;
        }
    }
#endif

    out->bt[0] = p_api_dev->ini_info.bt[0];
    out->bt[1] = p_api_dev->ini_info.bt[1];
    out->bt[2] = p_api_dev->ini_info.bt2[0];
    out->bt[3] = p_api_dev->ini_info.bt2[1];

    out->streams          = p_api_dev->ini_info.streams;
    out->prot             = p_api_dev->ini_info.prot;
    out->pbi_id_biu1      = p_api_dev->ini_info.pbi_id_biu1;
    out->asp_mon_id       = p_api_dev->ini_info.asp_mon_id;
    out->asp_bite_id      = p_api_dev->ini_info.asp_bite_id;
    out->pbi_id_biu2      = p_api_dev->ini_info.pbi_id_biu2;

    out->board_config     = p_api_dev->ini_info.board_config;
    out->glb_mem_size     = p_api_dev->ini_info.glb_mem_size;
    out->loc_dram_size    = p_api_dev->ini_info.loc_dram_size;
    out->shared_dram_size = p_api_dev->ini_info.shared_dram_size;

    out->board_type       = p_api_dev->ini_info.board_type;
    out->board_sub_type   = p_api_dev->ini_info.board_sub_type;
    out->hardware_variant = p_api_dev->ini_info.hardware_variant;

    return ret_val;
}


BYTE api_ini_legacy(TY_API_DEV *p_api_dev, BYTE mode)
{
    BYTE   ackfl   = API_ACK2;
    L_WORD ret_val = 0;

    ret_val = api_ini(p_api_dev, mode);

    if( ret_val != 0 )
    {
        ackfl = API_NAK;
        SET_NAK_INFO(ret_val);
    }

    p_api_dev->b_length = 5+2; 
    p_api_dev->w_length = 4;
    p_api_dev->lw_length = 9 + 18 + 3;
    p_api_dev->ack_b_ptr = &p_api_dev->ini_info.bt[ 0 ]; 
    p_api_dev->ack_w_ptr = &p_api_dev->ini_info.pbi_id_biu1;
    p_api_dev->ack_lw_ptr = &p_api_dev->ini_info.board_config;

    return ackfl;
}

L_WORD api_ini(TY_API_DEV *p_api_dev, BYTE mode)
{
  L_WORD lval = 0;
  short i, j;
  L_WORD ul_Temp;

  p_api_dev->uc_SRecMarkFound = 0;
  p_api_dev->uc_BmIniClearBuffer = 1; /* by default, clear BM buffer when calling ApiCmdBmIni */
  
#ifdef _CMPL4S1I1553
  /* Init Action Tabel Area in Local RAM area*/
  s1i_ini_action_table_area();
#endif 
  
  if (API_MILSCOPE_TYPE_AYX == mil_hw_scope_type_get(p_api_dev)) /* if we have an AyX MilScope */
  {
    /* Internal Arbitration Control Register - IACR (INTEL80321 Folder, Page 499, 633) */
    ul_Temp = (*((volatile UINT32*)(0xFFFFE780)));
    ul_Temp &= 0xFFFF3984; /* clear all priorities */
    ul_Temp |= 0x00008411; /* set PBI=Low, XScale=Med, DMA1 (used for MilSCope)=High, DMA0=Med, ATU=Med */
    (*((volatile UINT32*)(0xFFFFE780))) = ul_Temp;
    /*PRINTF0("Changed XScale priorities because MilScope was found!\r\n");*/
  }

  if( mode == 2 )
  {
    p_api_dev->reset_can_be_done = TRUE;

    /* Init global variables */
    p_api_dev->b_length = 0;
    p_api_dev->w_length = 0;
    p_api_dev->lw_length = 0;

    /* Set to OK with RESET command */
    for(i = 0; i < MAX_BIU; i++)
      p_api_dev->biu_active[ i ] = API_ERR;

    /* Interrupt Handler */
    for(i = 0; i < MAX_BIU; i++)
      p_api_dev->islp[ i ] = p_api_dev->glb_mem.biu[ i ].ir_log_addr;

    /* Temporary Arrays */
    for(i = 0; i < MAX_API_BDATA_SIZE; i++)
      p_api_dev->byte_data_arr[ i ] = 0;
    for(i = 0; i < MAX_API_WDATA_SIZE; i++)
      p_api_dev->word_data_arr[ i ] = 0;
    for(i = 0; i < MAX_API_LDATA_SIZE; i++)
      p_api_dev->lword_data_arr[ i ] = 0;

    /* Init SIM global variables */
#ifdef CMPL4SIM
    api_ini_bc_glb_vars(p_api_dev);
    api_ini_rt_glb_vars(p_api_dev);
    api_ini_rep_glb_vars(p_api_dev);
    api_ini_bc_queue_glb_vars(p_api_dev);
    api_ini_ls_systag_glb_vars(p_api_dev);
#endif 

    /* Init MON global variables */
#ifdef CMPL4MON
    api_ini_bm_glb_vars(p_api_dev);
    api_ini_bm_dyntagm_glb_vars(p_api_dev); /* 11.11.05 */
#endif 
  }

  /* System Init Info */
  p_api_dev->ini_info.bt[ 0 ]  = (BYTE)p_api_dev->drv_mode[ 0 ];
  p_api_dev->ini_info.bt[ 1 ]  = (BYTE)p_api_dev->drv_mode[ 1 ];
  p_api_dev->ini_info.bt2[ 0 ] = (BYTE)p_api_dev->drv_mode[ 2 ];
  p_api_dev->ini_info.bt2[ 1 ] = (BYTE)p_api_dev->drv_mode[ 3 ];
  p_api_dev->ini_info.streams  = (BYTE)p_api_dev->chns;
  p_api_dev->ini_info.prot = 1; /* 1553 */
  p_api_dev->ini_info.emod = 0; /* All AIM boards are little endian */
  p_api_dev->ini_info.irg = 1; /* IRIG impl */
  p_api_dev->ini_info.board_type       = (L_WORD)p_api_dev->BoardType;
  p_api_dev->ini_info.board_sub_type   = (L_WORD)p_api_dev->BoardSubType;
  p_api_dev->ini_info.hardware_variant = (L_WORD)p_api_dev->HwVariant;


#ifdef _AMC1553
  p_api_dev->ini_info.pbi_id_biu1 = 0;
  p_api_dev->ini_info.pbi_id_biu2 = 0;
#else 

  #ifndef _NUCLEUS
  p_api_dev->ini_info.pbi_id_biu1 = ( (MEM(GLBRAM_BP + BIU_VERSION_O) &0x00ff0000) >> 16);
  p_api_dev->ini_info.pbi_id_biu2 = ( (MEM(GLBRAM_BP + BIU_VERSION_O + BIU_ID_O) &0x00ff0000) >> 16);
  #endif 

#endif 

  p_api_dev->ini_info.res1 = 0;

#ifdef MON_VERSION
  p_api_dev->ini_info.asp_mon_id = MON_VERSION; 
#endif 

  p_api_dev->ini_info.asp_bite_id  = 0;
  p_api_dev->ini_info.board_config = p_api_dev->BoardConfig;

  /* Check NOVRAM settings API3910 Board */
  if( ul_StreamIs3910( p_api_dev, BIU_1-1 ) )
    p_api_dev->ini_info.prot = 2; /* 3910 protocol */
  else if( ul_StreamIsEFEX( p_api_dev, BIU_1-1 ) )
    p_api_dev->ini_info.prot = 3; /* EFEX protocol */
  else
    p_api_dev->ini_info.prot = 1; /* 1553 */

  /* if first PBI is a 1553 PBI look also at second PBI, if it is a 3910/EFEX PBI in order to get the information, */
  /* if it is a combined module. */
  /* The first PBI on combined modules is ALWAYS a 1553 PBI */
  if( 1 == p_api_dev->ini_info.prot )
  {
    if( ul_StreamIs3910( p_api_dev, p_api_dev->ul_NbOfStreams1 + BIU_1-1 ) )
      p_api_dev->ini_info.prot = 4; /* combined 1553 + 3910 protocol */
    else if( ul_StreamIsEFEX( p_api_dev, p_api_dev->ul_NbOfStreams1 + BIU_1-1 ) )
      p_api_dev->ini_info.prot = 5; /* combined 1553 + EFEX protocol */
  }

  p_api_dev->ini_info.streams = (BYTE)p_api_dev->chns;
  if( ul_StreamIsHS( p_api_dev, p_api_dev->ul_NbOfStreams1 + BIU_1-1))
    p_api_dev->ini_info.streams--; /* STANAG3910 or EFEX */
  if( ul_StreamIsHS( p_api_dev, BIU_1-1))
    p_api_dev->ini_info.streams--; /* STANAG3910 or EFEX */


  /* The physical addresses can not be accessed by the host. 
     64 bit addresses do not fit into 32bit L_WORDs
  */
  p_api_dev->ini_info.glb_mem_addr     = 0;
  p_api_dev->ini_info.loc_dram_addr    = 0;
  p_api_dev->ini_info.shared_dram_addr = 0;
  p_api_dev->ini_info.flash_ram_addr   = 0; 


  /* Set sizes */
  p_api_dev->ini_info.glb_mem_size     = (p_api_dev->GlobalRAMSize + p_api_dev->GlobalRAMSize2) / 0x10000L;
  p_api_dev->ini_info.loc_dram_size    = p_api_dev->LocalRAMSize  / 0x10000L;
  p_api_dev->ini_info.shared_dram_size = p_api_dev->SharedRAMSize / 0x10000L;
  p_api_dev->ini_info.flash_ram_size   = 0;


  /* PCI Header Info */
  for(i = 0, j = 0; i < 16; i++, j += 4)
  {
    lval = 0;
    p_api_dev->ini_info.pci[ i ] = lval;
  }

  if( mode == 2 )
  {

    /*** Init Interrupt Loglist pointer in shared RAM ***/

    p_api_dev->intr_list_p = (struct ty_api_intr_event_list*) (API_LOLI_MEM_START_ADDR);
    /* Init the pointer to interrupt loglist in the relative pointer Array */

    /* BIU Interrupt Loglist Events */
    /* Interrupt Loglist structure changed */
    p_api_dev->intr_list_p->put_cnt = 0L;
    p_api_dev->intr_list_p->get_cnt = 0L;
    for(j = 0; j < MAX_API_IR_EVENTS; j++)
    {
      p_api_dev->intr_list_p->entry[ j ].lla = 0xFFFFFFFFL;
      p_api_dev->intr_list_p->entry[ j ].llb = 0xFFFFFFFFL;
      p_api_dev->intr_list_p->entry[ j ].llc.ul_All = 0xFFFFFFFFL;
      p_api_dev->intr_list_p->entry[ j ].lld.ul_All = 0xFFFFFFFFL;
    } 

    /* API Timer Interrupt Events */
    for(j = 0; j < MAX_API_TIMER; j++)
    {
      p_api_dev->timer_intr[ j ].tcnt = 0L;
      p_api_dev->timer_intr[ j ].tena = 0L;
      p_api_dev->timer_intr[ j ].predev = 0xFFFFFFFFL;
      p_api_dev->timer_intr[ j ].isr_cnt = 0L;
    }

    /* System Memory Info */
    for(i = 0; i < MAX_BIU; i++)
    {
      p_api_dev->sys_mem[ i ].mbufs = 0x00;
      p_api_dev->sys_mem[ i ].sbufs = 0x00;
      p_api_dev->sys_mem[ i ].maddr = 0L;
      p_api_dev->sys_mem[ i ].saddr = 0L;
      p_api_dev->sys_mem[ i ].fw_vers = 0x0000;
      p_api_dev->sys_mem[ i ].sw_vers = 0x0000;
      p_api_dev->sys_mem[ i ].lca_id = 0x0000;
      p_api_dev->sys_mem[ i ].lca_chks = 0L;
    }

    /* Selftest */
    for(i = 0; i < MAX_BIU; i++)
    {
      p_api_dev->bite_stream[ i ] = API_OFF;
    }


    /******************************************/
    /*  Variables for 1760 DTS System         */
    /******************************************/
#ifdef _CMPL4DTS
     for (j=0; j<MAX_IP_DRV; j++) {
   
        ip_drv[j].used = API_OFF;
        ip_drv[j].ip_type = API_OFF;
        ip_drv[j].p_dhw_init_func = 0;
        ip_drv[j].p_dout_write_func = 0;
        ip_drv[j].p_dout_read_func = 0;
        ip_drv[j].p_din_read_func = 0;
      
        /* IP Access Setup */
        ip_drv[j].ip_setup.din.model   = 0L;
        ip_drv[j].ip_setup.din.ip_addr = 0L;
        ip_drv[j].ip_setup.din.sh_addr = 0L;
        ip_drv[j].ip_setup.din.data    = 0L;

        ip_drv[j].ip_setup.dout.model   = 0L;
        ip_drv[j].ip_setup.dout.ip_addr = 0L;
        ip_drv[j].ip_setup.dout.sh_addr = 0L;
        ip_drv[j].ip_setup.dout.data    = 0L;
       
        /* IP Sampling Task Setup */
        ip_drv[j].stask_setup.ena     = API_OFF;
        ip_drv[j].stask_setup.isr_cnt = 0L;
        ip_drv[j].stask_setup.rate    = 0L;
        ip_drv[j].stask_setup.res     = 0L;
        ip_drv[j].stask_setup.ustep   = 0L;

        /* Recording Status */
        ip_drv[j].rec_status.rec_con    = API_OFF;
        ip_drv[j].rec_status.rec_status = API_HALT;
        ip_drv[j].rec_status.status     = 0L;
        ip_drv[j].rec_status.wr_addr    = 0L;
        ip_drv[j].rec_status.rd_addr    = 0L;
        ip_drv[j].rec_status.entry_cnt  = 0L;
     }
#endif


    /* Init Pointer to API System Control BLock and Interrupt Loglist */
    for(i = 0; i < MAX_BIU; i++)
    {
      p_api_dev->cb_p[ i ] = (TY_API_SYS_CB*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].cb_addr);
      p_api_dev->islp_p[ i ] = (TY_API_ISLP*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].ir_log_addr);
    }

    /* Init Pointer to API Monitor Area */
#ifdef CMPL4SIM
    for(i = 0; i < MAX_BIU; i++)
    {
      p_api_dev->bm_tcb_desc_p[ i ] = (TY_API_BM_TCB_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].bm_tcb_addr);
    }
#endif 

    /* Init Pointer to API Simulator Area */
#ifdef CMPL4SIM
    for(i = 0; i < MAX_BIU; i++)
    {
      p_api_dev->rt_desc_p[ i ] = (TY_API_RT_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].rt_desc_addr);
      p_api_dev->rt_sa_desc_p[ i ] = (TY_API_RT_SA_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.rt_sa_desc);
      p_api_dev->rt_bh_desc_p[ i ] = (TY_API_RT_BH_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.rt_bh_area);
      p_api_dev->rt_sq_desc_p[ i ] = (TY_API_RT_SQ_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.rt_sq_area);
      p_api_dev->rt_eq_desc_p[ i ] = (TY_API_RT_EQ_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.rt_eq_area);
      p_api_dev->bc_bh_desc_p[ i ] = (TY_API_BC_BH_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_bh_area);
      p_api_dev->bc_sq_desc_p[ i ] = (TY_API_BC_SQ_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_sq_area);
      p_api_dev->bc_eq_desc_p[ i ] = (TY_API_BC_EQ_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_eq_area);
      p_api_dev->bc_xfer_desc_p[ i ] = (TY_API_BC_XFER_DESC*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_xfer_desc);
      p_api_dev->bc_hip_instr_p[ i ] = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_hip_instr);
      p_api_dev->bc_lip_instr_p[ i ] = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_lip_instr);
      p_api_dev->bc_acyc_instr_p[ i ] = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ i ].base.bc_acyc_instr);
    }
#endif 


    /* api_ini_startup(p_api_dev); */
	
    /* Init the Track Structures */
    memset(p_api_dev->ls_track, 0, sizeof(p_api_dev->ls_track) );
    memset(p_api_dev->hs_track, 0, sizeof(p_api_dev->hs_track) );
  }

#if defined(_NUCLEUS) /* MilScope functions for APX cards */
  if( API_MILSCOPE_TYPE_AYX == mil_hw_scope_type_get( p_api_dev ) ) 
  {
    mscope_start( p_api_dev, MSCOPE_STOP );
  }
#endif  
  
  

  return 0;
} /* end: api_ini */

NAMESPACE1553_END

