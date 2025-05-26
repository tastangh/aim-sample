/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Sockll.c
 *
 * \brief socket low level functions
 * This module provides functions internally used by the
 * Ai_Socket modules.
*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#if defined(__unix) || defined(TARGET_OS2) || defined(_VXBUS)
# include <sys/time.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
#endif
#if defined(WIN32)
# include <winsock2.h>
# pragma warning(disable:4127)  // conditional is constant (FD_SET)
#endif

#include "Ai_Validate.h"
#ifdef HAVE_NTLOG
# include "ntlog.h"
# include "ntscklog.h"
#endif

#include "Ai_Sockll.h"
#include "Ai_Socket.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

#if defined(HAVE_NTLOG) && defined(LOG_SELECT_ERRORS)
 static void        mg_logselecterror (
    int     errorcode); /* error code       */
#endif

/* Variablen ******************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 *  This function translates the specified time-out value [ms] into
 *  the specified 'timeval' structure.
*******************************************************************************/
extern  struct timeval  *ntsockll_timeout2timeval (/*@returned@*/
    struct timeval  *timestruct,    /*!< [out] 'timeval' structure          */
    unsigned long   timeout)        /*!< time-out in milliseconds,          */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
{
    unsigned long   sec;
    unsigned long   usec;

    VALIDATE_PTR(timestruct, NULL); /** \return NULL - invalid NULL         */
                                    /** reference                           */

    /* Check whether to wait infinitely.                                    */
    /* If there is infinite waiting specified, make the reference           */
    /* to the time-out values a null pointer.                               */
    if (timeout == NTSOCKET_TIMEOUT_INFINITE)
        return (struct timeval *) NULL;
                                    /** \return NULL - wait infinitely      */

    sec  = timeout / 1000L;
    usec = (timeout % 1000L) * 1000L;
    timestruct->tv_sec  = (long int) sec;
    timestruct->tv_usec = (long int) usec;

    return timestruct;              /** \return initialized structure       */
}

/***************************************************************************//**
 *  This function makes the specified socket handle structure invalid.
*******************************************************************************/
extern  void        ntsockll_clearhandle (/*@out@*/
    NTSOCKET_HANDLE *sockethandle)  /*!< [in,out] socket handle     */
{
    VALIDATE_PTR_VOID(sockethandle);

    /* Initialize the entries.                  */
    memset(sockethandle, 0, sizeof(NTSOCKET_HANDLE));

    sockethandle->handle    = NTSOCKET_HANDLEINVALID;
    sockethandle->status    = SOCKETSTATUS_UNCONNECTED;
}

/***************************************************************************//**
 *  Diese Funktion sets the entries of the specified "ntsocket handle"
 *  to the specified values.
*******************************************************************************/
extern  void        ntsockll_setsockethandle (/*@out@*/
    NTSOCKET_HANDLE *sockethandle,  /*!< [out] destination handle           */
    AiSocketFD      handle,         /*!< [in] the phyiscal socket handle    */
    int             status,         /*!< [in] the status of the handle      */
    NTSOCKET_STYLE  style,          /*!< [in] the communication style       */
    int             isclient,       /*!< [in] flag: socket is to be         */
                                    /*!< treated as client socket           */
    int             isserver)       /*!< [in] flag: socket is to            */
                                    /*!< initialized as 'server'            */
                                    /*!< socket                             */
{
    VALIDATE_PTR_VOID(sockethandle);

    /* Set each structure's value.                                          */
    sockethandle->initialized = INITIALIZED_MAGICVALUE;
    sockethandle->handle     = handle;
    sockethandle->status     = status;
    sockethandle->style      = style;
    sockethandle->isclient   = isclient;
    sockethandle->isserver   = isserver;
}

/***************************************************************************//**
 *  This function checks whether the socket layer is
 *  ready to send / receive data
 *  on the specified socket handle within the specified timeout.
 *
 *  The peek mode decides whether the incoming (receiving data) or outgoing
 *  (sending data) connection is checked.
*******************************************************************************/
extern  NTSOCKET_RET    ntsockll_peek (
    AiSocketFD      sockethandle,   /*!< [in] socket handle                 */
    unsigned long   timeout,        /*!< [in] timeout in [ms]               */
    PEEK_MODE       mode)           /*!< [in] Peek mode                     */
{
    struct timeval  l_timeout;
    struct timeval  *timeoutptr;
    fd_set          ready;
    int             nfound = -1;
    int             maxfd;
    int             errorcode = 0;

    /* Prepare the 'select' call:                                           */

    /* Setup the time out   values.                                         */
    timeoutptr = ntsockll_timeout2timeval(&l_timeout, timeout);

    /* Clear the 'select' bitstring.                                        */
    /* Enter the socket number into the bitstring.                          */
/* xx*/ FD_ZERO(&ready);
/* xx*/ FD_SET(sockethandle, &ready);

    /* Check the number of incoming connection requests within              */
    /* the specified time-out.                                              */
    maxfd = (int) sockethandle;
    switch (mode)
    {
    case PEEK_MODE_READ:
        nfound = select(maxfd + 1, &ready, FDSETNULL, FDSETNULL,
            timeoutptr);
        break;
    case PEEK_MODE_WRITE:
        nfound = select(maxfd + 1, FDSETNULL, &ready, FDSETNULL,
            timeoutptr);
        break;
    }

    /* Report general errors.                                               */
    if (nfound == -1)
    {
        errorcode = errno;
#if defined(HAVE_NTLOG) && defined(LOG_SELECT_ERRORS)
        mg_logselecterror(errorcode);
#endif
        /* If there occured an 'interrupted system call'                    */
        /* error when using POSIX threads, simply treat this as time-out.   */
        if (EINTR == errorcode)
            return NTSOCKET_RET_TIMEOUT;

        return NTSOCKET_RET_ESOCKET;    /** \return NTSOCKET_RET_ESOCKET    */
    }

    /* If a time-out occured, report it to the caller.                      */
    if (nfound == 0
    || !(FD_ISSET(sockethandle, &ready)))
        return NTSOCKET_RET_TIMEOUT;    /** \return NTSOCKET_RET_TIMEOUT    */

    return NTSOCKET_RET_OK;             /** \return NTSOCKET_RET_OK         */
}

#if defined(HAVE_NTLOG) && defined(LOG_SELECT_ERRORS)
/***************************************************************************//**
 *  Log the specified errno code as result of a select call.
*******************************************************************************/
static  void        mg_logselecterror (
        int         errorcode)      /*!< [in] error code                    */
{
    if (EINTR == errorcode)
    {
        (void) ntlog(ntsocket_log_debuglevel(),
            "%s select error(%d): %s",
            ntsocket_log_debugprefix(),
            errorcode,
            strnull(strerror(errorcode)));
        return;
    }

    (void) ntlog(ntsocket_log_warnlevel(),
        "%s select error(%d): %s",
        ntsocket_log_warnprefix(),
        errorcode,
        strnull(strerror(errorcode)));
}
#endif

#if 0
/* Aenderungsjournal ***********************************************************
 *
 * $Log: ntsockll.c $
 * Revision 1.9  2000/05/10 17:10:57  duerr
 * Changed logging of interrupted system call errors at select() to
 * use the configured debug level.
 *
 * Revision 1.8  1999/10/18 16:35:09  duerr
 * Changed logging to use new ntsocket_log_xxx functions.
 *
 * Revision 1.7  1999/08/16 07:46:19  duerr
 * Changed system include files for Win32.
 * Changed function interfaces data types to match those of "ntsocket".
 * Made use of "strnull" for select() error logging.
 *
 * Revision 1.6  1999/08/12 12:49:41  duerr
 * Added error logging via MSG_WARN with ntlog if HAVE_NTLOG is defined.
 *
 * Revision 1.5  1999/07/05 09:49:21  duerr
 * Improved _log_errno function
 *
 * Revision 1.4  1999/07/02 14:15:53  duerr
 * Added sys/types.h and unistd.h to the list of includes.
 *
 * Revision 1.3  1999/07/02 14:02:45  duerr
 * Added sys/time.h to list of standard includes.
 *
 * Revision 1.2  1999/06/17 05:32:34  duerr
 * Updated code for LCLint compliance.
 *
 * Revision 1.1  1999/05/08 10:46:35  klingler
 * Initial revision
 *
*******************************************************************************/
#endif
