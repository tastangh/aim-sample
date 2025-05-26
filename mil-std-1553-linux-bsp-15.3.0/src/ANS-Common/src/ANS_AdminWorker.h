/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_AdminWorker.h
 *
 * This module provides the ANS administrative commands worker thread.
 *
 *
 ******************************************************************************/
#ifndef ANS_ADMINWORKER_H
#define ANS_ADMINWORKER_H

/* Includes *******************************************************************/

#include "Ai_Socket.h"
#include "ANS_Client.h"
#include "ANS_CmdFrame.h"
#include "ANS_Thread.h"




/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

extern long int ANS_AdminWorkerCallback (
    AnsThreadId         threadId,
    struct AnsServerPeer* peer,
    struct AnsConnection* connection);


extern AnsCmdRspFrame* ANS_GetNumBoardsHandler(AnsCmdFrame* cmdFrame, MemChunk* responseMemory);


extern AnsCmdRspFrame* ANS_GetServerInfoHandler(AnsCmdFrame* cmdFrame, MemChunk* responseMemory);



#endif  /* ANS_ADMINWORKER_H */
