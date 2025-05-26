/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
smartbat.cpp
This module handles the batch mode of the SMART console program.
*/


#include <stdio.h>
#include <string.h>
#include "Ai_cdef.h"
#include "Wrapper.h"
#include "SmartLib.h"

#ifndef false
  #define false 0
#endif
#ifndef true
  #define true 1
#endif 

/****************************************************/
/* DLL internal structure                           */
/****************************************************/
#ifdef _SMARTAIM
  #define SL_MAX_BATCHMODE  4
#else
  #define SL_MAX_BATCHMODE  1
#endif

/****************************************************/
/* DLL internal prototypes                          */
/****************************************************/


/****************************************************/
/* DLL functions                                    */
/****************************************************/
/*!
Shows the usage screen.

@param  ac_Version   Array of characters representing the SMART version number. 

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 09.06.2010
*/

_API_EXTERN void _API_DLL_FUNC v_SLShowUsage(AiChar *ac_Version)
{
  printf("SMART %s\n", ac_Version );
  printf("\n"); 
  printf("Usage:\n"); 
  printf("SmartCon [--update-file|-u <update-file>] [--serial|-s <serial>] [--server <server>] [--autoboot|-a]\n");
  printf("         [--licence-key|-l <licencekey>] [--force|-f] [--mode|-m <mode-switch-ID] [COMMAND]\n");
  printf("\n");
  printf("If no command is specified, you will be presented a menu where you can select individual options\n\n");
  printf("Options:\n\n");
  printf("--update-file,-u    <update-file> will be used for board updates\n");
  printf("--serial,-s         Use in conjunction with update or mode switch command. Only update or switch mode of board with serial <serial> \n");
  printf("--server            Connect to remote server <server> and use the boards located there\n");
  printf("--autoboot,-a       automatically reboot or shut down system, instead of just telling to do so\n");
  printf("--licence-key,-l    Use in conjunction with mode switch command. Use licence key <licencekey> mode switch\n");
  printf("--force,-f          Force board updates, even if software versions are up to date\n");
  printf("--mode,-m           Use in conjunction with mode switch command. Mode ID to switch board to\n");
  printf("\n"); 
  printf("Commands:\n\n");
  printf("update              update boards by using the selected update file\n");
  printf("show-modes          shows all available mode switch options\n");
  printf("switch-mode         switches board(s) to the mode specified by --mode option\n");
  printf("nvr-dump            print board novram value\n");
  printf("help                Shows this help\n\n");
}

