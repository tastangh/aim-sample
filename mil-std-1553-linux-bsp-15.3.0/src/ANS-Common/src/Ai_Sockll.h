/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Sockll.h


 \brief socket low level functions

  This module provides functions internally used by the
  Ai_Socket modules.
*******************************************************************************/
#ifndef AI_SOCKLL_H
#define AI_SOCKLL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

#define NTSOCKET_C
#include "Ai_Socket.h"
#undef NTSOCKET_C

#if defined(__unix) || defined(_VXBUS)
# if defined(_MIT_POSIX_THREADS)
#  include <pthread/mit/pthread.h>
# elif defined(KERNELTHREADS)
#  include "pthread.h"
# endif
#endif
#include <time.h>

/* Konstanten *****************************************************************/

#define INITIALIZED_MAGICVALUE  0x39ae6e95L
                                    /*!< "magic" value to indicate          */
                                    /*!< initialized socket                 */
                                    /*!< descriptors                        */

#define FDSETNULL   ((fd_set *) 0)  /*!< NULL pointer reference of          */
                                    /*!< type "fd_set *" for use            */
                                    /*!< in calls to select().              */

/* Makros *********************************************************************/

/**
 * This macro checks whether the specified socket descriptor or
 * "ntsocket handle" is initialized properly.
 */
#define IS_INITIALIZED(socket)  ((socket).initialized == INITIALIZED_MAGICVALUE)

/**
 * This macro checks whether the specified port number is within a valid range.
 */
#define IS_VALIDPORT(port)  ((unsigned) (port) <= NTSOCKET_MAXPORT)

/* Typen **********************************************************************/

/**
 * Description of possible socket handle states.
 */
typedef enum tagSocketStatus_t
{
    SOCKETSTATUS_UNCONNECTED,   /*!< not connected                          */
    SOCKETSTATUS_BUSY           /*!< connected or somewhat busy             */
} SOCKETSTATUS;

/**
 * Description of the data peek mode.
 */
typedef enum tagPeekMode_t
{
    PEEK_MODE_READ,             /*!< check the incoming connection          */
    PEEK_MODE_WRITE             /*!< check the outgoing connection          */
} PEEK_MODE;

/* Prototypen *****************************************************************/

 extern struct timeval  *ntsockll_timeout2timeval (/*@returned@*/
    struct timeval  *timestruct,
    unsigned long   timeout);

 extern void        ntsockll_clearhandle (/*@out@*/
    NTSOCKET_HANDLE *sockethandle);

 extern void        ntsockll_setsockethandle (/*@out@*/
    NTSOCKET_HANDLE *sockethandle,
    AiSocketFD      handle,
    int             status,
    NTSOCKET_STYLE  style,
    int             isclient,
    int             isserver);

 extern NTSOCKET_RET    ntsockll_peek (
     AiSocketFD     sockethandle,
    unsigned long   timeout,
    PEEK_MODE       mode);

#ifdef __cplusplus
}
#endif


#endif
