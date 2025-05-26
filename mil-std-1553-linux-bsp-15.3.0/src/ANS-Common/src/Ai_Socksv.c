/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socksv.c
 *
 * This file contains definitions used for creating
 * server side sockets
*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "Ai_Validate.h"

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
#endif

#define NTSOCKET_C
#include "Ai_Socket.h"
#undef NTSOCKET_C

#include "Ai_Socksv.h"
#include "Ai_Sockll.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

#ifdef WIN32
# define SETSOCKOPT(a,b,c,d,e)  setsockopt(/*@-type@*/ (a) /*@=type@*/, (b), (c), (char *) (d), (e))
#else
# define SETSOCKOPT(a,b,c,d,e)  setsockopt((a), (b), (c), (d), (e))
#endif

/* Typen **********************************************************************/

#ifdef WIN32
typedef int socklen_t;
#endif

/* Prototypen *****************************************************************/

static  NTSOCKET_RET    mg_mkstreamserver (
    NTSOCKET            *ntsocket,
    unsigned int        port,
    int                 maxconnections);

static  NTSOCKET_RET    mg_mkdatagramserver (
    NTSOCKET            *ntsocket,
    unsigned int        port);

/* Variablen ******************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 *  This function creates a socket by use of the specified socket descriptor
 *  which acts as server listening to the specified port number
 *  for any incoming connection requests.
 *
 *  The 'maxconnections' parameter specifies the maximum length the queue
 *  of  pending connections may grow to (with connection orientated
 *  communication style, such as TCP, ... only).
 *  If a connection request arrives with the queue full the client may
 *  receive an error with  an indication of "ECONNREFUSED", or,
 *  if the underlying protocol supports retransmission,
 *  the request may be ignored so that retries may succeed.
 *
 *  On success the function 'ntsocket_accept' can be used to look out
 *  for incoming connection requests and accept these for further
 *  communication.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_createserver (
    NTSOCKET        *ntsocket,  /*!< [in,out] socket descriptor             */
    unsigned int    port,       /*!< [in] portnumber (positive              */
                                /*!< 16-bit integer)                        */
    int     maxconnections)     /*!< [in] maximal number of                 */
                                /*!< connections (1...n)                    */
{
    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    if (!IS_INITIALIZED(*ntsocket))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */
    if (!IS_VALIDPORT(port))
        return NTSOCKET_RET_EPORTNUMBER;
                                /** \return NTSOCKET_RET_EPORTNUMBER        */

    /* Perform communication style specific operations.                     */
    switch (ntsocket->style)
    {
    case NTSOCKET_STYLE_TCP:
        return mg_mkstreamserver(ntsocket, port, maxconnections);
                    /** \return Else see function's type                    */
    case NTSOCKET_STYLE_UDP:
        return mg_mkdatagramserver(ntsocket, port);

    case NTSOCKET_STYLE_RESERVED:
        break;
    }

    return NTSOCKET_RET_ESTYLE;     /** \return NTSOCKET_RET_ESTYLE         */
}

/***************************************************************************//**
 *  This function tries to accept one incoming connection request
 *  on an established server socket within the specified time-out if
 *  a connection request is pending.
 *
 *  On success the socket handle is stored into the user-provided
 *  place for the caller, and can thereafter be used to perform communication
 *  with the connected peer.
 *
 *  Notes:
 *
 *  Prior to this function the function 'ntsocket_createserver' must be called
 *  to make the socket listening for requests.
 *
 *  This function is applicable to _all_ communciation styles, including
 *  the connectionless protocol UDP (in contrast to the BSD Socket accept()
 *  interface).
 *
 *  UDP: In case of UDP communication styles this function simply checks
 *  whether there is a datagram incoming from any peer. On success the
 *  datagram is to be read with the 'ntsocket_readdatagram' function.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_accept (
    NTSOCKET        *ntsocket,  /*!< [in,out] socket descriptor             */
    unsigned long   timeout,    /*!< [in] time-out in milliseconds,         */
                                /*!< NTSOCKET_TIMEOUT_NONE or               */
                                /*!< NTSOCKET_TIMEOUT_INFINITE              */
    NTSOCKET_HANDLE *sockethandle)
                                /*!< [out] connection socket handle,        */
                                /*!< returned on success                    */
{
    NTSOCKET_RET        ret;
    struct sockaddr_in  remoteaddr;
    socklen_t           addrlen = sizeof(remoteaddr);

    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    if (!IS_INITIALIZED(*ntsocket))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED */

    /* Make sure that the socket is in listening mode.                      */
    if (!ntsocket->islistening)
        return NTSOCKET_RET_ENTSOCKET;
                                /** \return NTSOCKET_RET_ENTSOCKET  */

    /* Make the caller's socket handle invalid.                             */
    ntsockll_clearhandle(sockethandle);

    /* Wait for incoming connections                                        */
    if ((ret = ntsockll_peek(ntsocket->socket, timeout, PEEK_MODE_READ))
    != NTSOCKET_RET_OK)
        return ret;             /* \return See function type                */

    /* In case of a connectionless communication style address              */
    /* there is no need for further operations.                             */
    if (ntsocket->style != NTSOCKET_STYLE_TCP)
    {
        ntsockll_setsockethandle(sockethandle, ntsocket->socket,
            SOCKETSTATUS_BUSY, ntsocket->style,
            0, ntsocket->isserver);

        return NTSOCKET_RET_OK;
    }

    /* Accept the incoming connection.                                      */
    sockethandle->handle = accept(
        ntsocket->socket,
        (struct sockaddr *) &remoteaddr, &addrlen);

    /* On success let the caller use the new socket's handle.               */
    if (sockethandle->handle >= 0)
    {
#if defined(WIN32GNU) || defined(__unix) || defined(_VXBUS)
        int         flags;

        /* Put the socket into blocking mode                                */
        if ((flags = fcntl(sockethandle->handle, F_GETFL, 0)) >= 0)
        {
            flags &= ~(O_NONBLOCK);
            (void) fcntl(sockethandle->handle, F_SETFL, flags);
        }
#endif
        ntsockll_setsockethandle(sockethandle, sockethandle->handle,
            SOCKETSTATUS_BUSY, ntsocket->style,
            0, ntsocket->isserver);

        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */
    }

    /* Else make the socket handle invalid and report the socket            */
    /* error.                                                               */
    ntsockll_clearhandle(sockethandle);

    return NTSOCKET_RET_ESOCKET;
}

/***************************************************************************//**
 *  This function does any necessary operations to create a socket 'server'
 *  with fix port number in stream connection communication style (TCP, ..)
 *  mode.
*******************************************************************************/
static  NTSOCKET_RET    mg_mkstreamserver (
    NTSOCKET        *ntsocket,  /*!< [in,out] socket descriptor             */
    unsigned int    port,       /*!< [in] portnumber (positive              */
                                /*!< 16-bit integer)                        */
    int         maxconnections) /*!< [in] maximal number of                 */
                                /*!< connections (1...n)                    */
{
    struct sockaddr_in  server_addr;    /* server socket address            */
    int         optval = 1;
#if defined(WIN32GNU) || defined(__unix) || defined(_VXBUS)
    int         flags;
#endif
    /* This function assumes all parameters being checked by the            */
    /* local caller function, so 'asserts' are used to verify this.         */
    VALIDATE(ntsocket != NULL, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE(IS_INITIALIZED(*ntsocket), NTSOCKET_RET_ENOTINITIALIZED);
    VALIDATE(IS_VALIDPORT(port), NTSOCKET_RET_ENTSOCKET);
                                /** \return NTSOCKET_RET_ENULLPOINTER       */
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */
                                /** \return NTSOCKET_RET_ENTSOCKET          */

    /* Make sure the socket descriptor is not busy.                         */
    if (ntsocket->isbusy)
        return NTSOCKET_RET_EBUSY;  /** \return NTSOCKET_RET_EBUSY          */

    /* Initialize   the socket address with zero.                           */
    memset(&server_addr, 0, sizeof(server_addr));

    /* Setup the server's socket address entries.                           */

    /* Set the server socket address address family to 'Internet'.          */
    server_addr.sin_family = AF_INET;

    /* Setup the socket to listen on all local interfaces                   */
    /* addresses.                                                           */
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Set the server socket address port number                            */
    /* in network byte order.                                               */
    server_addr.sin_port = htons((unsigned short) port);

    /* Set the "reuse address" socket option.                               */
    if (SETSOCKOPT(ntsocket->socket,
        SOL_SOCKET, SO_REUSEADDR, (void *) &optval, sizeof(optval)) == -1)
        fprintf(stderr, "setsockopt failed!\n");

    /* Give the socket a "name". This means the socket is bound             */
    /* to the specified port address and is assigned the                    */
    /* specified addresses.                                                 */
    if (bind(ntsocket->socket,
        (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        /* Report an error if the socket couldn't be named.                 */
        return NTSOCKET_RET_EBIND;  /** \return NTSOCKET_RET_EBIND          */
    }

    /* In case of a non-TCP server initialisation is complete               */
    /* here.                                                                */
    if (ntsocket->style != NTSOCKET_STYLE_TCP)
    {
        ntsocket->islistening   = !0;
        ntsocket->isbusy    = !0;
        return NTSOCKET_RET_OK;
    }
#if defined(WIN32GNU) || defined(__unix) || defined(_VXBUS)
    /* Make the socket non-blocking. Since we will wait with                */
    /* select() before accepting incoming connections this will             */
    /* prevent hangups in case of connection resets (RST send               */
    /* from client) right before we call accept(). Note: This is            */
    /* based on information stated in W. Richard Stevens' UNIX              */
    /* Network Programming (Second Edition), Vol.1, page 424.               */
    if ((flags = fcntl(ntsocket->socket, F_GETFL, 0)) >= 0)
    {
        flags |= O_NONBLOCK;
        (void) fcntl(ntsocket->socket, F_SETFL, flags);
    }
#endif
    /* Make the socket accept incoming connections, if the                  */
    /* socket uses stream communication (TCP). Report an error              */
    /* if this fails.                                                       */
    if (listen(ntsocket->socket, maxconnections) < 0)
        return NTSOCKET_RET_ELISTEN;
                                /** \return NTSOCKET_RET_ELISTEN            */

    /* Now the socket is established as server end point.                   */
    /* The socket descriptor is marked listening for requests.              */
    ntsocket->islistening   = !0;
    ntsocket->isbusy        = !0;
    ntsocket->isserver      = !0;

    return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK                 */
}

/***************************************************************************//**
 *  This function does any necessary operations to create a socket 'server'
 *  with fix port number in datagram communication style (UDP, ..)
 *  mode.
*******************************************************************************/
static  NTSOCKET_RET    mg_mkdatagramserver (
    NTSOCKET            *ntsocket,  /*!< [in,out] socket descriptor         */
    unsigned int        port)       /*!< [in] portnumber (positive          */
                                    /*!< 16-bit integer)                    */
{
    struct sockaddr_in  server_addr;    /* server socket address            */

    /* This function assumes all parameters being checked by the            */
    /* local caller function, so 'asserts' are used to verify this.         */

    /* Initialize the socket address with zero.                             */
    memset(&server_addr, 0, sizeof(server_addr));

    /* Setup the server's socket address entries.                           */

    /* Set the server socket address address family to 'Internet'.          */
    server_addr.sin_family = AF_INET;

    /* Setup the socket to listen on all local interfaces                   */
    /* addresses.                                                           */
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Set the server socket address port number                            */
    /* in network byte order.                                               */
    server_addr.sin_port = htons((unsigned short) port);

    /* Give the socket a "name". This means the socket is bound             */
    /* to the specified port address and is assigned the                    */
    /* specified addresses.                                                 */
    if (bind(ntsocket->socket,
        (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        /* Report an error if the socket couldn't be named.                 */
        return NTSOCKET_RET_EBIND;  /** \return NTSOCKET_RET_EBIND          */
    }

    /* Now the socket is established as server end point.                   */
    /* The socket descriptor is marked listening for requests.              */
    ntsocket->islistening   = !0;
    ntsocket->isbusy    = !0;
    ntsocket->isserver  = !0;

    return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK                 */
}

#if 0
/* Aenderungsjournal ***********************************************************
 *
 * $Log: ntsocksv.c $
 * Revision 1.6  2005/07/19 09:46:18  klingler
 * Compilerwarnungen (unused variables) entfernt (f�r Win32)
 *
 * Revision 1.5  2001/04/26 19:51:18  klingler
 * Bedingte Compilierung bei den Teilen, die unter Windows nicht zur
 * Verfuegung stehen.
 * Typkonvertierungen fuer C++ korrigiert
 *
 * Revision 1.4  1999/07/06 12:33:10  duerr
 * Newly accepted socket connections now are put into blocking mode.
 *
 * Revision 1.3  1999/07/05 09:48:40  duerr
 * The socket server's socket is now put into non-blocking mode
 * before accept().
 *
 * Revision 1.2  1999/06/07 10:35:40  duerr
 * Server sockets are now created with socket option SO_REUSEADDR.
 *
 * Revision 1.1  1999/05/08 10:45:43  klingler
 * Initial revision
 *
*******************************************************************************/
#endif
