/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Connection.h
 *
 *  This header file contains declarations
 *  used for socket based connections by ANS
 *
 */

#ifndef ANS_CONNECTION_H_
#define ANS_CONNECTION_H_


#include "Ai_Socket.h"
#include "Ai_container.h"
#include "Ai_mutex.h"
#include "ANS_Types.h"




/*! \struct AnsConnection
 *
 * This structure describe one specific TCP based communication
 * channel between a client and a server.
 */
struct AnsConnection
{
    NTSOCKET_HANDLE handle;       /*!< Socket to use for communication */
    NTSOCKET_IPADDRESS host_ip;   /*!< IP of owner of the connection */
    NTSOCKET_IPADDRESS peer_ip;   /*!< IP of peer the connection is attached to */
    NTSOCKET_PORT host_port;      /*!< Port of owner of the connection */
    NTSOCKET_PORT peer_port;      /*!< Port of peer the connection is attached to */

    struct ai_list_head list;     /*!< List anchor for adding the connection to a list */
};


/*! \brief Constructor for ANS connection
 *
 * @param connection connection instance to initialize
 * @return 0 on success, error code on failure
 */
extern AnsStatus AnsConnection_init(struct AnsConnection* connection);


/*! \brief Wait for connection being initialized by remote peer
 *
 * @param connection the connection to wait
 * @param socket socket to wait for being connected. Must be a listening server socket
 * @param timeout the timeout to wait for connection in milliseconds
 * @return AnsStatus_OK if connection has been established, an error code on failure or timeout
 */
extern AnsStatus AnsConnection_accept(struct AnsConnection* connection, NTSOCKET* socket, unsigned long timeout);


/*! \brief Establish connection to specific peer
 *
 * This function will block until connection is established or timeout is reached
 * @param connection the connection to establish
 * @param address the address to connect to
 * @param port the port to connect to
 * @return AnsStatus_OK if connection has been established, an error code on failure or timeout
 */
extern AnsStatus AnsConnection_connect(struct AnsConnection* connection, const char* address, unsigned int port);


/*! \brief Close a connection
 *
 * Will close the corresponding socket
 * @param connection the connection to close
 */
extern void AnsConnection_close(struct AnsConnection* connection);


/*! \brief Destructor for ANS connection
 *
 *  The memory of the connection will be freed.
 *  Hence the connection pointer must be accessed no more after calling this function.
 * @param connection the connection to destroy
 */
extern void AnsConnection_destroy(struct AnsConnection* connection);


/*! \brief Send data over connection
 *
 * @param connection connection to send data on
 * @param data buffer that contains data to send
 * @param len number of bytes to send from buffer
 * @return AnsStatus_OK on success
 */
extern AnsStatus AnsConnection_send(struct AnsConnection* connection, const char* data, AiSize len);


/*! \brief Receive data on connection
 *
 *  Will block until either timeout is reached or \ref len bytes are received
 * @param connection connection to receive on
 * @param buffer buffer to put received data into
 * @param len number of bytes to receive
 * @param timeout in milliseconds
 * @return AnsStatus_OK on success
 */
extern AnsStatus AnsConnection_receive(struct AnsConnection* connection, char* buffer, AiSize len, unsigned long timeout);






#endif /* ANS_CONNECTION_H_ */
