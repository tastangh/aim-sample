/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Log.c

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

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#if defined(__unix)  || defined(_VXBUS)
# include <unistd.h>
# include <syslog.h>
# include <fcntl.h>
#endif



#include "Ai_Logdef.h"
#include "Ai_Logclt.h"
#include "Ai_Logsrv.h"
#include "Ai_Validate.h"
#include "Ai_Log.h"
#include "Ai_NotUsed.h"
#include "Ai_Logindex.h"
#if NTLOG_USE_SEMAPHORES
# include "Ai_Sem.h"
#endif
#include "Ai_Date.h"
#include "Ai_Time.h"

/* Constants ******************************************************************/

#define FMT_FILENAME    "log%05d"   /*!< General log file filename format   */
#define TIMEOUT_WAIT        ANS_CMD_RESP_TIMEOUT_MS  /*!< Log semaphore time-out [ms]        */
#define OPENMODE_LEN        4       /*!< Length of openmode-format          */

#define DEFAULT_LOGSIZE_MAX 100000  /*!< Maximum size of logfile (bytes)    */
#define MIN_LOGFILE_SIZE    1000    /*!< Minimum size of logfile (bytes)    */
#define DEFAULT_LOGFILES    10      /*!< Default amount of logfiles         */

#define BUFFERSIZE          1024    /*!< Size of the internal log buffer.   */

#define BUFFERSIZE_CVT  (BUFFERSIZE * 4)
                                    /*!< Size of the internal               */
                                    /*!< conversion log buffer.             */

#define EVENT_ID    0               /*!< Event Identifier                   */

#define SYMBOLIC_NL "<NL>"          /*!< Symbolic constant for replacement  */
                                    /*!< of the newline character.          */
#define SYMBOLIC_CR "<CR>"          /*!< Symbolic constant for replacement  */
                                    /*!< of the 'CR' character.             */

#define FMT_LOGENTRY_1S "%s\n"      /*!< Output-Format of log-entry         */

#define INITIALIZED_MAGIC   0xc29da0f2L
                                    /*!< "Magic" value to be used for the   */
                                    /*!< descriptor initialization indicator */

#if defined(__unix) || defined(TARGET_LINUX) 
# define NULL_DEVICE    "/dev/null" /*!< Null-Device                        */
#elif defined(_VXBUS)
# define NULL_DEVICE    "/null"     /*!< Null-Device                        */
#elif defined(WIN32)
# define NULL_DEVICE    "NUL:"      /*!< Null-Device                        */
#else
#error "No null device defined for this target"
#endif

/* Macros *********************************************************************/

#if NTLOG_USE_SEMAPHORES
/**
 * This macro defines the specified variable if support of semaphores is
 * activated.
 */
# define DEF_SEMAVAR(var)   var ;
#else
# define DEF_SEMAVAR(var)   /* nothing */
#endif

#if NTLOG_USE_SEMAPHORES
/**
 * This macro checks whether the specified semaphore counter has one of the
 * valid values "0" or "1".
 */
#define IS_VALID_SEMCOUNTER(sem)    ((sem) == 0 || (sem) == 1)
#endif

/**
 * This macro checks whether the specified log tape is valid
 */
#define IS_LOGTYPE(logtype)                             \
    ((logtype) >= NTLOG_TYPE_STANDALONE &&              \
    (logtype) < NTLOG_TYPE_RESERVED)

/**
 * This macro checks whether the specified log facility type data descriptor
 * is initialized. It return 0 if not initalized, else !0.
 */
#define IS_INITIALIZED(logdesc_ptr) \
    ((logdesc_ptr)->initialized == INITIALIZED_MAGIC)

/**
 * This macro determines whether the given index is valid or not.
 */
#define ISVALID_INDEX(index)                        \
    ((index >= NTLOG_INDEX_FIRST && index <= NTLOG_INDEX_MAX))

#if NTLOG_USE_SEMAPHORES
# define ENTER(havelock)                                    \
    {   (havelock) = 0;                                     \
        if (!mg_emergency)                                  \
        {                                                   \
            if (ntsem_wait(NTSEM_ID_NTLOG, TIMEOUT_WAIT)    \
                != NTSEM_RET_OK)                            \
                mg_emergency = 1;                           \
            else                                            \
            {                                               \
                (havelock) = 1;                             \
            }                                               \
        }                                                   \
    }
#else
# define ENTER(havelock)    /* nothing */
#endif

#if NTLOG_USE_SEMAPHORES
# define LEAVE(havelock)                                    \
    {   if ((havelock) == 1)                                \
        {                                                   \
            (void) ntsem_signal(NTSEM_ID_NTLOG);            \
            (havelock) = 0;                                 \
        }                                                   \
    }
#else
# define LEAVE(havelock)    /* nothing */
#endif

#if NTLOG_USE_SEMAPHORES
# define RETURN(havelock,what)                              \
{                                                           \
    LEAVE(havelock);                                        \
    return (what);                                          \
}
#else
# define RETURN(havelock,what)  return (what)
#endif

/* Types **********************************************************************/

/**
 * Description of the filename-variable of the log-file
 */
typedef NTLOG_FILENAME  FILENAME;

/**
 * Type of the openmode-variable
 */
typedef char    OPENMODE[OPENMODE_LEN + 1];

/* Variables ******************************************************************/

/**
 * This variable stores the file pointer of the logfile
 */
static  FILE    *mg_logfile;

/**
 * This variable stores the currently active loglevels
 */
static  NTLOG_LEVEL mg_loglevel;

/**
 * This variable stores the activated devices
 */
static  NTLOG_DEVICE    mg_devices = NTLOG_DEVICE_NONE;

/**
 * Flag: Enable/disable the cooked mode. In cooked mode, CR+LF sequences
 * get translated into plain text.
 */
static int      mg_cookedModeEnabled = 0;

/**
 * This variable stores the maximum size of the log file
 */
static  int     mg_maxlogfilesize = DEFAULT_LOGSIZE_MAX;

/**
 * This variable stores the maximum amount of logfile
 */
static  int     mg_logfiles = DEFAULT_LOGFILES;

/**
 * Handle for the null-device
 */
static  FILE    *mg_nulldevice;

/**
 * This variable specifies, if set, that a emergency event has
 * happened.
 */
static  int mg_emergency = 0;

/**
 * This variable stores the ntlog basic initialisation status.
 */
static  int mg_isinitialised = 0;

/**
 * This variable holds the registered log descriptor for global
 * user, if any. In case of no registered descriptor this variable
 * will be set to NULL.
 */
static  NTLOG_DESC  *mg_registered_desc = NULL;

/**
 * This variable contains the status of the log semaphore
 * initialization.
 */
#if NTLOG_USE_SEMAPHORES
static  int mg_sema_initflag = 0;
#endif

/* Prototypes *****************************************************************/

static  int     mg_open (NTLOG_INDEX    index);

static  int mg_close (void);

static  void    mg_convertnewlines (/*@out@*//*@unique@*/
    char        *dest,
/*@unique@*/
    char const  *source);

#if 0
static  int     mg_getdestsize (char const  *buffer);
#endif

static  int     mg_isdevice (
    NTLOG_DEVICE    device,
    NTLOG_LEVEL     level);

static  void    mg_syslog(
    NTLOG_LEVEL loglevel,
    char        *data);

#ifdef WIN32
static  WORD    mg_level2type (NTLOG_LEVEL level);
#endif

static  int     mg_adjust_index (void);

static  void    mg_log_timestamp (FILE  *fp);

static  int     mg_index2filename (
    FILENAME    filename,
    NTLOG_INDEX index);

static  int     mg_log2file (char const *message);

static  int     mg_reallocbuf (
    NTLOG_BUFFER    *bufptr,
    size_t      nbytes);

#if NTLOG_USE_SEMAPHORES
static  int mg_semainit (void);
#endif

static  int     mg_iswriteable (FILE *fp);

/* Functions ******************************************************************/

/***************************************************************************//**
 This function opens the all log devices for the registration of the
 log data.

 The log file index for the ring of log files is also provided to this
 function.

 If the log file index differs from NTLOG_INDEX_LASTUSED, the corresponding
 log file will be overwritten.

 If the log file index has the value NTLOG_INDEX_LASTUSED, the last
 recently used log file is continued.

 If the log file index of the last recently used log file could not be
 determined, the log file index '0' is used.

 Note: In case of the log facility is to be operated in either client
 or server mode, rather use "ntlog_open" instead.
*******************************************************************************/
extern  int     ntlog_open (
    NTLOG_INDEX index)  /*!< [in] log file index            */
{
    /* Ensure that the basic initialization is done.        */
    if ( ntlog_initialize() )
        return !0;      /** \return !0 - initialization failure */

#if NTLOG_USE_SEMAPHORES
    /* Try to generate the semaphore to block the printf commands   */
    if (mg_semainit())
        return !0;
#endif

    /* Open the current log file.                   */
    if ( (mg_devices & NTLOG_DEVICE_LOGFILE) != 0 )
    {
        if (mg_open(index))
            return !0;
    }

    return 0;   /** \return 0, if function completes successful */
}

/***************************************************************************//**
 This function initializes for operation in the specified mode.

 Notes:
 o In case of a specified "client" operation mode the specified
   FIFO is opened in write-only mode, which will block until
   the log server is connected. The argument "logindex" is ignored here.
 o In case of a specified "server" operation mode first the specified
   "logindex" is used to open or create the configured log files
   by a call of "ntlog_open".
   Then the specified FIFO is opened in non-blocking read-only mode.
   A following call of "ntlog_server" is required to start the
   log server operational mode.
 o In case of a specified standard operational mode the specified
   FIFO is ignored. Instead this function will result in a call to
   the standard "ntlog_open" function only.

 Any occuring errors will be logged to "stderr" if specified through
 the "logerrors" flag.
*******************************************************************************/
extern  int     ntlog_open4 (/*@out@                                        */
    NTLOG_DESC  *logdesc,   /*!< [out] Log facility descriptor              */
                            /*!< [out] to be initialized.                   */
    NTLOG_TYPE  logtype,    /*!< [in] Log facility operational              */
                            /*!< [in] mode type                             */
    NTLOG_INDEX logindex,   /*!< [in] Log file index for server or standard */
                            /*!< operational mode.                          */
/*@null@*/
    NTLOG_FIFONAME  logfifo,/*!< [in] File system path to the FIFO in       */
                            /*!< client or server mode (may be specified    */
                            /*!< as NULL for standard operational mode).    */
    int     log2stderr)     /*!< [in] Flag: log errors to stderr            */
{
    NOTUSED_PTR(logfifo);

    /* Validate the descriptor pointer.                                     */
    VALIDATE_PTR(logdesc, !0);  /** \return !0 - invalid NULL reference     */

    /* Ensure that the basic initialization is done.                        */
    if ( ntlog_initialize() )
        return !0;          /** \return !0 - initialization failure         */

    /* Initialize the descriptor.                                           */
    (void) memset(logdesc, 0, sizeof(*logdesc));
    logdesc->logtype = logtype;

    /* Make sure that the operational mode type is valid.                   */
    if (!IS_LOGTYPE(logtype))
    {
        if (log2stderr
        && mg_iswriteable(stderr))
            fprintf(stderr, "ntlog_open4: invalid log type!\n");

        return !0;      /** \return !0 - invalid log type                   */
    }

    switch (logtype)
    {
    case NTLOG_TYPE_STANDALONE:
        /* In standard operational mode simply use ntlog_open().            */
        if (ntlog_open(logindex) != 0)
            return !0;  /** \return see ntlog_open()                        */

        break;
#if NTLOG_CC_CLIENTCODE
    case NTLOG_TYPE_CLIENT:
        /* Perform log client specific operations if necessary.             */
        if (ntlogclt_open(
            &(logdesc->desc.client), logfifo, log2stderr) != 0)
            return !0;  /** \return see ntlogclt_open()                     */

        break;
#endif
#if NTLOG_CC_SERVERCODE
    case NTLOG_TYPE_SERVER:
        /* Perform log server specific operations if necessary.             */
        if (ntlogsrv_open(
            &(logdesc->desc.server), logfifo, log2stderr) != 0)
            return !0;  /** \return see ntlogsrv_open() */

        break;
#endif
    default:
        /* Report an error if the specified log type is not supported in    */
        /* this compilation.                                                */
        if (log2stderr
        && mg_iswriteable(stderr))
        {
            fprintf(stderr,
            "ntlog_open4: log type %d not supported!\n", logtype);
        }

        return !0;      /** \return !0 - log type not supported in this     */
                        /** compilation.                                    */
    }

    logdesc->initialized = INITIALIZED_MAGIC;

    return 0;           /** \return 0 - success                             */
}

/***************************************************************************//**
 This function performs an emergency log with no synchronsiation
 facility (mutex semaphores) applied.

 NOTE: Further log operations will not be synchronized anymore, since
 synchronization will be statically disabled.
*******************************************************************************/
extern  void        ntlog_emergency (
    NTLOG_LEVEL     level,      /*!< [in] log level                         */
    char const      *format,    /*!< [in] format string                     */
    char const      *logstr)    /*!< [in] message                           */
{
    /* Disable synchronization.                                             */
    mg_emergency = !0;

    /* Log the message.                                                     */
    (void) ntlog_printf(level, format, logstr);
}

/***************************************************************************//**
 This function sets the log facilities internal non-blocking "emergency
 status" to the specified value.
*******************************************************************************/
extern  void        ntlog_setemergency (
    int             status)     /*!< [in] new status (0=off, else on)       */
{
    mg_emergency = status != 0;
}

/***************************************************************************//**
 This function retrieves the internal "emergency status".

 A emergency status generally marks problems with the internal
 synchronization (mutex semaphore) mechanism.
*******************************************************************************/
extern  int     ntlog_isemergency (void)
{
    return mg_emergency;        /** \return 0 - okay, else emergency        */
}

/***************************************************************************//**
 This function registers the given log data to the log devices
 assigned to the given log level if the given log level is activated.

 Each log data entry is preceded by a time stamp and terminated by
 a newline for the registration onto the ring of log files.

 The log data is produced by formatting the variable argument list
 with the output format like the standard fprintf function.

 The log data is additionally logged to the system-specific event log,
 if the appropriate log level is set, and the system is supported
 by the current version of the log facility.

 In case of a registered global log facility descriptor reference
 (see ntlog_register) this function will act like ntlog_printf2().
*******************************************************************************/
int ntlog_printf (
    NTLOG_LEVEL level,      /*!< [in] log level                             */
/*@printflike@*/
    char const  *format,    /*!< [in] output format                         */
    ...)                    /*!< [in] variable argument list                */
{
    DEF_SEMAVAR(int l_havelock = 0)
    va_list arglist;
    int     cnt = -1;
    int     cookedMode;
    char    *bufptr = NULL;
    char    *cvtptr = NULL;
    char    *pLogMessage = NULL;

    /* Check the parameters                                                 */
    if (format == NULL)
        return -1;      /** \return -1 - invalid parameters                 */

    /* Ensure that the basic initialisation is done.                        */
    if ( ntlog_initialize() )
        return -1;      /** \return -1 - initialisation failure             */

    /* Check if log level is set and the log data has to be logged          */
    if (!ntlog_level(level))
        return 0;       /** \return 0 - log level is disabled               */

    /* Check how many characters would by produced                          */
    if (!mg_iswriteable(mg_nulldevice))
        return -1;      /** \return -1 - no write permission                */

    va_start(arglist, format);
    VALIDATE_PTR(mg_nulldevice, -1);
    ENTER(l_havelock);
    cookedMode = mg_cookedModeEnabled;
    cnt = vfprintf(mg_nulldevice, format, arglist);
    LEAVE(l_havelock);
    va_end(arglist);

    if (cnt < 1)
        return 0;

    /* Allocate enough memory to hold the log data.                         */
    ++cnt;
    ENTER(l_havelock);
    bufptr = (char *) malloc((size_t) cnt);
    LEAVE(l_havelock);
    if (NULL == bufptr)
        return -1;      /** \return -1 - out of memory                      */

    /* Allocate enough memory to hold the converted log data.               */
    if ( cookedMode )
    {
        ENTER(l_havelock);
        cvtptr = (char *) malloc((size_t) (cnt * 4));
        if (NULL == cvtptr)
        {
            free(bufptr);
            LEAVE(l_havelock);
            return -1;      /** \return -1 - out of memory                      */
        }
        LEAVE(l_havelock);
    }

    /* Write the log data line to the log buffer. Then convert any          */
    /* embedded newline and carriage return characters.                     */
    va_start(arglist, format);
    ENTER(l_havelock);
    (void) vsprintf(bufptr, format, arglist);
    LEAVE(l_havelock);
    va_end(arglist);

    if ( cookedMode )
    {
        mg_convertnewlines(cvtptr, bufptr);
        pLogMessage = cvtptr;
    }
    else
    {
        pLogMessage = bufptr;
    }

    /* In case of a registered log descriptor now simply act as             */
    /* log_printf2.                                                         */
    if (mg_registered_desc != NULL)
    {
        cnt = ntlog_printf2(mg_registered_desc, level, "%s", bufptr);

        /* Free the allocated memory.                                       */
        ENTER(l_havelock);
        free(bufptr);
        if ( NULL != cvtptr )
            free(cvtptr);
        LEAVE(l_havelock);
        return cnt;     /** \return see ntlog_printf2()                     */
    }

    /* Write the log entry to the ring of log files if necessary.           */
    if (mg_isdevice(NTLOG_DEVICE_LOGFILE, level))
    {
        (void) mg_log2file(pLogMessage);
    }

    /* Write the log entry to the stdout and the system-specific            */
    /* event log, if necessary.                                             */
    if (mg_isdevice(NTLOG_DEVICE_STDOUT, level)
    &&  mg_iswriteable(stdout))
    {
        ENTER(l_havelock);
        mg_log_timestamp(stdout);
        fprintf(stdout, FMT_LOGENTRY_1S, pLogMessage);
        LEAVE(l_havelock);
    }

    ENTER(l_havelock);
    if (mg_isdevice(NTLOG_DEVICE_EVENTLOG, level))
        mg_syslog(level, pLogMessage);

    /* Free the allocated memory.                                           */
    free(bufptr);
    if ( NULL != cvtptr )
        free(cvtptr);
    LEAVE(l_havelock);

    return cnt;         /** \return Number of written characters            */
}

/***************************************************************************//**
 This function registers the given log data to the log devices
 assigned to the given log level if the given log level is activated.

 Each log data entry in the log file is preceded by a time stamp and
 terminated by a newline.

 The log data is produced by formatting the variable argument list
 with the output format like the standard fprintf function.

 The log data is additionally logged to the system specific event log,
 if the appropriate log level is set, and the system is supported
 by the current version of the log facility.
*******************************************************************************/
extern  int     ntlog_printf2 (
    NTLOG_DESC  *logdesc,   /*!< [in,out] Log facility descriptor           */
    NTLOG_LEVEL level,      /*!< [in] log level                             */
/*@printflike@*/
    char const  *format,    /*!< [in] output format                         */
    ...)                    /*!< [in] variable argument list                */
{
    DEF_SEMAVAR(int l_havelock = 0)
    va_list arglist;
    int cnt = -1;

    /* Check the parameters                 */
    VALIDATE_PTR(logdesc, -1);
    VALIDATE_PTR(format, -1);   /** \return -1 - parameters are invalid     */
    if (!IS_INITIALIZED(logdesc))
        return -1;          /** \return -1 - uninitialized descriptor       */

    /* Ensure that the basic initialisation is done.                        */
    if (ntlog_initialize())
        return -1;      /** \return -1 - initialisation failure             */

    /* Check if log level is set and the log data has to be                 */
    /* logged                                                               */
    if (!ntlog_level(level))
        return 0;       /** \return 0 - log level is disabled               */

    /* In case of a log client simply log the data unconverted              */
    /* to the server.                                                       */
    if (logdesc->logtype == NTLOG_TYPE_CLIENT
    && mg_iswriteable(logdesc->desc.client.fpclient))
    {
        VALIDATE_PTR(logdesc->desc.client.fpclient, -1);
        ENTER(l_havelock);
        va_start(arglist, format);
        cnt = vfprintf(logdesc->desc.client.fpclient, format, arglist);
        va_end(arglist);

        /* Append a null byte to the stream.                                */
        (void) fputc(0, logdesc->desc.client.fpclient);
        (void) fflush(logdesc->desc.client.fpclient);
        LEAVE(l_havelock);
    }

    /* Check how many characters would by produced                          */
    VALIDATE_PTR(mg_nulldevice, -1);
    ENTER(l_havelock);
    va_start(arglist, format);
    cnt = mg_iswriteable(mg_nulldevice)
        ? vfprintf(mg_nulldevice, format, arglist) : -1;
    va_end(arglist);
    LEAVE(l_havelock);

    if (cnt < 1)
        return 0;

    /* Allocate enough memory to hold the log data and the                  */
    /* converted log data.                                                  */
    ++cnt;
    ENTER(l_havelock);
    if (mg_reallocbuf(&logdesc->logbuffer, (size_t) cnt) != 0
    ||  mg_reallocbuf(&logdesc->cvtbuffer, (size_t) (cnt * 4)) != 0)
    {
        LEAVE(l_havelock);
        return -1;      /** \return -1 - out of memory                      */
    }

    /* Write the log data line to the log buffer. Then convert              */
    /* any embedded newline and carriage return characters.                 */
    va_start(arglist, format);
    (void) vsprintf(logdesc->logbuffer.buffer, format, arglist);
    va_end(arglist);
    mg_convertnewlines(
        logdesc->cvtbuffer.buffer, logdesc->logbuffer.buffer);
    LEAVE(l_havelock);

    /* Write the log entry to the ring of log files if necessary.           */
    if (mg_isdevice(NTLOG_DEVICE_LOGFILE, level))
        (void) mg_log2file(logdesc->cvtbuffer.buffer);

    /* Write the log entry to the stdout and the system-specific            */
    /* event log, if necessary.                                             */
    if (mg_isdevice(NTLOG_DEVICE_STDOUT, level)
    &&  mg_iswriteable(stdout))
    {
        ENTER(l_havelock);
        mg_log_timestamp(stdout);
        fprintf(stdout, FMT_LOGENTRY_1S, logdesc->cvtbuffer.buffer);
        LEAVE(l_havelock);
    }

    ENTER(l_havelock);
    if (mg_isdevice(NTLOG_DEVICE_EVENTLOG, level))
        mg_syslog(level, logdesc->cvtbuffer.buffer);
    LEAVE(l_havelock);

    return cnt;         /** \return Number of written characters            */
}

/***************************************************************************//**
 This function closes the log facility opened for the specified mode.

 In case of client or server mode operation the opened FIFO is closed.
*******************************************************************************/
extern  int     ntlog_close4 (/*@only@*/
    NTLOG_DESC  *logdesc,   /*!< [in,out] Log facility descriptor           */
                            /*!< to be deinitialized.                       */
    NTLOG_TYPE  logtype,    /*!< [in] Log facility operational mode type    */
    int     log2stderr)     /*!< [in] Flag: log errors to stderr            */
{
    /* Validate the descriptor pointer.                                     */
    VALIDATE_PTR(logdesc, !0);  /** \return !0 - invalid NULL reference     */

    if (!IS_INITIALIZED(logdesc))
        return !0;      /** \return !0 - uninitialized descriptor           */

    /* Free any allocated buffers.                                          */
    if (logdesc->logbuffer.buffer != NULL)
        free(logdesc->logbuffer.buffer);
    if (logdesc->cvtbuffer.buffer != NULL)
        free(logdesc->cvtbuffer.buffer);

    memset(&logdesc->logbuffer, 0, sizeof(logdesc->logbuffer));
    memset(&logdesc->cvtbuffer, 0, sizeof(logdesc->cvtbuffer));

    switch (logtype)
    {
    case NTLOG_TYPE_STANDALONE:
        /* In standard operational mode simply use ntlog_close().           */
        if (ntlog_close() != 0)
            return !0;  /** \return see ntlog_close()                       */

        break;
#if NTLOG_CC_CLIENTCODE
    case NTLOG_TYPE_CLIENT:
        /* Perform log client specific operations if necessary.             */
        if (ntlogclt_close(logdesc, log2stderr) != 0)
            return !0;  /** \return see ntlogclt_close()                    */

        break;
#endif
#if NTLOG_CC_SERVERCODE
    case NTLOG_TYPE_SERVER:
        /* Perform log server specific operations if necessary.             */
        if (ntlogsrv_close(logdesc, log2stderr) != 0)
            return !0;  /** \return see ntlogsrv_close()                    */

        break;
#endif
    default:
        /* Report an error if the specified log type is not                 */
        /* supported in this compilation.                                   */
        if (log2stderr
        && mg_iswriteable(stderr))
            fprintf(stderr,
            "ntlog_close4: log type %d not supported!\n", logtype);

        return !0;      /** \return !0 - log type not supported in this     */
                        /** compilation.                                    */
    }
/*@-usereleased@*/
    logdesc->initialized = 0L;
/*@=usereleased@*/

    return 0;           /** \return 0 - success     */
}

/***************************************************************************//**
 This function closes all log devices.

 After a call to this function, no further log-data registration is possible.
*******************************************************************************/
extern  int     ntlog_close (void)
{
#if NTLOG_USE_SEMAPHORES
    /* Delete the blocking semaphore                                        */
    if (mg_sema_initflag)
        (void) ntsem_delete(NTSEM_ID_NTLOG);

    mg_sema_initflag = 0;
#endif
    mg_close();

    if ( NULL != mg_nulldevice )
    {
        (void) fclose(mg_nulldevice);
        mg_nulldevice = NULL;
    }

    mg_isinitialised = 0;

    return 0;           /** \return 0 on success                            */
}

/***************************************************************************//**
 This function checks, whether the given loglevel is selected.

 Note: Messages logged with a log level of LOG_LEVEL_SERVER are
 always considered to be selected.
*******************************************************************************/
extern  int     ntlog_level (
    NTLOG_LEVEL loglevel)   /*!< [in] Log-Level                             */
{
    unsigned long   thislevel;
    unsigned long   activelevel;

    thislevel = (unsigned long) loglevel;
    thislevel &= ~(unsigned long) NTLOG_DEVICE_ALL;
    activelevel = (unsigned long) mg_loglevel;

    /* Server generated log messages are always considered to be selected.  */
    if ((thislevel & NTLOG_LEVEL_SERVER) != 0)
        return !0;      /** \return !0 - log level is selected              */

    /* Else check whether the given log level is selected.                  */
    return (thislevel & activelevel) == thislevel;
                    /** \return A value different from 0, if log level is   */
                    /** selected                                            */
}

/***************************************************************************//**
 This function sets the log levels in the given log level description.

 The log data of the log levels given in the log level description are
 registered in the log files and system-dependent event logs.
*******************************************************************************/
extern  int     ntlog_lset (
    NTLOG_LEVEL level)      /*!< [in] log level description                 */
{
    int     newLevel;

     /* Set the given log levels                 */
    newLevel = ((int) level & ~NTLOG_DEVICE_ALL);

    mg_loglevel = (NTLOG_LEVEL) ((int) mg_loglevel | newLevel);
    return 0;           /** \return 0 on success                            */
}

/***************************************************************************//**
 This function clears the log levels in the given log level description.

 The log data of the log levels given in the log level descriptions are
 no longer registered on the log devices.
*******************************************************************************/
extern  int     ntlog_lclear (
    NTLOG_LEVEL level)      /*!< [in] log level description                 */
{
    int     newLevel;

    /* Clear the given log levels                                           */
    newLevel    = (level & ~NTLOG_DEVICE_ALL);
    newLevel    = ((int) mg_loglevel) & ~newLevel;
    mg_loglevel = (NTLOG_LEVEL) newLevel;

    return 0;           /** \return 0 on success                            */
}

/***************************************************************************//**
 This function activates the given devices for the registration of log data.

 All devices not set within the device parameter will be deactivated.

 Multiple devices may be combined by an OR-combination.
*******************************************************************************/
extern  int     ntlog_setdevice (
    NTLOG_DEVICE    device) /*!< [in] active log devices                    */
{
    mg_devices = (NTLOG_DEVICE) (device & NTLOG_DEVICE_ALL);

    return mg_devices;      /** \return activated devices                   */
}

/***************************************************************************//**
 This function determines the bit mask for the active configured log
 devices.
*******************************************************************************/
extern  NTLOG_DEVICE    ntlog_getdevices (void)
{
    return mg_devices;      /** \return bitmask of activated devices        */
}

/***************************************************************************//**
 This functions sets the maximum size of the logfile to the new logfile size.

 All further logging will consider the new logfile size.

 The logfile size is given in kbytes.
*******************************************************************************/
extern  int     ntlog_setlogfilesize (
    int     logfilesize)    /*!< [in] new size of logfile                   */
{
    /* Check the parameters                 */
    if (logfilesize * 1000 < MIN_LOGFILE_SIZE)
        return !0;  /** \return !0, if function failes                      */

    /* Set new logfilesize                  */
    mg_maxlogfilesize = logfilesize * 1000;

    return 0;       /** \return 0 on success                                */
}

/***************************************************************************//**
 This function enables or disables the cooked logging mode.
 In cooked mode, CR+LF sequences get translated into plaintext.
*******************************************************************************/
extern void     ntlog_setCookedMode (
    int         enabledFlag)        /*!< [in] enable/disable flag           */
{
    mg_cookedModeEnabled = (enabledFlag != 0);
}

/***************************************************************************//**
 This function sets the maximum amount of logfiles to the given
 amount of logfiles.

 All further logging will consider the new amount of logfiles.

 The enumeration value NTLOG_INDEX_MAX contains the maximum possible
 amount of logfiles.
*******************************************************************************/
extern  int     ntlog_setlogfiles (
    int         logfiles)   /*!< [in] max. number of logfiles               */
{
    /* Check parameters                     */
    if (logfiles < 0 || logfiles > NTLOG_INDEX_MAX)
        return !0;  /** \return !0, if function fails                       */

    /* Set new logfile size                                                 */
    mg_logfiles = logfiles;

    return 0;       /** \return 0, on success                               */
}

/***************************************************************************//**
 This function determines the current log file index and returns it.

 If the logindex file is invalid or does not exist, 0 is returned as
 the current logindex.
*******************************************************************************/
extern  int     ntlog_getindex (void)
{
    int ret;

    /* Return the current log file index                                    */
    if ((ret = logindex_read()) == LOGINDEX_FAIL)
        return 0;
            /** \return 0, if the logindex could not be determined          */

    if (!ISVALID_INDEX(ret))
        return 0;

    return ret;     /** \return Current logindex                            */
}

/***************************************************************************//**
 This function convertes the given log-index to the corresponding
 log-file name.
*******************************************************************************/
extern  int         ntlog_index2filename (/*@out@*/
    NTLOG_FILENAME  filename,   /*!< [out] log-file name                    */
    int             index)      /*!< [in] log-file index                    */
{
    /* Check the parameters.                                                */
    VALIDATE_PTR(filename, !0);

    return mg_index2filename(filename, (NTLOG_INDEX) index);
                    /** \return 0, if function is successful, !0 else       */
}

/***************************************************************************//**
 This function locks the current log file to enforce exclusive
 file access to the caller.

 Important: The log files are locked for any access until a following
 call to 'ntlog_unlock()' is performed. Repeated calls to 'ntlog_lock'
 may cause the software to fail or hang infinitely.
*******************************************************************************/
extern  void        ntlog_lock (void)
{
    DEF_SEMAVAR(int l_havelock = 0)

    /* Get the semaphore.                                                   */
    ENTER(l_havelock);

    /* Make sure that all file buffers are flushed.                         */
    if (mg_logfile != NULL)
        (void) fflush(mg_logfile);

    NOTUSED_INT(l_havelock);
}

/***************************************************************************//**
 This function unlocks the current log file formerly locked by a
 call to 'ntlog_lock'.

 Important: Repeated calls to 'ntlog_unlock' without prior calls to
 'ntlog_lock' may cause the software to fail or hang infinitely.
*******************************************************************************/
extern  void        ntlog_unlock (void)
{
    DEF_SEMAVAR(int l_havelock = 1)

    /* Release the semaphore.                                               */
    LEAVE(l_havelock);
}

/***************************************************************************//**
 This function prints the descriptive error text set by errno prefixed
 by the specified additional text to stderr if necessary.
*******************************************************************************/
extern  void        ntlog_errno (
    int             printflag,  /*!< [in] flag: output on/off               */
    char const      *text)      /*!< [in] additional text                   */
{
    if (!printflag)
        return;

    perror(text);
}

/***************************************************************************//**
 This function registers the specified initialised
 log facility descriptor for global use at any calls to "log_printf()".

 Note that there is only stored the specified reference by this function.
 So the caller must not destroy the appropriate descriptor before the
 very last use of the ntlog_printf function.

 See also ntlog_deregister()
*******************************************************************************/
extern  int     ntlog_register (
    NTLOG_DESC  *logdesc)   /*!< [in,out] log facility descriptor           */
{
    if (mg_registered_desc != NULL)
        return !0;      /** \return !0 - a registered descriptor already    */
                        /** is present                                      */

    mg_registered_desc = logdesc;

    return 0;           /** \return 0 - success                             */
}

/***************************************************************************//**
 This function deregisters a formerly registered global log facility
 descriptor reference.
*******************************************************************************/
extern  void        ntlog_deregister (void)
{
    mg_registered_desc = NULL;
}

/***************************************************************************//**
 This function opens a log file with the specified log index.
*******************************************************************************/
static  int     mg_open (
    NTLOG_INDEX index)  /*!< [in] Log file index                            */
{
    int         l_index;
    FILENAME    logfile;

    /* Check, whether logfile has already been openend                      */
    if (mg_logfile != NULL)
        return !0;

    /* If log-file index corresponds with NTLOG_INDEX_LAST_USED,            */
    /* determine the last used log-file index                               */
    l_index = (index == NTLOG_INDEX_LASTUSED) ? ntlog_getindex()
        : index;

    /* Create filename of the log-file                                      */
    if (mg_index2filename(logfile, (NTLOG_INDEX) l_index))
        return !0;

    /* If the log-index corresponds with NTLOG_INDEX_LASTUSED               */
    /* Log data must be appended to the log-file, otherwise                 */
    /* the log file will be overwritten.                                    */
    /* Therefore, the opening mode of the log-file must be determined       */

    /* Open the log-file and store the file pointer                         */
    if ((mg_logfile = fopen(logfile,
        (index == NTLOG_INDEX_LASTUSED) ? "a" : "w"))
        == NULL)
    {
        return !0;          /** \return !0 - file open error                */
    }

    /* Adjust the logindex file                                             */
    (void) logindex_write(l_index);

    return 0;               /** \return 0 - success                         */
}

/***************************************************************************//**
 This function closes the currently used log file
*******************************************************************************/
static  int mg_close (void)
{
    /* Check if log file is opened                                          */
    if ( NULL == mg_logfile )
        return 0;

    /* Close the Log file                                                   */
    if (fclose(mg_logfile) != 0)
    {
        if (mg_iswriteable(mg_logfile))
        {
            fprintf(mg_logfile, "FATAL ERROR: Logfile could not be closed!");
        }
        return !0;      /** \return !0, if function fails                   */
    }

    mg_logfile = NULL;

    return 0;           /** \return 0 on success                            */
}

/***************************************************************************//**
 This function replaces all newlines and carriage returns in the source
 buffer to symbolic constants. The converted buffer is written to the
 destination buffer.

 Note that the converted buffer can be larger by far. To guarantee that
 no memory violation occurs, the size of the destination buffer can
 be determined by the mg_getdestsize() function.

 The maximum size of the destination buffer is four times as large as
 the source buffer.

 Source and destination buffer must not overlap.
*******************************************************************************/
static  void        mg_convertnewlines (/*@out@*//*@unique@*/
    char            *dest,      /*!< [out] destination buffer               */
/*@unique@*/
    char const      *source)    /*!< [in] source buffer                     */
{
    VALIDATE_PTR_VOID(source);
    VALIDATE_PTR_VOID(dest);

    /* Initialize destination string                                        */
    *dest = '\0';

    while (*source != '\0')
    {
        switch(*source)
        {
          case '\n':    strcat(dest, SYMBOLIC_NL);
                break;
          case '\r':    strcat(dest, SYMBOLIC_CR);
                break;
          default:  strncat(dest, source, 1);
                break;
        }
        source++;
    }

    return;
}

#if 0 /*??PD not used */
/***************************************************************************//**
 Diese Funktion determines the size of the destination buffer needed to hold
 the newline converted log string of the given source buffer and returns
 it.
*******************************************************************************/
static  int     mg_getdestsize (
    char const  *buffer)    /*!< [in] source buffer                         */
{
    int total_len = 0;

    VALIDATE_PTR(buffer, 0);

    while (*buffer != '\0')
    {
        switch(*buffer)
        {
          case '\n':    total_len += strlen(SYMBOLIC_NL);
                break;
          case '\r':    total_len += strlen(SYMBOLIC_CR);
                break;
          default:  total_len++;
                break;
        }

        ++buffer;
    }

    return total_len;   /** \return length of destination buffer            */
}
#endif

/***************************************************************************//**
 This function determines, whether the given log level has
 to be logged in the given device or not.

 If the given device is not activated, the function returns logical false.
*******************************************************************************/
static  int         mg_isdevice (
    NTLOG_DEVICE    device, /*!< [in] device                                */
    NTLOG_LEVEL     level)  /*!< [in] log-level                             */
{
    int     deviceBits;

    deviceBits = (level & NTLOG_DEVICE_ALL);

    /* First check, if the given lock level has to be logged in             */
    /* the given device                                                     */
    if ((deviceBits & device) != device)
        return 0;

    /* Check if device is activated                                         */
    if ((device & mg_devices) != device)
        return 0;       /** \return 0, if log level does not have           */
                        /** to be logged onto the given device              */

    return !0;          /** \return 1, if loglevel has to be logged.        */
}

/***************************************************************************//**
 This function logs the log data to the system specific system's event log.
*******************************************************************************/
static  void        mg_syslog(
    NTLOG_LEVEL loglevel,   /*!< [in] Log level for logging                 */
    char        *data)      /*!< [in] Log data                              */
{
    VALIDATE_PTR_VOID(data);

#if defined(__unix) || defined(_VXBUS)

    /* Mask out all bit that do not specify message severity */
    loglevel &= NTLOG_LEVEL_ALL;

    /* Now map to syslog specific log level */
    switch(loglevel)
    {
        case NTLOG_LEVEL_NONE: loglevel = LOG_DEBUG; break;
        case NTLOG_LEVEL_ERROR: loglevel = LOG_ERR; break;
        case NTLOG_LEVEL_WARN: loglevel = LOG_WARNING; break;
        case NTLOG_LEVEL_DEBUG: loglevel = LOG_DEBUG; break;
        case NTLOG_LEVEL_INFO: loglevel = LOG_INFO; break;
        default: loglevel = LOG_DEBUG; break;
    }

    syslog(loglevel, "%.128s", data);
#endif
#ifdef WIN32_____
    ReportEvent(mg_eventlog,        /* Event-Log handle */
        mg_level2type(loglevel),    /* Type of Event    */
        0,              /* Source-Category  */
        EVENT_ID,           /* Event identifier */
        NULL,               /* User Security id */
        1,              /* Number of Msg-Strings*/
        0,              /* Size of BinData  */
        &data,              /* Message-String   */
        NULL);              /* BinData      */
#endif
#if defined(WIN32) && defined(OutputDebugStr)
    OutputDebugStr(data);
#elif defined(WIN32) && defined(OutputDebugString)
    OutputDebugStringA(data);
#endif
    return;
}

#if defined(WIN32) && 0
/***************************************************************************//**
 This function convertes the log level into the information type
 for the Windows NT event log.
*******************************************************************************/
static  WORD        mg_level2type (
    NTLOG_LEVEL level)  /*!< log level to convert                           */
{
    level &= ~NTLOG_DEVICE_ALL;

    if (level & NTLOG_LEVEL_ERROR)
        return EVENTLOG_ERROR_TYPE;
            /** \return EVENTLOG_ERROR_TYPE, if log level indicates an      */
            /** error                                                       */

    if (level & NTLOG_LEVEL_WARN)
        return EVENTLOG_WARNING_TYPE;
            /** \return EVENTLOG_WARNING_TYPE, if log level indicates a     */
            /** warning                                                     */

    return EVENTLOG_INFORMATION_TYPE;
            /** \return EVENTLOG_INFORMATION_TYPE, else                     */
}
#endif

/***************************************************************************//**
 This function changes the currently used log file, if the currently used
 log file entries exceeds the maximum amount of log file entries for
 the log files.

 Therefore the old log file is closed and the new one is opened.
 The Log file index file is also adjusted.

 If the index of the log file exceeds the maximum logfile index,
 the logindex 0 is used for the next log file.
*******************************************************************************/
static  int     mg_adjust_index (void)
{
    long int    size;
    int     idx;

    /* Check whether logfile is openend                                     */
    if (mg_logfile == NULL)
        return !0;

    /* Determine the amount of log file entries                             */
    if ((size = ftell(mg_logfile)) < 0)
        return !0;

    /* If no change of log file is neccessary, quit the function            */
    if (size < mg_maxlogfilesize)
        return 0;

    /* Read the current log file index                                      */
    idx = logindex_read();

    /* Close the current logfile                                            */
    if (mg_close())
        return !0;

    /* Adjust the logindex.                                                 */
    /* If the current logindex is invalid, the new logindex                 */
    /* is the minimum logindex value.                                       */
    idx = (ISVALID_INDEX(idx) && idx < mg_logfiles - 1) ? idx + 1
        : NTLOG_INDEX_FIRST;

    /* Open the new log file. Hereby, the logindex file is                  */
    /* automatically adjusted.                                              */
    if (mg_open((NTLOG_INDEX) idx))
        return !0;      /** \return !0 on failure                           */

    return 0;           /** \return 0 on success                            */
}

/***************************************************************************//**
 This function logs the the time stamp of the system's time
 to the given file descriptor.
*******************************************************************************/
static  void        mg_log_timestamp (
        FILE        *fp)    /* [in] file descriptor                         */
{
    CDATE       cdate;
    CDATE_OUT   strdate;
    CTIME       ctime;
    CTIME_OUT   strtime;

    VALIDATE_PTR_VOID(fp);

    /* Determine the system's date                                          */
    cdate_get(&cdate);
    cdate_date2out_iso(strdate, &cdate);

    if (!mg_iswriteable(fp))
        return;

    /* Log the system's date to the file descriptor                         */
    fprintf(fp, "%s ", strdate);

    /* Determine the system's time                                          */
    ctime_get(&ctime);
    ctime_time2out(strtime, &ctime);

    /* Log the system's time to the file descriptor                         */
    fprintf(fp, "%s ", strtime);
}

/***************************************************************************//**
 This function convertes a log file index into the corresponding
 log file name.
*******************************************************************************/
static  int     mg_index2filename (
    FILENAME    filename,   /*!< [out] log file name                        */
    NTLOG_INDEX index)      /*!< [in] log file index                        */
{
    VALIDATE_PTR(filename, !0);

    if (!ISVALID_INDEX(index))
        return !0;  /** \return !0, if function fails                       */

    if (sprintf(filename, FMT_FILENAME, index) != NTLOG_FILENAME_LEN)
        return !0;

    return 0;       /** \return 0, if function is successful                */
}

/***************************************************************************//**
 This function writes the specified log data string into the ring of
 log files. The specified string is prefixed by the current time
 stamp and terminated by a newline character.

 If necessary the index into the ring of log files is adjusted.

 Note: This function waits for the log file to get exclusively available
 if the use of semaphores is activated at compile time.
*******************************************************************************/
static  int     mg_log2file (
    char const  *message)   /*!< [in] message to be logged                  */
{
    DEF_SEMAVAR(int l_havelock = 0)

    VALIDATE_PTR(message, -1);

    /* Check, if log file is opened                                         */
    if (mg_logfile == NULL
    || !mg_iswriteable(mg_logfile))
        return -1;      /** \return -1 - log file is closed                 */

    /* Wait for the semaphore                                               */
    ENTER(l_havelock);

    /* If log file has reached the maximum amount of                        */
    /* log data, create and open the new log file                           */
    if (mg_adjust_index())
        RETURN(l_havelock, -1); /** \return -1 -access to the log file      */
                    /** \return failed                                      */

    /* Write the time stamp and the specified string to the log             */
    /* file.                                                                */
    mg_log_timestamp(mg_logfile);
    fprintf(mg_logfile, FMT_LOGENTRY_1S, message);
    (void) fflush(mg_logfile);

    /* Release the semaphore.                                               */
    LEAVE(l_havelock);

    return 0;           /** \return 0 - success                             */
}

/***************************************************************************//**
 This function reallocates the specified log buffer to hold the specified
 number of bytes.
*******************************************************************************/
static  int     mg_reallocbuf (
    NTLOG_BUFFER    *bufptr,    /*!< [in,out] log buffer                    */
    size_t          nbytes)     /*!< [in] required number of bytes          */
{
    char    *newptr;

    VALIDATE_PTR(bufptr, !0);

    /* Do nothing if there is enough space allocated.                       */
    if (bufptr->buffer != NULL
    && bufptr->buffersize >= nbytes)
        return 0;       /** \return 0 - success                             */

    newptr = (bufptr->buffer == NULL)
        ? (char *) malloc(nbytes)
        : (char *) realloc(bufptr->buffer, nbytes);

/*@-usereleased@*/
    if (newptr == NULL)
        return !0;      /** \return !0 - out of memory                      */

    bufptr->buffer      = newptr;
    bufptr->buffersize  = nbytes;
/*@=usereleased@*/

    return 0;
}

/***************************************************************************//**
 This function performs the basic initialization.
 Note: This function may be explicitly called at startup to enable the default
 log settings. If uncalled, this function implicitly  gets called at the
 very first ntlog_printf().
*******************************************************************************/
extern  int     ntlog_initialize (void)
{
    if (mg_isinitialised)
        return 0;       /** \return 0 - success                             */

    /* Open the system's NULL device                                        */
    if ((mg_nulldevice = fopen(NULL_DEVICE, "w")) == NULL)
        return !0;      /** \return !0 - failure                            */

#if NTLOG_USE_SEMAPHORES
    /* Try to generate the semaphore to block the printf commands           */
    if (mg_semainit())
        return !0;
#endif

    ntlog_setdevice(NTLOG_DEVICE_NONE);

    /* Activate the default log level. Errors always are logged.            */
    if ( 0 == (int) mg_loglevel )
    {
        ntlog_lset(NTLOG_LEVEL_ERROR);
        if ( mg_iswriteable(stdout) )
        {
            ntlog_setdevice(NTLOG_DEVICE_STDOUT);
        }
    }
#if defined(WIN32) || defined(__unix) || defined(_VXBUS)
    ntlog_setdevice((NTLOG_DEVICE) (mg_devices | NTLOG_DEVICE_EVENTLOG));
#endif

    mg_isinitialised = !0;

    return 0;
}

#if NTLOG_USE_SEMAPHORES
/***************************************************************************//**
 This function ensures that the log semaphore is initialized.
*******************************************************************************/
static  int mg_semainit (void)
{
    if (mg_sema_initflag)
        return 0;

    /* Try to generate the semaphore to block the printf commands           */
    if (ntsem_create(NTSEM_ID_NTLOG, 1) != NTSEM_RET_OK)
        return !0;      /** \return !0 - error                              */

    mg_sema_initflag = !0;

    return 0;           /** \return 0 - success                             */
}
#endif

/***************************************************************************//**
 This function checks whether the specified file descriptor is
 valid for a write operation.
*******************************************************************************/
static  int     mg_iswriteable (
    FILE        *fp)    /*!< [in] file descriptor           */
{
    int flags = 0;
    int fd = -1;

    /* A NULL reference is always invalid.                                  */
    if (NULL == fp)
        return 0;       /** \return 0 - not writeable                       */

    /* Check whether there is an integer descriptor available.              */
    if ((fd = fileno(fp)) < 1)
        return 0;

    /* Check whether a file error is pending.                               */
    if (ferror(fp) != 0)
        return 0;

#if defined(__unix) || defined(_VXBUS)
    /* Check whether the file is writeable.                                 */
    if ((flags = fcntl(fd, F_GETFL)) < 0)
        return 0;

    if ((flags & O_RDWR) == O_RDWR
    ||  (flags & O_WRONLY) == O_WRONLY)
        return !0;      /** \return !0 - writeable                          */
#elif defined(WIN32)
//    flags = fp->_flag;
//    if ( (flags & _IORW) || (flags & _IOWRT) )
//    _flag not available since Visual 2015 -> return always !0 - writeable      
        return !0;
#endif

    return 0;
}

#if 0
/* Module history **************************************************************
 *
 * $Log: ntlog.c $
 * Revision 1.18  1999/07/15 07:13:22  duerr
 * Added semaphore locking to ntlog_printf.
 *
 * Revision 1.17  1999/07/01 14:21:02  duerr
 * Implemented addtional file pointer write permission checks
 * before each write operation (new function mg_iswriteable).
 *
 * Revision 1.16  1999/07/01 06:43:59  duerr
 * Added validation check code for file handles before file write
 * operations. Semaphore deinitialization now is done only once.
 *
 * Revision 1.15  1999/06/30 09:13:16  duerr
 * Added semaphore support for log client mode.
 *
 * Revision 1.14  1999/06/29 12:33:15  duerr
 * Prepared code for LCLint.
 *
 * Revision 1.13  1999/05/12 10:08:41  duerr
 * Now a null byte is appended to each message sent to the log server.
 *
 * Revision 1.12  1999/05/10 13:44:26  duerr
 * Write operations to the log file now only happen if explicitely configured.
 *
 * Revision 1.11  1999/05/10 13:06:51  duerr
 * Added call to fflush after each write operation to log server.
 *
 * Revision 1.10  1999/05/10 12:13:06  duerr
 * Corrected error in new _deregister function name definition.
 *
 * Revision 1.9  1999/05/10 11:57:17  duerr
 * Added new _register and _deregister functions for global
 * log facility descriptor activation.
 *
 * Revision 1.8  1999/05/10 10:22:33  duerr
 * Added support for messages logged to standard output without _open
 * required.
 *
 * Revision 1.7  1999/05/05 15:50:06  duerr
 * Changed algorithm of _level function for simpler server log level
 * recognition.
 *
 * Revision 1.6  1999/05/05 11:32:21  duerr
 * Updated version imported from BSSlog
 *
 * Revision 1.5  1999/05/05 08:32:54  duerr
 * Corrected log server specific code and log level detection.
 *
 * Revision 1.4  1999/04/30 14:53:33  schoenma
 * Removed compiler errors.
 *
 * Revision 1.3  1999/04/26 09:59:35  duerr
 * Removed constants for "overwrite" and "append" file modes used
 * as arguments for fopen(2).
 * Implemented new function _printf2 which logs to a configured
 * server/client or regular log descriptor.
 * Changed function _printf: No static buffer is used anymore to
 * allow reentrance. The log descriptors now contain private
 * data buffers.
 *
 * Revision 1.2  1999/04/24 17:20:35  duerr
 * Added new functions _open4 and close4.
 *
 * Revision 1.1  1999/04/18 01:53:57  duerr
 * Initial revision
 *
*******************************************************************************/
#endif
