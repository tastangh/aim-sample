/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Dll_ini.c
 *
 * Driver communication module of the Application interface Contains the communication specific functions for driver communication.
 * 
 */

//****************************************************
// platform specific specific includes
//****************************************************
#include "AiOs.h"


//****************************************************
// my own include includes
//****************************************************
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

#ifdef _AIM_WINDOWS
#include <DBT.H>
#include <winioctl.h>
#include <setupapi.h>
#include <time.h>
#include <Dll_Fnc.h>

#include <AimGuids.h>
#endif


#ifdef _AIM_WINDOWS
    static char buf[2000];
#endif    




//**************************************************************************
//
//  _DLLInit
//
//**************************************************************************

#ifdef __BORLANDC__
#pragma argsused
#endif


#if defined(_AIM_VXI) || defined(_AIM_LVRT)
    AI_LIB_FUNC AiStatus AI_CALL_CONV M_AXI_PNP_AUTOINITIALIZE(AiUInt8, AiUInt8* ,AiSession[]);
#endif
//**************************************************************************
//
//  _DLLExit
//
//**************************************************************************

#ifdef __BORLANDC__    // %&10
#pragma argsused
#endif

