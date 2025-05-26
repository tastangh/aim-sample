/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_ServerPeer.h
 *
 */

#ifndef ANS_SERVERPEER_H_
#define ANS_SERVERPEER_H_


#include "Ai_types.h"
#include "Ai_container.h"
#include "ANS_Connection.h"
#include "Ai_mutex.h"


/*!< \def ANS_SERVER_PEER_ID_UNKNOWN
 * Specifies unknown peer ID
 */
#define ANS_SERVER_PEER_ID_UNKNOWN  ((AnsServerPeerId) -1)



/**
 * Definition of an ANS server peer ID.
 */
typedef AiInt32 AnsServerPeerId;


/*! \struct AnsServerPeer
 *
 * This structure represents a specific peer
 * connected to an ANS server
 */
struct AnsServerPeer
{
    AnsServerPeerId id;                 /*!< Unique ID of the peer */
    struct ai_list_head connections;    /*!< Connections between server and peer */
    struct ai_mutex* connection_lock;   /*!< Lock for connection list */
    struct ai_list_head list;           /*!< List anchor for adding this struct to a list */
    unsigned int reference_count;       /*!< Reference counter for determining lifetime of instance */
};



/*! \brief Initializes a server peer
 *
 * @param peer peer structure to initialize
 * @param id ID of peer
 */
extern void AnsServerPeer_init(struct AnsServerPeer* peer, AnsServerPeerId id);


/*! \brief Destroy server peer instance
 *
 * The memory of the struct will be freed and must no more be accessed
 * after calling this function
 * @param peer the peer instance to destroy
 */
extern void AnsServerPeer_destroy(struct AnsServerPeer* peer);


/*! \brief Register a connection for a specific server peer
 *
 * @param peer the server peer to register connection for
 * @param connection the connection to register
 */
extern void AnsServerPeer_registerConnection(struct AnsServerPeer* peer, struct AnsConnection* connection);


/*! \brief Unregisters a connection from a specific server peer
 *
 * @param peer the peer to remove connection from
 * @param connection the connection to remove
 */
extern void AnsServerPeer_removeConnection(struct AnsServerPeer* peer, struct AnsConnection* connection);


/*! \brief Closes all registered connections of a server peer
 *
 * @param peer the peer to close connections of
 */
extern void AnsServerPeer_closeConnections(struct AnsServerPeer* peer);



#endif /* ANS_SERVERPEER_H_ */
