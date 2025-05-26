/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_bite.c
Built-In-Selftest (BITE) module of the AYI1553 ASP Driver SW. This module 
contains the selftest functions.
*/

#include "mil_tsw_includes.h"


#define BITE_OK    0
#define BITE_ERROR 1

#define BITE_GLB_MEM     0
#define BITE_SHARED_RAM  1

/*#define CLK_1US 150*/
/*#define CLK_1MS 150000*/                                    
/*#define MAX_API_BITE_WAIT (1000*CLK_1US)*/

#define BITE_BUS_A 0
#define BITE_BUS_B 1

#define BITE_CPL_WRAP 0
#define BITE_CPL_ISO  1
#define BITE_CPL_TRF  2

NAMESPACE1553_START





TY_MIL_COM_TABLE_ENTRY axTargetSysBiteCommands[] = {
  {MIL_COM_SYS_BITE,      (TY_MIL_COM_FUNC_PTR)api_bite,        "api_bite",        sizeof(TY_MIL_COM_SYS_BITE_INPUT),   sizeof(TY_MIL_COM_SYS_BITE_OUTPUT)   }
};


void api_sys_bite_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysBiteCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysBiteCommands;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BOARD_EN_TEST    */
/*                                                                           */
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : status indication                                          */
/*                                                                           */
/*    Description :                                                          */
/*    This function enables simulator and monitor operation and tests        */
/*    the response in the Status Word registers.                             */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_board_en_test(TY_API_DEV *p_api_dev)
{
  L_WORD i    = 0;
  L_WORD stat = 0; 

  stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
  wbflush(); /* System Status Word */

  /* Check if BIU is enabled */
  if( (stat &API_SSW_OPE_BIT) == 0L)
  {
    /* Enable BIU Operation */
    ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);

    /* Check in Status Register */
    for(i = 0; i < p_api_dev->clk_1us *1000 /*MAX_API_BITE_WAIT*/; i++)
    {
      stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
      wbflush();
      if( (stat &API_SSW_OPE_BIT) == API_SSW_OPE_BIT)
      {
        break;
      }
      CALL_SCHEDULER(); /* in case it takes long give CPU away */
    }
    if(i == p_api_dev->clk_1us *1000 /*MAX_API_BITE_WAIT*/)
    {
       /* 12.02.99 */
      PRINTF0("Board Enable timeout\n\r");
      return (1); /* BIU Time Out Error */
    }
  }

  /* Monitor */
  /* clear high word in Status Register */
  stat &= 0x0000FFFF;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw = BSWAP32(stat);
  wbflush();

  /* Enable MON Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BM);

  /* Check in Status Register */
  for(i = 0; i < (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/); i++)
  {
     /* 12.02.99 */
    stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
    wbflush();
    if( (stat &API_SSW_MNE_BIT) == API_SSW_MNE_BIT)
    {
      /*PRINTF1("MNE Bit set: %08x\n\r",stat);*/
      break;
    }
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }
  if(i == (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/) )
  {
     /* 12.02.99 */
    PRINTF0("MNE timeout\n\r");
    return (2); /* Time Out Error */
  }

  /* Disable MON Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BM);
 

  /* Simulator */

  /* BC Test */
  /* clear high word in Status Register */
  stat &= 0x0000FFFF;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw = BSWAP32(stat);
  wbflush();

  /* Enable BC Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BC);

  /* Check in Status Register */

  for(i = 0; i < (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/); i++)
  {
     /* 12.02.99 */
    stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
    wbflush();
    if( (stat &API_SSW_BCE_BIT) == API_SSW_BCE_BIT)
    {
      /*PRINTF1("BCE Bit set: %08x\n\r",stat);*/
      break;
    }
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }
  if(i == (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/) )
  {
     /* 12.02.99 */
    PRINTF0("BCE timeout\n\r");
    PRINTF1("i = %d\n\r", i);
    PRINTF1("Status: %08x\n\r", stat);
    return (3); /* Time Out Error */
  }

  /* Disable BC Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BC);


  /* RT Test */
  /* clear high word in Status Register */
  stat &= 0x0000FFFF;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw = BSWAP32(stat);
  wbflush();

  /* Enable RT Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_RT);

  /* Check in Status Register */

  /*WAIT_CLOCKS(1000*p_api_dev->clk_1us*1000)*/ /* wait 1000ms */

  for(i = 0; i < (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/); i++)
  {
     /* 12.02.99 */
    stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
    wbflush();
    if( (stat &API_SSW_RTE_BIT) == API_SSW_RTE_BIT)
    {
      /*PRINTF1("RTE Bit set: %08x\n\r",stat);*/
      break;
    }
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }
  if(i == (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/) )
  {
     /* 12.02.99 */
    PRINTF0("RTE timeout\n\r");
    return (4); /* Time Out Error */
  }

  /* Disable RT Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_RT);
  

  /* Disable BIU Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BIU);

  return (BITE_OK);
} /* end: api_bite_board_en_test */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_INT_SELFTEST     */
/*                                                                           */
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : status indication                                          */
/*                                                                           */
/*    Description :                                                          */
/*    This function test the result of the power up BIT in the				 */
/*	  Status Word registers.					                             */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_int_selftest(TY_API_DEV *p_api_dev)
{
  L_WORD i       = 0;
  L_WORD stat    = 0;
  BYTE   err_val = 0;

  stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
  wbflush(); /* System Status Word */

  /* Test result of Power Up BITE */
  if( (err_val = (BYTE) ( (stat &0x0000FF00) >> 8) ) != 1)
  {
    PRINTF1("Internal Selftest Error: %d\n\r", err_val);
    if(err_val == 0)
    {
      err_val = 1;
    }
    return (err_val);
  }
  /* Check if BIU is enabled */

  if( (stat &API_SSW_OPE_BIT) == 0L)
  {

    /* Enable BIU Operation */
    ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);

    /* Check in Status Register */
    for(i = 0; i < (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/); i++)
    {
       /* 12.02.99 */
      stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
      wbflush();
      if( (stat &API_SSW_OPE_BIT) == API_SSW_OPE_BIT)
      {
        /*PRINTF1("ENA-BIU Status: %08x\n\r",stat);*/
        break;
      }
      CALL_SCHEDULER(); /* in case it takes long give CPU away */
    }
    if(i == (p_api_dev->clk_1us *1000 * 10 /*MAX_API_BITE_WAIT*10*/) )
    {
      PRINTF1("Internal Selftest Status error: %08x\n\r", stat);
      return (7); /* BIU Time Out Error */
    }
  }

  /* Disable BIU Operation */
  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BIU);

  return (BITE_OK);

} /* end: api_bite_int_selftest */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_PAGING_TEST      */
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
/*    Inputs    : Offset address [offset]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : Status Indication [status]                                 */
/*                                                                           */
/*    Description :                                                          */
/*    This function tests the API1553-DS memory by writing the selected page */
/*    number to the selected memory area.                                    */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_paging_test(TY_API_DEV *p_api_dev, L_WORD offset, BYTE mode)
{
  BYTE status;
  L_WORD volatile *ptr;
  L_WORD l, addr, page;

  status = BITE_OK;

  /* Write page number to memory */
  for(page = 0L, addr = offset; page < 16L; page++, addr += 0x10000L)
  {

    switch(mode)
    {
      default:
      case BITE_GLB_MEM:
         /* Global Memory */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);
        break;
      case BITE_SHARED_RAM:
         /* Shared RAM */
        ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(addr);
        break;
    }

    for(l = 0L; l < 0x10000L; l += 4L, ptr++)
    {
      if( ( (page == 0L) || (page == 1L) ) && (l < 0x100L) )
        ;
      else
        *ptr = BSWAP32(page);
      CALL_SCHEDULER(); /* in case it takes long give CPU away */
    }
  }

  /* Test result */
  for(page = 0L, addr = offset; page < 16L; page++, addr += 0x10000L)
  {

    switch(mode)
    {
      default:
      case BITE_GLB_MEM:
         /* Global Memory */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);
        break;
      case BITE_SHARED_RAM:
         /* Shared RAM */
        ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(addr);
        break;
    }

    for(l = 0L; l < 0x10000L; l += 4L, ptr++)
    {
      if( ( (page == 0L) || (page == 1L) ) && (l < 0x100L) )
        ;
      else
      {
        if(*ptr != BSWAP32(page) )
        {
          status = (BYTE) (BITE_ERROR + page);
          break;
        }
      }
      CALL_SCHEDULER(); /* in case it takes long give CPU away */
    }
  }

  return (status);

} /* end: api_bite_paging_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_FILL_RAM         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Fill pattern [fill]                                        */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function fills the API1553-DS RAM area with the specified pattern.*/
/*                                                                           */
/*****************************************************************************/

void api_bite_fill_ram(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, L_WORD fill, BYTE mode)
{
  L_WORD volatile *ptr;
  L_WORD l;

  switch(mode)
  {
    default:
    case BITE_GLB_MEM:
       /* Global Memory */
      ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
      break;
    case BITE_SHARED_RAM:
       /* Shared RAM */
      ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
      break;
  }

  /* Write memory */
  for(l = 0L; l < (eaddr - saddr); l += 4L, ptr++)
  {
    *ptr = BSWAP32(fill);
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

} /* end: api_bite_fill_ram */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_RDTST_RAM        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Fill pattern to test [fill]                                */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function tests the API1553-DS RAM area with the specified pattern.*/
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_rdtst_ram(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, L_WORD fill, BYTE mode)
{
  BYTE status;
  L_WORD volatile *ptr;
  L_WORD l;

  status = BITE_OK;

  switch(mode)
  {
    default:
    case BITE_GLB_MEM:
       /* Global Memory */
      ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
      break;
    case BITE_SHARED_RAM:
       /* Shared RAM */
      ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
      break;
  }

  /* Test memory */
  for(l = 0L; l < (eaddr - saddr); l += 4L, ptr++)
  {
    if(*ptr != BSWAP32(fill) )
    {
      PRINTF3("ERROR: Addr: [%08X] exp [%X] det[%X] \n\r", l, fill, BSWAP32(*ptr) );
      status = BITE_ERROR;
      break;
    }
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  return (status);

} /* end: api_bite_rdtst_ram */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_DAT_PAT          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function performs a data pattern test within the API1553-DS RAM.  */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_dat_pat(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode)
{
  L_WORD n, fill;
  BYTE status;

  fill = 0L;
  for(n = 0L; n < 4L; n++)
  {
    /* Test values : 0x0000, 0x5555, 0xAAAA, 0xFFFF */
    api_bite_fill_ram(p_api_dev, saddr, eaddr, fill, mode);
    status = api_bite_rdtst_ram(p_api_dev, saddr, eaddr, fill, mode);
    if(status == BITE_ERROR)
      return 1;
    fill += 0x55555555L;
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  return (BITE_OK);

} /* end: api_bite_dat_pat */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_WALK_BIT         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function performs a walking 1/0 test within the API1553-DS RAM.   */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_walk_bit(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode)
{
  L_WORD n, fill;
  BYTE status;

  /* Walking One Test */
  for(n = 0L, fill = 0x00000001L; n < 16L; n++, fill <<= 1L)
  {
    api_bite_fill_ram(p_api_dev, saddr, eaddr, fill, mode);
    status = api_bite_rdtst_ram(p_api_dev, saddr, eaddr, fill, mode);
    if(status == BITE_ERROR)
      return 2;
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  /* Walking Zero Test */
  for(n = 0L, fill = 0xfffffffeL; n < 16L; n++, fill <<= 1L)
  {
    if(n > 0)
      fill |= 0x00000001L;
    api_bite_fill_ram(p_api_dev, saddr, eaddr, fill, mode);
    status = api_bite_rdtst_ram(p_api_dev, saddr, eaddr, fill, mode);
    if(status == BITE_ERROR)
      return 2;
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  return (BITE_OK);

} /* end: api_bite_walk_bit */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_ADDR_TEST        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function performs a addressing test within the API1553-DS RAM.    */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_addr_test(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode)
{
  L_WORD volatile *ptr;
  L_WORD l, size;

  /* Write address value to memory */
  size = eaddr - saddr;
  for(l = saddr; l < (size + saddr); l += 4L)
  {
    switch(mode)
    {
      default:
      case BITE_GLB_MEM:
         /* Global Memory */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(l &0xFFFFFFFCL);
        break;
      case BITE_SHARED_RAM:
         /* Shared RAM */
        ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(l &0xFFFFFFFCL);
        break;
    }

    /* Write value */
    *ptr = BSWAP32(l);
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  /* Test memory */
  for(l = saddr; l < (size + saddr); l += 4L)
  {
    switch(mode)
    {
      default:
      case BITE_GLB_MEM:
         /* Global Memory */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(l &0xFFFFFFFCL);
        break;
      case BITE_SHARED_RAM:
         /* Shared RAM */
        ptr = (L_WORD*)API_SHARED_MEM_ADDR_ABS(l &0xFFFFFFFCL);
        break;
    }

    /* Check value */
    if(*ptr != BSWAP32(l) )
    {
      return 3;
    }
    CALL_SCHEDULER(); /* in case it takes long give CPU away */
  }

  return (BITE_OK);

} /* end: api_bite_addr_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BUS_TEST         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function performs a bus test within the API1553-DS RAM.           */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_bus_test(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode)
{
  L_WORD n, fill;
  L_WORD volatile *forward;
  L_WORD volatile *backward;
  BYTE status;

  fill = 0x00000000L;
  for(n = 0L; n < 2L; n++)
  {
    switch(mode)
    {
      default:
      case BITE_GLB_MEM:
         /* Global Memory */
        forward = (L_WORD*)API_GLB_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
        backward = (L_WORD*)API_GLB_MEM_ADDR_ABS( (eaddr - 4) &0xFFFFFFFCL);
        break;
      case BITE_SHARED_RAM:
         /* Shared RAM */
        forward = (L_WORD*)API_SHARED_MEM_ADDR_ABS(saddr &0xFFFFFFFCL);
        backward = (L_WORD*)API_SHARED_MEM_ADDR_ABS( (eaddr - 4) &0xFFFFFFFCL);
        break;
    }

    /* 2 tests, the first starts with fill=0x0000, second with 0xffff */
    *forward = fill; /* start address gets fill    */
    *backward = ~(*forward); /* end address gets the       */
    /* negated value of fill      */

    while(forward != (backward - 0x0001) )
    {
      forward++; /* increment of address x     */
      *forward = ~(*backward); /* x gets the negated value of*/
      backward--; /* y, decrement of y, y gets  */
      *backward = ~(*forward); /* the negated value of x     */
      CALL_SCHEDULER(); /* in case it takes long give CPU away */
    }

    /* First half of RAM has to be filled with start value of fill */
    status = api_bite_rdtst_ram(p_api_dev, saddr, saddr + (eaddr - saddr) / 2, fill, mode);
    if(status == BITE_ERROR)
      return 4;

    /* Second half with negated value of fill */
    fill = ~fill;
    status = api_bite_rdtst_ram(p_api_dev, saddr + (eaddr - saddr) / 2, eaddr, fill, mode);
    if(status == BITE_ERROR)
      return 4;
  }

  return (BITE_OK);

} /* end: api_bite_bus_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_RAMTEST          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : RAM start address [saddr]                                  */
/*                RAM end address [eaddr]                                    */
/*                Memory to check (Global/Shared) [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function performs a RAM Test within the API1553-DS memory.        */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_ramtest(TY_API_DEV *p_api_dev, L_WORD saddr, L_WORD eaddr, BYTE mode)
{
  BYTE echo;

  echo = BITE_OK;

  while(1)
  {
    echo = api_bite_dat_pat(p_api_dev, saddr, eaddr, mode);
    if(echo > 0)
      break;

    echo = api_bite_walk_bit(p_api_dev, saddr, eaddr, mode);
    if(echo > 0)
      break;

    echo = api_bite_addr_test(p_api_dev, saddr, eaddr, mode);
    if(echo > 0)
      break;

    echo = api_bite_bus_test(p_api_dev, saddr, eaddr, mode);
    if(echo > 0)
      break;

    break;
  }

  return (echo);

} /* end: api_bite_ramtest */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BC_BR_TEST       */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*				  MIL-Bus A/B 	[bus]                                     	 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes BC-BR transfers to SA1-SA30 on Bus A/ Bus B     */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_bc_br_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[32];
  WORD i, j, k;
  WORD rep_w, cw;
  L_WORD prev_ttag, cur_ttag;
  L_WORD volatile *lptr;
  L_WORD addr, cw_entry, dw_entry, entry_1, entry_2;
  TY_BC_INSTRUCTION xfers[32];
  TY_MIL_COM_LS_BUF_DEF_INPUT   buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT   buf_def_bc_in;
  TY_MIL_COM_LS_BUF_READ_INPUT  buf_read_in;
  TY_MIL_COM_LS_BUF_READ_OUTPUT buf_read_out;

  TY_MIL_COM_LS_RT_BH_DEF_INPUT       rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT      rt_bh_def_out;
  TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT rt_status_read_out;
  TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  rt_sa_msg_read_in;
  TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT rt_sa_msg_read_out;


  prev_ttag = 0L;
  cur_ttag = 0L;
  rep_w = 0;
  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  /* BC-Broadcast Transfer */
  for(i = 1; i < 31; i++)
  {
    xid = i;
    hid = i;
    bid = i;
    rt = 31;
    sa = (BYTE)i;
    wcnt = (BYTE)i;

    api_bc_xfer_def(p_api_dev, xid, hid, 0 /*BC-RT xfer_type*/, bus /*PRI chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, 0 /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 2 /*resp_exp1*/ /*01.02.99*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

    /* Buffer Header */
    api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);

    buf_def_bc_in.bt     = API_BUF_BC_TYPE;
    buf_def_bc_in.hid    = 0;
    buf_def_bc_in.bid    = bid;
    buf_def_bc_in.count  = 32;

    /* Define Buffer */
    for(j = 0; j < 32; j++)
        buf_def_bc_in.data[j] = i;

    api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

    /* Minor Frame */
    xfers[i-1].instr = API_BC_INSTR_XFER_CODE;
    xfers[i-1].xid   = xid;

  }

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 30 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = fid;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Init RT Buffer */
  for(k = 0; k < 32; k++)
  {
    buf_def_rt_in.data[k] = 0;
  }

  /* Init RT */
  api_rt_ini(p_api_dev, 31 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0xF800); /* 01.02.00 new Firmware (ms) */

  for(i = 1; i < 31; i++)
  {

    hid = i;
    bid = (i + 30);

    
    /* Buffer Header */
    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );



    buf_def_rt_in.bt     = API_BUF_RT_TYPE;
    buf_def_rt_in.hid    = 0;
    buf_def_rt_in.bid    = bid;
    buf_def_rt_in.count  = 32;

    api_buf_def(p_api_dev, &buf_def_rt_in, NULL );



    api_rt_sa_con(p_api_dev, 31 /*rt_addr*/, (BYTE)i /*sa_mc*/, hid, API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  }

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );

  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);

  /* wait */

  WAIT_CLOCKS(p_api_dev->clk_1us *1000 * 20);
  
  /* Stop BC Simulation */

  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );


  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* BC Verify */
  api_bc_status_read(p_api_dev);

  /* Verify global BC message count and error count */
  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt != 30)
  {
    PRINTF1("BC message count error: %d\n\r", p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt != 0)
  {
    PRINTF0("BC error count error\n\r");
    flag = BITE_ERROR;
    return (flag);
  }

  
  api_rt_status_read( p_api_dev, 0, &rt_status_read_out );

  /* Verify global RT message count and error count */
  if( rt_status_read_out.msg_cnt != 30)
  {
    PRINTF1("RT message count error: %d\n\r", rt_status_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if( rt_status_read_out.err_cnt != 0)
  {
    PRINTF1("RT error count error: %d\n\r", rt_status_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify BC Transfer count and Transfer error count */
  for(i = 1; i < 31; i++)
  {

    api_bc_xfer_read(p_api_dev, i /*xid*/, API_OFF /*clr_mode*/);


    if(p_api_dev->bc_xfer_dsp.msg_cnt != 1)
    {
      PRINTF1("BC transfer message count error: %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
      flag = BITE_ERROR;
      return (flag);
    }

    if(p_api_dev->bc_xfer_dsp.err_cnt != 0)
    {
      PRINTF1("BC transfer error count error: %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
      flag = BITE_ERROR;
      return (flag);
    }

    /* Verify BC transfer timetag */
    cur_ttag = p_api_dev->bc_xfer_dsp.cur_ttag;
    if(cur_ttag <= prev_ttag)
    {
      PRINTF1("BC transfer timetag error: %08x\n\r", cur_ttag);
      flag = BITE_ERROR;
      return (flag);
    }

    prev_ttag = cur_ttag;

    /* Verify BC transfer report word */
    rep_w = p_api_dev->bc_xfer_dsp.cur_brw &0x300F;
    if(rep_w != 0x2000)
    {
       /* Bus flag don't care for BC-BR */ /* 01.02.99 */
      PRINTF1("BC report word error: %04x\n\r", rep_w);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  prev_ttag = 0L;
  /* Verify RT Transfer count and Transfer error count */
  for(i = 1; i < 31; i++)
  {


    rt_sa_msg_read_in.rt_addr  = 31;
    rt_sa_msg_read_in.sa_mc    = i;
    rt_sa_msg_read_in.sa_type  = API_RT_SA_TYPE_RCV;
    rt_sa_msg_read_in.clr_mode = API_OFF;
    api_rt_sa_msg_read( p_api_dev, &rt_sa_msg_read_in, &rt_sa_msg_read_out );

    /* additionally RT transfer counter and RT error counter must be checked here */


    /* Verify RT transfer timetag */

    cur_ttag = rt_sa_msg_read_out.ttag;
    /*PRINTF1("count %d\n\r",i);*/
    if(cur_ttag < prev_ttag)
    {
      PRINTF0("RT transfer timetag error\n\r");
      PRINTF1("prev_ttag: %08x\n\r", prev_ttag);
      PRINTF1("cur_ttag:  %08x\n\r", cur_ttag);
      flag = BITE_ERROR;
      return (flag);
    }
    prev_ttag = cur_ttag;

    /* Coding Error in RT report word: 10110000 */
    /* Verify RT transfer report word */
    rep_w = rt_sa_msg_read_out.brw & 0x301F;

    if(bus == 0)
    {
       /* Bus A */
      if(rep_w != 0x1010)
      {
        PRINTF1("RT report word (Bus A) error: %04x\n\r", rep_w);
        flag = BITE_ERROR;
        return (flag);
      }
    }

    if(bus == 1)
    {
       /* Bus B */
      if(rep_w != 0x1000)
      {
        PRINTF1("RT report word (Bus B) error: %04x\n\r", rep_w);
        flag = BITE_ERROR;
        return (flag);
      }
    }

    /* Last Command word */
    cw = (31 << 11) | (i << 5) | i;
    if(rt_sa_msg_read_out.lcw != cw)
    {
      PRINTF1("RT command word error: %04x\n\r", cw);
      PRINTF1("RT command word error: %04x\n\r", rt_sa_msg_read_out.lcw);
      flag = BITE_ERROR;
      return (flag);
    }
    /* Test RT-Receive buffer */
    hid = 0;
    bid = (i + 30);

    buf_read_in.bt    = API_BUF_RT_TYPE;
    buf_read_in.hid   = hid;
    buf_read_in.bid   = bid;
    buf_read_in.count = 32;
    api_buf_read( p_api_dev, &buf_read_in, &buf_read_out );

    /* Read data from buffer */
    for(k = 0; k < i; k++)
    {
      if( buf_read_out.data[k] != i)
      {
        PRINTF3("RT data word1 error exp[%X] det[%X] Teststep [%X]\n\r", i, buf_read_out.data[k], k);
        flag = BITE_ERROR;
        return (flag);
      }
    }
    for(k = i; k < 32; k++)
    {
      if(buf_read_out.data[k] != 0)
      {
        PRINTF3("RT data word2 error  exp[%X] det[%X] Teststep [%X]\n\r", 0, buf_read_out.data[k], k);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }
  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    for(i = 1; i < 31; i++)
    {
      lptr++; /* Skip Timetag */
      lptr++;

      if(bus == 0)
      {
         /* Bus A */
        cw_entry = 0x80000000L | (31 << 11) | (i << 5) | i;
        dw_entry = 0xA0000000L | i;
      }
      else
      {
         /* Bus B*/
        cw_entry = 0xC0000000L | (31 << 11) | (i << 5) | i;
        dw_entry = 0xE0000000L | i;
      }
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      if(cw_entry != entry_1)
      {
        PRINTF1("BM cw_enty error: %08x\n\r", entry_1);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;

      /* Check Data Words */
      for(k = 0; k < i; k++)
      {
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error: %08x\n\r", entry_2);
          flag = BITE_ERROR;

          return (flag);
        }
        lptr++;
      }
    }
  }

  return (flag);
} /* end: api_bite_bc_br_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BC_RT_TEST       */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes BC-RT transfers on Bus A and RT-BC transfers	 */
/*    on Bus B                                                               */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_bc_rt_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE con)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid, nxw;
  BYTE flag;
  L_WORD frames[32];
  WORD i, j, k;
  WORD rep_w, cw, sw;
  L_WORD prev_ttag, cur_ttag;
  L_WORD volatile *lptr;
  L_WORD addr, cw_entry, dw_entry, sw_entry, entry_1, entry_2;
  TY_BC_INSTRUCTION xfers[64];

  TY_MIL_COM_LS_RT_BH_DEF_INPUT       rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT      rt_bh_def_out;
  TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT rt_status_read_out;
  TY_MIL_COM_LS_RT_MSG_READ_INPUT     rt_msg_read_in;
  TY_MIL_COM_LS_RT_MSG_READ_OUTPUT    rt_msg_read_out;
  TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  rt_sa_msg_read_in;
  TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT rt_sa_msg_read_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_bc_in;
  TY_MIL_COM_LS_BUF_READ_INPUT        buf_read_in;
  TY_MIL_COM_LS_BUF_READ_OUTPUT       buf_read_out;

  prev_ttag = 0L;
  cur_ttag = 0L;
  rep_w = 0;
  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  /* BC-RT Transfer */
  for(i = 1; i < 31; i++)
  {
    xid = i;
    hid = i;
    bid = i;
    rt = (BYTE)i;
    sa = (BYTE)i;
    wcnt = (BYTE)i;

    api_bc_xfer_def(p_api_dev, xid, hid, 0 /*BC-RT xfer_type*/, BITE_BUS_A /*chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, 0 /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

    /* Buffer Header */
    api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);


    buf_def_bc_in.bt     = API_BUF_BC_TYPE;
    buf_def_bc_in.hid    = 0;
    buf_def_bc_in.bid    = bid;
    buf_def_bc_in.count  = 32;

    /* Define Buffer */
    for(j = 0; j < 32; j++)
    {
      buf_def_bc_in.data[j] = i;
    }

    api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

    /* Minor Frame */
    xfers[i-1].instr = API_BC_INSTR_XFER_CODE;
    xfers[i-1].xid   = xid;

  }

  /* RT-BC Transfer */
  for(i = 1; i < 31; i++)
  {
    xid = i + 30;
    hid = i + 30;
    bid = i + 30;
    rt = (BYTE)i;
    sa = (BYTE)i;
    wcnt = (BYTE)i;

    api_bc_xfer_def(p_api_dev, xid, hid, 1 /*RT-BC xfer_type*/, BITE_BUS_B /*chn*/, rt /*xmt_rt*/, 0 /*rcv_rt*/, sa /*xmt_sa*/, 0 /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

    /* Buffer Header */
    api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);

    buf_def_bc_in.bt     = API_BUF_BC_TYPE;
    buf_def_bc_in.hid    = 0;
    buf_def_bc_in.bid    = bid;
    buf_def_bc_in.count  = 32;

    /* Define Buffer */
    for(j = 0; j < 32; j++)
    {
      buf_def_bc_in.data[j] = 0;
    }

    api_buf_def(p_api_dev, &buf_def_bc_in, NULL );


    /* Minor Frame */
    xfers[i-1+30].instr = API_BC_INSTR_XFER_CODE;
    xfers[i-1+30].xid   = xid;
  }

  /* Init RT Buffer */
  for(k = 0; k < 32; k++)
  {
    buf_def_rt_in.data[k] = 0;
  }

  /* Init RT */
  for(i = 1; i < 31; i++)
  {

    nxw = i << 11;
    api_rt_ini(p_api_dev, (BYTE)i /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, nxw);
    hid = i;
    bid = (i + 60);

    /* Buffer Header */
    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );


    buf_def_rt_in.bt     = API_BUF_RT_TYPE;
    buf_def_rt_in.hid    = 0;
    buf_def_rt_in.bid    = bid;
    buf_def_rt_in.count  = 32;

    api_buf_def(p_api_dev, &buf_def_rt_in, NULL );

    api_rt_sa_con(p_api_dev, (BYTE)i /*rt_addr*/, (BYTE)i /*sa_mc*/, hid, API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

    api_rt_sa_con(p_api_dev, (BYTE)i /*rt_addr*/, (BYTE)i /*sa_mc*/, hid, API_RT_SA_TYPE_XMT /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);
  }

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 60 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);

  /* wait */

  WAIT_CLOCKS(p_api_dev->clk_1us *1000 * 50);

  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );


  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* BC Verify */
  api_bc_status_read(p_api_dev);

  /* Verify global BC message count and error count */
  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt != 60)
  {
    PRINTF1("BC message count error %04x\n\r", p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt != 0)
  {
    PRINTF1("BC error count error %04x\n\r", p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* RT Verify */
  api_rt_status_read( p_api_dev, 0, &rt_status_read_out );

  /* Verify global RT message count and error count */
  if(rt_status_read_out.msg_cnt != 60)
  {
    PRINTF1("RT message count error: %d\n\r", rt_status_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_status_read_out.err_cnt != 0)
  {
    PRINTF1("RT error count error: %d\n\r", rt_status_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify BC Transfer count and Transfer error count */
  for(i = 1; i < 31; i++)
  {

    api_bc_xfer_read(p_api_dev, i /*xid*/, API_OFF /*clr_mode*/);


    if(p_api_dev->bc_xfer_dsp.msg_cnt != 1)
    {
      PRINTF1("BC transfer message count error: %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
      flag = BITE_ERROR;
      return (flag);
    }

    if(p_api_dev->bc_xfer_dsp.err_cnt != 0)
    {
      PRINTF1("BC transfer error count error: %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
      flag = BITE_ERROR;
      return (flag);
    }

    /* Verify BC transfer timetag */
    cur_ttag = p_api_dev->bc_xfer_dsp.cur_ttag;
    if(cur_ttag <= prev_ttag)
    {
      PRINTF1("BC transfer timetag error: %08x\n\r", cur_ttag);
      flag = BITE_ERROR;
      return (flag);
    }

    prev_ttag = cur_ttag;

    /* Verify BC transfer report word */
    rep_w = p_api_dev->bc_xfer_dsp.cur_brw;

    if(rep_w != 0x2010)
    {
       /* 01.01.99 */
      PRINTF1("Bus A rep_word error: %04x\n\r", rep_w);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  prev_ttag = 0L;

  /* Verify RT Transfer count and Transfer error count */
  for(i = 1; i < 31; i++)
  {

    rt_sa_msg_read_in.rt_addr  = i;
    rt_sa_msg_read_in.sa_mc    = i;
    rt_sa_msg_read_in.sa_type  = API_RT_SA_TYPE_XMT;
    rt_sa_msg_read_in.clr_mode = API_OFF;
    api_rt_sa_msg_read( p_api_dev, &rt_sa_msg_read_in, &rt_sa_msg_read_out );

    /* Verify RT transfer timetag */

    cur_ttag = rt_sa_msg_read_out.ttag;

    if(cur_ttag < prev_ttag)
    {
      PRINTF0("RT transfer timetag error\n\r");
      PRINTF1("prev_ttag: %08x\n\r", prev_ttag);
      PRINTF1("cur_ttag:  %08x\n\r", cur_ttag);
      flag = BITE_ERROR;
      return (flag);
    }
    prev_ttag = cur_ttag;

    /* Verify RT transfer report word */
    rep_w = rt_sa_msg_read_out.brw & 0x301F;

    if(rep_w != 0x2000)
    {
      PRINTF1("RT report word (Bus B) error: %04x\n\r", rep_w);
      flag = BITE_ERROR;
      return (flag);
    }

    /* Last Command word */
    cw = (i << 11) | (i << 5) | i | 0x0400;
    if(rt_sa_msg_read_out.lcw != cw)
    {
      PRINTF1("RT command word error: %04x (CW)\n\r", cw);
      PRINTF1("RT command word error: %04x (cur_lcw)\n\r", rt_sa_msg_read_out.lcw);
      flag = BITE_ERROR;
      return (flag);
    }

    /* Last Status word */
    sw = (i << 11);
    if(rt_sa_msg_read_out.lsw != sw)
    {
      PRINTF1("RT command word error: %04x (SW)\n\r", sw);
      PRINTF1("RT command word error: %04x (cur_lsw)\n\r", rt_sa_msg_read_out.lsw);
      flag = BITE_ERROR;
      return (flag);
    }


    rt_msg_read_in.rt_addr = i;
    api_rt_msg_read( p_api_dev, &rt_msg_read_in, &rt_msg_read_out );

    if(rt_msg_read_out.msg_cnt != 2)
    {
      PRINTF1("RT message count error: %04x\n\r",rt_msg_read_out.msg_cnt);
      flag = BITE_ERROR;
      return (flag);
    }
    if(rt_msg_read_out.err_cnt != 0)
    {
      PRINTF1("RT error count error: %04x\n\r",rt_msg_read_out.err_cnt);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  /* Test BC-Receive buffer */
  for(i = 1; i < 31; i++)
  {
    hid = 0;
    bid = (i + 30);

    buf_read_in.bt    = API_BUF_BC_TYPE;
    buf_read_in.hid   = hid;
    buf_read_in.bid   = bid;
    buf_read_in.count = 32;
    api_buf_read( p_api_dev, &buf_read_in, &buf_read_out );

    /* Read data from buffer */
    for(k = 0; k < i; k++)
    {
      if(buf_read_out.data[k] != i)
      {
        PRINTF2("BC data word1 error exp %x det %x\n\r", i, buf_read_out.data[k] );
        flag = BITE_ERROR;
        return (flag);
      }
    }
    for(k = i; k < 32; k++)
    {
      if(buf_read_out.data[k] != 0)
      {
        PRINTF2("BC data word2 error exp %X det %X\n\r", 0, buf_read_out.data[k]);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }

  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    /* BC-RT transfer */
    for(i = 1; i < 31; i++)
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus A (BC-RT) */
      cw_entry = 0x80000000L | (i << 11) | (i << 5) | i;
      sw_entry = 0xB0000000L | (i << 11);
      dw_entry = 0xA0000000L | i;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      if(cw_entry != entry_1)
      {
        PRINTF1("BM cw_entry error (BC-RT): %08x\n\r", entry_1);
        PRINTF1("BM cw_entry error (BC-RT): %08x\n\r", cw_entry);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;

      /* Check Data Words */
      for(k = 0; k < i; k++)
      {
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error (BC-RT): %08x\n\r", entry_2);
          flag = BITE_ERROR;
          PRINTF1("BC-RT Test DW%d Error", i);
          return (flag);
        }
        lptr++;
      }
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      if(sw_entry != entry_1)
      {
        PRINTF1("BM sw_entry error (BC-RT): %08x\n\r", entry_1);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;
    }

    cw_entry = 0L;
    sw_entry = 0L;
    dw_entry = 0L;

    /* RT-BC transfer */
    for(i = 1; i < 31; i++)
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus B (RT-BC) */
      cw_entry = 0xC0000000L | (i << 11) | (i << 5) | i | 0x0400;
      sw_entry = 0xF0000000L | (i << 11);
      dw_entry = 0xE0000000L | i;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Check Command Word */
      if(cw_entry != entry_1)
      {
        PRINTF1("BM cw_entry error (RT-BC): %08x\n\r", entry_1);
        PRINTF1("cw_entry %d\n\r", i);
        PRINTF1("BM cw_entry error (RT-BC): %08x\n\r", cw_entry);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;

      /* Check Status Word */
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      if(sw_entry != entry_1)
      {
        PRINTF1("BM sw_entry error (RT-BC): %08x\n\r", entry_1);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;

      /* Check Data Words */
      for(k = 0; k < i; k++)
      {
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error (RT-BC): %08x\n\r", entry_2);
          flag = BITE_ERROR;
          PRINTF1("RT-BC Test DW%d Error", i);
          return (flag);
        }
        lptr++;
      }
    }
  }
  return (flag);

} /* end: api_bite_bc_rt_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_RT_RT_TEST       */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*				  MIL-Bus A/B 	[bus]                                     	 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes RT-RT transfers on Bus A / Bus B	     		 */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_rt_rt_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid, nxw;
  BYTE flag;
  L_WORD frames[32];
  WORD i, k, j;
  WORD rep_w, cw, sw;
  L_WORD prev_ttag, cur_ttag;
  L_WORD volatile *lptr;
  L_WORD addr, cw1_entry, cw2_entry, sw1_entry, sw2_entry, dw_entry, entry_1, entry_2;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_BH_DEF_INPUT       rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT      rt_bh_def_out;
  TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT rt_status_read_out;
  TY_MIL_COM_LS_RT_MSG_READ_INPUT     rt_msg_read_in;
  TY_MIL_COM_LS_RT_MSG_READ_OUTPUT    rt_msg_read_out;
  TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  rt_sa_msg_read_in;
  TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT rt_sa_msg_read_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_bc_in;
  TY_MIL_COM_LS_BUF_READ_INPUT        buf_read_in;
  TY_MIL_COM_LS_BUF_READ_OUTPUT       buf_read_out;

  prev_ttag = 0L;
  cur_ttag = 0L;
  rep_w = 0;
  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  /* RT-RT Transfer */
  for(i = 1; i < 31; i++)
  {
    xid = i;
    hid = i;
    bid = i;
    rt = (BYTE)i;
    sa = (BYTE)i;
    wcnt = (BYTE)1;

    api_bc_xfer_def(p_api_dev, xid, hid, 2 /*RT-RT xfer_type*/, bus /*chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, sa /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);
    /* Minor Frame */
    xfers[i-1].instr = API_BC_INSTR_XFER_CODE;
    xfers[i-1].xid   = xid;

  }

  /* Init RT1-RT30 Receive Buffer */
  for(k = 0; k < 32; k++)
  {
    buf_def_rt_in.data[ k ] = 0;
  }

  /* Init RT */
  api_rt_ini(p_api_dev, 0 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0);

  for(i = 1; i < 31; i++)
  {

    /* Init RT0-Transmit Buffer */
    for(j = 0; j < 32; j++)
    {
      buf_def_bc_in.data[j] = i;
    }

    nxw = i << 11;
    api_rt_ini(p_api_dev, (BYTE)i /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, nxw);
    hid = i;
    bid = i;

    /* Buffer Header */
    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );


    buf_def_bc_in.bt     = API_BUF_RT_TYPE;
    buf_def_bc_in.hid    = 0;
    buf_def_bc_in.bid    = bid;
    buf_def_bc_in.count  = 32;

    api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

    api_rt_sa_con(p_api_dev, 0 /*rt_addr*/, (BYTE)i /*sa_mc*/, hid, API_RT_SA_TYPE_XMT /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

    /* Buffer Header */
    rt_bh_def_in.hid   = hid + 30;
    rt_bh_def_in.bid   = bid + 30;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );


    buf_def_rt_in.bt     = API_BUF_RT_TYPE;
    buf_def_rt_in.hid    = 0;
    buf_def_rt_in.bid    = bid + 30;
    buf_def_rt_in.count  = 32;

    api_buf_def(p_api_dev, &buf_def_rt_in, NULL );

    api_rt_sa_con(p_api_dev, (BYTE)i /*rt_addr*/, (BYTE)i /*sa_mc*/, (WORD) (hid + 30), API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  }

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 30 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);

  /* wait */

  WAIT_CLOCKS(p_api_dev->clk_1us *1000 * 10);


  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );


  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* BC Verify */
  api_bc_status_read(p_api_dev);

  /* Verify global BC message count and error count */
  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt != 30)
  {
    PRINTF0("BC global message count error\n\r");
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt != 0)
  {
    PRINTF0("BC global error count error\n\r");
    flag = BITE_ERROR;
    return (flag);
  }

  /* RT Verify */
  api_rt_status_read( p_api_dev, 0, &rt_status_read_out );

  /* Verify global RT message count and error count */
  if(rt_status_read_out.msg_cnt != 60)
  {
    PRINTF1("RT global message count error: %d\n\r", rt_status_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_status_read_out.err_cnt != 0)
  {
    PRINTF1("RT global error count error: %d\n\r",rt_status_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify BC Transfer count and Transfer error count */
  for(i = 1; i < 31; i++)
  {

    api_bc_xfer_read(p_api_dev, i /*xid*/, API_OFF /*clr_mode*/);


    if(p_api_dev->bc_xfer_dsp.msg_cnt != 1)
    {
      PRINTF1("BC transfer message count error (RT-RT): %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
      flag = BITE_ERROR;
      return (flag);
    }

    if(p_api_dev->bc_xfer_dsp.err_cnt != 0)
    {
      PRINTF1("BC transfer error count error (RT-RT): %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  /* Verify RT0 message and error counter */

  rt_msg_read_in.rt_addr = 0;
  api_rt_msg_read( p_api_dev, &rt_msg_read_in, &rt_msg_read_out );

  if(rt_msg_read_out.msg_cnt != 30)
  {
    PRINTF1("RT0 transfer message count error (RT-RT): %d\n\r", rt_msg_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_msg_read_out.err_cnt != 0)
  {
    PRINTF1("RT0 transfer error count error (RT-RT): %d\n\r", rt_msg_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify RT Transfer count and Transfer error count */
  prev_ttag = 0;
  for(i = 1; i < 31; i++)
  {


    rt_msg_read_in.rt_addr = i;
    api_rt_msg_read( p_api_dev, &rt_msg_read_in, &rt_msg_read_out );

    if(rt_msg_read_out.msg_cnt != 1)
    {
      PRINTF1("RT message count error (RT-RT): %04x\n\r", rt_msg_read_out.msg_cnt);
      flag = BITE_ERROR;
      return (flag);
    }
    if(rt_msg_read_out.err_cnt != 0)
    {
      PRINTF1("RT error count error (RT-RT): %04x\n\r", rt_msg_read_out.err_cnt);
      flag = BITE_ERROR;
      return (flag);
    }


    rt_sa_msg_read_in.rt_addr  = i;
    rt_sa_msg_read_in.sa_mc    = i;
    rt_sa_msg_read_in.sa_type  = API_RT_SA_TYPE_RCV;
    rt_sa_msg_read_in.clr_mode = API_OFF;
    api_rt_sa_msg_read( p_api_dev, &rt_sa_msg_read_in, &rt_sa_msg_read_out );

    /* Verify RT transfer timetag */

    cur_ttag = rt_sa_msg_read_out.ttag;

    if(cur_ttag < prev_ttag)
    {
      PRINTF1("RT transfer timetag error: (RT-RT) %08x\n\r", cur_ttag);
      PRINTF1("prev_ttag: %08x\n\r", prev_ttag);
      PRINTF1("cur_ttag:  %08x\n\r", cur_ttag);
      flag = BITE_ERROR;
      return (flag);
    }
    prev_ttag = cur_ttag;

    /* Verify RT transfer report word */
    rep_w = rt_sa_msg_read_out.brw & 0x301F;
    if(bus == 0)
    {
       /* Bus A */
      if(rep_w != 0x1010)
      {
        PRINTF1("RT report word %d\n\r", i);
        PRINTF1("RT report word (Bus A) error (RT-RT): %04x\n\r", rep_w);
        flag = BITE_ERROR;
        return (flag);
      }
    }

    if(bus == 1)
    {
       /* Bus B */
      if(rep_w != 0x1000)
      {
        PRINTF1("RT report word %d\n\r", i);
        PRINTF1("RT report word (Bus B) error: (RT-RT) %04x\n\r", rep_w);
        flag = BITE_ERROR;
        return (flag);
      }
    }

    /* Last Command word */
    cw = (i << 11) | (i << 5) | 1;
    if(rt_sa_msg_read_out.lcw != cw)
    {
      PRINTF1("RT command word error: %04x\n\r", cw);
      PRINTF1("RT command word error (RT-RT): %04x\n\r", rt_sa_msg_read_out.lcw);
      flag = BITE_ERROR;
      return (flag);
    }

    /* Last Status word */
    sw = (i << 11);
    if(rt_sa_msg_read_out.lsw != sw)
    {
      PRINTF1("RT status word error: %04x\n\r", sw);
      PRINTF1("RT status word error: (RT-RT) %04x\n\r", rt_sa_msg_read_out.lsw);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  /* Test RT-Receive buffer */
  for(i = 1; i < 31; i++)
  {
    hid = 0;
    bid = (i + 30);

    buf_read_in.bt    = API_BUF_RT_TYPE;
    buf_read_in.hid   = hid;
    buf_read_in.bid   = bid;
    buf_read_in.count = 32;
    api_buf_read( p_api_dev, &buf_read_in, &buf_read_out );

    /* Read data from buffer */
    for(k = 0; k < 1; k++)
    {
      if(buf_read_out.data[k] != i)
      {
        PRINTF1("addr: %x\n\r", buf_read_out.data[k]);
        PRINTF2("RT%d data word1 error: (RT-RT) %04x\n\r", i, buf_read_out.data[k]);
        flag = BITE_ERROR;
        return (flag);
      }
    }
    for(k = 1; k < 32; k++)
    {
      if(buf_read_out.data[k] != 0)
      {
        PRINTF2("RT%d data word2 error: (RT-RT) %04x\n\r", i, buf_read_out.data[k]);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }

  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    if(bus == 0)
    {
      /* RT-RT transfer Bus A */
      for(i = 1; i < 31; i++)
      {
        lptr++; /* Skip Timetag */
        lptr++;

        /* Bus A (RT-RT) */
        cw1_entry = 0x80000000L | (i << 11) | (i << 5) | 1;
        cw2_entry = 0x90000000L | (i << 5) | 0x0401;
        sw1_entry = 0xB0000000L;
        sw2_entry = 0xB0000000L | (i << 11);
        dw_entry = 0xA0000000L | i;

        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Receive Command */
        if(cw1_entry != entry_1)
        {
          PRINTF1("BM cw1_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM cw1_entry error (RT-RT): %08x\n\r", cw1_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Transmit Command */
        if(cw2_entry != entry_1)
        {
          PRINTF1("BM cw2_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM cw2_entry error (RT-RT): %08x\n\r", cw2_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Check Status Word 1 */
        if(sw1_entry != entry_1)
        {
          PRINTF1("BM sw1_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM sw1_entry error (RT-RT): %08x\n\r", sw1_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;

        /* Check Data Word */
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error: %08x\n\r", entry_2);
          flag = BITE_ERROR;
          PRINTF1("RT-RT Test DW%d Error", i);
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Check Status Word 2 */
        if(sw2_entry != entry_1)
        {
          PRINTF1("BM sw2_entry error (RT-RT): %08x\n\r", entry_1);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
      }
    }
    else
    {
      cw1_entry = 0L;
      sw1_entry = 0L;
      cw2_entry = 0L;
      sw2_entry = 0L;
      dw_entry = 0L;

      /* RT-RT transfer Bus B*/
      for(i = 1; i < 31; i++)
      {
        lptr++; /* Skip Timetag */
        lptr++;

        /* Bus B (RT-RT) */
        cw1_entry = 0xC0000000L | (i << 11) | (i << 5) | 1;
        cw2_entry = 0xD0000000L | (i << 5) | 0x0401;
        sw1_entry = 0xF0000000L;
        sw2_entry = 0xF0000000L | (i << 11);
        dw_entry = 0xE0000000L | i;

        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Receive Command */
        if(cw1_entry != entry_1)
        {
          PRINTF1("BM cw1_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM cw1_entry error (RT-RT): %08x\n\r", cw1_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Transmit Command */
        if(cw2_entry != entry_1)
        {
          PRINTF1("BM cw2_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM cw2_entry error (RT-RT): %08x\n\r", cw2_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Check Status Word 1 */
        if(sw1_entry != entry_1)
        {
          PRINTF1("BM sw1_entry error (RT-RT): %08x\n\r", entry_1);
          PRINTF1("BM sw1_entry error (RT-RT): %08x\n\r", sw1_entry);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
        /* Check Data Word */
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error (RT-RT): %08x\n\r", entry_2);
          flag = BITE_ERROR;
          PRINTF1("BC-RT Test DW%d Error", i);
          return (flag);
        }
        lptr++;
        entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
        /* Check Status Word 2 */
        if(sw2_entry != entry_1)
        {
          PRINTF1("BM sw2_entry error (RT-RT): %08x\n\r", entry_1);
          flag = BITE_ERROR;
          return (flag);
        }
        lptr++;
      }
    }

    /* Read BM Activity List */
    for(i = 1; i < 31; i++)
    {

      api_bm_rt_sa_act_read(p_api_dev, 0 /*rt*/, (BYTE)i /*sa*/, API_RT_SA_TYPE_XMT /*sa_type*/);

      /* Message Counter */
      if(p_api_dev->bm_rt_act_dsp.cc != 1)
      {
        PRINTF1("BM Activity List Message error (RT0): %08x\n\r", p_api_dev->bm_rt_act_dsp.cc);
        flag = BITE_ERROR;
        return (flag);
      }
      /* Error Counter */
      if(p_api_dev->bm_rt_act_dsp.ec != 0)
      {
        PRINTF1("BM Activity List Error error (RT0): %08x\n\r", p_api_dev->bm_rt_act_dsp.ec);
        flag = BITE_ERROR;
        return (flag);
      }
    }
    /* Read BM Activity List */
    for(i = 1; i < 31; i++)
    {

      api_bm_rt_sa_act_read(p_api_dev, (BYTE)i /*rt*/, (BYTE)i /*sa*/, API_RT_SA_TYPE_RCV /*sa_type*/);

      /* Message Counter */
      if(p_api_dev->bm_rt_act_dsp.cc != 1)
      {
        PRINTF2("BM Activity List Message error (RT%d): %08x\n\r", i, p_api_dev->bm_rt_act_dsp.cc);
        flag = BITE_ERROR;
        return (flag);
      }
      /* Error Counter */
      if(p_api_dev->bm_rt_act_dsp.ec != 0)
      {
        PRINTF2("BM Activity List Error error (RT%d): %08x\n\r", i, p_api_dev->bm_rt_act_dsp.ec);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }
  return (flag);
} /* end: api_bite_rt_rt_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_MC_TEST          */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*				  MIL-Bus A/B 	[bus]                                     	 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes a RT-BC Modecode transfer (MC00) on Bus A/B   	 */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_mc_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con)
{
  BYTE fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[32];
  L_WORD volatile *lptr;
  L_WORD addr, cw1_entry, sw1_entry, entry_1;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_MSG_READ_INPUT  rt_msg_read_in;
  TY_MIL_COM_LS_RT_MSG_READ_OUTPUT rt_msg_read_out;

  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  xid = 1;
  hid = 1;
  bid = 1;
  api_bc_xfer_def(p_api_dev, xid, hid, 1 /*RT-BC xfer_type*/, bus /*chn*/, 0 /*xmt_rt*/, 0 /*rcv_rt*/, 0 /*xmt_sa*/, 0 /*rcv_sa*/, 0 /*wcnt*/, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

  /* Buffer Header */
  api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);


  /* Minor Frame */
  xfers[0].instr = API_BC_INSTR_XFER_CODE;
  xfers[0].xid   = xid;


  /* Init RT */
  api_rt_ini(p_api_dev, 0 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0);

  api_rt_sa_con(p_api_dev, 0 /*rt_addr*/, 0 /*sa_mc*/, hid, API_RT_MC_TYPE_XMT /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 1 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);


  WAIT_CLOCKS(p_api_dev->clk_1us *1000);


  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );



  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* Verify BC Transfer count and Transfer error count */
  api_bc_xfer_read(p_api_dev, 1 /*xid*/, API_OFF /*clr_mode*/);

  if(p_api_dev->bc_xfer_dsp.msg_cnt != 1)
  {
    PRINTF1("BC transfer message count error: (MC) %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_xfer_dsp.err_cnt != 0)
  {
    PRINTF1("BC transfer error count error: (MC) %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }
  /* Verify RT0 message and error counter */

  rt_msg_read_in.rt_addr = 0;
  api_rt_msg_read( p_api_dev, &rt_msg_read_in, &rt_msg_read_out );

  if(rt_msg_read_out.msg_cnt != 1)
  {
    PRINTF1("RT0 transfer message count error: (MC) %d\n\r", rt_msg_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_msg_read_out.err_cnt != 0)
  {
    PRINTF1("RT0 transfer error count error (MC): %d\n\r", rt_msg_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }
  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    if(bus == 0)
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus A (RT-RT) */
      cw1_entry = 0x80000000L | 0x0400;
      sw1_entry = 0xB0000000L;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Receive Command */
      if(cw1_entry != entry_1)
      {
        PRINTF1("BM cw1_entry error (MC): %08x\n\r", entry_1);
        PRINTF1("BM cw1_entry error (MC): %08x\n\r", cw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Check Status Word 1 */
      if(sw1_entry != entry_1)
      {
        PRINTF1("BM sw1_entry error (MC): %08x\n\r", entry_1);
        PRINTF1("BM sw1_entry error (MC): %08x\n\r", sw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }
    }
    else
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus B (RT-RT) */
      cw1_entry = 0xC0000000L | 0x0400;
      sw1_entry = 0xF0000000L;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Receive Command */
      if(cw1_entry != entry_1)
      {
        PRINTF1("BM cw1_entry error (MC): %08x\n\r", entry_1);
        PRINTF1("BM cw1_entry error (MC): %08x\n\r", cw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Check Status Word 1 */
      if(sw1_entry != entry_1)
      {
        PRINTF1("BM sw1_entry error (MC): %08x\n\r", entry_1);
        PRINTF1("BM sw1_entry error (MC): %08x\n\r", sw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }

  return (flag);
} /* end: api_bite_mc_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BC_RESP_TOUT_TEST*/
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*				  MIL-Bus A/B 	[bus]                                     	 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes a RT-BC Modecode transfer (MC00)				 */
/*    on Bus A / Bus B with a BC-Timeout of 6us and a RT-Responstime of 8us. */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_bc_resp_tout_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con)
{
  BYTE fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[ 32 ];
  L_WORD volatile *lptr;
  L_WORD addr, cw1_entry, err_entry, entry_1;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_MSG_READ_INPUT    rt_msg_read_in;
  TY_MIL_COM_LS_RT_MSG_READ_OUTPUT   rt_msg_read_out;
  TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT sys_resp_tou_in;
  TY_MIL_COM_ACK                     ack;

  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  xid = 1;
  hid = 1;
  bid = 1;
  api_bc_xfer_def(p_api_dev, xid, hid, 1 /*RT-BC xfer_type*/, bus /*chn*/, 0 /*xmt_rt*/, 0 /*rcv_rt*/, 0 /*xmt_sa*/, 0 /*rcv_sa*/, 0 /*wcnt*/, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

  /* Buffer Header */
  api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);

  /* Minor Frame */
  xfers[0].instr = API_BC_INSTR_XFER_CODE;
  xfers[0].xid   = xid;

  /* Set BC-Timeout to 6us */
  sys_resp_tou_in.timeout = 24;
  api_sys_def_resp_tout(p_api_dev, &sys_resp_tou_in, &ack);


  /* Init RT */
  api_rt_ini(p_api_dev, 0 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 40 /* resp_time*/, 0);

  api_rt_sa_con(p_api_dev, 0 /*rt_addr*/, 0 /*sa_mc*/, hid, API_RT_MC_TYPE_XMT /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 1 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);


  WAIT_CLOCKS(p_api_dev->clk_1us *1000);


  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );



  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* Verify BC Transfer count and Transfer error count */
  api_bc_xfer_read(p_api_dev, 1 /*xid*/, API_OFF /*clr_mode*/);

  if(p_api_dev->bc_xfer_dsp.msg_cnt != 0)
  {
    PRINTF1("BC transfer message count error (TOUT): %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_xfer_dsp.err_cnt != 1)
  {
    PRINTF1("BC transfer error count error (TOUT): %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }
  /* Verify RT0 message and error counter */

  rt_msg_read_in.rt_addr = 0;
  api_rt_msg_read( p_api_dev, &rt_msg_read_in, &rt_msg_read_out );

  if(rt_msg_read_out.msg_cnt != 1)
  {
    PRINTF1("RT0 transfer message count error (TOUT): %d\n\r", rt_msg_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_msg_read_out.err_cnt != 0)
  {
    PRINTF1("RT0 transfer error count error (TOUT): %d\n\r", rt_msg_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }
  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    if(bus == 0)
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus A (RT-RT) */
      cw1_entry = 0x80000000L | 0x0400;
      err_entry = 0x10000000L | 0x8001;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Receive Command */
      if(cw1_entry != entry_1)
      {
        PRINTF1("BM cw1_entry error (Tout): %08x\n\r", entry_1);
        PRINTF1("BM cw1_entry error (Tout): %08x\n\r", cw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }

      lptr++;
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Check Status Word 1 */
      if(err_entry != entry_1)
      {
        PRINTF1("BM err_entry error (Tout): %08x\n\r", entry_1);
        PRINTF1("BM err_entry error (Tout): %08x\n\r", err_entry);
        flag = BITE_ERROR;
        return (flag);
      }
    }
    else
    {
      lptr++; /* Skip Timetag */
      lptr++;

      /* Bus B (RT-RT) */
      cw1_entry = 0xC0000000L | 0x0400;
      err_entry = 0x10000000L | 0x8001;

      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Receive Command */
      if(cw1_entry != entry_1)
      {
        PRINTF1("BM cw1_entry error (TOUT): %08x\n\r", entry_1);
        PRINTF1("BM cw1_entry error (MC): %08x\n\r", cw1_entry);
        flag = BITE_ERROR;
        return (flag);
      }

      lptr++;
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      /* Check Status Word 1 */
      if(err_entry != entry_1)
      {
        PRINTF1("BM err_entry error  (TOUT): %08x\n\r", entry_1);
        PRINTF1("BM err_entry error (MC): %08x\n\r", err_entry);
        flag = BITE_ERROR;
        return (flag);
      }
    }
  }

  return (flag);
} /* end: api_bite_bc_resp_tout_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BC_INTR_TEST     */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : none                                                    	 */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes a BC-RT transfer on Bus A. The Status-Flag      */
/*	  'p_api_dev->bc_status_dsp[p_api_dev->biu].status' indicates if interrupt service   	 */
/*	  routine was executed.													 */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_bc_intr_test(TY_API_DEV *p_api_dev)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[32];
  WORD j;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_BH_DEF_INPUT  rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT rt_bh_def_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT    buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT    buf_def_bc_in;

  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  /* BC-RT Transfer */
  xid = 1;
  hid = 1;
  bid = 1;
  rt = 1;
  sa = 1;
  wcnt = 1;

  api_bc_xfer_def(p_api_dev, xid, hid, 0 /*BC-RT xfer_type*/, BITE_BUS_A /*chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, 0 /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

  /* Buffer Header */
  api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);


  buf_def_bc_in.bt     = API_BUF_BC_TYPE;
  buf_def_bc_in.hid    = 0;
  buf_def_bc_in.bid    = bid;
  buf_def_bc_in.count  = 32;

  /* Define Buffer */
  for(j = 0; j < 32; j ++)
  {
        buf_def_bc_in.data[j] = j;
  }

  api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

  /* Minor Frame */
  xfers[0].instr = API_BC_INSTR_XFER_CODE;
  xfers[0].xid   = xid;

  /* Init RT */
  api_rt_ini(p_api_dev, 1 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0x800);

  hid = 2;
  bid = 2;

  /* Buffer Header */
    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );


  buf_def_rt_in.bt     = API_BUF_RT_TYPE;
  buf_def_rt_in.hid    = 0;
  buf_def_rt_in.bid    = bid;
  buf_def_rt_in.count  = 32;

  api_buf_def(p_api_dev, &buf_def_rt_in, NULL );

  api_rt_sa_con(p_api_dev, 1 /*rt_addr*/, 1 /*sa_mc*/, hid, API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 1 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Set BC status to busy */
  p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_BUSY;

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);

  /* wait */

  WAIT_CLOCKS(p_api_dev->clk_1us *1000);

  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );



  /* Test BC status */
  if(p_api_dev->bc_status_dsp[ p_api_dev->biu ].status != API_BC_STATUS_HALT)
  {
    PRINTF1("BC status error (INT): %d\n\r", p_api_dev->bc_status_dsp[ p_api_dev->biu ].status);
    flag = BITE_ERROR;
    return (flag);
  }
  return (flag);
} /* end: api_bite_bc_intr_test */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_DAC_TEST	     */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 01.04.99                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : Coupling Mode [mode]										 */
/*				  MIL-Bus A/B 	[bus]                                     	 */
/*                BM Mode       [con]                                        */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This test verifies if the output amplitude can be controled through  	 */
/*    the Coupling-and Amplitude-Registers for Bus A/B			             */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_dac_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, BYTE con)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[32];
  WORD i, j;
  L_WORD car_entry;
  L_WORD step;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_BH_DEF_INPUT  rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT rt_bh_def_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT    buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT    buf_def_bc_in;

  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* BC-RT Transfer */
  xid = 1;
  hid = 1;
  bid = 1;
  rt = 1;
  sa = 1;
  wcnt = 31;

  api_bc_xfer_def(p_api_dev, xid, hid, 0 /*BC-RT xfer_type*/, bus /*chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, 0 /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 0 /*resp_exp1*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

  /* Buffer Header */
  api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);

  buf_def_bc_in.bt     = API_BUF_RT_TYPE;
  buf_def_bc_in.hid    = 0;
  buf_def_bc_in.bid    = bid;
  buf_def_bc_in.count  = 32;

  /* Define Buffer */
  for(j = 0; j < 32; j++)
  {
      buf_def_bc_in.data[j] = j;
  }

  api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

  /* Minor Frame */
  xfers[0].instr = API_BC_INSTR_XFER_CODE;
  xfers[0].xid   = xid;

  /* Init RT */
  api_rt_ini(p_api_dev, 1 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0x800);

  /* Buffer Header */
  hid = 2;
  bid = 2;

    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );

  
  buf_def_rt_in.bt     = API_BUF_RT_TYPE;
  buf_def_rt_in.hid    = 0;
  buf_def_rt_in.bid    = bid;
  buf_def_rt_in.count  = 32;

  api_buf_def(p_api_dev, &buf_def_rt_in, NULL );


  api_rt_sa_con(p_api_dev, 1 /*rt_addr*/, 1 /*sa_mc*/, hid, API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 1 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = 1;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  step = 5;
  for(i = 255, j = 1; (i-(WORD)step) >= 0; i -= (WORD)step, j++)
  {
    if( i <= 5 )
      step = 1;

    if( 0 == i )
      break;

    /* Set Coupling and Amplitude Register */
    car_entry = (i << 16) | i;
    (*p_api_dev->cb_p[ p_api_dev->biu ]).car = BSWAP32(car_entry);
    wbflush();
    api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

    /* wait */

    WAIT_CLOCKS(p_api_dev->clk_1us *1000 * 50);


    /* Start BC Simulation */
    api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 1 /*cnt*/, 100 *100 /*ftime = 10ms*/, 0);

    /* wait */

    WAIT_CLOCKS(p_api_dev->clk_1us *1000);

    /* Read BM Activity List */
    api_bm_rt_sa_act_read(p_api_dev, 1 /*rt*/, 1 /*sa*/, API_RT_SA_TYPE_RCV /*sa_type*/);

    /* Message Counter */
    if(p_api_dev->bm_rt_act_dsp.cc != j)
    {
      /*PRINTF2("BM Activity List Message error (DAC) (%d): %08x\n\r",j,p_api_dev->bm_rt_act_dsp.cc);*/
      /*flag = BITE_ERROR;*/
      break;
    }

    if(p_api_dev->bm_rt_act_dsp.ec != 0)
    {
      /*PRINTF2("BM Activity List Error error (DAC) (%d): %08x\n\r",j,p_api_dev->bm_rt_act_dsp.ec);*/
      /*flag = BITE_ERROR;*/
      break;
    }

    /* Stop BC Simulation */
    api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);
  }

  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );



  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }
  /* Set Coupling and Amplitude Register to default values */
  /* 01.04.99: default value = 0x00800080 -> 0x00FA00FA */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).car = BSWAP32(0x00FA00FAUL); /* Default Amplitude,Isolated */
  api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

  if( (i < 1) || (i > 60) )
  {
    flag = BITE_ERROR;
    PRINTF1("Amplitude: %d\n\r", i+1);
  }

  return (flag);
} /* end: api_bite_dac_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_BC_BR_TEST       */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.02.01   Update :  07.02.02                                  */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    :                       									 */
/*                                                                           */
/*    Outputs   : status indication [flag]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes BC-BR transfers to SA1-SA30                     */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_minor_framing_time_test(TY_API_DEV *p_api_dev, BYTE mode, BYTE bus, L_WORD ftime, BYTE con)
{
  BYTE rt, sa, wcnt, fid;
  WORD xid, hid, bid;
  BYTE flag;
  L_WORD frames[32];
  WORD i, j, k;
  WORD rep_w, cw;
  L_WORD prev_ttag, cur_ttag;
  L_WORD volatile *lptr;
  L_WORD addr, cw_entry, dw_entry, entry_1, entry_2;
  L_WORD lSec;
  L_WORD lDay, lHour, lMin;
  TY_BC_INSTRUCTION xfers[32];

  TY_MIL_COM_LS_RT_BH_DEF_INPUT       rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT      rt_bh_def_out;
  TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT rt_status_read_out;
  TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT  rt_sa_msg_read_in;
  TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT rt_sa_msg_read_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_rt_in;
  TY_MIL_COM_LS_BUF_DEF_INPUT         buf_def_bc_in;
  TY_MIL_COM_LS_BUF_READ_INPUT        buf_read_in;
  TY_MIL_COM_LS_BUF_READ_OUTPUT       buf_read_out;

  prev_ttag = 0L;
  cur_ttag = 0L;
  rep_w = 0;
  flag = BITE_OK;

  /* Reset */
  api_sys_reset(p_api_dev, 1);

  switch(mode)
  {
    case BITE_CPL_WRAP:
      /* Wrap-around loop between on-board Encoder and Decoder */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_WRAP /*cpl*/);
      break;

    case BITE_CPL_ISO:
      /* Isolated coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      break;

    case BITE_CPL_TRF:
      /* Only for TEST Implementation */
      /* Transformer coupling */
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_TRF /*cpl*/);
      break;

    default:
      api_cal_cpl_con(p_api_dev, API_CAL_PRI_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
      api_cal_cpl_con(p_api_dev, API_CAL_SEC_BUS /*bus*/, API_CAL_CPL_ISO /*cpl*/);
  }

  /* Init BC */
  api_bc_ini(p_api_dev, 0 /*retr*/, 0 /*svrq*/, 0, 0); /* 07.02.02 */

  /* BC-Broadcast Transfer */
  i = 1;
  xid = i;
  hid = i;
  bid = i;
  rt = 31;
  sa = (BYTE)i;
  wcnt = (BYTE)i;

  api_bc_xfer_def(p_api_dev, xid, hid, 0 /*BC-RT xfer_type*/, bus /*PRI chn*/, 0 /*xmt_rt*/, rt /*rcv_rt*/, 0 /*xmt_sa*/, sa /*rcv_sa*/, wcnt, 0 /*xfer_ir*/, 0 /*xfer_hlt*/, 0 /*res1*/, 0 /*res2*/, 2 /*resp_exp1*/ /*01.02.99*/, 0 /*resp_exp2*/, 0 /*swxm*/, 0 /*type*/, 0 /*wpos*/, 0 /*bpos*/, 0 /*sync*/, 0 /*contig*/, 0 /*bc_bits*/, 0 /*delay_mode*/, 0 /*gap*/, NULL);

  /* Buffer Header */
  api_bc_bhd_def(p_api_dev, hid, bid, 0 /*sid*/, 0 /*eid*/, 0 /*qsize*/, 0 /*bqm*/, 0 /*bsm*/, 0 /*sts*/, 0 /*evs*/, 0 /*res*/);



  buf_def_bc_in.bt     = API_BUF_BC_TYPE;
  buf_def_bc_in.hid    = 0;
  buf_def_bc_in.bid    = bid;
  buf_def_bc_in.count  = 32;

  /* Define Buffer */
  for(j = 0; j < 32; j++)
  {
      buf_def_bc_in.data[j] = i;
  }

  api_buf_def(p_api_dev, &buf_def_bc_in, NULL );

  /* Minor Frame */
  xfers[0].instr = API_BC_INSTR_XFER_CODE;
  xfers[0].xid   = xid;

  fid = 1;
  api_bc_frame_def(p_api_dev, fid /*frame_id*/, 1 /*xfer_cnt*/, xfers);

  /* Major Frame */
  frames[ 0 ] = fid;
  api_bc_mframe_def(p_api_dev, 1 /*frame_cnt*/, frames);

  /* Init RT Buffer */
  for(k = 0; k < 32; k++)
  {
    buf_def_rt_in.data[k] = 0;
  }

  /* Init RT */
  api_rt_ini(p_api_dev, 31 /*rt_addr*/, API_RT_ENABLE /*con*/, 0 /*bus*/, 8 /* resp_time*/, 0xF800); /* 01.02.00 new Firmware (ms) */

  i = 1;
  hid = i;
  bid = (i + 30);


    rt_bh_def_in.hid   = hid;
    rt_bh_def_in.bid   = bid;
    rt_bh_def_in.sid   = 0;
    rt_bh_def_in.eid   = 0;
    rt_bh_def_in.qsize = 0;
    rt_bh_def_in.bqm   = 0;
    rt_bh_def_in.bsm   = 0;
    rt_bh_def_in.sqm   = 0;
    rt_bh_def_in.eqm   = 0;
    rt_bh_def_in.res   = 0;

    api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );

  buf_def_rt_in.bt     = API_BUF_RT_TYPE;
  buf_def_rt_in.hid    = 0;
  buf_def_rt_in.bid    = bid;
  buf_def_rt_in.count  = 32;

  api_buf_def(p_api_dev, &buf_def_rt_in, NULL );

  api_rt_sa_con(p_api_dev, 31 /*rt_addr*/, (BYTE)i /*sa_mc*/, hid, API_RT_SA_TYPE_RCV /* sa_type*/, API_RT_SA_ENABLE /* con*/, 0 /* rmod*/, 0 /* smod*/, 0 /*swm*/);

  /* Bus Monitor */
  if(con == API_ON)
  {
    api_bm_ini(p_api_dev);
    api_bm_ini_intr(p_api_dev, API_SCB_IMOD_DIS /*imod*/, API_SCB_SMOD_DIS /* smod*/);
    api_bm_ini_cap(p_api_dev, API_BM_CMOD_ALL /* cmod*/, 10000l /* tat*/, 0 /*mcc*/, 0 /*fsize*/);
    api_bm_mode(p_api_dev, API_BUSY);
  }

  /* Start RT */
  api_rt_mode(p_api_dev, API_BUSY );


  /* Start BC Simulation */
  api_bc_mode(p_api_dev, API_BUSY, API_BC_XFER_RUN_IMM, 100-(ftime / 1000) /*cnt*/, ftime /*ftime*/, 0);

  /* wait */

  WAIT_CLOCKS(p_api_dev->clk_1us *ftime *(100-(ftime / 1000) ) );

  /* Stop BC Simulation */
  api_bc_mode(p_api_dev, API_HALT, 0, 0, 0, 0);

  /* Stop RT */
  api_rt_mode(p_api_dev, API_HALT );



  /* Stop Bus Monitor */
  if(con == API_ON)
  {
    api_bm_mode(p_api_dev, API_HALT);
  }

  /* BC Verify */
  api_bc_status_read(p_api_dev);

  /* Verify global BC message count and error count */
  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt != (100-(ftime / 1000) ) )
  {
    PRINTF1("BC message count error: %d\n\r", p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt != 0)
  {
    PRINTF0("BC error count error\n\r");
    flag = BITE_ERROR;
    return (flag);
  }

  api_rt_status_read( p_api_dev, 0, &rt_status_read_out );

  /* Verify global RT message count and error count */
  if(rt_status_read_out.msg_cnt != (100-(ftime / 1000) ) )
  {
    PRINTF1("RT message count error: %d\n\r", rt_status_read_out.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(rt_status_read_out.err_cnt != 0)
  {
    PRINTF1("RT error count error: %d\n\r", rt_status_read_out.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify BC Transfer count and Transfer error count */
  i = 1;
  api_bc_xfer_read(p_api_dev, i /*xid*/, API_OFF /*clr_mode*/);


  if(p_api_dev->bc_xfer_dsp.msg_cnt != (100-(ftime / 1000) ) )
  {
    PRINTF1("BC transfer message count error: %d\n\r", p_api_dev->bc_xfer_dsp.msg_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  if(p_api_dev->bc_xfer_dsp.err_cnt != 0)
  {
    PRINTF1("BC transfer error count error: %d\n\r", p_api_dev->bc_xfer_dsp.err_cnt);
    flag = BITE_ERROR;
    return (flag);
  }

  /* Verify BC transfer timetag */
  cur_ttag = p_api_dev->bc_xfer_dsp.cur_ttag;
  if(cur_ttag <= prev_ttag)
  {
    PRINTF1("BC transfer timetag error: %08x\n\r", cur_ttag);
    flag = BITE_ERROR;
    return (flag);
  }

  prev_ttag = cur_ttag;

  /* Verify BC transfer report word */
  rep_w = p_api_dev->bc_xfer_dsp.cur_brw &0x300F;
  if(rep_w != 0x2000)
  {
     /* Bus flag don't care for BC-BR */ /* 01.02.99 */
    PRINTF1("BC report word error: %04x\n\r", rep_w);
    flag = BITE_ERROR;
    return (flag);
  }

  prev_ttag = 0L;
  /* Verify RT Transfer count and Transfer error count */
  i = 1;

  rt_sa_msg_read_in.rt_addr  = 31;
  rt_sa_msg_read_in.sa_mc    = i;
  rt_sa_msg_read_in.sa_type  = API_RT_SA_TYPE_RCV;
  rt_sa_msg_read_in.clr_mode = API_OFF;
  api_rt_sa_msg_read( p_api_dev, &rt_sa_msg_read_in, &rt_sa_msg_read_out );


  /* additionally RT transfer counter and RT error counter must be checked here */


  /* Verify RT transfer timetag */

  cur_ttag = rt_sa_msg_read_out.ttag;
  if(cur_ttag < prev_ttag)
  {
    PRINTF0("RT transfer timetag error\n\r");
    PRINTF1("prev_ttag: %08x\n\r", prev_ttag);
    PRINTF1("cur_ttag:  %08x\n\r", cur_ttag);
    flag = BITE_ERROR;
    return (flag);
  }
  prev_ttag = cur_ttag;

  /* Coding Error in RT report word: 10110000 */
  /* Verify RT transfer report word */
  rep_w = rt_sa_msg_read_out.brw & 0x301F;

  if(bus == 0)
  {
     /* Bus A */
    if(rep_w != 0x1010)
    {
      PRINTF1("RT report word (Bus A) error: %04x\n\r", rep_w);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  if(bus == 1)
  {
     /* Bus B */
    if(rep_w != 0x1000)
    {
      PRINTF1("RT report word (Bus B) error: %04x\n\r", rep_w);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  /* Last Command word */
  cw = (31 << 11) | (i << 5) | i;
  if(rt_sa_msg_read_out.lcw != cw)
  {
    PRINTF1("RT command word error: %04x\n\r", cw);
    PRINTF1("RT command word error: %04x\n\r", rt_sa_msg_read_out.lcw);
    flag = BITE_ERROR;
    return (flag);
  }
  /* Test RT-Receive buffer */
  hid = 0;
  bid = (i + 30);

  buf_read_in.bt    = API_BUF_RT_TYPE;
  buf_read_in.hid   = hid;
  buf_read_in.bid   = bid;
  buf_read_in.count = 32;
  api_buf_read( p_api_dev, &buf_read_in, &buf_read_out );

  /* Read data from buffer */
  for(k = 0; k < i; k++)
  {
    if(buf_read_out.data[k] != i)
    {
      PRINTF2("RT data word1 error exp %x  det %x\n\r", i, buf_read_out.data[k]);
      flag = BITE_ERROR;
      return (flag);
    }
  }
  for(k = i; k < 32; k++)
  {
    if(buf_read_out.data[k] != 0)
    {
      PRINTF2("RT data word2 error exp %x det %x\n\r", 0, buf_read_out.data[k]);
      flag = BITE_ERROR;
      return (flag);
    }
  }

  /* Verify BM */
  if(con == API_ON)
  {
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf; /* Starting entry */
    lptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);
    for(j = 1; j <= (100-(ftime / 1000) ); j++)
    {
      i = 1;
      lMin = (BSWAP32(*lptr) &0x0000003FL);
      lHour = (BSWAP32(*lptr) &0x000007C0L) >> 6;
      lDay = ( (BSWAP32(*lptr) &0x000FF800L) >> 11);
      lptr++;

      lSec = (BSWAP32(*lptr) &0x03F00000L) >> 20;
      /* get microseconds */

      cur_ttag = lDay * 24 * 60 * 60 * 1000 * 1000+lHour *60 * 60 * 1000 * 1000+lMin *60 * 1000 * 1000+lSec *1000 * 1000+(BSWAP32(*lptr) &0x000FFFFFL);

      /*        	fTime = (*lptr & 0x000FFFFFL);
       *            lMilSec = fTime/1000.0;
       *            lMicSec = fTime - (lMilSec*1000);
       *            PRINTF3("        TT-Lo %ld:%03ld:%03ldus\n\r",
       *                    lSec,lMilSec,lMicSec);
       */
      lptr++;



      if(j > 3)
      {
        if( ( (cur_ttag - prev_ttag) < (ftime - 10) ) || ( (cur_ttag - prev_ttag) > (ftime + 10) ) )
        {
          PRINTF3("[%04d] cur_ttag[%X] prev[%X]\n\r", j, cur_ttag, prev_ttag);
          PRINTF2("           delta[%d] ftime[%d]\n\r", ftime - (cur_ttag - prev_ttag), ftime);
          flag = BITE_ERROR;
          return (flag);
        }
      }
      prev_ttag = cur_ttag;


      if(bus == 0)
      {
         /* Bus A */
        cw_entry = 0x80000000L | (31 << 11) | (i << 5) | i;
        dw_entry = 0xA0000000L | i;
      }
      else
      {
         /* Bus B*/
        cw_entry = 0xC0000000L | (31 << 11) | (i << 5) | i;
        dw_entry = 0xE0000000L | i;
      }
      entry_1 = BSWAP32(*lptr) &0xF000FFFFL;
      if(cw_entry != entry_1)
      {
        PRINTF2("BM cw_entry[%X] error: %08x\n\r", j, entry_1);
        flag = BITE_ERROR;
        return (flag);
      }
      lptr++;

      /* Check Data Words */
      for(k = 0; k < i; k++)
      {
        entry_2 = BSWAP32(*lptr) &0xF000FFFFL;
        if(dw_entry != entry_2)
        {
          PRINTF1("BM dw_entry error: %08x\n\r", entry_2);
          flag = BITE_ERROR;
          PRINTF1("BC-BR Test DW%d Error", i);
          return (flag);
        }
        lptr++;
      }
    }
  }
  return (flag);
} /* end: api_bite_minor_framing_time_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_TRANSFER_TEST    */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 01.11.98                                   */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : status indication [echo]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes basic MilBus transfers (BC-RT, RT-RT, BC-BR).   */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_transfer_test(TY_API_DEV *p_api_dev)
{
  BYTE echo = BITE_OK;
  TY_MIL_COM_LS_SYS_CAL_CON_INPUT sys_cal_con_in;
  TY_MIL_COM_ACK                  ack;

  sys_cal_con_in.bus = API_CAL_PRI_BUS;
  sys_cal_con_in.con = 255;

  api_cal_xmt_con( p_api_dev, &sys_cal_con_in, &ack );

  sys_cal_con_in.bus = API_CAL_SEC_BUS;
  sys_cal_con_in.con = 255;

  api_cal_xmt_con( p_api_dev, &sys_cal_con_in, &ack );

  while(1)
  {
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_A, API_OFF);
    if(echo != BITE_OK)
    {
      echo = 1;
      break;
    }
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_B, API_OFF);
    if(echo != BITE_OK)
    {
      echo = 2;
      break;
    }
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_A, API_ON);
    if(echo != BITE_OK)
    {
      echo = 3;
      break;
    }
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_B, API_ON);
    if(echo != BITE_OK)
    {
      echo = 4;
      break;
    }
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
    if(echo != BITE_OK)
    {
      echo = 5;
      break;
    }
    echo = api_bite_bc_br_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
    if(echo != BITE_OK)
    {
      echo = 6;
      break;
    }
    echo = api_bite_bc_rt_test(p_api_dev, BITE_CPL_ISO, API_ON);
    if(echo != BITE_OK)
    {
      echo = 7;
      break;
    }
    echo = api_bite_rt_rt_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
    if(echo != BITE_OK)
    {
      echo = 8;
      break;
    }
    echo = api_bite_rt_rt_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
    if(echo != BITE_OK)
    {
      echo = 9;
      break;
    }
    echo = api_bite_mc_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
    if(echo != BITE_OK)
    {
      echo = 10;
      break;
    }
    echo = api_bite_mc_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
    if(echo != BITE_OK)
    {
      echo = 11;
      break;
    }
    echo = api_bite_bc_resp_tout_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
    if(echo != BITE_OK)
    {
      echo = 12;
      break;
    }
    echo = api_bite_bc_resp_tout_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
    if(echo != BITE_OK)
    {
      echo = 13;
      break;
    }
    break;
  }
  return (echo);
} /* end: api_bite_transfer_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE_TRANSFER_TEST    */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 14.02.01   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : status indication [echo]                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function executes Timing tests.                                   */
/*                                                                           */
/*****************************************************************************/

BYTE api_bite_timing_test(TY_API_DEV *p_api_dev)
{
  BYTE echo;

  echo = BITE_OK;

  while(1)
  {
    echo = api_bite_minor_framing_time_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_B, 50000 /*50ms*/, API_ON);
    if(echo != BITE_OK)
    {
      echo = 1;
      break;
    }
    echo = api_bite_minor_framing_time_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_B, 10000 /*10ms*/, API_ON);
    if(echo != BITE_OK)
    {
      echo = 2;
      break;
    }
    echo = api_bite_minor_framing_time_test(p_api_dev, BITE_CPL_WRAP, BITE_BUS_B, 1000 /*1ms*/, API_ON);
    if(echo != BITE_OK)
    {
      echo = 3;
      break;
    }
    /*echo = api_bite_bc_br_test(p_api_dev,BITE_CPL_WRAP, BITE_BUS_B, API_OFF);*/
    if(echo != BITE_OK)
    {
      echo = 2;
      break;
    }
    /*echo = api_bite_bc_br_test(p_api_dev,BITE_CPL_WRAP, BITE_BUS_A, API_ON);*/
    if(echo != BITE_OK)
    {
      echo = 3;
      break;
    }
    break;
  }
  return (echo);
} /* end: api_bite_transfer_test */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BITE                 Submodule : API_BITE                  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Selftest Mode (all/single) [mode]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function contains the selftest functions of the ASP Driver SW:    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_bite( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_BITE_INPUT * in, TY_MIL_COM_SYS_BITE_OUTPUT * out )
{
  BYTE echo;
  WORD status;
  L_WORD bm_size;
  BYTE disable_reset;
  BYTE mode = (BYTE)in->mode;

  echo = 0;
  status = 0;

  /*PRINTF1("Selftest 1553 on biu %d\n\r ",p_api_dev->biu);*/

  p_api_dev->bite_stream[ p_api_dev->biu ] = API_ON;


  /* Extract reset disable bit from mode */
  disable_reset  = (mode & 0x80);
  mode          &= ~0x80;



  /*** Selftest Implementation ****/
  if(mode == 0)
  {

    while(1)
    {

      /* Board Enable test */
      echo = api_bite_board_en_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 1;
        break;
      }

      /* Internal BITE test */
      echo = api_bite_int_selftest(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 2;
        break;
      }

      /* BIU CB area */
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr + 0x100L, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr + 0x10000L, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 3;
        break;
      }

      /* BIU BCRT DESC area */
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sa_desc, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sa_desc + 0x20000L, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 4;
        break;
      }

      /* BIU MON area */
      bm_size = p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;

      if(bm_size > 100000 )
          bm_size = 100000;
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + bm_size, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 5;
        break;
      }

#if defined(_NUCLEUS)
      /* Shared RAM test */
      echo = api_bite_paging_test(p_api_dev, 0x100000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 6;
        break;
      }

      /* Shared RAM Test, CMD area */
      echo = api_bite_ramtest(p_api_dev, 0x2100L, 0x4000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 7;
        break;
      }

      /* Shared RAM Test, ACK area */
      echo = api_bite_ramtest(p_api_dev, 0x5100L, 0x6000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 8;
        break;
      }
#endif 
      /* Basic Transfer Tests */
      echo = api_bite_transfer_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 9;
        break;
      }

      /* BC Interrupt Tests */
      echo = api_bite_bc_intr_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 10;
        break;
      }

#ifndef _AMC1553
       /*  D/A-Converter Tests not possible with DBTE PBI (0x1F)*/ /*BUILD0001*/
      if( ( (MEM(GLBRAM_BP + BIU_VERSION_O) &0x00ff0000) >> 16) != 0x1F)
      {

        /* D/A-Converter Tests Bus A */
        echo = api_bite_dac_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
        if(echo != BITE_OK)
        {
          status = 11;
          break;
        }

        /* D/A-Converter Tests Bus B */
        echo = api_bite_dac_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
        if(echo != BITE_OK)
        {
          status = 12;
          break;
        }
      }


      echo = api_bite_timing_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 13;
        break;
      }
#endif 
      break; /* end BITE */

    }
  }

  else
  {
    switch(mode)
    {

      case (1):  /* Board Enable test */
      echo = api_bite_board_en_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 1;
        break;
      }
      break;

      case (2):  /* Internal BITE test */
      echo = api_bite_int_selftest(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 2;
        break;
      }
      break;

      case (3):  /* BIU CB area */
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr + 0x100L, p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr + 0x10000L, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 3;
        break;
      }
      break;

      case (4):  /* BIU BCRT DESC area */
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sa_desc, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sa_desc + 0x20000L, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 4;
        break;
      }
      break;

      case (5):  /* BIU MON area */
      echo = api_bite_ramtest(p_api_dev, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf, BITE_GLB_MEM);
      if(echo != BITE_OK)
      {
        status = 5;
        break;
      }
      break;

      case (6):  /* Shared RAM test */
#if defined(_NUCLEUS)
      echo = api_bite_paging_test(p_api_dev, 0x100000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 6;
        break;
      }
#endif
      break;

      case (7):  /* Shared RAM Test, CMD area */
#if defined(_NUCLEUS)
      echo = api_bite_ramtest(p_api_dev, 0x2100L, 0x4000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 7;
        break;
      }
#endif
      break;

      case (8):  /* Shared RAM Test, ACK area */
#if defined(_NUCLEUS)
      echo = api_bite_ramtest(p_api_dev, 0x5100L, 0x6000L, BITE_SHARED_RAM);
      if(echo != BITE_OK)
      {
        status = 8;
        break;
      }
#endif
      break;

      case (9):  /* Basic Transfer Tests */
      echo = api_bite_transfer_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 9;
        break;
      }
      break;

      case (10):  /* BC Interrupt Tests */
      echo = api_bite_bc_intr_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 10;
        break;
      }
      break;

      case (11):  /* D/A-Converter Tests Bus A */
#ifndef _AMC1553
      echo = api_bite_dac_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_A, API_ON);
      if(echo != BITE_OK)
      {
        status = 11;
        break;
      }
#endif
      break;

      case (12):  /* D/A-Converter Tests Bus B */
#ifndef _AMC1553
      echo = api_bite_dac_test(p_api_dev, BITE_CPL_ISO, BITE_BUS_B, API_ON);
      if(echo != BITE_OK)
      {
        status = 12;
        break;
      }
#endif
      break;
      
      case (13):  /* Timing Tests */
#ifndef _AMC1553
      echo = api_bite_timing_test(p_api_dev);
      if(echo != BITE_OK)
      {
        status = 13;
        break;
      }
#endif
      break;
    }
  }

  if(echo != BITE_OK)
    PRINTF2("LS-Selftest Error: status=%03d  echo=%03d\r\n", status, echo);

  out->result = status;
  out->result <<= 8;
  out->result |= echo;

  if( (echo == BITE_OK) && ( (mode == 0) || (mode >= 100) ) )
  {
    /* 3910 Board only */
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
    {
      if( (uc_HsSubAddress >= 1) && (uc_HsSubAddress < 31) )
      { 
        /* only execute 3910 BITE, if HS SubAddress is valid */
        /* if set to 0xFF, the board is acting like a pure 1553 board */
        status = api39_bite(p_api_dev, mode);

        out->result = status;

        if( status != 0 )
          PRINTF2("HS-Selftest Error: status=%03d  echo=%03d\r\n", (status>>8), (echo & 0xFF) );
      }
    }
  }

  p_api_dev->bite_stream[ p_api_dev->biu ] = API_OFF;

  if( disable_reset == 0x80 )
  {
      /* Reset can be disabled, so the error
         can be investigated within the global RAM
         after an BITE failure.
      */
      PRINTF0("Warning: Reset is disabled for debugging purpose\r\n");
  }
  else
  {
      api_sys_reset(p_api_dev, 0);

      if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
          api39_sys_reset(p_api_dev, 0, 0);
  }
  
  return 0;

} /* end: api_bite */

NAMESPACE1553_END

