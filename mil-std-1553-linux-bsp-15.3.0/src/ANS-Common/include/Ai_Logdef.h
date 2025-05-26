/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Logdef.h

 \brief Definitions and Declarations for "ntlog"

 This module provides common definitions and declarations for the
 "Ai_Log" log facility.
*******************************************************************************/
#ifndef AI_LOGDEF_H
#define AI_LOGDEF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Target system checking *****************************************************/

#undef  TARGET_SUPPORTED
#if defined(__unix)
# define TARGET_SUPPORTED   1
#elif   defined(WIN32)
# define TARGET_SUPPORTED   1
#elif   defined(_VXBUS)
# define TARGET_SUPPORTED   1
#else
# error "This target OS isn't supported yet"
#endif

/* Conditional compiling ******************************************************/

#ifndef NTLOG_USE_SEMAPHORES
# define NTLOG_USE_SEMAPHORES   1   /*!< Compilation switch:                */
                                    /*!< enable use of semaphores           */
                                    /*!< for log synchronization            */
#endif
#ifndef NTLOG_CC_SERVERCODE
# define NTLOG_CC_SERVERCODE    0   /*!< Compilation switch:                */
                                    /*!< Compile support for log            */
                                    /*!< server code (currently UNIX only). */
#endif
#ifndef NTLOG_CC_CLIENTCODE
# define NTLOG_CC_CLIENTCODE    0   /*!< Compilation switch:                */
                                    /*!< Compile support for log            */
                                    /*!< client code (currently UNIX only). */
#endif

/* Includes *******************************************************************/

#include <stdio.h>

/* Constants ******************************************************************/

#define NTLOG_FILENAME_LEN  8   /*!< Length of the log file names           */
#define NTLOG_FIFONAME_SIZE 80  /*!< Size of the buffer to                  */
                                /*!< hold the FIFO name                     */

/* Makroes ********************************************************************/

/* Type definitions ***********************************************************/

/**
 * Definition of the log-file name
 */
typedef char    NTLOG_FILENAME[NTLOG_FILENAME_LEN + 1];

/**
 * Buffer to hold the FIFO name.
 */
typedef char    NTLOG_FIFONAME[NTLOG_FIFONAME_SIZE];

/**
 * This enumeration describes the different supported log devices.
 */
typedef enum tagNTLogDevice_t
{
    NTLOG_DEVICE_NONE       = 0x00000000,   /*!< none                       */
    NTLOG_DEVICE_STDOUT     = 0x00000001,   /*!< stdout (console)           */
    NTLOG_DEVICE_EVENTLOG   = 0x00000002,   /*!< system's event log         */
    NTLOG_DEVICE_LOGFILE    = 0x00000004,   /*!< log file                   */

    NTLOG_DEVICE_ALL        = 0x0000000f    /*!< all together               */

} NTLOG_DEVICE;

/**
 * Log buffer descriptor.
 */
typedef struct tagNTLogBuffer_t
{
    size_t      buffersize;     /*!< size of the allocated memory           */
    char        *buffer;        /*!< reference to allocated memory          */

} NTLOG_BUFFER;

/**
 * Supported types of log facility types.
 */
typedef enum tagNTLogType_t
{
    NTLOG_TYPE_STANDALONE,      /*!< stand-alone, the conventional log type */
    NTLOG_TYPE_SERVER,          /*!< log server                             */
    NTLOG_TYPE_CLIENT,          /*!< log client                             */

    /* reserved entry */
    NTLOG_TYPE_RESERVED

} NTLOG_TYPE;

/**
 * Log facility type descriptor data structure for log servers. This
 * structure contains all information used to access log server
 * output functions.
 */
typedef struct tagNTLogSrvDesc_t
{
    NTLOG_FIFONAME  logfifo;    /*!< name of FIFO special file              */
    int             hsrv_read;  /*!< file read handle in server mode        */
    FILE            *fpwrite;   /*!< file write pointer in server mode      */
    NTLOG_DEVICE    logdevice;  /*!< log devices to be used                 */
    char            buffer[512];/*!< read buffer                            */
    char            *wptr;      /*!< write pointer into buffer              */

} NTLOG_SRVDESC;

/**
 * Log facility type descriptor data structure for log clients. This
 * structure contains all information used to access log client
 * output functions.
 */
typedef struct tagNTLogCltDesc_t
{
    NTLOG_FIFONAME  logfifo;    /*!< name of FIFO special file              */
    FILE            *fpclient;  /*!< file pointer in client mode mode       */

} NTLOG_CLTDESC;

/**
 * Log facility type descriptor data structure. This structure
 * contains all information used to access either log server or log
 * client output functions. Note that this structure has to be
 * initialized by a call to 'ntlog_open4' before the first use.
 */
typedef struct tagNTLogDesc_t
{
    unsigned long   initialized;    /*!< initialization indicator           */
    NTLOG_TYPE  logtype;            /*!< type of log facility to be         */
                                    /*!< accessed                           */
    union
    {
        NTLOG_SRVDESC   server;
        NTLOG_CLTDESC   client;
    } desc;

    NTLOG_BUFFER    logbuffer;
    NTLOG_BUFFER    cvtbuffer;

} NTLOG_DESC;

/* Prototypes *****************************************************************/

#ifdef __cplusplus
}
#endif


#endif
