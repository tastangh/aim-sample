/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Log.h

 \brief Log facility

 This module registers any received log-data to one or more
 log devices if the currently activated log-level marks the log
 data to be registered.

 Each log data is provided with a log level to a function, which
 registers the given log data to the different log devices, if necessary.

 The following log devices are implemented yet:

 Ring of logfiles:

 The ring of logfiles devices saves the log data to a set of logfiles
 with defined names. Each logfile is  identified by a log index.
 If the logfile exceeds
 the maximum size, the next logfile in the ring of log files is used.

 The log facility keeps track of the current logfile-index.
 The log data is enhanced by a time stamp of the current time.

 System dependend event log:

 Additionally, the log facility logs log data to the system-dependend
 event log.

 Currently supported system's event logs are:

    o Windows NT Event Log
    o Unix syslog

 Standard output:

 The standard output device registers the log data to the standard output.

*******************************************************************************/
#ifndef AI_LOG_H
#define AI_LOG_H

/* Includes *******************************************************************/

/* The external interfaces contain the stdio.h's "FILE" data type.  */
#include <stdio.h>

#include "Ai_Logdef.h"

/* Constants ******************************************************************/

#define ntlog       ntlog_printf    /*!< Short replacement function name,   */
                                    /*!< which can be used instead of       */
                                    /*!< 'ntlog_printf'.                    */

/* Predefined time-out values   */
#define NTLOG_TIMEOUT_NONE      0L  /*!< Time-out value indicating "no      */
                                    /*!< wait", enforces immediate          */
                                    /*!< processing                         */

#define NTLOG_TIMEOUT_INFINITE  0xFFFFFFFFLU
                                    /*!< Time-out value indicating "wait    */
                                    /*!< forever", enforces blocked mode    */
                                    /*!< processing                         */

/* Macros *********************************************************************/

#if !defined(NDEBUG)
/**
 * This macro performs logging with the specified log level, if
 * the identifier "NDEBUG" is not defined. If "NDEBUG" is defined,
 * the macro resolves into an empty expression.
 *
 * Example:  NTLOG_D((NTLOG_LEVEL_DRIVER, "Format %s", string));
 */
# define NTLOG_D(expr)  { (void) ntlog_printf expr ; }
#else
# define NTLOG_D(expr)  /* empty    */
#endif

/* Types **********************************************************************/

/**
 * This enumeration describes the Log-Level index
 */
typedef enum tagNTLogIndex_t
{
    NTLOG_INDEX_INVALID     = -2,   /*!< Invalid Log file index             */
    NTLOG_INDEX_LASTUSED    = -1,   /*!< Last recently used Log file        */
    NTLOG_INDEX_FIRST       = 0,    /*!< First Log-Level index              */
    NTLOG_INDEX_MAX         = 99999 /*!< Last possible Log-Level index      */

} NTLOG_INDEX;

/**
 * Log-Level description. Multiple Log Levels may be combined by
 * an or-combination of the different Log Levels. Note that the
 * lower four bits are reserved to be combined with any combinations
 * of log devices as defined in the NTLOG_DEVICE enumeration.
 */
typedef enum tagNTLogLevel_t
{
    NTLOG_LEVEL_NONE    = 0x00000000,   /*!< No logging at all              */

    /* Generic log levels */
    NTLOG_LEVEL_00000010    = 0x00000010,
    NTLOG_LEVEL_00000020    = 0x00000020,
    NTLOG_LEVEL_00000040    = 0x00000040,
    NTLOG_LEVEL_00000080    = 0x00000080,
    NTLOG_LEVEL_00000100    = 0x00000100,
    NTLOG_LEVEL_00000200    = 0x00000200,
    NTLOG_LEVEL_00000400    = 0x00000400,
    NTLOG_LEVEL_00000800    = 0x00000800,
    NTLOG_LEVEL_00001000    = 0x00001000,
    NTLOG_LEVEL_00002000    = 0x00002000,
    NTLOG_LEVEL_00004000    = 0x00004000,
    NTLOG_LEVEL_00008000    = 0x00008000,
    NTLOG_LEVEL_00010000    = 0x00010000,
    NTLOG_LEVEL_00020000    = 0x00020000,
    NTLOG_LEVEL_00040000    = 0x00040000,
    NTLOG_LEVEL_00080000    = 0x00080000,
    NTLOG_LEVEL_00100000    = 0x00100000,
    NTLOG_LEVEL_00200000    = 0x00200000,
    NTLOG_LEVEL_00400000    = 0x00400000,
    NTLOG_LEVEL_00800000    = 0x00800000,
    NTLOG_LEVEL_01000000    = 0x01000000,
    NTLOG_LEVEL_02000000    = 0x02000000,
    NTLOG_LEVEL_04000000    = 0x04000000,
    NTLOG_LEVEL_08000000    = 0x08000000,

    /* internally used log levels */
    NTLOG_LEVEL_ERROR   = 0x00000010,   /*<! errors                         */
    NTLOG_LEVEL_WARN    = 0x00000020,   /*<! warnings                       */
    NTLOG_LEVEL_DEBUG   = 0x00000040,   /*<! debug messages                 */
    NTLOG_LEVEL_INFO    = 0x00000080,   /*<! informational messages         */

    NTLOG_LEVEL_SERVER  = 0x40000000,   /*<! used to identify server        */
                                        /*<! generated messages             */
    /* All log devices */
    NTLOG_LEVEL_ALL     = 0x7FFFFFF0    /*!< all together                   */

} NTLOG_LEVEL;

/* Prototypes *****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern  int     ntlog_initialize (void);

extern  int     ntlog_open (NTLOG_INDEX index);

extern  int     ntlog_open4 (/*@out@*/
    NTLOG_DESC  *logdesc,
    NTLOG_TYPE  logtype,
    NTLOG_INDEX logindex,
/*@null@*/
    NTLOG_FIFONAME  logfifo,
    int     log2stderr);

extern  void    ntlog_emergency (
    NTLOG_LEVEL level,
    char const  *format,
    char const  *logstr);

extern  void    ntlog_setemergency (int status);

extern  int     ntlog_isemergency (void);

#ifdef __GNUC__
 extern  int     ntlog_printf (
    NTLOG_LEVEL level,
/*@printflike@*/
    char const  *format,
    ...) __attribute__((format(printf, 2, 3)));  /* 2=format 3=params */
#else
extern  int     ntlog_printf (
    NTLOG_LEVEL level,
/*@printflike@*/
    char const  *format,
    ...);
#endif

#ifdef __GNUC__
extern  int     ntlog_printf2 (
    NTLOG_DESC  *logdesc,
    NTLOG_LEVEL level,
/*@printflike@*/
    char const  *format,
    ...) __attribute__((format(printf, 3, 4)));  /* 3=format 4=params */
#else
extern  int     ntlog_printf2 (
    NTLOG_DESC  *logdesc,
    NTLOG_LEVEL level,
/*@printflike@*/
    char const  *format,
    ...);
#endif

extern  int     ntlog_close4 (/*@only@*/
    NTLOG_DESC  *logdesc,
    NTLOG_TYPE  logtype,
    int         log2stderr);

extern  int     ntlog_close (void);

extern  int     ntlog_level (NTLOG_LEVEL loglevel);

extern  int     ntlog_lset (NTLOG_LEVEL level);

extern  int     ntlog_lclear (NTLOG_LEVEL   level);

extern void     ntlog_setCookedMode (
        int     enabledFlag);

extern  int     ntlog_setdevice (NTLOG_DEVICE   device);

extern  NTLOG_DEVICE    ntlog_getdevices (void);

extern  int     ntlog_setlogfilesize (int maxLogfileSize);

extern  int     ntlog_setlogfiles (int  maxLogfiles);

extern  int     ntlog_getindex (void);

extern  int     ntlog_index2filename (/*@out@*/
    NTLOG_FILENAME  filename,
    int     index);

extern  void        ntlog_lock (void);

extern  void        ntlog_unlock (void);

extern  void        ntlog_errno (
    int     printflag,
    char const  *text);

extern  int     ntlog_register (NTLOG_DESC  *logdesc);

extern  void        ntlog_deregister (void);

#ifdef __cplusplus
}
#endif

#endif /* NTLOG_H */

