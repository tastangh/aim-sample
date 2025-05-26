/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Multiplexer.h
 *
 * This module provides the ANS connection multiplexer thread.
 *
 *
 ******************************************************************************/
#ifndef ANS_MULTIPLEXER_H
#define ANS_MULTIPLEXER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

extern void *ANS_MultiplexerThread (void *thisarg);

#ifdef __cplusplus
}
#endif

#endif  /* ANS_MULTIPLEXER_H */
