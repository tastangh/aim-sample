/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Glob.c
 *
 * Global variables of the APC Application Interface.
 * 
 */


//***************************
// Windows specific includes
//***************************
#include "AiOs.h"


#ifdef _AIM_WINDOWS
#include "CONTROL.h" /* old IOCTLs for compatibility */
#include "AimIoctl.h"
#endif

//***************************
// my own include
//***************************
#include "Ai_cdef.h"
#ifdef _API3910
  #include "Ai3910i_def.h"
  #include "Ai3910i_fnc.h"
  #include "Ai1553_Internal_def.h"
  #include "Ai3910_Internal_def.h"
#else
  #include "Ai1553i_def.h"
  #include "Ai1553i_fnc.h"
  #include "Ai1553_Internal_def.h"
#endif
#include "DEFS_V.H"



#if defined(_AIM_VXI)
#include <vpptype.h>
#include <visa.h>
#include <visa_io.h>
#endif

#ifdef _AIM_VME
#include "AiMilVxWorksIo.h"
#include "AiVmeGeneric.h"
#include <stdio.h>
#include <stdlib.h>    /* for malloc and free */
#include <string.h>    /* for memcpy */
  #ifdef _AMC1553
    // monitor include files for AMC
    #include "amc_compat.h"
    #include "type_def.h"
    #include "prototypes.h"
    #include "AiMilVxWorksNoAsp.h"
  #endif //#ifdef _AMC1553 
#endif


//***************************
// misc global vars
//***************************
AiUInt8 tmp_byte;
AiUInt16 tmp_word;
AiUInt32 tmp_lword;
AiUInt8 *b_p = &tmp_byte;
AiUInt16 *w_p = &tmp_word;
AiUInt32 *lw_p = &tmp_lword;
AiInt16 tg_os;

TY_DEVICE_INFO_LIST * px_MilDeviceInfoListStart = NULL;
TY_DEVICE_INFO_LIST * px_MilDeviceInfoListEnd   = NULL;

TY_SRV           Srv[MAX_API_SERVER_PER_SLOT];

#if defined(_AIM_WINDOWS) || defined(_AIM_LVRT)
  FILE *stream = NULL;
  SYSTEMTIME SystemTime = {0};
#elif defined _AIM_LINUX || defined(_AIM_RTX64)
  FILE* stream = NULL; 
#endif

#if defined(_AIM_VME)
    /* In VxWorks Version 6 and higher we must not define errno */
    #if (defined(_WRS_VXWORKS_MAJOR))
      #if (_WRS_VXWORKS_MAJOR < 6) /* from VxWorks target/h/version.h */
        AiInt errno;
      #endif
    #else
        AiInt errno;
    #endif
#endif

#ifdef _AIM_VME

 #ifdef _AMC1553
    TY_MONITOR_DATA_STRUCT ini_mon_data[ 8 ]; /* MONITOR function data            */
 #endif

#endif


#ifndef _AIM_VME
  TY_PNP_CALLBACK_FUNC_PTR v_MilPnPCallback = NULL;
#endif

char szLogFileName[256+12];

//****************************************************
// In most cases
//****************************************************
AiUInt32 Aiul_DbgLevel = 0; // initial value is set in DllInit()
AiUInt32 Aiul_GlobalTestValue = 0; // This value can be used to check if several threads/task share a commom memory
AiUInt16 Aiapi_max_boards = 0;

#if defined(_AIM_WINDOWS) || defined(_AIM_RTX64) || defined(_AIM_VME) || defined(_AIM_LINUX)

  AiUInt8 b_RegisteredServersCount = 1;    // local server is registered



  AiUInt16 api_max_ext_boards = 0;



       char     dbgBuf[2000] = {0};


      AiUInt32    ul_AttachedApps = 0;

#endif


#if defined(_AIM_VXI)
 AiUInt8 b_RegisteredServersCount = 1;    // local server is registered

 AiUInt16 Aiapi_max_ext_boards = 0; 
 AiUInt8 AiBoard[MAX_API_MODULE]={0,0,0,0};   
 TY_RAM_BASE  AiramBase_p[MAX_API_MODULE]={{0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,0,0,0,0}};                                                     

 AiChar        AidbgBuf[2000];
#if defined(_AIM_LVRT)
 TyIntFuncElement Aiax_IntFuncTable[ MAX_API_MODULE ][ MAX_BIU ] =
                       { {{{NULL,NULL,NULL,NULL,NULL},0},
                         {{NULL,NULL,NULL,NULL,NULL},0}},
                         {{{NULL,NULL,NULL,NULL,NULL},0},
                         {{NULL,NULL,NULL,NULL,NULL},0}},
                         {{{NULL,NULL,NULL,NULL,NULL},0},
                         {{NULL,NULL,NULL,NULL,NULL},0}},
                         {{{NULL,NULL,NULL,NULL,NULL},0},
                         {{NULL,NULL,NULL,NULL,NULL},0}}};
#endif

 /* the following flag is used in WriteRepArea() as indicator, whether replay buffer 1 or 2 is active */
  AiUInt8 auc_ReplayFlag[MAX_API_MODULE][MAX_BIU] = 
               { 
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, 
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,},
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, 
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,},
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, 
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,},
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, 
                  { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}, { 0, 0, 0, 0,}
               };
 AiUInt8 auc_HsReplayFlag[MAX_API_MODULE] = { 0, 0, 0, 0, 0, 0, 0, 0, \
                                              0, 0, 0, 0, 0, 0, 0, 0, \
                                              0, 0, 0, 0, 0, 0, 0, 0, \
                                              0, 0, 0, 0, 0, 0, 0, 0};

 AiUInt32    ul_AttachedApps = 0;
#endif // _AIM_VXI

// end: glob

