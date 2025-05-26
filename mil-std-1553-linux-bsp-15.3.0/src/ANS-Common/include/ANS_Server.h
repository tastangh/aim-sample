/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Server.h
 *
 * \brief Generic ANS Server Thread implementation
 *
 * This module provides the ANS Server Thread.
 *
 *
 ******************************************************************************/
#ifndef ANS_SERVER_H
#define ANS_SERVER_H


#include "ANS_Types.h"
#include "ANS_ServerPeer.h"
#include "Ai_container.h"
#include "ANS_Board.h"
#include "ANS_CmdFrame.h"
#include "Ai_mutex.h"




#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/


/*! \struct AnsServerInfo
 *
 * Holds some information about the ANS Server application. \n
 * Is used in GetServerInfo command.
 */
struct AnsServerInfo
{
    AiChar name[128];           /*!< Name of the server as zero-terminated ASCII string */
    AiChar description[128];    /*!< Description of the server as zero-terminated ASCII string */
};


/*! \struct AnsServerVersion
 *
 * Holds the version of the ANS Server application.
 *
 */
struct AnsServerVersion
{
    AiUInt32  majorVersion;         /*!< Major Version of the ANS Server. Must be increased if either protocol API or ANS protocol changes */
    AiUInt32  minorVersion;         /*!< Minor Version of the ANS Server. Must be increased if either protocol API gets new functionality or ANS protocol */
    AiUInt32  patchVersion;         /*!< Patch Version of the ANS Server */
    AiChar    versionString[128];   /*!< Full version string of ANS Server. Should also contain Git specific information */
};


/*! \struct AnsCommandHandlers
 *
 * This structure holds all available command handlers
 * for an ANS Server
 */
struct AnsCommandHandlers
{
    AnsCmdRspFrame* (*getNumBoards)(AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*getServerInfo)(AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*protocolLibHandler)(AnsCmdFrame*, MemChunk*);
};


/*! \struct AnsBoardCommandHandlers
 *
 * This structure holds handler functions for all
 * board related commands a server implementation should support.
 */
struct AnsBoardCommandHandlers
{
    AnsCmdRspFrame* (*open)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*close)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*readMemory)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*writeMemory)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*ioCommand)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*ioCommandStruct)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*getDriverVersion)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*getSoftwareVersions)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*getDriverInfo)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*getBoardMemSize)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*openBoardEventStream)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*closeBoardEventStream)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*registerCallback)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*unregisterCallback)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
    AnsCmdRspFrame* (*protocolHandler)(struct AnsBoard* board, struct AnsConnection* connection, AnsCmdFrame*, MemChunk*);
};


/*! \struct AnsServer
 *
 * This structure describes an ANS Server.
 * A specific ANS server process will host exactly one instance of this type
 */
struct AnsServer
{
    AiUInt16 port;                                          /*!< The port the server will listen on for connections */
    AiUInt16 client_port;                                   /*!< The client port for UDP broad casts */
    struct AnsServerInfo info;                              /*!< Server specific information see \ref struct AnsServerInfo*/
    struct AnsServerVersion version;                        /*!< Version of the server. See \ref struct AnsServerVersion */
    struct AnsCommandHandlers command_handlers;             /*!< Command handler callbacks. See \ref struct AnsCommandHandlers */
    struct AnsBoardCommandHandlers board_command_handlers;  /*!< Board related command handlers. See \ref struct AnsBoardCommandHandlers */
    AiUInt32 hosted_board_count;                            /*!< Number of boards the server hosts */
    struct ai_list_head connected_peers;                    /*!< List of connected peers */
    struct ai_mutex* peer_lock;                             /*!< Lock for connected peers list */
    struct ai_list_head open_boards;                        /*!< List of all open boards that are attached to a server */
    struct ai_mutex* board_lock;                            /*!< Lock for open board list */
};


/*!< Global AnsServer instance used by ANS server applications
 */
extern struct AnsServer g_AnsServer;



/* Prototypes *****************************************************************/


/*! \brief Constructor of an ANS server
 *
 * @param server the server instance to initialize
 */
extern void AnsServer_init(struct AnsServer* server);


/*! \brief Broadcast server info for ANS detection
*
* This function broadcasts a I am here packet on the UDP port
* with the same port number as the TCP port.
* @param server the server to request connected peer from
* @return AnsStatus enum
*/
extern AnsStatus AnsServer_send_info(const struct AnsServer* server, int broadcast, const char * receiver, int port );

/*! \brief Request a connected peer instance from server
 *
 * This function will check if peer with specified ID
 * is connected. It will grab the reference count of the peer.
 * Hence peer must be released using \ref AnsServer_releasePeer
 * when no more accessed
 * @param server the server to request connected peer from
 * @param id ID of peer to request
 * @return pointer to requested peer if found, NULL if peer with given ID is not connected
 */
extern struct AnsServerPeer* AnsServer_requestPeer(const struct AnsServer* server, AnsServerPeerId id);


/*! \brief Create new peer
 *
 * This function will create a new peer instance on the specified server. \n
 * The caller will hold a reference to the peer afterwards and needs to release
 * it with \ref AnsServer_releasePeer once no more needed.
 * @param server the server to create peer instance for
 * @return pointer to created peer instance on success, NULL otherwise.
 */
extern struct AnsServerPeer* AnsServer_createPeer(struct AnsServer* server);


/*! \brief Release a peer
 *
 * Will release the reference for a specific peer.
 * Peer instance will be destroyed if no more references are held for it
 * @param server the server to release peer instance from
 * @param peer the peer to release
 */
extern void AnsServer_releasePeer(struct AnsServer* server, struct AnsServerPeer* peer);


/*! \brief Request a specific board from Server
 *
 * This function will check if board with specified handle
 * is available. It will grab the reference count of the board.
 * Hence you must release it with \ref AnsServer_releaseBoard once no more needed
 * @param server server to request board from
 * @param handle handle of the board to request
 * @return pointer to requested board on success. NULL if board is not available on server
 */
extern struct AnsBoard* AnsServer_requestBoard(struct AnsServer* server, AiHandle handle);


/*! \brief Create a board on server
 *
 * This function will create a new board instance on the specified server. \n
 * The caller will hold a reference to the board afterwards and needs to release
 * it with \ref AnsServer_releaseBoard once no more needed.
 * @param server the server to create board on
 * @param handle the handle to assign to the board
 * @return pointer to created board on success. NULL on failure
 */
extern struct AnsBoard* AnsServer_createBoard(struct AnsServer* server, AiHandle handle);


/*! \brief Release a specific board
 *
 * Will release the reference for a specific board.
 * Board instance will be destroyed if no more references are held for it
 * @param server the server to release the board from
 * @param board the board to release
 */
extern void AnsServer_releaseBoard(struct AnsServer* server, struct AnsBoard* board);


/*! \brief Set port the server will listen on for connections
*
* @param server the server to set port for
* @param port the port the server shall listen on
*/
extern void AnsServer_setPort(struct AnsServer* server, AiUInt16 port);


/*! \brief Set UDP port the client will listen on for device arrival messages
*
* @param server the server to set port for
* @param the port the client will listen on
*/
extern void AnsServer_setClientPort(struct AnsServer* server, AiUInt16 port);


/*! \brief Set server info
 *
 * @param server the server to set info for
 * @param new_info the info to set
 */
extern void AnsServer_setInfo(struct AnsServer* server, const struct AnsServerInfo* new_info);


/*! \brief Get server name
 *
 * @param server the server to get name of
 * @return name of the server on success, NULL on error
 */
static AI_INLINE const char* AnsServer_getName(const struct AnsServer* server)
{
    return server->info.name;
}


/*! \brief Get server description
 *
 * @param server the server to get description of
 * @return server description on success, NULL on error
 */
static AI_INLINE const char* AnsServer_getDescription(const struct AnsServer* server)
{
    return server->info.description;
}


/*! \brief Set server version
 *
 * @param server the server to set version for
 * @param new_version the version to set
 */
extern void AnsServer_setVersion(struct AnsServer* server, const struct AnsServerVersion* new_version);


/*! \brief Get major version of server
 *
 * @param server The server to get major version of
 * @return the server's major version
 */
static AiUInt32 AI_INLINE AnsServer_getMajorVersion(const struct AnsServer* server)
{
    return server->version.majorVersion;
}


/*! \brief Get minor version of server
 *
 * @param server The server to get minor version of
 * @return the server's minor version
 */
static AiUInt32 AI_INLINE AnsServer_getMinorVersion(const struct AnsServer* server)
{
    return server->version.minorVersion;
}


/*! \brief Get patch version of server
 *
 * @param server The server to get patch version of
 * @return the server's patch version
 */
static AiUInt32 AI_INLINE AnsServer_getPatchVersion(const struct AnsServer* server)
{
    return server->version.patchVersion;
}


/*! \brief get full version of server
 *
 * @param server the server to get version of
 * @return the full version as ASCII string
 */
static AI_INLINE const char* AnsServer_getVersion(const struct AnsServer* server)
{
    return server->version.versionString;
}


/*! \brief Set board command handlers of a server
 *
 * The function will not delete handlers that are already set but are not defined in the given set. \n
 * This means if handler is set to NULL in _new_handlers_ it will not overwrite the existing one
 * @param server the server to set board command handlers for
 * @param new_handlers the boar command handlers to set
 */
extern void AnsServer_setBoardCommandHandlers(struct AnsServer* server, const struct AnsBoardCommandHandlers* new_handlers);


/*! \brief Get board command handlers
 *
 * @param server the server to get board command handlers of
 * @return the current board command handlers of the server
 */
static AI_INLINE const struct AnsBoardCommandHandlers* AnsServer_getBoardCommandHandlers(const struct AnsServer* server)
{
    return &server->board_command_handlers;
}


/*! \brief Get number of boards the server hosts
 *
 * @param server the server to get number of hosted boards of
 * @return the number of hosted boards
 */
static AiUInt32 AI_INLINE AnsServer_getHostedBoardCount(const struct AnsServer* server)
{
    return server->hosted_board_count;
}


/*! \brief Set number of boards the server hosts
 *
 * @param server the server to set number of hosted boards of
 * @param num_boards the number of hosted boards to set
 */
extern void AnsServer_setHostedBoardCount(struct AnsServer* server, AiUInt32 num_boards);


/*! \brief Set a server's command handlers
 *
 * The function will not delete handlers that are already set but are not defined in the given set. \n
 * This means if handler is set to NULL in _handlers_ it will not overwrite the existing one
 * @param server the server to set command handlers for
 * @param handlers the handlers to set
 */
extern void AnsServer_setCommandHandlers(struct AnsServer* server, const struct AnsCommandHandlers* handlers);


/*! \brief Get command handlers of server
 *
 * @param server the server to get command handlers for
 * @return the command handlers
 */
static AI_INLINE const struct AnsCommandHandlers* AnsServer_getCommandHandlers(const struct AnsServer* server)
{
    return &server->command_handlers;
}


extern void *AnsServer_thread(void *pParam);
extern void *AnsServer_receive_broadcast_request(void *pParam);




#ifdef __cplusplus
}
#endif

#endif  /* ANS_SERVER_H */
