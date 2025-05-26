/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socktl.h


 \brief socket additional tools

  This module provides additional tools for
  which simplify the usage of the Ai_Socket module.

*******************************************************************************/
#ifndef AI_SOCKTL_H
#define AI_SOCKTL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

#include "Ai_Socket.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

extern  int         ntsocket_setip (/*@out@*/
    NTSOCKET_IPADDRESS  *pAdr,
    char const          *val);

extern  int         ntsocket_isvalidip (
    char const      *val);

extern  NTSOCKET_RET    ntsocket_getpeername (
    NTSOCKET_HANDLE const   *sockethandle,
/*@out@*/
    NTSOCKET_IPADDRESS  *pIpAddress);

extern  char const  *ntsocket_geterror (
    NTSOCKET_RET    retcode);

extern  NTSOCKET_RET ntsocket_getsourcename (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_IPADDRESS* ip_addr);


extern  NTSOCKET_RET ntsocket_getsourceport (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_PORT* port);


extern  NTSOCKET_RET ntsocket_getpeerport (const NTSOCKET_HANDLE* sock_handle, NTSOCKET_PORT* port);



#ifdef __cplusplus
}
#endif

#endif
