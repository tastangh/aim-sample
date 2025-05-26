/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Client.h
 *
 * This file contains declarations for functions for
 * the management of ClientIDs
 *
 *
 ******************************************************************************/
#ifndef ANS_CLIENT_H
#define ANS_CLIENT_H

/* Includes *******************************************************************/

#include "Ai_cdef.h"
#include "ANS_Types.h"
#include "Ai_Socket.h"
#include "ANS_CmdFrame.h"
#include "ANS_AdminCommands.h"
#include "Ai_container.h"
#include "ANS_ClientPeer.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Constants ******************************************************************/
               

/* Macros *********************************************************************/


/* Type definitions ***********************************************************/



/*! \struct AnsClient
 *
 * This structure is used by ANS client applications
 * to keep track of all connected ANS servers.
 */
struct AnsClient
{
    struct ai_list_head peers;  /*!< List of \ref AnsClientPeer objects for all connected servers */
    struct ai_mutex* peer_lock; /*!< Lock the list of connected peers */
};


/*!< Global variable that can be used by ANS client applications
 * to implement an ANS client layer
 */
extern struct AnsClient g_AnsClient;


/* Prototypes *****************************************************************/


/*! \brief Initializes an AnsClient structure
 *
 * @param client the client structure to initialize
 * @return AnsStatus_OK on success. An error code otherwise
 */
extern AnsStatus AnsClient_init(struct AnsClient* client);


/*! \brief Request a specific client peer
 *
 * Search for a connected peer with given URL and port
 * The library will increase the reference count of the AnsClientPeer struct on success.
 * Hence \ref AnsClient_releasePeer has to be called if peer is no more needed by caller
 * @param client the AnsClient to request the peer from
 * @param peer_address the address of the peer. This may be either an IP address or a server name that will be resolved by DNS
 * @param port the port of the peer
 * @return Reference to requested peer on success, NULL otherwise
 */
extern struct AnsClientPeer* AnsClient_requestPeer(struct AnsClient* client, const char* peer_address, unsigned short port);


/*! \brief Create a new Client peer structure
 *
 * The caller will hold a reference to the peer.
 * Hence it must call \ref AnsClient_releasePeer when peer is no more needed
 * @param client the client to create a peer for
 * @param peer_address the address of the peer. This may be either an IP address or a server name that will be resolved by DNS
 * @param port the port of the peer
 * @return reference to created peer on success, NULL on failure
 */
extern struct AnsClientPeer* AnsClient_createPeer(struct AnsClient* client, const char* peer_address, unsigned short port);


/*! \brief Release a specific peer from usage
 *
 * The peer will be deleted if no more references are hold to a peer.
 * Hence the peer must be no more accessed after calling this function
 * @param client the client to release peer from
 * @param peer the peer to release
 */
extern void AnsClient_releasePeer(struct AnsClient* client, struct AnsClientPeer* peer);


/*! \brief Extract server address and port from a URL
 *
 * A server URL can consist of the server name or IP extended
 * by a port number separated with a colon e.g. 192.168.0.1:5000. \n
 * This function will split up the URL into these address and port components.
 * @param url The URL to split up
 * @param peer_address the extracted peer address will be stored here
 * @param addr_len size of _peer_address_ in bytes
 * @param port the extracted port number will be stored here. If URL contains no explicit port specification, it will be set to 0
 * @return AnsStatus_OK on success, error code on failure
 */
extern AnsStatus AnsClient_translateServerURL(const char* url, char* peer_address, size_t addr_len,
                                              unsigned int* port);


extern AnsStatus AnsClient_discover(int port);
extern AnsStatus AnsClient_discover_thread_start(AnsDiscoverThreadConfig * config);
extern AnsStatus AnsClient_discover_thread_stop(AnsDiscoverThreadConfig * config);


#ifdef __cplusplus
}
#endif

#endif  /* ANS_CLIENT_H */
