/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#include "mil_tsw_includes.h"

#include "mil_utils.h"

#include "Ai_def.h"

/*#include "i39_defv.h"
 * include causes errors -> Workarround: */
extern BYTE uc_HsSubAddress;


NAMESPACE1553_START


/* to satisfy GNU */
L_WORD efex_switch( TY_API_DEV *p_api_dev, BYTE Board);






TY_MIL_COM_TABLE_ENTRY axTargetSysUtilCommands[] = {
  {MIL_COM_SYS_UTIL,         (TY_MIL_COM_FUNC_PTR)api_sys_util,             "api_sys_util",             sizeof(TY_MIL_COM_SYS_UTIL_INPUT),         0   },
};



void api_sys_util_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysUtilCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysUtilCommands;
}


static L_WORD api_sys_util_ef_time_out_def( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_UTIL_INPUT * in, TY_MIL_COM_SYS_UTIL_OUTPUT * out )
{
  L_WORD      ulReturnValue = 0;

#ifndef _AMC1553
  L_WORD      ulSysStatReg  = 0;
  L_WORD      ulSysCtrlReg  = 0;
#endif

  BYTE        ucEtoof       = 0;
  BYTE        ucEfexTo      = 0;

  out->data[0] = 0;

  /* -- check input parameters and read values --- */

  if( in->count < 3 )
  {
    /* didn't get enough data to read the parameters */
    PRINTF1("in->count=%d\r\n", in->count );
    return API_ERR_WRONG_CMD_SIZE;
  }

  ucEtoof  = (BYTE)in->data[1];
  ucEfexTo = (BYTE)in->data[2];
  
  /* -- check hardware compatibility --- */

  if( ul_StreamIsEFEX( p_api_dev, p_api_dev->biu ) != 1 )
  {
    /* command is only valid for efex */
    ulReturnValue = 1;
    out->data[0] = ulReturnValue;
    return 0;
  }


#ifndef _AMC1553

  /* -- check if BIU is enabled and report an error if so --- */
  ulSysStatReg = BSWAP32( (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ssw);

  if( ulSysStatReg & (1 << 20) )
    /* Replay is running */
    ulReturnValue = 2;

  if( ulSysStatReg & (2 << 20) )
    /* RT is running */
    ulReturnValue = 3;

  if( ulSysStatReg & (4 << 20) )
    /* BC is running */
    ulReturnValue = 4; 
  
  if( ulSysStatReg & (8 << 20) )
    /* BM is running */
    ulReturnValue = 5;


  if( ulReturnValue == 0 )
  {
    /* -- Stopp BIU operation and write config values ---*/
    ulSysCtrlReg = BSWAP32( (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_scw);

    ulSysCtrlReg &= ~(0x1F<<27);                  /* clear cmd area */
    ulSysCtrlReg |=  (   9<<27);                  /* BIU disable    */

    ulSysCtrlReg &= ~(0x3FF);                     /* clear ETOOF and EFEXTO */
    ulSysCtrlReg |= ( ucEtoof  & 0x3F ) << 4;     /* ETOOF  9...4           */
    ulSysCtrlReg |= ( ucEfexTo & 0xF  );          /* EFEXTO 3...0           */

    (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_scw = BSWAP32(ulSysCtrlReg);

    wait_clocks(50 * clk_1us);

    /* -- Start BIU operation ---*/
    ulSysCtrlReg  = BSWAP32( (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_scw);
    ulSysCtrlReg &= ~(0x1F<<27);                  /* clear cmd area */
    ulSysCtrlReg |=  (   1<<27);                  /* BIU enable     */
    (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_scw = BSWAP32(ulSysCtrlReg);
  
    wait_clocks(50 * clk_1us);

  }


  /* done */

  if( ulReturnValue > 0 )
    PRINTF1("\r\n Error in %08X = api_sys_util_ef_time_out_def()\r\n", ulReturnValue );

#endif

  out->data[0] = ulReturnValue;
  return 0;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_UTIL              */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.03.03   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Parameter Count [count]                                    */
/*                List of Parameters [data_p]                                */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_SYS_UTIL' instruction                   */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_util( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_UTIL_INPUT * in, TY_MIL_COM_SYS_UTIL_OUTPUT * out )
{
  int i = 0;
  L_WORD ret_val = 0;
  BYTE out_count = 0;
  L_WORD ul_mode;
  AiUInt32 ul_addr, ul_data;
#if defined(_NUCLEUS) /* do only for onboard-SW */
  L_WORD tmpl;
  UINT32 api_ir_data_queue, tm2;
  L_WORD volatile *src_pp;
  L_WORD volatile *dest_p;
  L_WORD volatile *LastSrc_p;
#endif 

  out->xAck.ulSize = sizeof(TY_MIL_COM_SYS_UTIL_OUTPUT); /* Default ack size for 1 data word */


  ul_mode = in->data[0];
  switch(ul_mode)
  {
    case 0:
      ul_addr = in->data[1];
      ul_data = in->data[2];

      ret_val = mil_hw_novram_write(p_api_dev, ul_addr /* nvByteAddr */, ul_data /* nvdata */);
      out->data[0] = ret_val;

      out_count = 1;
      break;

    case 1:
      ul_addr = in->data[1];

      ret_val = mil_hw_novram_read(p_api_dev, ul_addr, &ul_data);
      out->data[0] = ul_data;

      out_count = 1;
      break;

#if defined(_NUCLEUS)      /* do only for onboard-SW */
    case 2:
      out_count = 0;

      mil_hw_novram_read(p_api_dev, NVR_BOARDTYPE_A, &ul_data);
      out->data[ out_count ] = ul_data;
      out_count++;

      ret_val = mil_hw_novram_read(p_api_dev, NVR_SERIAL_A, &ul_data);
      out->data[ out_count ] = ul_data;
      out_count++;

      out->data[ out_count ] = MIL_TSW_LEGACY_VERSION;
      out_count++;
      out->data[ out_count ] = 0;
      out_count++;
      out->data[ out_count ] = MIL_TSW_LEGACY_BUILD;
      out_count++;
      out->data[ out_count ] = (WORD)BSWAP32(p_api_dev->cb_p[ 0 ]->cid);
      out_count++;
      out->data[ out_count ] = (WORD)BSWAP32(p_api_dev->cb_p[ 0 ]->lca_id);
      out_count++;
      out->data[ out_count ] = (WORD) ( (TY_API_SYS_CB*) ( ( (BYTE*)p_api_dev->cb_p[ 0 ]) + 0x10000) )->cid;
      out_count++;
      out->data[ out_count ] = (WORD) ( (TY_API_SYS_CB*) ( ( (BYTE*)p_api_dev->cb_p[ 0 ]) + 0x10000) )->lca_id;
      out_count++;
      out->data[ out_count ] = p_api_dev->SrecCheckSum;
      out_count++;
      break;
    case 3:
      set_time_mark(&api_ir_data_queue);
      WAIT_CLOCKS(p_api_dev->clk_1us *10)tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time (10us?): %d \n\r", tm2 / 75, 0, 0);
      set_time_mark(&api_ir_data_queue);
      WAIT_CLOCKS(p_api_dev->clk_1us *100)tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time (100us?): %d \n\r", tm2 / 75, 0, 0);
      set_time_mark(&api_ir_data_queue);
      WAIT_CLOCKS(p_api_dev->clk_1us *1000)tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time (1000us?): %d \n\r", tm2 / 75, 0, 0);
      set_time_mark(&api_ir_data_queue);
      WAIT_CLOCKS(p_api_dev->clk_1us *10000)tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time (10000us?): %d \n\r", tm2 / 75, 0, 0);
      src_pp = (L_WORD*)API_GLB_MEM_ADDR_ABS(0x10000);
      dest_p = (L_WORD*)0xA0300000;
      /*******************************************************************/
      set_time_mark(&api_ir_data_queue);
      for(i = 0; i < 0x2040; i += 4)
      {
        tmpl =  *src_pp;
        wbflush();
        *dest_p = BSWAP32(tmpl);
        wbflush();
        dest_p++;
        LastSrc_p++;
      }
      tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time1: %d %f\n\r", tm2 / 75, (float) (0x2040) *1000 * 1000 / 1024 / 1024 / (float)tm2 *75, 0);

      /*******************************************************************/
      set_time_mark(&api_ir_data_queue);
      src_pp = (L_WORD*)API_GLB_MEM_ADDR_ABS(0x10000);
      dest_p = (L_WORD*)0xA0300000;
      tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time2: %d \n\r", tm2 / 75, 0, 0);
      set_time_mark(&api_ir_data_queue);
      for(i = 0; i < 0x2040; i += 4)
      {
        tmpl =  *src_pp;
        *dest_p = BSWAP32(tmpl);
        dest_p++;
        LastSrc_p++;
      }
      tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time2: %d %f\n\r", tm2 / 75, (float) (0x2040) *1000 * 1000 / 1024 / 1024 / (float)tm2 *75, 0);
      /*******************************************************************/
      src_pp = (L_WORD*)API_GLB_MEM_ADDR_ABS(0x10000);
      dest_p = (L_WORD*)0xA0300000;
      set_time_mark(&api_ir_data_queue);
      for(i = 0; i < 0x2040; i += 4)
      {
        *dest_p =  *src_pp;
        dest_p++;
        LastSrc_p++;
      }
      tm2 = time_since_mark(&api_ir_data_queue);
      PRINTF3("time3: %d %f\n\r", tm2 / 75, (float) (0x2040) *1000 * 1000 / 1024 / 1024 / (float)tm2 *75, 0);
      /*******************************************************************/
      src_pp = (L_WORD*)API_GLB_MEM_ADDR_ABS(0x20000);
      dest_p = (L_WORD*)0xA0300000;

      PRINTF3("src_pp %X dest_p %X \n\r", (L_WORD)src_pp &0x0FFFFFFF, (L_WORD)dest_p &0x0FFFFFFF, 0);
      PRINTF0("vor");
      set_time_mark(&api_ir_data_queue);
      DMALoc2Loc(0, ( (L_WORD)src_pp &0x0FFFFFFF), ( (L_WORD)dest_p &0x0FFFFFFF), 0x2040);
      DMAWaitIdle(0);
      src_pp += 0x2040;
      dest_p += 0x2040;
      tm2 = time_since_mark(&api_ir_data_queue);
      if(tm2 > 0)
        PRINTF2("time4: %d %f\n\r", tm2 / 75, (float) (0x2040) *1000 * 1000 / 1024 / 1024 / (float)tm2 *75);
      break;
      /*******************************************************************/

#endif /* do only for onboard-SW */
#ifdef _API3910
    case 4:
      out_count = 1;

      out->data[0] = 1;
      ret_val = efex_switch(p_api_dev, (BYTE)in->data[5] );

      break;
#endif

    case 5:
      out_count = 1;
      ret_val = api_sys_util_ef_time_out_def( p_api_dev, in, out );
      break;

    case 6:
      out_count = 1;
      ret_val = mil_hw_novram_start(p_api_dev);
      out->data[0] = ret_val;
      break;
    case 7:
      out_count = 1;
      ret_val = mil_hw_novram_finalize( p_api_dev );
      out->data[0] = ret_val;
      break;
    case 8:
      out_count = 1;

      if( in->count > 1 )
      {
          out->data[0] = 0;
          
          v_InitializeDrvModes( p_api_dev );

          ul_data = in->data[1];

          for( i=0; i<4; i++ ) 
              if( p_api_dev->drv_mode[i] == 0 ) p_api_dev->drv_mode[i] = (WORD)ul_data;
      }
      else
      {
          out->data[ 0 ] = 1;
      }
      break;

    default:
      break;

  }



  out->count = out_count;
   /* Correct ack size according to data count */
  out->xAck.ulSize = sizeof(TY_MIL_COM_ACK) + (out->count + 1) * sizeof(L_WORD);

  return ret_val;

} /* end: api_sys_util */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_UTIL              */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.03.03   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Parameter Count [count]                                    */
/*                List of Parameters [data_p]                                */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_SYS_UTIL' instruction                   */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_util_legacy(TY_API_DEV *p_api_dev, BYTE in_count, BYTE *data_p)
{
  BYTE ackfl, out_count = 0;
  L_WORD ul_mode;
  AiUInt32 ul_addr, ul_data;

  ackfl = API_ACK2;

  ul_mode = MAKE_B2LWORD(*(data_p), *(data_p + 1), *(data_p + 2), *(data_p + 3) );
  switch(ul_mode)
  {
    case 0:
      ul_addr = MAKE_B2LWORD(*(data_p + 4), *(data_p + 5), *(data_p + 6), *(data_p + 7) );
      ul_data = MAKE_B2LWORD(*(data_p + 8), *(data_p + 9), *(data_p + 10), *(data_p + 11) );

      p_api_dev->lword_data_arr[ 0 ] = mil_hw_novram_write(p_api_dev, ul_addr /* nvByteAddr */, ul_data /* nvdata */);

      out_count = 1;
      break;

    case 1:
      ul_addr = MAKE_B2LWORD(*(data_p + 4), *(data_p + 5), *(data_p + 6), *(data_p + 7) );

      mil_hw_novram_read(p_api_dev, ul_addr, &ul_data );
      p_api_dev->lword_data_arr[ 0 ] = ul_data;

      out_count = 1;
      break;

    case 6:
      out_count = 1;
      p_api_dev->lword_data_arr[ 0 ] = mil_hw_novram_start( p_api_dev );
      break;
    case 7:
      out_count = 1;
      p_api_dev->lword_data_arr[ 0 ] = mil_hw_novram_finalize( p_api_dev );
      break;
    default:
      break;

  }

  p_api_dev->byte_data_arr[ 0 ] = out_count;

  p_api_dev->b_length = 1;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = out_count;
  p_api_dev->ack_b_ptr = &(p_api_dev->byte_data_arr[ 0 ]);
  p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_sys_util */


NAMESPACE1553_END


/*** end of file ***/

