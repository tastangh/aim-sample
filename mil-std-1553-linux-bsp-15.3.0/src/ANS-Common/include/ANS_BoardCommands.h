/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_BoardCommands.h
 *
 *  This header file contains definitions for generic,
 *  board related commands
 *
 */

#ifndef ANS_BOARDCOMMANDS_H_
#define ANS_BOARDCOMMANDS_H_




/*! \enum BoardCommandID
 * Enumeration for board related command IDs \n
 * Values equal or greater than BoardCommandID can be used for protocol
 * specific board commands.
 */
typedef enum eBoardCommandID
{
    OpenBoardID = 0,            /*!< OpenBoardID    */
    CloseBoardID,               /*!< CloseBoardID   */
    ReadMemoryID,               /*!< ReadMemoryID   */
    WriteMemoryID,              /*!< WriteMemoryID  */
    IoCommandID,                /*!< IoCommandID    */
    IoCommandStructID,          /*!< IoCommandStructID  */
    GetDriverVersionID,         /*!< GetDriverVersionID */
    GetSoftwareVersionsID,      /*!< GetSoftwareVersionsID  */
    GetDriverInfoID,            /*!< GetDriverInfoID    */
    GetBoardMemSizeID,          /*!< ID for command that retrieves board memory sizes */
    OpenBoardEventStreamID,     /*!< ID for command that opens an event notification stream for a board */
    CloseBoardEventStreamID,    /*!< ID for command that closes an event notification stream of a board */
    RegisterCallbackID,         /*!< ID for registering board event callbacks */
    UnregisterCallbackID,       /*!< ID for unregistering board event callbacks */
    BoardCommandProtID = 256    /*!< BoardCommandProtID */
}BoardCommandID;


#pragma pack(1)

/*! \struct AnsOpenBoardEventStreamResponsePayload
 *
 * This structure contains the response for the OpenBoardEventStreamID command
 */
struct AnsOpenBoardEventStreamResponsePayload
{
    AiUInt32 handle; /*!< Handle of the opened event stream. This is a 32bit value as AiHandle has size of 32bit on 32bit peers */
    AiUInt16 port;   /*!< port the event stream can be connected to. Is in little endian format */
};
#pragma pack()


#pragma pack(1)

/*! \struct AnsCloseBoardEventStreamPayload
 *
 * This structure contains the command parameters for the CloseBoardEventStreamID command
 */
struct AnsCloseBoardEventStreamPayload
{
    AiUInt32 handle;    /*!< Handle of the event stream to close */
};
#pragma pack()



#endif /* ANS_BOARDCOMMANDS_H_ */
