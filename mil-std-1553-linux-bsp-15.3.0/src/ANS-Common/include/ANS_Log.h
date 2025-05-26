/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Log.h
 *
 * \brief ANS Logging Declarations
 *
 * This module provides the ANS logging mechanism.
 *
 *
 ******************************************************************************/
#ifndef ANS_LOG_H
#define ANS_LOG_H

/* Includes *******************************************************************/

#include "Ai_Log.h"

/* Constants ******************************************************************/

#define ANSLOG_DEVICES  (NTLOG_DEVICE_STDOUT | NTLOG_DEVICE_EVENTLOG)
                                /*!< Devices to be used for logging         */

/*! \def ANSLOG_LOG_LEVEL
 *
 * Default active ANS log levels
 */
#define ANSLOG_LOG_LEVEL  (NTLOG_LEVEL_ERROR | NTLOG_LEVEL_WARN)
//#define ANSLOG_LOG_LEVEL  (NTLOG_LEVEL_ALL)  // AIM 429 SRV Integration

/* Macros *********************************************************************/

/**
 * Log the specified debug message using the configured log setting.
 */
#define ANSLogDebug(format, ...)   ntlog_printf((NTLOG_LEVEL) (ANSLOG_DEVICES | NTLOG_LEVEL_DEBUG), format, ## __VA_ARGS__)


/**
 * Log the specified informational message using the configured log setting.
 */
#define ANSLogInfo(format, ...)   ntlog_printf((NTLOG_LEVEL) (ANSLOG_DEVICES | NTLOG_LEVEL_INFO), format, ## __VA_ARGS__)


/**
 * Log the specified warning message using the configured log setting.
 */
#define ANSLogWarn(format, ...)   ntlog_printf((NTLOG_LEVEL) (ANSLOG_DEVICES | NTLOG_LEVEL_WARN), format, ## __VA_ARGS__)


/**
 * Log the specified error message using the configured log setting.
 */
#define ANSLogError(format, ...)   ntlog_printf((NTLOG_LEVEL) (ANSLOG_DEVICES | NTLOG_LEVEL_ERROR), format, ## __VA_ARGS__)



/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

extern int ANSLogInitialize (void);
extern int ANSLogShutdown (void);

#endif  /* ANS_LOG_H */
