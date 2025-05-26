/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_opr.c
  1553 operation entry module. This module implements the interface to
  the 1553 ASP Driver Software.                   
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START

/* Local Prototypes */
BYTE api_opr_ls(TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p);
BYTE api_opr_hs(TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p);


/*****************************************************************************/
/*                                                                           */
/*   Module : API_OPR                  Submodule : API_OPR_ACK               */
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
/*    Inputs    : Destination BIU/SYS [dest]                                 */
/*                Acknowledge type [ack]                                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge information [ack_p]                */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to generate formated acknowledge information */
/*    for API instruction acknowledgement.                                   */
/*                                                                           */
/*****************************************************************************/

void api_opr_ack(TY_API_DEV *p_api_dev, BYTE dest, BYTE ack, BYTE *ack_p)
{
  BYTE b1, b2, b3, b4 = 0;
  BYTE *ack_data_p;
  BYTE *ack_size_p;
  BYTE *lack_b_ptr;
  WORD *lack_w_ptr;
  L_WORD *lack_lw_ptr;
  WORD count;
  short i;

  ack_data_p = ack_p;
  ack_size_p = ack_p + 2;
  lack_b_ptr = p_api_dev->ack_b_ptr;
  lack_w_ptr = p_api_dev->ack_w_ptr;
  lack_lw_ptr = p_api_dev->ack_lw_ptr;

  /* Init Acknowledge Header */
  *(ack_data_p + 0) = dest;
  *(ack_data_p + 1) = 0;
  *(ack_data_p + 2) = 0;
  *(ack_data_p + 3) = 0;


  /* Each global error is only reported once */
  if( p_api_dev->ulGlobalError != 0 )
  {
      /* Overwrite ACK with global error state */
      ack = API_NAK;
      SET_NAK_INFO(p_api_dev->ulGlobalError);

      p_api_dev->ulGlobalError = 0;
  }


  /* Decode Acknowledge */
  switch(ack)
  {
    case API_ACK:
      *(ack_data_p + 1) = API_TG_ACK;
      break;
    case API_ACK2:
      *(ack_data_p + 1) = API_TG_ACK2;
      count = 4;
      ack_data_p += count;
      /* Write Bytes */
      for(i = 0; i < p_api_dev->b_length; i++)
      {
        *ack_data_p =  *lack_b_ptr;
        ack_data_p++;
        count++;
        lack_b_ptr++;
      }
      /* Write Words */
      for(i = 0; i < p_api_dev->w_length; i++)
      {
        b1 = (BYTE) (*lack_w_ptr);
        b2 = (BYTE) (*lack_w_ptr >> 8);
        *ack_data_p = b2;
        ack_data_p++;
        *ack_data_p = b1;
        ack_data_p++;
        count += 2;
        lack_w_ptr++;
      }
      /* Write LongWords */
      for(i = 0; i < p_api_dev->lw_length; i++)
      {
        b1 = (BYTE) (*lack_lw_ptr);
        b2 = (BYTE) (*lack_lw_ptr >> 8);
        b3 = (BYTE) (*lack_lw_ptr >> 16);
        b4 = (BYTE) (*lack_lw_ptr >> 24);
        *ack_data_p = b4;
        ack_data_p++;
        *ack_data_p = b3;
        ack_data_p++;
        *ack_data_p = b2;
        ack_data_p++;
        *ack_data_p = b1;
        ack_data_p++;
        count += 4;
        lack_lw_ptr++;
      }
      /* Write size info */
      *(ack_size_p + 0) = (BYTE) (count >> 8);
      *(ack_size_p + 1) = (BYTE) (count);
      break;

    default:
      PRINTF1("%x -> default: API_NAK\r\n", ack);
      SET_NAK_INFO(API_ERR_NAK);
      /* no break, continue with API_NAK*/
      AI_TSW_FALLTHROUGH;
    case API_NAK:
      *(ack_data_p + 1) = API_TG_NAK;
      ack_data_p += 4;

      /* we use the nak_info as return value in the library
       * this is why it must be set in case of an error     
       */

      if( *p_api_dev->lword_data_arr == 0 )
        SET_NAK_INFO(API_ERR_NAK);

      PRINTF1("NAK info: %08x\r\n", *lack_lw_ptr);
      PRINTF0("the highest 8 bit usually show a general error\r\n");
      PRINTF0("the lowest 8 bit usually show a low speed function specific error\r\n");
      PRINTF0("the bit 8 to 16 usually show a high speed function specific error\r\n");
      /* Write one LongWord */
      b1 = (BYTE) (*lack_lw_ptr);
      b2 = (BYTE) (*lack_lw_ptr >> 8);
      b3 = (BYTE) (*lack_lw_ptr >> 16);
      b4 = (BYTE) (*lack_lw_ptr >> 24);
      *ack_data_p = b4;      ack_data_p++;
      *ack_data_p = b3;      ack_data_p++;
      *ack_data_p = b2;      ack_data_p++;
      *ack_data_p = b1;      ack_data_p++;
      break;
  }

} /* end: api_opr_ack */





static void api_opr_struct_get_table( L_WORD command, TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *table = NULL;
    *size  = 0;

    switch( command & MIL_COM_GROUP_MASK )
    {
        case MIL_COM_SYS_OFFSET:
            api_sys_command_table( table, size );
            break;
        case MIL_COM_SYS_CAL_OFFSET:
            api_ls_sys_cal_command_table( table, size );
            break;
        case MIL_COM_SYS_TRACK_OFFSET:
            api_ls_sys_command_table( table, size );
            break;
        case MIL_COM_SYS_UTIL_OFFSET:
            api_sys_util_command_table( table, size );
            break;
        case MIL_COM_SYS_VERSIONS_OFFSET:
            api_sys_versions_command_table( table, size );
            break;
        case MIL_COM_SYS_FIFO_OFFSET:
            api_sys_fifo_command_table( table, size );
            break;
        case MIL_COM_SYS_EXEC_OFFSET:
            api_sys_exec_command_table( table, size );
            break;
        case MIL_COM_SYS_BITE:
            api_sys_bite_command_table( table, size );
            break;
        case MIL_COM_SYS_INI:
            api_sys_ini_command_table( table, size );
            break;
        case MIL_COM_BC_LS_OFFSET:
            api_ls_bc_command_table( table, size );
            break;
        case MIL_COM_RT_LS_OFFSET:
            api_ls_rt_command_table( table, size );
            break;
        case MIL_COM_BM_LS_OFFSET:
            api_ls_bm_command_table( table, size );
            break;
        case MIL_COM_BM_LS_DQUEUE_OFFSET:
            api_ls_bm_dataqueue_command_table( table, size );
            break;
        case MIL_COM_BUF_LS_OFFSET:
            api_ls_buf_command_table( table, size );
            break;
        case MIL_COM_SCOPE_OFFSET:
            api_scope_command_table( table, size );
            break;
        case MIL_COM_REP_OFFSET:
            api_rep_command_table( table, size );
            break;
#ifdef _API3910
        case MIL_COM_SYS_HS_OFFSET:
            api_hs_sys_command_table( table, size );
            break;
        case MIL_COM_SYS_HS_TRACK_OFFSET:
            api_hs_sys_track_command_table( table, size );
            break;
        case MIL_COM_SYS_HS_CAL_OFFSET:
            api_hs_sys_cal_command_table( table, size );
            break;
        case MIL_COM_BC_HS_OFFSET:
            api_hs_bc_command_table( table, size );
            break;
        case MIL_COM_RT_HS_OFFSET:
            api_hs_rt_command_table( table, size );
            break;
        case MIL_COM_BM_HS_OFFSET:
            api_hs_bm_command_table( table, size );
            break;
        case MIL_COM_BC_EF_OFFSET:
            api_ef_bc_command_table( table, size );
            break;
        case MIL_COM_RT_EF_OFFSET:
            PRINTF1("Subtable for MIL_COM_EF_RT_OFFSET not yet implemented (cmd=%08X)\r\n", command );
            break;
        case MIL_COM_BM_EF_OFFSET:
            PRINTF1("Subtable for MIL_COM_EF_BM_OFFSET not yet implemented (cmd=%08X)\r\n", command );
            break;
        case MIL_COM_REP_HS_OFFSET:
            api_hs_rep_command_table( table, size );
            break;
        case MIL_COM_BUF_HS_OFFSET:
            api_hs_buf_command_table( table, size );
            break;
#endif
        case MIL_COM_CUSTOM_SYSTEM_OFFSET:
            PRINTF1("Subtable for MIL_COM_CUSTOM_SYSTEM_OFFSET not implemented (cmd=%08X)\r\n", command );
            break;
        case MIL_COM_CUSTOM_DTS_OFFSET:
            api_custom_dts_command_table( table, size );
            break;
        default:
            PRINTF1("No valid subtable found for cmd=%08X\r\n", command );
            break;
    }
}

static void api_opr_struct( TY_API_DEV * p_api_dev, TY_MIL_COM * cmd_p, TY_MIL_COM_ACK * ack_p )
{
  TY_MIL_COM_TABLE_ENTRY * axTargetCommands = NULL;
  TY_MIL_COM_FUNC_PTR      Function         = NULL;
  L_WORD                   ulTableEntry     = 0;
  L_WORD                   ulTableSize      = 0;
  L_WORD                   ulIsHsCommand    = 0;


  /* -- init acknowlege information -- */
  ack_p->ulMagic       = MIL_COM_MAGIC;
  ack_p->ulError       = 0;
  ack_p->ulCommand     = cmd_p->ulCommand;
  ack_p->ulSize        = sizeof(TY_MIL_COM_ACK);
  ack_p->ulSwapControl = 0;



  do
  {
    /* -- get table for command group --- */  
    api_opr_struct_get_table( cmd_p->ulCommand, &axTargetCommands, &ulTableSize );

    if( axTargetCommands == NULL )
    {
        ack_p->ulError = API_ERR_CMD_NOT_FOUND;
        break;
    }
    
    /* -- get function by array index --- */  

    ulTableEntry = cmd_p->ulCommand & MIL_COM_CMD_MASK;

    if( ulTableEntry > ulTableSize  )
    {
      ack_p->ulError = API_ERR_CMD_NOT_FOUND;
      break;
    }

    if( axTargetCommands[ulTableEntry].ulFuncCode != cmd_p->ulCommand )
    {
      ack_p->ulError = API_ERR_CMD_NOT_FOUND;
      break;
    }

    Function = axTargetCommands[ulTableEntry].pFunc;

    /* -- check and execute command --- */  

    if( Function == 0 )
    {
      ack_p->ulError = API_ERR_CMD_NOT_FOUND;
      break;
    }


    /* -- check if command is hs or ls command --- */

    if(cmd_p->ulCommand & MIL_COM_GROUP_HS_MASK)
    {
      ulIsHsCommand = 1;
    }

    
    /* -- convert stream to biu and hs_stream --- */

    if( cmd_p->ulStream > 0 )
      mil_hw_convert_stream_to_LogicalBiu( p_api_dev, cmd_p->ulStream - 1, ulIsHsCommand, &p_api_dev->biu, &p_api_dev->hs_stream );
    else
      mil_hw_convert_stream_to_LogicalBiu( p_api_dev, 0,                   ulIsHsCommand, &p_api_dev->biu, &p_api_dev->hs_stream );
    
    
    /* -- function found debug print it --- */
    
    if( p_api_dev->ul_DbgLevel & DBG_FCT_CALL )
    {
      PRINTF5("calling cmd=%s(%08X), dest=%d (biu=%d, hs_stream=%d)\r\n", 
                      axTargetCommands[ulTableEntry].pszFuncName,
                      cmd_p->ulCommand, 
                      cmd_p->ulStream, 
                      p_api_dev->biu, 
                      p_api_dev->hs_stream );
    }
   
    
    /* -- check provided command size --- */

    if( cmd_p->ulSize < axTargetCommands[ulTableEntry].ulMinimumSize )
    {
      ack_p->ulError = API_ERR_WRONG_CMD_SIZE;
      break;
    }

    /* -- check expected acknowlege size --- */

    if(    ( axTargetCommands[ulTableEntry].ulMaximumAckSize > 0                        )
        && ( axTargetCommands[ulTableEntry].ulMaximumAckSize > cmd_p->ulExpectedAckSize ) )
    {
      ack_p->ulError = API_ERR_WRONG_ACK_SIZE;
      break;
    }


    /* -- Init default ack size for this command -- */
    /* If the command has a variable ack size ulMaximumAckSize=0 this field needs to be set in the function */
    ack_p->ulSize = axTargetCommands[ulTableEntry].ulMaximumAckSize;

    /* -- finally execute function --- */
    ack_p->ulError = Function( p_api_dev, cmd_p, ack_p );

    /* -- acknowledge size handling for error cases -- */
    if (ack_p->ulError != 0)
    {
        ack_p->ulSize = sizeof(TY_MIL_COM_ACK);
    }

    /* -- check if function did set a valid ack size -- */
    if (ack_p->ulSize == 0)
    {
        ack_p->ulSize = sizeof(TY_MIL_COM_ACK);
        ack_p->ulError = API_ERR_ACK_SIZE_UNDEFINED;
    }


  }while(0);

  /* -- Check for global error state --- */

  if( p_api_dev->ulGlobalError != 0 )
  {
      /* Overwrite ACK with global error state */
      ack_p->ulError = p_api_dev->ulGlobalError;
      ack_p->ulSize  = sizeof(TY_MIL_COM_ACK);

      /* each global error is reported only once */
      p_api_dev->ulGlobalError = 0;
  }


  /* -- print warning in case of an error --- */  
  if( p_api_dev->ul_DbgLevel & DBG_FCT_CALL)
  {
	  if( ack_p->ulError != 0 )
	  {
		  PRINTF6("calling cmd=%s(%08X), dest=%d (biu=%d, hs_stream=%d) failed with 0x%08X\r\n", 
                      (Function) ? (axTargetCommands[ulTableEntry].pszFuncName) : "unknown",
                      cmd_p->ulCommand, 
                      cmd_p->ulStream, 
                      p_api_dev->biu, 
                      p_api_dev->hs_stream,
                      ack_p->ulError );
	  }
  }



} /* end: api_opr_struct */





/*! \brief Execute host to target command in TSW */
/*!
    Entry function for the API1553-DS related Driver operations.
    This function will distribute the the API1553/3910-DS instruction
    to the Low Speed or High Speed operation routines.
    After execution of the instruction, acknowledge information is
    generated. The following groups of instruction are decoded
    
    \callgraph
   
    \param  p_api_dev      TY_API_DEV*             The global variable container.
    \param  cmd_p          BYTE *                  Instruction information
    \param  ack_p          BYTE *                  Instruction acknowledge information
*/
void api_opr(TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p)
{
  BYTE ackfl, dest;
  L_WORD ul_cmd;
  L_WORD  ulMagic;

  ackfl = API_NAK;

  ulMagic = MAKE_B2LWORD(*(cmd_p + 0), *(cmd_p + 1), *(cmd_p + 2), *(cmd_p + 3) ); /* Instruction Code */

  if( ulMagic == MIL_COM_MAGIC )
  {
    /* New host to target communication */
    api_opr_struct( p_api_dev, (TY_MIL_COM*)cmd_p, (TY_MIL_COM_ACK*)ack_p );
    return;
  }


  /* old host to target communication */

  *cmd_p &=  0x1F; /* Remove module from first byte and keep biu  */

  dest =  *cmd_p;
  ul_cmd = MAKE_B2LWORD(*(cmd_p + 1), *(cmd_p + 2), *(cmd_p + 3), *(cmd_p + 4) ); /* Instruction Code */

  /* reset NAK information */
  p_api_dev->lword_data_arr[0] = 0;
  p_api_dev->ack_lw_ptr = p_api_dev->lword_data_arr;

  if( p_api_dev->ul_DbgLevel & DBG_FCT_CALL)
  {
    if( 0x00000036 != ul_cmd )
      PRINTF2("calling cmd=%08X, dest=%d\r\n", ul_cmd, dest);
  }
  if( ( (ul_cmd >= API_LS_CMD_DEST_LL) && (ul_cmd <= API_GEN_IO_OUT_CHN_READ) ) || (ul_cmd == API_READ_SW_VERS_EX) )
  {
    ackfl = api_opr_ls(p_api_dev, cmd_p, ack_p);
  }
  else if( (ul_cmd >= API3910_HS_CMD_DEST_LL) && (ul_cmd <= API3910_HS_CMD_DEST_UL) )
  {
    ackfl = api_opr_hs(p_api_dev, cmd_p, ack_p);
  }
  else
  {
    PRINTF1("Unsupported Command  0x%x \n\r", ul_cmd);
    SET_NAK_INFO(API_ERR_CMD_NOT_FOUND);
  }

  /* ACKnowledge */
  api_opr_ack(p_api_dev, dest, ackfl, ack_p);

} /* end: api_opr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_OPR                  Submodule : API_OPR_LS                */
/*                                                                           */
/*   Author : Marco Motz /RK           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.05.09                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Instruction information [cmd_p]                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge information [ack_p]                */
/*                                                                           */
/*    Description :                                                          */
/*    Entry function for the API1553-DS related Driver operations.           */
/*    This function is utilized to decode the API1553-DS instruction         */
/*    information and to activate the appropriate handler routines.          */
/*    After execution of the instruction, acknowledge information is         */
/*    generated. The following groups of instruction are decoded :           */
/*                                                                           */
/*    - API1553-DS Initialization                                            */
/*    - API1553-DS Selftest                                                  */
/*    - API1553-DS System Control                                            */
/*    - API1553-DS Buffer Functions                                          */
/*    - API1553-DS Scenario Functions                                        */
/*    - API1553-DS Bus Controller Functions                                  */
/*    - API1553-DS Remote Terminal Functions                                 */
/*    - API1553-DS Bus Monitor Functions                                     */
/*    - API1553-DS Replay Functions                                          */
/*    - API1553-DS MIL-Bus Control and Calibration Functions                 */
/*    - API1553-DS MIL-Bus Protocol Functions                                */
/*    - API1553-DS Message Filter Recording Functions 						 */
/*                                                                           */
/*****************************************************************************/

BYTE api_opr_ls(TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p)
{
  BYTE dest, ackfl, flag;
  L_WORD cmd;
  BYTE *info_p;
  WORD size;
  ackfl = API_NAK;

  dest =  *cmd_p; /* Destination */
  cmd = MAKE_B2LWORD(*(cmd_p + 1), *(cmd_p + 2), *(cmd_p + 3), *(cmd_p + 4) ); /* Instruction Code */
  size = MAKE_WORD(*(cmd_p + 5), *(cmd_p + 6) ); /* Instruction Size */ 
  info_p = cmd_p + 7; /* Instruction Data */

  p_api_dev->hs_stream = API39_HS_BIU_1;

  /* Decode BIU */
  switch(dest)
  {
    case BIU_SYS:
      p_api_dev->biu = 0;
      flag = API_OK;
      break;
    case BIU_1:
    case BIU_2:
    case BIU_3:
    case BIU_4:
    case BIU_5:
    case BIU_6:
    case BIU_7:
    case BIU_8:
      if( (API_TYPE_1553 == p_api_dev->auc_StreamType[ dest - 1]) ||
          (cmd == API_READ_SW_VERS) ||
          (cmd == API_READ_SW_VERS_EX) )
      {
        p_api_dev->biu = dest - 1;
        
        if( (p_api_dev->biu+1) > (short)p_api_dev->ul_NbOfStreams1 )
          p_api_dev->hs_stream = API39_HS_BIU_2;
        else
          p_api_dev->hs_stream = API39_HS_BIU_1;

        flag = API_OK;
        if( (cmd != API_RESET) && (cmd != API_READ_SW_VERS) && (cmd != API_READ_SW_VERS_EX) )
        {
          if(p_api_dev->biu_active[ p_api_dev->biu ] != API_OK)
          {
            flag = API_ERR;
            SET_NAK_INFO(API_ERR_BIU_NOT_ACTIVE);
          }
        }
      }
      else
      {
        flag = API_ERR;
        SET_NAK_INFO(API_ERR_LS_CMD_ON_HS_BIU);
        PRINTF1("Input Biu not OK (biu=%d)\n\r", dest-1);
      }
      break;
    default:
      p_api_dev->biu = 0; /* undefined */
      flag = API_ERR;
      SET_NAK_INFO(API_ERR_BIU_OUT_OF_RANGE);
      break;
  }

  /* Test on Error */
  if(flag == API_OK)
  {
    /* Decode Instructions */
    switch(cmd)
    {
      /* Initialize Command */
      case API_INI:
        if( dest != BIU_SYS )
        {
          SET_NAK_INFO(API_ERR_BIU_OUT_OF_RANGE);
        }
        else if( size < 8 )
        {
          SET_NAK_INFO(API_ERR_WRONG_CMD_SIZE);
        }
        else
        {
#ifdef _CMPL4S1I1553
          ackfl = s1i_ini(*info_p); /* used for INI command */
          if(ackfl == API_NAK)
            break;
#endif 
          ackfl = api_x_ini(p_api_dev,  *info_p);
        }

        break;

        /*** System Commands - BIU ***/
      case API_RESET:
        /* Set the single function globals back to default */
          if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
          {
             p_api_dev->drv_exe[ p_api_dev->biu ]       = 0;
             p_api_dev->drv_rt_status[ p_api_dev->biu ] = 0;
          }
          ackfl = api_x_sys(p_api_dev, cmd, size, info_p);
        break;

      case API_READ_SW_VERS:
      case API_READ_SW_VERS_EX:
#ifdef _CMPL4S1I1553
          ackfl = s1i_sys(cmd, size, info_p); /* used for RESET command */
          if(ackfl == API_NAK)
          {
            break;
          }
#endif 
          ackfl = api_x_sys(p_api_dev, cmd, size, info_p);
        break;

        /*** System Commands - SYS ***/
      case API_LOAD_SREC:
      case API_PROG_FLASH:
      case API_SYS_GET_MEM_PARTITION:
      case API_SYS_UTIL:
#ifdef _CMPL4DTS
      case API_IP_MODULE_ADDR_INIT: 
      case API_IP_OUTPUT_CHN_WRITE: 
      case API_IP_OUTPUT_CHN_READ:  
      case API_IP_INPUT_CHN_READ:   
      case API_IP_STASK_CTRL:

      case API_GEN_IO_GET_BOARDS:
      case API_GEN_IO_ADDR_INIT:
      case API_GEN_IO_OUT_CHN_WRITE:
      case API_GEN_IO_IN_CHN_READ:
      case API_GEN_IO_STASK_CTRL:
      case API_GEN_IO_READ:
      case API_GEN_IO_OUT_CHN_READ:
#endif
        if(dest == BIU_SYS)
          ackfl = api_x_sys(p_api_dev, cmd, size, info_p);
        else
          SET_NAK_INFO(API_ERR_BIU_OUT_OF_RANGE);
        break;

        /*** Buffer Commands */
      case API_RAM_WRITE:
      case API_RAM_READ:
      case API_RAM_LWORD_WR:
      case API_RAM_WORD_WR:
      case API_RAM_BYTE_WR:
      case API_RAM_LWORD_RD:
      case API_RAM_WORD_RD:
      case API_RAM_BYTE_RD:
        if(dest == BIU_SYS)
          ackfl = api_x_buf(p_api_dev, cmd, size, info_p);
        else
          SET_NAK_INFO(API_ERR_BIU_OUT_OF_RANGE);
        break;

      /*** Scope Commands ***/
      case API_SCOPE_SETUP:
      case API_SCOPE_TRG_DEF:
      case API_SCOPE_CALIBRATE:
        ackfl = mil_scope_cmd(p_api_dev, cmd, size, info_p);
        break;

      default:
        break;
    }
  }

  return (ackfl);

} /* end: api_opr_ls */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_OPR                  Submodule : API_OPR_HS                */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Instruction information [cmd_p]                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge information [ack_p]                */
/*                                                                           */
/*    Description :                                                          */
/*    Entry function for the API3910 related Driver operations.            	 */
/*    This function is utilized to decode the API3910 instruction         	 */
/*    information and to activate the appropriate handler routines.          */
/*    After execution of the instruction, acknowledge information is         */
/*    generated. The following groups of instruction are decoded :           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

BYTE api_opr_hs(TY_API_DEV *p_api_dev, BYTE *cmd_p, BYTE *ack_p)
{
  BYTE dest, ackfl;
  BYTE flag = API_OK;
  BYTE *info_p;
  WORD size;
  L_WORD cmd;

  ackfl = API_NAK;

  dest =  *cmd_p; /* Destination */
  cmd = MAKE_B2LWORD(*(cmd_p + 1), *(cmd_p + 2), *(cmd_p + 3), *(cmd_p + 4) ); /* Instruction Code */
  size = MAKE_WORD(*(cmd_p + 5), *(cmd_p + 6) ); /* Instruction Size */
  info_p = cmd_p + 7; /* Instruction Data */

  p_api_dev->hs_stream = API39_HS_BIU_1;

  /* Decode BIU */
  switch(dest)
  {
    case BIU_SYS:
      p_api_dev->biu = 0;
      break;
    case BIU_2:
    case BIU_3:
    case BIU_4:
    case BIU_6:
      if( API_TYPE_3910 == p_api_dev->auc_StreamType[ dest - 1] )
      {
        p_api_dev->biu = dest - 1;
        
        if( (p_api_dev->biu+1) > (short)p_api_dev->ul_NbOfStreams1 )
          p_api_dev->hs_stream = API39_HS_BIU_2;
        else
          p_api_dev->hs_stream = API39_HS_BIU_1;
      }
      else
      {
        flag = API_ERR;
        SET_NAK_INFO(API_ERR_HS_CMD_ON_LS_BIU);
      }
      break;
    default:
      p_api_dev->biu = 1; /* HS biu  */
      flag = API_ERR;
      SET_NAK_INFO(API_ERR_BIU_OUT_OF_RANGE);
      PRINTF1("HS Input Biu not OK (biu=%d)\n\r", dest-1);
      break;
  }

  /* Test on Error */
  if(flag == API_OK)
  {
    /* Decode Instructions */
    switch(cmd)
    {
      case API39_GPIO_RESET:
      case API39_GPIO_BROADC_CMD:
      case API39_GPIO_BROADC_WRITE:
      case API39_GPIO_CMD:
      case API39_GPIO_STATUS:
      case API39_GPIO_READ:
      case API39_GPIO_WRITE:
      case API39_GPIO_READ_BLOCK:
      case API39_GPIO_WRITE_BLOCK:
        ackfl = api_gpio(p_api_dev, cmd, size, info_p);
        break;

      default:
        break;
    }
  }

  return (ackfl);

} /* end: api_opr_hs */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_OPR                  Submodule : API_OPR_SF                */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 13.05.02   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Command selected (cmd)                                     */
/*                Command Information (info_p)                               */
/*                                                                           */
/*    Return    : 0: Command Exectued                                        */
/*                1: Command Not Executed                                    */
/*                                                                           */
/*    Description :                                                          */
/*    This function is called for single function mode modules and will      */
/*    allow only one mode of operation to execute at a time.  This         	 */
/*    will allow Monitor Mode to run concurrently with Mailbox mode.         */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
L_WORD api_opr_sf(TY_API_DEV *p_api_dev, L_WORD cmd, BYTE rt, BYTE mode)
{
  L_WORD sfpFlag = 0;

  /* These global parameters have been defined to keep track of which
   * modes are executing and will also keep track of how each RT is configured.
   * p_api_dev->drv_exe - Indicates which mode are currently executing
   *    BIT 0:  Set indicates Bus Controller is running
   *    BIT 1:  Set indicates Bus Monitor is running (No longer used always 0)
   *    BIT 2:  Set indicates RT running in emulation mode
   *    Bit 3:  Set indicates RT running in Mailbox mode (No longer used always 0)
   * p_api_dev->drv_rt_status - Indicates which RT's are Initialized for emulation mode
   *    BIT 0:  Set indicates RT0 Initialized in Emulation Mode
   *    BIT 1:  Set indicates RT0 Initialized in Emulation Mode
   *     ...................................................
   *     ...................................................
   *     ...................................................
   *    BIT 31: Set indicates RT31 Initialized in Emulation Mode
   */

  switch(cmd)
  {
    case MIL_COM_LS_BC_START:
      if(p_api_dev->drv_exe[ p_api_dev->biu ] &0xE)
      {
          sfpFlag = API_ERR_BC_NOT_AVAILABLE; /* Return error if RT or BM running */
      }
      else
      {
        /* Set Bus Controller Bit */
        p_api_dev->drv_exe[ p_api_dev->biu ] = 1;
      }
      break;
    case MIL_COM_LS_BC_HALT:
      /* Clear Bus Controller Bit */
      p_api_dev->drv_exe[ p_api_dev->biu ] &= 0xE;
      break;
    case MIL_COM_LS_RT_INI:
      if(mode == 1)
      /* IF RT is Set for emulation */
      {
        /* Set the bit for the specific RT */
        p_api_dev->drv_rt_status[ p_api_dev->biu ] |= (1 << rt);
      }
      else
      {
        /* Clear bit for specific */
        p_api_dev->drv_rt_status[ p_api_dev->biu ] &= ~(1 << rt);
      }
      break;
    case MIL_COM_LS_RT_START:
      if(p_api_dev->drv_rt_status[ p_api_dev->biu ])
      /* If RT Configured for Emulation */
      {
        if(p_api_dev->drv_exe[ p_api_dev->biu ] &0x3)
        {
          /* Return error if Bus Controller or Monitor currently running */
          sfpFlag = API_ERR_INVALID_DEVICE_STATE;
        }
        else
        {
          /* Set Remote Terminal Emulation Bit */
          p_api_dev->drv_exe[ p_api_dev->biu ] |= 0x4;
        }
      }

      break;
    case MIL_COM_LS_RT_HALT:
      /* Clear both the Remote Terminal Mailbox and Emulation Bits */
      p_api_dev->drv_exe[ p_api_dev->biu ] &= 0x3;
      break;
  }

  return sfpFlag;
}

NAMESPACE1553_END

/* end: api_opr.c */

