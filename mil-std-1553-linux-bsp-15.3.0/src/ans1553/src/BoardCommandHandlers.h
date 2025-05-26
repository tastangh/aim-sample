/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */


/*! \file BoardCommandHandlers.h
 *
 *  This file contains declarations for the board
 *  command handler functions of the ANS1553
 */

#ifndef BOARDCOMMANDHANDLERS_H_
#define BOARDCOMMANDHANDLERS_H_


#include "ANS_CmdFrame.h"
#include "ANS_Board.h"




AnsCmdRspFrame* handleOpenCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleCloseCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleReadMemCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleWriteMemCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleIoCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleReadVersionCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleGetDriverVersionCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleIoCommandStruct(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleGetDriverInfoCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);


AnsCmdRspFrame* handleRegisterCallbackCommand(struct AnsBoard*, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);


AnsCmdRspFrame* handleUnregisterCallbackCommand(struct AnsBoard*, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);

#endif /* BOARDCOMMANDHANDLERS_H_ */
