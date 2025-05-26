/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i_x_sys.c
    System Control module of the API3910 ASP Driver SW. 
    This module comprises the handler routines for the System Control 
    instructions. Entry function in this module is 'api_x_sys'.       
    The module will distribute the function calls to the 1553 api_sys 
    module and the 3910 api39_sys module
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START

/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                Submodule : API_X_SYS_BIU_SWITCH        */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 31.12.99    Status: [I][ ][   ]            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Low Speed or High Speed Identifier [uc_LsHsId]             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function switches the biu to switch the biu parameter to the HS   */
/*    biu or the LS biu                                                      */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_sys_biu_switch(TY_API_DEV *p_api_dev, BYTE uc_LsHsId)
{

  if(uc_LsHsId == (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1))
   /* Low Speed */
    p_api_dev->biu = (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1);
  else
   /* High Speed */
    p_api_dev->biu = (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_2);

  return (0);
} /* api_x_sys_biu_switch */


/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                  Submodule : API_X_SYS_BOARD_TYPE 	 */
/*																			 */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 31.12.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function identifies the board type. The function will be used to  */
/*    branch into the 3910 or 1553 part of the Driver Software               */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_sys_board_type(TY_API_DEV *p_api_dev)
{
  BYTE uc_api39_board_type;

  /* BIU1 is always 1553 */
  if(p_api_dev->biu == (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_1))
    uc_api39_board_type = API39_TYPE_1553;
  else
   /* Biu 2 */
  {
    if( ul_StreamIs3910( p_api_dev, p_api_dev->biu ) )
     /* check Board is 3910 */
      uc_api39_board_type = API39_TYPE_3910;
    else
      if( ul_StreamIsEFEX( p_api_dev, p_api_dev->biu ))
     /* check Board is EFEX */
        uc_api39_board_type = API39_TYPE_EFEX;
      else
        uc_api39_board_type = API39_TYPE_1553;
  }

  return (uc_api39_board_type);

} /* api_x_sys_board_type */


/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS               Submodule : API_X_SYS_READ_BOARD_CONFIG	 */
/*																			 */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 31.12.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Board Configuration PBI Type [ul_apiBoardInfo]             */
/*                                                                           */
/*    Description :                                                          */
/*    This function will read the Board Configuration of the equipped        */
/*    PBI module                                                             */
/*                                                                           */
/*****************************************************************************/

L_WORD api_x_sys_read_board_config(TY_API_DEV *p_api_dev)
{
  L_WORD ul_apiConfInfo, ul_apiBoardInfo;


  ul_apiConfInfo = (BSWAP32( (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_cid) );
  ul_apiBoardInfo = ul_apiConfInfo &API39_SCB_FID_CONFIG_BITMASK;
  ul_apiBoardInfo = ul_apiBoardInfo >> API39_SCB_FID_CONFIG_BITPOS;

  return (ul_apiBoardInfo);

} /* api_x_sys_read_board_config */

/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                  Submodule : API_X_SYS_RESET			 */
/*																			 */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 17.03.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Initialize Control [mode]                                  */
/*   				 Monitor Buffer Size [uc_mbfs]							 */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RESET' instruction to distribute the 	 */
/*    instruction to the related Low Speed or High Speed module to initialize*/
/*    the API3910 hardware and software to an initial state.                 */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_sys_reset(TY_API_DEV *p_api_dev, BYTE uc_rc, BYTE uc_mbfs)
{
  BYTE ackfl;

  ackfl = api_sys_reset_legacy(p_api_dev, uc_rc); /* API1553 */

  /* check LS Reset performed OK */
  if(ackfl == API_ACK2)
  {
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
      ackfl = api39_sys_reset_legacy(p_api_dev, uc_rc, uc_mbfs);
  }

  return (ackfl);

} /* end: api_x_sys_reset */

/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                  Submodule : API_X_SYS_READ_SW_VERS    */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 25.05.00     Status: [I][T][OK ]           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none									                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_READ_SW_VERS' instruction to distribute */
/*    the instruction to the related Low Speed or High Speed module to 		 */
/*	 return the API3910 ASP Driver SW version number and Firmware version	 */
/* 	 number.   																 */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_sys_read_sw_vers(TY_API_DEV *p_api_dev)
{
  BYTE ackfl;


  /* check 3910 Board equipped */
  if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) && (p_api_dev->biu == (p_api_dev->hs_stream*p_api_dev->ul_NbOfStreams1 + API39_BIU_2)) )
   /* check Board is 3910 or EFEX*/
    ackfl = api39_sys_read_sw_vers_legacy(p_api_dev);
   /* API3910 */
  else
   /* Low Speed  */
    ackfl = api_sys_read_sw_vers_legacy(p_api_dev);
   /* API1553 */

  return (ackfl);

} /* end: api_x_sys_read_sw_vers */

/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                  Submodule : API_X_SYS_READ_SW_VERS_EX */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 13.01.05   Update : 13.01.05     Status: [I][T][OK ]           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none									                                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_READ_SW_VERS_EX' instruction to         */
/*    distribute the instruction to the related Low Speed or High Speed 	   */
/*	  module to return the API3910 SW version numbers.                     	 */
/*                                                                           */
/*****************************************************************************/

static BYTE api_x_sys_read_sw_vers_ex(TY_API_DEV *p_api_dev)
{
  BYTE ackfl;

  ackfl = api_sys_read_sw_vers_ex_legacy(p_api_dev);

  return (ackfl);

} /* end: api_x_sys_read_sw_vers_ex */



/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                Submodule : API_X_SYS_GET_MEM_PARTITION */
/*                                                                           */
/*   Author : Marco Motz             Project   : API3910                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 28.08.02     Status: [I][T][OK ]           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none									                     */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_SYS_GET_MEM_PARTITION' instruction      */
/*    to return the Global memory layout information.                        */
/*                                                                           */
/*****************************************************************************/

static BYTE api_x_sys_get_mem_partition(TY_API_DEV *p_api_dev, BYTE uc_mode)
{
  BYTE ackfl;

  ackfl = api_sys_get_mem_partition_legacy(p_api_dev, uc_mode);
  if( API_ACK2 == ackfl )
  {
    p_api_dev->biu       = BIU_2-1;
    p_api_dev->hs_stream = 0;
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
      ackfl = api39_sys_get_mem_partition_legacy(p_api_dev, uc_mode);
    if( API_ACK2 == ackfl )
    {
      p_api_dev->biu       = p_api_dev->ul_NbOfStreams1 + BIU_2-1;
      p_api_dev->hs_stream = 1;
      if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        ackfl = api39_sys_get_mem_partition_legacy(p_api_dev, uc_mode);
    }
  }

  return (ackfl);

} /* end: api_x_sys_get_mem_partition */
/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_SYS                  Submodule : API_X_SYS                 */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Instruction identifier [cmd],                              */
/*                Instruction size [size],                                   */
/*                Instruction parameter list [info_p]                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    Entry function for the API3910 System related operations.           	 */
/*    This function is utilized to decode the instruction information and    */
/*    to activate the appropriate handler routine. After execution of the    */
/*    instruction acknowledge information is returned. Instructions:         */
/*                                                                           */
/* 	 	******	modified functions for 3910 extension **************		 */
/*   - API_RESET:         	  Reset Command                                  */
/*   - API_READ_SW_VERS:      Read Software Version Command                  */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_sys(TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p)
{
  BYTE ackfl;

  ackfl = API_NAK;

  switch(cmd)
  {
    case API_RESET:
      ackfl = api_x_sys_reset(p_api_dev,  *info_p, *(info_p + 1) );
      break;
    case API_READ_SW_VERS:
      if(size == 7)
        ackfl = api_x_sys_read_sw_vers(p_api_dev);
      break;
    case API_READ_SW_VERS_EX:
      if(size == 7)
        ackfl = api_x_sys_read_sw_vers_ex(p_api_dev);
      break;
    case API_SYS_GET_MEM_PARTITION:
       /* 28.08.02 */
      ackfl = api_x_sys_get_mem_partition(p_api_dev,  *info_p);
      break;

      /**************************************/
      /* standard 1553 unmodified functions */
      /**************************************/

    default:
      ackfl = api_sys(p_api_dev, cmd, size, info_p);
      break;
  }

  return (ackfl);

} /* end: api_x_sys */

NAMESPACE1553_END

