/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file BoardCommandHandlers.h
 *
 *  This file contains declarations for the board
 *  command handler functions of the ANS1553
 *
 */

#ifndef BOARDPROTOCOLHANDLERS_H_
#define BOARDPROTOCOLHANDLERS_H_


#include "ANS_CmdFrame.h"
#include "ANS_Board.h"



AnsCmdRspFrame* handleProtocolCommand(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame* command, MemChunk* response_memory);

AnsCmdRspFrame* handleDataQueueOpenCommand(   AnsCmdFrame*, MemChunk*);
AnsCmdRspFrame* handleDataQueueCloseCommand(  AnsCmdFrame*, MemChunk*);
AnsCmdRspFrame* handleDataQueueControlCommand(AnsCmdFrame*, MemChunk*);
AnsCmdRspFrame* handleDataQueueReadCommand(   AnsCmdFrame*, MemChunk*);

AnsCmdRspFrame* handleGetDeviceConfigCommand(   AnsCmdFrame*, MemChunk*);
AnsCmdRspFrame* handleSetDeviceConfigCommand(   AnsCmdFrame*, MemChunk*);


#endif /* BOARDPROTOCOLHANDLERS_H_ */
