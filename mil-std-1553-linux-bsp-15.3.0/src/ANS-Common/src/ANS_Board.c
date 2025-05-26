/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Board.c
 *
 */


#include <stdlib.h>
#include "ANS_Board.h"




void AnsBoard_init(struct AnsBoard* board, AiHandle handle)
{
    board->handle = handle;
    board->command_connection = NULL;
    board->command_connection_lock = ai_mutex_create();
    AI_LIST_INIT(board->event_observers);
    board->event_observers_lock = ai_mutex_create();
    board->reference_count = 1;
}


void AnsBoard_destroy(struct AnsBoard* board)
{
    struct AnsBoardEventObserver* current = NULL;
    struct AnsBoardEventObserver* next = NULL;

    ai_mutex_lock(board->command_connection_lock);

    if(board->command_connection)
    {
        AnsConnection_destroy(board->command_connection);
        board->command_connection = NULL;
    }

    ai_mutex_release(board->command_connection_lock);
    ai_mutex_free(board->command_connection_lock);

    /* Now destroy all registered event observers */
    ai_mutex_lock(board->event_observers_lock);

    ai_list_for_each_entry_safe(current, next, &board->event_observers, struct AnsBoardEventObserver, list)
    {
        ai_list_del(&current->list);
        AnsBoardEventObserver_destroy(current);
    }

    ai_mutex_release(board->event_observers_lock);
    ai_mutex_free(board->event_observers_lock);

    free(board);
}


void AnsBoard_registerConnection(struct AnsBoard* board, struct AnsConnection* connection)
{
    board->command_connection = connection;
}


void AnsBoard_removeConnection(struct AnsBoard* board, struct AnsConnection* connection)
{
    board->command_connection = NULL;
}


void AnsBoard_lock(struct AnsBoard* board)
{
    ai_mutex_lock(board->command_connection_lock);
}


void AnsBoard_unlock(struct AnsBoard* board)
{
    ai_mutex_release(board->command_connection_lock);
}


void AnsBoard_registerEventObserver(struct AnsBoard* board, struct AnsBoardEventObserver* observer)
{
    ai_mutex_lock(board->event_observers_lock);

    ai_list_add_tail(&observer->list, &board->event_observers);

    ai_mutex_release(board->event_observers_lock);
}


struct AnsBoardEventObserver* AnsBoard_getEventObserver(struct AnsBoard* board, AiHandle observer_handle)
{
    struct AnsBoardEventObserver* current = NULL;

    ai_mutex_lock(board->event_observers_lock);

    ai_list_for_each_entry(current, &board->event_observers, struct AnsBoardEventObserver, list)
    {
        if(current->handle == observer_handle)
        {
            ai_mutex_release(board->event_observers_lock);
            return current;
        }
    }

    ai_mutex_release(board->event_observers_lock);

    return NULL;
}


void AnsBoard_unregisterEventObserver(struct AnsBoard* board, struct AnsBoardEventObserver* observer)
{
    ai_mutex_lock(board->event_observers_lock);

    ai_list_del(&observer->list);

    ai_mutex_release(board->event_observers_lock);
}


void AnsBoard_publishEvent(struct AnsBoard* board, const char* data, AiSize size)
{
    struct AnsBoardEventObserver* current = NULL;

    ai_mutex_lock(board->event_observers_lock);

    /* Send event on all registered event observers */
    ai_list_for_each_entry(current, &board->event_observers, struct AnsBoardEventObserver, list)
    {
        AnsBoardEventObserver_sendEvent(current, data, size);
    }

    ai_mutex_release(board->event_observers_lock);
}


AnsStatus AnsBoardEventObserver_init(struct AnsBoardEventObserver* observer, AiHandle handle)
{
    observer->handle = handle;

    return AnsConnection_init(&observer->remote_peer);
}


AnsStatus AnsBoardEventObserver_waitConnect(struct AnsBoardEventObserver* observer, NTSOCKET* observer_socket,
                                            unsigned long timeout)
{
    return AnsConnection_accept(&observer->remote_peer, observer_socket, timeout);
}


AnsStatus AnsBoardEventObserver_connect(struct AnsBoardEventObserver* observer, const char* address, unsigned int port)
{
    return AnsConnection_connect(&observer->remote_peer, address, port);
}


void AnsBoardEventObserver_disconnect(struct AnsBoardEventObserver* observer)
{
    AnsConnection_close(&observer->remote_peer);
}


void AnsBoardEventObserver_destroy(struct AnsBoardEventObserver* observer)
{
    AnsBoardEventObserver_disconnect(observer);

    free(observer);
}


AnsStatus AnsBoardEventObserver_sendEvent(struct AnsBoardEventObserver* observer, const char* data, AiSize length)
{
    return AnsConnection_send(&observer->remote_peer, data, length);
}


AnsStatus AnsBoardEventObserver_waitEvent(struct AnsBoardEventObserver* observer, char* data,
                                          AiSize length, unsigned long timeout)
{
    return AnsConnection_receive(&observer->remote_peer, data, length, timeout);
}

