/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Socksv.h
 *
 * This file contains declarations used for creating
 * server side sockets
*******************************************************************************/
#ifndef AI_SOCKSV_H
#define AI_SOCKSV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

extern	NTSOCKET_RET	ntsocket_createserver (
	NTSOCKET	*ntsocket,
	unsigned int	port,
	int		maxconnections);

extern	NTSOCKET_RET	ntsocket_accept (
	NTSOCKET	*ntsocket,
	unsigned long	timeout,
	NTSOCKET_HANDLE	*sockethandle);

#ifdef __cplusplus
}
#endif


#endif
