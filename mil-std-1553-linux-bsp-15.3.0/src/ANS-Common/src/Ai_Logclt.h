/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file

\brief Log Client specific Functions

 This module provides log client specific functions for use within the
 log facility.
*******************************************************************************/
#ifndef AI_LOGCLT_H
#define AI_LOGCLT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

#include "Ai_Logdef.h"

/* Constants ******************************************************************/

/* Makroes ********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

#if NTLOG_CC_CLIENTCODE
extern int      ntlogclt_open (/*@out@*/
    NTLOG_CLTDESC   *logdesc,
/*@null@*//*@unique@*/
    NTLOG_FILENAME  logfifo,
    int             log2stderr);
#endif

#if NTLOG_CC_CLIENTCODE
extern  int     ntlogclt_close (/*@only@*/
    NTLOG_DESC  *logdesc,   /*FPM Log facility descriptor   */
                    /*FPM to be deinitialized.  */
    int     log2stderr);    /*FPI Flag: log errors to   */
                    /*FPI stderr            */
#endif

#ifdef __cplusplus
}
#endif


#endif
