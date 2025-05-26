/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_exec_sys.c
    Exec sys commands
*/

#include "mil_tsw_includes.h"



NAMESPACE1553_START



TY_MIL_COM_TABLE_ENTRY axTargetSysExecCommands[] = {
  {MIL_COM_SYS_EXEC,             (TY_MIL_COM_FUNC_PTR)api_sys_exec,             "api_sys_exec",             sizeof(TY_MIL_COM_SYS_EXEC_INPUT),             sizeof(TY_MIL_COM_ACK)        }
};



void api_sys_exec_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysExecCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysExecCommands;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_EXEC_SYS_MODE13   */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 16.04.07   Update : 16.04.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : System Command control [con]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_EXEC_SYS' instruction 13                */
/*                                                                           */
/*****************************************************************************/
static BYTE api_sys_exec_sys_bypass_ir_handling(TY_API_DEV *p_api_dev, BYTE con)
{
    /* Was only implemented for AYI. No longer used*/
    return API_ACK;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_EXEC_SYS          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 16.04.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : System Command mode [mode]                                 */
/*                System Command control [con]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_EXEC_SYS' instruction to execute a      */
/*    system related command on the API1553 target.                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_exec( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_EXEC_INPUT * in, TY_MIL_COM_ACK * out )
{
  BYTE biu;
  L_WORD ret_val = 0;
#ifndef _AMC1553  
  L_WORD tmpl;
#endif  
  BYTE uc_TmpBiu;
  BYTE uc_TmpHsStream;
  BYTE mode = (BYTE)in->mode;
  BYTE con  = (BYTE)in->con;

  switch(mode)
  {
    case 1:
      /* Enable/Disable IRIG from external source */
      mil_hw_irig_set_external(p_api_dev, con);
      break;

    case 2:
       /* 0=UART Trg Connect, 1=Simulator Trigger Out */
      biu = (BYTE)p_api_dev->biu;
      p_api_dev->biu = 0;
      if( (con &0x01) == 0)
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car &= BSWAP32(0xFFFF7FFFL);
      else
        (*p_api_dev->cb_p[ p_api_dev->biu ]).car |= BSWAP32(0x00008000L);
      api_io_mode_con(p_api_dev, API_CAL_MODE, API_ON);
      p_api_dev->biu = biu;
      break;

    case 3:
       /* Perform ASP Interrupt */
      biu = (BYTE)p_api_dev->biu;
      p_api_dev->biu = 0;
      api_io_mode_con(p_api_dev, API_AINTR_MODE, API_ON);
      p_api_dev->biu = biu;
      break;

    case 4:
       /* Boot LCA */
      ret_val = API_ERR_PLATTFORM_NOT_SUPPORTED;
      break;

    case 5:
       /* Boot BIU1 */
       ret_val = API_ERR_PLATTFORM_NOT_SUPPORTED;
      break;

    case 6:
       /* Test ISR */
#if defined(_NUCLEUS)  /* do only for onboard-SW */
      api_ir(px_api_dev,-1);
#else 
      ret_val = API_ERR_FUNCTION_NOT_SUPPORTED;
#endif /* _AMC1553 */
      break;

    case 7:
      ret_val = API_ERR_FUNCTION_NOT_SUPPORTED;
      break;

    case 8:
       /* Set High Speed Subaddress other than 26 */
      if( (con >= 1) && (con < 31) )
        uc_HsSubAddress = con;
      if(con == 0) /* if set to 0, the board is acting like a pure 1553 board */
        uc_HsSubAddress = 0xFF;
      break;

    case 9:
       /* FODDL */
      uc_TmpBiu      = (BYTE)p_api_dev->biu;
      uc_TmpHsStream = (BYTE)p_api_dev->hs_stream;
      p_api_dev->biu = con / 2;
      if( (BIU_2-1) == p_api_dev->biu )
        p_api_dev->biu = BIU_1-1;
      if( (BIU_4-1) == p_api_dev->biu )
        p_api_dev->biu = BIU_3-1;
      if( (BIU_1-1) == p_api_dev->biu )
        p_api_dev->hs_stream = API39_HS_BIU_1;
      if( (BIU_3-1) == p_api_dev->biu )
        p_api_dev->hs_stream = API39_HS_BIU_2;
      if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        ret_val = api39_sys_foddl(p_api_dev, (con % 2));
      p_api_dev->biu = uc_TmpBiu;
      p_api_dev->hs_stream = uc_TmpHsStream;
      break;

#ifndef _AMC1553
    case 10:
       /* reset Target SW*/
      /* ACKnowledge */
      api_opr_ack(p_api_dev, BIU_SYS /*dest*/, API_ACK, p_api_dev->ack_p);

      /* Target Communication Control Register */
      /* Clear Execution Bit */
      tmpl = p_api_dev->targ_com_reg_p->tg_control;
      wbflush();
      tmpl &= API_TG_COM_CONTROL_EXE_BIT_RES;
      p_api_dev->targ_com_reg_p->tg_control = tmpl;
      wbflush();
      /* Target Communication Status Register */
      /* Set Completed Bit, clear Execution Bit */
      tmpl = 0L;
      tmpl |= API_TG_COM_STATUS_CPL_BIT_SET;
      tmpl &= API_TG_COM_STATUS_EXE_BIT_RES;
      p_api_dev->targ_com_reg_p->tg_status = tmpl;
      wbflush();
      api1553_gen_int2host(p_api_dev, GT_ICAUSE_CMD__INDEX); /* generate Interrupt at end of communication. */

      WAIT_CLOCKS(1000*p_api_dev->clk_1us * 100); /* wait some time to allow everyone to read ACK data */

  #ifndef _NUCLEUS
      disable_int(~ALL_INTS);
      Jump(0xBFC00000);
  #else
      restart_mon();
  #endif 
      break;

    case 11:
        ret_val = API_ERR_INVALID_MODE; /* No longer supported with BSP V12 */
      break;

    case 12:
      ret_val = API_ERR_INVALID_MODE; /* No longer supported with BSP V12 */
      break;
#endif 

    case 13:
      /* Bypass the ASP Interrupt handling */
      api_sys_exec_sys_bypass_ir_handling(p_api_dev, con);
      break;

    case 14:
      break;
      
    case 16:
      p_api_dev->uc_BmIniClearBuffer = con;
      break;
      

    default:
       /* future api_sys_exec_sys functions */
      PRINTF0("\n\rapi_sys_exec_sys: function not implemented !\n\r");
      break;
  }

  return ret_val;

} /* end: api_sys_exec_sys */


NAMESPACE1553_END


/*** end of file ***/

