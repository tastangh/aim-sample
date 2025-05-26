/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_ServerPeer.c
 *
 */


#include "ANS_ServerPeer.h"
#include "ANS_Log.h"
#include <stdlib.h>



void AnsServerPeer_init(struct AnsServerPeer* peer, AnsServerPeerId id)
{
    peer->id = id;
    AI_LIST_INIT(peer->connections);
    peer->reference_count = 1;

    peer->connection_lock = ai_mutex_create();
}


extern void AnsServerPeer_destroy(struct AnsServerPeer* peer)
{
    ANSLogDebug("Destroying client with id %d", peer->id);

    ai_mutex_free(peer->connection_lock);

    free(peer);
}


void AnsServerPeer_registerConnection(struct AnsServerPeer* peer, struct AnsConnection* connection)
{
    ai_mutex_lock(peer->connection_lock);

    ai_list_add_tail(&connection->list, &peer->connections);

    ai_mutex_release(peer->connection_lock);
}


void AnsServerPeer_removeConnection(struct AnsServerPeer* peer, struct AnsConnection* connection)
{
    ANSLogDebug("Removing connection %p", connection);

    ai_mutex_lock(peer->connection_lock);

    ai_list_del(&connection->list);

    ai_mutex_release(peer->connection_lock);
}


void AnsServerPeer_closeConnections(struct AnsServerPeer* peer)
{
    struct AnsConnection* current;

    ai_mutex_lock(peer->connection_lock);

    ai_list_for_each_entry(current, &peer->connections, struct AnsConnection, list)
    {
        AnsConnection_close(current);
    }

    ai_mutex_release(peer->connection_lock);
}


