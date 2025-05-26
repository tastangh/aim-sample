/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartLibAutoUpdateNone.c
Contains the interface functions for the SmartLib library in case autoupdate is not used.
*/

#include <stdio.h>
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Wrapper.h"
#include "SmartLib.h"
#include "AiOs.h"






_API_EXTERN void _API_DLL_FUNC  SLShutdown(enum sl_post_update_actions shutdown_flags)
{
    printf("shutdown is not yet supported for Linux\n");
}

_API_EXTERN void _API_DLL_FUNC  SLRegisterRestart(char * command_line)
{
    printf("application restart is not yet supported for Linux\n");
}
