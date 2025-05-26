/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socktl.c
 *
 * \brief socket additional tools

  This module provides additional tools for
  which simplify the usage of the Ai_Socket module.

*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(__unix) || defined(TARGET_OS2) || defined(_VXBUS)
# include <sys/time.h>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
#endif
#if defined(WIN32)
# pragma warning(disable:4996)
# include <winsock2.h>
#endif

#include "Ai_Strncpy.h"
#include "Ai_Validate.h"
#include "Ai_Sockll.h"
#include "Ai_Socktl.h"

/* Konstanten *****************************************************************/

#define FORMAT_IP   "%d.%d.%d.%d"   /*!< Format of an IP-Address    */
#define FORMAT_MAX  4               /*!< Elements of an IP-Adress   */

/* Macroes ********************************************************************/

/**
 * Check whether the specified IP octet is in range of a byte.
 */
#define ISVALIDRANGE(_ip_) ((_ip_) >= 0 && (_ip_) <= 255)

/* Types **********************************************************************/

#ifdef WIN32
typedef int socklen_t;
#endif

/* Prototypen *****************************************************************/

/* Variablen ******************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 *  This function sets a NTsocket IP address to a new value.
*******************************************************************************/
extern  int         ntsocket_setip (/*@out@*/
    NTSOCKET_IPADDRESS  *pAdr,  /*!< [out] IP adress                        */
    char const          *val)   /*!< [in] Value to set IP adress to         */
{
    /* Check the parameters                                                 */
    VALIDATE_PTR(pAdr, -1);     /** \return -1 on fatal system error        */
    VALIDATE_PTR(val, -1);

    /* Check, whether IP Adress is valid                                    */
    if ( !ntsocket_isvalidip(val))
        return 1;               /** \return 1, if IP address is not valid   */

    /* Copy the value to the ip address                                     */
    (void) STRNCPY(pAdr->ipAddress, val, sizeof(pAdr->ipAddress));

    return 0;                   /** \return 0, if function succeeded        */
}

/***************************************************************************//**
 *  This function checks, whether a given IP adress is valid or not.
*******************************************************************************/
extern  int         ntsocket_isvalidip (
    char const      *val)   /*!< [in] ip address to check                   */
{
    int ip1, ip2, ip3, ip4;

    /* Check the parameter                                                  */
    VALIDATE_PTR(val, 0);   /** \return 0, if IP address is not valid       */

    /* Check whether ip address is an empty string, what is valid for now   */
    if ( 0 == strlen(val) )
        return 1;

    /* Check whether the ip address has a correct length                    */
    if (strlen(val) > NTSOCKET_IPADDRESS_SIZE)
        return 0;

    /* Check, whether the ip address has the correct format                 */
    if (sscanf(val, FORMAT_IP, &ip1, &ip2, &ip3, &ip4) < FORMAT_MAX)
        return 0;

    /* Check, whether it's elements have a correct value range              */
    if (! (ISVALIDRANGE(ip1)
    && ISVALIDRANGE(ip2)
    && ISVALIDRANGE(ip3)
    && ISVALIDRANGE(ip4)))
        return 0;

    /* The IP adress is now valid                                           */
    return 1;               /** \return 1, if ip address is valid           */
}

/***************************************************************************//**
 *  This function retrieves the IP address of the peer host connected
 *  to the specified open socket handle.
 *
 *  The IP address is stored into the specified provided destination buffer.
*******************************************************************************/
extern  NTSOCKET_RET    ntsocket_getpeername (
    NTSOCKET_HANDLE const   *sockethandle,  /*!< [in] socket handle         */
/*@out@*/
    NTSOCKET_IPADDRESS      *pIpAddress)    /*!< [out] destination buffer   */
{
    struct  sockaddr_in peeraddress;
    socklen_t           yy;

    /* Validate the arguments.                                              */
    VALIDATE_PTR(sockethandle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(pIpAddress, NTSOCKET_RET_ENULLPOINTER);
                                    /** \return NTSOCKET_RET_ENULLPOINTER   */

    /* Make sure the socket handle is initialized.                          */
    if (!IS_INITIALIZED(*sockethandle))
        return NTSOCKET_RET_ENOTINITIALIZED;
                                /** \return NTSOCKET_RET_ENOTINITIALIZED    */

    /* Initialize the destination buffer.                                   */
    (void) memset(pIpAddress->ipAddress, 0, sizeof(pIpAddress->ipAddress));

    /* Get the peer's IP address. Report an error if this fails,            */
    /* which means that the socket is not connected.                        */
    yy = sizeof(peeraddress);
    if (getpeername(sockethandle->handle,
        (struct sockaddr *) &peeraddress, &yy) < 0)
        return NTSOCKET_RET_EBROKEN;
                                /** \return NTSOCKET_RET_EBROKEN            */

    /* Store the IP address as ASCII string for the caller.                 */
    (void) STRNCPY(pIpAddress->ipAddress, inet_ntoa(peeraddress.sin_addr),
        sizeof(pIpAddress->ipAddress));

    return NTSOCKET_RET_OK;     /** \return NTSOCKET_RET_OK                 */
}

/***************************************************************************//**
 *  This function returns the specified module's error status as text.
*******************************************************************************/
extern  char const  *ntsocket_geterror (
    NTSOCKET_RET    retcode)        /*!< [in] error status                  */
{
    switch (retcode)
    {
    case NTSOCKET_RET_OK:
        return "NTSSOCKET_RET_OK: everything was okay";
    case NTSOCKET_RET_CLOSED_BY_PEER:
        return "NTSOCKET_RET_CLOSED_BY_PEER: the peer has closed the connection";
    case NTSOCKET_RET_TIMEOUT:
        return "NTSOCKET_RET_TIMEOUT: a time-out condition occured";
    case NTSOCKET_RET_ENOTINITIALIZED:
        return "NTSOCKET_RET_ENOTINITIALIZED: "
            " the specified socket descriptor is not initialized";
    case NTSOCKET_RET_EBUSY:
        return "NTSOCKET_RET_EBUSY: the socket descriptor already "
            "is in use as client socket, server socket "
            " listening for connection requests, or as "
            " datagram server socket";
    case NTSOCKET_RET_ENTSOCKET:
        return "NTSOCKET_RET_ENTSOCKET: the specified socket "
            "descriptor is invalid for the specified operation";
    case NTSOCKET_RET_EPORTNUMBER:
        return "NTSOCKET_RET_EPORTNUMBER: a invalid portnumber "
            "is specified";
    case NTSOCKET_RET_EIPADDRESS:
        return "NTSOCKET_RET_EIPADDRESS: a invalid IP address is "
            "specified";
    case NTSOCKET_RET_ESTYLE:
        return "NTSOCKET_RET_ESTYLE: a invalid communication style "
            "is specified, or the specified communication style "
            "is not valid for the operation";
    case NTSOCKET_RET_ECREATE:
        return "NTSOCKET_RET_ECREATE: a socket couldn't be created";
    case NTSOCKET_RET_EBIND:
        return "NTSOCKET_RET_EBIND: the socket couldn't be bound to "
            "an ip-address";
    case NTSOCKET_RET_ELISTEN:
        return "NTSOCKET_RET_ELISTEN: the socket couldn't be made "
            "listening for incoming connection "
            "requests (only applicable for stream sockets)";
    case NTSOCKET_RET_ECLOSE:
        return "NTSOCKET_RET_ECLOSE: the socket couldn't be closed";
    case NTSOCKET_RET_EBUFFER:
        return "NTSOCKET_RET_EBUFFER: the specified buffer is "
            "invalid, because it's specified through a "
            "NULL-pointer, or the buffer's size is invalid";
    case NTSOCKET_RET_EBROKEN:
        return "NTSOCKET_RET_EBROKEN: the connection is probably "
            "broken";
    case NTSOCKET_RET_ECONNECT:
        return "NTSOCKET_RET_ECONNECT: the socket connection to "
            "the peer failed";
    case NTSOCKET_RET_ENULLPOINTER:
        return "NTSOCKET_RET_ENULLPOINTER: a invalid null reference "
            "was specified";
    case NTSOCKET_RET_ESOCKET:
        return "NTSOCKET_RET_ESOCKET: some kind of internal socket "
            "error occured";
    case NTSOCKET_RET_EINTERNAL:
        return "NTSOCKET_RET_EINTERNAL: a internal software error "
            "occured (generally based on unexpected OS "
            " behaviour)";
    case NTSOCKET_RET_EWRITE:
        return "NTSOCKET_RET_EWRITE: Write error occured";
    }

    return "undefined";         /** \return descriptive text                */
}


NTSOCKET_RET ntsocket_getsourcename (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_IPADDRESS* ip_addr)
{
    struct  sockaddr_in socket_address;
    socklen_t           yy;

    /* Validate the arguments. */
    VALIDATE_PTR(sock_handle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(ip_addr, NTSOCKET_RET_ENULLPOINTER);

    /* Make sure the socket handle is initialized. */
    if (!IS_INITIALIZED(*sock_handle))
           return NTSOCKET_RET_ENOTINITIALIZED;

    /* Initialize the destination buffer. */
    (void) memset(ip_addr->ipAddress, 0, sizeof(ip_addr->ipAddress));

    /* Get the source IP address. Report an error if this fails, */
       /* which means that the socket is not connected.          */
    yy = sizeof(socket_address);
    if (getsockname(sock_handle->handle, (struct sockaddr *) &socket_address, &yy) < 0)
    {
        return NTSOCKET_RET_EBROKEN;
    }


    /* Store the IP address as ASCII string for the caller. */
    (void) STRNCPY(ip_addr->ipAddress, inet_ntoa(socket_address.sin_addr), sizeof(ip_addr->ipAddress));

    return NTSOCKET_RET_OK;
}


NTSOCKET_RET ntsocket_getsourceport (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_PORT* port)
{
    struct  sockaddr_in socket_address;
    socklen_t           yy;

    /* Validate the arguments. */
    VALIDATE_PTR(sock_handle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(port, NTSOCKET_RET_ENULLPOINTER);

    /* Make sure the socket handle is initialized. */
    if (!IS_INITIALIZED(*sock_handle))
        return NTSOCKET_RET_ENOTINITIALIZED;

    /* Get the source IP address. Report an error if this fails, */
    /* which means that the socket is not connected.          */
    yy = sizeof(socket_address);
    if (getsockname(sock_handle->handle, (struct sockaddr *) &socket_address, &yy) < 0)
    {
        return NTSOCKET_RET_EBROKEN;
    }

    *port = ntohs(socket_address.sin_port);

    return NTSOCKET_RET_OK;
}


NTSOCKET_RET ntsocket_getpeerport (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_PORT* port)
{
    struct  sockaddr_in socket_address;
    socklen_t           yy;

    /* Validate the arguments. */
    VALIDATE_PTR(sock_handle, NTSOCKET_RET_ENULLPOINTER);
    VALIDATE_PTR(port, NTSOCKET_RET_ENULLPOINTER);

    /* Make sure the socket handle is initialized. */
    if (!IS_INITIALIZED(*sock_handle))
        return NTSOCKET_RET_ENOTINITIALIZED;

    /* Get the source IP address. Report an error if this fails, */
    /* which means that the socket is not connected.          */
    yy = sizeof(socket_address);
    if (getpeername(sock_handle->handle, (struct sockaddr *) &socket_address, &yy) < 0)
    {
        return NTSOCKET_RET_EBROKEN;
    }

    *port = ntohs(socket_address.sin_port);

    return NTSOCKET_RET_OK;
}


