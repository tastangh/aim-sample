/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Log.c
 *
 * This file provides the ANS logging mechanism.
 *
 ******************************************************************************/

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "ANS_Log.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

/* Variables ******************************************************************/

static NTLOG_DESC   g_ntlogDescriptor;

/* Functions ******************************************************************/

/**
 * This function initializes the ANS429 log facility.
 *
 */
extern int ANSLogInitialize (void)
{
    int     rc;

    /*
     * Perform basic initialization.
     */
    ntlog_initialize();

    /*
     * Setup the log methods (here stdout and syslog)
     */
    ntlog_setdevice(ANSLOG_DEVICES);

    /*
     * Setup the logging levels
     */
    ntlog_lset(ANSLOG_LOG_LEVEL);

    /*
     * Initialize the underlying log facility.
     */
    rc = ntlog_open4(&g_ntlogDescriptor, NTLOG_TYPE_STANDALONE, NTLOG_INDEX_FIRST, NULL, !0);
    if ( 0 != rc )
    {
        return !0;          /** \return !0 - error                      */
    }

    return 0;               /** \return 0 - success                     */
}

/**
 * This function shuts the ANS429 log facility down.
 * Call this function to prepare for a proper program shutdown.
 */
extern int ANSLogShutdown (void)
{
    int     rc;

    rc = ntlog_close4(&g_ntlogDescriptor, NTLOG_TYPE_STANDALONE, !0);

    return rc;          /** \return !0 - error, 0 = success             */
}
