/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i_x_init.c
    Initialisaion module of the AYI3910 ASP Driver SW.
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START


#ifdef CMPL4SIM


/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_INI                  Submodule : I_X_INI                   */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Initialize Control [uc_mode]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API39_INI' instruction to initialize the    */
/*    API3910 ASP Driver Software.                                        	*/
/*                                                                           */
/*****************************************************************************/

BYTE api_x_ini(TY_API_DEV *p_api_dev, BYTE uc_mode)
{
  BYTE ackfl;

  p_api_dev->biu = 0;
  ackfl = api_ini_legacy(p_api_dev, uc_mode);

#if !defined(_AIM_VME) || defined(AVI_IO_TEST) /* There is no HS board yet, where the TSW runs within the LIB */
  if(API_ACK2 != ackfl) 
    PRINTF0("api_ini FAILED\n\r");
  else
  {
    p_api_dev->biu = BIU_2-1;
    p_api_dev->hs_stream = 0;
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
    {
      ackfl = api39_ini_legacy(p_api_dev, uc_mode);
      (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, API39_IR_LOG_ADDR); /* Interrupt Loglist Pointer */
      p_api_dev->islp[ p_api_dev->biu ] = API39_IR_LOG_ADDR;
    }
    if(API_ACK2 != ackfl)
      PRINTF0("api39_ini(BIU_2) FAILED\n\r");
    else
    {
      p_api_dev->biu = BIU_4-1;
      p_api_dev->hs_stream = 1;
      if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
      {
        ackfl = api39_ini_legacy(p_api_dev, uc_mode);
        (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, API39_IR_LOG_ADDR); /* Interrupt Loglist Pointer */
        p_api_dev->islp[ p_api_dev->biu ] = API39_IR_LOG_ADDR;
        if(API_ACK2 != ackfl)
          PRINTF0("api39_ini(BIU_4) FAILED\n\r");
        else
        {
          p_api_dev->biu = BIU_6-1;
          p_api_dev->hs_stream = 1;
          if( ul_StreamIsHS( p_api_dev, p_api_dev->biu) )
          {
            ackfl = api39_ini_legacy(p_api_dev, uc_mode);
            (*px_api39_cb_p[ p_api_dev->hs_stream ]).ul_ilp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, API39_IR_LOG_ADDR); /* Interrupt Loglist Pointer */
            p_api_dev->islp[ p_api_dev->biu ] = API39_IR_LOG_ADDR;
            if(API_ACK2 != ackfl)
              PRINTF0("api39_ini(BIU_6) FAILED\n\r");
          }
        }
      }
    }
  }
#endif /* #if !defined(_AIM_VME) || defined(AVI_IO_TEST) */

  return (ackfl);
} /* end: api39_ini */

#endif

NAMESPACE1553_END

