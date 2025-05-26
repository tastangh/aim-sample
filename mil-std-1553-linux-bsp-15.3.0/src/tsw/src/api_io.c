/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_io.c
    Hardware interface module of the 1553 Driver Software.               
    This module comprises service functions for accessing the 1553       
    hardware and implements the interface to the 1553 hardware           
    within the 1553 ASP Driver Software.                                 
*/

#include "mil_tsw_includes.h"




NAMESPACE1553_START



/*****************************************************************************/
/***                    Buffer Functions                                   ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_WRITE_LWORD        */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                Data to write [lval]                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to write a single Longword to the API1553-DS     */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_write_lword(TY_API_DEV *p_api_dev, L_WORD addr, L_WORD lval)
{
  L_WORD volatile *ptr;

  addr &= 0x03FFFFFCL; /* even Byte address, divisible by 4 */

  /* Set pointer to Global Memory address */
  ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Write data to the Global Memory */
  *ptr = BSWAP32(lval);

  UsbSynchronizeMemoryArea(p_api_dev, addr, sizeof(AiUInt32), Out);

} /* end: api_io_write_lword */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_READ_LWORD         */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                                                                           */
/*    Outputs   : Data to read [lval]                                        */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to read a single Longword from the API1553-DS    */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_read_lword(TY_API_DEV *p_api_dev, L_WORD addr)
{
  L_WORD volatile *ptr;
  L_WORD lval;

  addr &= 0x03FFFFFCL; /* even Byte address, divisible by 4 */

  UsbSynchronizeActiveMemoryBlocks(p_api_dev, addr, sizeof(AiUInt32), In);

  /* Set pointer to Global Memory address */
  ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Read data from the Global Memory */
  lval = BSWAP32(*ptr);

  return (lval);

} /* end: api_io_read_lword */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_WRITE_WORD         */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                Data to write [wval]                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to write a single Word to the API1553-DS         */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_write_word(TY_API_DEV *p_api_dev, L_WORD addr, WORD wval)
{
  WORD volatile *ptr;

  addr &= 0x03FFFFFEL; /* even Byte address, divisible by 2 */

  /* Set pointer to Global Memory address */
  ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Write data to the Global Memory */
  *ptr = BSWAP16(wval);

  UsbSynchronizeMemoryArea(p_api_dev, addr, sizeof(AiUInt16), Out);

} /* end: api_io_write_word */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_READ_WORD          */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                                                                           */
/*    Outputs   : Data to read [wval]                                        */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to read a single Word from the API1553-DS        */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

WORD api_io_read_word(TY_API_DEV *p_api_dev, L_WORD addr)
{
  WORD volatile *ptr;
  WORD wval;

  addr &= 0x03FFFFFEL; /* even Byte address, divisible by 2 */

  UsbSynchronizeActiveMemoryBlocks(p_api_dev, addr, sizeof(AiUInt16), In);

  /* Set pointer to Global Memory address */
  ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Read data from the Global Memory */
  wval = BSWAP16(*ptr);

  return (wval);

} /* end: api_io_read_word */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_WRITE_BYTE         */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                Data to write [wval]                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to write a single Byte to the API1553-DS         */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_write_byte(TY_API_DEV *p_api_dev, L_WORD addr, BYTE bval)
{
  BYTE volatile *ptr;

  /* Set pointer to Global Memory address */
  ptr = (BYTE*)API_GLB_MEM_ADDR_ABS(addr);

  /* Write data to the Global Memory */
  *ptr = bval;

  UsbSynchronizeMemoryArea(p_api_dev, addr, sizeof(AiUInt8), Out);

} /* end: api_io_write_byte */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_READ_BYTE          */
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
/*    Inputs    : Global RAM Address [addr]                                  */
/*                                                                           */
/*    Outputs   : Data to read [bval]                                        */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to read a single Byte from the API1553-DS        */
/*    Global RAM.                                                            */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_read_byte(TY_API_DEV *p_api_dev, L_WORD addr)
{
  BYTE volatile *ptr;
  BYTE bval;

  UsbSynchronizeActiveMemoryBlocks(p_api_dev, addr, sizeof(AiUInt8), In);

  /* Set pointer to Global Memory address */
  ptr = (BYTE*)API_GLB_MEM_ADDR_ABS(addr);

  /* Read data from the Global Memory */
  bval =  *ptr;

  return (bval);

} /* end: api_io_read_byte */

/*****************************************************************************/
/***                     System Control Block                              ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_MODE_CON           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 25.03.19                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : API1553-DS mode (RT/BC/SFT/CAL) [mode],                    */
/*                API1553-DS mode control (Enable/Disable) [con]             */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to enable or disable the specified API1553-DS    */
/*    operational mode.                                                      */
/*                                                                           */
/*****************************************************************************/
void api_io_mode_con(TY_API_DEV *p_api_dev, BYTE mode, BYTE con)
{
  L_WORD scw, ssw, cmd_inout;
  L_WORD scw_command;
  L_WORD count;
  L_WORD bit_pos;
  L_WORD bits_set = 0;

  scw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).scw);
  wbflush(); /* System Control Word */
  scw &= 0x07FFFFFFL;
  ssw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).ssw);
  wbflush(); /* System Status Word */
  switch(mode)
  {
    case API_RT_MODE:
      if(con == API_ON)
      {

        /* RT Operation stopped before ? */
        if (((ssw & API_SSW_RTE_BIT) == 0L) &&  /*((ssw & API_SSW_RPE_BIT) == 0L) && */ /* Replay disabled ? */
            ((ssw & API_SSW_SFE_BIT) == 0L))
        {
           /* Selftest disabled ? */

          /* Check if BIU is enabled */
            if ((ssw & API_SSW_OPE_BIT) == 0L)
          {
              /* Enable BIU Operation */
              ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);
          }

          UsbSynchronizeRT(p_api_dev, p_api_dev->biu, Out);

          /* Enable RT Operation */
          cmd_inout = 0;
          if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_ACTIVATE_SYNC_MC))
              /* enable sync counter if supported by FW */
              cmd_inout |= API_SCB_COM_INOUT_64US_CNT;

          if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_SINGLE_OPERATION))
              /* set single RT address and execute single RT mode if supported by FW */
          {
              if (p_api_dev->single_rt_addr[p_api_dev->biu])
              {
                  cmd_inout |= API_SCB_COM_INOUT_SINGLE_RT;
                  cmd_inout |= (p_api_dev->single_rt_addr[p_api_dev->biu] & API_SCB_COM_INOUT_RTADDR_MASK) << API_SCB_COM_INOUT_RTADDR_POS;

                  /* evaluate parity of RT address */
                  for (bit_pos = 0; bit_pos < 8; bit_pos++)
                  {
                      if (p_api_dev->single_rt_addr[p_api_dev->biu] & (1 << bit_pos))
                          bits_set++;
                  }

                  if ((bits_set % 2) == 0) /* if needed, add parity bit for RT address to get odd parity */
                      cmd_inout |= API_SCB_COM_INOUT_RTADDR_PARITY;
              }
          }

          (*p_api_dev->cb_p[p_api_dev->biu]).cmd_inout = BSWAP32(cmd_inout);

          ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_RT);
        }
        /*PRINTF0("RT started !\n\r");*/
      }
      else
      {

        /* RT Operation started before ? */
        if ((ssw & API_SSW_RTE_BIT) == API_SSW_RTE_BIT)
        {

          /* Disable RT Operation */
          ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_RT);
      
          UsbSynchronizeRT(p_api_dev, p_api_dev->biu, In);
        }
        /*PRINTF0("RT stopped !\n\r");*/
      }
      break;

    case API_BC_MODE:
        if(con == API_ON)
        {

            /* BC Operation stopped before ? */
            if (((ssw & API_SSW_BCE_BIT) == 0L) && ((ssw & API_SSW_RPE_BIT) == 0L) &&  /* Replay disabled ? */
                ((ssw & API_SSW_SFE_BIT) == 0L))
            {
                /* Selftest disabled ? */

                /* Check if BIU is enabled */
                if ((ssw & API_SSW_OPE_BIT) == 0L)
                {
                    /* Enable BIU Operation */
                    ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);
                }

                UsbSynchronizeBC(p_api_dev, p_api_dev->biu, Out);

                if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_TRAFFIC_OVERRIDE))
                {
                    /* enable traffic override if enabled and supported by FW */
                    if (p_api_dev->bc_traffic_override_enabled[p_api_dev->biu])
                    {
                        (*p_api_dev->cb_p[p_api_dev->biu]).cmd_inout = BSWAP32(API_SCB_COM_INOUT_BC_TRAFFIC_OVERRIDE);
                    }
                    else
                    {
                        (*p_api_dev->cb_p[p_api_dev->biu]).cmd_inout = BSWAP32(0);
                    }
                }

                /* Enable BC Operation */
                ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BC);
            }
        }
        else
        {
            /* BC Operation started before ? */
            if ((ssw & API_SSW_BCE_BIT) == API_SSW_BCE_BIT)
            {

                /* Disable BC Operation */
                ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BC);
            }

            if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeBC(p_api_dev, p_api_dev->biu, In);
            }
        }
      break;

    case API_BM_MODE:
      if(con == API_ON)
      {

          /* BM Operation stopped before ? */
          if (((ssw & API_SSW_MNE_BIT) == 0L) &&
              ((ssw & API_SSW_SFE_BIT) == 0L))
          {
              /* Selftest disabled ? */

              /* Check if BIU is enabled */
              if ((ssw & API_SSW_OPE_BIT) == 0L)
              {
                  /* Enable BIU Operation */
                  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);
              }

              UsbSynchronizeBM(p_api_dev, p_api_dev->biu, Out);

              /* Enable BM Operation */
              ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BM);
          }
          /*PRINTF0("BM started !\n\r");*/
      }
      else
      {

        /* BM Operation started before ? */
        if ((ssw & API_SSW_MNE_BIT) == API_SSW_MNE_BIT)
        {
            /* Disable BM Operation */
            ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BM);

            UsbSynchronizeBM(p_api_dev, p_api_dev->biu, In);
        }
        /*PRINTF0("BM stopped !\n\r");*/
      }
      break;

    case API_REP_MODE:
      if(con == API_ON)
      {

        /* REP Operation stopped before ? */
        if (((ssw & API_SSW_RPE_BIT) == 0L) && ((ssw & API_SSW_BCE_BIT) == 0L) &&  /* BC disabled ? */
            ((ssw & API_SSW_SFE_BIT) == 0L))
        {
          /* Selftest disabled ? */

          /* Check if BIU is enabled */
          if ((ssw & API_SSW_OPE_BIT) == 0L)
          {
              /* Enable BIU Operation */
              ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU);
          }

          UsbSynchronizeReplay(p_api_dev, p_api_dev->biu, Out);

          /* Enable REP Operation */
          ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_REP);
       
        }
      }
      else
      {
        /* REP Operation started before ? */
        if ((ssw & API_SSW_RPE_BIT) == API_SSW_RPE_BIT)
        {
          /* Disable Replay Operation */
          ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_REP);

          UsbSynchronizeReplay(p_api_dev, p_api_dev->biu, In);

        }
      }
      break;

    case API_SFT_MODE:
      if(con == API_ON)
      {

        /* Selftest or other operation still in progress ? */
        if (((ssw & API_SSW_SFE_BIT) == 0L) && ((ssw & API_SSW_RTE_BIT) == 0L) && ((ssw & API_SSW_BCE_BIT) == 0L) && ((ssw & API_SSW_RPE_BIT) == 0L) && ((ssw & API_SSW_MNE_BIT) == 0L))
        {

          /* Check if BIU is enabled */
          if ((ssw & API_SSW_OPE_BIT) == 0L)
          {

            /* Enable BIU Operation */
            scw |= (API_SCB_COM_ENA_BIU << 27);
            (*p_api_dev->cb_p[p_api_dev->biu]).scw = BSWAP32(scw);
            wbflush();

            /* Check in Status Register */
            count = 0;
            while(1)
            {

              /* wait */

              WAIT_CLOCKS(10 *p_api_dev->clk_1us);
              count++;
              if(count > 10000)
              {
                PRINTF1("TIMEOUT: API_SSW_OPE_BIT stat[%X]\n\r", ssw);
                break;
              }

              ssw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).ssw);
              wbflush();
              if( (ssw & API_SSW_OPE_BIT) == API_SSW_OPE_BIT)
                break;
            }
          }

          /* Enable Selftest */
          scw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).scw);
          wbflush();
          scw &= 0x07FFFFFFL;
          scw |= (API_SCB_COM_ENA_BITE << 27);
          (*p_api_dev->cb_p[p_api_dev->biu]).scw = BSWAP32(scw);
          wbflush();

          /* Check in Status Register */
          count = 0;
          while(1)
          {

            /* wait */

            WAIT_CLOCKS(10 *p_api_dev->clk_1us);
            count++;
            if(count > 1000)
            {
              PRINTF1("TIMEOUT: API_SSW_SFE_BIT stat[%X]\n\r", ssw);
              break;
            }

            ssw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).ssw);
            wbflush();
            if ((ssw & API_SSW_SFE_BIT) == API_SSW_SFE_BIT)
              break;
          }
        }
      }
      break;

    case API_CAL_MODE:
      if(con == API_ON)
      {
          /* Read Coupling and Control Register */
          if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_RD_CAR) != API_OK)
              PRINTF0("CAL failed!\n\r");
      }
      break;

    case API_PROT_MODE:
      if(con == API_ON)
      {
          /* Read RT Protocol Register 1 and 2 */
          ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_RD_RTP);
      }
      break;

    case API_RD_IRG_MODE:
      if(con == API_ON)
      {

        /* Upadet System Time Register */
        scw_command = (API_SCB_COM_RD_IRIG << 27);
        scw |= scw_command;
        (*p_api_dev->cb_p[p_api_dev->biu]).scw = BSWAP32(scw);
        wbflush();

        /* Check in Status Register */
        count = 0;
        while(1)
        {
          /* wait */

          WAIT_CLOCKS(10 *p_api_dev->clk_1us);
          count++;
          if(count > 10000)
          {
            PRINTF1("TIMEOUT: API_SCB_COM_RD_IRIG stat[%X]\n\r", ssw);
            break;
          }

          ssw = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);
          wbflush();
          if ((ssw & 0xF8000000L) == scw_command)
            break;
        }
      }
      break;

    case API_AINTR_MODE:
      if(con == API_ON)
      {

        /* Read Coupling and Control Register */
        scw_command = (API_SCB_COM_RD_IRIG << 27);
        scw |= scw_command;
        (*p_api_dev->cb_p[p_api_dev->biu]).scw = BSWAP32(scw);
        wbflush();

        /* Check in Status Register */
        count = 0;
        while(1)
        {

          /* wait */

          WAIT_CLOCKS(10 *p_api_dev->clk_1us);
          count++;
          if(count > 10000)
          {
            PRINTF1("TIMEOUT: API_SCB_COM_ASP_INTR stat[%X]\n\r", ssw);
            break;
          }

          ssw = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).ssw);
          wbflush();
          if ((ssw & 0xF8000000L) == scw_command)
            break;
        }
      }
      break;

    case API_BIU_MODE:
      if(con == API_ON)
      {
          UsbSynchronizeSystemControlBlock(p_api_dev, p_api_dev->biu, Out);
        
          /* Enable BIU Operation */
          if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_ENA_BIU) == API_OK)
              p_api_dev->b_BiuEnabled[p_api_dev->biu] = TRUE;
      }
      else
      {

          /* Disable BIU Operation */
          if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BIU == API_OK))
              p_api_dev->b_BiuEnabled[p_api_dev->biu] = FALSE;

          UsbSynchronizeSystemControlBlock(p_api_dev, p_api_dev->biu, In);
      }
      break;

    case API_BC_CONT_MODE:
      if(con == API_ON)
      {

        /* Continue BC after Halt */
        scw_command = (API_SCB_COM_RD_IRIG << 27);
        scw |= scw_command;
        (*p_api_dev->cb_p[p_api_dev->biu]).scw = scw;
        wbflush();

        /* Check in Status Register */
        count = 0;
        while(1)
        {

          /* wait */

          WAIT_CLOCKS(10 *p_api_dev->clk_1us);
          count++;
          if(count > 10000)
          {
            PRINTF1("TIMEOUT: API_SCB_COM_BC_CONT stat[%X]\n\r", ssw);
            break;
          }

          ssw = (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw;
          wbflush();
          if ((ssw & 0xF8000000L) == scw_command)
            break;
        }
      }
      break;

    default:
      break;
  }
} /* end: api_io_mode_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_TEST_BIU           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.11.98   Update : 01.11.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : BIU Status [st]                                            */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to check if the selected BIU is active.          */
/*                                                                           */
/*****************************************************************************/

short api_io_test_biu(TY_API_DEV *p_api_dev)
{
  short s_Status = API_ERR;

  /* Issue Disable BIU Command and check if it worked*/
  s_Status = ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_DIS_BIU);

  return (s_Status);

} /* end: api_io_test_biu */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CB_INI             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.08.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Initialise ALL/BC/BT/BM section [mode]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the specified part of the System Control */
/*    Block of the selected BIU.                                             */
/*                                                                           */
/*****************************************************************************/

void api_io_cb_ini(TY_API_DEV *p_api_dev, short mode)
{
  L_WORD tmpl;
  int i;

  switch(mode)
  {
    case INIT_ALL:
      /* Init all System Control Register */
      tmpl = 0L;
      tmpl = (p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 0x10000L) - 1L;
      tmpl <<= 8; /* MBFS */
      tmpl |= API_SYS_RESP_TOUT_DEF; /* RSP */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl); /* System Control Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw = 0L; /* System Status Word */
       /* (*p_api_dev->cb_p[p_api_dev->biu]).cid = 0L; */ /* Configuration Identification Register */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].ir_log_addr); /* Interrupt Loglist Pointer */
       /* (*p_api_dev->cb_p[p_api_dev->biu]).car = 0L; */ /* Coupling and Amplitude Register */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 = 0L; /* RT Protocol Definition Register 1 */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 = 0L; /* RT Protocol Definition Register 2 */
       /* (*p_api_dev->cb_p[p_api_dev->biu]).stl = 0L; */ /* Reserved */ 
       /* (*p_api_dev->cb_p[p_api_dev->biu]).sth = 0L; */ /* Reserved */ 
       /* (*p_api_dev->cb_p[p_api_dev->biu]).lca_id = 0L; */ /* LCA Version / Revision */ 
       /* (*p_api_dev->cb_p[p_api_dev->biu]).glb_mem = 0L; */ /* Global Memory Size */
       /* (*p_api_dev->cb_p[p_api_dev->biu]).lca_chks = 0L; */ /* LCA-Checksum */
#ifdef CMPL4SIM
      /* Bus Controller and Remote Terminals */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = 0L; /* Major Frame Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mft = 0L; /* Minor Frame Time */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr); /* High Priority BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).lip = 0L; /* Low Priority BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).aip = 0L; /* Acyclic Event BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).res1 = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = 0L; /* System Command Input / Output Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc = 0L; /* RT Transfer Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec = 0L; /* RT Error Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_mc = 0L; /* BC Transfer Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_ec = 0L; /* BC Error Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_init = 0L; /* Sync Counter Init Value */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_tth = 0L; /* Sync Counter TT High */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_ttl = 0L; /* Sync Counter TT Low */
      
#else 
      /* set all zero */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mft = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).lip = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).aip = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).res1 = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_mc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_ec = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_init = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_tth = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_ttl = 0L;
      
#endif 

#ifdef CMPL4MON
      /* Bus Monitor */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_get = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Get Offset */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbba = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Base Address */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbfp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Fill Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).msw = 0L; /* Monitor Status Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(0x00FF0000L); /* Monitor Function Trigger Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = 0L; /* Monitor Trigger Index Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Start Trigger Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0L; /* Monitor Message Capture Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = BSWAP32(API_BM_TAT_DEF); /* Monitor Trace after Trigger Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).swxm = BSWAP32(API_BM_SWXM_DEF); /* Monitor Status Word Exception Mask */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_cnt = 0L; /* Monitor Message Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_err = 0L; /* Monitor Message Error Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_mc = 0L; /* Monitor Transfer Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_ec = 0L; /* Monitor Error Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_sec = 0L; /* Monitor Word Counter, secondary bus */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_pri = 0L; /* Monitor Word Counter, primary bus */
      if( p_api_dev->ulUseEnhancedBmActivityPage == 1 )
          (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_act_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].bm_act_addr; /* Monitor Activity Page Address */
      else
          (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_act_addr = 0;
#else 
      /* set all zero */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_get = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbba = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbfp = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).msw = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).swxm = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_cnt = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_err = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_mc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_ec = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_sec = 0L; /* Monitor Word Counter, secondary bus */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_pri = 0L; /* Monitor Word Counter, primary bus */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_act_addr = 0L;
#endif 

#ifdef CMPL4REP
      /* Replay */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rcw = 0L; /* Replay Control Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = 0L; /* Replay RT Enable Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbb = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf); /* Replay Buffer Base Address */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf); /* Replay Actula Buffer Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rec = 0L; /* Replay Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).raec = 0L; /* Replay Actual Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramto = 0L; /* Replay Absolute Minute Offset Register */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramio = 0L; /* Replay Absolute Microsecond Offset Register*/
      for(i = 0; i < 4; i++)
        (*p_api_dev->cb_p[ p_api_dev->biu ]).res4[ i ] = 0L;
       /* Reserved */
#else 
      /* set all zero */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rcw = 0L; /* Replay Control Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = 0L; /* Replay RT Enable Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbb = 0L; /* Replay Buffer Base Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbp = 0L; /* Replay Buffer Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rec = 0L; /* Replay Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).raec = 0L; /* Replay Actual Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramto = 0L; /* Replay Absolute Minute Offset Register */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramio = 0L; /* Replay Absolute Microsecond Offset Register*/
      for(i = 0; i < 4; i++)
        (*p_api_dev->cb_p[ p_api_dev->biu ]).res4[ i ] = 0L;
#endif 
      break;

#ifdef CMPL4SIM
    case INIT_BC:
      /* Bus Controller */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mfc = 0L; /* Major Frame Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mft = 0L; /* Minor Frame Time */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).hip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_hip_instr); /* High Priority BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).lip = 0L; /* Low Priority BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).aip = 0L; /* Acyclic Event BC Instruction List Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_mc = 0L; /* BC Global Message Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_ec = 0L; /* BC Global Error Counter */
      break;

    case INIT_RT:
      /* Remote Terminals */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc = 0L; /* RT Global Message Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec = 0L; /* RT Global Error Counter */
      break;
#endif 

#ifdef CMPL4MON
    case INIT_BM:
      /* Bus Monitor */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_get = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Get Offset */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbba = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Base Address */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mbfp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Buffer Fill Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).msw = 0L; /* Monitor Status Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(0x00FF0000L); /* Monitor Function Trigger Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = 0L; /* Monitor Trigger Index Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Monitor Start Trigger Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0L; /* Monitor Message Capture Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = BSWAP32(API_BM_TAT_DEF); /* Monitor Trace after Trigger Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).swxm = BSWAP32(API_BM_SWXM_DEF); /* Monitor Status Word Exception Mask */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_cnt = 0L; /* Monitor Message Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_msg_err = 0L; /* Monitor Message Error Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_mc = 0L; /* Monitor Transfer Counter */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_ec = 0L; /* Monitor Error Counter */
      if( p_api_dev->ulUseEnhancedBmActivityPage == 1 )
          (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_act_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].bm_act_addr; /* Monitor Activity Page Address */
      else
          (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_act_addr = 0;
      break;
#endif 

#ifdef CMPL4REP
    case INIT_REP:
      /* Replay */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rcw = 0L; /* Replay Control Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = 0L; /* Replay RT Enable Word */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbb = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf); /* Replay Buffer Base Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rbp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf); /* Replay Actual Buffer Pointer */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rec = 0L; /* Replay Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).raec = 0L; /* Replay Actual Entry Count */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramto = 0L; /* Replay Absolute Minute Offset Register */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).ramio = 0L; /* Replay Absolute Microsecond Offset Register*/
      break;
#endif 
    default:
      break;
  }

} /* end: api_io_cb_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RESP_TOUT_DEF      */
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
/*    Inputs    : SIM+BM Response Timeout [tout]                             */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the Response Timeout value for the       */
/*    selected BIU.                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_resp_tout_def(TY_API_DEV *p_api_dev, BYTE tout)
{

  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw &= BSWAP32(0xFFFFFF00L);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw |= BSWAP32(tout);

  if(p_api_dev->b_BiuEnabled[p_api_dev->biu])
  {
      UsbSynchronizeLWord(p_api_dev, 
                          API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->scw), Out);
  }

} /* end: api_io_resp_tout_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DEF_MILBUS_PROT   */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 01.02.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Milbus protocol type [mil_prot]                            */
/*                RT set mode [mode]                                         */
/*                RT [rt]                                                    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DEF_MILBUS_PROT' instruction to set the */
/*    Milbus Protocol register.                                              */
/*                                                                           */
/*****************************************************************************/

void api_io_milbus_prot_def(TY_API_DEV *p_api_dev, BYTE mil_prot, BYTE mode, BYTE rt)
{
  L_WORD tmpl;
  BYTE rt_val;

  switch(mode)
  {
    case API_RT_ALL:
       /* All RT's */
      if(mil_prot == API_MILBUS_PROT_A)
      {
        (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 = BSWAP32(0x55555555L);
        (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 = BSWAP32(0x55555555L);
      }
      else
      {
        if(mil_prot == API_MILBUS_PROT_B)
        {
          (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 = 0L;
          (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 = 0L;
        }
      }
      break;

    case API_RT_SING:
       /* Single RT */
      tmpl = 0;
      rt_val = 0;
      rt_val = rt &0x0F;

      if(mil_prot == API_MILBUS_PROT_A)
      {
        tmpl = 1L;
        tmpl <<= (rt_val *2);
        rt_val = rt &0x10; /* test if rtpd1 or rtpd2 */

        if(!rt_val)
        {
          (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 |= BSWAP32(tmpl); /* rtpd1 */
        }
        else
          (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 |= BSWAP32(tmpl);
         /* rtpd2 */
      }
      else
      {
        if(mil_prot == API_MILBUS_PROT_B)
        {
          tmpl = 3L;
          tmpl <<= (rt_val *2);
          rt_val = rt &0x10; /* test if rtpd1 or rtpd2 */
          tmpl = ~tmpl; /* noch fehlerhaft */

          if(!rt_val)
          {
            (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 &= BSWAP32(tmpl);
          }
          else
            (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 &= BSWAP32(tmpl);
        }
      }
      break;

    default:
      break;
  }

  UsbSynchronizeLWord(p_api_dev, 
                      API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[ p_api_dev->biu ]->rtpd1), Out);
  UsbSynchronizeLWord(p_api_dev, 
                      API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[ p_api_dev->biu ]->rtpd2), Out);

  /* Enable mode */
  api_io_mode_con(p_api_dev, API_PROT_MODE, API_ON);
} /* end: api_io_milbus_prot_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_MILPROT_INI        */
/*                                                                           */
/*   Author : Michael Michael          Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 01.02.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the MILbus Protocol mode of the          */
/*    selected BIU.                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_milprot_ini(TY_API_DEV *p_api_dev)
{

  (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd1 = 0L; /* default 1553B MILbus Protocol*/
  (*p_api_dev->cb_p[ p_api_dev->biu ]).rtpd2 = 0L;

  api_io_mode_con(p_api_dev, API_PROT_MODE, API_ON);

} /* end: api_io_milprot_ini */


/*****************************************************************************/
/***                     Interrupt Loglist                                 ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_ISLP_INI           */
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
/*    This function is used to init the Interrupt Status Loglist of the      */
/*    selected BIU.                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_islp_ini(TY_API_DEV *p_api_dev)
{
  short i;

  for(i = 0; i < MAX_API_ISLP_SIZE; i++)
  {
    (*p_api_dev->islp_p[ p_api_dev->biu ])[ i ].ir_status = 0x00000000L;
    (*p_api_dev->islp_p[ p_api_dev->biu ])[ i ].ir_ptr = BSWAP32(0x0000CAFEUL);
  }

} /* end: api_io_islp_ini */

/*****************************************************************************/
/***                     Simulator Buffer                                  ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_SIM_BUF_INI        */
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
/*    This function is used to initialize the API1553-DS SIM buffer area.    */
/*                                                                           */
/*****************************************************************************/

void api_io_sim_buf_ini(TY_API_DEV *p_api_dev)
{
  L_WORD volatile *ptr;
  L_WORD i;
  ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]);
  for(i = 0; i < p_api_dev->glb_mem.sim_buf_size[ p_api_dev->biu ] / 4; i++)
    *(ptr + i) = 0L;
} /* end: api_io_sim_buf_ini */

/*****************************************************************************/
/***                     Calibration Functions                             ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CAL_INI            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 16.02.01                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the Calibration mode of the selected BIU.*/
/*                                                                           */
/*****************************************************************************/

void api_io_cal_ini(TY_API_DEV *p_api_dev)
{
#ifdef _AMC1553
  (*p_api_dev->cb_p[ p_api_dev->biu ]).car = BSWAP32(0x04FA04FAUL); /* Default Amplitude,WRAPPED (no relay) */
#else 
  if( ( (MEM(GLBRAM_BP + BIU_VERSION_O) &0x00ff0000) >> 16) == 0x1F)
  {
     /* If DBTE PBI (0x1F)*/
    (*p_api_dev->cb_p[ p_api_dev->biu ]).car = BSWAP32(0x04FA04FAUL); /* Default Amplitude,WRAPPED (no relay) */
  }
  else
  {
    (*p_api_dev->cb_p[ p_api_dev->biu ]).car = BSWAP32(0x00FA00FAUL); /* Default Amplitude,Isolated */
  }
#endif 
  api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

} /* end: api_io_cal_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CAL_SIG_CON        */
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
/*    Inputs    : MILbus channel selection [bus]                             */
/*                Calibration mode control [con]                             */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the Calibration mode of the specified */
/*    API1553-DS MILbus channel.                                             */
/*                                                                           */
/*****************************************************************************/

void api_io_cal_sig_con(TY_API_DEV *p_api_dev, BYTE bus, BYTE con)
{
  switch(bus)
  {
    case API_CAL_PRI_BUS:
      if(con == API_CAL_SIG_ENA)
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(0x00001000L);
      else
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFFFFEFFFL);
      break;
    case API_CAL_SEC_BUS:
      if(con == API_CAL_SIG_ENA)
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(0x10000000L);
      else
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xEFFFFFFFL);
      break;
    default:
      break;
  }

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->car),
                      Out);

  /* Enable mode */
  api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

} /* end: api_io_cal_sig_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CAL_CPL_CON        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98                 Update : 08.08.07                     */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : MILbus channel selection [bus]                             */
/*                Bus Coupling mode control [cpl]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the BusCoupling mode of the specified */
/*    API1553-DS MILbus channel.                                             */
/*                                                                           */
/*****************************************************************************/

void api_io_cal_cpl_con(TY_API_DEV *p_api_dev, BYTE bus, BYTE cpl)
{
    L_WORD tmpl        = 0;

    tmpl = cpl;
    tmpl &= 0x0000000FL;


    /* -- configure new coupling in the firmware control block --- */

    switch(bus)
    {
    case API_CAL_PRI_BUS:
        tmpl <<= 8;
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFFFFF0FFL);
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(tmpl);
        break;
    case API_CAL_SEC_BUS:
        tmpl <<= 24;
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xF0FFFFFFL);
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(tmpl);
        break;
    default:
        break;
    }

    UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->car),
        Out);

    /* -- execute the firmware command to switch the coupling --- */
    api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);
} /* end: api_io_cal_cpl_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CAL_XMT_CON        */
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
/*    Inputs    : MILbus channel selection [bus]                             */
/*                Transmitter output amplitude control [amp]                 */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the Transmitter Output amplitude of   */
/*    the specified API1553-DS MILbus channel.                               */
/*                                                                           */
/*****************************************************************************/

void api_io_cal_xmt_con(TY_API_DEV *p_api_dev, BYTE bus, BYTE amp)
{
  L_WORD tmpl;

  tmpl = 0L;
  tmpl = amp;

  switch(bus)
  {
    case API_CAL_PRI_BUS:
      (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFFFFDF00L);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(tmpl);
      break;
    case API_CAL_SEC_BUS:
      tmpl <<= 16;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFF00DFFFL);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(tmpl);
      break;
    case API_CAL_PACKED:
        (*p_api_dev->cb_p[p_api_dev->biu]).car &= BSWAP32(0xFF00DF00L);
        (*p_api_dev->cb_p[p_api_dev->biu]).car |= BSWAP32(tmpl);  /* Set primary amp. */
        tmpl <<= 16;
        (*p_api_dev->cb_p[p_api_dev->biu]).car |= BSWAP32(tmpl);  /* Set secondary amp. */
        break;
    case API_CAL_HIGHRES:
      (*p_api_dev->cb_p[p_api_dev->biu]).car &= BSWAP32(0xFF00DF00L);
      (*p_api_dev->cb_p[p_api_dev->biu]).car |= 1<<13;          /* High Resolution Range (HRR) Bit*/
      (*p_api_dev->cb_p[p_api_dev->biu]).car |= BSWAP32(tmpl);  /* Set primary amp. */
      tmpl <<= 16;
      (*p_api_dev->cb_p[p_api_dev->biu]).car |= BSWAP32(tmpl);  /* Set secondary amp. */
      break;
    default:
      break;
  }

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->car),
                      Out);

  /* Enable mode */
  api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

} /* end: api_io_cal_xmt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_CAL_TRANS_CON      */
/*                                                                           */
/*   Author : Thomas Jahn               Project   : API1553-DS               */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 11.12.00   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Data Transmission Mode [con]    1:500Kbit / 0:1Mbit        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the Calibration mode of the specified */
/*    API1553-DS MILbus channel.                                             */
/*                                                                           */
/*****************************************************************************/

void api_io_cal_trans_con(TY_API_DEV *p_api_dev, BYTE con)
{
  /* This function has been removed with BSP 12.0.0 */
  /* When it was re-introduced with BSP 12.6.0 the former bit 13 was occupied
   * by a differen function. This is why the bit was moved from 13 to 15.
   * Bit 15 on the other hand was ITC on AyI before (No longer supported).
   **/
  if(con == API_CAL_TRANS_500K)
    (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(0x00008000L);
  else
    (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFFFF7FFFL);

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->car),
                      Out);

  /* Enable mode */
  api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);

} /* end: api_io_cal_trans_con */



#ifdef CMPL4SIM
/*****************************************************************************/
/***                 Remote Terminal Functions                             ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_IR_CLR          */
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
/*    Inputs    : Remote Terminal Address [rt_addr]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to clear the selected RT Interrupt Flags.    */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_ir_clr(TY_API_DEV *p_api_dev, BYTE rt_addr)
{
  p_api_dev->rt_ir_status[ p_api_dev->biu][ rt_addr ] = API_OFF;

} /* end: api_io_rt_ir_clr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_IR_GET          */
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
/*    Inputs    : Remote Terminal Address [rt_addr]                          */
/*                                                                           */
/*    Outputs   : ISR flag                                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to return the selected RT Interrupt Flag.    */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_rt_ir_get(TY_API_DEV *p_api_dev, BYTE rt_addr)
{
  return (p_api_dev->rt_ir_status[ p_api_dev->biu][ rt_addr ]);

} /* end: api_io_rt_ir_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_GLB_CNT_INI     */
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
/*    This function is used to initialize the global message/error count     */
/*    location of the specified API1553-DS BIU.                              */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_glb_cnt_ini(TY_API_DEV *p_api_dev)
{
  (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec = 0L;

} /* end: api_io_rt_glb_cnt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_MSG_CNT_INI     */
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
/*    Inputs    : API1553-DS RT address [rt_addr]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the message count location         */
/*    of the specified API1553-DS RT.                                        */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_msg_cnt_ini(TY_API_DEV *p_api_dev, BYTE rt_addr)
{
  /* RT Descriptor */
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].msg_cnt = 0L;
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].err_cnt = 0L;

} /* end: api_io_rt_msg_cnt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_DESC_INI        */
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
/*    Inputs    : API1553-DS RT Address [rt_addr]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the RT and subaddress/modecode     */
/*    descriptor of the specified API1553-DS RT.                             */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_desc_ini(TY_API_DEV *p_api_dev, BYTE rt_addr)
{
  L_WORD tmpl, addr, tmp_addr;
  WORD i, rt_hid;

  /**************************************/
  /***   RT Descriptors               ***/
  /**************************************/

  /* RT Descriptor Control Word */
  tmpl = 0L;
  tmpl = API_RT_RESP_TIME_DEF;
  tmpl <<= 16;
  tmpl += ( (rt_addr &0x1F) << 11);

  /* RT SA Descriptor Base Pointer */
  addr = API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sa_desc);
  addr += (rt_addr *MAX_API_RT_SA_AREA_SIZE);

  /* RT Descriptor */
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl = BSWAP32(tmpl);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_desc = BSWAP32(addr);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw = BSWAP32(0x0000FFFFUL);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].msg_cnt = 0L;
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].err_cnt = 0L;
  for(i = 0; i < 11; i++)
    (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].res[ i ] = 0L;

  /* used for RT Enable/Disable on the fly */
  p_api_dev->rt_setup[ p_api_dev->biu ].rt_con[ rt_addr ] = API_RT_DISABLE;


  /**************************************/
  /***   RT SA Descriptors            ***/
  /**************************************/

  /* RT SA Descriptor Control Word */
  tmpl = 0L;
  tmpl += API_RT_SA_RESP_MODE_DEF;
  tmpl <<= 28;

  /* RT Buffer Header Pointer */
  addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area;

  /* RT SA Descriptor */
  for(i = 0; i < MAX_API_RT_SA; i++)
  {
    tmp_addr = (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ i ].bhd_ptr & BSWAP32(0x03fffffful);
    tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, tmp_addr);
    rt_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);
    api_io_rt_fw_dytag_dis(p_api_dev, rt_hid);

    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ i ].sa_ctrl = BSWAP32(tmpl);
    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ i ].bhd_ptr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, addr);
  }

  /* RT MC Descriptor */
  for(i = 0; i < MAX_API_RT_MC; i++)
  {
    tmp_addr = (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ MAX_API_RT_SA + i ].bhd_ptr & BSWAP32(0x03fffffful);
    tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, tmp_addr);
    rt_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);
    api_io_rt_fw_dytag_dis(p_api_dev, rt_hid);

    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ MAX_API_RT_SA + i ].sa_ctrl = BSWAP32(tmpl);
    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ MAX_API_RT_SA + i ].bhd_ptr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, addr);
  }

  /**************************************/
  /***   RT Dynamic Data              ***/
  /**************************************/

  /* RT DYTAG SA Descriptor Allocation Area */
  for(i = 0; i < MAX_API_RT_XMT_SA; i++)
    p_api_dev->rt_setup[ p_api_dev->biu ].rt_dytag[ rt_addr ].sa[ i ] = 0;

  /**************************************/
  /***   RT System Dynamic Data       ***/
  /**************************************/
  
  api_io_rt_systag_free(p_api_dev, 1/*RT*/, rt_addr, 0/*na*/);

} /* end: api_io_rt_desc_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_BH_INI          */
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
/*    Inputs    : API1553-DS RT Buffer Header Index [index]                  */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected RT Buffer Header      */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_bh_ini(TY_API_DEV *p_api_dev, WORD index)
{
  L_WORD tmpl, sq_addr, eq_addr, bp_addr;
  L_WORD tmp1, tmp2, tmp3, tmp4, bs;

  tmpl = 0L;

  /* Data Buffer Queue Mode */
  tmp1 = 0L;
  tmp1 = API_RT_BH_BQM_CYCLIC &0x03;
  tmp1 <<= 28;

  /* Data Buffer Store Mode */
  tmp2 = 0L;
  tmp2 = API_RT_BH_BSM_DISCARD &0x03;
  tmp2 <<= 26;

  /* Buffer Size */
  bs = 0L;
  bs = (MAX_API_BUF_SIZE / 4) &0x1F;
  bs <<= 18;

  /* Status Queue Size */
  tmp3 = 0L;
  tmp3 = API_RT_BH_STS_ONE_SQ &0x03;
  tmp3 <<= 12;

  /* Event Queue Size */
  tmp4 = 0L;
  tmp4 = API_RT_BH_EVS_NO_EQ &0x03;
  tmp4 <<= 10;


  /* RT Buffer Header Control Word */
  tmpl = tmp1 | tmp2 | tmp3 | tmp4 | bs;

  /* RT Status Queue Base Pointer */
  sq_addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sq_area));
  sq_addr &= 0x03FFFFFCL; /* even address value, divisible by 4 */

  /* RT Event Queue Base Pointer */
  eq_addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_eq_area));
  eq_addr &= 0x03FFFFFCL; /* even address value, divisible by 4 */

  /* RT Buffer Queue Base Pointer */
  bp_addr = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]));
  bp_addr &= 0x03FFFFFCL; /* even address value, divisible by 4 */

  /**************************************/
  /***   RT Buffer Header             ***/
  /**************************************/
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].bh_ctrl = BSWAP32(tmpl);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].sq_addr = BSWAP32(sq_addr);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].eq_addr = BSWAP32(eq_addr);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr = BSWAP32(bp_addr);

} /* end: api_io_rt_bh_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_SQ_INI          */
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
/*    Inputs    : API1553-DS RT Status Queue Index [index]                   */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected RT Status Queue       */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sq_ini(TY_API_DEV *p_api_dev, WORD index)
{
  /**************************************/
  /***   RT Status Queue Entry        ***/
  /**************************************/
  (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ index ].sq_ctrl = 0L;
  (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ index ].lcw_lsw = BSWAP32(0x0000FFFFUL);
  (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ index ].act_buf_ptr = 0L;
  (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ index ].time_tag = 0L;

} /* end: api_io_rt_sq_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_EQ_INI          */
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
/*    Inputs    : API1553-DS RT Event Queue Index [index]                    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected RT Event Queue        */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_eq_ini(TY_API_DEV *p_api_dev, WORD index)
{
  /**************************************/
  /***   RT Event Queue Entry         ***/
  /**************************************/
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ index ].con1 = 0L;
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ index ].addr1 = 0L;
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ index ].con2 = 0L;
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ index ].addr2 = 0L;

  p_api_dev->rt_dytag_status[ p_api_dev->biu ][ index ] = API_OFF; /* disable all bits for all 32 RTs */
} /* end: api_io_rt_eq_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_INI             */
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
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS RTs.                */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_ini(TY_API_DEV *p_api_dev)
{
  WORD i;
  BYTE rt;
  /* RT specific registers of System Control Block */
  api_io_cb_ini(p_api_dev, INIT_RT);

  /* Global RT message/error counter */
  api_io_rt_glb_cnt_ini(p_api_dev);

  /* RT Descriptors and RT SA/MC Descriptors */
  for(rt = 0; rt < MAX_API_RT; rt++)
  {
    api_io_rt_desc_ini(p_api_dev, rt);
  }

  /* RT Buffer Header */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area; i++)
  {
    api_io_rt_bh_ini(p_api_dev, i);
  }

  /* RT Status Queue */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_sq_area; i++)
  {
    api_io_rt_sq_ini(p_api_dev, i);
  }

  /* RT Event Queue */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_eq_area; i++)
  {
    api_io_rt_eq_ini(p_api_dev, i);
  }

  /* 25.05.00 : System Dynamic Data */
  api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_RT_ALL, 0, 0);

  UsbRTBufferListClear(p_api_dev, p_api_dev->biu);

} /* end: api_io_rt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_CON             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 24.07.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Operation Control [con],                                */
/*                RT Bus Response Control [bus],                             */
/*                RT Response Time in 0.25 usec steps [resp_time],           */
/*                Next RT Status Word [nxw]                                  */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to enable or disable the specified RT.           */
/*                                                                           */
/*****************************************************************************/
void api_io_rt_con(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE bus, BYTE resp_time, WORD nxw)
{
  L_WORD tmpl;

  api_io_rt_desc_ini(p_api_dev, rt_addr);

  switch(con)
  {
    case API_RT_DISABLE:
      /* System Dynamic Data */
      api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_RT_ONLY, rt_addr, 0);
      break;

    case API_RT_ENABLE:
        tmpl = 0L;
        tmpl = resp_time;
        tmpl <<= 16;
        tmpl += nxw;

        /* Bus Response Control */
        switch(bus &0x03)
        {
        case API_RT_RESP_PRI:
            tmpl |= 0x02000000L;
            break;
        case API_RT_RESP_SEC:
            tmpl |= 0x04000000L;
            break;
        case API_RT_RESP_BOTH:
        default:
            break;
        }

        /* Enable RT for Simulation */
        (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl = BSWAP32(tmpl | 0x08000000L);
        p_api_dev->rt_setup[ p_api_dev->biu ].rt_con[ rt_addr ] = con;
        break;

    case API_RT_MAILBOX:
      tmpl = 0L;
      tmpl += nxw;

      /* Enable RT for Mailboxing */
      (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl = BSWAP32(tmpl | 0x10000000L);
      p_api_dev->rt_setup[ p_api_dev->biu ].rt_con[ rt_addr ] = con;
      break;
    default:
      break;
  }

} /* end: api_io_rt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_ENA_DIS         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 24.07.02   Update : 24.07.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt_addr]                                       */
/*                RT Enable / Disable [con]                                  */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function can be used to enable or disable the specified RT on the */
/*    the fly.                                                               */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_ena_dis_set_operation(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE new_mode)
{
    p_api_dev->rt_setup[ p_api_dev->biu ].rt_con[ rt_addr ] = new_mode;
    api_io_rt_ena_dis(p_api_dev, rt_addr, con);
}

void api_io_rt_ena_dis(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con)
{
  BYTE mode;

  if(con == API_ON)
  {
    /* Enable RT (Simulation or Mailbox) */
    mode = p_api_dev->rt_setup[ p_api_dev->biu ].rt_con[ rt_addr ];
    switch(mode)
    {
      case API_RT_ENABLE:
          /* Enable Simulation RT */
        (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl |= BSWAP32(0x08000000L);
        break;
      case API_RT_MAILBOX:
        /* Enable Mailboxed RT */
        (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl |= BSWAP32(0x10000000L);
        break;
      default:
        break;
    }
  }
  else
  {
    /* Disable RT */
    (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl &= BSWAP32(0xC7FFFFFFL);
  }

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[p_api_dev->biu])[rt_addr].rt_ctrl), Out);
  }

} /* end: api_io_rt_ena_dis */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_CON_RESP        */
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
/*    Inputs    : API1553-DS RT address [rt_addr],                           */
/*                API1553-DS RT response time [resp_time]                    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the response time of the specified     */
/*    API1553-DS RT.                                                         */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_con_resp(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE resp_time)
{
  L_WORD tmpl, lval;

  lval = 0L;
  lval = resp_time;

  /* RT Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl);
  LWORD_RESET_BITS(tmpl, 16, 8);
  LWORD_SET_BITS(tmpl, lval, 16, 8);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl = BSWAP32(tmpl);

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl), 
          Out);
  }

} /* end: api_io_rt_con_resp */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_CON_NXW         */
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
/*    Inputs    : API1553-DS RT address [rt_addr],                           */
/*                API1553-DS RT next status word [nsw]                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the next status word of the specified  */
/*    API1553-DS RT.                                                         */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_con_nxw(TY_API_DEV *p_api_dev, BYTE rt_addr, WORD nxw)
{
  L_WORD tmpl, lval;

  lval = 0L;
  lval = nxw;

  /* RT Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl);
  LWORD_RESET_BITS(tmpl, 0, 16);
  LWORD_SET_BITS(tmpl, lval, 0, 16);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl = BSWAP32(tmpl);

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl), 
          Out);
  }

} /* end: api_io_rt_con_nxw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_CON_LCW         */
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
/*    Inputs    : API1553-DS RT address [rt_addr],                           */
/*                API1553-DS RT last command word [lcw]                      */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the last command word of the specified */
/*    API1553-DS RT.                                                         */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_con_lcw(TY_API_DEV *p_api_dev, BYTE rt_addr, WORD lcw)
{
  L_WORD tmpl, lval;

  lval = 0L;
  lval = lcw;

  /* RT Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw);
  LWORD_RESET_BITS(tmpl, 16, 16);
  LWORD_SET_BITS(tmpl, lval, 16, 16);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw = BSWAP32(tmpl);

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[p_api_dev->biu])[rt_addr].lcw_lsw), Out);
  }

} /* end: api_io_rt_con_lcw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_CON_LSW         */
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
/*    Inputs    : API1553-DS RT address [rt_addr],                           */
/*                API1553-DS RT last status word [lsw]                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the last status word of the specified  */
/*    API1553-DS RT.                                                         */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_con_lsw(TY_API_DEV *p_api_dev, BYTE rt_addr, WORD lsw)
{
  L_WORD tmpl, lval;

  lval = 0L;
  lval = lsw;

  /* RT Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw);
  LWORD_RESET_BITS(tmpl, 0, 16);
  LWORD_SET_BITS(tmpl, lval, 0, 16);
  (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw = BSWAP32(tmpl);

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[p_api_dev->biu])[rt_addr].lcw_lsw), Out);
  }

} /* end: api_io_rt_con_lsw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_BH_EQ_CON       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Enable/Disable Event Queue Processing [con]                */
/*                RT Buffer Header ID [rt_hid]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control RT Event Queue processing.            */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_bh_eq_con(TY_API_DEV *p_api_dev, WORD con, L_WORD rt_hid)
{
  L_WORD tmpl;

  if(con == API_ON)
  {
    tmpl = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bh_ctrl);
    LWORD_RESET_BITS(tmpl, 10, 2);
    LWORD_SET_BITS(tmpl, API_RT_BH_EVS_ONE_EQ, 10, 2);
    (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bh_ctrl = BSWAP32(tmpl);
  }
  else
  {
    tmpl = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bh_ctrl);
    LWORD_RESET_BITS(tmpl, 10, 2);
    (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bh_ctrl = BSWAP32(tmpl);
  }

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[rt_hid].bh_ctrl), Out);
  }

} /* end: api_io_rt_bh_eq_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_FW_DYTAG_DIS    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Buffer Header ID [rt_hid]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_fw_dytag_dis(TY_API_DEV *p_api_dev, WORD rt_hid)
{

  if(rt_hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_eq_area)
  {
    if(p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena == API_ON)
    {
      p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena = API_OFF;
      api_io_rt_bh_eq_con(p_api_dev, API_OFF, rt_hid);
      api_io_rt_eq_ini(p_api_dev, rt_hid);
    }
  }

} /* end: api_io_rt_fw_dytag_dis */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_RT_SYSTAG_FREE       */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_systag_free(TY_API_DEV *p_api_dev, short mode, BYTE rt, BYTE sa)
{
short i,j;

   if (mode == 0) {
     /* All */
     for(i=0; i<MAX_API_RT; i++) {
       for(j=0; j<MAX_API_RT_XMT_SA; j++) {
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[i].sa[j].id = 0;
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[i].sa[j].cnt = 0;
          if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[i].sa[j].p != NULL) {
             FREE(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[i].sa[j].p);
             p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[i].sa[j].p = NULL;
          }
       }
     }
   }
   else if (mode == 1) {
     /* On RT Level */
     if (rt < MAX_API_RT) {
       for(j=0; j<MAX_API_RT_XMT_SA; j++) {
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[j].id = 0;
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[j].cnt = 0;
          if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[j].p != NULL) {
             FREE(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[j].p);
             p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[j].p = NULL;
          }
       }
     }
   }
   else if (mode == 2) {
     /* On RT and SA Level */
     if ((rt < MAX_API_RT) && (sa < MAX_API_RT_XMT_SA)) {
       p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].id = 0;
       p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt = 0;
       if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p != NULL) {
          FREE(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p);
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p = NULL;
       }
     }  
   }
   
} /* end: api_io_rt_systag_free */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_RT_SYSTAG_ADD        */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_systag_add(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE tag_ix)
{
short j,found,cnt;

   if ((rt < MAX_API_RT) && (sa < MAX_API_RT_XMT_SA)) {

      if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p == NULL) {
#ifdef DEBUG
         PRINTF3("RT MALLOCed\n",0,0,0);
#endif         
         /* Allocate Memory and Clear */
         p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p = (BYTE *) MALLOC(MAX_API_SYS_LS_SYSTAGS);
         memset(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p, 0, MAX_API_SYS_LS_SYSTAGS);
      }    

      if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p != NULL) {

         /* Search if Systag Index is already existing in the list */
         cnt = p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt;
         for (j=0,found=0; j<cnt; j++) {
            if ( p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p[j] == (tag_ix+1) ) {
               found = 1;
               break;
            } 
         }

         if (found == 0) {
         
            /* Add the Systag Index and increment count */
            if (cnt < MAX_API_SYS_LS_SYSTAGS) {
               p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p[cnt] = tag_ix + 1;
               p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt++;
               p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].id = tag_ix + 1;
            }
         }
        
#ifdef DEBUG
         PRINTF3("Systag List id=%d cnt=%d:",
            p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].id,
            p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt,0);
         for (j=0; j<p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt; j++) {
            PRINTF3(" %d",p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p[j],0,0);
         }
         PRINTF3("\n",0,0,0);         
#endif
     }
   }
   
} /* end: api_io_rt_systag_add */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_RT_SYSTAG_SUB        */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_systag_sub(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE tag_ix)
{
short j,k,found,cnt;
BYTE tmp_arr[MAX_API_SYS_LS_SYSTAGS];

   if ((rt < MAX_API_RT) && (sa < MAX_API_RT_XMT_SA)) {

      if (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p != NULL) {

         memset(tmp_arr, 0, MAX_API_SYS_LS_SYSTAGS);

         /* Search if Systag Index is already existing in the list */
         cnt = p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt;
         for (j=0,k=0,found=0; j<cnt; j++) {
            if ( p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p[j] == (tag_ix+1) ) {
               found = 1;
            } 
            else {
               /* Copy the list (without specified index) */
               tmp_arr[k] = p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p[j]; k++;
            }
         }

         if (found == 1) {
            /* Sub the Systag Index and decrement count */
            if (cnt > 0) {
               p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].cnt--;
               memcpy(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[rt].sa[sa].p, tmp_arr, MAX_API_SYS_LS_SYSTAGS);
            }
         }
      }
   }
   
} /* end: api_io_rt_systag_sub */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_RT_SA_SYSTAG_INIT_DW */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt]                                            */
/*                RT Subaddress [sa]                                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_systag_init_dw(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa)
{
WORD rtsa, rt_hid, index;
BYTE rt_dytag_i;
L_WORD tmp_addr, dbp;

  rtsa = rt;
  rtsa = rtsa << 8;
  rtsa |= sa;

  /* Calculate Data Buffer Pointer */
  index = sa + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_XMT);
  tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
  rt_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);
  dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bp_addr & BSWAP32(0x03fffffful) );
  rt_dytag_i = p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt ].sa[ sa ].id;
  api_io_ls_systag_init_dw(p_api_dev, rt_dytag_i, API_RT_MODE, rtsa, dbp);

} /* end: api_io_rt_sa_systag_init_dw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO            Submodule : API_IO_RT_SA_SYSTAG_INIT_DATASET */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt]                                            */
/*                RT Subaddress [sa]                                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_systag_init_dataset(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa)
{
WORD rt_hid, index;
BYTE rt_dytag_i;
L_WORD tmp_addr, dbp;

  /* Calculate Data Buffer Pointer */
  index = sa + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_XMT);
  tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
  rt_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);
  dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ rt_hid ].bp_addr & BSWAP32(0x03fffffful) );
  rt_dytag_i = p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt ].sa[ sa ].id;
  api_io_ls_systag_init_dataset(p_api_dev, rt_dytag_i, dbp);

} /*  end: api_io_rt_sa_systag_init_dataset */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_SA_SYSTAG_CON   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_systag_con(TY_API_DEV *p_api_dev, BYTE tag_ix, BYTE rt_addr, BYTE sa)
{
WORD index;

  if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mode == API_SYSTAG_ENA)
  {
    /* Enable System Dynamic Data */
    if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].fct != API_SYSTAG_DATASET)
    {
      api_io_rt_systag_add(p_api_dev, rt_addr, sa, tag_ix);

      if(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa ].cnt == 1)
      {
        /* Enable RT/SA/XMT Access Interrupt, RT-SA shall be already enabled */
        index = sa + (MAX_API_RT_MC_SA * API_RT_SA_TYPE_XMT);
        if( (BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl) &0xC0000000L) != 0L) {
          (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl &= BSWAP32(0x3FFFFFFFUL);
          (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl |= BSWAP32(0x80000000UL);
        } 
      }

      /* Init Dynamic Data Word */
      api_io_rt_sa_systag_init_dw(p_api_dev, rt_addr, sa);
    }
    else
    {
      /* Dataset */
      p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa ].id = tag_ix + 1;

      /* Enable RT/SA/XMT Access Interrupt, RT-SA shall be already enabled */
      index = sa + (MAX_API_RT_MC_SA * API_RT_SA_TYPE_XMT);
      if( (BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl) &0xC0000000UL) != 0L) {
        (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl &= BSWAP32(0x3FFFFFFFUL);
        (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl |= BSWAP32(0x80000000UL);
      }

      /* Init Dynamic Data Word */
      api_io_rt_sa_systag_init_dataset(p_api_dev, rt_addr, sa);
    }
  }
  else
  {
    /* Disable System Dynamic Data */
    if(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa ].cnt != 0) {
       api_io_rt_systag_sub(p_api_dev, rt_addr, sa, tag_ix);
    }
    
    if(p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa ].cnt == 0) {
       p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa ].id = 0;

      /* Disable RT/SA/XMT Access Interrupt */

      /* future release shall verify which other function of the target    */
      /* software makes use of the same Interrupt: at present SA Access    */
      /* Interrupt can only be enabled by the user (via RT-SA-Con command) */
      /* and Systag (automatically enabled). When trying to disable the    */
      /* Interrupt the software must evaluate if another function makes    */
      /* use of the same interrupt type. */
      if(p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_tx[ sa ] == API_OFF)
      {
        index = sa + (MAX_API_RT_MC_SA * API_RT_SA_TYPE_XMT);
        if ((BSWAP32((*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl) &0x80000000UL) != 0L)
        {
            (*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl &= BSWAP32(0x3FFFFFFFL);
            (*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl |= BSWAP32(0x40000000UL);
        }
      }
    }
  }

} /* end: api_io_rt_sa_systag_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_BHD_DEF         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 04.06.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Buffer Header ID [hid],                                 */
/*                RT Data Buffer ID [bid],                                   */
/*                RT Status Queue ID [sid],                                  */
/*                RT Event Queue ID [eid],                                   */
/*                Queue Size definition [qsize],                             */
/*                Buffer Queue Mode [bqm],                                   */
/*                Buffer Store Mode [bsm],                                   */
/*                Status Queue Mode [sts],                                   */
/*                Event Queue Mode [evs],                                    */
/*                Reserved [res]                                             */
/*                                                                           */
/*    Outputs   : Amount of contiguous Data Buffers [nbufs]                  */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the buffer header structure of the     */
/*    specified API1553-DS RT.                                               */
/*                                                                           */
/*****************************************************************************/

WORD api_io_rt_bhd_def(TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, 
                                              BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res)
{
L_WORD tmpl, tmp1, tmp2, tmp3, tmp4, bs, qs;
WORD nbufs;

  api_io_rt_fw_dytag_dis(p_api_dev, hid);
  api_io_rt_bh_ini(p_api_dev, hid);

  tmpl = res; /* dummy */
  tmpl = 0L;

  /* Data Buffer Queue Mode */
  tmp1 = 0L;
  tmp1 = bqm & 0x03;
  tmp1 <<= 28;

  /* Data Buffer Store Mode */
  tmp2 = 0L;
  tmp2 = bsm & 0x03;
  tmp2 <<= 26;

  /* Buffer Size */
  bs = 0L;
  bs = (MAX_API_BUF_SIZE / 4) & 0x1F;
  bs <<= 18;

  /* Queue Size */
  qs = 0L;
  qs = qsize & 0x0F;
  nbufs = 1L << qs;
  qs <<= 14;

  /* Status Queue Size */
  tmp3 = 0L;
  tmp3 = sts & 0x03;
  tmp3 <<= 12;

  /* Event Queue Size */
  tmp4 = 0L;
  tmp4 = evs & 0x03;
  tmp4 <<= 10;

  tmpl = tmp1 | tmp2 | tmp3 | tmp4 | bs | qs;

  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl  = BSWAP32(tmpl);

  tmpl = BSWAP32((*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr = BSWAP32( tmpl + (MAX_API_RT_SQ_DESC_SIZE * sid) );

  tmpl = BSWAP32((*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr = BSWAP32( tmpl + (MAX_API_RT_EQ_DESC_SIZE * eid) );

  tmpl = BSWAP32((*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
  (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr = BSWAP32( tmpl + (MAX_API_BUF_SIZE * bid) );


  UsbRTBufferListAdd(p_api_dev, p_api_dev->biu, hid, 
                     API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[hid]));

  return (nbufs);

} /* end: api_io_rt_bhd_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_SA_CON          */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Subaddress/Modecode [sa_mc],                            */
/*                RT Buffer Header ID [hid],                                 */
/*                RT Subaddress Type [sa_type],                              */
/*                RT Subaddress/Modecode Control (Enable/Disable) [con],     */
/*                RT Response Mode [rmod],                                   */
/*                RT Status Word Mask Control [smod],                        */
/*                RT Status Word Modification Mask [swm]                     */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the specified RT subaddress/modecode  */
/*    of the specified API1553-DS RT.                                        */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_con(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm)
{
  L_WORD tmpl, addr, tmp1, tmp2, tmp3, tmp4, tmp_addr;
  WORD i, index, rt_hid;

  index = sa_mc + (MAX_API_RT_MC_SA *sa_type);

  if(con != API_RT_SA_DISABLE)
  {

    /* RT SA Descriptor Control Word */
    tmpl = 0L;

    tmp1 = 0L;
    tmp1 = con &0x03; /* Subaddress Control */
    tmp1 <<= 30;

    tmp2 = 0L;
    tmp2 = rmod &0x03; /* Response Mode */
    tmp2 <<= 28;

    tmp3 = 0L;
    tmp3 = smod &0x03; /* Status Word Mask Control */
    tmp3 <<= 26;

    tmp4 = 0L;
    tmp4 = swm &0x07FF; /* Status Word Mask */
    tmp4 <<= 15;

    tmpl = tmp1 | tmp2 | tmp3 | tmp4;

    /* RT Buffer Header Pointer */
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area + (hid *MAX_API_RT_BH_DESC_SIZE);
  }
  else
  {

    /* RT SA Descriptor Control Word */
    tmpl = 0L;
    tmpl += API_RT_SA_RESP_MODE_DEF;
    tmpl <<= 28;

    /* RT Buffer Header Pointer */
    addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area;

    /* Find and disable RT XMT SA queue tagging */
    if(p_api_dev->rt_setup[ p_api_dev->biu ].rt_queue[ rt_addr ].sa[ sa_mc ] != 0)
    {
      i = p_api_dev->rt_setup[ p_api_dev->biu ].rt_queue[ rt_addr ].sa[ sa_mc ] - 1;
      /* Xfer slot */
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].mode    = API_OFF;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].xid     = 0;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].rt      = 0xFF;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].sa      = 0xFF;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].flag    = API_OFF;
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].buf_cnt = 0;

      p_api_dev->rt_setup[ p_api_dev->biu ].rt_queue[ rt_addr ].sa[ sa_mc ] = 0;
    }

    tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
    rt_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);
    api_io_rt_fw_dytag_dis(p_api_dev, rt_hid);

    if( (sa_type == API_RT_SA_TYPE_XMT) && (p_api_dev->rt_setup[ p_api_dev->biu ].rt_systag[ rt_addr ].sa[ sa_mc ].id != 0) )
    {
      api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_SA_ONLY, rt_addr, sa_mc);
      api_io_rt_systag_free(p_api_dev, 2/*SA*/, rt_addr, sa_mc);    
    }
  }

  (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl = BSWAP32(tmpl);
  tmpl = BSWAP32((*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr);
  LWORD_RESET_BITS(tmpl, 0, 26);
  LWORD_SET_BITS(tmpl, BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, addr)), 0, 26);
  (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr = BSWAP32(tmpl);



  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
    UsbSynchronizeRTSubAddressDescriptorArea( p_api_dev, p_api_dev->biu, Out ); 
  }

} /* end: api_io_rt_sa_con */




/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_SA_ERR_CON      */
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
/*    Inputs    : RT address [rt_addr],                                      */
/*                RT subaddress / modecode [sa_mc],                          */
/*                RT subaddress type [sa_type],                              */
/*                RT subaddress error injection type [type],                 */
/*                RT error word position [wpos],                             */
/*                RT error bit position [bpos],                              */
/*                Sync field error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits]                   */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the specified RT subaddress/modecode  */
/*    error injection of the specified API1553-DS RT.                        */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_err_con(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, BYTE sa_type, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits)
{
  WORD err_type, err_ext, err_wno, index;
  L_WORD tmpl, err;

  tmpl = 0L;
  err = 0L;

  err_type = 0;
  err_ext = 0;
  err_wno = 0;

  index = sa_mc + (MAX_API_RT_MC_SA *sa_type);

  switch(type)
  {
    case API_ERR_NONE:
      break;
    case API_ERR_CSYNC:
      err_type = 7;
      err_ext = sync &0x3F;
      err_ext <<= 9;
      wpos = 0;
      break;
    case API_ERR_DSYNC:
      err_type = 7;
      err_ext = sync &0x3F;
      err_ext <<= 9;
      break;
    case API_ERR_PARITY:
      err_type = 1;
      break;
    case API_ERR_MANHI:
      if( (bpos >= 4) && (bpos < 20) )
      {
        err_type = 5;          /* manchester error */

        err_ext = 23-bpos - 4; /* bpos 4..19  FW swaps the bits (e.g. Bit Pos 4 / Physical Manchester Err at bit 19) */
        err_ext <<= 9;         /* data bit of the error */
        err_ext |= 1 << 14;    /* manchester stuck at high fault */
      }
      if( bpos == 20 ) /* special case for parity bit */
      {
        err_type = 5;          /* manchester error */
        err_ext  = 1 << 13;    /* error is in parity bit */
        err_ext |= 1 << 14;    /* manchester stuck at high fault */
      }
      break;
    case API_ERR_MANLO:
      if( (bpos >= 4) && (bpos < 20) )
      {
        err_type = 5;          /* manchester error */

        err_ext = 23-bpos - 4; /* bpos 4..19  FW swaps the bits (e.g. Bit Pos 4 / Physical Manchester Err at bit 19) */
        err_ext <<= 9;         /* data bit of the error */
      }
      if( bpos == 20 ) /* special case for parity bit */
      {
        err_type = 5;          /* manchester error */
        err_ext = 1 << 13;     /* error is in parity bit */
      }
      break;
    case API_ERR_CONTIG:
      err_type = 4;
      err_ext = contig &0x3F;
      err_ext <<= 9;
      break;
    case API_ERR_WCHI:
      err_type = 0;
      err_ext = 0x0200;
      wpos = 1;
      break;
    case API_ERR_WCLO:
      err_type = 0;
      err_ext = 0x0400;
      wpos = 1;
      break;
    case API_ERR_BCHI:
      err_type = 2;
      err_ext = bc_bits &0x03;
      err_ext <<= 10;
      err_ext |= 0x4000;
      break;
    case API_ERR_BCLO:
      err_type = 2;
      err_ext = bc_bits &0x03;
      err_ext <<= 10;
      break;
    case API_ERR_ALTBUS:
      err_type = 0;
      err_ext = 0x0600;
      wpos = 0;
      break;
    case API_ERR_ZXING_NEG:
    case API_ERR_ZXING_POS:
      if( (bpos >= 4) && (bpos <= 20) )
      {
        if(API_ERR_ZXING_NEG == type)
          err_type = 6;
        else
          err_type = 3;
        err_ext = bpos - 1;
        if( (contig % 2) != 0 )
          err_ext |= 0x20;
        err_ext <<= 9;

        if( contig >= 2 )
        {
          /* only AyX and AMC-NG boards are supported for high resolution zero crossing deviation */

          if( mil_hw_is_high_res_zero_crossing_supported( p_api_dev ) )
          {
            tmpl = BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr);
            LWORD_RESET_BITS(tmpl, 26, 4);
            LWORD_SET_BITS(tmpl, (contig-2), 26, 4);
            (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr = BSWAP32(tmpl);
          }
        }
      }
      break;
    case API_ERR_PHYSICAL_ERROR_SUPPRESSION:
        err_type = 0;
        err_ext  = 0x04<<9;
        wpos = 0;
        break;
    case API_ERR_WCLO_EXT:
        err_type = 0;
        err_ext  = 0x05<<9;
        break;
    case API_ERR_MSG_LENGTH_HI_IGNORE:
        err_type = 0;
        err_ext  = 0x06<<9;
        wpos = 0;
        break;
    case API_ERR_MSG_LENGTH_LO_IGNORE:
        err_type = 0;
        err_ext  = 0x07<<9;
        break;
    default:
      break;
  }

  err_type &= 0x0007;
  err_type <<= 6;
  if(wpos <= 32)
    err_wno = wpos;

  if( (type == API_ERR_ZXING_NEG) || (type == API_ERR_ZXING_POS) )
    err_wno++;
  err_ext &= 0xFE00;


  /* RT Subaddress Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl);
  err = err_ext | err_type | err_wno;
  LWORD_RESET_BITS(tmpl, 0, 15);
  LWORD_SET_BITS(tmpl, err, 0, 15);
  (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl = BSWAP32(tmpl);


  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
    UsbSynchronizeRTSubAddressDescriptorArea( p_api_dev, p_api_dev->biu, Out ); 
  }

} /* end: api_io_rt_sa_err_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_SA_FIFO_CON     */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Subaddress [sa],                                        */
/*                RT/SA Fifo Tagging Index [fifo_i]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the RT XMT Subaddress Fifo           */
/*    Tagging function.                                                      */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_sa_fifo_con(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE fifo_i)
{
  WORD index, id, j;
  L_WORD desc_addr, dbp, saddr, current_id;
  WORD volatile *dqueue_p;
  WORD volatile *dbp_p;

  p_api_dev->rt_setup[ p_api_dev->biu ].rt_queue[ rt_addr ].sa[ sa ] = fifo_i;

  if(fifo_i != 0)
  {

     /* 13.10.99 */ /* write first fifo buffer to transmitt buffer */
    index = sa + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_XMT);
    /* Get Buffer Address from Buffer Header */
    desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
    desc_addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area;
    index = (WORD) (desc_addr / MAX_API_RT_BH_DESC_SIZE);
    dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr);
    dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp);

    id = (fifo_i - 1);
    saddr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].saddr;
    current_id = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].cur_buf_addr;
    /* Set pointer to Dataset Memory address */
    dqueue_p = (WORD*)API_FIFO_MEM_ADDR_ABS(saddr);

    /* Copy Dataset to Buffer */
    for(j = 0; j < MAX_API_BUF_SIZE / 2; j++)
    {
      *(dbp_p + j) = *(dqueue_p + j);
    }

    current_id += MAX_API_BUF_SIZE;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].cur_buf_addr = current_id;

    /* Enable RT/SA/XMT Access Interrupt */
    index = sa + (MAX_API_RT_MC_SA *API_RT_SA_TYPE_XMT);
    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl &= BSWAP32(0x3FFFFFFFUL);
    (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl |= BSWAP32(0x80000000L);
  }
} /* end: api_io_rt_sa_fifo_con */

void api_io_rt_dwc_set(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE sa_type, BYTE dwc)
{
    L_WORD sa_ctrl = dwc & 0x3F;
    WORD index = 0;

    index = sa + (MAX_API_RT_MC_SA *sa_type);

    (*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl &= BSWAP32(0xFFFFFFC0UL);
    (*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl |= BSWAP32(sa_ctrl);
}

BYTE api_io_rt_dwc_get(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa, BYTE sa_type)
{
    BYTE dwc;
    WORD index;

    index = sa + (MAX_API_RT_MC_SA *sa_type);

    dwc = BSWAP32((*p_api_dev->rt_sa_desc_p[p_api_dev->biu])[rt_addr].sa_mc[index].sa_ctrl) & 0x0000003F;

    return(dwc);
}

#endif 


#ifdef CMPL4SIM
/*****************************************************************************/
/***                     Bus Controller Functions                          ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_IR_CLR          */
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
/*    This function is utilized to clear the BC Interrupt Flag.              */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_ir_clr(TY_API_DEV *p_api_dev)
{
  p_api_dev->bc_ir_status[ p_api_dev->biu ] = API_OFF;

} /* end: api_io_bc_ir_clr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_IR_GET          */
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
/*    Outputs   : ISR flag                                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to return the BC Interrupt Flag.             */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_bc_ir_get(TY_API_DEV *p_api_dev)
{
  return (p_api_dev->bc_ir_status[ p_api_dev->biu ]);

} /* end: api_io_bc_ir_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_GLB_CNT_INI     */
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
/*    This function is used to initialize the global message/error count     */
/*    location of the specified API1553-DS BIU.                              */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_glb_cnt_ini(TY_API_DEV *p_api_dev)
{
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_mc = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bc_ec = 0L;

} /* end: api_io_bc_glb_cnt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                  Submodule : API_IO_BC_XFER_DESC_TC_INI */
/*                                                                           */
/*   Author : Marco Motz              Project   : API1553-DS                 */
/*                                                                           */
/*   Source : C                       Tools     : PC/AT; Norton Editor;      */
/*                                                CYGNUS, GNU-C, As, and LD  */
/*                                                IDT-C 5.1 Toolkit          */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid]                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to clear the transfer/error count locations of   */
/*    the specified API1553-DS BC Transfer descriptor.                       */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_desc_tc_ini(TY_API_DEV *p_api_dev, WORD xid)
{

  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].msg_cnt = 0L;
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].err_cnt = 0L;

} /* end: api_io_bc_xfer_desc_tc_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                  Submodule : API_IO_BC_XFER_DESC_INI    */
/*                                                                           */
/*   Author : Marco Motz              Project   : API1553-DS                 */
/*                                                                           */
/*   Source : C                       Tools     : PC/AT; Norton Editor;      */
/*                                                CYGNUS, GNU-C, As, and LD  */
/*                                                IDT-C 5.1 Toolkit          */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid]                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialise the specified API1553-DS BC        */
/*    Transfer descriptor.                                                   */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_desc_ini(TY_API_DEV *p_api_dev, WORD xid)
{
  L_WORD addr, tmpl;

  /* BC Transfer Descriptor Control Word */
  tmpl = 0x30000000L; /* NOP transfer type by default */

  /* BC Buffer Header Pointer */
  addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area;

  /**************************************/
  /***   BC Transfer Descriptor       ***/
  /**************************************/
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, addr & 0x03ffffff);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = 0x00000000L;
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].img_swm = BSWAP32(0x000007FFUL);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].msg_cnt = 0L;
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].err_cnt = 0L;
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].fifo_id = BSWAP32(xid);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].dytg_id = BSWAP32(xid);

  /**************************************/
  /***   BC System Dynamic Data       ***/
  /**************************************/
  
  api_io_bc_systag_free(p_api_dev, 1/*single*/, xid);

  if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl), Out);
  }

} /* end: api_io_bc_xfer_desc_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_BH_INI          */
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
/*    Inputs    : API1553-DS BC Buffer Header Index [index]                  */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected BC Buffer Header      */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

static void api_io_bc_bh_ini(TY_API_DEV *p_api_dev, WORD index)
{
L_WORD tmpl, sq_addr, eq_addr, bp_addr;
L_WORD tmp1, tmp2, tmp3, tmp4, bs;

  tmpl = 0L;

  /* Data Buffer Queue Mode */
  tmp1 = 0L;
  tmp1 = API_BC_BH_BQM_CYCLIC & 0x03;
  tmp1 <<= 28;

  /* Data Buffer Store Mode */
  tmp2 = 0L;
  tmp2 = API_BC_BH_BSM_DISCARD & 0x03;
  tmp2 <<= 26;

  /* Buffer Size */
  bs = 0L;
  bs = (MAX_API_BUF_SIZE / 4) & 0x1F;
  bs <<= 18;

  /* Status Queue Size */
  tmp3 = 0L;
  tmp3 = API_BC_BH_STS_ONE_SQ & 0x03;
  tmp3 <<= 12;

  /* Event Queue Size */
  tmp4 = 0L;
  tmp4 = API_BC_BH_EVS_NO_EQ & 0x03;
  tmp4 <<= 10;

  /* BC Buffer Header Control Word */
  tmpl = tmp1 | tmp2 | tmp3 | tmp4 | bs;

  /* BC Status Queue Base Pointer */
  sq_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_sq_area;

  /* BC Event Queue Base Pointer */
  eq_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_eq_area;

  /* BC Buffer Queue Base Pointer */
  bp_addr = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];

  /**************************************/
  /***   BC Buffer Header             ***/
  /**************************************/
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].bh_ctrl = BSWAP32(tmpl);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].sq_addr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, sq_addr);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].eq_addr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, eq_addr);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, bp_addr);

} /* end: api_io_bc_bh_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_SQ_INI          */
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
/*    Inputs    : API1553-DS BC Status Queue Index [index]                   */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected BC Status Queue       */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_sq_ini(TY_API_DEV *p_api_dev, WORD index)
{
  /**************************************/
  /***   BC Status Queue Entry        ***/
  /**************************************/
  (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ index ].sq_ctrl = 0L;
  (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ index ].lsw2_lsw1 = 0xFFFFFFFFL;
  (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ index ].act_buf_ptr = 0L;
  (*p_api_dev->bc_sq_desc_p[ p_api_dev->biu ])[ index ].time_tag = 0L;

} /* end: api_io_bc_sq_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_EQ_INI          */
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
/*    Inputs    : API1553-DS BC Event Queue Index [index]                    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the selected BC Event Queue        */
/*    descriptor.                                                            */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_eq_ini(TY_API_DEV *p_api_dev, WORD index)
{
  /**************************************/
  /***   BC Event Queue Entry         ***/
  /**************************************/
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ index ].con1 = 0L;
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ index ].addr1 = 0L;
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ index ].con2 = 0L;
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ index ].addr2 = 0L;

  p_api_dev->bc_dytag_status[ p_api_dev->biu ][ index ] = API_OFF;
} /* end: api_io_bc_eq_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_INI             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS BC.                 */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_ini(TY_API_DEV *p_api_dev, BYTE retr, BYTE svrq, BYTE tbm, BYTE gsb)
{
  L_WORD tmpl, instr;
  WORD i;

  /* BC specific registers of System Control Block */
  api_io_cb_ini(p_api_dev, INIT_BC);

  /* System Control Word Register */
  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  LWORD_RESET_BITS(tmpl, 24, 2);
  LWORD_SET_BITS(tmpl, retr, 24, 2);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl); /* System Control Word */

  /* set BC transfer bus mode */
  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  LWORD_RESET_BITS(tmpl, 17, 1);
  LWORD_SET_BITS(tmpl, tbm, 17, 1);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl); /* System Control Word */

  /* set BC global start bus */
  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  LWORD_RESET_BITS(tmpl, 16, 1);
  LWORD_SET_BITS(tmpl, gsb, 16, 1);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl); /* System Control Word */

  /* Global BC message/error counter */
  api_io_bc_glb_cnt_ini(p_api_dev);

  /* BC Transfer Descriptors */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; i++)
  {
    api_io_bc_xfer_desc_ini(p_api_dev, i);
  }

  /* BC High Priority List */
  instr = MAKE_INSTR(API_BC_INSTR_HALT, 0);
  for(i = 0; i < MAX_API_BC_HIP_INSTR; i++)
  {
    *(p_api_dev->bc_hip_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
  }

  /* BC Low Priority List */
  instr = MAKE_INSTR(API_BC_INSTR_HALT, 0);
  for(i = 0; i < MAX_API_BC_LIP_INSTR; i++)
  {
    *(p_api_dev->bc_lip_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
  }

  /* BC Acyclic List */
  instr = MAKE_INSTR(API_BC_INSTR_HALT, 0);
  for(i = 0; i < MAX_API_BC_ACYC_INSTR; i++)
  {
    *(p_api_dev->bc_acyc_instr_p[ p_api_dev->biu ] + i) = BSWAP32(instr);
  }

  /* BC Buffer Header */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area; i++)
  {
    api_io_bc_bh_ini(p_api_dev, i);
  }

  /* BC Status Queue */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_sq_area; i++)
  {
    api_io_bc_sq_ini(p_api_dev, i);
  }

  /* BC Event Queue */
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area; i++)
  {
    api_io_bc_eq_ini(p_api_dev, i);
  }

  /* System Dynamic Data */
  api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_BC_ALL, 0, 0);

  /* Service Request and Vector Word handling */
  for(i = 0; i < 32; i++)
  {
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_instr[ i ] = 0L; /* Transfer Instruction */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_instr_rtrt[ i ] = 0L; /* Transfer Instruction */
  }

  for(i = 0; i < 2; i++)
  {
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].data = 0L; /* Data Buffer to store received Vector Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].data = 0L; /* Data Buffer to store received Vector Word */

    /* Special Transfer Descriptor */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].xf_ctrl = 0L; /* Transfer Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].bhd_ptr = 0L; /* Buffer Header Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].cw2_cw1 = 0L; /* Command Word1 and Command Word 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].img_swm = 0L; /* Gap Time Word and Status Word Exception */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].msg_cnt = 0L; /* Transfer Counter */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].err_cnt = 0L; /* Error Counter */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ i ].xf_last = 0L; /* Descriptor Pointer to Transfer causing Vector Word MC */

    /* Buffer Header */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].bh_ctrl = 0L; /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].sq_addr = 0L; /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].eq_addr = 0L; /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].bp_addr = 0L; /* Data Buffer Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].bh_ctrl = 0L; /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].sq_addr = 0L; /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].eq_addr = 0L; /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].bp_addr = 0L; /* Data Buffer Base Pointer */

    /* Status Queue */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].sq_ctrl = 0L; /* Status Queue Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].lcw_lsw = 0L; /* Last Command Word and Last Status Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].act_bptr = 0L; /* Actual Data Buffer Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].time_tag = 0L; /* Time Tag Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].sq_ctrl = 0L; /* Status Queue Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].lcw_lsw = 0L; /* Last Command Word and Last Status Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].act_bptr = 0L; /* Actual Data Buffer Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].time_tag = 0L; /* Time Tag Word */

    /* Event Queue */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].con1 = 0L; /* Event Queue Control Word and Step Size or Mask Word 1 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].addr1 = 0L; /* Address or Lower/Upper Limit values 1 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].con2 = 0L; /* Event Queue Control Word and Step Size or Mask Word 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ i ].addr2 = 0L; /* Address or Lower/Upper Limit values 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].con1 = 0L; /* Event Queue Control Word and Step Size or Mask Word 1 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].addr1 = 0L; /* Address or Lower/Upper Limit values 1 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].con2 = 0L; /* Event Queue Control Word and Step Size or Mask Word 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ i ].addr2 = 0L; /* Address or Lower/Upper Limit values 2 */

  }
  if(svrq == API_ON)
  {

    for(i = 0; i < 32; i++)
    {
      (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_instr[ i ] = BSWAP32(0x40000180L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Transfer Instruction */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_instr_rtrt[ i ] = BSWAP32(0x40000280L + p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr); /* Transfer Instruction */
    }
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 0 ].xf_ctrl = BSWAP32(0x11100000L); /* Transfer Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 0 ].bhd_ptr = BSWAP32(0x000002B8L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Buffer Header Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 0 ].cw2_cw1 = BSWAP32(0x00000410L); /* Command Word1 and Command Word 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 0 ].xf_ctrl = BSWAP32(0x11100000L); /* Transfer Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 0 ].bhd_ptr = BSWAP32(0x00000320L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Buffer Header Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 0 ].cw2_cw1 = BSWAP32(0x00000410L); /* Command Word1 and Command Word 2 */

    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 0 ].bh_ctrl = BSWAP32(0x00401000L); /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 0 ].sq_addr = BSWAP32(0x000002C8L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 0 ].eq_addr = BSWAP32(0x000002D8L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 0 ].bp_addr = BSWAP32(0x000002E8L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Data Buffer Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 0 ].bh_ctrl = BSWAP32(0x00401000L); /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 0 ].sq_addr = BSWAP32(0x00000330L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 0 ].eq_addr = BSWAP32(0x00000340L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 0 ].bp_addr = BSWAP32(0x00000350L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Data Buffer Base Pointer */


    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 1 ].xf_ctrl = BSWAP32(0x11100000L); /* Transfer Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 1 ].bhd_ptr = BSWAP32(0x000002ECL + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Buffer Header Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc[ 1 ].cw2_cw1 = BSWAP32(0x00000FF0UL); /* Command Word1 and Command Word 2 */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 1 ].xf_ctrl = BSWAP32(0x11100000L); /* Transfer Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 1 ].bhd_ptr = BSWAP32(0x00000354L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Buffer Header Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_desc_rtrt[ 1 ].cw2_cw1 = BSWAP32(0x00000FF0UL); /* Command Word1 and Command Word 2 */

    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 1 ].bh_ctrl = BSWAP32(0x00401000L); /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 1 ].sq_addr = BSWAP32(0x000002FCL + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 1 ].eq_addr = BSWAP32(0x0000030CL + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh[ 1 ].bp_addr = BSWAP32(0x0000031CL + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Data Buffer Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 1 ].bh_ctrl = BSWAP32(0x00401000L); /* Buffer Header Control Word */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 1 ].sq_addr = BSWAP32(0x00000364L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Status Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 1 ].eq_addr = BSWAP32(0x00000374L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Event Queue Base Pointer */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).xf_bh_rtrt[ 1 ].bp_addr = BSWAP32(0x00000384L + (p_api_dev->glb_mem.biu[ p_api_dev->biu ].cb_addr & ~API_GRAM2_OFFSET)); /* Data Buffer Base Pointer */
  }

  UsbTransferListClear(p_api_dev, p_api_dev->biu);

} /* end: api_io_bc_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_BH_EQ_CON       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Enable/Disable Event Queue Processing [con]                */
/*                BC Buffer Header ID [bc_hid]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control BC Event Queue processing.            */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_bh_eq_con(TY_API_DEV *p_api_dev, WORD con, L_WORD bc_hid)
{
  L_WORD tmpl;

  if(con == API_ON)
  {
    tmpl = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bh_ctrl);
    LWORD_RESET_BITS(tmpl, 10, 2);
    LWORD_SET_BITS(tmpl, API_RT_BH_EVS_ONE_EQ, 10, 2);
    (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bh_ctrl = BSWAP32(tmpl);
  }
  else
  {
    tmpl = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bh_ctrl);
    LWORD_RESET_BITS(tmpl, 10, 2);
    (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bh_ctrl = BSWAP32(tmpl);
  }

  if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_bh_desc_p[p_api_dev->biu])[bc_hid].bh_ctrl), Out);
  }

} /* end: api_io_bc_bh_eq_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_FW_DYTAG_DIS    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Buffer Header ID [bc_hid]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_fw_dytag_dis(TY_API_DEV *p_api_dev, WORD bc_hid)
{

  if(bc_hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area)
  {
    if(p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ bc_hid ].ena == API_ON)
    {
      p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ bc_hid ].ena = API_OFF;
      api_io_bc_bh_eq_con(p_api_dev, API_OFF, bc_hid);
      api_io_bc_eq_ini(p_api_dev, bc_hid);
    }
  }

} /* end: api_io_bc_fw_dytag_dis */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BC_SYSTAG_FREE       */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_free(TY_API_DEV *p_api_dev, short mode, WORD xid)
{
short j;

   if (mode == 0) {
     /* All */
     for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; j++) {
       p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ j ].id = 0;
       p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ j ].cnt = 0;
       if (p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ j ].p != NULL) {
          FREE(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ j ].p);
          p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ j ].p = NULL;
       }   
     }  
   }
   else if (mode == 1) {
     /* Single ID only */
     if (xid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) {
       p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id = 0;
       p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt = 0;
       if (p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p != NULL) {
          FREE(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p);
          p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p = NULL;
       } 
     }  
   }
   
} /* end: api_io_bc_systag_free */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BC_SYSTAG_ADD        */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_add(TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix)
{
short j,found,cnt;

   if (xid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) {

      if (p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p == NULL) {
#ifdef DEBUG
         PRINTF3("BC MALLOCed\n",0,0,0);
#endif
         
         /* Allocate Memory and Clear */
         p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p = (BYTE *) MALLOC(MAX_API_SYS_LS_SYSTAGS);
         memset(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p, 0, MAX_API_SYS_LS_SYSTAGS);
      }    

      if (p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p != NULL) {

         /* Search if Systag Index is already existing in the list */
         cnt = p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt;
         for (j=0,found=0; j<cnt; j++) {
            if ( p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p[j] == (tag_ix+1) ) {
               found = 1;
               break;
            } 
         }

         if (found == 0) {
         
            /* Add the Systag Index and increment count */
            if (cnt < MAX_API_SYS_LS_SYSTAGS) {
               p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p[cnt] = tag_ix + 1;
               p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt++;
               p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id = tag_ix + 1;
            }
         }
        
         /* DEBUG */
#ifdef DEBUG
         PRINTF3("Systag List id=%d cnt=%d:",
            p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id,
            p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt,0);
         for (j=0; j<p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt; j++) {
            PRINTF3(" %d",p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p[j],0,0);
         }
         PRINTF3("\n",0,0,0);         
#endif
      }
   }
   
} /* end: api_io_bc_systag_add */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BC_SYSTAG_SUB        */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 15.10.05   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_sub(TY_API_DEV *p_api_dev, WORD xid, BYTE tag_ix)
{
short j,k,found,cnt;
BYTE tmp_arr[MAX_API_SYS_LS_SYSTAGS];

   if (xid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) {

      if (p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p != NULL) {

         memset(tmp_arr, 0, MAX_API_SYS_LS_SYSTAGS);

         /* Search if Systag Index is already existing in the list */
         cnt = p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt;
         for (j=0,k=0,found=0; j<cnt; j++) {
            if ( p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p[j] == (tag_ix+1) ) {
               found = 1;
            } 
            else {
               /* Copy the list (without specified index) */
               tmp_arr[k] = p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p[j]; k++;
            }
         }

         if (found == 1) {
            /* Sub the Systag Index and decrement count */
            if (cnt > 0) {
               p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt--;
               memcpy(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].p, tmp_arr, MAX_API_SYS_LS_SYSTAGS);
            }
         }
      }
   }
   
} /* end: api_io_bc_systag_sub */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BC_SYSTAG_INIT_DW    */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_init_dw(TY_API_DEV *p_api_dev, WORD xid)
{
WORD bc_hid;
BYTE bc_dytag_i;
L_WORD tmp_addr, dbp;

  /* Calculate Data Buffer Pointer */
  tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr & BSWAP32(0x03fffffful) );
  bc_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area) / MAX_API_BC_BH_DESC_SIZE);
  dbp = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bp_addr & 0x03ffffff);
  bc_dytag_i = p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id;
  api_io_ls_systag_init_dw(p_api_dev, bc_dytag_i, API_BC_MODE, xid, dbp);

} /* end: api_io_bc_systag_init_dw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO               Submodule : API_IO_BC_SYSTAG_INIT_DATASET */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_init_dataset(TY_API_DEV *p_api_dev, WORD xid)
{
WORD bc_hid;
BYTE bc_dytag_i;
L_WORD tmp_addr, dbp;

  /* Calculate Data Buffer Pointer */
  tmp_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr & BSWAP32(0x03fffffful) );
  bc_hid = (WORD) ( (tmp_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area) / MAX_API_BC_BH_DESC_SIZE);
  dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ bc_hid ].bp_addr & BSWAP32(0x03fffffful) );
  bc_dytag_i = p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id;
  api_io_ls_systag_init_dataset(p_api_dev, bc_dytag_i, dbp);

} /*  end: api_io_bc_systag_init_dataset */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_SYSTAG_CON      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_systag_con(TY_API_DEV *p_api_dev, BYTE tag_ix, WORD xid)
{
L_WORD tmpl;

  if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mode == API_SYSTAG_ENA)
  {
    /* Enable System Dynamic Data */
    if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].fct != API_SYSTAG_DATASET)
    {
      api_io_bc_systag_add(p_api_dev, xid, tag_ix);
      
      if(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt == 1) {
        /* Enable Interrupt on End of Transfer */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        LWORD_RESET_BITS(tmpl, 20, 3);
        LWORD_SET_BITS(tmpl, API_BC_XFER_IR_EOT, 20, 3);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
      }

      /* Init Dynamic Data Word */
      api_io_bc_systag_init_dw(p_api_dev, xid);
    }
    else
    {
      /* Dataset */
      p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id = tag_ix + 1;

      /* Enable Interrupt on End of Transfer */
      tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
      LWORD_RESET_BITS(tmpl, 20, 3);
      LWORD_SET_BITS(tmpl, API_BC_XFER_IR_EOT, 20, 3);
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

      /* Init Dynamic Data Word */
      api_io_bc_systag_init_dataset(p_api_dev, xid);
    }
  }
  else
  {
    /* Disable System Dynamic Data */
    if(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt != 0) {
       api_io_bc_systag_sub(p_api_dev, xid, tag_ix);
    }
       
    if(p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].cnt == 0) {
       p_api_dev->bc_setup[ p_api_dev->biu ].bc_systag[ xid ].id = 0;
       
       /* Disable Interrupt */
       if( API_OFF == p_api_dev->bc_setup[ p_api_dev->biu ].xfer_interrupt[ xid ])
       {
         tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
         LWORD_RESET_BITS(tmpl, 20, 3);
         (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
       }
    }
  }

} /* end: api_io_bc_systag_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_BHD_DEF         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 04.06.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Buffer Header ID [hid],                                 */
/*                BC Data Buffer ID [bid],                                   */
/*                BC Status Queue ID [sid],                                  */
/*                BC Event Queue ID [eid],                                   */
/*                Queue Size definition [qsize],                             */
/*                Buffer Queue Mode [bqm],                                   */
/*                Buffer Store Mode [bsm],                                   */
/*                Status Queue Mode [sts],                                   */
/*                Event Queue Mode [evs],                                    */
/*                Reserved [res]                                             */
/*                                                                           */
/*    Outputs   : Amount of contiguous Data Buffers [nbufs]                  */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the buffer header structure of the     */
/*    specified API1553-DS BC.                                               */
/*                                                                           */
/*****************************************************************************/

WORD api_io_bc_bhd_def(TY_API_DEV *p_api_dev, WORD hid, WORD bid, WORD sid, WORD eid, BYTE qsize, 
                                              BYTE bqm, BYTE bsm, BYTE sts, BYTE evs, BYTE res)
{
L_WORD tmpl, tmp1, tmp2, tmp3, tmp4, bs, qs;
WORD nbufs;

  api_io_bc_fw_dytag_dis(p_api_dev, hid);
  api_io_bc_bh_ini(p_api_dev, hid);

  tmpl = res; /* dummy */
  tmpl = 0L;

  /* Data Buffer Queue Mode */
  tmp1 = 0L;
  tmp1 = bqm & 0x03;
  tmp1 <<= 28;

  /* Data Buffer Store Mode */
  tmp2 = 0L;
  tmp2 = bsm & 0x03;
  tmp2 <<= 26;

  /* Buffer Size */
  bs = 0L;
  bs = (MAX_API_BUF_SIZE / 4) & 0x1F;
  bs <<= 18;

  /* Queue Size */
  qs = 0L;
  qs = qsize & 0x0F;
  nbufs = 1L << qs;
  qs <<= 14;

  /* Status Queue Size */
  tmp3 = 0L;
  tmp3 = sts & 0x03;
  tmp3 <<= 12;

  /* Event Queue Size */
  tmp4 = 0L;
  tmp4 = evs & 0x03;
  tmp4 <<= 10;

  tmpl = tmp1 | tmp2 | tmp3 | tmp4 | bs | qs;

  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl  = BSWAP32(tmpl);
  
  tmpl = BSWAP32((*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr = BSWAP32( tmpl + (MAX_API_BC_SQ_DESC_SIZE * sid) );
  
  tmpl = BSWAP32((*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr = BSWAP32( tmpl + (MAX_API_BC_EQ_DESC_SIZE * eid) );

  tmpl = BSWAP32((*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
  (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr = BSWAP32( tmpl + (MAX_API_BUF_SIZE * bid) );

  return (nbufs);

} /* end: api_io_bc_bhd_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_XFER_ERR_CON    */
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
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                Transfer error injection type [type],                      */
/*                Transfer error word position [wpos],                       */
/*                Transfer error bit position [bpos],                        */
/*                Sync field error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits]                   */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up the error injection parameter within   */
/*    the specified BC Transfer Descriptor.                                  */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_err_con(TY_API_DEV *p_api_dev, WORD xid, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits)
{
  WORD   err_type         = 0;
  WORD   err_ext          = 0;
  WORD   err_wno          = 0;
  L_WORD tmpl             = 0;
  L_WORD err              = 0; 
  BYTE*  pPhysicalAddress = 0;
  L_WORD ulRelativeOffset = 0;

  switch(type)
  {
    case API_ERR_NONE:
      break;
    case API_ERR_CSYNC:
      err_type = 7;
      err_ext = sync &0x3F;
      err_ext <<= 9;
      wpos = 0;
      break;
    case API_ERR_DSYNC:
      err_type = 7;
      err_ext = sync &0x3F;
      err_ext <<= 9;
      break;
    case API_ERR_PARITY:
      err_type = 1;
      break;
    case API_ERR_MANHI:
      if( (bpos >= 4) && (bpos < 20) )
      {
        err_type = 5;          /* manchester error */
        err_ext = 23-bpos - 4; /* bpos 4..19  FW swaps the bits (e.g. Bit Pos 4 / Physical Manchester Err at bit 19) */
        err_ext <<= 9;         /* data bit of the error */
        err_ext |= 1 << 14;    /* manchester stuck at high fault */
      }
      if( bpos == 20 ) /* special case for parity bit */
      {
        err_type = 5;          /* manchester error */
        err_ext  = 1 << 13;    /* error is in parity bit */
        err_ext |= 1 << 14;    /* manchester stuck at high fault */
      }
      break;
    case API_ERR_MANLO:
      if( (bpos >= 4) && (bpos < 20) )
      {
        err_type = 5;          /* manchester error */
        err_ext = 23-bpos - 4; /* bpos 4..19  FW swaps the bits (e.g. Bit Pos 4 / Physical Manchester Err at bit 19) */
        err_ext <<= 9;         /* data bit of the error */
      }
      if( bpos == 20 ) /* special case for parity bit */
      {
        err_type = 5;          /* manchester error */
        err_ext  = 1 << 13;    /* error is in parity bit */
      }

      break;
    case API_ERR_CONTIG:
      err_type = 4;
      err_ext = contig &0x3F;
      err_ext <<= 9;
      break;
    case API_ERR_WCHI:
      err_type = 0;
      err_ext = 0x0200;
     
      if( wpos == 0 ) 
        /* compatibility mode, if wpos is not specified as count value use one */
        wpos = 1;
      else
        wpos = (wpos & 0x3F);

      break;
    case API_ERR_WCLO:
      err_type = 0;
      err_ext = 0x0400;

      if( wpos == 0 ) 
        /* compatibility mode, if wpos is not specified as count value use one */
        wpos = 1;
      else
        wpos = (wpos & 0x3F);

      break;
    case API_ERR_BCHI:
      err_type = 2;
      err_ext = bc_bits &0x03;
      err_ext <<= 10;
      err_ext |= 0x4000;
      break;
    case API_ERR_BCLO:
      err_type = 2;
      err_ext = bc_bits &0x03;
      err_ext <<= 10;
      break;
    case API_ERR_ALTBUS:
      /* Reserved */
      break;
    case API_ERR_ZXING_NEG:
    case API_ERR_ZXING_POS:
      if( (bpos >= 4) && (bpos <= 20) )
      {
        if(API_ERR_ZXING_NEG == type)
          err_type = 6;
        else
          err_type = 3;
        err_ext = bpos - 1;

        /*
        contig |  DLL   | Bit 14 | EWE 
        -------------------------------
        125    |    0   |     0  |  0
        187,5  |    1   |     1  |  0
        156,25 |    2   |     0  |  1
        218,75 |    3   |     1  |  1
        
        EWE 4 bits before the buffer header pointer
            In case of zero crossing deviation
            Only the first bit is used
            0 => low resolution
            1 => high resolution

        */


        /* set Bit 14 if contig is 1 or 3 */
        if( (contig % 2) != 0 )
          err_ext |= 0x20;
        err_ext <<= 9;

        if( contig >= 2 )
        {
          if( mil_hw_is_high_res_zero_crossing_supported( p_api_dev ) )
          {
            /* set EWE for high resolution */
            tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr);
            LWORD_RESET_BITS(tmpl, 26, 4);
            LWORD_SET_BITS(tmpl, 1, 26, 4);
            (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr = BSWAP32(tmpl);
          }
        }
        else
        {
          if( mil_hw_is_high_res_zero_crossing_supported( p_api_dev ) )
          {
            /* clear EWE for low resolution */
            tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr);
            LWORD_RESET_BITS(tmpl, 26, 4);
            (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr = BSWAP32(tmpl);
          }
        }
      }
      break;
    case API_ERR_HSSA:
      break;
    case API_ERR_BUS_SWITCH:
      err_type = 0;
      err_ext = 0x800;
      break;
    default:
      break;
  }

  err_type &= 0x0007;
  err_type <<= 6;
  if( ( (API_ERR_DSYNC == type) && (wpos <= 33) ) ||  /* 33 because of possibility of superceding cmd test for rt validation test plan support */
  ( (API_ERR_DSYNC != type) && (wpos <= 32) ) )
  {
    err_wno = wpos;
  }

  if( (type == API_ERR_ZXING_NEG) || (type == API_ERR_ZXING_POS) )
    err_wno++;
  err_ext &= 0xFE00;


  /* BC Transfer Descriptor Control Word */
  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
  err = err_ext | err_type | err_wno;
  LWORD_RESET_BITS(tmpl, 0, 15);
  LWORD_SET_BITS(tmpl, err, 0, 15);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
  {
    pPhysicalAddress = (BYTE*)&((*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
    ulRelativeOffset = API_GLB_MEM_ADDR_REL(pPhysicalAddress);
    
    /* Write back xf_ctrl and bc bh pointer */
    UsbSynchronizeMemoryArea( p_api_dev, ulRelativeOffset, 2*4, Out );
  }


} /* end: api_io_bc_xfer_err_con */



/*! \brief This function reads specific fields of a transfer descriptor */
/*!
    The field to read is specified by the field variable and the xid.

    The parameters in value[4] depend on the field:

    field: /see XFER_DESC_FIELD_CHN
    value[0] => 0: Primary Bus
                1: Secondary Bus

    field: /see XFER_DESC_FIELD_CW1
    value[0] => CW

    field: /see XFER_DESC_FIELD_CW2
    value[0] => CW

    field: /see XFER_DESC_FIELD_TRTYPE
    value[0] => 0: BC to RT
                1: RT to BC
                2: RT to RT
                3: NOP
    value[1] => CW1
    value[2] => CW2

    field: /see XFER_DESC_FIELD_CW1_CW2
    value[0] => CW1
    value[1] => CW2




    \param  TY_API_DEV*    The global variable container.
    \param  L_WORD         The transfer id
    \param  L_WORD         The filed to read
    \param  L_WORD[4]      The the values of the field



    \return L_WORD         Zero on success or error code on failure.
*/
L_WORD api_io_bc_xfer_desc_get(TY_API_DEV *p_api_dev, L_WORD xid, L_WORD mode, L_WORD value[4] )
{
    L_WORD retval = 0;
    L_WORD tmpl   = 0;

    memset( value, 0, sizeof(L_WORD) * 4 );

    switch(mode)
    {
    case XFER_DESC_FIELD_CHN:
        /* Extract CHN as value[0] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        value[0] = EXTRACT_BITS(tmpl, 25, 0x1);
        break;
    case XFER_DESC_FIELD_CW1:
        /* Extract CW1 as value[0] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        value[0] = EXTRACT_BITS(tmpl, 0, 0xFFFF);
        break;
    case XFER_DESC_FIELD_CW2:
        /* Extract CW2 as value[0] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        value[0] = EXTRACT_BITS(tmpl, 16, 0xFFFF);
        break;
    case XFER_DESC_FIELD_CW1_CW2:
        /* Extract CW1 as value[0] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        value[0] = EXTRACT_BITS(tmpl, 0, 0xFFFF);
        /* Extract CW2 as value[1] */
        value[1] = EXTRACT_BITS(tmpl, 16, 0xFFFF);
        break;
    case XFER_DESC_FIELD_TRTYPE:
        /* Extract TrType as value[0] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        value[0] = EXTRACT_BITS(tmpl, 28, 0x3);
        /* Extract CW1 as value[1] */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        value[1] = EXTRACT_BITS(tmpl, 0, 0xFFFF);
        /* Extract CW2 as value[2] */
        value[2] = EXTRACT_BITS(tmpl, 16, 0xFFFF);
        break;
    default:
        retval = API_ERR_INVALID_MODE;
        break;
    }


    return retval;
}


/*! \brief This function writes specific fields of a transfer descriptor */
/*!
    The field to write is specified by the field variable and the xid. For details on
    available fields and their values \see api_io_bc_xfer_desc_get

    \param  TY_API_DEV*    The global variable container.
    \param  L_WORD         The transfer id
    \param  L_WORD         The filed to write
    \param  L_WORD[4]      The the values for the field

    \return L_WORD         Zero on success or error code on failure.
*/
L_WORD api_io_bc_xfer_desc_mod(TY_API_DEV *p_api_dev, L_WORD xid, L_WORD mode, L_WORD value[4] )
{
    L_WORD retval = 0;
    L_WORD tmpl   = 0;

    switch(mode)
    {
    case XFER_DESC_FIELD_CHN:
        /* Set channel bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        LWORD_RESET_BITS(tmpl, 25, 1);
        LWORD_SET_BITS(tmpl, value[0], 25, 1);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
        break;
    case XFER_DESC_FIELD_CW1:
        /* Set CW1 bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        LWORD_RESET_BITS(tmpl, 0, 16);
        LWORD_SET_BITS(tmpl, value[0], 0, 16);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(tmpl);
        break;
    case XFER_DESC_FIELD_CW2:
        /* Set CW2 bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        LWORD_RESET_BITS(tmpl, 16, 16);
        LWORD_SET_BITS(tmpl, value[0], 16, 16);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(tmpl);
        break;
    case XFER_DESC_FIELD_CW1_CW2:
        /* Set CW1/CW2 bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        LWORD_RESET_BITS(tmpl, 0, 16);
        LWORD_SET_BITS(tmpl, value[0], 0, 16);  /* CW1 */
        LWORD_RESET_BITS(tmpl, 16, 16);
        LWORD_SET_BITS(tmpl, value[1], 16, 16); /* CW2 */
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(tmpl);
        break;
    case XFER_DESC_FIELD_TRTYPE:
        /* Set TrType to NOP to avoid invalid transfers */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        LWORD_RESET_BITS(tmpl, 28, 2);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
        /* Set CW1/CW2 bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1);
        LWORD_RESET_BITS(tmpl, 0, 16);
        LWORD_SET_BITS(tmpl, value[1], 0, 16);  /* CW1 */
        LWORD_RESET_BITS(tmpl, 16, 16);
        LWORD_SET_BITS(tmpl, value[2], 16, 16); /* CW2 */
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(tmpl);
        /* Set TrType bits */
        tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
        LWORD_RESET_BITS(tmpl, 28, 2);
        LWORD_SET_BITS(tmpl, value[0], 28, 2);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
        break;
    default:
        retval = API_ERR_INVALID_MODE;
        break;

    }


    return retval;

}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_XFER_DEF        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                BC Buffer Header ID [hid],                                 */
/*                Transfer Type [xfer_type],                                 */
/*                Transfer Channel [chn],                                    */
/*                RT Address of transmitting terminal [xmt_rt],              */
/*                RT Address of receiving terminal [rcv_rt],                 */
/*                Subaddress of transmitting terminal [xmt_sa],              */
/*                Subaddress of receiving terminal [rcv_sa],                 */
/*                Word Count/Modecode field [wcnt],                          */
/*                Transfer Interrupt Control [xfer_ir],                      */
/*                Transfer Halt Control [xfer_hlt],                          */
/*                Retry enable/disable [rte],                                */
/*                SW Exception Handling [sxh],                               */
/*                Response control [rsp],                                    */
/*                Reserved [res],                                            */
/*                Status Word Exception Mask [swxm],                         */
/*                Transfer error injection type [type],                      */
/*                Transfer error word position [wpos],                       */
/*                Transfer error bit position [bpos],                        */
/*                Sync field error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits],                  */
/*                Transfer Delay Mode [delay_mode],                          */
/*                Transfer Wait Time [gap]                                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set-up a complete BC Transfer including       */
/*    error injection.                                                       */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_def(TY_API_DEV *p_api_dev, WORD xid, WORD hid, BYTE xfer_type, BYTE chn, BYTE xmt_rt, BYTE rcv_rt, BYTE xmt_sa, BYTE rcv_sa, BYTE wcnt, BYTE xfer_ir, BYTE xfer_hlt, BYTE rte, BYTE sxh, BYTE rsp, BYTE res /* 11.12.00 */, L_WORD swxm, BYTE type, BYTE wpos, BYTE bpos, BYTE sync, BYTE contig, BYTE bc_bits, BYTE delay_mode, WORD gap)
{
  WORD cw1, cw2, img, swm;
  BYTE dly, sxi, srvw_id;
  L_WORD tmpl;
  /*
  PRINTF0("api_io_bc_xfer_def:\n");
  PRINTF1("xid: %08x\n", xid);
  PRINTF1("hid: %08x\n", hid);
  PRINTF1("xfer_type: %08x\n", xfer_type);
  PRINTF1("chn: %08x\n", chn);
  PRINTF1("xmt_rt: %08x\n", xmt_rt);
  PRINTF1("rcv_rt: %08x\n", rcv_rt);
  PRINTF1("xmt_sa: %08x\n", xmt_sa);
  PRINTF1("rcv_sa: %08x\n", rcv_sa);
  PRINTF1("wcnt: %08x\n", wcnt);
  PRINTF1("xfer_ir: %08x\n", xfer_ir);
  PRINTF1("xfer_hlt: %08x\n", xfer_hlt);
  PRINTF1("rte: %08x\n", rte);
  PRINTF1("sxh: %08x\n", sxh);
  PRINTF1("rsp: %08x\n", rsp);
  PRINTF1("swxm: %08x\n", swxm);
  PRINTF1("type: %08x\n", type);
  PRINTF1("wpos: %08x\n", wpos);
  PRINTF1("bpos: %08x\n", bpos);
  PRINTF1("sync: %08x\n", sync);
  PRINTF1("contig: %08x\n", contig);
  PRINTF1("bc_bits: %08x\n", bc_bits);
  PRINTF1("delay_mode: %08x\n", delay_mode);
  PRINTF1("gap: %08x\n", gap);
   */

  cw1 = 0;
  cw2 = 0;
  img = 0;
  swm = 0;

  /* Service Request and Vector Word handling */
  sxi = API_BC_SRVW_DIS;
  srvw_id = API_BC_SRVW_ID_NONE;
  if(p_api_dev->bc_srvw_con[ p_api_dev->biu ] == API_ON)
  {
    switch(sxh)
    {
       /* decode sxh */
      case API_BC_SRVW_DIS:
        break;
      case API_BC_SRVW_ENA:
        sxi = API_BC_SRVW_ENA;
        break;
      case API_BC_SRVW_SINGLE:
        srvw_id = API_BC_SRVW_ID_SINGLE;
        break;
      case API_BC_SRVW_MULTIPLE:
        srvw_id = API_BC_SRVW_ID_MULDEL;
        break;
      default:
        break;
    }
  }

  /* Init BC Transfer Descriptor */
  api_io_bc_xfer_desc_ini(p_api_dev, xid);

  /* 25.05.00 : System Dynamic Data */
  api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_XID_ONLY, xid, 0);

  /* Buffer Header Pointer */
  tmpl = BSWAP32((*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr);
  tmpl += (hid *MAX_API_RT_BH_DESC_SIZE);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr = BSWAP32(tmpl);

  /* Command Words */
  switch(xfer_type &0x03)
  {
    case API_BC_XFER_TYPE_BCRT:
       /* BC-RT Transfer */
      cw1 = MAKE_CW(rcv_rt, 0, rcv_sa, wcnt);
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(MAKE_W2LWORD(0, cw1) );

      /* Service Request and Vector Word handling */
      switch(srvw_id)
      {
      case API_BC_SRVW_ID_NONE:
        break;
      case API_BC_SRVW_ID_SINGLE:
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_SINGLE_RX ].ta[ rcv_rt ].sa_mid[ rcv_sa ].xid = xid;
        break;
      case API_BC_SRVW_ID_MULDEL:
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_RX ].ta[ rcv_rt ].sa_mid[ rcv_sa ].xid = xid;
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_DELETE_RX ].ta[ rcv_rt ].sa_mid[ rcv_sa ].xid = xid;
        break;
      default:
        break;
      }
      break;

    case API_BC_XFER_TYPE_RTBC:
       /* RT-BC Transfer */
      cw1 = MAKE_CW(xmt_rt, 1, xmt_sa, wcnt);
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(MAKE_W2LWORD(0, cw1) );

      /* Service Request and Vector Word handling */
      switch(srvw_id)
      {
      case API_BC_SRVW_ID_NONE:
        break;
      case API_BC_SRVW_ID_SINGLE:
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_SINGLE_TX ].ta[ xmt_rt ].sa_mid[ xmt_sa ].xid = xid;
        break;
      case API_BC_SRVW_ID_MULDEL:
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_MULTIP_TX ].ta[ xmt_rt ].sa_mid[ xmt_sa ].xid = xid;
        p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ API_BC_SRVW_ID_DELETE_TX ].ta[ xmt_rt ].sa_mid[ xmt_sa ].xid = xid;
        break;
      default:
        break;
      }
      break;

    case API_BC_XFER_TYPE_RTRT:
       /* RT-RT Transfer */
      cw1 = MAKE_CW(rcv_rt, 0, rcv_sa, wcnt);
      cw2 = MAKE_CW(xmt_rt, 1, xmt_sa, wcnt);
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1 = BSWAP32(MAKE_W2LWORD(cw2, cw1) );
      break;

    default:
      break;
  }


  dly = delay_mode &0x03;
  swm = swxm &0x07FF;
  switch(dly)
  {
    case API_BC_XFER_IMG_TWT:
    case API_BC_XFER_IMG_STD:
      img = dly;
      img <<= 14;
      img |= (gap &0x3FFF);
      break;
    case API_BC_XFER_IMG_FAST:
      img = dly;
      img <<= 14;
      img |= (gap &0x003F);
      break;
    default:
      img = 0;
      break;
  }

  /* Intermessage Gap Time Word and Status Word Exception Mask */
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].img_swm = BSWAP32(MAKE_W2LWORD(img, swm) );


  /*** Setup BC Transfer Descriptor Control Word ***/

  /* Service Request and Vector Word handling */
  if(sxi == API_BC_SRVW_ENA)
  {
    /* Allow only for BC-RT or RT-BC, no MC transfer type, no Fast-IMG */
    if( ( ( (xfer_type == API_BC_XFER_TYPE_BCRT) && (rcv_sa <= 31) ) || ( (xfer_type == API_BC_XFER_TYPE_RTBC) && (xmt_sa <= 31) ) || ( (xfer_type == API_BC_XFER_TYPE_RTRT) && (xmt_sa <= 31) && (rcv_sa <= 31) ) ) && (dly != API_BC_XFER_IMG_FAST) )
    {
      ;
    }
    else
      sxi = API_BC_SRVW_DIS;
  }
  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Status Word Exception Handling */
  LWORD_RESET_BITS(tmpl, 15, 2);
  LWORD_SET_BITS(tmpl, sxi, 15, 2); /* 11.12.00: mm */
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Halt Control */
  LWORD_RESET_BITS(tmpl, 17, 3);
  LWORD_SET_BITS(tmpl, xfer_hlt, 17, 3);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Interrupt Control */
  LWORD_RESET_BITS(tmpl, 20, 3);
  LWORD_SET_BITS(tmpl, xfer_ir, 20, 3);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  if(rte <= API_BC_XFER_RETRY_ENA)
  {
    tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Retry Control */
    LWORD_RESET_BITS(tmpl, 23, 1);
    LWORD_SET_BITS(tmpl, rte, 23, 1); /* 11.12.00: mm */
    (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
  }
  else
  {
     /* set toggle bus bit */
    tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].img_swm); /* Toggle Bus Control */
    LWORD_RESET_BITS(tmpl, 15, 1);
    LWORD_SET_BITS(tmpl, (BYTE) (rte - 2), 15, 1);
    (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].img_swm = BSWAP32(tmpl);
  }

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Auto Return Control */
  LWORD_RESET_BITS(tmpl, 24, 1);
  LWORD_SET_BITS(tmpl, 0, 24, 1);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Channel Select */
  LWORD_RESET_BITS(tmpl, 25, 1);
  LWORD_SET_BITS(tmpl, chn, 25, 1);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Response Control */
  LWORD_RESET_BITS(tmpl, 26, 2);
  LWORD_SET_BITS(tmpl, rsp, 26, 2);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  /* Error Injection */
  api_io_bc_xfer_err_con(p_api_dev, xid, type, wpos, bpos, sync, contig, bc_bits);

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl); /* Transfer Type */
  LWORD_RESET_BITS(tmpl, 28, 2);
  LWORD_SET_BITS(tmpl, xfer_type, 28, 2);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);

  if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
  {
      /* If BC is running, we have to set up the transfer on-board immediately */
      UsbTransferListAdd(p_api_dev, p_api_dev->biu, xid, 
                     &(*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ], TRUE);
  }
  else
  {
      UsbTransferListAdd(p_api_dev, p_api_dev->biu, xid, 
                     &(*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ], FALSE);
  }

  

} /* end: api_io_bc_xfer_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_XFER_CTRL       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 15.12.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                Enable/Disable BC Transfer [mode]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the specified BC transfer.            */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_ctrl(TY_API_DEV *p_api_dev, WORD xid, BYTE mode)
{
L_WORD tmpl,xfer_type;
BYTE ena_mode;
WORD cw1, cw2;

    cw1 = 0;
    cw2 = 0;

    ena_mode = mode & 0x01;
    
    if(ena_mode == API_OFF)
    {
      /* Insert NOP Transfer Type */
      xfer_type = API_BC_XFER_TYPE_NOP;
      tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
      LWORD_RESET_BITS(tmpl, 28, 2);
      LWORD_SET_BITS(tmpl, xfer_type, 28, 2);
      (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
    }
    else
    {
      /* Restore original Transfer Type from Command Words */
      tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
      xfer_type = (tmpl >> 28) & 0x03L;
      if(xfer_type == API_BC_XFER_TYPE_NOP)
      {
        cw1 = LO_WORD(BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1) );
        cw2 = HI_WORD(BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].cw2_cw1) );
        if( (cw1 != 0x0000) && (cw2 != 0x0000) )
          xfer_type = API_BC_XFER_TYPE_RTRT;
         /* RT-RT Transfer */
        else
        {
          /* Check Transmit/Receive Bit */
          if( (cw1 &0x0400) == 0x0000)
            xfer_type = API_BC_XFER_TYPE_BCRT;
           /* BC-RT Transfer */
          else
            xfer_type = API_BC_XFER_TYPE_RTBC;
           /* RT-BC Transfer */
        }

        LWORD_RESET_BITS(tmpl, 28, 2);
        LWORD_SET_BITS(tmpl, xfer_type, 28, 2);
        (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
      }
    }

    wbflush();

    if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeLWord(p_api_dev, 
            API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl), Out);
    }

} /* end: api_io_bc_xfer_ctrl */


L_WORD api_io_bc_modify_def(TY_API_DEV *p_api_dev, L_WORD id, TY_API_BC_MODIFY_INSTRUCTION * modify, L_WORD * pbi_relative_offset )
{
    L_WORD i;
    L_WORD * fw_modify_descriptor;
    L_WORD hip_base = p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_hip_instr;
    L_WORD hip_size = p_api_dev->glb_mem.biu[p_api_dev->biu].size.bc_hip_instr;
    L_WORD hip_end  = hip_base + hip_size;
    L_WORD offset;

    /* -- validate input --- */

    if (id < 1)
        return API_ERR_INVALID_ID;


    /* -- prepare offsets --- */

    offset = hip_end - (id * 6*sizeof(L_WORD));

    fw_modify_descriptor = (L_WORD*)API_GLB_GLOBREL_TO_TSWPTR(offset);

    /* -- sync mirror in --- */

    if (p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
    {
        for (i=0; i<6; i++){
            UsbSynchronizeLWord(p_api_dev, offset + i, In);
        }
    }
    
    /* -- limit input --- */

    modify->type                        &= 0x3;
    modify->compare_control             &= 0x7;       /* COMPC */
    modify->write_type                  &= 0x3;       /* WRTYP */
    modify->arithmetic_operation        &= 0x7;       /* OPC   */
    modify->re_load                     &= 0x1;       /* RL    */
    modify->operand                     &= 0xFFFFF;   /* OPR   */
    modify->compare_word_source_offset  &= 0x3ffffff;
    modify->destination_offset          &= 0x3ffffff;


    /* -- write descriptor --- */

    fw_modify_descriptor[0] =  (modify->type<<30)
                             | (modify->compare_control<<26) 
                             | (modify->write_type<<24) 
                             | (modify->arithmetic_operation<<21) 
                             | (modify->re_load<<20) 
                             | modify->operand;

    fw_modify_descriptor[1] = modify->compare_word_source_offset;
    fw_modify_descriptor[2] = modify->destination_offset;
    fw_modify_descriptor[3] = modify->mask_word;
    fw_modify_descriptor[4] = modify->compare_word;
    fw_modify_descriptor[5] = modify->destination_argument;


    /* -- sync mirror out -- */
    if (p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
    {
        for( i=0; i<6; i++ ){
            UsbSynchronizeLWord(p_api_dev, offset + i, Out);
        }
    }


    /* -- return pbi offset for use with modify instruction -- */

    *pbi_relative_offset = API_GLB_GLOBREL_TO_PBIREL(offset);

    return 0;
}













#endif 

#ifdef CMPL4SIM
/*****************************************************************************/
/***                   Fifo Queue Functions                                ***/
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_INI_FIFO          */
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
/*    Inputs    : Amount of Fifo's in queue [f_nbr]                          */
/*                Amount of Buffer's in one Fifo [size]                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to allocate a buffer pool in the              */
/*     shared RAM area                                           */
/*                                                                           */
/*****************************************************************************/
void api_io_ini_fifo(TY_API_DEV *p_api_dev, BYTE f_nbr, WORD size)
{
  L_WORD queue_size, tmp_size, t_size;
  L_WORD i;

  p_api_dev->queue_setup[ p_api_dev->biu ].max_queue = f_nbr;
  p_api_dev->queue_setup[ p_api_dev->biu ].max_size = size;

  tmp_size = ( (size + 1) / 2) *2;
  t_size = (tmp_size *MAX_API_BUF_SIZE);
  queue_size = (size *MAX_API_BUF_SIZE);

  for(i = 0; i < p_api_dev->queue_setup[ p_api_dev->biu ].max_queue; i++)
  {

    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr = (API_FIFO_MEM_START_ADDR + (i *queue_size) );
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].half_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr + (t_size / 2);
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].mode = API_OFF;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].xid = 0;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].rt = 0xFF;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].sa = 0xFF;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].flag = API_OFF;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].buf_cnt = 0;
  }

} /* end: api_io_ini_fifo */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_XFER_FIFO_CON   */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Transfer ID [xid],                                      */
/*                BC Fifo Tagging Index [fifo_i]                             */
/*                                                                           */
/*    Outputs   : none                                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to control the BC Transfer Fifo Data function.    */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_xfer_fifo_con(TY_API_DEV *p_api_dev, WORD xid, WORD fifo_i)
{
  WORD index, id, j;
  L_WORD tmpl, desc_addr, dbp, saddr, current_id;
  WORD volatile *dqueue_p;
  WORD volatile *dbp_p;


  /* Dynamic Tagging Index */

  tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].fifo_id);
  LWORD_RESET_BITS(tmpl, 16, 16);
  LWORD_SET_BITS(tmpl, fifo_i, 16, 16);
  (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].fifo_id = BSWAP32(tmpl);
  if(fifo_i != 0)
  {

    /* Get Buffer Address from Buffer Header */
    desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].bhd_ptr & BSWAP32(0x03fffffful) );
    desc_addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area;
    index = (WORD) (desc_addr / MAX_API_BC_BH_DESC_SIZE);
    dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr & BSWAP32(0x03fffffful) );
    dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp);

    id = (fifo_i - 1);
    saddr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].saddr;
    current_id = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].cur_buf_addr;
    /* Set pointer to Dataset Memory address */
    dqueue_p = (WORD*)API_FIFO_MEM_ADDR_ABS(saddr);

    /* Copy Dataset to Buffer */
    for(j = 0; j < MAX_API_BUF_SIZE / 2; j++)
    {
      *(dbp_p + j) = *(dqueue_p + j);
    }

    current_id += MAX_API_BUF_SIZE;
    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ id ].cur_buf_addr = current_id;

    /* Enable Interrupt on End of Transfer */
    tmpl = BSWAP32( (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl);
    LWORD_RESET_BITS(tmpl, 20, 3);
    LWORD_SET_BITS(tmpl, API_BC_XFER_IR_EOT, 20, 3);
    (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid ].xf_ctrl = BSWAP32(tmpl);
  }

} /* end: api_io_bc_xfer_fifo_con */
#endif 

#ifdef CMPL4REP
/*****************************************************************************/
/***                   Replay Functions                                    ***/
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_REP_RT_CON        */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.08.99   Update : 01.08.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Replay mode[con]                                      */
/*                RT set mode [mode]                                         */
/*                RT [rt]                                                    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_REP_RT_CON' instruction to set the     */
/*    Replay Remote Terminal Enable register.                                */
/*                                                                           */
/*****************************************************************************/

void api_io_rep_rt_con(TY_API_DEV *p_api_dev, BYTE con, BYTE mode, BYTE rt)
{
  L_WORD tmpl;

  switch(con)
  {
    case API_RT_ALL:
       /* All RT's */
      if(mode == API_ON)
      {
        (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = 0xFFFFFFFFL;
      }
      else
      {
        if(mode == API_OFF)
        {
          (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = 0L;
        }
      }
      break;

    case API_RT_SING:
       /* Single RT */
      tmpl = 0;
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew);

      if(mode == API_ON)
      {
        LWORD_RESET_BITS(tmpl, rt, 1);
        LWORD_SET_BITS(tmpl, 1, rt, 1);
      }
      else
      {
        if(mode == API_OFF)
        {
          LWORD_RESET_BITS(tmpl, rt, 1);
        }
      }
      (*p_api_dev->cb_p[ p_api_dev->biu ]).rrew = BSWAP32(tmpl);

      break;

    default:
      break;
  }

} /* end: api_io_rep_rt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_REP_BUF_INI        */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.08.99   Update : 01.08.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS REP buffer area.    */
/*                                                                           */
/*****************************************************************************/

void api_io_rep_buf_ini(TY_API_DEV *p_api_dev)
{
  L_WORD volatile *ptr;
  L_WORD i;

  ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf);
  for(i = 0; i < API_REP_BUF_AREA_BYTE_SIZE / 4; i++)
    *(ptr + i) = 0L;

} /* end: api_io_rep_buf_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_REP_INI            */
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
/*    This function is used to initialize the API1553-DS REP Mode.           */
/*                                                                           */
/*****************************************************************************/

void api_io_rep_ini(TY_API_DEV *p_api_dev, BYTE cet, BYTE nct, BYTE cyc, BYTE nimg, BYTE alt, BYTE rlt, BYTE rint, L_WORD min, L_WORD msec, BYTE sign, L_WORD fsize)

{
  L_WORD tmpl, lval, entry_cnt;

  /* REP specific registers of System Control Block */
  api_io_cb_ini(p_api_dev, INIT_REP);

  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).rcw);

  lval = ( (API_REP_BUF_AREA_BYTE_SIZE / 0x10000) - 1);

  /* Set RBSZ Bits */
  LWORD_RESET_BITS(tmpl, 0, 10);
  LWORD_SET_BITS(tmpl, lval, 0, 10);

  /* Set RINT Bits */
  LWORD_RESET_BITS(tmpl, 16, 3);
  LWORD_SET_BITS(tmpl, rint, 16, 3);

  /* Set CET Bit */
  LWORD_RESET_BITS(tmpl, 20, 1);
  LWORD_SET_BITS(tmpl, cet, 20, 1);

  /* Set NCT Bit */
  LWORD_RESET_BITS(tmpl, 21, 1);
  LWORD_SET_BITS(tmpl, nct, 21, 1);

  /* Set CYC Bit */
  LWORD_RESET_BITS(tmpl, 22, 1);
  LWORD_SET_BITS(tmpl, cyc, 22, 1);

  /* Set NIMG Bit */
  LWORD_RESET_BITS(tmpl, 23, 1);
  LWORD_SET_BITS(tmpl, nimg, 23, 1);

  /* Set ALT Bit */
  LWORD_RESET_BITS(tmpl, 24, 1);
  LWORD_SET_BITS(tmpl, alt, 24, 1);

  /* Set RLT Bit */
  LWORD_RESET_BITS(tmpl, 25, 1);
  LWORD_SET_BITS(tmpl, rlt, 25, 1);

  (*p_api_dev->cb_p[ p_api_dev->biu ]).rcw = BSWAP32(tmpl); /* Replay Control Word */

  entry_cnt = (fsize / 4);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).rec = BSWAP32(entry_cnt); /* Replay Entry Count */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).raec = BSWAP32(entry_cnt); /* Replay Actual Entry Count */

  p_api_dev->sim_rep[ p_api_dev->biu ].mode = API_OFF;
  p_api_dev->sim_rep[ p_api_dev->biu ].rpi_cnt = 0; /* 25.05.00 */
  p_api_dev->sim_rep[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf;
  p_api_dev->sim_rep[ p_api_dev->biu ].size = 0L;
  p_api_dev->sim_rep[ p_api_dev->biu ].entry_cnt = entry_cnt;
  p_api_dev->sim_rep[ p_api_dev->biu ].flag = API_OFF;

  /*          
  PRINTF1("mode:        %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].mode);
  PRINTF1("rpi_cnt:    %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].rpi_cnt);
  PRINTF1("saddr:       %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].saddr);
  PRINTF1("size:        %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].size);
  PRINTF1("entry_cnt:  %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].entry_cnt);
  PRINTF1("flag:       %d\n\r",p_api_dev->sim_rep[p_api_dev->biu].flag);
   */

  /* set Replay Absolute Minute/Microsecond Register */
  if(min > 0x80520)
    min = 0x80520;
  if(msec > 0x39386FF)
    msec = 0x39386FF;

  if(sign != 0)
  {
    /* negative Offset */
    min = (~min) + 1;
    msec = (~msec) + 1;
  }

  (*p_api_dev->cb_p[ p_api_dev->biu ]).ramto = BSWAP32(min); /* Replay Absolute Minute Offset Register */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).ramio = BSWAP32(msec); /* Replay Absolute Microsecond Offset Register */

  /* Clear Buffer */
  api_io_rep_buf_ini(p_api_dev);

} /* end: api_io_rep_ini */
#endif 

#ifdef CMPL4MON
/*****************************************************************************/
/***                   Bus Monitor Functions                               ***/
/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_IR_CLR          */
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
/*    This function is utilized to clear the BM Interrupt Flag.              */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ir_clr(TY_API_DEV *p_api_dev)
{
  p_api_dev->bm_ir_status[ p_api_dev->biu ] = API_OFF;

} /* end: api_io_bm_ir_clr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_IR_GET          */
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
/*    Outputs   : ISR flag                                                   */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to return the BM Interrupt Flag.             */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_bm_ir_get(TY_API_DEV *p_api_dev)
{
  return (p_api_dev->bm_ir_status[ p_api_dev->biu ]);

} /* end: api_io_bm_ir_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_GLB_CNT_INI     */
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
/*    This function is used to initialize the global message/error count     */
/*    location of the specified API1553-DS BIU.                              */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_glb_cnt_ini(TY_API_DEV *p_api_dev)
{
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_mc = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_ec = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_pri = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_wcnt_sec = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_xfer_cnt_pri = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_xfer_cnt_sec = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_err_cnt_pri = 0L;
  (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_err_cnt_sec = 0L;
} /* end: api_io_bm_glb_cnt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_RT_ACT_INI_CNT  */
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
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS BM Activity         */
/*    Message and Error counter locations.                                   */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_rt_act_ini_cnt(TY_API_DEV *p_api_dev, BYTE rt)
{
    TY_API_BM_ACTIVITY *entry;
    AiUInt32 sa;

    for(sa = 0; sa < MAX_API_RT_MC_SA; sa++)
    {
        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
        entry->status = 0;
        entry->transfer_count = 0;
        entry->error_count = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
        entry->status = 0;
        entry->transfer_count = 0;
        entry->error_count = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
        entry->status = 0;
        entry->transfer_count = 0;
        entry->error_count = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
        entry->status = 0;
        entry->transfer_count = 0;
        entry->error_count = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
        if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;
    }
} /* end: api_io_bm_rt_act_ini_cnt */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_RT_ACT_INI      */
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
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS BM Activity and     */
/*    Message Filter page descriptors.                                       */
/*                                                                           */
/*****************************************************************************/


void api_io_bm_rt_act_ini(TY_API_DEV *p_api_dev, BYTE rt)
{
  volatile TY_API_BM_ACTIVITY *entry;
  AiUInt32 sa;

  for(sa = 0; sa < MAX_API_RT_MC_SA; sa++)
  {
      entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
      entry->control = BSWAP16(0x2000);
      entry->status = 0;
      entry->transfer_count = 0;
      entry->error_count = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

      entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
      entry->control = BSWAP16(0x2000);
      entry->status = 0;
      entry->transfer_count = 0;
      entry->error_count = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

      entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
      entry->control = BSWAP16(0x2000);
      entry->status = 0;
      entry->transfer_count = 0;
      entry->error_count = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;

      entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
      entry->control = BSWAP16(0x2000);
      entry->status = 0;
      entry->transfer_count = 0;
      entry->error_count = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->transfer_count_2 = 0;
      if (p_api_dev->ulUseEnhancedBmActivityPage)   entry->error_count_2 = 0;
  }

} /* end: api_io_bm_rt_act_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_TCB_DESC_INI    */
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
/*    Inputs    : Trigger Control Block Index [tid]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS BM Activity and     */
/*    Message Filter page descriptors.                                       */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_tcb_desc_ini(TY_API_DEV *p_api_dev, BYTE tid)
{
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].trg_ctrl = 0L; /* Trigger Control Word */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].next_eom = 0L; /* Next Index, EOM Index */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].wmask = 0L; /* Word Mask */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].uli_lli = 0L; /* Upper and Lower Limit */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].res[ 0 ] = 0L; /* Reserved */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].res[ 1 ] = 0L; /* Reserved */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].res[ 2 ] = 0L; /* Reserved */
  (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].res[ 3 ] = BSWAP32(tid); /* Reserved */

} /* end: api_io_bm_tcb_desc_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_TCB_DESC_CON    */
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
/*    Inputs    : Trigger Control Block Index [tid]                          */
/*                Trigger Type [tt]                                          */
/*                Strobe on Trigger [sot]                                    */
/*                Interrupt on Trigger [tri]                                 */
/*                Inversion of Limit Check [inv]                             */
/*                Reset Mask for Monitor Status Trigger Word [tres]          */
/*                Set Mask for Monitor Status Trigger Word [tset]            */
/*                Trigger Specification [tsp]                                */
/*                Next Trigger Control Block Index to use [next]             */
/*                Next Trigger Control Block Index to use on EOM [eom]       */
/*                Trigger Data Word [tdw]                                    */
/*                Trigger Mask Word [tmw]                                    */
/*                Trigger Upper Limit value [tuli]                           */
/*                Trigger Lower Limit value [tlli]                           */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to setup a complete BM Trigger Control Block.    */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_tcb_desc_con(TY_API_DEV *p_api_dev, BYTE tid, BYTE tt, BYTE sot, BYTE tri, BYTE inv, BYTE tres, BYTE tset, BYTE tsp, BYTE next, BYTE eom, WORD tdw, WORD tmw, WORD tuli, WORD tlli)
{
  L_WORD tmpl;

  tmpl = 0L;

  switch(tt)
  {
    case API_BM_TCB_TYPE_ERR:
       /* Trigger on Error Condition */
      LWORD_SET_BITS(tmpl, tt, 29, 3);
      if(sot == API_BM_TCB_SOT_ENA)
        LWORD_SET_BITS(tmpl, 1, 22, 1);
      if(tri == API_BM_TCB_TRI_ENA)
        LWORD_SET_BITS(tmpl, 1, 21, 1);
      if(inv == API_BM_TCB_INV_ENA)
        LWORD_SET_BITS(tmpl, 1, 20, 1);
      LWORD_SET_BITS(tmpl, tres, 8, 8);
      LWORD_SET_BITS(tmpl, tset, 0, 8);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].trg_ctrl = BSWAP32(tmpl);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].next_eom = BSWAP32(MAKE_B2LWORD(0, 0, 0xFF, 0xFF) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].wmask = BSWAP32(MAKE_W2LWORD(0, tdw) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].uli_lli = BSWAP32(MAKE_W2LWORD(0, 0) );
      break;

    case API_BM_TCB_TYPE_EXT:
       /* Trigger on External Event */
      LWORD_SET_BITS(tmpl, tt, 29, 3);
      if(sot == API_BM_TCB_SOT_ENA)
        LWORD_SET_BITS(tmpl, 1, 22, 1);
      if(tri == API_BM_TCB_TRI_ENA)
        LWORD_SET_BITS(tmpl, 1, 21, 1);
      LWORD_SET_BITS(tmpl, tres, 8, 8);
      LWORD_SET_BITS(tmpl, tset, 0, 8);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].trg_ctrl = BSWAP32(tmpl);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].next_eom = BSWAP32(MAKE_B2LWORD(0, 0, 0xFF, 0xFF) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].wmask = BSWAP32(MAKE_W2LWORD(0, 0) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].uli_lli = BSWAP32(MAKE_W2LWORD(0, 0) );
      break;

    case API_BM_TCB_TYPE_RCV:
       /* Trigger on Received Word */
      LWORD_SET_BITS(tmpl, tt, 29, 3);
      LWORD_SET_BITS(tmpl, tsp, 23, 6);
      if(sot == API_BM_TCB_SOT_ENA)
        LWORD_SET_BITS(tmpl, 1, 22, 1);
      if(tri == API_BM_TCB_TRI_ENA)
        LWORD_SET_BITS(tmpl, 1, 21, 1);
      if(inv == API_BM_TCB_INV_ENA)
        LWORD_SET_BITS(tmpl, 1, 20, 1);
      LWORD_SET_BITS(tmpl, tres, 8, 8);
      LWORD_SET_BITS(tmpl, tset, 0, 8);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].trg_ctrl = BSWAP32(tmpl);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].next_eom = BSWAP32(MAKE_B2LWORD(0, 0, next, eom) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].wmask = BSWAP32(MAKE_W2LWORD(0, tmw) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].uli_lli = BSWAP32(MAKE_W2LWORD( (WORD) (tdw &tmw), (WORD) (tdw &tmw) ) );
      break;

    case API_BM_TCB_TYPE_DATA:
       /* Trigger on Data Value */
      LWORD_SET_BITS(tmpl, tt, 29, 3);
      LWORD_SET_BITS(tmpl, tsp, 23, 6);
      if(sot == API_BM_TCB_SOT_ENA)
        LWORD_SET_BITS(tmpl, 1, 22, 1);
      if(tri == API_BM_TCB_TRI_ENA)
        LWORD_SET_BITS(tmpl, 1, 21, 1);
      if(inv == API_BM_TCB_INV_ENA)
        LWORD_SET_BITS(tmpl, 1, 20, 1);
      LWORD_SET_BITS(tmpl, tres, 8, 8);
      LWORD_SET_BITS(tmpl, tset, 0, 8);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].trg_ctrl = BSWAP32(tmpl);
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].next_eom = BSWAP32(MAKE_B2LWORD(0, 0, next, eom) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].wmask = BSWAP32(MAKE_W2LWORD(0, tmw) );
      (*p_api_dev->bm_tcb_desc_p[ p_api_dev->biu ])[ tid ].uli_lli = BSWAP32(MAKE_W2LWORD( (WORD) (tuli &tmw), (WORD) (tlli &tmw) ) );
      break;

    default:
      break;
  }

} /* end: api_io_bm_tcb_desc_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_BUF_INI         */
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
/*    This function is used to initialize the API1553-DS BM buffer area.     */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_buf_ini(TY_API_DEV *p_api_dev)
{
  L_WORD volatile *ptr;
  L_WORD i;

  ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf);
  for(i = 0; i < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4; i++)
    *(ptr + i) = 0L;

} /* end: api_io_bm_buf_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI_MSG_FLT     */
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
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                Message Capture Enable Bits for Receive  RT SA [rcv_sa]    */
/*                Message Capture Enable Bits for Transmit RT SA [xmt_sa]    */
/*                Message Capture Enable Bits for Receive  SA MC [rcv_mc]    */
/*                Message Capture Enable Bits for Transmit SA MC [xmt_mc]    */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set/reset the Message Filter bits within      */
/*    the BM Activity and Message Filter page for the selected RT.           */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ini_msg_flt(TY_API_DEV *p_api_dev, BYTE rt, L_WORD rcv_sa, L_WORD xmt_sa, L_WORD rcv_mc, L_WORD xmt_mc)
{
    TY_API_BM_ACTIVITY *entry;
    L_WORD mask;
    AiUInt32 sa;

    for(sa = 0; sa < MAX_API_RT_MC_SA; sa++)
    {
        mask = 0x1L << sa;

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
        if( (rcv_sa & mask) == 0 )
            entry->control &= BSWAP16(0xDFFF);
        else
            entry->control |= BSWAP16(0x2000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
        if( (xmt_sa & mask) == 0L)
            entry->control &= BSWAP16(0xDFFF);
        else
            entry->control |= BSWAP16(0x2000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
        if( (rcv_mc & mask) == 0L)
            entry->control &= BSWAP16(0xDFFF);
        else
            entry->control |= BSWAP16(0x2000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
        if( (xmt_mc & mask) == 0L)
            entry->control &= BSWAP16(0xDFFF);
        else
            entry->control |= BSWAP16(0x2000);
    }

} /* end: api_io_bm_ini_msg_flt */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI_MSG_ILL     */
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
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                Message Illegalize Bits for Receive  RT SA [rcv_sa]        */
/*                Message Illegalize Bits for Transmit RT SA [xmt_sa]        */
/*                Message Illegalize Bits for Receive  SA MC [rcv_mc]        */
/*                Message Illegalize Bits for Transmit SA MC [xmt_mc]        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to set/reset the Message Illegalize bits within  */
/*    the BM Activity and Message Filter page for the selected RT.           */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ini_msg_ill(TY_API_DEV *p_api_dev, BYTE rt, L_WORD rcv_sa, L_WORD xmt_sa, L_WORD rcv_mc, L_WORD xmt_mc)
{
    TY_API_BM_ACTIVITY *entry;
    L_WORD mask;
    AiUInt32 sa;

    for(sa = 0; sa < MAX_API_RT_MC_SA; sa++)
    {
        mask = 0x1L << sa;

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
        if( (rcv_sa & mask) == 0 )
            entry->control &= BSWAP16(0xEFFF);
        else
            entry->control |= BSWAP16(0x1000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
        if( (xmt_sa & mask) == 0L)
            entry->control &= BSWAP16(0xEFFF);
        else
            entry->control |= BSWAP16(0x1000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
        if( (rcv_mc & mask) == 0L)
            entry->control &= BSWAP16(0xEFFF);
        else
            entry->control |= BSWAP16(0x1000);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
        if( (xmt_mc & mask) == 0L)
            entry->control &= BSWAP16(0xEFFF);
        else
            entry->control |= BSWAP16(0x1000);
    }

} /* end: api_io_bm_ini_msg_ill */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI_FTW         */
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
/*    Inputs    : Stop Trigger Mask pattern     [htm]                        */
/*                Stop Trigger Compare pattern  [htc]                        */
/*                Start Trigger Mask pattern    [stm]                        */
/*                Start Trigger Compare pattern [stc]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the API1553-DS BM Function Trigger Word. */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ini_ftw(TY_API_DEV *p_api_dev, BYTE con, BYTE htm, BYTE htc, BYTE stm, BYTE stc)
{
  L_WORD tmpl;

  tmpl = 0L;

  /* Monitor Function Trigger Word */
  switch(con)
  {
    default:
    case 0:
       /* all */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(MAKE_B2LWORD(htm, htc, stm, stc) );
      break;
    case 1:
       /* only stc */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw);
      LWORD_RESET_BITS(tmpl, 0, 8);
      LWORD_SET_BITS(tmpl, stc, 0, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(tmpl);
      break;
    case 2:
       /* only stm */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw);
      LWORD_RESET_BITS(tmpl, 8, 8);
      LWORD_SET_BITS(tmpl, stm, 8, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(tmpl);
      break;
    case 3:
       /* only htc */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw);
      LWORD_RESET_BITS(tmpl, 16, 8);
      LWORD_SET_BITS(tmpl, htc, 16, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(tmpl);
      break;
    case 4:
       /* only htm */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw);
      LWORD_RESET_BITS(tmpl, 24, 8);
      LWORD_SET_BITS(tmpl, htm, 24, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mftw = BSWAP32(tmpl);
      break;
  }

} /* end: api_io_bm_ini_ftw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI_TIW         */
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
/*    Inputs    : External Trigger TCB Index   [eti]                         */
/*                Any Error TCB Index          [aei]                         */
/*                Sequence Trigger 0 TCB Index [ti0]                         */
/*                Sequence Trigger 1 TCB Index [ti1]                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the API1553-DS BM Trigger Index Word.    */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ini_tiw(TY_API_DEV *p_api_dev, BYTE con, BYTE eti, BYTE aei, BYTE ti0, BYTE ti1)
{
  L_WORD tmpl;

  tmpl = 0L;

  /* Monitor Trigger Index Word */
  switch(con)
  {
    default:
    case 0:
       /* all */
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = BSWAP32(MAKE_B2LWORD(eti, aei, ti1, ti0) );
      break;
    case 1:
       /* only ti0 */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw);
      LWORD_RESET_BITS(tmpl, 0, 8);
      LWORD_SET_BITS(tmpl, ti0, 0, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = BSWAP32(tmpl);
      break;
    case 2:
       /* only ti1 */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw);
      LWORD_RESET_BITS(tmpl, 8, 8);
      LWORD_SET_BITS(tmpl, ti1, 8, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = BSWAP32(tmpl);
      break;
    case 3:
       /* only aei */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw);
      LWORD_RESET_BITS(tmpl, 16, 8);
      LWORD_SET_BITS(tmpl, aei, 16, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = BSWAP32(tmpl);
      break;
    case 4:
       /* only eti */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw);
      LWORD_RESET_BITS(tmpl, 24, 8);
      LWORD_SET_BITS(tmpl, eti, 24, 8);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mtiw = BSWAP32(tmpl);
      break;
  }


  ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_RD_TIW);

} /* end: api_io_bm_ini_tiw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_RT_ACT_CON      */
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
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                RT Subaddress/Modecode [sa_mc]                             */
/*                Subaddress Type [sa_type]                                  */
/*                Time Tag Mode [tagm]                                       */
/*                Trigger Block Function [tfct]                              */
/*                Trigger Control Block Index [tid]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the specified Trigger Control Index      */
/*    field within the API1553-DS BM Activity and Message Filter page.       */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_rt_act_con(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa_mc, BYTE sa_type, BYTE tagm, BYTE tfct, BYTE tid)
{
    AiUInt16 tmp;
    TY_API_BM_ACTIVITY *entry;
    entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa_mc, sa_type);

    tmp = BSWAP16(entry->control);

    WORD_RESET_BITS(tmp, 8, 2);
    WORD_SET_BITS(tmp, tfct, 8, 2);
    WORD_RESET_BITS(tmp, 0, 8);
    WORD_SET_BITS(tmp, tid, 0, 8);

    entry->control = BSWAP16(tmp);

    /*
    L_WORD tmpl = 0L;
    AiUInt32 *pActDesc;

    GET_ACT_DESC(p_api_dev, rt, sa_mc, sa_type, pActDesc);
    tmpl = BSWAP32( pActDesc[ACT_CTRL] );
    LWORD_RESET_BITS(tmpl, 24, 2);
    LWORD_SET_BITS(tmpl, tfct, 24, 2);
    LWORD_RESET_BITS(tmpl, 16, 8);
    LWORD_SET_BITS(tmpl, tid, 16, 8);

    pActDesc[ACT_CTRL] = BSWAP32(tmpl);
    */

} /* end: api_io_bm_rt_act_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI             */
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
/*    This function is used to initialize the API1553-DS BM.                 */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_ini(TY_API_DEV *p_api_dev)
{
  WORD tcb;
  BYTE rt;

  /* BM specific registers of System Control Block */
  api_io_cb_ini(p_api_dev, INIT_BM);

  /* Global BM message/error counter */
  api_io_bm_glb_cnt_ini(p_api_dev);

  /* Init Monitor Activity and Filter Page */
  for(rt = 0; rt < MAX_API_RT; rt++)
    api_io_bm_rt_act_ini(p_api_dev, rt);

  /* Init Monitor Trigger Control Block Area */
  for(tcb = 0; tcb < MAX_API_TCB; tcb++)
    api_io_bm_tcb_desc_ini(p_api_dev, (BYTE)tcb);

} /* end: api_io_bm_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_IMOD_SET        */
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
/*    Inputs    : Interrupt Mode [imod]                                      */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the API1553-DS BM related Interrupts.    */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_imod_set(TY_API_DEV *p_api_dev, BYTE imod)
{
  L_WORD tmpl;

  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  LWORD_RESET_BITS(tmpl, 18, 2);
  LWORD_SET_BITS(tmpl, imod, 18, 2);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);

} /* end: api_io_bm_imod_set */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_SMOD_SET        */
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
/*    Inputs    : Strobe Mode [smod]                                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the API1553-DS BM related Out Strobes.   */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_smod_set(TY_API_DEV *p_api_dev, BYTE smod)
{
  L_WORD tmpl;

  tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
  LWORD_RESET_BITS(tmpl, 20, 2);
  LWORD_SET_BITS(tmpl, smod, 20, 2);
  (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);

} /* end: api_io_bm_smod_set */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INTR_CON        */
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
/*    Inputs    : Interrupt Mode [imod]                                      */
/*                Strobe Mode [smod]                                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize API1553-DS BM related Interrupts   */
/*    and Output Strobes.                                                    */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_intr_con(TY_API_DEV *p_api_dev, BYTE imod, BYTE smod)
{
  api_io_bm_imod_set(p_api_dev, imod);
  api_io_bm_smod_set(p_api_dev, smod);

} /* end: api_io_bm_intr_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_CAP_CON         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Capture Mode [cmod]                                        */
/*                Trace After Trigger counter [tat]                          */
/*                Message Capture counter [mcc]                              */
/*                Recording File size [fsize]                                */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to initialize the API1553-DS BM Capture mode.    */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_cap_con(TY_API_DEV *p_api_dev, BYTE cmod, L_WORD tat, WORD mcc, WORD fsize)
{
  L_WORD hfi_cnt, tmpl;

  tmpl = 0L;
  hfi_cnt = 0L;

  switch(cmod)
  {
    case API_BM_CMOD_ALL:
       /* Standard Capture mode */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
      LWORD_RESET_BITS(tmpl, 22, 2);
      LWORD_SET_BITS(tmpl, API_SCB_MMOD_STD_CAP, 22, 2);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0L;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = BSWAP32(tat);
      break;

    case API_BM_CMOD_ONLY:
       /* Capture Only mode */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
      LWORD_RESET_BITS(tmpl, 22, 2);
      LWORD_SET_BITS(tmpl, API_SCB_MMOD_SEL_CAP, 22, 2);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = BSWAP32(mcc);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = BSWAP32(tat);
      break;

    case API_BM_CMOD_REC:
       /* Recording mode */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
      LWORD_RESET_BITS(tmpl, 22, 2);
      LWORD_SET_BITS(tmpl, API_SCB_MMOD_STD_CAP, 22, 2);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = BSWAP32(tat);

      /* Init amount of Half Buffer Full interrupts */
      /* required to meet the specified file size */
      /*     hfi_cnt = 1024L * 1024L;
       *     hfi_cnt *= fsize;
       *     hfi_cnt /= (p_api_dev->glb_mem.biu[p_api_dev->biu].size.bm_buf / 2);
       */

      /* Recording */
      p_api_dev->bm_rec[ p_api_dev->biu ].mode = API_ON;
      p_api_dev->bm_rec[ p_api_dev->biu ].hfi_org = hfi_cnt;
      p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt = hfi_cnt;
      p_api_dev->bm_rec[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
      p_api_dev->bm_rec[ p_api_dev->biu ].size = 0L;
      break;

    case API_BM_CMOD_MSG_FLT_REC:
       /* Message Filter Recording mode */
      tmpl = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).scw);
      LWORD_RESET_BITS(tmpl, 22, 2);
      LWORD_SET_BITS(tmpl, API_SCB_MMOD_STD_CAP, 22, 2);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).scw = BSWAP32(tmpl);
      (*p_api_dev->cb_p[ p_api_dev->biu ]).mcc = 0;
      (*p_api_dev->cb_p[ p_api_dev->biu ]).tat = 0;

      /* Recording */
      p_api_dev->bm_rec[ p_api_dev->biu ].mode = API_ON;
      p_api_dev->bm_rec[ p_api_dev->biu ].hfi_org = 0L;
      p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt = 0L;
      p_api_dev->bm_rec[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
      p_api_dev->bm_rec[ p_api_dev->biu ].size = 0L;

      p_api_dev->bm_msg_flt_mode[ p_api_dev->biu ] = API_ON;
      break;

    default:
      break;
  }

} /* end: api_io_bm_cap_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_SWXM_CON        */
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
/*    Inputs    : Status Word Exception Mask [swxm]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to init the  API1553-DS BM Status Word Exception */
/*    Mask.                                                                  */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_swxm_con(TY_API_DEV *p_api_dev, WORD swxm)
{
  /* Monitor Status Word Exception Mask */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).swxm = BSWAP32(MAKE_W2LWORD(0, swxm) );

} /* end: api_io_bm_swxm_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MBFP       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.11.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Monitor Buffer Fill Pointer [mbfp]                         */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to read the current Monitor Buffer Fill Pointer  */
/*    register of the API1553-DS System Control Block.                       */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_bm_read_mbfp(TY_API_DEV *p_api_dev, short dest_biu )
{
  L_WORD mbfp;

  if (p_api_dev->bm_status[dest_biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->mbfp), In);
  }
  
  mbfp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[dest_biu]).mbfp);

  return (mbfp);

} /* end: api_io_bm_read_mbfp */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BM_INI_MSG_FLT_REC */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.99   Update : 01.05.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Amount of Filter Command Words [cnt]                       */
/*                Filter Command Word (MS-Byte first) [cw]                   */
/*                First Data Word position in message 'cw' [pos1]            */
/*                Last Data Word position in message 'cw' [pos2]             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_MSG_FLT_REC' instruction to init */
/*    the BM Message Filter Recording of the API1553-DS BM.                  */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_bm_ini_msg_flt_rec(TY_API_DEV *p_api_dev, BYTE cnt, BYTE *mfrec_p)
{
  WORD i, j, rt, tr, sa;
  WORD tmp_cw, word_cnt;
  L_WORD rcv_sa, xmt_sa, rcv_mc, xmt_mc;
  L_WORD mask;
  short k;
  TY_API_BM_ACTIVITY *entry;

  /* Disable all RT Subaddresses */
  for(rt = 0; rt < MAX_API_RT; rt++)
  {
    api_io_bm_ini_msg_flt(p_api_dev, (BYTE)rt, 0L /*rcv_sa*/, 0L /*xmt_sa*/, 0L /*rcv_mc*/, 0L /*xmt_mc*/);
  }
  /* Clear Message Filter Input Array */
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cnt = 0;
  for(i = 0; i < MAX_API_BM_MSG_FLT_CMD; i++)
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] = 0;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] = 0;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ] = 0;
  }

  if(cnt > 0)
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cnt = cnt;
    for(i = 0, j = 0; i < cnt; i++)
    {

      rcv_sa = 0;
      xmt_sa = 0;
      rcv_mc = 0;
      xmt_mc = 0;

      j = 4 * i;
      tmp_cw = *(mfrec_p + j);
      tmp_cw = tmp_cw << 8;
      tmp_cw += *(mfrec_p + j + 1);
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] = tmp_cw;
      word_cnt = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] &0x001F);
      if(word_cnt == 0)
        word_cnt = 32;
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] = *(mfrec_p + j + 2);
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ] = *(mfrec_p + j + 3);

      if( (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ]) || 
          (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] < 1)        || 
          (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] > word_cnt) || 
          (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ] < 1)        || 
          (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ] > word_cnt) )
      {

        return API_NAK;
      }

      /* Enable RT Subadress */
      rt = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] >> 11) &0x001F;
      tr = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] >> 10) &0x0001;
      sa = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] >> 5) &0x001F;

      /* Enable RT Subaddress */

      if(tr == 0)
      {
         /* Receive Subaddress */

        if(sa == 0 || sa == 31)
        {
           /* Receive Modecode */
          rcv_mc |= (1L << word_cnt);
        }
        else
        {
          rcv_sa |= (1L << sa);
        }
      }

      if(tr == 1)
      {
         /* Transmit Subaddress */

        if(sa == 0 || sa == 31)
        {
           /* Transmit Modecode */
          xmt_mc |= (1L << word_cnt);
        }
        else
        {
          xmt_sa |= (1L << sa);
        }

      }

      for(k = 0; k < MAX_API_RT_MC_SA; k++)
      {
        mask = 0x1L << k;

        if( (rcv_sa & mask) != 0L)
        {
          entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
          entry->control |= BSWAP16(0x2000);
          break;
        }

        if( (xmt_sa & mask) != 0L)
        {
          entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
          entry->control |= BSWAP16(0x2000);
          break;
        }

        if( (rcv_mc & mask) != 0L)
        {
          entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
          entry->control |= BSWAP16(0x2000);
          break;
        }

        if( (xmt_mc & mask) != 0L)
        {
          entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
          entry->control |= BSWAP16(0x2000);
          break;
        }
      }
    } /* end: for */

  }
  else
    return API_NAK;

  return 0;

} /* end: api_io_bm_ini_msg_flt_rec */



#define FW_WAIT_TIMEOUT 10000

short ExecuteFirmwareCommand( TY_API_DEV *p_api_dev, short dest_biu, BYTE uc_FirmwareCmd )
{
  L_WORD ul_Temp;
  L_WORD ul_LoopCount;
  L_WORD ul_CurrentStatusWord;

  /* clear command area of system control word in order to setup new command */
  ul_Temp = BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).scw);
  ul_Temp &= 0x07FFFFFF;
  (*p_api_dev->cb_p[ dest_biu ]).scw = BSWAP32(ul_Temp);
  wbflush();

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->scw),
                      Out);

  /* Wait until No command operation has been processed by BIU */
  ul_LoopCount = 0;
  do
  {
      UsbSynchronizeLWord(p_api_dev, 
                          API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->ssw),
                          In);

      ul_CurrentStatusWord = BSWAP32(p_api_dev->cb_p[dest_biu]->ssw) & 0xF8000000;
      ul_LoopCount++;

      if(ul_CurrentStatusWord != 0)
           WAIT_CLOCKS(10 * p_api_dev->clk_1us);

  }while(ul_CurrentStatusWord != 0 && ul_LoopCount < FW_WAIT_TIMEOUT);
   
  if(ul_LoopCount >= FW_WAIT_TIMEOUT)
  {
#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
      PRINTF1("ExecuteFirmwareCommand : Timeout while waiting for NOP (loop=%d)\r\n", ul_LoopCount );
#endif /*#ifndef _AIM_VME*/
      return (API_ERR);
  }

  /* setup firmware command */
  ul_Temp = BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).scw);
  ul_Temp &= 0x07FFFFFF;
  ul_Temp |= (uc_FirmwareCmd << 27);
  (*p_api_dev->cb_p[ dest_biu ]).scw = BSWAP32(ul_Temp);
  wbflush();

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->scw),
                      Out);

  ul_LoopCount = 0;
  do
  {
      UsbSynchronizeLWord(p_api_dev, 
                          API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->ssw),
                          In);

      ul_CurrentStatusWord = BSWAP32(p_api_dev->cb_p[dest_biu]->ssw) & 0xF8000000;
      ul_LoopCount++;

      if(ul_CurrentStatusWord != (uc_FirmwareCmd << 27))
      {
           WAIT_CLOCKS(10 * p_api_dev->clk_1us);
           if(ul_LoopCount == (FW_WAIT_TIMEOUT/4))
           {
               /* try again to call the firmware command, 
               in case of a interrupt e.g. BC stop interrupt using also an firmware command
               it is possible to overwrite the firmware command and that results in an firmware timeout */
               ul_Temp = BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).scw);
               ul_Temp &= 0x07FFFFFF;
               ul_Temp |= (uc_FirmwareCmd << 27);
               (*p_api_dev->cb_p[ dest_biu ]).scw = BSWAP32(ul_Temp);            
           }
      }

  }while(ul_CurrentStatusWord != (uc_FirmwareCmd << 27) && (ul_LoopCount < FW_WAIT_TIMEOUT));

  if(ul_LoopCount >= FW_WAIT_TIMEOUT)
  {
#if !defined(_AIM_VME) || defined(AVI_IO_TEST)
    PRINTF1("ExecuteFirmwareCommand : Timeout while waiting for CMD (loop=%d)\r\n", ul_LoopCount );
#endif
    return (API_ERR);
  }

  /* clear command area of system control word in order to show command is ready */
  ul_Temp = BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).scw);
  ul_Temp &= 0x07FFFFFF;
  (*p_api_dev->cb_p[ dest_biu ]).scw = BSWAP32(ul_Temp);
  wbflush();

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->scw),
                      Out);

  return (API_OK);
}
#undef FW_WAIT_TIMEOUT


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_MSG_FLT_GET_DATA */
/*                                                                            */
/*   Author : Michael Stahl            Project   : API1553-DS                 */
/*                                                                            */
/*   Source : C                        Tools     : PC/AT; Norton Editor;      */
/*                                                 CYGNUS, GNU-C, As, and LD  */
/*                                                 IDT-C 5.1 Toolkit          */
/*----------------------------------------------------------------------------*/
/*   Create : 01.05.99   Update : 25.07.02                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*    Inputs    : L_WORD address to read Data from [addr]                     */
/*            : Index of Filter Command Word [index]                        */
/*                                                                            */
/*    Outputs   : Instruction acknowledge type [ackfl]                        */
/*                                                                            */
/*    Description :                                                           */
/*    This function is used to extract the Message Data and Status info       */
/*    from the Bus Monitor Memory.                                     */
/*                                                                            */
/******************************************************************************/
#define BM_ENTRY_SIZE 4

static BYTE api_io_bm_msg_flt_get_data(TY_API_DEV* p_api_dev, L_WORD addr, WORD index, BYTE con)
{
    BYTE echo, flag, timeout;
    WORD i, rt, err_w, err_k, stat_b, par_err;
    L_WORD tag;
    L_WORD volatile* bm_ptr;
    int k;
    BYTE uc_NbOfFoundStatusWords = 0;
    BYTE uc_NoEntry = 0;
    const  WORD       ucBiu = p_api_dev->biu;
    const  L_WORD     lBmBase = p_api_dev->glb_mem.biu[ucBiu].base.bm_buf;
    const  L_WORD     lBmSize = p_api_dev->glb_mem.biu[ucBiu].size.bm_buf;


    /* Lookup Table for Error handling */
    static WORD lut[16] =
    {
      0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
    };

    echo = API_ERR;

    /* this is the timetag of the message start */
    if ((addr - BM_ENTRY_SIZE) == p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp) /* there is no entry currently available */ /* in case of no data word received (erroneous transfer) */
    {
        addr = p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp;
        uc_NoEntry = 1;
    }

    if (addr > ((lBmBase + lBmSize) - BM_ENTRY_SIZE))
        addr = lBmBase;

    /* Set pointer to Global Memory address */
    bm_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    /*if(addr != p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_mbfp)*/
    {
        if (con == 3)
            rt = (WORD)((p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry[p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt - 1] & 0xF800) >> 11);
        else
            rt = (WORD)((p_api_dev->bm_msg_flt_dsp[ucBiu].cw[index] & 0xF800) >> 11);

        /* Read Block from Bus Monitor Memory */
        flag = API_OFF;
        timeout = API_OFF;
        for (i = 0; (i < 80) && (flag == API_OFF) && (timeout == API_OFF); i++)
        {
            if (p_api_dev->bm_status[ucBiu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(bm_ptr), In);
            }

            tag = BSWAP32(*bm_ptr);

            if (0 == uc_NoEntry) /* at least one entry is available */
            {
                switch (tag & 0xF0000000)
                {
                case 0x10000000:
                    /* Error */
                    if (con != 1)
                    {
                        err_w = (WORD)BSWAP32(*bm_ptr);
                        err_k = 0;
                        for (k = 14; k >= 0; k--)
                        {
                            if ((err_w & lut[k]) == lut[k])
                            {
                                err_k = (WORD)k;
                                err_k++;
                                break;
                            }
                        }
                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw_errw &= 0x0FFF;
                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw_errw += (err_k << 12);
                    }
                    else
                    {
                        err_w = (WORD)BSWAP32(*bm_ptr);
                        err_w &= 0x8000;
                        err_w |= 0x1000;
                        par_err = (WORD)BSWAP32(*bm_ptr) & 0x0010;
                        par_err = par_err << 7;
                        err_w |= par_err;

                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw_errw |= err_w;
                    }
                    /*PRINTF("found err\n\r", 0, 0, 0);*/
                    break;

                case 0x80000000:
                    /* 1.Command Word on pri. bus */
                case 0xC0000000:
                    /* 1.Command Word on sec. bus */

                    flag = API_ON;
                    uc_NbOfFoundStatusWords = 0;
                    /*PRINTF("found cw1\n\r",0, 0,0);*/
                    break;

                case 0x90000000:
                    /* 2.Command Word on pri. bus */
                case 0xD0000000:
                    /* 2.Command Word on sec. bus */

                    p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry[p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt] = BSWAP32(*bm_ptr);
                    p_api_dev->bm_msg_flt_dsp[ucBiu].cur_cw[1] = BSWAP32(*bm_ptr);
                    p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt++;
                    /*PRINTF("found cw2\n\r",0, 0,0);*/

                    break;

                case 0xA0000000:
                    /* Data Word on pri. bus */
                case 0xE0000000:
                    /* Data Word on sec. bus */

                    if (p_api_dev->bm_msg_flt_dsp[p_api_dev->biu].data_cnt < 32)
                    {
                        p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry[p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt] = BSWAP32(*bm_ptr);
                        p_api_dev->bm_msg_flt_dsp[ucBiu].data[p_api_dev->bm_msg_flt_dsp[ucBiu].data_cnt] = BSWAP32(*bm_ptr);
                        p_api_dev->bm_msg_flt_dsp[ucBiu].data_cnt++;
                        p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt++;
                    }
                    /*PRINTF("found data\n\r",0, 0,0);*/
                    break;


                case 0xB0000000:
                    /* Status Word on pri. bus */
                case 0xF0000000:
                    /* Status Word on sec. bus */

                    if (0 == uc_NbOfFoundStatusWords)
                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw[0] = BSWAP32(*bm_ptr);
                    else
                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw[1] = BSWAP32(*bm_ptr);
                    uc_NbOfFoundStatusWords++;

                    if (con != 1)
                    {
                        if (rt != 31)
                        {
                            p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw_errw |= (WORD)(BSWAP32(*bm_ptr) & 0x000007FFL);
                            p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry[p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt] = BSWAP32(*bm_ptr);
                            p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt++;
                        }
                    }
                    else
                    {
                        p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry[p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt] = BSWAP32(*bm_ptr);
                        p_api_dev->bm_msg_flt_dsp[ucBiu].bm_entry_cnt++;
                        stat_b = 0;
                        stat_b = ((WORD)BSWAP32(*bm_ptr)) & 0x0008;
                        stat_b = stat_b << 1;
                        p_api_dev->bm_msg_flt_dsp[ucBiu].cur_sw_errw |= stat_b;
                    }
                    /*PRINTF("found status\n\r",0, 0,0);*/
                    break;
                default:
                    break;
                }
            }

            while (addr == p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp)
            {
                if (p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp != api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu))
                {
                    /* if data has been received, while standing at the end of the previous data, do not investigate
                     * timeout condition, go on reading and analyzing the data
                     */
                    p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);
                    uc_NoEntry = 0;
                    break;
                }
                else
                {
                    timeout = API_ON;
                    echo = API_OK;
                    p_api_dev->bm_msg_flt_dsp[ucBiu].cur_entry_ptr = addr; /* First Entry for next CW */
                    break;
                }
            }

            if (addr != p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp)
            {
                addr += BM_ENTRY_SIZE;
                bm_ptr++;
            }

            if (addr > (lBmBase + lBmSize - BM_ENTRY_SIZE))
            {
                addr = p_api_dev->glb_mem.biu[ucBiu].base.bm_buf;
                /* Set pointer to Global Memory address */
                bm_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);
            }
        }

        /* Message Validation OK if next Command Word found */
        if (flag == API_ON)
        {
            echo = API_OK;
            p_api_dev->bm_msg_flt_dsp[ucBiu].cur_entry_ptr = addr - 12; /* First Entry for next CW */
        }
    }

    return (echo);
} /* end: api_io_bm_msg_flt_get_data */


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : Input words with bytes swapped                              */
/*                                                                            */
/*    Description :                                                           */
/*    Swap input word.                                                        */
/*                                                                            */
/******************************************************************************/
static WORD swap_word( WORD value )
{
  return ((((value)&0x00FF)<<8)+ (((value)&0xFF00)>>8));
}


static void api_io_bm_msg_flt_clear_update_flag( TY_API_DEV* p_api_dev, L_WORD volatile * api_ptr )
{
  *api_ptr &= BSWAP32(0x0FFFFFFFUL);

  if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(api_ptr), Out);
  }

}


static L_WORD api_io_bm_msg_flt_select_previous( TY_API_DEV *p_api_dev, L_WORD addr )
{
  const  WORD       ucBiu       = p_api_dev->biu;
  const  L_WORD     lBmBase     = p_api_dev->glb_mem.biu[ucBiu].base.bm_buf;
  const  L_WORD     lBmSize     = p_api_dev->glb_mem.biu[ucBiu].size.bm_buf;

  if( addr == lBmBase )
    /* reached bottom => wrap around to top entry */
    return lBmBase + lBmSize - BM_ENTRY_SIZE;
  else
    /* walk back one entry */
    return addr - BM_ENTRY_SIZE;
}


static L_WORD api_io_bm_msg_flt_select_next( TY_API_DEV *p_api_dev, L_WORD addr )
{
  const  WORD       ucBiu       = p_api_dev->biu;
  const  L_WORD     lBmBase     = p_api_dev->glb_mem.biu[ucBiu].base.bm_buf;
  const  L_WORD     lBmSize     = p_api_dev->glb_mem.biu[ucBiu].size.bm_buf;

  addr = addr + BM_ENTRY_SIZE;
  if( addr > (lBmBase + lBmSize - BM_ENTRY_SIZE) )
  {
    /* reached top => wrap around to bottom */
    addr = lBmBase;
  }

  return addr;
}


static void api_io_bm_msg_flt_clear_format( TY_API_DEV * p_api_dev )
{
  BYTE ucDataWordIndex = 0;

  /* This was only done in format 3,4,5 before */
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[0] = 0x0000;
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[1] = 0x0000;
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[1] = 0x0000;

  /* This was done for all formats */

  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw = 0x0000;
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt = 0;

  /* clear data word area */
  for(ucDataWordIndex = 0; ucDataWordIndex < 32; ucDataWordIndex++)
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ ucDataWordIndex ] = 0x0000;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 0 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format0( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size, WORD index )
{
  BYTE    ucDataWordIndex = 0;
  L_WORD  tmsec           = 0;
  L_WORD  tusec           = 0;
  L_WORD  tsec            = 0;
  L_WORD  tmin            = 0;
  L_WORD  tstd            = 0;

  int     iLen            = 0;
  int     iDcnt           = 0;

  iLen   = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ index ] - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + 1;
  iDcnt  = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt      - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ];

  if(iDcnt < 0)
    iLen = 0;
  else if(iLen > (iDcnt + 1) )
    iLen = iDcnt + 1;

  /* Calculate Time for this Message */
  tusec  = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low &0x000FFFFFL );
  tmsec  = tusec / 1000L;
  tusec  = tusec % 1000L;
  tsec   = ( (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low &0x03F00000L) >> 20);
  tmsec += (tsec *1000);
  tmin   = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high &0x0000003FL);
  tmsec += (tmin *60 * 1000);
  tstd   = ((p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high &0x000007C0L) >> 6);
  tmsec += (tstd *60 * 60 * 1000);

  if( (max_size - 6 - iLen) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size )
  {
    /* copy message header */
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 0) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 1) = (WORD) iLen;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 2) = (WORD) (tmsec);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 3) = (WORD) (tmsec >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 4) = (WORD)tusec;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 5) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ index ];

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 6;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size) = (WORD) p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + ucDataWordIndex - 1 ];
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size++;
    }

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_ON;
    return API_ERR;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 1 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format1( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size, WORD index )
{
  BYTE   ucDataWordIndex = 0;
  L_WORD  tmsec           = 0;
  L_WORD  tusec           = 0;
  L_WORD  tsec            = 0;
  L_WORD  tmin            = 0;
  L_WORD  tstd            = 0;

  int     iLen            = 0;
  int     iDcnt           = 0;

  iLen   = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ index ] - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + 1;
  iDcnt  = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt      - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ];

  if(iDcnt < 0)
    iLen = 0;
  else if(iLen > (iDcnt + 1) )
    iLen = iDcnt + 1;

  /* Calculate Time for this Message */
  tusec  = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low &0x000FFFFFL );
  tmsec  = tusec / 1000L;
  tusec  = tusec % 1000L;
  tsec   = ( (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low &0x03F00000L) >> 20);
  tmsec += (tsec *1000);
  tmin   = (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high &0x0000003FL);
  tmsec += (tmin *60 * 1000);
  tstd   = ((p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high &0x000007C0L) >> 6);
  tmsec += (tstd *60 * 60 * 1000);


  if( (max_size - 6-iLen) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)
  {
    /* copy message header */
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 0) = swap_word( p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw );
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 1) = swap_word( (WORD)iLen );
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 2) = swap_word( (WORD) (tmsec) );
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 3) = swap_word( (WORD) (tmsec >> 16) );
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 4) = swap_word( (WORD)tusec );
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 5) = swap_word( p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ index ] );

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 6;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size) = swap_word( (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + ucDataWordIndex - 1 ] );
      
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size++;
    }

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_ON;
    return API_ERR;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 2 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format2( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size, WORD index )
{
  BYTE ucDataWordIndex = 0;

  int     iLen            = 0;
  int     iDcnt           = 0;

  iLen   = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ index ] - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + 1;
  iDcnt  = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt      - p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ];

  if(iDcnt < 0)
    iLen = 0;
  else if(iLen > (iDcnt + 1) )
    iLen = iDcnt + 1;

  if( (max_size - 7-iLen) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)
  {
    /* copy message header */
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 0) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 1) = (WORD) iLen;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 2) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 3) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 4) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 5) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 6) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ index ];

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 7;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size) = (WORD) p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ index ] + ucDataWordIndex - 1 ];
      
      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size++;
    }

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_ON;
    return API_ERR;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 3 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format3( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size )
{
  BYTE       ucDataWordIndex = 0;
  const int  iLen            = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt;

  if( (max_size - 7 - iLen) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)
  {
    /* copy message header */
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 0) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 1) = (WORD) iLen;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 2) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 3) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 4) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 5) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 6) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry[ 0 ]);

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 7;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size) = (WORD)(p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ ucDataWordIndex ]);

      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size++;
    }

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_ON;
    return API_ERR;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 4 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format4( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size )
{
  BYTE       ucDataWordIndex = 0;
  const WORD wBiu            = p_api_dev->biu;
  const int  iLen            = p_api_dev->bm_msg_flt_dsp[wBiu].data_cnt;
  L_WORD     lOutSize        = p_api_dev->bm_msg_flt_dsp[wBiu].out_size;

  if( (max_size - 10 - iLen) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)
  {

    /* copy message header */
    *(dest_ptr + lOutSize + 0) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_sw_errw          );
    *(dest_ptr + lOutSize + 1) = (WORD)( iLen                                                 );
    *(dest_ptr + lOutSize + 2) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_high        );
    *(dest_ptr + lOutSize + 3) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_high  >> 16 );
    *(dest_ptr + lOutSize + 4) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_low         );
    *(dest_ptr + lOutSize + 5) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_low   >> 16 );
    *(dest_ptr + lOutSize + 6) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_cw[ 0 ]          );
    *(dest_ptr + lOutSize + 7) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_cw[ 1 ]          );
    *(dest_ptr + lOutSize + 8) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_sw[ 0 ]          );
    *(dest_ptr + lOutSize + 9) = (WORD)( p_api_dev->bm_msg_flt_dsp[wBiu].cur_sw[ 1 ]          );

    lOutSize += 10;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + lOutSize) = (WORD)(p_api_dev->bm_msg_flt_dsp[wBiu].data[ ucDataWordIndex ]);
      lOutSize++;
    }

    p_api_dev->bm_msg_flt_dsp[wBiu].out_size = lOutSize;

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[wBiu].ovfl = API_ON;
    return API_ERR;
  }
}


/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Martin Haag              Project   : 1553/3910 TSW              */
/*                                                                            */
/*   Source : C                        Tools     :                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*   Create : 12.05.09   Update : 12.05.09                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*                                                                            */
/*    Return    : API_OK    - In case of success                              */
/*                API_ERR   - In case of an overflow                          */
/*                                                                            */
/*    Description :                                                           */
/*    copy format 5 data in api_io_bm_read_msg_flt_rec()                      */
/*                                                                            */
/******************************************************************************/
static BYTE api_io_bm_msg_flt_copy_format5( TY_API_DEV *p_api_dev, WORD volatile * dest_ptr, L_WORD max_size )
{
  BYTE       ucDataWordIndex = 0;
  const int  iLen            = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt;

  if( (max_size - 14-(iLen*2)) > p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)
  {
    /* copy message header */
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 0) = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 1) = iLen*2;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 2) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 3) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 4) = (WORD)p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low;
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 5) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 6) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[ 0 ]);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 7) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[ 0 ] >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 8) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[ 1 ]);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 9) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[ 1 ] >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 10) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[ 0 ]);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 11) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[ 0 ] >> 16);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 12) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[ 1 ]);
    *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size + 13) = (WORD) (p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[ 1 ] >> 16);

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 14;

    /* copy message data */
    for(ucDataWordIndex = 0; ucDataWordIndex < iLen; ucDataWordIndex++ )
    {
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size)   = (WORD)(p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ ucDataWordIndex ]);
      *(dest_ptr + p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size+1) = (WORD)(p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ ucDataWordIndex ] >> 16);

      p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size += 2;
    }

    return API_OK;
  }
  else
  {
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_ON;
    return API_ERR;
  }
}



static BYTE api_io_bm_msg_flt_copy( TY_API_DEV * p_api_dev, L_WORD volatile * api_ptr, L_WORD lLastCmdWord, L_WORD max_size, BYTE con, BYTE bus_flag, BYTE index )
{
  WORD volatile   * dest_ptr  = 0;
  BYTE              retval   = 0;

  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[0] = BSWAP32(*api_ptr);
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry[ p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry_cnt ] = BSWAP32(*api_ptr);
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry_cnt++;

  api_io_bm_msg_flt_clear_format( p_api_dev );

  if(con == 1)
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw = 0x1000;

  /* set bus_flag in status information word */
  p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw |= (bus_flag << 11); /* 10.06.99 */


  dest_ptr = (WORD*)API_REC_SHARED_MEM_START_ADDR; /* Shared RAM */
  /* Get Message Data and Status Information */
  retval = api_io_bm_msg_flt_get_data( p_api_dev, lLastCmdWord + BM_ENTRY_SIZE, index, con );

  if( retval != API_OK )
    /* no message found */
    return API_ON;

  /* message found */
  if(      0 == con )
    retval = api_io_bm_msg_flt_copy_format0( p_api_dev, dest_ptr, max_size, index );
  else if( 1 == con )
    retval = api_io_bm_msg_flt_copy_format1( p_api_dev, dest_ptr, max_size, index );
  else if( 2 == con )
    retval = api_io_bm_msg_flt_copy_format2( p_api_dev, dest_ptr, max_size, index );
  else if( 3 == con )
    retval = api_io_bm_msg_flt_copy_format3( p_api_dev, dest_ptr, max_size );
  else if( 4 == con )
    retval = api_io_bm_msg_flt_copy_format4( p_api_dev, dest_ptr, max_size );
  else  /* 5 == con */
    retval = api_io_bm_msg_flt_copy_format5( p_api_dev, dest_ptr, max_size );

  if( retval == API_ERR )
    /* overflow => Stop BM */
    api_io_mode_con( p_api_dev, API_BM_MODE, API_OFF );


  return API_OFF;
}




static L_WORD api_io_bm_msg_flt_trigger_dependent( TY_API_DEV *p_api_dev, BYTE mode )
{
  L_WORD            chk_addr    = 0;
  BYTE              ucTimeout   = 0;
  L_WORD volatile * api_ptr     = 0;
  const  WORD       ucBiu       = p_api_dev->biu;
  const  L_WORD     lBmBase     = p_api_dev->glb_mem.biu[ucBiu].base.bm_buf;
  const  L_WORD     lBmSize     = p_api_dev->glb_mem.biu[ucBiu].size.bm_buf;

  if(p_api_dev->bm_status[ucBiu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[ucBiu]->msw), In);
      UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[ucBiu]->mstp), In);
  }

  if( mode == 1 )
  {
    /* mode = API_BM_FLT_MODE_DEPENDENT => 'SIZE' depending on CT event */
    if( p_api_dev->bm_msg_flt_dsp[ucBiu].trigd == API_OFF)
    {
      /* Check if Monitor data capturing started */
      if( (BSWAP32( (*p_api_dev->cb_p[ucBiu]).msw) &0x00010000L) == 0x00010000L)
      {
        /* Monitor Status Word */
        /* CT event, Look for previous Timetag */
        p_api_dev->bm_msg_flt_dsp[ucBiu].trigd = API_ON;

        /* Start at Monitor Start Trigger Pointer */
        chk_addr = BSWAP32( (*p_api_dev->cb_p[ucBiu]).mstp);

        for( ucTimeout = 0; ucTimeout < 50; ucTimeout++ )
        {
            if(p_api_dev->bm_status[ucBiu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, chk_addr, In);
            }

          api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(chk_addr);

          if(    ((BSWAP32(*(api_ptr    )) &0xF0000000L) != 0L          )   /* entry is updated                        */
              && ((BSWAP32(*(api_ptr    )) &0xF0000000L) == 0x20000000L )   /* entry is an Time Tag Low Word entry     */
              && ((BSWAP32(*(api_ptr - 1)) &0xF0000000L) == 0x30000000L ) ) /* prev. entry is Time Tag High Word entry */
          {
                chk_addr -= BM_ENTRY_SIZE;
                break;
          }
          chk_addr -= 4;
          if( chk_addr < lBmBase )
              /* wrap around */
              chk_addr = lBmBase + lBmSize - BM_ENTRY_SIZE;
        }

        /* Set old_mbfp to first Time Tag High Word entry */
        p_api_dev->bm_msg_flt_dsp[ucBiu].old_mbfp = chk_addr;

        if( p_api_dev->bm_msg_flt_dsp[ucBiu].old_mbfp == lBmBase )
          chk_addr = lBmBase + lBmSize - BM_ENTRY_SIZE;
        else
          chk_addr = p_api_dev->bm_msg_flt_dsp[ucBiu].old_mbfp - BM_ENTRY_SIZE;

        /* Reset Update flag */
        api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(chk_addr);
        *api_ptr &= BSWAP32(0x0FFFFFFFUL);

        if(p_api_dev->bm_status[ucBiu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, chk_addr, Out);
        }
      }
      else
      {
        /* Not triggerd */
        p_api_dev->bm_msg_flt_dsp[ucBiu].old_mbfp = p_api_dev->bm_msg_flt_dsp[ucBiu].cur_mbfp;
        (*p_api_dev->cb_p[ucBiu]).bm_get = p_api_dev->bm_msg_flt_dsp[ucBiu].old_mbfp;
      }
    } /* end: trigd == API_OFF */
  }
  else
  {
    /* mode = API_BM_FLT_MODE_INDEPENDENT => nothing to do */
  }

  return chk_addr;
}

/******************************************************************************/
/*                                                                            */
/*   Module : API_IO                   Submodule : API_IO_BM_READ_MSG_FLT_REC */
/*                                                                            */
/*   Author : Michael Stahl            Project   : API1553-DS                 */
/*                                                                            */
/*   Source : C                        Tools     : PC/AT; Norton Editor;      */
/*                                                 CYGNUS, GNU-C, As, and LD  */
/*                                                 IDT-C 5.1 Toolkit          */
/*----------------------------------------------------------------------------*/
/*   Create : 01.05.99   Update : 10.06.99                                    */
/*----------------------------------------------------------------------------*/
/*    Descriptions                                                            */
/*    ------------                                                            */
/*    Inputs    : Message Filter Recording Mode [mode]                        */
/*              : Message Filter Recording Format [con]                       */
/*              : Size of allocated buffer in Words [max_size]                */
/*              : Size of allocated messages [max_msg]                        */
/*                                                                            */
/*    Outputs   : Instruction acknowledge type [ackfl]                        */
/*                                                                            */
/*    Description :                                                           */
/*    This function handles the 'API_BM_READ_MSG_FLT_REC' instruction to      */
/*    read the BM Message Filter Recording data of the API1553-DS BM.         */
/*                                                                            */
/******************************************************************************/
BYTE api_io_bm_read_msg_flt_rec(TY_API_DEV* p_api_dev, BYTE mode, BYTE con, L_WORD max_size, L_WORD max_msg)
{
    BYTE              ackfl = API_ACK2;
    BYTE              found = API_OFF;
    BYTE              msg_flag = API_OFF;
    BYTE              bus_flag = 0;
    BYTE              j = 0;
    WORD              data = 0;
    L_WORD            chk_addr = 0;
    L_WORD            addr = 0;
    L_WORD            lNumMonEntries = 0;
    L_WORD            tag = 0;
    L_WORD            lWordType = 0;
    L_WORD            lLastCmdWord = 0;
    L_WORD            lCurMonEntry = 0;
    L_WORD volatile* api_ptr = 0;

    const short       wBiu = p_api_dev->biu;
    const L_WORD      lBmBase = p_api_dev->glb_mem.biu[wBiu].base.bm_buf;
    const L_WORD      lBmSize = p_api_dev->glb_mem.biu[wBiu].size.bm_buf;

    p_api_dev->bm_msg_flt_dsp[wBiu].ovfl = API_OFF;

    if (p_api_dev->bm_status[wBiu] == API_BUSY)
    {
        UsbSynchronizeBMSystemControlBlock(p_api_dev, wBiu, In);
    }

    if (p_api_dev->bm_rec[wBiu].mode != API_ON)
        /* Recording is not active => return */
        return (ackfl);

    /* check if data is available */
    if ((p_api_dev->cb_p[wBiu]->msw & BSWAP32(1 << 16)) == 0)
        /* Monitor did not trigger => return */
        return (ackfl);

    if ((p_api_dev->cb_p[wBiu]->bm_mc == 0)
        && (p_api_dev->cb_p[wBiu]->bm_ec == 0))
        /* Monitor did not receive any messages yet => return */
        return (ackfl);

    /* Get Current Monitor Buffer Fill Pointer */
    p_api_dev->bm_msg_flt_dsp[wBiu].out_size = 0;
    p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);

    api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp);

    /* Handle trigger dependent mode */
    api_io_bm_msg_flt_trigger_dependent(p_api_dev, mode);

    if (p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp == p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp)
    {
        /* No new data available => return */
        return (ackfl);
    }

    /* old_mbfp is the position where we stopped processing during the last call */
    addr = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
    api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

    /* calculate amount of words */
    if (addr < p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp)
    {
        /* direct case */
        lNumMonEntries = p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp - addr;
        lNumMonEntries = lNumMonEntries / BM_ENTRY_SIZE;
    }
    else
    {
        /* wrap around case */
        /* current read pos to top */
        lNumMonEntries = lBmBase + lBmSize - addr;
        /* bottom to monitor fill pointer */
        lNumMonEntries = lNumMonEntries + p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp - lBmBase;

        lNumMonEntries = lNumMonEntries / BM_ENTRY_SIZE;
    }

    if (((*p_api_dev->cb_p[wBiu]).msw >> 18) & 0x1)
    {
        /* BM PUT/GET overflow */
        p_api_dev->bm_msg_flt_dsp[wBiu].ovfl = API_ON;
        return (ackfl);
    }

    /* Read Monitor Block */
    p_api_dev->bm_msg_flt_dsp[wBiu].bm_entry_cnt = 0;

    for (lCurMonEntry = 0; lCurMonEntry < lNumMonEntries; lCurMonEntry++)
    {
        if (p_api_dev->bm_status[wBiu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(api_ptr), In);
        }

        tag = BSWAP32(*api_ptr);
        lWordType = tag & 0xF0000000L;
        found = API_ON;

        if (lWordType == 0x30000000L)     /* Time Tag High entry       */
        {
            p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_high = (tag & 0x000FFFFFL);
        }
        else if (lWordType == 0x20000000L)     /* Time Tag Low entry        */
        {
            p_api_dev->bm_msg_flt_dsp[wBiu].cur_ttag_low = (tag & 0x03FFFFFFL);
        }
        else if ((lWordType == 0x80000000L)    /* 1.Command Word on pri bus */
              || (lWordType == 0xC0000000L))   /* 1.Command Word on sec bus */
        {

            bus_flag = (lWordType == 0x80000000L) ? 1 : 0; /* set bus_flag to 1 for primary bus */

            p_api_dev->bm_msg_flt_dsp[wBiu].bm_entry_cnt = 0;

            lLastCmdWord = addr;

            if ((3 == con) || (4 == con) || (5 == con)) /* without filtering */
            {
                found = api_io_bm_msg_flt_copy(p_api_dev, api_ptr, lLastCmdWord, max_size, con, bus_flag, 0);
            }
            else /* (0 == con) || (1 == con) || (2 == con)      with    filtering */
            {
                /* message filter iteration */
                data = (WORD)BSWAP32(*api_ptr);

                for (j = 0; j < p_api_dev->bm_msg_flt_dsp[wBiu].cnt; j++)
                {
                    if (p_api_dev->bm_msg_flt_dsp[wBiu].cw[j] == data)
                    {
                        found = api_io_bm_msg_flt_copy(p_api_dev, api_ptr, lLastCmdWord, max_size, con, 0, j);
                    }
                }
            }

            if (found == API_OFF)
            {
                /* store current entry pointer as last processed pointer */
                p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp = p_api_dev->bm_msg_flt_dsp[wBiu].cur_entry_ptr;
                (*p_api_dev->cb_p[wBiu]).bm_get = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;

                /* reset update flag of previous entry */
                chk_addr = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
                chk_addr = api_io_bm_msg_flt_select_previous(p_api_dev, chk_addr);
                api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(chk_addr);
                api_io_bm_msg_flt_clear_update_flag(p_api_dev, api_ptr);

                /* reset api_ptr to current -- new */
                chk_addr = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
                api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(chk_addr);
            }

            max_msg--;
            if ((max_msg == 0) || (lCurMonEntry >= lNumMonEntries))
                /* done - we have read enough data */
                msg_flag = API_ON;
        }

        /* jump to next entry */

        addr = api_io_bm_msg_flt_select_next(p_api_dev, addr);
        api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

        if (p_api_dev->bm_msg_flt_dsp[wBiu].ovfl == API_ON)
            /* overflow occured => stop */
            break;

        if (msg_flag == API_ON)
            /* max_msg limit reached => stop */
            break;

    } /* for( lCurMonEntry = 0; lCurMonEntry < lNumMonEntries; lCurMonEntry++) */


    if (addr == p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp)
    {
        /* We catched up with the bus monitor fill pointer.
           We have to check if the last message was incomplete. */
        if (found == API_ON)
        {
            /* message was incomplete => walk back to last timetag high entry */
            if (lLastCmdWord != 0)
            {
                /* we need at least a command word */

                while (1)
                {

                    lLastCmdWord -= BM_ENTRY_SIZE;
                    if (lLastCmdWord < lBmBase)
                        /* wrap around */
                        lLastCmdWord = lBmBase + lBmSize - BM_ENTRY_SIZE;

                    /* Set pointer to Global Memory address */
                    api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(lLastCmdWord);

                    if (p_api_dev->bm_status[wBiu] == API_BUSY)
                    {
                        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(api_ptr), In);
                    }

                    /* Time Tag High entry */
                    if ((BSWAP32(*api_ptr) & 0xF0000000L) == 0x30000000L)
                        break;
                }
                /* store last processed position */
                p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp = lLastCmdWord;
                (*p_api_dev->cb_p[wBiu]).bm_get = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
            }
        }
        else
        {
            /* message was complete => we should be on the next valid entry */
            /* store last processed position */
            p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp = p_api_dev->bm_msg_flt_dsp[wBiu].cur_mbfp;
            (*p_api_dev->cb_p[wBiu]).bm_get = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
        }

        /* walk back one message to reset updated flag */
        chk_addr = p_api_dev->bm_msg_flt_dsp[wBiu].old_mbfp;
        chk_addr = api_io_bm_msg_flt_select_previous(p_api_dev, chk_addr);

        /* reset update flag of previous entry */
        api_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(chk_addr);
        api_io_bm_msg_flt_clear_update_flag(p_api_dev, api_ptr);
    }


    return (ackfl);
} /* end: api_io_bm_read_msg_flt_rec */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                Submodule : API_IO_BM_DYNTAGM_ENTRY_INIT */
/*                                                                           */
/*   Author : Marco Motz            Project   : API1553-DS                   */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*                                              CYGNUS, GNU-C, As, and LD    */
/*                                              IDT-C 5.1 Toolkit            */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_entry_init(TY_API_DEV *p_api_dev, L_WORD tag_ix)
{

#ifdef DEBUG_DYNTAGMON
  if (tag_ix == 0)  
     PRINTF1("api_io_bm_dyntagm_entry_init %d\n",tag_ix);
#endif

  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].con         = API_BM_DYNTAG_MON_DIS;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].rt          = 0;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa          = 0;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa_type     = 0;
   
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].res         = 0;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_type = 0;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_wpos = 0;
   
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt   = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].bad_cnt     = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt    = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dw          = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].state       = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt_contig = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt_contig = 0L;
    
} /* end: api_io_bm_dyntagm_entry_init */   

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO            Submodule : API_IO_BM_DYNTAGM_ENTRY_INIT_CNT */
/*                                                                           */
/*   Author : Marco Motz            Project   : API1553-DS                   */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*                                              CYGNUS, GNU-C, As, and LD    */
/*                                              IDT-C 5.1 Toolkit            */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_entry_init_cnt(TY_API_DEV *p_api_dev, L_WORD tag_ix)
{
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].bad_cnt   = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt  = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dw        = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].state     = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt_contig = 0L;
  p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt_contig = 0L;
    
} /* end: api_io_bm_dyntagm_entry_init_cnt */   

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                Submodule : API_IO_BM_DYNTAGM_LUT_FREE   */
/*                                                                           */
/*   Author : Marco Motz            Project   : API1553-DS                   */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*                                              CYGNUS, GNU-C, As, and LD    */
/*                                              IDT-C 5.1 Toolkit            */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_lut_free(TY_API_DEV *p_api_dev, L_WORD mode, L_WORD tag_ix)
{
L_WORD i,j;

#ifdef DEBUG_DYNTAGMON
   PRINTF1("api_io_bm_dyntagm_lut_free mode=%d\n",mode);
#endif
 
   if (mode == 0) {
     /* Clear entire Dyntag Monitor RT-SA Lookup Table */
     for(i=0; i<MAX_API_RT; i++) {
       for(j=0; j<MAX_API_RT_SA; j++) {
         p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[i].sa[j].cnt = 0L;
         if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[i].sa[j].p != NULL) {
             FREE(p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[i].sa[j].p);  
             p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[i].sa[j].p = NULL;
         }
       }
     }    
   } 
   else if (mode == 1) {
      ;
   }
   
} /* end: api_io_bm_dyntagm_lut_free */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BM_DYNTAGM_ADD       */
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
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_add(TY_API_DEV *p_api_dev, L_WORD rt, L_WORD sa, L_WORD tag_ix)
{
L_WORD j,k,found,cnt;

   if ((rt < MAX_API_RT) && (sa < MAX_API_RT_SA)) {

      if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p == NULL) {
#ifdef DEBUG_DYNTAGMON
         PRINTF0("MALLOCed\n");
#endif         
         /* Allocate Memory and Clear */
         p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p = (L_WORD *) MALLOC(sizeof(L_WORD) * MAX_API_BM_DYNTAG_MON_ID);
         for (k=0;k<MAX_API_BM_DYNTAG_MON_ID;k++) 
             p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[k] = 0L;
      }    

      if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p != NULL) {

         /* Search if Dyntag Monitor Index is already existing in the list */
         cnt = p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt;
         for (j=0,found=0; j<cnt; j++) {
            if ( p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[j] == (tag_ix+1) ) {
               found = 1;
               break;
            } 
         }

         if (found == 0) {
         
            /* Add the Dyntag Monitor Index and increment count */
            if (cnt < MAX_API_BM_DYNTAG_MON_ID) {
               p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[cnt] = tag_ix + 1;
               p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt++;
            }
         }
        
#ifdef DEBUG_DYNTAGMON
         PRINTF1("Dyntag List cnt=%d:", p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt);
         for (j=0; j<p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt; j++) {
            PRINTF1(" %d",p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[j]);
         }
         PRINTF0("\n");
#endif
      }
   }
   
} /* end: api_io_bm_dyntagm_add */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_BM_DYNTAGM_SUB       */
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
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_sub(TY_API_DEV *p_api_dev, L_WORD rt, L_WORD sa, L_WORD tag_ix)
{
L_WORD j,k,found,cnt;
L_WORD tmp_arr[MAX_API_BM_DYNTAG_MON_ID];

   if ((rt < MAX_API_RT) && (sa < MAX_API_RT_SA)) {

      if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p != NULL) {

         for(k=0;k<MAX_API_BM_DYNTAG_MON_ID;k++) tmp_arr[k] = 0L;

         /* Search if Dyntag Monitor Index is already existing in the list */
         cnt = p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt;
         for (j=0,k=0,found=0; j<cnt; j++) {
            if ( p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[j] == (tag_ix+1) ) {
               found = 1;
            } 
            else {
               /* Copy the list (without specified index) */
               tmp_arr[k] = p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[j]; k++;
            }
         }

         if (found == 1) {
            /* Sub the Dyntag Monitor Index and decrement count */
            if (cnt > 0) {
               p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].cnt--;
               for (k=0;k<MAX_API_BM_DYNTAG_MON_ID;k++)
                   p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[sa].p[k] = tmp_arr[k];
            }
         }
      }
   }

} /* end: api_io_bm_dyntagm_sub */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                Submodule : API_IO_BM_DYNTAGM_CON        */
/*                                                                           */
/*   Author : Marco Motz            Project   : API1553-DS                   */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*                                              CYGNUS, GNU-C, As, and LD    */
/*                                              IDT-C 5.1 Toolkit            */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bm_dyntagm_con(TY_API_DEV *p_api_dev, L_WORD tag_ix, L_WORD rt, L_WORD sa, L_WORD sa_type)
{
  L_WORD index = sa + (sa_type * MAX_API_RT_RCV_SA);
  L_WORD tmpl  = 0;
  AiUInt16 tmp;
  TY_API_BM_ACTIVITY *entry;

  if (p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].con == API_BM_DYNTAG_MON_ENA) {

#ifdef DEBUG_DYNTAGMON
    PRINTF3("api_io_bm_dyntagm_con(ENA) id=%d rt=%d sa=%d ",tag_ix+1,rt,sa);
    PRINTF2("tr=%d index=%d\n",sa_type,index);
#endif

    api_io_bm_dyntagm_add(p_api_dev,rt,index,tag_ix);

    if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[index].cnt == 1) {

#ifdef DEBUG_DYNTAGMON
      PRINTF0("ENABLE Interrupt on Received Message\n");
#endif
      
      /* ENABLE Interrupt on Received Message */
      switch(sa_type) {
        case API_RT_SA_TYPE_RCV:
        case API_RT_SA_TYPE_XMT:
            entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, sa_type);
            tmp = BSWAP16(entry->control);
            WORD_RESET_BITS(tmpl, 10, 2);
            WORD_SET_BITS(tmpl, 0x1, 10, 2);
            entry->control = BSWAP16(tmp);
             break;
        default:
             break;
      }
    }
  }
  else
  {

#ifdef DEBUG_DYNTAGMON
    PRINTF3("api_io_bm_dyntagm_con(DIS) id=%d rt=%d sa=%d ",tag_ix+1,rt,sa);
    PRINTF2("tr=%d index=%d\n",sa_type,index);
#endif

    if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[index].cnt != 0) {
       api_io_bm_dyntagm_sub(p_api_dev,rt,index,tag_ix);
    }
    
    if (p_api_dev->bm_ls_dyntagm_lut[ p_api_dev->biu ].rt[rt].sa[index].cnt == 0) {

#ifdef DEBUG_DYNTAGMON
      PRINTF0("DISABLE Interrupt on Received Message\n");
#endif

      /* DISABLE Interrupt on Received Message */
      switch(sa_type) {
        case API_RT_SA_TYPE_RCV:
        case API_RT_SA_TYPE_XMT:
            entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, sa_type);
            tmp = BSWAP16(entry->control);
            WORD_RESET_BITS(tmpl, 10, 2);
            entry->control = BSWAP16(tmp);
            /*
             GET_ACT_DESC(p_api_dev, rt, sa, sa_type, pActDesc);
             tmpl = BSWAP32( pActDesc[ACT_CTRL] );
             LWORD_RESET_BITS(tmpl, 26, 2);
             pActDesc[ACT_CTRL] = BSWAP32(tmpl);
             */
             break;

        default:
             break;
      }
    }
  }

} /* end: api_io_bm_dyntagm_con */

#endif 

/*****************************************************************************/
/** 1760 Protocol Functions                                                 **/
/*****************************************************************************/
#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_INI          */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

void api_io_c1760_ini(TY_API_DEV *p_api_dev, BYTE biu_id, BYTE mode)
{
  BYTE i;

  p_api_dev->c1760_setup[ biu_id ].used = mode;
  for(i = 0; i < MAX_API_C1760_BUF; i++)
  {
    p_api_dev->c1760_setup[ biu_id ].c1760_buf[ i ].buf_id = 0xFFFF;
  }

} /* end: api_io_c1760_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_MODE_USED    */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_c1760_mode_used(TY_API_DEV *p_api_dev, BYTE biu_id)
{
  BYTE st;

  st = API_ERR;

  if(p_api_dev->c1760_setup[ biu_id ].used == API_ON)
    st = API_OK;

  return (st);

} /* end: api_io_c1760_mode_used */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_BUF_GET      */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_c1760_buf_get(TY_API_DEV *p_api_dev, BYTE biu_id, WORD buf_id, BYTE *c1760_ix)
{
  BYTE i, repeat, st;

  st = API_ERR;

  *c1760_ix = 0;

  if(api_io_c1760_mode_used(p_api_dev, biu_id) == API_OK)
  {
    for(i = 0, repeat = 0; (i < MAX_API_C1760_BUF) && (repeat == 0); i++)
    {
      if(p_api_dev->c1760_setup[ biu_id ].c1760_buf[ i ].buf_id == buf_id)
      {
        *c1760_ix = i;
        st = API_OK;
        repeat = 1;
      }
    }
  }

  return (st);

} /* end: api_io_c1760_buf_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_CON          */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_c1760_con(TY_API_DEV *p_api_dev, BYTE biu_id, BYTE mode, WORD buf_id, BYTE c01, BYTE c02[ ])
{
  BYTE st;
  BYTE ix, stx, i;

  st = API_ERR;
  if(buf_id < p_api_dev->glb_mem.sim_buf_count[ biu_id ])
  {
    switch(mode)
    {
      case API_C1760_ENA:
        /* Check for 1760 setup already exist */
        if(api_io_c1760_buf_get(p_api_dev, biu_id, buf_id, &ix) == API_OK)
        {
          /* Check 1760 parameter */
          stx = API_OK;
          if(c01 > MAX_API_C1760_WPOS)
            stx = API_ERR;
          for(i = 0; (i < MAX_API_C1760_C02) && (stx == API_OK); i++)
          {
            if(c02[ i ] > MAX_API_C1760_WPOS)
              stx = API_ERR;
          }
          if(stx == API_OK)
          {
            /* Setup 1760 parameter */
            p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].c01 = c01;
            for(i = 0; i < MAX_API_C1760_C02; i++)
              p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].c02[ i ] = c02[ i ];
            st = API_OK;
          }
        }
        else
        {
          /* Check for new 1760 setup */
          if(api_io_c1760_buf_get(p_api_dev, biu_id, 0xFFFF /*new*/, &ix) == API_OK)
          {
            /* Check 1760 parameter */
            stx = API_OK;
            if(c01 > MAX_API_C1760_WPOS)
              stx = API_ERR;
            for(i = 0; (i < MAX_API_C1760_C02) && (stx == API_OK); i++)
            {
              if(c02[ i ] > MAX_API_C1760_WPOS)
                stx = API_ERR;
            }
            if(stx == API_OK)
            {
              /* Setup 1760 parameter */
              p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].buf_id = buf_id;
              p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].c01 = c01;
              for(i = 0; i < MAX_API_C1760_C02; i++)
                p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].c02[ i ] = c02[ i ];
              st = API_OK;
            }
          }
        }
        break;

      case API_C1760_DIS:
        /* Check for 1760 setup exist */
        if(api_io_c1760_buf_get(p_api_dev, biu_id, buf_id, &ix) == API_OK)
        {
          p_api_dev->c1760_setup[ biu_id ].c1760_buf[ ix ].buf_id = 0xFFFF;
          st = API_OK;
        }
        break;

      default:
        break;
    }
  }

  return (st);

} /* end: api_io_c1760_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_MSC_ROTR     */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

static WORD api_io_c1760_msc_rotr(TY_API_DEV *p_api_dev, WORD wval, BYTE cnt)
{
  BYTE j;
  WORD tmpw;

  tmpw = wval;
  for(j = 0; j < cnt; j++)
  {
    if( (tmpw &0x0001) == 0x0001)
    {
      tmpw = tmpw >> 1;
      tmpw |= 0x8000;
    }
    else
    {
      tmpw = tmpw >> 1;
    }
  }

  return (tmpw);

} /* end: api_io_c1760_msc_rotr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_MSC_ROTL     */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    tbd                                                                    */
/*                                                                           */
/*****************************************************************************/

static WORD api_io_c1760_msc_rotl(TY_API_DEV *p_api_dev, WORD wval, BYTE cnt)
{
  BYTE j;
  WORD tmpw;

  tmpw = wval;
  for(j = 0; j < cnt; j++)
  {
    if( (tmpw &0x8000) == 0x8000)
    {
      tmpw = tmpw << 1;
      tmpw |= 0x0001;
    }
    else
    {
      tmpw = tmpw << 1;
    }
  }

  return (tmpw);

} /* end: api_io_c1760_msc_rotl */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_CALC_C01_FMT */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    Creates and returns 1760 checksum word c01 from 'wdata[]'.             */
/*                                                                           */
/*****************************************************************************/

WORD api_io_c1760_calc_c01_fmt(TY_API_DEV *p_api_dev, WORD wdata[ ], BYTE epos)
{
  BYTE i;
  WORD tmpw;

  tmpw = 0x0000;

  /* Checksum calculation */

  /* Rotate right */
  for(i = 0; (i < epos) && (i < MAX_API_C1760_WPOS); i++)
  {
    wdata[ i ] = api_io_c1760_msc_rotr(p_api_dev, wdata[ i ], i);
  }

  /* Add all */
  for(i = 0; (i < epos) && (i < MAX_API_C1760_WPOS); i++)
    tmpw ^= wdata[ i ];

  /* Rotate left */
  tmpw = api_io_c1760_msc_rotl(p_api_dev, tmpw, epos);

  return (tmpw);

} /* end: api_io_c1760_calc_c01_fmt */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_CALC_C02_FMT */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    Creates and returns 1760 checksum word c02 from 'wdata'.               */
/*                                                                           */
/*****************************************************************************/

static WORD api_io_c1760_calc_c02_fmt(TY_API_DEV *p_api_dev, WORD wdata)
{
  WORD tmpw, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14;

  tmpw = (wdata >> 5);

  c0 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 1) &0x0001) + ( (tmpw >> 2) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 10) &0x0001);

  c1 = ( (tmpw >> 3) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 10) &0x0001);

  c2 = ( (tmpw >> 1) &0x0001) + ( (tmpw >> 2) &0x0001) + ( (tmpw >> 4) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 9) &0x0001) + ( (tmpw >> 10) &0x0001);

  c3 = ( (tmpw >> 1) &0x0001) + ( (tmpw >> 3) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 9) &0x0001);

  c4 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 2) &0x0001) + ( (tmpw >> 4) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 9) &0x0001) + ( (tmpw >> 10) &0x0001);

  c5 = ( (tmpw >> 2) &0x0001) + ( (tmpw >> 3) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 9) &0x0001);

  c6 = ( (tmpw >> 3) &0x0001) + ( (tmpw >> 4) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 10) &0x0001);

  c7 = ( (tmpw >> 1) &0x0001) + ( (tmpw >> 2) &0x0001) + ( (tmpw >> 4) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 10) &0x0001);

  c8 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 1) &0x0001) + ( (tmpw >> 3) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 10) &0x0001);

  c9 = ( (tmpw >> 4) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 10) &0x0001);

  c10 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 1) &0x0001) + ( (tmpw >> 2) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 9) &0x0001) + ( (tmpw >> 10) &0x0001);

  c11 = ( (tmpw >> 3) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 7) &0x0001);

  c12 = ( (tmpw >> 4) &0x0001) + ( (tmpw >> 7) &0x0001) + ( (tmpw >> 8) &0x0001);

  c13 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 5) &0x0001) + ( (tmpw >> 8) &0x0001) + ( (tmpw >> 9) &0x0001);

  c14 = ( (tmpw >> 0) &0x0001) + ( (tmpw >> 1) &0x0001) + ( (tmpw >> 6) &0x0001) + ( (tmpw >> 9) &0x0001) + ( (tmpw >> 10) &0x0001);

  tmpw = 0x0000;
  tmpw = ( (c0 &0x0001) << 1) | ( (c1 &0x0001) << 2) | ( (c2 &0x0001) << 3) | ( (c3 &0x0001) << 4) | ( (c4 &0x0001) << 5) | ( (c5 &0x0001) << 6) | ( (c6 &0x0001) << 7) | ( (c7 &0x0001) << 8) | ( (c8 &0x0001) << 9) | ( (c9 &0x0001) << 10) | ( (c10 &0x0001) << 11) | ( (c11 &0x0001) << 12) | ( (c12 &0x0001) << 13) | ( (c13 &0x0001) << 14) | ( (c14 &0x0001) << 15);

  return (tmpw);

} /* end: api_io_c1760_calc_c02_fmt */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_C1760_BUF_SET      */
/*                                                                           */
/*   Author : Karl Specht              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : tbd                                                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    Creates and returns 1760 checksum word c02 from 'wdata'.               */
/*                                                                           */
/*****************************************************************************/

BYTE api_io_c1760_buf_set(TY_API_DEV *p_api_dev, BYTE biu_id, BYTE c1760_ix, WORD volatile *buf_data)
{
  BYTE st, i;
  WORD tmpw;
  WORD volatile data[ MAX_API_BUF_SIZE / 2 ];

  st = API_ERR;

  UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(buf_data), MAX_API_BUF_SIZE, In);

  /* Copy buffer data */
  for(i = 0; i < (MAX_API_BUF_SIZE / 2); i++)
    data[ i ] = BSWAP16(*(buf_data + i) );
  /* Generate C02 checksum */
  for(i = 0; i < MAX_API_C1760_C02; i++)
  {
    if( (p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c02[ i ] != API_C1760_DIS) && (p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c02[ i ] >= 2) && (p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c02[ i ] <= 32) )
    {
      /* Generate C01 checksum of preceeding word */
      tmpw = api_io_c1760_calc_c02_fmt(p_api_dev, data[ p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c02[ i ] - 2 ]);

      /* Write C02 to buffer */
      *(buf_data + p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c02[ i ] - 1) = BSWAP16(tmpw);
    }
  }

  /* Generate C01 checksum */
  if(p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c01 != API_C1760_DIS)
  {
    /* Copy buffer data */
    for(i = 0; i < (MAX_API_BUF_SIZE / 2); i++)
      data[ i ] = BSWAP16(*(buf_data + i) );

    /* Generate C01 checksum */
    tmpw = api_io_c1760_calc_c01_fmt(p_api_dev, (WORD*)data, (BYTE) (p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c01 - 1) );

    /* Write C01 to buffer */
    *(buf_data + p_api_dev->c1760_setup[ biu_id ].c1760_buf[ c1760_ix ].c01 - 1) = BSWAP16(tmpw);
  }

  UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(buf_data), MAX_API_BUF_SIZE, Out);

  st = API_OK;

  return (st);

} /* end: api_io_c1760_buf_set */
#endif 


/*****************************************************************************/
/** mm: 25.05.00 Firmware Dynamic Data Functions                            **/
/*****************************************************************************/
#ifdef CMPL4SIM
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DYTAG_DEF          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_dytag_def(TY_API_DEV *p_api_dev, BYTE bc_rt, WORD hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4, L_WORD *w1, L_WORD *w2, L_WORD *w3, L_WORD *w4)
{
  L_WORD tmpl, tmpl1, tmpl2, tmpl3, tmpl4;
  L_WORD opc, func, wpos, fdw;
  int w1_used, w2_used, w3_used, w4_used;

  tmpl1 = 0L;
  tmpl2 = 0L;
  tmpl3 = 0L;
  tmpl4 = 0L;

  opc = 0L;
  func = 0L, wpos = 0L;
  fdw = 0L;

  w1_used = 0;
  w2_used = 0;
  w3_used = 0;
  w4_used = 0;

  if(mode == API_LS_DYTAG_EFA_MODE)
  {

    /* Init Dynamic Data Word 1 */
    switch(tag_fct1 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_SAW_16:
      case API_LS_DYTAG_FCT_SAW_8_LOW:
      case API_LS_DYTAG_FCT_SAW_8_HIGH:
        func = (tag_fct1 &0x7F) - 1;
        opc = (tag_fct1 &0x80) >> 7;
        wpos = wpos1 &0x1F;
        fdw = min1;
        if(opc == API_LS_DYTAG_DIRECT_MODIFY)
          fdw = 0L;
        tmpl1 = (opc << 28) + (func << 24) + (wpos << 16) + fdw;
        tmpl1 |= API_LS_DYTAG_EFA_TAG_EQ_TYPE;
        w1_used = 1;
        break;
      default:
        break;
    }

    /* Init Dynamic Data Word 2 */
    switch(tag_fct2 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_SAW_16:
      case API_LS_DYTAG_FCT_SAW_8_LOW:
      case API_LS_DYTAG_FCT_SAW_8_HIGH:
        func = (tag_fct2 &0x7F) - 1;
        opc = (tag_fct2 &0x80) >> 7;
        wpos = wpos2 &0x1F;
        fdw = min2;
        if(opc == API_LS_DYTAG_DIRECT_MODIFY)
          fdw = 0L;
        tmpl2 = (opc << 28) + (func << 24) + (wpos << 16) + fdw;
        tmpl2 |= API_LS_DYTAG_EFA_TAG_EQ_TYPE;
        w2_used = 1;
        break;
      default:
        break;
    }

    /* Init Dynamic Data Word 3 */
    switch(tag_fct3 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_SAW_16:
      case API_LS_DYTAG_FCT_SAW_8_LOW:
      case API_LS_DYTAG_FCT_SAW_8_HIGH:
        func = (tag_fct3 &0x7F) - 1;
        opc = (tag_fct3 &0x80) >> 7;
        wpos = wpos3 &0x1F;
        fdw = min3;
        if(opc == API_LS_DYTAG_DIRECT_MODIFY)
          fdw = 0L;
        tmpl3 = (opc << 28) + (func << 24) + (wpos << 16) + fdw;
        tmpl3 |= API_LS_DYTAG_EFA_TAG_EQ_TYPE;
        w3_used = 1;
        break;
      default:
        break;
    }

    /* Init Dynamic Data Word 4 */
    switch(tag_fct4 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_SAW_16:
      case API_LS_DYTAG_FCT_SAW_8_LOW:
      case API_LS_DYTAG_FCT_SAW_8_HIGH:
        func = (tag_fct4 &0x7F) - 1;
        opc = (tag_fct4 &0x80) >> 7;
        wpos = wpos4 &0x1F;
        fdw = min4;
        if(opc == API_LS_DYTAG_DIRECT_MODIFY)
          fdw = 0L;
        tmpl4 = (opc << 28) + (func << 24) + (wpos << 16) + fdw;
        tmpl4 |= API_LS_DYTAG_EFA_TAG_EQ_TYPE;
        w4_used = 1;
        break;
      default:
        break;
    }

    /* Set Last Bit */
    if( (w1_used == 1) && (w2_used == 1) && (w3_used == 1) && (w4_used == 1) )
      tmpl4 |= API_LS_DYTAG_LAST_BIT_SET;
    else if( (w1_used == 1) && (w2_used == 1) && (w3_used == 1) )
      tmpl3 |= API_LS_DYTAG_LAST_BIT_SET;
    else if( (w1_used == 1) && (w2_used == 1) )
      tmpl2 |= API_LS_DYTAG_LAST_BIT_SET;
    else if(w1_used == 1)
      tmpl1 |= API_LS_DYTAG_LAST_BIT_SET;
    else
    {
      tmpl1 = 0L;
      tmpl2 = 0L;
      tmpl3 = 0L;
      tmpl4 = 0L;

      /* Disable Event Queue processing */
      if(bc_rt == API_BC_MODE)
      {
        tmpl = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl);
        LWORD_RESET_BITS(tmpl, 10, 2);
        (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl = BSWAP32(tmpl);
      }
      else
      {
        tmpl = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl);
        LWORD_RESET_BITS(tmpl, 10, 2);
        (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl = BSWAP32(tmpl);
      }
    }
  }
  else if(mode == API_LS_DYTAG_STD_MODE)
  {

    /* Init Dynamic Data Word 1 */
    switch(tag_fct1 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_POS_RAMP:
      case API_LS_DYTAG_FCT_NEG_RAMP:
      case API_LS_DYTAG_FCT_POS_TRIANGLE:
      case API_LS_DYTAG_FCT_NEG_TRIANGLE:
        wpos = wpos1 &0x1F;
        opc = tag_fct1 &0x7F;
        tmpl1 = (opc << 27) + (wpos << 16) + step1;
        tmpl2 = max1;
        tmpl2 <<= 16;
        tmpl2 += min1;
        break;
      case API_LS_DYTAG_FCT_XMT_WORD:
        wpos = wpos1 &0x1F;
        opc = tag_fct1 &0x7F;
        tmpl1 = (opc << 27) + (wpos << 16);
        tmpl2 = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]));
        tmpl2 += ( (min1 &0x07FFL) *MAX_API_BUF_SIZE);
        tmpl2 += ( (max1 &0x001FL) *2L);
        tmpl2 &= 0x03FFFFFEL;
        break;
      case API_LS_DYTAG_FCT_SYNC_COUNTER:
        wpos = wpos1 &0x1F;
        opc = tag_fct1 &0x7F;
        tmpl1 = (opc << 27) + (wpos << 16);
		break;
      default:
        break;
    }

    /* Init Dynamic Data Word 2 */
    switch(tag_fct2 &0x7F)
    {
      case API_LS_DYTAG_FCT_DISABLE:
        break;
      case API_LS_DYTAG_FCT_POS_RAMP:
      case API_LS_DYTAG_FCT_NEG_RAMP:
      case API_LS_DYTAG_FCT_POS_TRIANGLE:
      case API_LS_DYTAG_FCT_NEG_TRIANGLE:
        wpos = wpos2 &0x1F;
        opc = tag_fct2 &0x7F;
        tmpl3 = (opc << 27) + (wpos << 16) + step2;
        tmpl4 = max2;
        tmpl4 <<= 16;
        tmpl4 += min2;
        break;
      case API_LS_DYTAG_FCT_XMT_WORD:
        wpos = wpos2 &0x1F;
        opc = tag_fct2 &0x7F;
        tmpl3 = (opc << 27) + (wpos << 16);
        tmpl4 = BSWAP32(ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]));
        tmpl4 += ( (min2 &0x07FFL) *MAX_API_BUF_SIZE);
        tmpl4 += ( (max2 &0x001FL) *2L);
        tmpl4 &= 0x03FFFFFEL;
        break;
      case API_LS_DYTAG_FCT_SYNC_COUNTER:
        wpos = wpos2 &0x1F;
        opc = tag_fct2 &0x7F;
        tmpl3 = (opc << 27) + (wpos << 16);
		break;

      default:
        break;
    }
  }

  *w1 = tmpl1;
  *w2 = tmpl2;
  *w3 = tmpl3;
  *w4 = tmpl4;

} /* end: api_io_dytag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_RT_DYTAG_DEF       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_rt_dytag_def(TY_API_DEV *p_api_dev, WORD rt_hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4)
{
  L_WORD w1, w2, w3, w4;

  api_io_dytag_def(p_api_dev, API_RT_MODE, rt_hid, mode, tag_fct1, min1, max1, step1, wpos1, tag_fct2, min2, max2, step2, wpos2, tag_fct3, min3, max3, step3, wpos3, tag_fct4, min4, max4, step4, wpos4,  &w1, &w2, &w3, &w4);

  /* Write RT Event Queue Descriptor */
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ rt_hid ].con1 = BSWAP32(w1);
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ rt_hid ].addr1 = BSWAP32(w2);
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ rt_hid ].con2 = BSWAP32(w3);
  (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ rt_hid ].addr2 = BSWAP32(w4);

  p_api_dev->rt_dytag_status[ p_api_dev->biu ][ rt_hid ] = 0xFFFFFFFF; /* enable all bits for all 32 RTs */
} /* end: api_io_rt_dytag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_BC_DYTAG_DEF       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_bc_dytag_def(TY_API_DEV *p_api_dev, WORD bc_hid, WORD mode, WORD tag_fct1, WORD min1, WORD max1, WORD step1, WORD wpos1, WORD tag_fct2, WORD min2, WORD max2, WORD step2, WORD wpos2, WORD tag_fct3, WORD min3, WORD max3, WORD step3, WORD wpos3, WORD tag_fct4, WORD min4, WORD max4, WORD step4, WORD wpos4)
{
  L_WORD w1, w2, w3, w4;

  api_io_dytag_def(p_api_dev, API_BC_MODE, bc_hid, mode, tag_fct1, min1, max1, step1, wpos1, tag_fct2, min2, max2, step2, wpos2, tag_fct3, min3, max3, step3, wpos3, tag_fct4, min4, max4, step4, wpos4,  &w1, &w2, &w3, &w4);

  /* Write BC Event Queue Descriptor */
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ bc_hid ].con1 = BSWAP32(w1);
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ bc_hid ].addr1 = BSWAP32(w2);
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ bc_hid ].con2 = BSWAP32(w3);
  (*p_api_dev->bc_eq_desc_p[ p_api_dev->biu ])[ bc_hid ].addr2 = BSWAP32(w4);

  p_api_dev->bc_dytag_status[ p_api_dev->biu ][ bc_hid ] = API_ON;
} /* end: api_io_bc_dytag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_LS_SYSTAG_ENTRY_INIT */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_ls_systag_entry_init(TY_API_DEV *p_api_dev, BYTE tag_ix)
{

  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mode = API_SYSTAG_DIS;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].run_mode = API_SYSTAG_RUN_RESUME;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bc_rt = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].xid_rtsa = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].fct = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = 0;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].init = API_OFF;
  p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].index = 0;

} /* end: api_io_ls_systag_entry_init */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_LS_SYSTAG_DIS      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_ls_systag_dis(TY_API_DEV *p_api_dev, BYTE mode, WORD par1, WORD par2)
{
  short i;
  WORD rt_addr, sub_addr, xid;

  switch(mode)
  {
    case API_LS_SYSTAG_RESET_ALL:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
        api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
      break;

    case API_LS_SYSTAG_RESET_BC_ALL:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
      {
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].bc_rt == API_BC_MODE)
          api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
      }
      break;

    case API_LS_SYSTAG_RESET_RT_ALL:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
      {
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].bc_rt == API_RT_MODE)
          api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
      }
      break;

    case API_LS_SYSTAG_RESET_RT_ONLY:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
      {
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].bc_rt == API_RT_MODE)
        {
          rt_addr = (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].xid_rtsa >> 8) &0x1F;
          if(par1 == rt_addr)
            api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
        }
      }
      break;

    case API_LS_SYSTAG_RESET_SA_ONLY:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
      {
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].bc_rt == API_RT_MODE)
        {
          rt_addr = (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].xid_rtsa >> 8) &0x1F;
          sub_addr = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].xid_rtsa &0x1F;
          if( (par1 == rt_addr) && (par2 == sub_addr) )
            api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
        }
      }
      break;


    case API_LS_SYSTAG_RESET_XID_ONLY:
      for(i = 0; i < MAX_API_SYS_LS_SYSTAGS; i++)
      {
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].bc_rt == API_BC_MODE)
        {
          xid = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ i ].xid_rtsa;
          if(par1 == xid)
            api_io_ls_systag_entry_init(p_api_dev, (BYTE)i);
        }
      }
      break;

    default:
      break;
  }

} /* end: api_io_ls_systag_dis */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                 Submodule : API_IO_LS_SYSTAG_INIT_DW    */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_ls_systag_init_dw(TY_API_DEV *p_api_dev, BYTE dytag_i, BYTE bc_rt_mode, WORD xid_rtsa, L_WORD dbp)
{
WORD dataset_id, states_cnt, compl_no, chksum, chksum_no;
WORD dw, bufw[32], min, max, wpos, bpos, mask, j, k;
WORD volatile *src_p;
WORD volatile *dbp_p;
BYTE tag_ix;

  if(dytag_i != 0)
  {
    tag_ix = dytag_i - 1;
    min = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min;
    max = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max;
    mask = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask;
    wpos = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos;
    bpos = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos;

    /* Init Data Buffer Word */
    if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].init == API_ON)
    {
      switch(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].fct)
      {
        case API_SYSTAG_TRIANG_INC:
        case API_SYSTAG_RAMP_INC:
          dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);
          dw = BSWAP16(*dbp_p) &~mask; /* read value from buffer */
          dw |= ( (min << bpos) &mask); /* create initial value */
          *dbp_p = BSWAP16(dw);
          break;

        case API_SYSTAG_TRIANG_DEC:
        case API_SYSTAG_RAMP_DEC:
          dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);
          dw = BSWAP16(*dbp_p) &~mask; /* read value from buffer */
          dw |= ( (max << bpos) &mask); /* create initial value */
          *dbp_p = BSWAP16(dw);
          break;

        case API_SYSTAG_STATES:
          dataset_id = min; /* Dataset Id */
          states_cnt = max; /* Amount of States */

          /* Set pointer to Dataset Memory address */
          src_p = (WORD*)API_DATASET_MEM_ADDR_ABS(dataset_id);

          /* Set pointer to Global Memory address */
          dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);

          /* Copy from Source to Destination position */
          dw = BSWAP16(*dbp_p) & ~mask; /* read value from buffer */
          dw |= ( (BSWAP16(*src_p) << bpos) & mask); /* create initial value */
          *dbp_p = BSWAP16(dw);

          if(states_cnt > 1)
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].index++;
          break;

        default:
          break;
      }
    }


    /* Look for all Complements and correct init value */
    for(j = 0; j < MAX_API_SYS_LS_SYSTAGS; j++)
    {
      if( (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].bc_rt == bc_rt_mode) &&
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].fct == API_SYSTAG_COMP) && 
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].xid_rtsa == xid_rtsa) && 
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].init == API_ON) )
      {

        compl_no = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].step; /* Complement */
        wpos     = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].wpos; /* Destination */
        min      = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].min;  /* Source */

        /* Destination Pointer */
        dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);

        /* Source Pointer */
        src_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * min);

        /* Read physical Value */
        dw = BSWAP16(*src_p);

        /* Calculate complement */
        switch(compl_no)
        {
          case 1:
            /* 1's complement */
            *dbp_p = BSWAP16(~dw);
            break;
          case 2:
            /* 2's complement */
            *dbp_p = BSWAP16(~dw + 1);
            break;
          default:
            break;
        }
      }
    } /* end for */

    /* Look for all Checksums and correct init value */
    for(j=0; j<MAX_API_SYS_LS_SYSTAGS; j++) {
      if( (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[j].bc_rt == bc_rt_mode) &&
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[j].fct == API_SYSTAG_CHECKSUM) && 
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[j].xid_rtsa == xid_rtsa) && 
          (p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[j].init == API_ON) ) {

        chksum_no = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].step; /* Checksum calculation function (Plus or Xor) */
        wpos      = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].wpos; /* Destination */
        min       = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].min; /* Source (Start) */
        max       = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ j ].max; /* Source (End) */

        if (max >= min) {
    
          chksum = 0;
          
          /* Destination Pointer */
          dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);

          /* Source Pointer */
          src_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * min);
          
          /* Calculate Checksum */
          switch(chksum_no) {
            case 0:
              
              /* PLUS */
              do {
     
                /* Read physical Value */
                dw = BSWAP16(*src_p);
            
                /* Build Checksum */
                chksum += dw;
            
                /* next word */
                src_p++;
                min++;

              } while(max >= min);

              /* Init Checksum word */
              *dbp_p = BSWAP16(chksum);
              break;
        
            case 1:
              
              /* XOR */
              do {
     
                /* Read physical Value */
                dw = BSWAP16(*src_p);
            
                /* Build Checksum */
                chksum ^= dw;
            
                /* next word */
                src_p++;
                min++;
            
              } while(max >= min);

              /* Init Checksum word */
              *dbp_p = BSWAP16(chksum);
              break;
              
            case 2: 
            
              /* 1760 */
          
              /* Copy buffer data */
              for(k=0; (k<wpos) && (k<32); k++) {
                 bufw[k] = BSWAP16(*src_p);
                 /* next word */
                 src_p++;
              }

              /* Generate C01 checksum */
              chksum = api_io_c1760_calc_c01_fmt(p_api_dev, (WORD*)bufw, (BYTE)wpos);

              /* Init Checksum word */
              *dbp_p = BSWAP16(chksum);
              break;  
  
            default:
              break;
          }
        }
      }  
    } /* end for */
  } /* end if */

} /* end: api_io_ls_systag_init_dw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO               Submodule : API_IO_LS_SYSTAG_INIT_DATASET */
/*                                                                           */
/*   Author : Marco Motz             Project   : API1553-DS                  */
/*                                                                           */
/*   Source : C                      Tools     : PC/AT; Norton Editor;       */
/*                                               CYGNUS, GNU-C, As, and LD   */
/*                                               IDT-C 5.1 Toolkit           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_ls_systag_init_dataset(TY_API_DEV *p_api_dev, BYTE dytag_i, L_WORD dbp)
{
  WORD dataset_id, j;
  WORD volatile *dset_p;
  WORD volatile *dbp_p;
  BYTE tag_ix;

  if(dytag_i != 0)
  {
    tag_ix = dytag_i - 1;

    /* Dataset Start Id */
    p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min;

    /* Write first dataset buffer to transmitt buffer */
    dataset_id = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos;

    /* Set pointer to Global Memory */
    dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp);

    /* Set pointer to Dataset Memory */
    dset_p = (WORD*)API_DATASET_MEM_ADDR_ABS(dataset_id);

    /* Copy first Dataset to Buffer */
    for(j = 0; j < MAX_API_BUF_SIZE / 2; j++)
      *(dbp_p + j) = *(dset_p + j);

    if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max > 1)
      p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos++;
  }

} /*  end: api_io_ls_systag_init_dataset */



/******************************************************************************/
/*                                                                            */
/*    Descriptions                                                            */
/*    ------------                                                             */
/*    Inputs    :                                                             */
/*                                                                            */
/*    Outputs   : none                                                        */
/*                                                                            */
/*    Description :   Sets the special operation mode (firmware command 0x18) */
/*                                                                            */
/******************************************************************************/


/* supported with firmware V02.36 and later */
void api_io_set_special_op_mode( TY_API_DEV *p_api_dev, AiUInt32 mode)
{
  AiUInt32 stat;
  stat = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw);

  if( mode == 1)
  {
      if( (stat & API_SSW_SCM_BIT) == API_SSW_SCM_BIT)
        return; /* abort, as the mode is already enabled*/

      (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = BSWAP32(0x00000001);    /* special global counter mode (BC, RT counter counts with any transfer) */
      ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_SET_SOM);  /* API_SCB_COM_SET_SOM: "set special operation mode" */
  }

  if( mode == 0 )
  {
      if( (stat & API_SSW_SCM_BIT) == 0UL)
        return; /* abort, as the mode is already disabled*/

      (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = BSWAP32(0x00000000);    /* clear special global counter mode bit */
      ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_SET_SOM);  /* API_SCB_COM_SET_SOM: "set special operation mode" */
  }
}



#endif

NAMESPACE1553_END

