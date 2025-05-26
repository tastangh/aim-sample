/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Connection.c
 *
 *  This header file contains declarations
 *  used for socket based connections by ANS
 *
 */


#include <stdlib.h>
#include "Ai_Socktl.h"
#include "Ai_Socksv.h"
#include "Ai_Sockll.h"
#include "Ai_Strncpy.h"
#include "ANS_Connection.h"
#include "ANS_Log.h"
#include "ANS_Types.h"



/*! \brief Setup host/peer info for a connection
 *
 * @param connection the connection to set-up info for
 * @return AnsStatus_OK on success an error code otherwise
 */
static AnsStatus AI_INLINE setupConnectionInfo(struct AnsConnection* connection)
{
    NTSOCKET_RET ret;

    ret = NTSOCKET_RET_OK;
    do
    {
        ret = ntsocket_getpeername(&connection->handle, &connection->peer_ip);
        if (NTSOCKET_RET_OK != ret)
        {
            break;
        }

        ret = ntsocket_getsourcename(&connection->handle, &connection->host_ip);
        if (NTSOCKET_RET_OK != ret)
        {
            break;
        }

        ret = ntsocket_getpeerport(&connection->handle, &connection->peer_port);
        if (NTSOCKET_RET_OK != ret)
        {
            break;
        }

        ret = ntsocket_getsourceport(&connection->handle, &connection->host_port);
        if (NTSOCKET_RET_OK != ret)
        {
            break;
        }
    }while(0);


    return ret == NTSOCKET_RET_OK ? AnsStatus_OK : AnsStatus_Error;
}




AnsStatus AnsConnection_init(struct AnsConnection* connection)
{
    ntsockll_clearhandle(&connection->handle);

    return AnsStatus_OK;
}


extern AnsStatus AnsConnection_accept(struct AnsConnection* connection, NTSOCKET* socket, unsigned long timeout)
{
    if(ntsocket_accept(socket, timeout, &connection->handle) != NTSOCKET_RET_OK)
    {
        return AnsStatus_SocketConnectError;
    }

    return setupConnectionInfo(connection);
}


AnsStatus AnsConnection_connect(struct AnsConnection* connection, const char* ip_address, unsigned int port)
{
    NTSOCKET socket;

    if(ntsocket_create(&socket, NTSOCKET_STYLE_TCP) != NTSOCKET_RET_OK)
    {
        return AnsStatus_SocketCreateError;
    }

    if(ntsocket_connect(&socket, port, ip_address, &connection->handle) != NTSOCKET_RET_OK)
    {
        ntsocket_destroy(&socket);
        return AnsStatus_SocketConnectError;
    }

    return setupConnectionInfo(connection);
}


void AnsConnection_close(struct AnsConnection* connection)
{
    /* Close the active socket */
    if ( ntsocket_isconnected(&connection->handle) == NTSOCKET_RET_OK)
    {
        ntsocket_disconnect(&connection->handle);
    }

    memset(connection->host_ip.ipAddress, 0, sizeof(connection->host_ip.ipAddress));
    memset(connection->peer_ip.ipAddress, 0, sizeof(connection->peer_ip.ipAddress));
}


void AnsConnection_destroy(struct AnsConnection* connection)
{
    AnsConnection_close(connection);

    free(connection);
}


AnsStatus AnsConnection_send(struct AnsConnection* connection, const char* data, AiSize len)
{
    NTSOCKET_RET sock_stat;
    int bytes_send;

    sock_stat = ntsocket_write(&connection->handle, data, len, &bytes_send );

    if ( NTSOCKET_RET_OK != sock_stat )
    {
        ANSLogError("Failed to send data on connection %p", connection);
        return AnsStatus_SocketWriteError;
    }

    return AnsStatus_OK;
}


AnsStatus AnsConnection_receive(struct AnsConnection* connection, char* buffer, AiSize len, unsigned long timeout)
{
    NTSOCKET_RET sock_stat;
    size_t bytes_received;

    sock_stat = ntsocket_readBlock(&connection->handle, timeout, buffer, len, &bytes_received);

    switch(sock_stat)
    {
        case NTSOCKET_RET_TIMEOUT:
            return AnsStatus_Timeout;

        case NTSOCKET_RET_OK:
            return AnsStatus_OK;

        case NTSOCKET_RET_CLOSED_BY_PEER:
            return AnsStatus_SocketDisconnected;

        default:
            return AnsStatus_SocketReadError;
    }

    return AnsStatus_OK;

}


