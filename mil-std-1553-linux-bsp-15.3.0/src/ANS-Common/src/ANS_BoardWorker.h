/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_BoardWorker.h
 *
 * This module provides the ANS board worker thread.
 *
 *
 ******************************************************************************/
#ifndef ANS_BOARDWORKER_H
#define ANS_BOARDWORKER_H

/* Includes *******************************************************************/

#ifndef WIN32
# include <pthread.h>
#endif
#include "ANS_Types.h"
#include "ANS_CmdFrame.h"
#include "ANS_Client.h"
#include "Ai_Socket.h"
#include "ANS_Thread.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

extern long int ANS_BoardWorkerCallback (
    AnsThreadId         threadId,
    struct AnsServerPeer* peer,
    struct AnsConnection* connection);


/*! \brief This is a generic handler for the OpenBoardEventStreamID board command
 *
 * @param board the board to open event stream for
 * @param connection the connection the command was received on
 * @param cmdFrame the raw command frame received
 * @param responseMemory the memory pool to use for sending the command response
 * @return NULL if handler was successful, as it will send its own response
 */
extern AnsCmdRspFrame* ANS_OpenBoardEventStream(struct AnsBoard* board, struct AnsConnection* connection,
                                                AnsCmdFrame* cmdFrame, MemChunk* responseMemory);


/*! \brief This is a generic handler for the CloseBoardEventStreamID board command
 *
 * @param board the board to close event stream for
 * @param connection the connection the command was received on
 * @param cmdFrame the raw command frame received
 * @param responseMemory the memory pool to use for sending the command response
 * @return the response frame that shall be sent back
 */
extern AnsCmdRspFrame* ANS_CloseBoardEventStream(struct AnsBoard* board, struct AnsConnection* connection,
                                                 AnsCmdFrame* cmdFrame, MemChunk* responseMemory);


#endif  /* ANS_BOARDWORKER_H */
