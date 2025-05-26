/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_rep.c
    Replay module of the 1553 ASP Driver SW. This module comprises the handler
    routines for the Replay instructions.
*/

#include "mil_tsw_includes.h"

NAMESPACE1553_START





TY_MIL_COM_TABLE_ENTRY axTargetRepCommands[] = {
  {MIL_COM_REP_INI,    (TY_MIL_COM_FUNC_PTR)api_rep_ini,       "api_rep_ini",       sizeof(TY_MIL_COM_REP_INI_INPUT),     sizeof(TY_MIL_COM_ACK)               },
  {MIL_COM_REP_START,  (TY_MIL_COM_FUNC_PTR)api_rep_start_cmd, "api_rep_start_cmd", sizeof(TY_MIL_COM),                   sizeof(TY_MIL_COM_ACK)               },
  {MIL_COM_REP_HALT,   (TY_MIL_COM_FUNC_PTR)api_rep_halt_cmd,  "api_rep_halt_cmd",  sizeof(TY_MIL_COM),                   sizeof(TY_MIL_COM_ACK)               },
  {MIL_COM_REP_RT_CON, (TY_MIL_COM_FUNC_PTR)api_rep_rt_con,    "api_rep_rt_con",    sizeof(TY_MIL_COM_REP_RT_CON_INPUT),  sizeof(TY_MIL_COM_ACK)               },
  {MIL_COM_REP_STATUS, (TY_MIL_COM_FUNC_PTR)api_rep_status,    "api_rep_status",    sizeof(TY_MIL_COM),                   sizeof(TY_MIL_COM_REP_STATUS_OUTPUT) },
};


void api_rep_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetRepCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetRepCommands;
}




/*****************************************************************************/
/*                                                                           */
/*   Module : API_REP                  Submodule : API_REP_INI               */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI' instruction to initialize the   */
/*    API1553-DS BM mode.                                                    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rep_ini( TY_API_DEV *p_api_dev, TY_MIL_COM_REP_INI_INPUT * in, TY_MIL_COM_ACK * out )
{
  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_REPLAY))
    return API_ERR_REPLAY_NOT_AVAILABLE;

  api_io_mode_con(p_api_dev, API_REP_MODE, API_OFF);
  
  api_io_rep_ini(p_api_dev, (BYTE)  in->cet, 
                            (BYTE)  in->nct, 
                            (BYTE)  in->cyc, 
                            (BYTE)  in->nimg, 
                            (BYTE)  in->alt, 
                            (BYTE)  in->rlt, 
                            (BYTE)  in->rint, 
                            (L_WORD)in->min, 
                            (L_WORD)in->msec, 
                            (BYTE)  in->sign, 
                            (L_WORD)in->fsize);

  p_api_dev->rep_status[ p_api_dev->biu ] = API_REP_STOP;
  api_io_rep_rt_con(p_api_dev, API_RT_ALL, API_ON, 0);

  return 0;

} /* end: api_rep_ini */


/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_REP                 Submodule : API_X_REP_GET_LS_ONLY_INFO */
/*                                                                           */
/*   Author : Marco Motz              Project   : API3910                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : LS Only Replay enabled/disabled                            */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to read the ALT bit from the LS Control Block.   */
/*    For LS Replay Only ALT shall be disabled by the application.           */
/*    For synchronous start of a LS & HS Replay ALT shall be enabled.        */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_rep_get_ls_only_info(TY_API_DEV *p_api_dev)
{
  BYTE flag;
  L_WORD tmpl;

  flag = 0;

  /* Read the LS-BIU1 Replay Control Word */
  tmpl = BSWAP32( (*p_api_dev->cb_p[ (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1) ]).rcw);
  if( (tmpl &0x01000000L) == 0x01000000L)
    flag = 1;
  /* Sync Replay enabled */ 

  return (flag);

} /* end: api_x_rep_get_ls_only_info */

/*****************************************************************************/
/*                                                                           */
/*   Module : I39_BM             Submodule : API_X_REP_CALC_TTHIGH           */
/*                                                                           */
/*   Author : Marco Motz         Project   : API3910                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Timetag High Word [ttag_high]                              */
/*                                                                           */
/*    Outputs   : Minutes                                                    */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

int api_x_rep_calc_tthigh(L_WORD ttag_high)
{
  int tmpl, mm, hh, dd;

  tmpl = 0L;

  mm = 0L;
  hh = 0L;
  dd = 0L;

  /* Minutes */
  mm = ttag_high &0x0000003FL;

  /* Hours */
  hh = ttag_high &0x000007C0L;
  hh >>= 6;
  hh *= 60L;

  /* Days */
  dd = ttag_high &0x000FF800L;
  dd >>= 11;
  dd *= 24L;
  dd *= 60L;

  /* Sum-up */
  tmpl = dd + hh + mm;

  return (tmpl);

} /* end: api_x_rep_calc_tthigh */

/*****************************************************************************/
/*                                                                           */
/*   Module : I39_BM             Submodule : API_X_REP_CALC_TTLOW            */
/*                                                                           */
/*   Author : Marco Motz         Project   : API3910                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Timetag Low Word [ttag_low]                                */
/*                                                                           */
/*    Outputs   : Micro-Seconds                                              */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

int api_x_rep_calc_ttlow(L_WORD ttag_low)
{
  int tmpl, us, ss;

  tmpl = 0L;

  us = 0L;
  ss = 0L;

  /* Micro seconds */
  us = ttag_low &0x000FFFFFL;

  /* Seconds */
  ss = ttag_low &0x03F00000L;
  ss >>= 20;
  ss *= 1000000L;

  /* Sum-up */
  tmpl = ss + us;

  return (tmpl);

} /* end: api_x_rep_calc_ttlow */


/*******************************************************************************/
/*                                                                             */
/*   Module : API_REP                  Submodule : API_REP_MODE                */
/*                                                                             */
/*   Author : Michael Stahl            Project   : API1553-DS                  */
/*                                                                             */
/*   Source : C                        Tools     : PC/AT; Norton Editor;       */
/*                                                 CYGNUS, GNU-C, As, and LD   */
/*                                                 IDT-C 5.1 Toolkit           */
/*-----------------------------------------------------------------------------*/
/*   Create : 01.08.99   Update : 25.05.00                                     */
/*-----------------------------------------------------------------------------*/
/*    Descriptions                                                             */
/*    ------------                                                             */
/*    Inputs    : REP Control (START/HALT) [con]                               */
/*                                                                             */
/*    Outputs   : Instruction acknowledge type [ackfl]                         */
/*                                                                             */
/*    Description :                                                            */
/*    This function handles the 'API_REP_START' and 'API_REP_HALT' instruction */
/*    to control the replay operations (START / STOP) of the API1553-DS        */
/*                                                                             */
/*******************************************************************************/
L_WORD api_rep_start_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
  BYTE   flag    = 0;
  BYTE   flag_ls = 0;

  L_WORD t_day;
  L_WORD t_hour;
  L_WORD t_min;
  L_WORD t_sec;
  L_WORD t_usec;
  L_WORD sth;
  L_WORD stl;
  L_WORD volatile *ptr;

  int ls_tm_min     = 0;
  int ls_tm_usec    = 0;
  int hs_tm_min     = 0;
  int hs_tm_usec    = 0;
  int sys_tm_min    = 0;
  int sys_tm_usec   = 0;
  int delta_tm_min  = 0;
  int delta_tm_usec = 0;
  int offs_tm_min   = 0;
  int offs_tm_usec  = 0;
  int usec          = 0;
  int min           = 0;
  short j            = 0;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_REPLAY))
      return API_ERR_REPLAY_NOT_AVAILABLE;

  if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
      return API_ERR_REPLAY_NOT_AVAILABLE;

  if (API_DEVICE_MODE_IS_NOREPLAY_NOERROR(p_api_dev->drv_mode[p_api_dev->biu]))
      return API_ERR_REPLAY_NOT_AVAILABLE;

  if( ! ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
    /* LS only */
    return api_rep_start(p_api_dev);


  /* LS and HS handling */

  flag = api_x_rep_get_ls_only_info(p_api_dev);
  if(flag == 1)
  {
    /* Get LS Timetag */
    ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf);
    for(flag_ls = 0, j = 0; j < 100; j++, ptr++)
    {
       /* Scan first 100 entries in LS buffer on Timetags */
      if( ( (BSWAP32(*(ptr + 0) ) &0xF0000000L) == 0x30000000L /*TTAG-Hi*/) && ( (BSWAP32(*(ptr + 1) ) &0xF0000000L) == 0x20000000L /*TTAG-Lo*/) )
      {
        ls_tm_min = api_x_rep_calc_tthigh(BSWAP32(*(ptr + 0) ) );
        ls_tm_usec = api_x_rep_calc_ttlow(BSWAP32(*(ptr + 1) ) );
        flag_ls = 1;
        break;
      }
    }

    /* Get HS Timetag (expected to be located on fixed position) */
    ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(Api39_glb_mem.stream[ p_api_dev->hs_stream ].base.rep_buf);
    hs_tm_min = api_x_rep_calc_tthigh(BSWAP32(*(ptr + 3) ) );
    hs_tm_usec = api_x_rep_calc_ttlow(BSWAP32(*(ptr + 4) ) );

    mil_hw_irig_get_time(p_api_dev, &t_day, &t_hour, &t_min, &t_sec, &t_usec, &sth, &stl);

    sys_tm_min  = api_x_rep_calc_tthigh(sth);
    sys_tm_usec = api_x_rep_calc_ttlow(stl);

    /* Calculate difference bewteen SYS & HS */
    offs_tm_min = sys_tm_min - hs_tm_min;
    offs_tm_usec = sys_tm_usec - hs_tm_usec;

    /* Calculate difference bewteen LS & HS */
    if(flag_ls == 1)
    {
      delta_tm_min = hs_tm_min - ls_tm_min;
      delta_tm_usec = hs_tm_usec - ls_tm_usec;
    }

    usec = offs_tm_usec + delta_tm_usec + 1000000L;
    min = offs_tm_min + delta_tm_min;
    min = min + (usec / 60000000L);
    usec = usec % 60000000L;

    /* Init LS Abs Offset Registers */
    (*p_api_dev->cb_p[ (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1) ]).ramto = BSWAP32(min); /* Replay Absolute Minute Offset Register */
    (*p_api_dev->cb_p[ (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1) ]).ramio = BSWAP32(usec); /* Replay Absolute Microsecond Offset Register*/

    /* Init HS Abs Offset Registers */
    (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ramto = BSWAP32(min); /* Replay Absolute Minute Offset Register */
    (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ramio = BSWAP32(usec); /* Replay Absolute Microsecond Offset Register*/
  }

  /* Start Replay */
  api_rep_start(p_api_dev );
  api39_rep_start(p_api_dev);

  return 0;

} /* end: api_x_rep_mode */



L_WORD api_rep_halt_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
  if( ! ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
    /* LS only */
    return api_rep_halt(p_api_dev);

  /* LS and HS handling */
  api_rep_halt(p_api_dev );
  api39_rep_halt(p_api_dev);

  return 0;
} /* end: api_x_rep_mode */






L_WORD api_rep_start( TY_API_DEV *p_api_dev )
{
  if( p_api_dev->rep_status[ p_api_dev->biu ] != API_REP_BUSY )
  {
    /* REP ON */

    /* Init System Control Block registers */
    (*p_api_dev->cb_p[ p_api_dev->biu ]).rbb  = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf);
    (*p_api_dev->cb_p[ p_api_dev->biu ]).rbp  = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf);
    (*p_api_dev->cb_p[ p_api_dev->biu ]).rec  = BSWAP32(p_api_dev->sim_rep[ p_api_dev->biu ].entry_cnt);
    (*p_api_dev->cb_p[ p_api_dev->biu ]).raec = BSWAP32(p_api_dev->sim_rep[ p_api_dev->biu ].entry_cnt);

    p_api_dev->sim_rep[ p_api_dev->biu ].mode    = API_ON;
    p_api_dev->sim_rep[ p_api_dev->biu ].rpi_cnt = 0L;
    p_api_dev->sim_rep[ p_api_dev->biu ].saddr   = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf;
    p_api_dev->sim_rep[ p_api_dev->biu ].flag    = API_OFF;

    api_io_mode_con(p_api_dev, API_REP_MODE, API_ON);
    p_api_dev->rep_status[ p_api_dev->biu ] = API_REP_BUSY;
  }

  return 0;
} /* end: api_rep_start */


L_WORD api_rep_halt(TY_API_DEV *p_api_dev )
{
    /* REP OFF */
    p_api_dev->sim_rep[ p_api_dev->biu ].mode = API_OFF;
    api_io_mode_con(p_api_dev, API_REP_MODE, API_OFF);
    p_api_dev->rep_status[ p_api_dev->biu ] = API_REP_STOP;

    return 0;

} /* end: api_rep_halt */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_REP_RT_CON			 */
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
/*    Inputs    : RT Replay mode[con]			                             */
/*                RT set mode [mode]                                         */
/*                RT [rt]                                                    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_REP_RT_CON' instruction to set the      */
/*    Replay Remote Terminal Enable register.                                */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rep_rt_con( TY_API_DEV *p_api_dev, TY_MIL_COM_REP_RT_CON_INPUT  * in, TY_MIL_COM_ACK * out )
{
    if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_REPLAY))
        return API_ERR_REPLAY_NOT_AVAILABLE;

    api_io_rep_rt_con(p_api_dev, (BYTE)in->con, (BYTE)in->mode, (BYTE)in->rt);

  return 0;
} /* end: api_rep_rt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_REP                  Submodule : API_REP_STATUS_READ		 */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_REP_STATUS_READ' instruction to read    */
/*    the actual Replay status.	                                             */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rep_status( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_REP_STATUS_OUTPUT * out )
{
  BYTE   replay_mode = 0;
  L_WORD act_cnt     = 0;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_REPLAY))
      return API_ERR_REPLAY_NOT_AVAILABLE;

  if(p_api_dev->rep_status[p_api_dev->biu] == API_REP_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
                          API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->raec), In);
  }

  out->rpi_cnt = p_api_dev->sim_rep[ p_api_dev->biu ].rpi_cnt;
  out->saddr   = p_api_dev->sim_rep[ p_api_dev->biu ].saddr;
  out->size    = p_api_dev->sim_rep[ p_api_dev->biu ].size;
  out->raec    = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).raec);
  act_cnt      = out->raec;
  

  if(p_api_dev->rep_status[ p_api_dev->biu ] == API_REP_BUSY)
  {
    /* Monitor the Actual Replay Entry Count required */
    /* for Host Application to decide when Replay finished */
    if( (act_cnt == 0) || (act_cnt > BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).rec) ) )
    {
      replay_mode = API_OFF;
    }
    /* Repay finished */
    else
      replay_mode = API_ON;
    /* Replay running */
  }
  else
  {
    replay_mode = API_OFF;
  }
  /* Replay stopped */

  out->status = replay_mode;

  return 0;

} /* end: api_rep_status_read */


NAMESPACE1553_END

