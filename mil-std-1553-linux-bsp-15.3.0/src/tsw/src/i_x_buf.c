/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i_x_init.c
    3910 Buffer handling module of the ASP Driver SW.
    This module comprises the handler routines for the 3910 Buffer 
    instructions. 
*/

#include "mil_tsw_includes.h"



NAMESPACE1553_START


/*****************************************************************************/
/*                                                                           */
/*   Module : I_X_BUF                  Submodule : I_X_BUF                   */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API3910                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Create : 31.12.99   Update : 17.03.00    Status: [I][ ][   ]            */
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
/*    Entry function for the API3910 Buffer Handling related operations.  	*/
/*    This function is utilized to decode the Buffer instruction information */
/*    and to activate the appropriate handler routine. After execution of    */
/*    the instruction, acknowledge information is returned.                  */
/*                                                                           */
/*    Instructions:                                                          */
/*    - API_RAM_WRITE:       	  Direct Write Global Memory                 */
/*    - API_RAM_READ:        	  Direct Read Global Memory                  */
/*    - API_RAM_LWORD_WR:    	  Direct Write Longword                      */
/*    - API_RAM_LWORD_RD:    	  Direct Read Longword                       */
/*    - API_RAM_WORD_WR:     	  Direct Write Word                          */
/*    - API_RAM_WORD_RD:     	  Direct Read Word                           */
/*    - API_RAM_BYTE_WR:     	  Direct Write Byte                          */
/*    - API_RAM_BYTE_RD:     	  Direct Read Byte                           */
/*                                                                           */
/*****************************************************************************/

BYTE api_x_buf(TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p)
{
  BYTE ackfl = API_NAK;

  switch(cmd)
  {

    /******************************************************************************/
    /*         HS-Buffer functions                                          		*/
    /*                                                                           	*/
    /******************************************************************************/

    default:
      /* compatibility to standard 1553 calls */
      size = size - 3; /* CMD size incremented from 1 to 4  */
      /* call of standard LS commands */
      ackfl = api_buf(p_api_dev, (BYTE)cmd, size, info_p);
      break;
  }

  return (ackfl);

} /* end: api_x_buf */

NAMESPACE1553_END

