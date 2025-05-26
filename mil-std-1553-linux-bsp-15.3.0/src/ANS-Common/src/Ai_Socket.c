/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socket.c
 * This module provides a platform independent socket
 * implementation
*******************************************************************************/

/* Includes ******************************************************************/

#include <stdlib.h>
#if defined(__unix) || defined(_VXBUS)
# if defined(_MIT_POSIX_THREADS)
#  include <pthread/mit/pthread.h>
# elif defined(KERNELTHREADS)
#  include <pthread.h>
# endif
#endif
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
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
//# include <ws2ipdef.h>
#endif

#if defined(HAVE_NTLOG)
# include "ntlog.h"
# include "ntscklog.h"
#endif
#include "Ai_Validate.h"
#include "Ai_Strncpy.h"
#include "Ai_Socket.h"

#include "Ai_Sockll.h"

/* Konstanten ****************************************************************/

#ifdef WIN32
# define SENDTO(a,b,c,d,e,f)  sendto((a), (b), (c), (d), (e), (f))
# define SEND(a,b,c,d)  send(/*@-type@*/ (a) /*@=type@*/, (b), (c), (d))
# define RECVFROM(a,b,c,d,e,f)  recvfrom((a), (b), (c), (d), (e), (f))
# define RECV(a,b,c,d)  recv(/*@-type@*/ (a) /*@=type@*/, (b), ((int) c), (d))
# define SHUTDOWN(a,b)  shutdown(/*@-type@*/ (a) /*@=type@*/, (b))
# define CLOSESOCKET(a) closesocket(/*@-type@*/ (a) /*@=type@*/)
# define GETPEERNAME(a,b,c) getpeername(/*@-type@*/ (a) /*@=type@*/, (b), (c))
# define CONNECT(a,b,c) connect(/*@-type@*/ (a) /*@=type@*/, (b), /*@-type@*/ (c) /*@=type@*/)
# define BIND(a,b,c) bind(/*@-type@*/ (a) /*@=type@*/, (b), /*@-type@*/ (c) /*@=type@*/)
# define GETSOCKOPT(a,b,c,d,e)  getsockopt(/*@-type@*/ (a) /*@=type@*/, (b), (c), (char *) (d), (e))
# define SETSOCKOPT(a,b,c,d,e)  setsockopt(/*@-type@*/ (a) /*@=type@*/, (b), (c), (char *) (d), (e))
#else
# define SENDTO(a,b,c,d,e,f)  sendto((a), (b), (c), (d), (e), (f))
# define SEND(a,b,c,d)  send((a), (b), (c), (d))
# define RECVFROM(a,b,c,d,e,f) recvfrom((a), (b), (c), (d), (e), (f))
# define RECV(a,b,c,d)  recv((a), (b), (c), (d))
# define SHUTDOWN(a,b)  shutdown((a), (b))
# define CLOSESOCKET(a) closesocket((a))
# define GETPEERNAME(a,b,c) getpeername((a), (b), (c))
# define CONNECT(a,b,c) connect((a), (b), (c))
# define BIND(a,b,c) bind((a), (b), (c))
# define GETSOCKOPT(a,b,c,d,e)  getsockopt((a), (b), (c), (d), (e))
# define SETSOCKOPT(a,b,c,d,e)  setsockopt((a), (b), (c), (d), (e))
#endif

/* Windows specific constants                                               */
#if defined(WIN32)
#define WS_VERSION_REQD     MAKEWORD(2, 2)  /*!< winsock v2.2 required      */
#define WS_VERSION_MAJOR    HIBYTE(WS_VERSION_REQD)
                                            /*!< major version number       */
#define WS_VERSION_MINOR    LOBYTE(WS_VERSION_REQD)
                    /*!< minor version number                               */
#define MIN_SOCKETS_REQD    5               /*!< min. number of sockets     */
#endif

/* MSVC 4 doesn't define the BSD constants per default                      */
#if defined(WIN32) && !defined(ENOTCONN)
# define ENOTCONN   WSAENOTCONN
#endif

#ifndef  CC_DEBUG
# define CC_DEBUG    0       /*!< Flag: Enable debug code                   */
#endif

/* Macros *******************************************************************/

/**
 * This macro checks whether the specified communication style is valid.
 */
#define IS_VALIDSTYLE(style)    ((int) (style) >= 0             \
                && (int) (style) < NTSOCKET_STYLE_RESERVED)

#if CC_DEBUG
/**
 * This macro executes the specified command if debug mode is enabled by the
 * CC_DEBUG flag.
 */
# define DEBUGCMD(cmd)   { cmd ; }
#else
# define DEBUGCMD(cmd)   /* nothing                                         */
#endif

/* System specific macros ***************************************************/

#if defined(WIN32)
# define    SPECIFIC_STARTUP()  mg_win32_startup()
#else
/**
 * This macro performs any necessary OS specific startup operation, or simply
 * reports "okay" if not necessary.
 */
# define    SPECIFIC_STARTUP()  0
#endif

#if defined(WIN32)
# define    SPECIFIC_CLEANUP()  mg_win32_cleanup()
#else
/**
 * This macro performs any necessary OS specific cleanup operation, or simply
 * reports "okay" if not necessary.
 */
# define    SPECIFIC_CLEANUP()  0
#endif

#if defined(WIN32)
# define    SPECIFIC_CLOSESOCKET(sock)  CLOSESOCKET(sock)
#else
/**
 * This macro performs the OS specific operation to close a specified socket.
 * It return 0 on success, other values indicate failures.
 */
# define    SPECIFIC_CLOSESOCKET(sock)  close(sock)
#endif

/* Types ********************************************************************/

#ifdef WIN32
typedef int socklen_t;
#endif

/* Prototypen ***************************************************************/

 static NTSOCKET_RET    mg_closesocket (
    NTSOCKET_HANDLE *ntsocket_handle);

 static void        mg_clear (/*@out@*/
    NTSOCKET    *ntsocket);

#if defined(WIN32)
 static int     mg_win32_startup (void);
#endif

#if defined(WIN32)
 static int     mg_win32_cleanup (void);
#endif

 static int     mg_is_connected (
    NTSOCKET_HANDLE const   *sockethandle);

/* Variablen ****************************************************************/

/**
 * Flag: Winsock API has been initialized (Win32 only).
 */
#if defined(WIN32)
static  int mg_winsockinitialized;
#endif

/* Funktionen ***************************************************************/

/***************************************************************************//**
 * This function initializes the OS's socket subsystem to be used by
 * the application.
 *
 * This function has to be called at startup on following systems:
 *
 *  Windows 95/NT       (WIN32)
*******************************************************************************/
extern  int     ntsocket_startup (void)
{
    return SPECIFIC_STARTUP();  /** \return  0 - okay                       */
                                /** \return !0 - some kind of failure       */
}

/***************************************************************************//**
 * This function deinitializes the OS's socket subsystem from being used by
 * the application.
 *
 * This function has to be called before termination on following systems:
 *
 *  Windows 95/NT       (WIN32)
*******************************************************************************/
extern  int     ntsocket_cleanup (void)
{
    return SPECIFIC_CLEANUP();  /** \return 0 - okay                        */
                                /** \return < 0 - some kind of failure      */
}

/***************************************************************************//**
 * This function creates a socket descriptor for further use with the
 * specified communication style. Furthermore this function allocates
 * a socket to be used with the specified communication style, which
 * is TCP, UDP, etc.
 *
 * This function is to be called prior to any other function of this module
 * to prepare the socket descriptor for proper usage.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_create(/*@out@                            */
    NTSOCKET    *ntsocket,  /*!< [out] Socket descriptor                    */
    NTSOCKET_STYLE  ntsocket_style)
    /*!< [in] communication style (TCP, UDP, etc.)  */
{
    int style = SOCK_STREAM;

    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);
    /** \return NTSOCKET_RET_ENULLPOINTER   */
    if (!IS_VALIDSTYLE(ntsocket_style))
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* Initialize all decriptor components.                                 */
    mg_clear(ntsocket);

    /* Store the communication style.                                       */
    ntsocket->style = ntsocket_style;

    /* Setup the connection style (TCP, UDP, ...) to be used at             */
    /* the socket() call.                                                   */
    switch (ntsocket_style)
    {
    case NTSOCKET_STYLE_UDP:
        style = SOCK_DGRAM;
        break;

    case NTSOCKET_STYLE_TCP:
        style = SOCK_STREAM;
        break;

    case NTSOCKET_STYLE_RESERVED:
    default:
        return NTSOCKET_RET_EINTERNAL;
    }

    /* Create the socket.                                                   */
    ntsocket->socket = socket(AF_INET, style, 0);
    if (ntsocket->socket < 0)
        return NTSOCKET_RET_ECREATE;    /** \return NTSOCKET_RET_ECREATE    */

    /* Mark the descriptor successfully initialized.                        */
    ntsocket->initialized   = INITIALIZED_MAGICVALUE;

    return NTSOCKET_RET_OK;             /** \return NTSOCKET_RET_OK         */
}

extern  NTSOCKET_RET    ntsocket_create_udp(/*@out@                            */
        NTSOCKET    *ntsocket,  /*!< [out] Socket descriptor                    */
        int          broad_cast)
    /*!< [in] communication style (TCP, UDP, etc.)  */
{
    int broadcastEnable = 1;

    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);

    /* Initialize all decriptor components.                                 */
    mg_clear(ntsocket);

    /* Store the communication style.                                       */
    ntsocket->style = SOCK_DGRAM;

    /* Create the socket.                                                   */
    ntsocket->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ntsocket->socket < 0)
        return NTSOCKET_RET_ECREATE;    /** \return NTSOCKET_RET_ECREATE    */

    if(broad_cast)
    {
        SETSOCKOPT(ntsocket->socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    }

    /* Mark the descriptor successfully initialized.                        */
    ntsocket->initialized   = INITIALIZED_MAGICVALUE;

    return NTSOCKET_RET_OK;             /** \return NTSOCKET_RET_OK         */
}


extern  NTSOCKET_RET    ntsocket_bind_udp_bcast(
    NTSOCKET        *ntsocket,  /*!< [in,out] initialized socket descriptor */
    unsigned int    port)
    /*!< [out] socket handle to be returned on  */
    /*!< success                                */
{
    struct sockaddr_in  my_addr;        /* client socket address            */

    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);

    if (!IS_VALIDPORT(port))
        return NTSOCKET_RET_EPORTNUMBER;
    /** \return NTSOCKET_RET_EPORTNUMBER        */

    /* Initialize   the two socket addresses with zero.                     */
    (void)memset(&my_addr, 0, sizeof(my_addr));

    /* Setup the server's socket address entries.                           */

    /* Set the server socket address address family to 'Internet'.          */
    my_addr.sin_family = AF_INET;

    /* Set the server socket address port number and IP address             */
    /* in network byte order.                                               */
    my_addr.sin_port = htons(port);

#if defined(WIN32)
    /* set internet-address to number                                       */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
#else
    /* Return an error if the IP address couldn't be set.                   */
    inet_aton("255.255.255.255", &my_addr.sin_addr);
#endif

    if (BIND(ntsocket->socket, (struct sockaddr *)& my_addr, sizeof(my_addr)) != 0)
    {
        return NTSOCKET_RET_ECONNECT;
    }

    /*
    * Store the connection information to the socket descriptor and report
    * success.
    */
    ntsocket->isbusy        = !0;
    ntsocket->isconnected   = !0;
    ntsocket->isclient      = !0;

    return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK                 */
}

/***************************************************************************//**
 * This function deinitializes the specified socket descriptor.
 *
 * The associated socket is shutdowned and closed.
 * Note that the socket descriptor can be used for no further
 * socket connection without calling 'ntsocket_create' again first.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_destroy (
    NTSOCKET        *ntsocket)  /*!< [in,out] Socket descriptor             */
{
    NTSOCKET_RET    ntsocket_ret = NTSOCKET_RET_OK;
    NTSOCKET_HANDLE sockethandle;

    /* Check the parameter                                                  */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */

    /* Make sure the descriptor is initialized.                             */
    if (!IS_INITIALIZED(*ntsocket))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Temporary build a "ntsocket handle" marked "busy" to enforce proper  */
    /* disconnection.                                                       */
    ntsockll_setsockethandle(
        &sockethandle,
        ntsocket->socket,
        SOCKETSTATUS_BUSY,
        ntsocket->style,
        ntsocket->isclient,
        ntsocket->isserver);

    /* Disconnect and close the socket.                                     */
    ntsocket_ret = ntsocket_disconnect(&sockethandle);

    /* Make the socket descriptor invalid.                                  */
    mg_clear(ntsocket);

    /* Report the status of operation to the caller.                        */
    return ntsocket_ret;        /** \return else see the function's type    */
}

/***************************************************************************//**
 * This function creates a socket connection to the specified port at
 * the specified socket server's IP address.
 * The valid socket handle is returned to the caller on success, and
 * can thereafter be used to perform further communication with the
 * connected "server".
 *
 * Notes:
 *
 * The socket descriptor has to be initialized with a call to 'ntsocket_create'.
 *
 * After success in connecting to the another network endpoint, and performing
 * communication, the socket handle should be closed by a call to the
 * 'ntsocket_disconnect' function.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_connect (
    NTSOCKET        *ntsocket,  /*!< [in,out] initialized socket descriptor */
    unsigned int    port,       /*!< [in] port number                       */
    char const      *ipaddress, /*!< [in] server IP address in standard     */
                                /*!< numbers-and-dots notation              */
/*@out@                                                                     */
    NTSOCKET_HANDLE *sockethandle)
                                /*!< [out] socket handle to be returned on  */
                                /*!< success                                */
{
    struct sockaddr_in  server_addr;    /* server socket address            */
    struct sockaddr_in  my_addr;        /* client socket address            */

    /* Validate the arguments                                               */
    VALIDATE_PTR(ntsocket, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(ipaddress, NTSOCKET_RET_EIPADDRESS);
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);

    if (!IS_INITIALIZED(*ntsocket))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    if (!IS_VALIDPORT(port))
        return NTSOCKET_RET_EPORTNUMBER;
                                /** \return NTSOCKET_RET_EPORTNUMBER        */

    /* Make sure the IP address is a string with non-zero length.           */
    if ( strlen(ipaddress) < 1 )
        return NTSOCKET_RET_EIPADDRESS;
                                /** \return NTSOCKET_RET_EIPADDRESS         */

    /* Initialize the callers socket handle.                                */
    ntsockll_clearhandle(sockethandle);

    /* Initialize   the two socket addresses with zero.                     */
    (void) memset(&server_addr, 0, sizeof(server_addr));
    (void) memset(&my_addr, 0, sizeof(my_addr));

    /* Setup the server's socket address entries.                           */

    /* Set the server socket address address family to 'Internet'.          */
    server_addr.sin_family = AF_INET;

    /* Set the server socket address port number and IP address             */
    /* in network byte order.                                               */
    server_addr.sin_port = htons((unsigned short) port);

#if defined(WIN32)
    /* set internet-address to number                                       */
    server_addr.sin_addr.s_addr = inet_addr(ipaddress);
    if ( -1 == server_addr.sin_addr.s_addr )
    {
        return NTSOCKET_RET_EIPADDRESS;
    }
#else
    /* Return an error if the IP address couldn't be set.                   */
    if ( inet_aton(ipaddress, &server_addr.sin_addr) == 0 )
    {
        return NTSOCKET_RET_EIPADDRESS;
    }
#endif
    /* Connect to the server.                                               */
    if (CONNECT(ntsocket->socket,
        (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0)
    {
        /* In case of an error close the socket.                            */
        (void) SPECIFIC_CLOSESOCKET(ntsocket->socket);

        return NTSOCKET_RET_ECONNECT;   /** \return NTSOCKET_RET_ECONNECT   */
    }

    /* Store the open connection's socket handle for the caller             */
    /* and mark the handle being connected.                                 */
    ntsockll_setsockethandle(
        sockethandle,
        ntsocket->socket,
        SOCKETSTATUS_BUSY,
        ntsocket->style,
        !0,
        0);

    /*
     * Store the connection information to the socket descriptor and report
     * success.
     */
    ntsocket->isbusy        = !0;
    ntsocket->isconnected   = !0;
    ntsocket->isclient      = !0;

    return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK                 */
}

/***************************************************************************//**
 *  This function disconnects the specified connected socket.
 *
 *  Notes:
 *
 *  The socket handle can be used no longer after call to this function.
 *
 *  This function will perform immediate connection shutdown
 *  with stream connection styles (TCP) sockets in server mode only.
 *  The physical shutdown of datagram connection styles (UDP, ...) sockets
 *  and sockets in client mode (sockets connected to a server) will be
 *  physically closed by the call to 'ntsocket_destroy'.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_disconnect (
    NTSOCKET_HANDLE *sockethandle)  /*!< [in,out] socket handle             */
{
    NTSOCKET_RET    ntsocket_ret = NTSOCKET_RET_OK;
    NTSOCKET_RET    thisret;

    /* Validate the arguments.                                              */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /*
     * Ignore any disconnected socket handles, sockets with datagram connection
     * styles, and sockets connected in  client mode.
     */
    if ( SOCKETSTATUS_BUSY != sockethandle->status
    ||  NTSOCKET_STYLE_TCP != sockethandle->style )
    {
        /*
        * Try to close the socket in case of UDP and make the handle invalid.
        */
        mg_closesocket(sockethandle);
        ntsockll_clearhandle(sockethandle);
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */
    }

    /*
     * Properly shutdown the socket. Disallow further sends and
     * receives on this socket.
     */
    if ( SHUTDOWN(sockethandle->handle, 2) != 0 )
    {
#if defined(WIN32)
        switch (WSAGetLastError())
        {
        case WSANOTINITIALISED:
            ntsocket_ret = NTSOCKET_RET_EINTERNAL;
                                    /** \return NTSOCKET_RET_EINTERNAL      */
            break;

        case WSAENETDOWN:
        case WSAEINVAL:
        case WSAEINPROGRESS:
        case WSAENOTCONN:
        case WSAENOTSOCK:
        default:
            break;
        }
#else
        switch (errno)
        {
    /*@-unrecog@                                                            */
        case ENOTCONN:
    /*@=unrecog@                                                            */
        case EBADF:
            /* Ignore an error if the socket is not                         */
            /* connected, or the socket already is closed.                  */
            break;

    /*@-unrecog@                                                            */
        case ENOTSOCK:
    /*@=unrecog@                                                            */
            //NTASSERT("Handle is no socket handle"==NULL);
            ntsocket_ret = NTSOCKET_RET_EINTERNAL;
            break;

        default:
            //NTASSERT("Unknown error at shutdown"==NULL);
            ntsocket_ret = NTSOCKET_RET_EINTERNAL;
            break;
        }
#endif
    }

    /*?? PD Wait until the socket connection has finished.                  */

    /* Finally close the socket.                                            */
    thisret = mg_closesocket(sockethandle);
    if ( NTSOCKET_RET_OK != thisret )
        ntsocket_ret = thisret;

    /* Make the socket handle invalid.                                      */
    ntsockll_clearhandle(sockethandle);

    return ntsocket_ret;    /** \return Else see the function's return      */
                            /*  data type                                   */
}

/***************************************************************************//**
 *  This function reads a number bytes from the socket into a buffer
 *  within the specified time-out.
 *  The number of bytes which have been read is returned.
 *  Note: This function tries to read the requested number of bytes, looping
 *  through the receive process as long as necessary.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_readdatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_read (
    NTSOCKET_HANDLE *sockethandle,  /*!< [in] socket handle                 */
    unsigned long   timeout,        /*!< [in] time-out in milliseconds,     */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
/*@out@                                                                     */
    char        *buffer,            /*!< [out] destination buffer           */
    size_t      buffersize,         /*!< [in] size of the buffer            */
/*@out@                                                                     */
    size_t      *nread)             /*!< [out] number of bytes read         */
{
    size_t      ii;
    NTSOCKET_RET    ntsocket_ret;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(buffer, NTSOCKET_RET_EBUFFER);
    VALIDATE_PTR(nread, NTSOCKET_RET_ENULLPOINTER);
                                /** \return NTSOCKET_RET_EBUFFER            */
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Initialize the number of bytes read.                                 */
    *nread = 0;

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != sockethandle->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* If there is nothing to read, simply return "okay".                   */
    if ( buffersize < 1 )
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */

    /* Try to read the specified number of bytes.                           */
    for (ii = 0; ii < buffersize; ii++)
    {
        /* Read the next character.                                         */
        ntsocket_ret = ntsocket_getc(sockethandle, timeout, &buffer[ii]);
        if ( NTSOCKET_RET_OK != ntsocket_ret )
            return ntsocket_ret;    /** \return see function's type         */

        /* Adjust the number of bytes read.                                 */
        ++*nread;
    }

    /* In case the whole buffer was read, report success.                   */
    return NTSOCKET_RET_OK;         /** \return NTSOCKET_RET_OK             */
}

/***************************************************************************//**
 *  This function reads the next incoming data block from the socket into
 *  a buffer  within the specified time-out.
 *  The number of bytes which have been read is returned.
 *  Note: This function returns any pending data bytes up to the specified
 *  number of bytes as soon as possible.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_readdatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_readNextData (
    NTSOCKET_HANDLE *phSocket,      /*!< [in] socket handle                 */
    unsigned long   timeout,        /*!< [in] time-out in milliseconds,     */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
/*@out@                                                                     */
    char        *pBuffer,           /*!< [out] destination buffer           */
    size_t      bufferSize,         /*!< [in] size of the buffer            */
/*@out@                                                                     */
    size_t      *pNread)            /*!< [out] number of bytes read         */
{
    int             nread;
    NTSOCKET_RET    ntsocket_ret;

    /* Validate the arguments                                               */
    VALIDATE_PTR(pNread, NTSOCKET_RET_ENULLPOINTER);
    *pNread = 0;
    VALIDATE_PTR(phSocket, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(pBuffer, NTSOCKET_RET_EBUFFER);
                                /** \return NTSOCKET_RET_EBUFFER            */
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    /* Make sure the socket handle is initialized.                          */
    if ( !IS_INITIALIZED(*phSocket) )
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != phSocket->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* If there is nothing to read, simply return "okay".                   */
    if (bufferSize < 1)
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */

    /* Check, whether data is pending on the connection                     */
    ntsocket_ret = ntsockll_peek(phSocket->handle, timeout, PEEK_MODE_READ);
    if ( NTSOCKET_RET_OK != ntsocket_ret )
        return ntsocket_ret;

    /*
     * Read the pending data character from the socket connection.
     * Note: recv may return 0 for
     * a) Windows:
     *    If the connection has been gracefully closed, the return value
     *    is zero.
     * b) Linux:
     *    The return value will be 0 when the peer has performed an orderly
     *    shutdown.
     */
    nread = RECV(phSocket->handle, pBuffer, bufferSize, 0);
    if ( nread < 0 )
    {
        ntsocket_ret = NTSOCKET_RET_EBROKEN;
                                /** \return NTSOCKET_RET_EBROKEN        */
    }
    else if ( 0 == nread )
    {
        ntsocket_ret = NTSOCKET_RET_CLOSED_BY_PEER;
                                /** \return NTSOCKET_RET_CLOSED_BY_PEER */
    }
    else
    {
        *pNread = nread;
        ntsocket_ret = NTSOCKET_RET_OK;
    }

    return ntsocket_ret;
}

/***************************************************************************//**
 *  This function reads the a data block from the socket into
 *  a buffer  within the specified time-out.
 *  The number of bytes which have been read is returned.
 *  Note: This function wait until all requested data is read.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_readdatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_readBlock (
    NTSOCKET_HANDLE *phSocket,  /*!< [in] socket handle                     */
    unsigned long   timeout,    /*!< [in] time-out in milliseconds,         */
                                /*!< NTSOCKET_TIMEOUT_NONE or               */
                                /*!< NTSOCKET_TIMEOUT_INFINITE              */
/*@out@*/
    char        *pBuffer,       /*!< [out] destination buffer               */
    size_t      bytesRequired,  /*!< [in] number of data bytes required     */
                                /*!< to read                                */
/*@out@*/
    size_t      *pNread)        /*!< [out] number of bytes actually read    */
{
    size_t          nTotalRead;
    size_t          bytesRemaining;
    int             nRead;
    NTSOCKET_RET    ntsocket_ret = NTSOCKET_RET_OK;
    char            *pWrite;

    /* Validate the arguments                                               */
    VALIDATE_PTR(pNread, NTSOCKET_RET_ENULLPOINTER);
    *pNread = 0;
    VALIDATE_PTR(phSocket, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(pBuffer, NTSOCKET_RET_EBUFFER);
                                /** \return NTSOCKET_RET_EBUFFER            */
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    /* Make sure the socket handle is initialized.                          */
    if ( !IS_INITIALIZED(*phSocket) )
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != phSocket->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* If there is nothing to read, simply return "okay".                   */
    if ( bytesRequired < 1 )
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */

    nTotalRead      = 0;
    bytesRemaining  = bytesRequired;
    pWrite          = pBuffer;
    while ( bytesRemaining > 0 )
    {
        /* Check, whether data is pending on the connection                 */
        ntsocket_ret = ntsockll_peek(phSocket->handle, timeout, PEEK_MODE_READ);
        if ( NTSOCKET_RET_OK != ntsocket_ret )
            return ntsocket_ret;

        /* Read the pending data character from the socket connection.      */
        nRead = RECV(phSocket->handle, pWrite, bytesRemaining, 0);
        if ( nRead < 0 )
        {
            ntsocket_ret = NTSOCKET_RET_EBROKEN;
                                    /** \return NTSOCKET_RET_EBROKEN        */
            break;
        }
        else if ( 0 == nRead )
        {
            ntsocket_ret = NTSOCKET_RET_CLOSED_BY_PEER;
                                    /** \return NTSOCKET_RET_CLOSED_BY_PEER */
            break;
        }
        else
        {
            bytesRemaining  -= nRead;
            nTotalRead      += nRead;
            pWrite          += nRead;
            ntsocket_ret     = NTSOCKET_RET_OK;
        }

    } /* while */

    *pNread = nTotalRead;

    return ntsocket_ret;
}

/***************************************************************************//**
 *  This function reads in at most one less than 'buffersize' characters
 *  from the specified socket and stores them into the buffer pointed to
 *  by 'buffer'.
 *  Reading  stops after an EOF or a newline.  If a newline is read,
 *  it is stored into the  buffer.  A '\0' is stored after the last character
 *  in the buffer.
 *
 *  Reading stops if no character gets available within the specified time-out.
 *
 *  Note: The buffer lacks a trailing newline if reading stops because
 *  there are 'buffersize' - 1 characters read, and the last character
 *  isn't a newline itself.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_readdatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_fgets (
    NTSOCKET_HANDLE *sockethandle,  /*!< [in] socket handle                 */
    unsigned long   timeout,        /*!< [in] time-out in milliseconds,     */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
/*@out@                                                                     */
    char        *buffer,            /*!< [out] destination buffer           */
    size_t      buffersize)         /*!< [in] size of the buffer            */

{
    size_t      ii;
    NTSOCKET_RET    ntsocket_ret;
    char        *bufferptr = buffer;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */
    VALIDATE_PTR(buffer, NTSOCKET_RET_EBUFFER);
                                    /** \return NTSOCKET_RET_EBUFFER        */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != sockethandle->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* If there is nothing to read, simply return "okay".                   */
    if (buffersize < 1)
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */

    /* Try to read the specified number of bytes.                           */
    for (ii = 0; ii < buffersize - 1; ii++)
    {
        /* Read the next character.                                         */
        ntsocket_ret = ntsocket_getc(sockethandle, timeout, bufferptr);
        if ( NTSOCKET_RET_OK != ntsocket_ret )
        {
            /* If a error occured, terminate the                            */
            /* buffer with a trailing null and report                       */
            /* time-out to the caller.                                      */
            *bufferptr = '\0';

            return ntsocket_ret;    /** \return See function's type         */
        }

        /* Check whether the newline character was read.                    */
        /* In this case terminate the buffer and stop reading.              */
        if (*bufferptr++ == '\n')
            break;
    }

    /* Terminate the destination buffer.                                    */
    *bufferptr = '\0';

    /* In case the whole buffer was read, report success.                   */
    return NTSOCKET_RET_OK;
}

/***************************************************************************//**
 *  This function reads the next character from the connection socket
 *  within the specified time-out.
 *
 *  The character that is read is stored into the place specified by
 *  the 'ch' parameter. On failure there is no character stored.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_readdatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_getc (
    NTSOCKET_HANDLE *sockethandle,  /*!< [in] open socket handle            */
    unsigned long   timeout,        /*!< [in] time-out in milliseconds,     */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
/*@out@                                                                     */
    char            *ch)            /*!< [out] character read on success    */
{
    int             nread;
    NTSOCKET_RET    ret;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(ch, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                    /** \return                             */
                                    /**NTSOCKET_RET_ENOTINITIALIZED         */

    /* Make sure this is a stream socket.                                   */
    if (sockethandle->style != NTSOCKET_STYLE_TCP)
        return NTSOCKET_RET_ESTYLE;
                                    /** \return NTSOCKET_RET_ESTYLE         */

    if ( NTSOCKET_HANDLEINVALID == sockethandle->handle )
        return NTSOCKET_RET_ENTSOCKET;
                                    /** \return NTSOCKET_RET_ENTSOCKET      */

#if 0   /* CK: Performance!!!, sollte jetzt dank korrekter recv()           */
    /* CK: trotdem funktionieren                                            */
    /* Ensure that the socket is connected.                                 */
    if (!mg_is_connected(sockethandle))
        return NTSOCKET_RET_EBROKEN;    /** \return NTSOCKET_RET_EBROKEN    */
#endif

    /* Check, whether data is pending on the connection                     */
    if ((ret = ntsockll_peek(sockethandle->handle, timeout, PEEK_MODE_READ))
    != NTSOCKET_RET_OK)
        return ret;

    /* Read the character from the socket connection.                       */
    nread = RECV(sockethandle->handle, ch, 1, 0);

#ifdef HAVE_NTLOG
    if (1 != nread)
    {
        (void) ntlog(ntsocket_log_warnlevel(),
            "%s ntsocket_getc: recv rc(%d) errno: %s",
            ntsocket_log_warnprefix(),
            (int) nread, strnull(strerror(errno)));
    }
#endif

    /* Check whether there was data read.                                   */
    /* Report an error if no data was available or a socket error           */
    /* occured.                                                             */
    if ( nread < 0 )
    {
        ret = NTSOCKET_RET_EBROKEN; /** \return NTSOCKET_RET_EBROKEN        */
    }
    else if ( 0 == nread )
    {
        ret = NTSOCKET_RET_CLOSED_BY_PEER;
                                    /** \return NTSOCKET_RET_CLOSED_BY_PEER */
    }
    else
    {
        ret = NTSOCKET_RET_OK;      /** \return NTSOCKET_RET_OK             */
    }

    return ret;
}

extern  NTSOCKET_RET    ntsocket_sendto(
    NTSOCKET *socket,  /*!< [in] open socket handle            */
    char const  *ip,                /*!< [in] receiver ip                   */
    int         port,               /*!< [in] receiver port                 */
    void       *buffer,            /*!< [in] source buffer                 */
    int         length,             /*!< [in] length of the buffer          */
    /*@out@*//*@null@                                                           */
    int         *nwritten)          /*!< [out] reference to storage for     */
    /*!< number of bytes written,           */
    /*!< or NULL                            */
{
    struct sockaddr_in      si;

    si.sin_family = AF_INET;
    si.sin_port   = htons(port);

#if defined(WIN32)
    /* set internet-address to number                                       */
    si.sin_addr.s_addr = inet_addr(ip);
#else
    /* Return an error if the IP address couldn't be set.                   */
    inet_aton(ip, &si.sin_addr);
#endif

    *nwritten = SENDTO(socket->socket, buffer, length, 0, (struct sockaddr*) &si, sizeof(si));

    return NTSOCKET_RET_OK;
}


extern  NTSOCKET_RET    ntsocket_recvfrom(
    NTSOCKET *socket,  /*!< [in] open socket handle            */
    void *      buffer,               /*!< [in] source buffer                 */
    int         length,             /*!< [in] length of the buffer          */
    /*@out@*//*@null@                                                           */
    int         *nreceived,          /*!< [out] reference to storage for     */
    /*!< number of bytes received,           */
    /*!< or NULL                            */
    char       * ip)
{
    char * dummy;
    socklen_t sender_addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in      si;

    *nreceived = RECVFROM(socket->socket, buffer, length, 0, (struct sockaddr*) &si /*out param*/, &sender_addr_size);

    if (*nreceived < 0)
    {
        *nreceived = 0;
        return NTSOCKET_RET_EBROKEN;
    }
    else if (*nreceived == 0)
    {
        *nreceived = 0;
        return NTSOCKET_RET_CLOSED_BY_PEER;
    }

#if defined(WIN32)
    /* set internet-address to number                                       */
    dummy = inet_ntoa(si.sin_addr);
    strncpy_s( ip, 64, dummy, 64);
#else
    /* set internet-address to number                                       */
    dummy = inet_ntoa(si.sin_addr);
    strncpy( ip, dummy, 64);
#endif

    return NTSOCKET_RET_OK;
}


/***************************************************************************//**
 *  This function writes the contents of the specified buffer with
 *  specified length to the specified connected socket.
 *
 *  The number of bytes which have been written is returned, if the
 *  according pointer reference is non-NULL.
 *
 *  Notes:
 *
 *  This function applies to stream sockets (TCP) only.
 *
 *  To write data by use of datagram communication styles see the
 *  function 'ntsocket_writedatagram'.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_write (
    NTSOCKET_HANDLE *sockethandle,  /*!< [in] open socket handle            */
    char const  *buffer,            /*!< [in] source buffer                 */
    int         length,             /*!< [in] length of the buffer          */
/*@out@*//*@null@                                                           */
    int         *nwritten)          /*!< [out] reference to storage for     */
                                    /*!< number of bytes written,           */
                                    /*!< or NULL                            */
{
    int     nbytes;
    int     *nbytesptr;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */
    VALIDATE_PTR(buffer, NTSOCKET_RET_EBUFFER);
                                    /** \return NTSOCKET_RET_EBUFFER        */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                    /** \return                             */
                                    /**NTSOCKET_RET_ENOTINITIALIZED         */

    /* Make sure that the socket is a stream socket.                        */
    if ( NTSOCKET_STYLE_TCP != sockethandle->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    /* Setup where to store the number of bytes written.                    */
    nbytesptr = (nwritten == NULL) ? &nbytes : nwritten;

    /* Initialize the number of bytes written.                              */
    *nbytesptr = 0;

    /* If there is nothig to read, simply return "okay".                    */
    if (length < 1)
        return NTSOCKET_RET_OK;

    /* Make sure that the socket is connected.                              */
    /* Report an error if not connected.                                    */
    if (!mg_is_connected(sockethandle))
        return NTSOCKET_RET_EBROKEN;

    /* Write the data to the socket.                                        */
    *nbytesptr = SEND(sockethandle->handle, buffer, length, 0);

#ifdef HAVE_NTLOG
    if (length != *nbytesptr)
    {
        (void) ntlog(ntsocket_log_warnlevel(),
            "%s ntsocket_write: status(%d): %s",
            ntsocket_log_warnprefix(),
            *nbytesptr, strnull(strerror(errno)));
    }
#endif
    /* Check whether the expected number of bytes has actually              */
    /* been written.                                                        */
    return *nbytesptr < length
        ? NTSOCKET_RET_EWRITE       /** \return NTSOCKET_RET_EBROKEN        */
        : NTSOCKET_RET_OK;          /** \return NTSOCKET_RET_OK             */
}

/***************************************************************************//**
 *  This function writes the contents of the specified buffer with
 *  specified length to the specified connected socket with the specified
 *  time-out value applied to each byte being transmitted.
 *
 *  The number of bytes which have been written is returned, if the
 *  according pointer reference is non-NULL.
 *
 *  Notes:
 *
 *  This function applies to stream sockets (TCP) only.
 *
 *  To write data by use of datagram communication styles see the
 *  function 'ntsocket_writedatagram'.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_writetimed (
    NTSOCKET_HANDLE     *sockethandle,  /*!< [in] open socket handle        */
    char const          *buffer,        /*!< [in] source buffer             */
    size_t              length,         /*!< [in] length of the buffer      */
/*@out@*//*@null@                                                           */
    size_t              *nwritten,      /*!< [out] reference to storage for */
                                        /*!< number of bytes written,       */
                                        /*!< or NULL                        */
    unsigned long   timeout)            /*!< [in] time-out in [ms],         */
                                        /*!< NTSOCKET_TIMEOUT_NONE or       */
                                        /*!< NTSOCKET_TIMEOUT_INFINITE      */
{
    NTSOCKET_RET    ntsocketret;
    size_t      nbytes;
    size_t      *nbytesptr;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */
    VALIDATE_PTR(buffer, NTSOCKET_RET_EBUFFER);
                                    /** \return NTSOCKET_RET_EBUFFER        */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Setup where to store the number of bytes written.                    */
    nbytesptr = (nwritten == NULL) ? &nbytes : nwritten;

    /* Initialize the number of bytes written.                              */
    *nbytesptr = 0;

    /* If there is nothig to read, simply return "okay".                    */
    if (length < 1)
        return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK             */

    /* Write the data to the socket.                                        */
    for (*nbytesptr = 0; *nbytesptr < length; ++*nbytesptr)
    {
        ntsocketret = ntsocket_putc(sockethandle, timeout, buffer[*nbytesptr]);
        if ( NTSOCKET_RET_OK == ntsocketret )
            continue;

        return ntsocketret;         /** \return see function's type         */
    }

    return NTSOCKET_RET_OK;
}

/***************************************************************************//**
 *  This function tries to transmit the specified character by use of
 *  the connection socket within the specified time-out.
 *
 *  This function is applicable to stream communication styles (TCP, ...)
 *  only. For datagram sockets use 'ntsocket_writedatagram' instead.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_putc (
    NTSOCKET_HANDLE *sockethandle,  /*!< [in] open socket handle            */
    unsigned long   timeout,        /*!< [in] time-out in milliseconds,     */
                                    /*!< NTSOCKET_TIMEOUT_NONE or           */
                                    /*!< NTSOCKET_TIMEOUT_INFINITE          */
    char            ch)             /*!< [in] character to transmit         */
{
    NTSOCKET_RET    ret;
    int             nwritten;

    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if (sockethandle->style != NTSOCKET_STYLE_TCP)
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    if ( NTSOCKET_HANDLEINVALID == sockethandle->handle )
        return NTSOCKET_RET_ENTSOCKET;  /** \return NTSOCKET_RET_ENTSOCKET  */

    /* Make sure that the socket is connected.                              */
    /* Report an error if not connected.                                    */
    if (!mg_is_connected(sockethandle))
        return NTSOCKET_RET_EBROKEN;    /** \return NTSOCKET_RET_EBROKEN    */

    /* Check, if we are able to write the data                              */
    ret = ntsockll_peek(sockethandle->handle, timeout, PEEK_MODE_WRITE);
    if ( NTSOCKET_RET_OK != ret )
    {
        return ret;
    }

    /* Write the character to the socket connection.                        */
    nwritten = SEND(sockethandle->handle, &ch, 1, 0);

#ifdef HAVE_NTLOG
    if (nwritten != 1)
    {
        (void) ntlog(ntsocket_log_warnlevel(),
            "%s ntsocket_putc: status(%d): %s",
            ntsocket_log_warnprefix(),
            nwritten, strnull(strerror(errno)));
    }
#endif

    /* Report an error if no data was written or a socket error             */
    /* occured.                                                             */
    return nwritten != 1
        ? NTSOCKET_RET_EBROKEN      /** \return NTSOCKET_RET_EBROKEN        */
        : NTSOCKET_RET_OK;          /** \return NTSOCKET_RET_OK             */
}

/***************************************************************************//**
 *  This function checks whether incoming data is pending to be received within
 *  the specified timeout over the specified socket handle.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_peek (
    NTSOCKET_HANDLE     *sockethandle,  /*!< [in] open socket handle        */
    unsigned long       timeout)        /*!< [in] time-out in milliseconds, */
                                        /*!< NTSOCKET_TIMEOUT_NONE          */
                                        /*!< NTSOCKET_TIMEOUT_INFINITE      */
{
    /* Validate the arguments                                               */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != sockethandle->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    if ( NTSOCKET_HANDLEINVALID == sockethandle->handle )
        return NTSOCKET_RET_ENTSOCKET;

    /* Make sure that the socket is connected.                              */
    /* Report an error if not connected.                                    */
    if (!mg_is_connected(sockethandle))
        return NTSOCKET_RET_EBROKEN;    /** \return NTSOCKET_RET_EBROKEN    */

    /* Check, if there is data waiting to be received                       */
    return ntsockll_peek(sockethandle->handle, timeout, PEEK_MODE_READ);
        /** \return NTSOCKET_RET_OK, if data is waiting to be received      */

        /** \return NTSOCKET_RET_ETIMEOUT, if no data is waiting to         */
        /* be received                                                      */
}

/***************************************************************************//**
 * This function empties the specified socket's receive buffers.
*******************************************************************************/
extern void ntsocket_flush (
     NTSOCKET_HANDLE *pSockethandle)  /*!< [in] open socket handle        */
{
    char            buffer[128];
    size_t          nRead;
    NTSOCKET_RET    ret;

    VALIDATE_PTR_VOID(pSockethandle);

    do
    {
        nRead = 0;
        ret = ntsocket_readNextData(pSockethandle, NTSOCKET_TIMEOUT_NONE, buffer, sizeof(buffer), &nRead);

        if ( nRead < 1 )
            break;

    } while ( NTSOCKET_RET_OK == ret );
}

/***************************************************************************//**
 *  This function checks whether the connection is still established or not.
*******************************************************************************/
extern  NTSOCKET_RET        ntsocket_isconnected (
    const NTSOCKET_HANDLE   *handle)    /*!< [in] handle to check           */
{
    /* Validate the arguments                                               */
    VALIDATE_PTR(handle, NTSOCKET_RET_ENULLPOINTER);
                                /** \return NTSOCKET_RET_ENULLPOINTER       */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*handle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Make sure this is a stream socket.                                   */
    if ( NTSOCKET_STYLE_TCP != handle->style )
        return NTSOCKET_RET_ESTYLE; /** \return NTSOCKET_RET_ESTYLE         */

    if ( NTSOCKET_HANDLEINVALID == handle->handle )
        return NTSOCKET_RET_ENTSOCKET;

    if (mg_is_connected(handle))
        return NTSOCKET_RET_OK; /** \return NTSOCKET_RET_OK, falls          */
                                /* die Verbindung noch besteht              */

    return NTSOCKET_RET_EBROKEN;
                    /** \return NTSOCKET_RET_EBROKEN,                       */
                    /* falls die Verbindung                                 */
                    /* beendet wurde                                        */
}

/***************************************************************************//**
 *  This function closes the specified socket in a OS specific manner.
 *
 *  Note: This function does no further checks if there are any pending
 *  data.
*******************************************************************************/
static  NTSOCKET_RET    mg_closesocket (
    NTSOCKET_HANDLE     *ntsocket_handle)   /*!< [in,out] socket handle     */
{
    /* Close the socket.                                                    */
    return SPECIFIC_CLOSESOCKET(ntsocket_handle->handle) == 0
        ? NTSOCKET_RET_OK       /** \return NTSOCKET_RET_OK                 */
        : NTSOCKET_RET_ECLOSE;  /** \return NTSOCKET_RET_ECLOSE             */
}

/***************************************************************************//**
 *  This function makes the specified socket descriptor structure invalid.
*******************************************************************************/
static  void        mg_clear (/*@out@                                       */
    NTSOCKET        *ntsocket)  /*!< [out] socket descriptor                */
{
    VALIDATE_PTR_VOID(ntsocket);

    /* Initialize all decriptor components.                                 */
    (void) memset(ntsocket, 0, sizeof(NTSOCKET));

    ntsocket->socket = NTSOCKET_HANDLEINVALID;
}

#if defined(WIN32)
/***************************************************************************//**
 *  This function performs any necessary startup operations on the
 *  Win32 platforms.
*******************************************************************************/
static  int     mg_win32_startup (void)
{
    WSADATA wsadata;

    /* Do nothing if Winsock already is initialized.                        */
    if ( mg_winsockinitialized )
        return 0;       /** \return 0 - okay                                */

    /* Initialize the Winsock API.                                          */
    if ( 0 != WSAStartup(WS_VERSION_REQD, &wsadata) )
        return !0;      /** \return !0 - error                              */

    /* Check whether the socket DLL is valid.                               */
    if ((LOBYTE(wsadata.wVersion) < WS_VERSION_MAJOR)
    || (LOBYTE(wsadata.wVersion) == WS_VERSION_MAJOR
    && HIBYTE(wsadata.wVersion) < WS_VERSION_MINOR))
        return !0;

    /* Check whether the number of sockets is exceeded.                     */
    /* Note: On Windows 7 x64 a value of zero was seen here.                */
    if ( 0 != wsadata.iMaxSockets )
    {
        if ( wsadata.iMaxSockets < MIN_SOCKETS_REQD)
            return !0;
    }

    /* Mark the Winsock API initialized.                                    */
    mg_winsockinitialized = !0;

    return 0;
}
#endif

#if defined(WIN32)
/***************************************************************************//**
 *  This function performs any necessary cleanup operations on the
 *  Win32 platforms.
*******************************************************************************/
static  int     mg_win32_cleanup (void)
{
    /* Do nothing if the API is not initialized yet.                        */
    if ( !mg_winsockinitialized )
        return 0;           /** \return 0 - okay                            */

    /* Perform deinitialization.                                            */
    if ( WSACleanup() == SOCKET_ERROR )
        return !0;          /** \return !0 - error                          */

    /* Mark the API deinitialized.                                          */
    mg_winsockinitialized = 0;

    return 0;
}
#endif

/***************************************************************************//**
 *  This function checks whether the specified socket handle belongs to
 *  a currently connected socket.
*******************************************************************************/
static  int     mg_is_connected (
    NTSOCKET_HANDLE const   *sockethandle)  /*!< [in] socket handle         */
{
    struct sockaddr name;
    socklen_t      namelen;
    int     value = 0;
    socklen_t      len;
    int     rc;

    VALIDATE_PTR(sockethandle, 0);

    /* Check whether the socket is connected.                               */
    len = (int) sizeof(value);
    rc = GETSOCKOPT(
        sockethandle->handle, SOL_SOCKET, SO_ERROR, (void *) &value, &len);
    if ( 0 != rc
    ||   0 != value )
    {
        DEBUGCMD(printf("getsockopt rc = %d\n", rc))
        DEBUGCMD(printf("getsockopt SO_ERROR = %d\n", value))
        return 0;
    }

    namelen = sizeof(name);
    if ( GETPEERNAME(sockethandle->handle, &name, &namelen) != -1 )
        return !0;

    if ( ENOTCONN == errno )
        return 0;           /** \return 0 - not connected                   */

    DEBUGCMD(fprintf(stderr, "getpeername: errno(%d): %s\n",
        errno, strerror(errno)))

    return !0;              /** \return !0 - connected                      */
}

#if 0
/* Aenderungsjournal ***********************************************************
 *
 * $Log: ntsocket.c $
 * Revision 1.32  2001/04/26 19:49:50  klingler
 * Bei Schreibfehler wird nicht mehr ausschliesslich NTSOCKET_RET_EBROKEN
 * zurueckgeliefert, sondern zwischen Verbindungsabbruch und
 * Schreibfehler unterschieden.
 * Typkonvertierungen fuer C++ korrigiert (size_t <-> int)
 *
 * Revision 1.30  1999/10/18 16:34:23  duerr
 * Changed logging to use new ntsocket_log_xxx functions.
 *
 * Revision 1.29  1999/08/16 07:49:02  duerr
 * Added some macros for system socket functions to avoid warnings
 * with LCLint under Win32. Improved error logging.
 *
 * Revision 1.28  1999/08/14 07:59:12  duerr
 * Added additional call for _isconnected after select(2) in _getc()
 *
 * Revision 1.27  1999/08/12 12:49:41  duerr
 * Added error logging via MSG_WARN with ntlog if HAVE_NTLOG is defined.
 *
 * Revision 1.26  1999/07/02 14:10:20  duerr
 * Changed hard debug printfs to be switchable by the CC_DEBUG constant.
 *
 * Revision 1.25  1999/07/01 06:36:58  duerr
 * Added support for Linux kernel threads
 *
 * Revision 1.24  1999/06/28 10:01:53  duerr
 * Fixed bug in call to getsockopt (mg_is_connected): valuelen is
 * now passed initialized to getsockopt.
 *
 * Revision 1.23  1999/06/15 08:07:49  duerr
 * _writetimed: changed temp. variables to match new function
 * parameter types (size_t).
 *
 * Revision 1.22  1999/06/14 04:59:34  duerr
 * Updated interfaces for LCLint compliance.
 *
 * Revision 1.21  1999/06/07 10:37:24  duerr
 * Extended _isconnected function.
 *
 * Revision 1.20  1999/06/07 10:30:16  duerr
 * Changed include path to pthread.h
 *
 * Revision 1.19  1999/05/10 11:19:17  klingler
 * Added _peek function.
 *
 * Revision 1.18  1999/05/08 10:45:15  klingler
 * Module splitted into several diffent submodules
 *
 * Revision 1.17  1999/05/05 15:06:19  duerr
 * Added new function interfaces _writetimed and _putc.
 *
 * Revision 1.16  1997/11/25 15:01:46  duerr
 * Removed unused function '_close'.
 * Added support for MIT-pthread bugs on Linux systems related
 * to select(2) calls.
 *
 * Revision 1.15  1997/08/25 10:43:20  duerr
 * Removed superflous code in _getc function.
 *
 * Revision 1.14  1997/08/25 08:23:25  duerr
 * Splitted _createserver function in stream and datagram related specific
 * internal functions.
 * Function _accept now treats datagram styles properly.
 * Functions _read, _write, _getc, _fgets now are specialized to
 * stream connections, since there have been problems with datagram
 * sockets.
 * New datagram specific functions _readdatagram and _writedatagram.
 * Added new function '_geterror' to retrieve error status related
 * descriptive messages.
 *
 * Revision 1.13  1997/08/21 13:11:48  duerr
 * Specialized 'ntsocket_write' to handle stream sockets only.
 * A new function '_writedatagram' is to be implemented to perform
 * datagram socket output.
 *
 * Revision 1.12  1997/08/20 12:26:46  duerr
 * Function _write now handles argument 'nwritten' optional.
 * The specification of a NULL reference is allowed.
 *
 * Revision 1.11  1997/08/20 10:51:58  duerr
 * Implemented new function "_getpeername" to retrieve the IP address
 * of a connected peer. Included header file "strncpy.h".
 *
 * Revision 1.10  1997/08/20 10:20:22  duerr
 * Implemented new macros named SPECIFIC_xxx to call OS dependend
 * functions.
 *
 * Revision 1.9  1997/08/19 21:19:15  duerr
 * Removed constant "SOCKETUNCONNECTED", since this was not used
 * anymore.
 * Severak function descriptions are now more detailled.
 *
 * Revision 1.8  1997/08/19 15:54:21  duerr
 * Implemented additional WIN32 specific code.
 * Code tested on Linux and Windows NT 4.0.
 *
 * Revision 1.7  1997/08/19 13:05:42  duerr
 * New function "_fgets" implemented, to support line-wise input.
 *
 * Revision 1.6  1997/08/19 12:10:54  duerr
 * New function "setsockethandle" implemented to set all elements
 * of the "ntsocket handle" structure.
 * Invalid handles in "_destroy" function (e.g. not connected handles)
 * no longer lead to errors - they're simply ignored.
 *
 * Revision 1.5  1997/08/18 22:23:34  duerr
 * Functions _getc and _read now support user specified time-out
 * values in milliseconds.
 * Function _read now makes use of sequential calls to _getc.
 * New static function mg_timeout2timeval implemented.
 *
 * Revision 1.4  1997/08/18 20:56:59  duerr
 * Support for extended "ntsocket handle" implemented.
 * Datagram communication style sockets and sockets in "client mode"
 * are now treated different from stream and "server mode" sockets.
 *
 * Revision 1.3  1997/08/18 17:45:24  duerr
 * Implemented the functions _connect, _createserver, _disconnect,
 * _create, and _destroy. First release, untested.
 *
 * Revision 1.2  1997/08/11 15:25:41  duerr
 * Changed interfaces to match ANSI C standard.
 * Renamed file to "ntsocket" (NewTec socket).
 *
 * Revision 1.1  1997/07/19 10:29:21  duerr
 * Initial revision
 *
*******************************************************************************/
#endif
