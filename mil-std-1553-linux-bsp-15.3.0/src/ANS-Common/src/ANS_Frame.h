/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Frame.h
 *
 * \brief Generic ANS frame implementation
 *
 * This file contains the implementation of the generic ANS frame, as well
 * as read and write functions
 *
 *
 ******************************************************************************/
#ifndef ANS_FRAME_H
#define ANS_FRAME_H

/* Includes *******************************************************************/

#include "ANS_Header.h"
#include "Ai_cdef.h"
#include "ANS_MemChunk.h"

/* Constants ******************************************************************/
                            
/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

extern AnsStatus ANS_Frame_read (
    struct AnsConnection* connection,
    unsigned long const timeOutMs,
    MemChunk            *pRxBuffer);

#endif /* ANS_FRAME_H */
