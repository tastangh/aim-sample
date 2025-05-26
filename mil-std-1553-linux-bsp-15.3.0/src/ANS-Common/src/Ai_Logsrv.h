/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Logsrv.h

\brief Log Server Specific Functions

 This module provides log server specific functions for use within
 the log facility.
*******************************************************************************/
#ifndef AI_LOGSRV_H
#define AI_LOGSRV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

#include "Ai_Logdef.h"

/* Constants ******************************************************************/

/* Macroes ********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

#if NTLOG_CC_SERVERCODE
extern  int         ntlogsrv_open (/*@out@*/
    NTLOG_SRVDESC   *logdesc,
/*@null@*//*@unique@*/
    NTLOG_FILENAME  logfifo,
    int             log2stderr);
#endif

#if NTLOG_CC_SERVERCODE
 extern int         ntlogsrv_process (
    NTLOG_SRVDESC   *logdesc,
    unsigned long   timeout);
#endif

#if NTLOG_CC_SERVERCODE
 extern int     ntlogsrv_close (/*@only@*/
    NTLOG_DESC  *logdesc,
    int         log2stderr);

#endif

#if NTLOG_CC_SERVERCODE
 extern int         ntlogsrv_setdevices (
    NTLOG_SRVDESC   *logdesc,
    NTLOG_DEVICE    logdevice);
#endif

extern  char /*@observer@*/ *ntlogsrv_getrcsid (void);

#ifdef __cplusplus
}
#endif


#endif
