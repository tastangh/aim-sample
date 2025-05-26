/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socket.h

 \brief System independent TCP/IP sockets (BSD and Winsock support)

  This module provides functions to access the systems TCP/IP socket
  API in a system-independent manner.

  This module currently supports following systems with the specified
  compile-time defines:

    TARGET_QNX  - QNX 4
    TARGET_OS2  - OS/2
    __unix  - UNIX systems, BSD socket based sockets
    WIN32   - Windows NT/95, WINSOCK-API based sockets

  Brief usage description:

 1. TCP sockets

 1.1 Communication with server sides

    o Perform startup operation (ntsocket_startup)
    o Create the socket descriptor (ntsocket_create)
    o Connect the socket to a server (ntsocket_connect)
    o Use the connected socket handle to perform communication with
      the connected peer (ntsocket_write, _read, _getc, _fgets, ...)
    o Disconnect the connected socket (ntsocket_disconnect) after
      communication is done
    o Deinitialize the socket descriptor (ntsocket_destroy)
    o Perform cleanup operation (ntsocket_cleanup)

 1.2 Communication with client sides

    o Perform startup operation (ntsocket_startup)
    o Create the socket descriptor (ntsocket_create)
    o Create the server socket endpoint (ntsocket_createserver)
    o Watch out for incoming connection requests, accept the
      pending request (ntsocket_accept)
    o Use the connected socket handle to perform communication with
      the connected peer (ntsocket_write, _read, _getc, _fgets, ...)
    o Disconnect the connected socket (ntsocket_disconnect) after
      communication is done
    o Deinitialize the socket descriptor (ntsocket_destroy)
    o Perform cleanup operation (ntsocket_cleanup)

 2. UDP sockets

 2.1 Communication with server sides

    o Perform startup operation (ntsocket_startup)
    o Create the socket descriptor (ntsocket_create)
    o 'Connect' the socket to a server (ntsocket_connect)
    o Use the 'connected' socket handle to perform communication with
      the connected peer (ntsocket_writedatagram, ntsocket_readdatagram)
    o Disconnect the connected socket (ntsocket_disconnect) after
      communication is done
    o Deinitialize the socket descriptor (ntsocket_destroy)
    o Perform cleanup operation (ntsocket_cleanup)

 2.2 Communication with client sides

    o Perform startup operation (ntsocket_startup)
    o Create the socket descriptor (ntsocket_create)
    o Create the server socket endpoint (ntsocket_createserver)
    o * Watch out for incoming datagrams, accept the
        pending datagrams (ntsocket_accept)
    o Use the 'connected' socket handle to perform communication with
      the connected peer (ntsocket_writedatagram, ntsocket_readdatagram)
    o Back to step * (don't 'disconnect')
    o Disconnect the connected socket (ntsocket_disconnect) after
      all communication is done
    o Deinitialize the socket descriptor (ntsocket_destroy)
    o Perform cleanup operation (ntsocket_cleanup)

\section Glossary
\subsection AL Application Layer
 This layer of the OSI Reference Model consists of application programs
 that use the network.
 In the context of TCP/IP a application is any network process that
 occurs above the Transport Layer.

\subsection Datagram
 The "datagram" is the packet format defined by Internet Protocol.

\subsection DLL Data Link Layer
 This layer of the OSI Reference Model provides reliable data delivery
 accross the phyisal link.
 TCP/IP rarely creates protocols in the Data Link Layer. Most RFCs that
 relate to the Data Link Layer talk about how IP can make use of
 existing data link protocols.

\subsection ICMP
 An integral part of IP is the "Internet Control Message Protocol" (ICMP)
 defined in RFC 792. This protocol is part of the Internet Layer and
 uses IP datagram delivery facility to send its messages.
 ICMP sends messages that perform the following control, error reporting,
 and informational functions for TCP/IP

    o Flow Control
      When datagrams arrive too fast for processing, the destination
      host or an intermediate gateway sends an ICMP "Source Quench
      Message" back to the sender. This tells the source to
      temporarily stop sending datagrams.

    o Detecting unreachable destinations
      When a destination is unreachable, the system detecting the
      problem sends a "Destination Unreachable Message" to the
      datagram's source.
      If the unreachable destination is a network or a host, the
      message is sent by an intermediate gateway. But if the
      destination is an unreachable port, the destination host
      sends the message.

    o Redirecting routes
      A gateway sends the "ICMP Redirect Message" to tell a host to
      use another gateway, presumably because the other gateway is a
      better choice. This message can only be used when the source
      host is on the same network as both gateways.

    o Checking remote hosts
      A host can send the "ICMP Echo Message" to see if a remote
      system's Internet Protocol is up and operational. When a
      system receives an echo message, it sends the same packet
      back to the source host.
      The UNIX "ping" command uses this message.

\subsection IP
 The "Internet Protocol" is the building block of
 the Internet. Its functions include

    o defining the datagram, which is the basic unit of transmission
      in the Internet;
    o defining the Internet addressing scheme;
    o moving data between the Network Access Layer and the Host-to-Host
      Transport Layer;
    o routing datagrams to remote hosts;
    o performing fragmentation and re-assembly of datagrams.

 IP is a connectionless protocol. This means that IP doesn't exchange
 control information (called a "handshake") to establish an end-to-end
 connection before transmitting data. Internet Protocol relies on
 protocols in other layers to establish the connection if they require
 connection-orientated service.
 IP also relies on protocols in the other layers to provide error
 detection and error recovery. The Internet Protocol is sometimes
 called an "unreliable protocol" because it contains no error detection
 and recovery code. This is not to say that the IP protocol cannot be
 relied on. IP can be relied upon to accurately
 deliver your data to the connected network, but it doesn't check
 whether the data was correctly received. Protocols in other layers
 of the TCP/IP architecture provide this checking if it is required.

\subsection ISO
 International Standards Organization

\subsection NL Network Layer
 This layer of the OSI Reference Model manages connections across the
 network for the upper layers.
 The Internet Protocol (IP), which isolates the upper layers from the
 underlying network and handles the addressing and delivery of data,
 is usually described as TCP/IP's Network Layer.

\subsection OSI OSI Reference Model
 An architectural model developed by the ISO is frequently used to
 describe the structure and function of data communication protocols.
 This architectural model, called the "Open Systems Interconnect (OSI)
 Reference Model", provides a common reference for discussing
 communications. The OSI Reference Model contains seven "layers" that
 define the functions of data communication protocols.

 The layers identified by name are

    o Application Layer
    o Presentation Layer
    o Session Layer
    o Transport Layer
    o Network Layer
    o Data Link Layer
    o Physical Layer

\subsection PL Physical Layer
 This layer of the OSI Reference Model defines the physical
 characteristics of the network media.
 TCP/IP does not define physical standards. It makes use of existing
 standards.

\subsection Port Port number
 The 16-bit number called the "port number" identifies the application
 that the TCP/IP data is bound for.

\subsection PL Presentation Layer
 This layer of the OSI Reference Model standardizes data presentation to
 the applications.
 In the context of TCP/IP this function is handled within the
 applications.

\subsection SL Session Layer
 This layer of the OSI Reference Model manages sessions between
 applications.
 In the context of TCP/IP this layer is not identifiable as separate
 layer in the protocol hierarchie. In TCP/IP, this function largely
 occurs in the Transport Layer, and the term "session" is not used.
 For TCP/IP, the terms "socket" and "port" are used to describe the path
 over which cooperating applications communicate.

\subsection Socket
 The combination of an IP address and a port number is called a
 "socket". A socket uniquely identifies a single network process within
 the entire Internet (or the local network/machine if not connected to
 the Internet). A pair of sockets, one socket for the receiving host
 and one for the sending host, define the connection for connection-
 orientated protocols such as TCP.

\subsection TCP
 Applications that use the transport protocol to provide reliable data
 delivery use the "Transmission Control Protocol" (TCP) because it
 verifies that data is delivered accross the network accurately and in
 the proper sequence. TCP is a reliable, connection-orientated, byte-
 stream protocol.
 TCP provides reliability with a mechanism called "Positive
 Acknowledgement with Re-transmission" (PAR).
 Simply stated, a system using PAR sends the data again, unless it
 hears from the remote system that the data arrived okay.
 The unit of data exchanged between cooperating TCP modules is called
 a "segment". Each segment contains a checksum that the recipient uses
 to verify that the data is undamaged. If the data segment is received
 undamaged, the receiver sends a "positive acknowledgement" back to the
 sender. If the data segment is damaged, the receiver discards it.
 After an appropriate time-out period, the sending TCP module
 re-transmits any segment for which no positive acknowledgment
 has been received.
 TCP is connection\-orientated. It establishes a logical end-to-end
 connection between the two communicating hosts. Control information,
 called a "handshake", is exchanged between the two endpoints to
 establish a dialogue before data is transmitted.

\subsection TL Transport Layer
 This layer of the OSI Reference Model provides end-to-end error
 detection and correction.
 In TCP/IP this function is performed by the "Transmission Control
 Protocol" (TCP).
 However, TCP/IP offers a second Transport Layer service, "User
 Datagram Protocol" (UDP) that does not perform the end-to-end
 reliability checks.

\subsection UDP UDP
 The "User Datagram Protocol" gives application programs direct access to
 a datagram delivery service, like the delivery service that IP provides.
 This allows applications to exchange messages over the network with a
 minimum of protocol overhead.
 UDP is an unreliable, connectionless datagram protocol. Within your
 computer, UDP will deliver data correctly.

*******************************************************************************/
#ifndef AI_SOCKET_H
#define AI_SOCKET_H


#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Target system checking *****************************************************/

#undef  TARGET_SUPPORTED
#if defined(__unix) || defined(TARGET_QNX) || defined(_VXBUS)
# define TARGET_SUPPORTED   1
#elif   defined(TARGET_OS2)
# define TARGET_SUPPORTED   1
#elif   defined(WIN32)
# define TARGET_SUPPORTED   1
#elif   defined(_VXBUS)
# define TARGET_SUPPORTED   1
#else
# error "This target OS isn't supported yet"
#endif

/* Include-Dateien ************************************************************/

#if defined(__unix) || defined(TARGET_OS2) || defined(_VXBUS)
    #include <sys/socket.h>
#elif defined WIN32
    #include "WinSock2.h"
#endif

/* Konstanten *****************************************************************/

#define NTSOCKET_MAXPORT    0xFFFFU
                            /*!< Highest possible port number       */
                            /*!< (limited to 16-bit)                */
#if defined(SOMAXCONN)
#define NTSOCKET_MAXCONNECTIONS SOMAXCONN
                            /*!< Maximal number of pending incoming */
                            /*!< connection (queue length), if      */
                            /*!< 'SOMAXCONN' is defined in the      */
                            /*!< header file 'sys/socket.h'.        */
#else
#define NTSOCKET_MAXCONNECTIONS 5
                            /*!< Maximal number of pending incoming */
                            /*!< connection (queue length), if      */
                            /*!< there is no 'SOMAXCONN' defined.   */
                            /*!< This value is taken from the "BSD  */
                            /*!< Socket Reference document" and may */
                            /*!< differ on some systems, but should */
                            /*!< be reasonably secure.              */
#endif

#define NTSOCKET_HANDLEINVALID  -1
                            /*!< Indicator of a invalid socket      */
                            /*!< handle                             */

/* Predefined time-out values   */
#define NTSOCKET_TIMEOUT_NONE       0L
                                    /*!< Time-out value indicating  */
                                    /*!< "no wait", enforces        */
                                    /*!< immediate processing       */
#define NTSOCKET_TIMEOUT_INFINITE   0xFFFFFFFFLU
                                    /*!< Time-out value indicating  */
                                    /*!< "wait forever", enforces   */
                                    /*!< blocked mode processing    */
#define NTSOCKET_IPADDRESS_SIZE 16  /*!< Minimal size of a buffer   */
                                    /*!< to hold a IP address in    */
                                    /*!< dotted decimal notation.   */

/* Makros *********************************************************************/

/* Typen **********************************************************************/

#if defined(__unix) || defined(_VXBUS)
    typedef int AiSocketFD;
#elif defined WIN32
    typedef SOCKET AiSocketFD;
#endif


/**
 * Type description of the module's function return codes.
 */
typedef enum tagNTSocketRet_t
{
    NTSOCKET_RET_OK,        /*!< everything was okay            */
    NTSOCKET_RET_CLOSED_BY_PEER,
                            /*!< the peer has performed an orderly  */
                            /*!< shutdown on the socket             */
    NTSOCKET_RET_TIMEOUT,   /*!< a time-out condition occured       */
    NTSOCKET_RET_ENOTINITIALIZED,
                            /*!< the specified socket descriptor is */
                            /*!< not initialized            */
    NTSOCKET_RET_EBUSY,     /*!< the socket descriptor already is in    */
                            /*!< use as client socket, server socket    */
                            /*!< listening for connection requests,     */
                            /*!< or as datagram server socket       */
    NTSOCKET_RET_ENTSOCKET, /*!< the specified socket descriptor is */
                            /*!< invalid for the specified operation    */
    NTSOCKET_RET_EPORTNUMBER,
                            /*!< a invalid portnumber is specified  */
    NTSOCKET_RET_EIPADDRESS,
                            /*!< a invalid IP address is specified  */
    NTSOCKET_RET_ESTYLE,    /*!< a invalid communication style is   */
                            /*!< specified, or the specified        */
                            /*!< communication style is not valid for   */
                            /*!< the operation          */
    NTSOCKET_RET_ECREATE,   /*!< a socket couldn't be created       */
    NTSOCKET_RET_EBIND,     /*!< the socket couldn't be bound to    */
                            /*!< an ip-address          */
    NTSOCKET_RET_ELISTEN,   /*!< the socket couldn't be made        */
                            /*!< listening for incoming connection  */
                            /*!< requests (only applicable for stream   */
                            /*!< sockets)               */
    NTSOCKET_RET_ECLOSE,    /*!< the socket couldn't be closed  */
    NTSOCKET_RET_EBUFFER,   /*!< the specified buffer is invalid,   */
                            /*!< because it's specified through a   */
                            /*!< NULL-pointer, or the buffer's size */
                            /*!< is invalid             */
    NTSOCKET_RET_EBROKEN,   /*!< the connection is probably broken  */
    NTSOCKET_RET_ECONNECT,  /*!< the socket connection to the peer  */
                            /*!< failed             */
    NTSOCKET_RET_ENULLPOINTER,
                            /*!< a invalid null reference was specified */
    NTSOCKET_RET_ESOCKET,   /*!< some kind of internal socket error */
                            /*!< occured                */
    NTSOCKET_RET_EINTERNAL, /*!< a internal software error occured  */
                            /*!< (generally based on unexpected OS  */
                            /*!< behaviour)             */
    NTSOCKET_RET_EWRITE,    /*!< an error occured while writing the */
                            /*!< the data to the socket. Possible   */
                            /*!< reasons are that the socket buffer */
                            /*!< overflow occured due to the remote */
                            /*!< station did not receive the data or    */
                            /*!< the connection has been broken down    */
} NTSOCKET_RET;

/**
 * Description of supported styles of communication.
 */
typedef enum tagNTSocketStyle_t
{
    NTSOCKET_STYLE_TCP, /*!< TCP (stream socket, connection orientated */
    NTSOCKET_STYLE_UDP, /*!< UDP (datagram socket, connectionless   */
                        /*!< and unreliable)            */

    /* reserved entries */
    NTSOCKET_STYLE_RESERVED /*!< reserved (last) entry      */

} NTSOCKET_STYLE;

/**
 * Description of a buffer to hold an IP address in dotted decimal
 */
typedef struct tagNTSocketIpAddress_t
{
    char    ipAddress[NTSOCKET_IPADDRESS_SIZE];

} NTSOCKET_IPADDRESS;


typedef unsigned short NTSOCKET_PORT;


/**
 * Definition of NTSOCKET.
 */
typedef struct tagNTSocket_t
{
    int     initialized;    /*!< initialization indicator       */
    NTSOCKET_STYLE  style;  /*!< communication style (TCP,  */
                            /*!< UDP, etc.)                 */
    AiSocketFD socket;        /*!< socket handle              */
    int     isbusy;         /*!< flag: socket is in use as  */
                            /*!< client socket or some kind */
                            /*!< of server socket               */
    int     isconnected;    /*!< flag: socket is connected    */
                            /*!< either as client or            */
                            /*!< installed as server            */
    int     islistening;    /*!< flag: the socket is listening */
                            /*!< for incoming connection        */
                            /*!< requests                       */
    int     isclient;       /*!< flag: socket descriptor is   */
                            /*!< initialized as 'client     */
                            /*!< socket                     */
    int     isserver;       /*!< flag: socket descriptor is   */
                            /*!< initialized as 'server     */
                            /*!< socket'                        */
} NTSOCKET;

/**
 * Definition of NTSOCKET_HANDLE.
 */
typedef struct tagNTSocketHandle_t
{
    int     initialized;    /*!< initialization indicator   */
    AiSocketFD handle;     /*!< the socket handle      */
    int     status;         /*!< the status of the handle   */
    NTSOCKET_STYLE  style;  /*!< the communication style    */
    int     isclient;       /*!< flag: socket is to be  */
                            /*!< treated as client socket   */
    int     isserver;       /*!< flag: socket is            */
                            /*!< initialized as 'server */
                            /*!< socket'                    */
} NTSOCKET_HANDLE;

/* Prototypen *****************************************************************/

 extern int     ntsocket_startup (void);

 extern int     ntsocket_cleanup (void);

 extern NTSOCKET_RET    ntsocket_create (/*@out@*/
    NTSOCKET    *ntsocket,
    NTSOCKET_STYLE  ntsocket_style);

 extern  NTSOCKET_RET    ntsocket_create_udp(
     NTSOCKET    *ntsocket,
     int          broad_cast );

 extern  NTSOCKET_RET    ntsocket_bind_udp_bcast(
     NTSOCKET        *ntsocket,  /*!< [in,out] initialized socket descriptor */
     unsigned int    port);

 extern NTSOCKET_RET    ntsocket_destroy (
    NTSOCKET    *ntsocket);

 extern NTSOCKET_RET    ntsocket_connect (
    NTSOCKET    *ntsocket,
    unsigned int    port,
    char const  *ipaddress,
/*@out@*/
    NTSOCKET_HANDLE *sockethandle);

 extern NTSOCKET_RET    ntsocket_disconnect (
    NTSOCKET_HANDLE *sockethandle);

 extern NTSOCKET_RET    ntsocket_read (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout,
/*@out@*/
    char        *buffer,
    size_t      buffersize,
/*@out@*/
    size_t      *nread);

 extern NTSOCKET_RET    ntsocket_readNextData (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout,
/*@out@*/
    char        *buffer,
    size_t      buffersize,
/*@out@*/
    size_t      *nread);

 extern  NTSOCKET_RET    ntsocket_readBlock (
     NTSOCKET_HANDLE *phSocket,
     unsigned long   timeout,
/*@out@*/
     char        *pBuffer,
     size_t      bytesRequired,
 /*@out@*/
     size_t      *pNread);

 extern NTSOCKET_RET    ntsocket_fgets (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout,
/*@out@*/
    char        *buffer,
    size_t      buffersize);

 extern NTSOCKET_RET    ntsocket_getc (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout,
/*@out@*/
    char        *ch);

 extern  NTSOCKET_RET    ntsocket_sendto(
     NTSOCKET *sockethandle,
     char const  *ip,
     int         port,
     void        *buffer,
     int         length,
     int         *nwritten);

 extern  NTSOCKET_RET    ntsocket_recvfrom(
     NTSOCKET *socket,
     void        *buffer,
     int         length,
     int         *nreceived,
     char       * ip );

 extern NTSOCKET_RET    ntsocket_write (
    NTSOCKET_HANDLE *sockethandle,
    char const  *buffer,
    int     length,
/*@out@*//*@null@*/
    int     *nwritten);

 extern NTSOCKET_RET    ntsocket_writetimed (
    NTSOCKET_HANDLE *sockethandle,
    char const  *buffer,
    size_t      length,
/*@out@*//*@null@*/
    size_t      *nwritten,
    unsigned long   timeout);

 extern NTSOCKET_RET    ntsocket_putc (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout,
    char        ch);

 extern void ntsocket_flush (
     NTSOCKET_HANDLE *sockethandle);

 extern NTSOCKET_RET    ntsocket_peek (
    NTSOCKET_HANDLE *sockethandle,
    unsigned long   timeout);

 extern NTSOCKET_RET        ntsocket_isconnected (
    const NTSOCKET_HANDLE   *handle);

#ifdef __cplusplus
}
#endif


#endif
