/* SPDX-FileCopyrightText: 2003-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_cal.c
MIL-Bus Control and Calibration module of the AYI1553ASP Driver 
Software. This module comprises the handler routines for    
the API1553-DS MIL-Bus Control and Calibration instructions.
*/

#include "mil_tsw_includes.h"

NAMESPACE1553_START




TY_MIL_COM_TABLE_ENTRY axTargetLssysCalCommands[] = {
   /* LS RT Functions */
  {MIL_COM_LS_SYS_CAL_SIG_CON,  (TY_MIL_COM_FUNC_PTR)api_cal_sig_con,         "api_cal_sig_con",        sizeof(TY_MIL_COM_LS_SYS_CAL_CON_INPUT),       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_SYS_CAL_CPL_CON,  (TY_MIL_COM_FUNC_PTR)api_cal_cpl_con_cmd,     "api_cal_cpl_con_cmd",    sizeof(TY_MIL_COM_LS_SYS_CAL_CON_INPUT),       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_SYS_CAL_XMT_CON,  (TY_MIL_COM_FUNC_PTR)api_cal_xmt_con,         "api_cal_xmt_con",        sizeof(TY_MIL_COM_LS_SYS_CAL_CON_INPUT),       sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_SYS_CAL_TRANS,    (TY_MIL_COM_FUNC_PTR)api_cal_trans_con,       "api_cal_trans_con",      sizeof(TY_MIL_COM_LS_SYS_CAL_TRANS_INPUT),     sizeof(TY_MIL_COM_ACK)  },
};


void api_ls_sys_cal_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLssysCalCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLssysCalCommands;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_CAL                  Submodule : API_CAL_SIG_CON           */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_CAL_SIG_CON' instruction to control     */
/*    the Calibration mode of the specified API1553-DS MILbus channel.       */
/*                                                                           */
/*****************************************************************************/
L_WORD api_cal_sig_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;

  BYTE bus = (BYTE)in->bus;
  BYTE con = (BYTE)in->con;

  if( (bus == API_CAL_PRI_BUS) || (bus == API_CAL_SEC_BUS) )
  {
    switch(con)
    {
      case API_CAL_SIG_DIS:
      case API_CAL_SIG_ENA:
        api_io_cal_sig_con(p_api_dev, bus, con);
        break;
      default:
        ret_val = API_ERR_INVALID_CON;
        break;
    }
  }
  else
    ret_val = API_ERR_INVALID_BUS;

  return ret_val;

} /* end: api_cal_sig_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_CAL                  Submodule : API_CAL_CPL_CON           */
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
/*                Bus Coupling mode control [cpl]                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_CAL_CPL_CON' instruction to control     */
/*    the Bus Coupling mode of the specified API1553-DS MILbus channel.      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_cal_cpl_con_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
    return api_cal_cpl_con(p_api_dev, (BYTE)in->bus, (BYTE)in->con);
}

L_WORD api_cal_cpl_con(TY_API_DEV *p_api_dev, BYTE bus, BYTE cpl)
{
    L_WORD ulAvailable = 0;

    /* -- check input parameters --- */

    if( (bus != API_CAL_PRI_BUS) && (bus != API_CAL_SEC_BUS) )
    {
        return API_ERR_PARAM3_NOT_IN_RANGE;
    }

    /* -- use digital wrap if isolated is not available  --- */

    if(cpl == API_CAL_CPL_ISO)
    {
        if( p_api_dev->x_CouplingCapabilities.b.b_Isolated == AiFalse )
            cpl = API_CAL_CPL_WRAP;
    }


    /* -- check if the selected mode is available for this device --- */

    switch(cpl)
    {
        /* check if coupling is valid for this device */
    case API_CAL_CPL_ISO:
        ulAvailable = p_api_dev->x_CouplingCapabilities.b.b_Isolated;
        break;
    case API_CAL_CPL_TRF:
        ulAvailable = p_api_dev->x_CouplingCapabilities.b.b_Transformer;
        break;
    case API_CAL_CPL_DIR:
        ulAvailable = p_api_dev->x_CouplingCapabilities.b.b_Direct;
        break;
    case API_CAL_CPL_EXT:
        ulAvailable = p_api_dev->x_CouplingCapabilities.b.b_Network;
        break;
    case API_CAL_CPL_WRAP:
        ulAvailable = p_api_dev->x_CouplingCapabilities.b.b_DigitalWrap;
        break;
    default:
        ulAvailable = AiFalse;
        break;
    }


    if( ulAvailable == AiFalse )
    {
        return API_ERR_PLATTFORM_NOT_SUPPORTED;
    }


    /* switch coupling with firmware command */

    api_io_cal_cpl_con(p_api_dev, bus, cpl);



    return 0;

} /* end: api_cal_cpl_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_CAL                  Submodule : API_CAL_XMT_CON           */
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
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_CAL_XMT_CON' instruction to control     */
/*    the Transmitter Output amplitude of the specified API1553-DS MILbus    */
/*    channel.                                                               */
/*                                                                           */
/*****************************************************************************/
L_WORD api_cal_xmt_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
  AiUInt32 ulCanChangeAmplitudeHighRes;
  AiUInt32 ulCanChangeAmplitude = 0, ulCanChangeAmplitudePacked = 0;
  BYTE bus = (BYTE)in->bus;
  BYTE amp = (BYTE)in->con;

  mil_hw_can_change_amplitude(p_api_dev, p_api_dev->biu, &ulCanChangeAmplitude, &ulCanChangeAmplitudePacked);
  mil_hw_can_change_amplitude_high_res(p_api_dev, p_api_dev->biu, &ulCanChangeAmplitudeHighRes);

  if ((bus < API_CAL_PRI_BUS) || (bus > API_CAL_PACKED))
      return API_ERR_INVALID_BUS;

  if ((bus == API_CAL_HIGHRES) && !ulCanChangeAmplitudeHighRes)
      return API_ERR_PLATTFORM_NOT_SUPPORTED;

  /* Do not abort with an error if the board does not supporte a variable amplitude */
  /* This is neccessary because PBA.pro calls ApiCmdCalXmtCon regardless if the board can change the amplitude or not */

  if ((bus == API_CAL_PRI_BUS) || (bus == API_CAL_SEC_BUS))
      if (ulCanChangeAmplitudePacked)
          bus = API_CAL_PACKED;

  api_io_cal_xmt_con(p_api_dev, bus, amp);

  return 0;

} /* end: api_cal_xmt_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_CAL                  Submodule : API_CAL_TRANS_CON         */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 11.12.00   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Data Transmission Mode [con] 1:500Kbit/ 0:1Mbit            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_CAL_TRANS_CON' instruction to control   */
/*    the Data Transmission Mode (500Kbit/1Mbit).                            */
/*                                                                           */
/*****************************************************************************/
L_WORD api_cal_trans_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_SYS_CAL_TRANS_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;

  if( p_api_dev->ul_DeviceGroup != AI_DEVICE_AYE )
      return API_ERR_PLATTFORM_NOT_SUPPORTED;

  switch( in->con )
  {
    case API_CAL_TRANS_1M:
    case API_CAL_TRANS_500K:
      api_io_cal_trans_con(p_api_dev, (BYTE)in->con);
      break;
    default:
      ret_val = API_ERR_INVALID_CON;
      break;
  }

  return ret_val;
} /* end: api_cal_trans_con */



NAMESPACE1553_END

