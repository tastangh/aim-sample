/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_ClientPeer.h
 *
 */

#ifndef ANS_CLIENTPEER_H_
#define ANS_CLIENTPEER_H_


#include "ANS_Connection.h"
#include "ANS_AdminCommands.h"
#include "ANS_ServerPeer.h"
#include "ANS_Board.h"


/*! \def ANS_CLIENT_PEER_MAX_ADDR_LEN
 * Maximum number of characters an ANS peer address can consists of
 */
#define ANS_CLIENT_PEER_MAX_ADDR_LEN 128



/*! \struct AnsClientPeer
 *
 * This structure represents on specific peer that is connected to
 * an ANS client application
 */
struct AnsClientPeer
{
    char address[ANS_CLIENT_PEER_MAX_ADDR_LEN];      /*!< The address of the peer. Either an IP address, or a DNS resolvable symbolic name */
    unsigned short port;                             /*!< The port of the peer */
    AnsServerPeerId peer_id;                         /*!< ID of the peer */
    struct ai_mutex* connection_lock;                /*!< Lock for the peer connection */
    struct AnsConnection* connection;                /*!< The primary connection to the peer */
    struct ai_list_head list;                        /*!< List anchor to add the peer to a list */
    struct ai_list_head boards;                      /*!< List of open boards on that peer */
    struct ai_mutex* board_lock;                     /*!< Lock for synchronizing access to board list */
    unsigned int reference_count;                    /*!< Reference count for the peer instance */
};



/*! \brief Initializes an AnsClientPeer structure
 *
 * @param peer the structure to initialize
 * @param url URL of the peer
 * @param port port of the peer
 */
extern void AnsClientPeer_init(struct AnsClientPeer* peer, const char* url, unsigned short port);


/*! \brief Open connection a peer
 *
 * Will establish the primary connection to an ANS server instance
 * and perform the initial protocol handshake
 * @param peer the peer to connect to
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsClientPeer_connect(struct AnsClientPeer* peer);


/*! \brief Request number of available boards on a specific peer instance
 *
 * @param peer the peer to request board count of
 * @param board_count the number of boards on the peer will be stored here
 * @return AnsStatus_OK on success, a negative error code otherwise
 */
extern AnsStatus AnsClientPeer_requestBoardCount(struct AnsClientPeer* peer, unsigned int* board_count);


/*! \brief Get server specific information from a peer
 *
 * @param peer the peer to get information of
 * @param server_info the server information will be stored here
 * @return AnsStatus_OK on success, a negative error code otherwise
 */
extern AnsStatus AnsClientPeer_getServerInfo(struct AnsClientPeer* peer, AnsGetServerInfoResponsePayload* server_info);


/*! \brief Transmit command to peer and await response
 *
 * This function can be used to transmit arbitrary command frames to the peer.
 * The function will block until response is received or until timeout of \ref  ANS_CMD_RESP_TIMEOUT_MS
 * @param peer the peer to transmit command to
 * @param command the command to transmit
 * @param command_size the size of the command frame in bytes
 * @param response_memory the response frame will be stored here
 * @return AnsStatus_OK on success, a negative error code otherwise
 */
extern AnsStatus AnsClientPeer_transmitCommand(struct AnsClientPeer* peer, AnsCmdFrame* command, AiSize command_size,
                                               MemChunk* response_memory);


/*! \brief Deletes an AnsClientPeer structure
 *
 * The memory reserved for the structure will be freed.
 * Hence the peer must be no more accessed after calling this function
 * @param peer the peer to destroy
 */
extern void AnsClientPeer_destroy(struct AnsClientPeer* peer);


/*! \brief Disconnect from a peer
 *
 * @param peer the peer to diconnect from
 */
extern void AnsClientPeer_disconnect(struct AnsClientPeer* peer);


/*! \brief Request access to a specific board on a peer
 *
 * @param peer the peer to request board access from
 * @param board_handle the handle of the board to request access to
 * @return pointer to requested board on success, NULL otheriwse
 */
extern struct AnsBoard* AnsClientPeer_requestBoard(struct AnsClientPeer* peer, AiHandle board_handle);


/*! \brief Create new board on a specific peer
 *
 * @param peer the peer instance to create board for
 * @param board_handle the handle of the board to create
 * @return pointer to created board on success, NULL otherwise
 */
extern struct AnsBoard* AnsClientPeer_createBoard(struct AnsClientPeer* peer, AiHandle board_handle);


/*! \brief Release access to a specific board
 *
 * The board might be deleted if no more references are held to it.
 * Hence it must no more being accessed after calling this function
 * @param peer the peer to release board of
 * @param board the board to release
 */
extern void AnsClientPeer_releaseBoard(struct AnsClientPeer* peer, struct AnsBoard* board);


/*! \brief Open command channel for a specific board
 *
 * A board specific connection to the server will be opened and the initial
 * protocol handshake will be performed.
 * @param peer the peer that hosts the board to connect to
 * @param board the board to open a specific connection to
 * @return AnsStatus_OK on success, an error code otherwise.
 */
extern AnsStatus AnsClientPeer_connectBoard(struct AnsClientPeer* peer, struct AnsBoard* board);


/*! \brief Disconnect command channel for a specific board
 *
 * @param peer the peer that hosts the board to disconnect from
 * @param board the board to disconnect from
 */
extern void AnsClientPeer_disconnectBoard(struct AnsClientPeer* peer, struct AnsBoard* board);


/*! \brief Transmit a board specific command
 *
 * This function will block until response is received
 * or \ref ANS_CMD_RESP_TIMEOUT_MS timeout is reached
 * @param peer the peer that hosts the board to transmit command to
 * @param board the board to transmit command to
 * @param command the command frame to transmit
 * @param command_size size of the command frame in bytes
 * @param response_memory the response data will be stored here
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsClientPeer_transmitBoardCommand(struct AnsClientPeer* peer, struct AnsBoard* board,
                                                    AnsCmdFrame* command, AiSize command_size,
                                                    MemChunk* response_memory);


/*! \brief Opens a board event stream
 *
 * This function will create a socket connection to the peer system for receiving events from a specific board. \n
 * The function will return an observer object that can be used to wait for board events
 * @param peer the peer system that hosts the board to register for events
 * @param board the board to register event stream for
 * @param observer The created observer object will be stored here.
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsClientPeer_openBoardEventStream(struct AnsClientPeer* peer, struct AnsBoard* board,
                                                    struct AnsBoardEventObserver** observer);


/*! \brief Close a board event stream
 *
 * This function will close a socket connection for event receiving that has been previously allocated
 * using \ref AnsClientPeer_openBoardEventStream. \n
 * The caller is responsible for destroying the corresponding event observer object using \ref AnsBoardEventObserver_destroy
 * afterwards.
 * @param peer the peer system that hosts the board
 * @param board the board to unregister event stream from
 * @param observer_handle the handle of the \ref  struct AnsBoardEventObserver object to unregister
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsClientPeer_closeBoardEventStream(struct AnsClientPeer* peer, struct AnsBoard* board,
                                                     AiHandle observer_handle);




#endif /* ANS_CLIENTPEER_H_ */
