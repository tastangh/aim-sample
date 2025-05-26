/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Board.h
 *
 *  This header file offers definition
 *  for protocol independent AIM boards
    that may be handled by ANS

 */

#ifndef ANS_BOARD_H_
#define ANS_BOARD_H_


#include "ANS_Connection.h"
#include "Ai_types.h"
#include "Ai_container.h"
#include "Ai_mutex.h"




/*! \struct AnsBoard
 *
 * This structure describes AIM boards
 * that can be handled by ANS in a protocol independent way.
 */
struct AnsBoard
{
    AiHandle handle;                                /*!< Handle of the board. Must be unique identifier */
    struct AnsConnection* command_connection;       /*!< Connection that is used for requesting commands from the board*/
    struct ai_mutex* command_connection_lock;       /*!< Lock for command connection */
    struct ai_list_head list;                       /*!< List anchor to add a board to a list */
    struct ai_list_head event_observers;            /*!< List of \ref struct AnsBoardEventStream instances that are notified when board event is published */
    struct ai_mutex* event_observers_lock;          /*!< Lock for synchronizing access to event observers */
    unsigned int reference_count;                   /*!< Reference count that counts number of users of a specific instance */
};


/*! \struct AnsBoardEventObserver
 *
 * Event observers can be registered for specific boards. \n
 * They contain a network stream over which board events can be published
 * to remote peers
 */
struct AnsBoardEventObserver
{
    AiHandle handle;                    /*!< Handle of the observer. May be used as unique identifier */
    struct AnsConnection remote_peer;   /*!< The socket connection used by the observer to connect to a remote peer */
    struct ai_list_head list;           /*!< List anchor for adding an observer to a list */
};



/*! \brief Constructor for a board instance
 *
 * @param board the board instance to initialize
 * @param handle the handle to assign to the board
 */
extern void AnsBoard_init(struct AnsBoard* board, AiHandle handle);


/*! \brief Destructor for a board
 *
 * Note that the memory of the board instance will be free.
 * Hence the board pointer must be no more accessed after calling this function
 * @param board the board to destroy
 */
extern void AnsBoard_destroy(struct AnsBoard* board);


/*! \brief Register a connection for a specific board
 *
 * @param board the board to register connection for
 * @param connection the connection to register
 */
extern void AnsBoard_registerConnection(struct AnsBoard* board, struct AnsConnection* connection);


/*! \brief Unregisters a connection from a specific board
 *
 * @param board the board to remove connection from
 * @param connection the connection to remove
 */
extern void AnsBoard_removeConnection(struct AnsBoard* board, struct AnsConnection* connection);


/*! \brief Lock access to a board's command connection
 *
 * @param board the board to lock
 */
extern void AnsBoard_lock(struct AnsBoard* board);


/*! \brief Unlock access to a board's command connection
 *
 * @param board the board to unlock command connection of
 */
extern void AnsBoard_unlock(struct AnsBoard* board);


/*! \brief Register an event observer
 *
 * Once an event is published for a specific board, it will be sent
 * over the given event stream after this function is called.
 * @param board the board to register observer for
 * @param observer the observer to register
 */
extern void AnsBoard_registerEventObserver(struct AnsBoard* board, struct AnsBoardEventObserver* observer);


/*! \brief Get a specific event observer by its handle from a given board
 *
 * @param board the board to search for the given observer
 * @param observer_handle the handle of the observer to search for
 * @return pointer to the event observer if found, NULL if it is not registered
 */
extern struct AnsBoardEventObserver* AnsBoard_getEventObserver(struct AnsBoard* board, AiHandle observer_handle);


/*! \brief Unregister a specific observer from board's event publishing
 *
 * @param board the board to unregister observer from
 * @param observer the observer to unregister
 */
extern void AnsBoard_unregisterEventObserver(struct AnsBoard* board, struct AnsBoardEventObserver* observer);


/*! \brief Publish board event to all registered observers
 *
 * @param board the board to publish event for
 * @param data byte stream that contains raw event data
 * @param size number of bytes the event data consists of
 */
extern void AnsBoard_publishEvent(struct AnsBoard* board, const char* data, AiSize size);


/*! \brief Constructor function for board event observers
 *
 * @param observer the event observer to initialize
 * @param handle to set for the observer
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsBoardEventObserver_init(struct AnsBoardEventObserver* observer, AiHandle handle);


/*! \brief Wait on given socket until connection from remote peer is established
 *
 * This function will block until a remote connection is initialized
 * on the given socket or the timeout is reached
 * @param observer the observer that awaits connection
 * @param observer_socket the socket to wait for a connection. This must be a listening server socket.
 * @param timeout time in milliseconds to wait for connection
 * @return AnsStatus_OK on success, an error code on failure or if timeout is reached.
 */
extern AnsStatus AnsBoardEventObserver_waitConnect(struct AnsBoardEventObserver* observer, NTSOCKET* observer_socket,
                                                   unsigned long timeout);


/*! \brief Connect event observer to remote peer
 *
 * This function will block until connection is established
 * or the socket layer's connection timeout is reached
 * @param observer the observer to connect
 * @param address the address of the remote peer to connect to
 * @param port the port of the remote peer to connect to
 * @return AnsStatus_OK on success, an error code on failure or if timeout is reached
 */
extern AnsStatus AnsBoardEventObserver_connect(struct AnsBoardEventObserver* observer, const char* address, unsigned int port);


/*! \brief Destructor of board event observer
 *
 * Will close connection to remote peer if any is established. \n
 * The memory occupied by the observer will be freed. \n
 * Hence the pointer to the observer must be no more accessed after calling this function.
 * @param observer the observer to destroy
 */
extern void AnsBoardEventObserver_destroy(struct AnsBoardEventObserver* observer);


/*! \brief Disconnect even observer from remote peer
 *
 * @param observer the observer to disconnect
 */
extern void AnsBoardEventObserver_disconnect(struct AnsBoardEventObserver* observer);


/*! \brief Send event to observer's remote peer
 *
 * @param observer the observer to send event for
 * @param data byte stream of raw event data
 * @param length length of event data in bytes
 * @return AnsStatus_OK on success, an error code otherwise
 */
extern AnsStatus AnsBoardEventObserver_sendEvent(struct AnsBoardEventObserver* observer, const char* data,
                                                 AiSize length);


/*! \brief Wait for event from remote peer
 *
 * This function will block until event from the observer's remote peer is received
 * or if timeout is reached
 * @param observer the observer to wait for event
 * @param data the event data will be stored in this buffer
 * @param length the size of the event data buffer in bytes
 * @param timeout time in milliseconds to wait for event
 * @return AnsStatus_OK if event was received, an error code on failure or if timeout was reached
 */
extern AnsStatus AnsBoardEventObserver_waitEvent(struct AnsBoardEventObserver* observer, char* data,
                                                 AiSize length, unsigned long timeout);

#endif /* ANS_BOARD_H_ */
