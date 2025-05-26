/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#include "mil_tsw_includes.h"


/* to satisfy GNU */
L_WORD efex_switch( TY_API_DEV *p_api_dev, BYTE Board);

/*****************************************************************************/
/*                                                                           */
/*   Module : API_LIC                  Submodule : API_SYS_EFEX_SWITCH       */
/*                                                                           */
/*****************************************************************************/

L_WORD efex_switch( TY_API_DEV *p_api_dev, BYTE ucSwitchType )
{
  L_WORD retVal = 0;
  BYTE   ucRebootPbi1       = FALSE;
  BYTE   ucRebootPbi2       = FALSE;
  BYTE   ucPermanent        = FALSE;

  L_WORD ulBoardType        =  0;
  L_WORD ulBoardTypeOld     =  0;
  L_WORD ulBoardTypeNew     =  0;

  L_WORD ulLcaBootSec   = 64;
  L_WORD ulLsBiuBootSec = 64;
  L_WORD ulHsBiuBootSec = 64;
  L_WORD ulLsBiuFreq    =  0;
  L_WORD ulHsBiuFreq    =  0;



  ulBoardType = p_api_dev->BoardType;

  PRINTF2("ulBoardType=%08X switch_type=%d\r\n", ulBoardType, ucSwitchType );



  /* -- check if this board can be switched --- */

  if( !mil_hw_switch_available(p_api_dev) )
  {
      PRINTF0("EFA/EFEx switch not available for this plattform\n\r" );
      return API_ERR_FUNCTION_NOT_SUPPORTED;
  }

  /* -- check if input values are valid --- */

  if( ucSwitchType > 4 )
  {
      PRINTF1("Invalid switch type %d\n\r", ucSwitchType );
      return API_ERR_INVALID_MODE;
  }


  /* -- set new hs boardtype identifier --- */

  if( (ucSwitchType == 1) || (ucSwitchType == 3) )
  {
    /* 3910 Permanent(1)/Temporary(3) */
    ulBoardTypeOld   = 0x70;
    ulBoardTypeNew   = 0x30;
  }
  else if( (ucSwitchType == 2) || (ucSwitchType == 4) )
  {

    /* EFEx Permanent(2)/Temporary(4) */
    ulBoardTypeOld   = 0x30;
    ulBoardTypeNew   = 0x70;
  }


  /* -- get the boot sectors for the new boardtype --- */


  mil_hw_switch_get_param( p_api_dev, 
                                 ulBoardTypeNew, 
                                 &ulLsBiuBootSec, 
                                 &ulHsBiuBootSec, 
                                 &ulLcaBootSec,
                                 &ulLsBiuFreq,
                                 &ulHsBiuFreq );


  /* We always make a temporary switch
     The NOVRAM is however only updated if 
     a permanent switch is requested.
     This ensures that the user does not have
     to reboot if he makes a permanent switch.
  */


  /* -- calculate new boardtype value --- */

  if( (ulBoardTypeOld<<8) == (ulBoardType & 0x0000FF00) )
  {
    ucRebootPbi1 = TRUE;

    ulBoardType &= 0xFFFF00FF;
    ulBoardType |= (ulBoardTypeNew<<8);
  }


  if( (ulBoardTypeOld<<24) == (ulBoardType & 0xFF000000) )
  {
    ucRebootPbi2 = TRUE;

    ulBoardType  &= 0x00FFFFFF;
    ulBoardType  |= (ulBoardTypeNew<<24);
  }



  /* -- If the device is already in the required mode we have nothing to do */

  if(    (ucRebootPbi1 == FALSE) 
      && (ucRebootPbi2 == FALSE))
  {
      PRINTF0("No changes neccessary\n\r" );
      return 0;
  }



  /* -- check if changes need to be made permanent --- */

  if( (ucSwitchType == 1) || (ucSwitchType == 2) )
  {
    ucPermanent = TRUE;
  }

    
  /* -- Set global variables and if permanent NOVRAM values --- */

  mil_hw_set_globals_bt( p_api_dev, ucPermanent, ulBoardType );


  if( ucRebootPbi1 == TRUE )
      mil_hw_set_globals_pbi1( p_api_dev, ucPermanent, ulLcaBootSec, ulLsBiuBootSec, ulHsBiuBootSec, ulLsBiuFreq, ulHsBiuFreq );
  
  if( ucRebootPbi2 == TRUE )
      mil_hw_set_globals_pbi2( p_api_dev, ucPermanent, ulLcaBootSec, ulLsBiuBootSec, ulHsBiuBootSec, ulLsBiuFreq, ulHsBiuFreq );


  /* -- Reboot the TSW to reinitialize the board --- */

  mil_hw_reboot_tsw( p_api_dev );

  /* -- done --- */

  return retVal;

}
