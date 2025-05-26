/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_rt.c
    Remote Terminal (RT) module of the AYI1553 ASP 	Driver SW. This module 
    comprises the handler routines for the RT instructions.
*/

#include "mil_tsw_includes.h"

NAMESPACE1553_START

#ifdef CMPL4SIM



TY_MIL_COM_TABLE_ENTRY axTargetLsRtCommands[] = {
   /* LS RT Functions */
  {MIL_COM_LS_RT_INI,        (TY_MIL_COM_FUNC_PTR)api_rt_ini_cmd,         "api_rt_ini_cmd",        sizeof(TY_MIL_COM_LS_RT_INI_INPUT),       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_CON,        (TY_MIL_COM_FUNC_PTR)api_rt_con_cmd,         "api_rt_con_cmd",        sizeof(TY_MIL_COM_LS_RT_CON_INPUT),       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_SA_CON,     (TY_MIL_COM_FUNC_PTR)api_rt_sa_con_cmd,      "api_rt_sa_con_cmd",     sizeof(TY_MIL_COM_LS_RT_SA_CON_INPUT),    sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_INI_RESP,   (TY_MIL_COM_FUNC_PTR)api_rt_ini_resp,        "api_rt_ini_resp",       sizeof(TY_MIL_COM_LS_RT_INI_RESP_INPUT),  sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_INI_NXW,    (TY_MIL_COM_FUNC_PTR)api_rt_ini_nxw,         "api_rt_ini_nxw",        sizeof(TY_MIL_COM_LS_RT_INI_NXW_INPUT),   sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_INI_LCW,    (TY_MIL_COM_FUNC_PTR)api_rt_ini_lcw,         "api_rt_ini_lcw",        sizeof(TY_MIL_COM_LS_RT_INI_LCW_INPUT),   sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_INI_LSW,    (TY_MIL_COM_FUNC_PTR)api_rt_ini_lsw,         "api_rt_ini_lsw",        sizeof(TY_MIL_COM_LS_RT_INI_LSW_INPUT),   sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_BH_DEF,     (TY_MIL_COM_FUNC_PTR)api_rt_bhd_def,         "api_rt_bhd_def",        sizeof(TY_MIL_COM_LS_RT_BH_DEF_INPUT),    sizeof(TY_MIL_COM_LS_RT_BH_DEF_OUTPUT)   },
  {MIL_COM_LS_RT_BH_GET,     (TY_MIL_COM_FUNC_PTR)api_rt_bhd_read,        "api_rt_bhd_read",       sizeof(TY_MIL_COM_LS_RT_BH_GET_INPUT),    sizeof(TY_MIL_COM_LS_RT_BH_DEF_OUTPUT)   }, /* same output as BH def*/
  {MIL_COM_LS_RT_SA_ERR_CON, (TY_MIL_COM_FUNC_PTR)api_rt_sa_err_con,      "api_rt_sa_err_con",     sizeof(TY_MIL_COM_LS_RT_SA_ERR_CON_INPUT),sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_SA_ERR_GET, (TY_MIL_COM_FUNC_PTR)api_rt_sa_err_get,      "api_rt_sa_err_get",     sizeof(TY_MIL_COM_LS_RT_SA_ERR_GET_INPUT),sizeof(TY_MIL_COM_LS_RT_SA_ERR_GET_OUTPUT)},
  {MIL_COM_LS_RT_DYTAG_DEF,  (TY_MIL_COM_FUNC_PTR)api_rt_dytag_def,       "api_rt_dytag_def",      sizeof(TY_MIL_COM_LS_RT_DYTAG_DEF_INPUT), sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_DYTAG_GET,  (TY_MIL_COM_FUNC_PTR)api_rt_dytag_get,       "api_rt_dytag_get",      sizeof(TY_MIL_COM_LS_RT_DYTAG_GET_INPUT), sizeof(TY_MIL_COM_LS_RT_DYTAG_GET_OUTPUT)  },
  {MIL_COM_LS_RT_START,      (TY_MIL_COM_FUNC_PTR)api_rt_start,           "api_rt_start",          sizeof(TY_MIL_COM),                       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_HALT,       (TY_MIL_COM_FUNC_PTR)api_rt_halt,            "api_rt_halt",           sizeof(TY_MIL_COM),                       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_RT_STATUS_READ,(TY_MIL_COM_FUNC_PTR)api_rt_status_read,     "api_rt_status_read",    sizeof(TY_MIL_COM),                       sizeof(TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT)},
  {MIL_COM_LS_RT_MSG_READ,   (TY_MIL_COM_FUNC_PTR)api_rt_msg_read,        "api_rt_msg_read",       sizeof(TY_MIL_COM_LS_RT_MSG_READ_INPUT),  sizeof(TY_MIL_COM_LS_RT_MSG_READ_OUTPUT)},
  {MIL_COM_LS_RT_MSG_READ_ALL,(TY_MIL_COM_FUNC_PTR)api_rt_msg_read_all,   "api_rt_msg_read_all",   sizeof(TY_MIL_COM),                       sizeof(TY_MIL_COM_LS_RT_MSG_READ_ALL_OUTPUT)},
  {MIL_COM_LS_RT_SA_MSG_READ,(TY_MIL_COM_FUNC_PTR)api_rt_sa_msg_read,     "api_rt_sa_msg_read",    sizeof(TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT),sizeof(TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT)},
  {MIL_COM_LS_RT_GLOBAL_CON, (TY_MIL_COM_FUNC_PTR)api_rt_global_con,      "api_rt_global_con",     sizeof(TY_MIL_COM_LS_RT_GLOBAL_CON_INPUT),sizeof(TY_MIL_COM_ACK)},
  {MIL_COM_LS_RT_GET_SIM_INFO,(TY_MIL_COM_FUNC_PTR)api_rt_get_sim_info,   "api_rt_get_sim_info",   sizeof(TY_MIL_COM_LS_RT_GET_SIM_INFO_INPUT), sizeof(TY_MIL_COM_LS_RT_GET_SIM_INFO_OUTPUT)},
  {MIL_COM_LS_RT_SA_BH_GET,  (TY_MIL_COM_FUNC_PTR)api_rt_sa_bh_get,       "api_rt_sa_bh_get",      sizeof(TY_MIL_COM_LS_RT_SA_BH_GET_INPUT), sizeof(TY_MIL_COM_LS_RT_SA_BH_GET_OUTPUT)},
  {MIL_COM_LS_RT_MODE_CTRL,  (TY_MIL_COM_FUNC_PTR)api_rt_mode_ctrl,       "api_rt_mode_ctrl",      sizeof(TY_MIL_COM_LS_RT_MODE_CTRL_INPUT), sizeof(TY_MIL_COM_ACK) },
  {MIL_COM_LS_RT_DWC_SET,    (TY_MIL_COM_FUNC_PTR)api_rt_dwc_set,         "api_rt_dwc_set",        sizeof(TY_MIL_COM_LS_RT_DWC_SET_INPUT),   sizeof(TY_MIL_COM_ACK) },
  {MIL_COM_LS_RT_DWC_GET,    (TY_MIL_COM_FUNC_PTR)api_rt_dwc_get,         "api_rt_dwc_get",        sizeof(TY_MIL_COM_LS_RT_DWC_GET_INPUT),   sizeof(TY_MIL_COM_LS_RT_DWC_GET_OUTPUT) }
};

void api_ls_rt_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLsRtCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLsRtCommands;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_INI                */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 07.02.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Operation Control [con],                                */
/*                RT Bus Response Control [bus],                             */
/*                RT Response Time in 0.25 usec steps [resp_time],           */
/*                Next RT Status Word [nxw]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_INI' instruction to initialize the   */
/*    specified API1553-DS RT and to define the default RT Response Time in  */
/*    0.25 usec steps and the Next RT Status Word.                           */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_ini_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_INPUT * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if (api_opr_sf(p_api_dev, in->cmd.ulCommand, (BYTE)in->rt_addr, (BYTE)in->con) != 0)
            return API_ERR_RT_NOT_AVAILABLE;

    return api_rt_ini( p_api_dev, (BYTE)in->rt_addr, (BYTE)in->con, (BYTE)in->bus, (BYTE)in->resp_time, (WORD)in->nxw );
}

static void api_rt_enable_sa_mc( TY_API_DEV *p_api_dev, BYTE rt_addr, WORD swm )
{
    BYTE   i = 0;

    /* RT Subaddress */
    for(i = 1; i < (MAX_API_RT_MC_SA - 1); i++)
    {
        api_io_rt_sa_con(p_api_dev, rt_addr, i /*sa_mc*/, 0 /*hid*/, API_RT_SA_TYPE_RCV, API_RT_SA_ENABLE, 0, 0, swm);
        api_io_rt_sa_con(p_api_dev, rt_addr, i /*sa_mc*/, 0 /*hid*/, API_RT_SA_TYPE_XMT, API_RT_SA_ENABLE, 0, 0, swm);
    }
    /* Modecode */
    for(i = 0; i < (MAX_API_RT_MC_SA); i++)
    {
        api_io_rt_sa_con(p_api_dev, rt_addr, i /*sa_mc*/, 0 /*hid*/, API_RT_MC_TYPE_RCV, API_RT_SA_ENABLE, 0, 0, swm);
        api_io_rt_sa_con(p_api_dev, rt_addr, i /*sa_mc*/, 0 /*hid*/, API_RT_MC_TYPE_XMT, API_RT_SA_ENABLE, 0, 0, swm);
    }
}

L_WORD api_rt_ini( TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con, BYTE bus, BYTE resp_time, WORD nxw )
{
    BYTE   i         = 0;
    L_WORD ret_val   = 0;


  if(resp_time < API_RT_RESP_TIME_MIN)
    resp_time = API_RT_RESP_TIME_DEF;

  if(rt_addr < MAX_API_RT)
  {
    for(i = 0; i < MAX_API_RT_XMT_SA; i++)
    {
      p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_rx[ i ] = API_OFF;
      p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_tx[ i ] = API_OFF;
    }

    switch(con)
    {
      case API_RT_DISABLE:
        api_io_rt_ir_clr(p_api_dev, rt_addr);
        api_io_rt_con(p_api_dev, rt_addr, con, bus, resp_time, nxw);
        if(rt_addr == p_api_dev->single_rt_addr[p_api_dev->biu])
            p_api_dev->single_rt_addr[p_api_dev->biu] = 0;
        break;
      case API_RT_ENABLE:
          api_io_rt_ir_clr(p_api_dev, rt_addr);
          api_io_rt_con(p_api_dev, rt_addr, con, bus, resp_time, nxw);
          api_rt_enable_sa_mc(p_api_dev, rt_addr, 0x400);
        break;
      case API_RT_MAILBOX:
        api_io_rt_ir_clr(p_api_dev, rt_addr);
        api_io_rt_con(p_api_dev, rt_addr, con, bus, resp_time, nxw);
        api_rt_enable_sa_mc(p_api_dev, rt_addr, 0);
        break;
      case API_RT_ENABLE_DUAL_REDUNDANT:
          if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_SINGLE_OPERATION))
          {
              /* set single RT address and execute single RT mode if supported by FW */       
              if ((p_api_dev->single_rt_addr[p_api_dev->biu] != 0) && (rt_addr != p_api_dev->single_rt_addr[p_api_dev->biu]) && (rt_addr != 31))
              {
                  /* dual redundant mode is not allowed if another RT is enabled as single RT */
                  ret_val = API_ERR_CON_NOT_ALLOWED;
              }
              else
              {
                  if( rt_addr != 31)
                      p_api_dev->single_rt_addr[p_api_dev->biu] = rt_addr;
                  api_io_rt_ir_clr(p_api_dev, rt_addr);
                  api_io_rt_con(p_api_dev, rt_addr, API_RT_ENABLE, bus, resp_time, nxw);
                  api_rt_enable_sa_mc(p_api_dev, rt_addr, 0x400);
              }
          }
          else
              ret_val = API_ERR_CMD_NOT_SUPPORTED_BY_FW;
          break;
      default:
        ret_val = API_ERR_INVALID_CON;
        break;
    }

    /* Create a dummy buffer with header ID 0 and buffer ID 0, which is used for all 
       subaddresses that are only enabled by default and not by a call to RTSACon. */
    api_io_rt_bhd_def(p_api_dev, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  }
  else
    ret_val = API_ERR_INVALID_RT;

  if( ret_val != 0 )
      return ret_val;

  if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
    api39_rt_ini(p_api_dev, rt_addr, con);

  return (ret_val);

} /* end: api_rt_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_CON                */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Enable / Disable [con]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_CON' instruction to enable or disable*/
/*    the specified API1553-DS RT on-the fly.                                */
/*                                                                           */
/*****************************************************************************/

L_WORD api_rt_con_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if (api_opr_sf(p_api_dev, in->cmd.ulCommand, (BYTE)in->rt_addr, (BYTE)in->con) != 0)
            return API_ERR_RT_NOT_AVAILABLE;

    return api_rt_con( p_api_dev, (BYTE)in->rt_addr, (BYTE)in->con );
}

L_WORD api_rt_con( TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE con)
{
  L_WORD ret_val = 0;

  if(rt_addr < MAX_API_RT)
  {
    if( (con == API_OFF) || (con == API_ON) )
      api_io_rt_ena_dis(p_api_dev, rt_addr, con);
    else
      ret_val = API_ERR_INVALID_CON;
  }
  else
    ret_val = API_ERR_INVALID_RT;


  if( ret_val != 0 )
      return ret_val;

  if( ul_StreamIs3910( p_api_dev, p_api_dev->biu ) )
   /* check Board is 3910 */
    api39_rt_con(p_api_dev, rt_addr, con);

  return (ret_val);

} /* end: api_rt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_INI_RESP           */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Response Time in 0.25 usec steps [resp_time]            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_INI_RESP' instruction to define the  */
/*    RT Response Time in 0.25 usec steps of the specified API1553-DS RT.    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_ini_resp( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_RESP_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val   = 0;
  BYTE   rt_addr   = (BYTE)in->rt_addr;
  BYTE   resp_time = (BYTE)in->resp_time;

  if(rt_addr < MAX_API_RT)
  {
    api_io_rt_con_resp(p_api_dev, rt_addr, resp_time);
  }
  else
    ret_val = API_ERR_INVALID_RT;

  return ret_val;

} /* end: api_rt_ini_resp */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_INI_NXW            */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                Next RT Status Word [nxw]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_INI_NXW' instruction to define the   */
/*    Next RT Status Word of the specified API1553-DS RT.                    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_ini_nxw( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_NXW_INPUT  * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;
  BYTE   rt_addr = (BYTE)in->rt_addr;
  WORD   nxw     = (WORD)in->nxw;

  if(rt_addr < MAX_API_RT)
  {
    api_io_rt_con_nxw(p_api_dev, rt_addr, nxw);
  }
  else
    ret_val = API_ERR_INVALID_RT;

  return (ret_val);

} /* end: api_rt_ini_nxw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_INI_LCW            */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Last Command Word [lcw]                                 */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_INI_LCW' instruction to define the   */
/*    RT Last Command Word of the specified API1553-DS RT.                   */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_ini_lcw( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_LCW_INPUT  * in, TY_MIL_COM_ACK * out )
{
  L_WORD ackfl   = 0;
  BYTE   rt_addr = (BYTE)in->rt_addr;
  WORD   lcw     = (WORD)in->lcw;

  if(rt_addr < MAX_API_RT)
  {
    api_io_rt_con_lcw(p_api_dev, rt_addr, lcw);
  }
  else
    ackfl = API_ERR_INVALID_RT;

  return (ackfl);

} /* end: api_rt_ini_lcw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_INI_LSW            */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Last Status Word [lsw]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_INI_LSW' instruction to define the   */
/*    RT Last Status Word of the specified API1553-DS RT.                    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_ini_lsw( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_INI_LSW_INPUT  * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;
  BYTE   rt_addr = (BYTE)in->rt_addr;
  WORD   lsw     = (WORD)in->lsw;

  if(rt_addr < MAX_API_RT)
  {
    api_io_rt_con_lsw(p_api_dev, rt_addr, lsw);
  }
  else
    ret_val = API_ERR_INVALID_RT;

  return (ret_val);

} /* end: api_rt_ini_lsw */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_BHD_DEF            */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_BH_DEF' instruction to define a      */
/*    RT Buffer Header structure.                                            */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_bhd_def( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_BH_DEF_INPUT * in, TY_MIL_COM_LS_RT_BH_DEF_OUTPUT * out )
{
    L_WORD ret_val = 0;
    BYTE   sts_ena = 0;
    WORD   nbufs   = 0;
    WORD   sq_id   = 0;

    WORD hid   = (WORD)in->hid;
    WORD bid   = (WORD)in->bid;
    BYTE qsize = (BYTE)in->qsize;
    BYTE bqm   = (BYTE)in->bqm;
    BYTE bsm   = (BYTE)in->bsm;
    BYTE sts   = (BYTE)in->sqm;


  if( (hid > 0) && (hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area) )
  {
    /* 04.06.07 */
    sq_id = hid; 
    sts_ena = API_RT_BH_STS_ONE_SQ;
    if (p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_sq_area == p_api_dev->glb_mem.sim_buf_count[ p_api_dev->biu ])
    {
      sq_id = bid; /* use corresponding Buffer ID for StatusQ */
      if ((qsize != 0) && (sts == API_RT_BH_STS_QSIZE_SQ))
        sts_ena = API_RT_BH_STS_QSIZE_SQ;
    }

    nbufs = api_io_rt_bhd_def(p_api_dev, hid, 
                                         bid,                        /* Abs. Buffer ID */
                                         /*sid*/sq_id,               /* StatusQ ID */ /* 04.06.07 */
                                         /*eid*/hid,                 /* EventQ ID */
                                         qsize,                      /* Queue Size */ 
                                         bqm,                        /* Buffer Queue Mode */
                                         bsm,                        /* Buffer Store Mode */
                                         /*sts*/sts_ena,             /* StatusQ Size */ /* 04.06.07 */
                                         /*evs*/API_RT_BH_EVS_NO_EQ, /* EventQ Size */ 
                                         /*res*/0);
    
    out->bid      = bid;
    out->sid      = sq_id;
    out->eid      = hid;
    out->nbufs    = nbufs;
    out->hid_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area + (hid *MAX_API_RT_BH_DESC_SIZE);
    out->bid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
    out->sid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);
    out->eid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr);

  }
  else
    ret_val = API_ERR_INVALID_HID;

  return (ret_val);

} /* end: api_rt_bhd_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_SA_CON             */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Subaddress/Modecode [sa_mc],                            */
/*                RT Buffer Header ID [hid],                                 */
/*                RT Subaddress Type [sa_type],                              */
/*                RT Subaddress/Modecode Control (Enable/Disable) [con],     */
/*                RT Response Mode [rmod],                                   */
/*                RT Status Word Mask Control [smod],                        */
/*                RT Status Word Modification Mask [swm]                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_SA_CON' instruction to control the   */
/*    specified RT Subaddress/Modecode of the specified API1553-DS RT.       */
/*                                                                           */
/*****************************************************************************/



L_WORD api_rt_sa_con_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
    L_WORD ret_val = 0;

    BYTE rt_addr = (BYTE)in->rt_addr ;
    BYTE sa_mc   = (BYTE)in->sa_mc;
    WORD hid     = (WORD)in->sa_hid;
    BYTE sa_type = (BYTE)in->sa_type;
    BYTE con     = (BYTE)in->con;
    BYTE rmod    = (BYTE)in->rmod;
    BYTE smod    = (BYTE)in->smod;
    WORD swm     = (WORD)in->swm;

    if ((sa_type == API_RT_MC_TYPE_XMT) && (sa_mc == 0) && (con != API_RT_SA_DISABLE))
    {
        /* Special handling for dynamic bus control modecode if BC is disabled */
        if (API_DEVICE_MODE_IS_DISABLE_BC(p_api_dev->drv_mode[p_api_dev->biu]))
        {
            /* We allow the following two cases:
               DBC MC enabled  with smod=API_RT_SWM_OR and swm=0x400     --> ME
               DBC MC disabled                                           --> No Response Error
            */
            if ((smod != 0) || (swm != 0x400))
                return API_ERR_BC_NOT_AVAILABLE;
        }
    }

    ret_val = api_rt_sa_con(p_api_dev, rt_addr, sa_mc, hid, sa_type, con, rmod, smod, swm);

    if( ret_val != 0 )
        return ret_val;

    /* HS RT handling */
    if( (sa_mc == uc_HsSubAddress) && ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
    {
      if(sa_type == API_RT_SA_TYPE_RCV)
      {
        /* 21.09.00: set up HS link for HS-RX-SA26 only */
        ret_val = api39_rt_sa_con(p_api_dev, rt_addr, sa_mc, hid, sa_type, con, rmod, smod, swm);
      }
      else if( sa_type == API_RT_SA_TYPE_XMT )
      {
        /* 21.09.00: set Buffer Pointer to RT descriptor for HS-TX-SA26 only */
        (*p_api_dev->rt_bh_desc_p[ (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1) ])[ hid ].bp_addr = BSWAP32(API39_RT_DESC_ADDR + ( (L_WORD)rt_addr *MAX_API39_RT_DESC_SIZE) + 0x40);
      }
    }


    return ret_val;
}

L_WORD api_rt_sa_con(TY_API_DEV *p_api_dev, BYTE rt_addr, BYTE sa_mc, WORD hid, BYTE sa_type, BYTE con, BYTE rmod, BYTE smod, WORD swm)
{
    L_WORD ret_val = 0;

    /* LS RT handling */
    if( rt_addr >= MAX_API_RT )
        return API_ERR_INVALID_RT;

    if( sa_mc >= MAX_API_RT_MC_SA )
        return API_ERR_INVALID_RT_SA;

    if( (hid <= 0) || (hid >= p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area) )
        return API_ERR_INVALID_HID;

    if( (sa_type != API_RT_SA_TYPE_RCV) && (sa_type != API_RT_SA_TYPE_XMT) && (sa_type != API_RT_MC_TYPE_RCV) && (sa_type != API_RT_MC_TYPE_XMT) )
        return API_ERR_INVALID_RT_SA_TYPE;

    switch(con)
    {
      case API_RT_SA_ENA_INTR_ON_XFER:
      case API_RT_SA_ENA_INTR_ON_ERR:
        if( (sa_type == API_RT_SA_TYPE_RCV) || (sa_type == API_RT_MC_TYPE_RCV) )
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_rx[ sa_mc ] = API_ON;
        if( (sa_type == API_RT_SA_TYPE_XMT) || (sa_type == API_RT_MC_TYPE_XMT) )
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_tx[ sa_mc ] = API_ON;
        api_io_rt_sa_con(p_api_dev, rt_addr, sa_mc, hid, sa_type, con, rmod /*API_RT_SA_RESP_MODE_DEF*/, smod, swm);
        break;
      case API_RT_SA_DISABLE:
      case API_RT_SA_ENABLE:
        if( (sa_type == API_RT_SA_TYPE_RCV) || (sa_type == API_RT_MC_TYPE_RCV) )
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_rx[ sa_mc ] = API_OFF;
        if( (sa_type == API_RT_SA_TYPE_XMT) || (sa_type == API_RT_MC_TYPE_XMT) )
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_int[ rt_addr ].sa_tx[ sa_mc ] = API_OFF;
        api_io_rt_sa_con(p_api_dev, rt_addr, sa_mc, hid, sa_type, con, rmod /*API_RT_SA_RESP_MODE_DEF*/, smod, swm);
        break;
      default:
          ret_val =  API_ERR_INVALID_CON;
        break;
    }

  return ret_val;

} /* end: api_rt_sa_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_SA_ERR_CON         */
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
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Subaddress/Modecode [sa_mc],                            */
/*                RT Subaddress Type [sa_type],                              */
/*                RT Subaddress Error Injection Type [type],                 */
/*                RT Error Word Position [wpos],                             */
/*                RT Error Bit Position [bpos],                              */
/*                Sync Field Error half-bit-pattern [sync],                  */
/*                Amount of Gap error half-bits [contig],                    */
/*                Amount of Bit count error half [bc_bits]                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_SA_CON_ERR' instruction to control   */
/*    the specified RT Subaddress/Modecode Error Injection of the specified  */
/*    API1553-DS RT.                                                         */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_sa_err_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_ERR_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
  BYTE rt_addr = (BYTE)in->rt_addr;
  BYTE sa_mc   = (BYTE)in->sa_mc;
  BYTE sa_type = (BYTE)in->sa_type;
  BYTE type    = (BYTE)in->type;
  BYTE wpos    = (BYTE)in->wpos;
  BYTE bpos    = (BYTE)in->bpos;
  BYTE sync    = (BYTE)in->sync;
  BYTE contig  = (BYTE)in->contig;
  BYTE bc_bits = (BYTE)in->bc_bits;

  if (! mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_ERR_INJECTION))
    return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

  if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
      return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

  if (API_DEVICE_MODE_IS_NOREPLAY_NOERROR(p_api_dev->drv_mode[p_api_dev->biu]))
      return API_ERR_ERROR_INJECTION_NOT_AVAILABLE;

  if(rt_addr >= MAX_API_RT)
      return API_ERR_INVALID_RT;

  if(sa_mc >= MAX_API_RT_MC_SA)
      return API_ERR_INVALID_RT_SA;

  if(sa_type > API_RT_SA_TYPE_ALL)
      return API_ERR_INVALID_RT_SA_TYPE;

  switch(type)
  {
    case API_ERR_PHYSICAL_ERROR_SUPPRESSION:
    case API_ERR_WCLO_EXT:
    case API_ERR_MSG_LENGTH_HI_IGNORE:
    case API_ERR_MSG_LENGTH_LO_IGNORE:
        if (! mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_AT91_RT_EXTENDED_ERR))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    default:
      break;
  }

  api_io_rt_sa_err_con(p_api_dev, rt_addr, sa_mc, sa_type, type, wpos, bpos, sync, contig, bc_bits);
  return 0;

} /* end: api_rt_sa_err_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_SA_ERR_GET         */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.06.99   Update : 10.06.99                                   */
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
L_WORD api_rt_sa_err_get( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_ERR_GET_INPUT * in, TY_MIL_COM_LS_RT_SA_ERR_GET_OUTPUT * out )
{
  WORD sa_ctrl1, index;
  L_WORD sa_ctrl2;
  BYTE err_ext, err_type, err_wno, deviation2;
  BYTE type = 0, wpos = 0, bpos = 0, sync = 0, contig = 0, bc_bits = 0; /* bytes of err_spec */

  BYTE rt_addr = (BYTE)in->rt_addr;
  BYTE sa_mc   = (BYTE)in->sa_mc;
  BYTE sa_type = (BYTE)in->sa_type;


  index = sa_mc + (MAX_API_RT_MC_SA *sa_type);
  sa_ctrl1 = (WORD) (BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].sa_ctrl) );
  sa_ctrl2 = (L_WORD) (BSWAP32( (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr) );

  err_ext = (sa_ctrl1 &0x7E00) >> 9;
  err_type = (sa_ctrl1 &0x01C0) >> 6;
  err_wno = (sa_ctrl1 &0x003F);

  deviation2 = (sa_ctrl2 & 0x03C00000) >> 26;

  wpos = err_wno;

  switch(err_type)
  {
    case 0:
       /*no err*/
      switch(err_ext)
      {
      case 1:
        type = API_ERR_WCHI;
        break;
      case 2:
        type = API_ERR_WCLO;
        break;
      case 3:
        type = API_ERR_ALTBUS;
        break;
      default:
        type = API_ERR_NONE;
        wpos = 0;
        break;
      }
      break;
    case 1:
       /*parity*/
      type = API_ERR_PARITY;
      break;
    case 2:
       /*bit count*/
      if(err_ext &0x20)
        type = API_ERR_BCHI;
      else
        type = API_ERR_BCLO;

      bc_bits = (err_ext &0x6) >> 1; /* get only the number of error bits (1..3) */
      /* the information starts at bit 10 and not at bit 9... */
      break;
    case 3:
    case 6:
       /*zero crossing high deviation*/
      if(3 == err_type )
        type = API_ERR_ZXING_POS;
      else
        type = API_ERR_ZXING_NEG;
      wpos = err_wno - 1;
      if(err_ext &0x20)
      {
        contig = 1;
        err_ext &= 0x1F;
      }
      bpos = err_ext + 1;

      /* only AyX and AMC boards are supported for high resolution zero crossing deviation */
      if( mil_hw_is_high_res_zero_crossing_supported(p_api_dev) )
        contig += deviation2*2;
      break;
    case 4:
       /*gap error*/
      type = API_ERR_CONTIG;
      contig = err_ext;
      break;
    case 5:
       /*manchester*/
      if(err_ext &0x20)
        type = API_ERR_MANHI;
      else
        type = API_ERR_MANLO;

      err_ext &= 0x1f;
      /* bpos 4..19  FW swaps the bits (e.g. Bit Pos 4 / Physical Manchester Err at bit 19) */
      bpos = 23-err_ext - 4;
      break;
    case 7:
       /*sync field*/
      if(0 == wpos)
        type = API_ERR_CSYNC;
      else
        type = API_ERR_DSYNC;
      sync = err_ext;
      break;
  }



  out->type    = type;
  out->wpos    = wpos;
  out->bpos    = bpos;
  out->sync    = sync;
  out->contig  = contig;
  out->bc_bits = bc_bits;

  return 0;

} /* end: api_rt_sa_err_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_DYTAG_DEF          */
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
L_WORD api_rt_dytag_def( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DYTAG_DEF_INPUT  * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;
  WORD   rt_hid  = (WORD)in->rt_hid;
  WORD   con     = (WORD)in->con;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_EVENT_QUEUE))
      return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

  if(rt_hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_eq_area)
  {
    if(con == API_RT_DYTAG_ENA)
    {
      /* Enable Dynamic Data Function */
      if(p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena == API_OFF)
      {
        p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena = API_ON;

        /* Enable Event Queue processing */
        api_io_rt_bh_eq_con(p_api_dev, API_ON, rt_hid);

        /* Init Event Queue Descriptor */
        api_io_rt_dytag_def(p_api_dev, 
                            rt_hid, 
                            (WORD)in->mode, 
                            (WORD)in->tag_fct[0], 
                            (WORD)in->min[0],
                            (WORD)in->max[0],
                            (WORD)in->step[0], 
                            (WORD)in->wpos[0], 
                            (WORD)in->tag_fct[1],
                            (WORD)in->min[1], 
                            (WORD)in->max[1], 
                            (WORD)in->step[1], 
                            (WORD)in->wpos[1], 
                            (WORD)in->tag_fct[2], 
                            (WORD)in->min[2], 
                            (WORD)in->max[2], 
                            (WORD)in->step[2], 
                            (WORD)in->wpos[2], 
                            (WORD)in->tag_fct[3], 
                            (WORD)in->min[3], 
                            (WORD)in->max[3], 
                            (WORD)in->step[3], 
                            (WORD)in->wpos[3]);
      }
    }
    else
    {
      /* Disable Dynamic Data Function */
      if(p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena == API_ON)
      {
        p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ rt_hid ].ena = API_OFF;

        /* Disable Event Queue processing */
        api_io_rt_bh_eq_con(p_api_dev, API_OFF, rt_hid);

        /* Reset Event Queue Descriptor */
        api_io_rt_eq_ini(p_api_dev, rt_hid);
      }
    }
  }
  else
    ret_val = API_ERR_INVALID_HID;

  return (ret_val);

} /* end: api_rt_dytag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_DYTAG_DEF_GET      */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
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
L_WORD api_rt_dytag_get( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DYTAG_GET_INPUT * in, TY_MIL_COM_LS_RT_DYTAG_GET_OUTPUT * out )
{
  L_WORD w1, w2, w3, w4;
  WORD opc, func, wpos, fdw;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_EVENT_QUEUE))
      return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

  /* get RT Event Queue Descriptor */
  w1 = BSWAP32( (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ in->rt_hid ].con1);
  w2 = BSWAP32( (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ in->rt_hid ].addr1);
  w3 = BSWAP32( (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ in->rt_hid ].con2);
  w4 = BSWAP32( (*p_api_dev->rt_eq_desc_p[ p_api_dev->biu ])[ in->rt_hid ].addr2);

  out->mode = (WORD) ( (w1 &0xc0000000) >> 30);

  if(out->mode == 0)
  {
    out->tag_fct[0] = (WORD) ( (w1 &0x38000000) >> 27); /* tag_fct */
    out->min[0]     = (WORD) (  w2 &0x0000FFFF);        /* min */
    out->max[0]     = (WORD) ( (w2 &0xFFFF0000) >> 16); /* max */
    out->step[0]    = (WORD) (  w1 &0x0000FFFF);        /* step */
    out->wpos[0]    = (WORD) ( (w1 &0x001F0000) >> 16); /* wpos */

    out->tag_fct[1] = (WORD) ( (w3 &0x38000000) >> 27); /* tag_fct */
    out->min[1]     = (WORD) (  w4 &0x0000FFFF);        /* min */
    out->max[1]     = (WORD) ( (w4 &0xFFFF0000) >> 16); /* max */
    out->step[1]    = (WORD) (  w3 &0x0000FFFF);        /* step */
    out->wpos[1]    = (WORD) ( (w3 &0x001F0000) >> 16); /* wpos */
  }
  else
  {
    opc  = (WORD) ( (w1 &0x30000000) >> 28);
    func = (WORD) ( (w1 &0x07000000) >> 24);
    wpos = (WORD) ( (w1 &0x001F0000) >> 16);
    fdw  = (WORD) (  w1 &0x0000ffff);
    out->tag_fct[0] = func + 1+(opc << 7); /* tag_fct */
    out->min[0]     = fdw;  /* min */
    out->max[0]     = 0;    /* max */
    out->step[0]    = 0;    /* step */
    out->wpos[0]    = wpos; /* wpos */

    opc  = (WORD) ( (w2 &0x30000000) >> 28);
    func = (WORD) ( (w2 &0x07000000) >> 24);
    wpos = (WORD) ( (w2 &0x001F0000) >> 16);
    fdw  = (WORD) (  w2 &0x0000ffff);
    out->tag_fct[1] = func + 1+(opc << 7); /* tag_fct */
    out->min[1]     = fdw;  /* min */
    out->max[1]     = 0;    /* max */
    out->step[1]    = 0;    /* step */
    out->wpos[1]    = wpos; /* wpos */

    opc  = (WORD) ( (w3 &0x30000000) >> 28);
    func = (WORD) ( (w3 &0x07000000) >> 24);
    wpos = (WORD) ( (w3 &0x001F0000) >> 16);
    fdw  = (WORD) (  w3 &0x0000ffff);
    out->tag_fct[2] = func + 1+(opc << 7); /* tag_fct */
    out->min[2]     = fdw; /* min */
    out->max[2]     = 0; /* max */
    out->step[2]    = 0; /* step */
    out->wpos[2]    = wpos; /* wpos */

    opc  = (WORD) ( (w4 &0x30000000) >> 28);
    func = (WORD) ( (w4 &0x07000000) >> 24);
    wpos = (WORD) ( (w4 &0x001F0000) >> 16);
    fdw  = (WORD) (  w4 &0x0000ffff);
    out->tag_fct[3] = func + 1+(opc << 7); /* tag_fct */
    out->min[3]     = fdw; /* min */
    out->max[3]     = 0; /* max */
    out->step[3]    = 0; /* step */
    out->wpos[3]    = wpos; /* wpos */
  }

  return 0;

} /* end: api_rt_dytag_def_get */



L_WORD api_rt_start( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if( api_opr_sf(p_api_dev, in->ulCommand, 0, 0) != 0 )
            return API_ERR_RT_NOT_AVAILABLE;

    return api_rt_mode( p_api_dev, API_BUSY );
} /* end: api_rt_start */



L_WORD api_rt_halt( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
        if( api_opr_sf(p_api_dev, in->ulCommand, 0, 0) != 0 )
            return API_ERR_RT_NOT_AVAILABLE;

    return api_rt_mode( p_api_dev, API_HALT );
} /* end: api_rt_stop */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_MODE               */
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
/*    Inputs    : RT Control (START/HALT) [con]                              */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_START' and 'API_RT_HALT' instruction */
/*    to control the dynamic operations (START / STOP) of the API1553-DS RT. */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_mode( TY_API_DEV *p_api_dev, BYTE con )
{
  L_WORD ret_val = 0;
  BYTE   i       = 0;

  if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
    api39_rt_mode(p_api_dev, con);


  if(con == API_BUSY)
  {

    /* Clear Global RT Message and Error Counter */
    api_io_rt_glb_cnt_ini(p_api_dev);

    /* Clear RT Message and Error Counter locations */
    for(i = 0; i < MAX_API_RT; i++)
      api_io_rt_msg_cnt_ini(p_api_dev, i);

    api_io_mode_con(p_api_dev, API_RT_MODE, API_ON);
    p_api_dev->rt_status[ p_api_dev->biu ] = API_BUSY;
  }
  else
  {
    api_io_mode_con(p_api_dev, API_RT_MODE, API_OFF);
    p_api_dev->rt_status[ p_api_dev->biu ] = API_HALT;
  }

  return (ret_val);

} /* end: api_rt_mode */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_STATUS_READ        */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_STATUS_READ' instruction to read the */
/*    API1553-DS RT mode status and message/error counter information.       */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_status_read( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_RT_STATUS_READ_OUTPUT * out )
{

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc), In);
      UsbSynchronizeLWord(p_api_dev, 
          API_GLB_MEM_ADDR_REL(&(*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec), In);
  }

  /* RT Message Counter */
  out->msg_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_mc);
  
  /* RT Error Counter */
  out->err_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).rt_ec);

  out->dsp     = p_api_dev->rt_status[ p_api_dev->biu ];

  return 0;

} /* end: api_rt_status_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_MSG_READ           */
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
/*    Inputs    : RT Address [rt_addr]                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_MSG_READ' instruction to read the    */
/*    message status information of the specified API1553-DS RT.             */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_msg_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_MSG_READ_INPUT * in, TY_MIL_COM_LS_RT_MSG_READ_OUTPUT * out )
{
  L_WORD ret_val = 0;
  BYTE   rt_addr = (BYTE)in->rt_addr;

  if(rt_addr < MAX_API_RT)
  {
    if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
    {
      UsbSynchronizeRTDescriptor(p_api_dev, (unsigned char) p_api_dev->biu, rt_addr, In);
    }

    /* RT Message Counter */
    out->msg_cnt = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].msg_cnt);
   
    /* RT Error Counter */
    out->err_cnt = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].err_cnt);

    /* RT Status */
    out->nxw     = LO_WORD(BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl) );
    out->lsw     = LO_WORD(BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw) );
    out->lcw     = HI_WORD(BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].lcw_lsw) );

  }
  else
    ret_val = API_ERR_INVALID_RT;

  return (ret_val);

} /* end: api_rt_msg_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_MSG_READ_ALL       */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_MSG_READ_ALL' instruction to read    */
/*    the transfer/error count information of all RTs.                       */
/*                                                                           */
/*****************************************************************************/

L_WORD api_rt_msg_read_all( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_RT_MSG_READ_ALL_OUTPUT * out )
{
  BYTE i;

  if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
  {
      UsbSynchronizeRTDescriptorArea(p_api_dev, p_api_dev->biu, In);
  }

  for(i = 0; i < MAX_API_RT; i++)
  {
      /* Message Counter */
      out->msg_cnt[i] = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ i ].msg_cnt);
      /* Error Counter */
      out->err_cnt[i] = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ i ].err_cnt);
  }

  return 0;

} /* end: api_rt_msg_read_all */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_SA_MSG_READ        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Address [rt_addr],                                      */
/*                RT Subaddress/Modecode [sa_mc],                            */
/*                RT Subaddress Type [sa_type],                              */
/*                Reset Buffer Status Bits after Command call [clr_mode]     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_SA_MSG_READ' instruction to read     */
/*    the RT Subaddress/Modecode message status information of the specified */
/*    API1553-DS RT/SA.                                                      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_sa_msg_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_MSG_READ_INPUT* in, TY_MIL_COM_LS_RT_SA_MSG_READ_OUTPUT * out )
{
  BYTE rt_addr  = (BYTE)in->rt_addr;
  BYTE sa_mc    = (BYTE)in->sa_mc;
  BYTE sa_type  = (BYTE)in->sa_type;
  BYTE clr_mode = (BYTE)in->clr_mode;

  WORD index = 0;
  WORD hid   = 0;
  WORD sqid  = 0;

  L_WORD ret_val             = 0;
  L_WORD addr                = 0;
  L_WORD ul_SqAddr           = 0;
  L_WORD ul_CurrentBufIndex  = 0;
  L_WORD ul_Temp             = 0;
  L_WORD sq_mode             = 0; /* Status Queue Mode */
  L_WORD bq_mode             = 0; /* Buffer Queue Mode */
  L_WORD ul_qsize            = 0; /* Queue Size */

  if(rt_addr < MAX_API_RT)
  {

    /* Calculate Buffer Header ID */
    index = sa_mc + (MAX_API_RT_MC_SA *sa_type);

    addr = (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt_addr ].sa_mc[ index ].bhd_ptr;

    addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, addr & BSWAP32(0x03fffffful) );

    if( addr < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area )
    {
      SET_NAK_INFO(API_ERR_ADDRESS_OUT_OF_RANGE);
    }
    addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area;

    hid = (WORD) (addr / MAX_API_RT_BH_DESC_SIZE);

    ul_Temp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl);
    ul_qsize = ((ul_Temp >> 14) & 0xF);     
    ul_qsize = 1<<ul_qsize; /* Buffer Queue Size, power of 2 */

    /* Synchronization of Buffer Header Control word is only necessary for queue size > 1
       Otherwise the current buffer index will not change */
    if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY && ul_qsize > 1)
    {
        UsbSynchronizeLWord(p_api_dev, 
                            API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl), 
                            In);
    }

    sq_mode = (ul_Temp >> 12) & 0x3L; /* check if status queue enabled */
    bq_mode  = (ul_Temp >> 28) & 0x3L; /* Buffer Queue Mode */
    ul_SqAddr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);

    ul_CurrentBufIndex = (*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[hid].bh_ctrl & 0x000000FF;

    /* Set Pointer to Status Entry */
    switch (bq_mode)
    {
    case API_RT_BH_BQM_CYCLIC:
        /* Set Pointer to last used Status Entry */
        if (ul_CurrentBufIndex == 0L)
            ul_CurrentBufIndex = ul_qsize - 1;
        else
            ul_CurrentBufIndex--;
        break;
    case API_RT_BH_BQM_ONCE:
        /* Set Pointer to last used Status Entry */
        if (ul_CurrentBufIndex == 0L)
        {
            /* Empty, not received anythin yet */
            /* If nothing has been received we set the current index to the end of the queue to be compliant with the cyclic mode.*/
            ul_CurrentBufIndex = ul_qsize - 1; 
        }
        else
        {
            // LIF (Last Index Flag) was introduced in FW2.71
            if((*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[hid].bh_ctrl & (1<<23) )
                ul_CurrentBufIndex = ul_CurrentBufIndex; /* Last Index Flag is set the last index in the queue is already updated */
            else
                ul_CurrentBufIndex--; /* Last Index Flag is not set the last index in the queue is updated next */
        }
        break;
    default:
    case API_RT_BH_BQM_HOST:
        /* Use Index as is */
        break;
    }

    if (sq_mode == API_RT_BH_STS_QSIZE_SQ)
    {
      sqid = (WORD)( ((ul_SqAddr + ul_CurrentBufIndex*MAX_API_RT_SQ_DESC_SIZE) - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sq_area) / MAX_API_RT_SQ_DESC_SIZE );
    }
    else
    {
      sqid = (WORD)( (ul_SqAddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sq_area) / MAX_API_RT_SQ_DESC_SIZE );
    }



    if(hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area)
    {

      /* Read from RT Buffer Header */
      addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
      addr -= p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
      addr /= MAX_API_BUF_SIZE;
      out->bid = (WORD)(ul_CurrentBufIndex + addr);

      if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
      {
          UsbSynchronizeMemoryArea(p_api_dev, 
              API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_sq_desc_p[p_api_dev->biu])[sqid]), 
              MAX_API_RT_SQ_DESC_SIZE, In);
      }

      /* Read from Status Queue Entry */
      out->lcw = HI_WORD(BSWAP32( (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].lcw_lsw) );
      out->lsw = LO_WORD(BSWAP32( (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].lcw_lsw) );
      out->buf = BSWAP32( (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].act_buf_ptr);
      out->ttag = BSWAP32( (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].time_tag);
      out->brw = HI_WORD(BSWAP32( (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl) );

      /* Set Buffer Status Bits to Buffer Empty state */
      /* 01.05.99 */
      if(clr_mode != API_DONT_MODIFY_STATUS_BITS)
      {
        BYTE bResetErr = 0;
        BYTE bResetStat = 0;
        
        if(API_RESET_STATUS_BITS == (clr_mode & API_RESET_STATUS_BITS) )
          bResetStat = 1;
        if(API_RESET_ERROR_BITS == (clr_mode & API_RESET_ERROR_BITS) )
          bResetErr = 1;

        if(bResetStat != 0)
        {
          if( (out->brw &0x1000) == 0x1000)
          /* clear Buffer Full Bit */
            (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xEFFFFFFFL);
          if( (out->brw &0x2000) == 0x2000)
          /* clear Buffer Empty Bit */
            (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xDFFFFFFFL);
        }
        if(bResetErr != 0)
        {
          if( (out->brw &0x000F) == 0)
          /* clear Buffer Empty Bit */
            (*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl &= BSWAP32(0xFFF0FFFFL);
        }

        if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, 
                API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_sq_desc_p[ p_api_dev->biu ])[ sqid ].sq_ctrl), 
                Out);
        }
      }
    }
    else
      ret_val = API_ERR_INVALID_HID;
  }
  else
    ret_val = API_ERR_INVALID_RT;

  return (ret_val);

} /* end: api_rt_sa_msg_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_GLOBAL_CON         */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.06.99   Update : 10.06.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT setup array count [count]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_GLOBAL_CON' instruction to setup     */
/*    all RT's.                                                              */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_global_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_GLOBAL_CON_INPUT* in, TY_MIL_COM_ACK * out )
{
  WORD volatile *dest_ptr;
  BYTE volatile *b_addr;
  BYTE retfl, i;
  WORD k;
  L_WORD tmpl, addr;
  L_WORD ret_val = 0;

  WORD count = (WORD)in->count;

  BYTE mode; /* usage control */
  BYTE rt_addr; /* Remote Terminal */
  BYTE rt_con; /* Remote Terminal Operation Control*/
  BYTE sa_mc; /* Subaddress/Modecode */
  BYTE sa_type; /* Subaddress Type */
  BYTE sa_con; /* Subaddress/Modecode Control */
  BYTE resp_time; /* RT response time */
  WORD nxw; /* Next status word */
  BYTE smod; /* Status Word Mask control */
  WORD swm; /* Status Word Modification Mask */
  WORD hid; /* Buffer Header ID */
  WORD bid; /* Data Buffer ID */

  TY_MIL_COM_LS_RT_BH_DEF_INPUT  rt_bh_def_in;
  TY_MIL_COM_LS_RT_BH_DEF_OUTPUT rt_bh_def_out;
  TY_MIL_COM_LS_BUF_DEF_INPUT    buf_def_in;
  b_addr = 0L;

  /* Set Pointers */
  dest_ptr = (WORD*)API_REC_SHARED_MEM_START_ADDR; /* Shared RAM */

  for(k = 0; k < count; k++)
  {
    /* get data from replay memory */
    for(i = 0; i < 32; i++)
    {
      buf_def_in.data[i] = *dest_ptr;
      dest_ptr++;
    }
    b_addr = (BYTE*)dest_ptr;

    mode = *(b_addr + 0);
    rt_addr = *(b_addr + 1);
    rt_con = *(b_addr + 2);
    sa_mc = *(b_addr + 3);
    sa_type = *(b_addr + 4);
    sa_con = *(b_addr + 5);
    resp_time = *(b_addr + 6);

    smod = *(b_addr + 7);

    dest_ptr = (WORD*) (b_addr + 8);

    nxw = *(dest_ptr + 0);
    swm = *(dest_ptr + 1);
    hid = *(dest_ptr + 2);
    bid = *(dest_ptr + 3);

    switch(mode)
    {
      case 0:
         /* RT Ini mode */
        /* Read RT Descriptor Control Word */
        tmpl = BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt_addr ].rt_ctrl);

        switch(rt_con)
        {
        case 0:
           /* Disable RT */
          if( (tmpl &0x08000000L) == 0x08000000L)
          {

            ret_val = api_rt_ini(p_api_dev, rt_addr, 0 /*rt_con*/, API_RT_RESP_BOTH /*bus*/, resp_time, nxw);
          }
          break;

        case 1:
           /* Enable RT */
          if( (tmpl &0x08000000L) == 0L)
          {

            ret_val = api_rt_ini(p_api_dev, rt_addr, 1 /*rt_con*/, API_RT_RESP_BOTH /*bus*/, resp_time, nxw);

          }
          break;

        default:
          ret_val = API_ERR_INVALID_CON;
          return (ret_val);
        }
        break;

      case 1:
         /* Subaddress and buffer initialization */

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

        ret_val = api_rt_bhd_def( p_api_dev, &rt_bh_def_in, &rt_bh_def_out );

        if(ret_val != 0)
        {
          return (ret_val);
        }

        buf_def_in.bt     = API_BUF_RT_TYPE;
        buf_def_in.hid    = 0;
        buf_def_in.bid    = bid;
        buf_def_in.count  = 32;

        ret_val = api_buf_def(p_api_dev, &buf_def_in, NULL );

        if(ret_val != 0)
        {
          return ret_val;
        }

        ret_val = api_rt_sa_con(p_api_dev, rt_addr, sa_mc, hid, sa_type, sa_con, 0 /*rmod*/, smod, swm);

        break;

      case 2:
         /* Update transmit buffer */

        if( (sa_type == API_RT_SA_TYPE_XMT) || (sa_type == API_RT_MC_TYPE_XMT) )
        {
          /* Use absolute Buffer ID */
          addr = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
          addr += (bid *MAX_API_BUF_SIZE);
          addr = API_GLB_MEM_ADDR_REL(addr);

          retfl = api_buf_ram_write(p_api_dev, addr, 32, (BYTE*)buf_def_in.data /* @AIMFIXME: function needs to be adapted for this to work*/ );

          if(retfl != API_ACK)
          {
            ret_val = API_ERR;
          }
        }

        break;
      default:
        ret_val = API_ERR_INVALID_MODE;
        return (ret_val);
    }
    dest_ptr += 4;
  }
  return ret_val;

} /* end: api_rt_global_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_GET_SIM_INFO       */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.06.99   Update : 10.06.99                                   */
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
L_WORD api_rt_get_sim_info( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_GET_SIM_INFO_INPUT * in, TY_MIL_COM_LS_RT_GET_SIM_INFO_OUTPUT * out )
{
  BYTE mode;
  L_WORD RxSA, TxSA, RxMC, TxMC;
  L_WORD RxMID[ 8 ], TxMID[ 8 ], HSMC;
  L_WORD RT_SA_Base_Ptr;
  BYTE i;
  L_WORD RT_MID_Base_Ptr;

  BYTE rt = (BYTE)in->rt;

  mode = (BYTE) ( (BSWAP32( (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt ].rt_ctrl) & 0x38000000) >> 27);

  RT_SA_Base_Ptr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ rt ].sa_desc);
  RxSA = TxSA = RxMC = TxMC = HSMC = 0;
  for(i = 0; i < 8; i++)
    TxMID[ i ] = RxMID[ i ] = 0;

  if(mode != 2)
  {
    for(i = 0; i < MAX_API_RT_MC_SA; i++)
    {
      if( (i>0) && (i<31) )
      {
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8) & BSWAP32(0xc0000000) )
          RxSA |= (1 << i);
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x100) & BSWAP32(0xc0000000) )
          TxSA |= (1 << i);
      }
      if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x200) & BSWAP32(0xc0000000) )
        RxMC |= (1 << i);
      if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x300) & BSWAP32(0xc0000000) )
        TxMC |= (1 << i);
    }
  }
  else
  {
    /* As a FW-Workaround all SA are enabled, but the pointers are default. So we take care of it at this point. */
    for(i = 0; i < MAX_API_RT_MC_SA; i++)
    {
      if( (i>0) && (i<31) )
      {
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+4) != BSWAP32(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) )
          RxSA |= (1 << i);
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x100 + 4) != BSWAP32(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) )
          TxSA |= (1 << i);
      }
      if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x200 + 4) != BSWAP32(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) )
        RxMC |= (1 << i);
      if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_SA_Base_Ptr + i * 8+0x300 + 4) != BSWAP32(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) )
        TxMC |= (1 << i);
    }
  }

  if(ul_StreamIsHS( p_api_dev, p_api_dev->biu))
  {
    RT_MID_Base_Ptr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*api39_rt_desc_p[ p_api_dev->hs_stream])[ rt ].ul_MidDesc);

    for(i = 0; i < 128; i++)
    {
      if( i>0 )
      {
        if( (*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_MID_Base_Ptr + i * 8) ) & BSWAP32(0xc0000000) )
          RxMID[ (i / 32) ] |= (1 << (i % 32) );
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_MID_Base_Ptr + i * 8+0x400) & BSWAP32(0xc0000000) )
          TxMID[ (i / 32) ] |= (1 << (i % 32) );
      }

      if(i < 32)
      {
        if(*(L_WORD*)API_GLB_MEM_ADDR_ABS(RT_MID_Base_Ptr + i * 8+0x800) & BSWAP32(0xc0000000) )
          HSMC |= (1 << i);
      }
    }
  }

  out->mode  = mode;
  out->rx_sa = RxSA;
  out->tx_sa = TxSA;
  out->rx_mc = RxMC;
  out->tx_mc = TxMC;

  for(i = 0; i < 8; i++)
  {
    out->rx_mid[i] = RxMID[ i ];
    out->tx_mid[i] = TxMID[ i ];
  }

  out->hs_mc = HSMC;

  return 0;

} /* end: api_rt_get_sim_info */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_BHD_READ           */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.06.99   Update : 04.06.07                                   */
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

L_WORD api_rt_bhd_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_BH_GET_INPUT * in, TY_MIL_COM_LS_RT_BH_DEF_OUTPUT * out )
{
    L_WORD ret_val = 0;
    L_WORD tmpl    = 0;
    WORD   hid     = (WORD)in->hid;

    if( (hid > 0) && (hid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area) )
    {
        out->hid_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area + (hid * MAX_API_RT_BH_DESC_SIZE);
        out->bid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
        out->sid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].sq_addr);
        out->eid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].eq_addr);

        tmpl       = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl );
        out->bid   = (WORD)( ( out->bid_addr - p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ] )   / MAX_API_BUF_SIZE        );
        out->sid   = (WORD)( ( out->sid_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_sq_area ) / MAX_API_RT_SQ_DESC_SIZE );
        out->eid   = (WORD)( ( out->eid_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_eq_area ) / MAX_API_RT_EQ_DESC_SIZE );
        out->nbufs = 0x1L << ((tmpl >> 14) & 0xF); /* Buffer Queue Size, multiples of 2 */
    }
    else
    {
        ret_val = API_ERR_INVALID_HID;
    }

    return (ret_val);

} /* end: api_rt_bhd_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_SA_BH_GET          */
/*                                                                           */
/*   Author : Jochen Pfaadt            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 10.06.99   Update : 10.06.99                                   */
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
L_WORD api_rt_sa_bh_get( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_SA_BH_GET_INPUT  * in, TY_MIL_COM_LS_RT_SA_BH_GET_OUTPUT * out )
{
  L_WORD index    = 0;

  BYTE rt      = (BYTE)in->rt;
  BYTE sa_type = (BYTE)in->sa_type;
  BYTE sa_mc   = (BYTE)in->sa_mc;

  index = sa_mc + ( (MAX_API_RT_MC_SA) *sa_type);

  out->hid_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
  out->hid      = (WORD) ( (out->hid_addr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area) / MAX_API_RT_BH_DESC_SIZE);

  return 0;

} /* end: api_rt_sa_bh_get */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_MODE_CTRL          */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 19.02.07   Update : -                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Mode [ul_RtMode]                                        */
/*                RT Mode Ctrl [ul_Ctrl]                                     */
/*                Param1 [ul_Param1]                                         */
/*                Param2 [ul_Param2]                                         */
/*                Param3 [ul_Param3]                                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_MODE_CTRL' instruction to control    */
/*    some RT modes on the fly.                                              */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_mode_ctrl( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_MODE_CTRL_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;
  L_WORD ul_Hid;
  L_WORD ul_RtAddr;
  L_WORD ul_RtDytagStatus;
  L_WORD ul_NewMode;
  L_WORD ul_Tmp;

  switch(in->rt_mode)
  {
    case API_RT_MODE_LANE_CTRL:
       /* Read & Modify the RT Descriptor Control Word */

       ul_Tmp = BSWAP32((*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ in->param1 ].rt_ctrl);

       ul_Tmp &= 0xF9FFFFFFL; /* Bus Response Control field */
       switch (in->param2) {
         case API_RT_RSP_PRI_BUS:
              ul_Tmp |= 0x02000000L; /* respond to Command Words from PRI only */
              break;
         case API_RT_RSP_SEC_BUS:
              ul_Tmp |= 0x04000000L; /* respond to Command Words from SEC only */
              break;
         default:
              break;   
       }
       (*p_api_dev->rt_desc_p[ p_api_dev->biu ])[ in->param1 ].rt_ctrl = BSWAP32(ul_Tmp);
       break;
  
    case API_RT_MODE_CONFIGURED_DYTAGS:
      if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_EVENT_QUEUE))
          return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

      for( ul_Hid=0; ul_Hid<(MAX_API_RT_HID * MAX_MEM_MUL); ul_Hid++ )
      {
        if( 0xFFFFFFFF == in->param1 ) /* control all RTs */
        {
          for( ul_RtAddr=0; ul_RtAddr<MAX_API_RT; ul_RtAddr++ )
          {
            ul_RtDytagStatus = (p_api_dev->rt_dytag_status[ p_api_dev->biu ][ul_Hid] >> ul_RtAddr) & 1;
            if( API_ON == ul_RtDytagStatus )
            {
              if (API_ON == in->rt_ctrl) /* Enable Event Queue processing */
                api_io_rt_bh_eq_con(p_api_dev, API_ON, ul_Hid);
              else /* Disable Event Queue processing */
                api_io_rt_bh_eq_con(p_api_dev, API_OFF, ul_Hid);
            }
          }
        }
        else /* control specific RT only */
        {
          ul_RtDytagStatus = (p_api_dev->rt_dytag_status[ p_api_dev->biu ][ul_Hid] >> in->param1) & 1;
          if( API_ON == ul_RtDytagStatus )
          {
            if (API_ON == in->rt_ctrl) /* Enable Event Queue processing */
              api_io_rt_bh_eq_con(p_api_dev, API_ON, ul_Hid);
            else /* Disable Event Queue processing */
              api_io_rt_bh_eq_con(p_api_dev, API_OFF, ul_Hid);
          }
        }
      }
      break;

    case API_RT_MODE_OPERATION_CTRL:
      ul_RtAddr = in->param1;
      ul_NewMode = in->param2;

      /* it is mandatory to switch off the RT before changing the mode */
      api_io_rt_ena_dis(p_api_dev, (BYTE)ul_RtAddr, API_OFF);
      if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
          api39_io_rt_ena_dis(p_api_dev, (BYTE)ul_RtAddr, API_OFF);

      if( (ul_NewMode == API_RT_ENABLE) || (ul_NewMode == API_RT_MAILBOX) )
      {
          if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
              api39_io_rt_ena_dis_set_operation(p_api_dev, (BYTE)ul_RtAddr, API_ON, (BYTE)ul_NewMode);
          api_io_rt_ena_dis_set_operation(p_api_dev, (BYTE)ul_RtAddr, API_ON, (BYTE)ul_NewMode);
      }
      break;

    default:
      ret_val = API_ERR_INVALID_MODE;
      break;
  }

  return ret_val;

} /* end: api_rt_mode_ctrl */


L_WORD api_rt_dwc_set(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DWC_SET_INPUT * in, TY_MIL_COM_ACK * out)
{
    L_WORD ret_val = 0;

    BYTE rt_addr = (BYTE)in->rt_addr;
    BYTE sa_mc = (BYTE)in->sa_mc;
    BYTE sa_type = (BYTE)in->sa_type;
    BYTE dwc = (BYTE)in->word_cnt;

    /* functionality only available on embedded boards */
    if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
    {
        /* LS RT handling */
        if (rt_addr >= MAX_API_RT)
            return API_ERR_INVALID_RT;

        if (sa_mc >= MAX_API_RT_MC_SA)
            return API_ERR_INVALID_RT_SA;

        if ((sa_type != API_RT_SA_TYPE_RCV) && (sa_type != API_RT_SA_TYPE_XMT))
            return API_ERR_INVALID_RT_SA_TYPE;

        api_io_rt_dwc_set(p_api_dev, rt_addr, sa_mc, sa_type, dwc);
    }
    else
        ret_val = API_ERR_DWC_NOT_AVAILABLE;

    return ret_val;
}

L_WORD api_rt_dwc_get(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_RT_DWC_GET_INPUT * in, TY_MIL_COM_LS_RT_DWC_GET_OUTPUT * out)
{
    L_WORD ret_val = 0;

    BYTE rt_addr = (BYTE)in->rt_addr;
    BYTE sa_mc = (BYTE)in->sa_mc;
    BYTE sa_type = (BYTE)in->sa_type;

    /* functionality only available on embedded boards */
    if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
    {
        /* LS RT handling */
        if (rt_addr >= MAX_API_RT)
            return API_ERR_INVALID_RT;

        if (sa_mc >= MAX_API_RT_MC_SA)
            return API_ERR_INVALID_RT_SA;

        if ((sa_type != API_RT_SA_TYPE_RCV) && (sa_type != API_RT_SA_TYPE_XMT))
            return API_ERR_INVALID_RT_SA_TYPE;

        out->word_cnt = api_io_rt_dwc_get(p_api_dev, rt_addr, sa_mc, sa_type);
    }
    else
        ret_val = API_ERR_DWC_NOT_AVAILABLE;

    return ret_val;
}

#endif

NAMESPACE1553_END

