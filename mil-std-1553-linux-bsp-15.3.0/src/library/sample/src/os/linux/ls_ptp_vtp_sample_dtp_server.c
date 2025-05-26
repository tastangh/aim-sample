
/* SPDX-FileCopyrightText: 2021-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

#include "AiOs.h"
#include "Api1553.h"
#include "../../ls_ptp_vtp_sample.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

//############################################################//
// Used to break while with CTRL+C
//############################################################//
int server_sockfd, client_sockfd;
int wait_for_signal=0;

void signal_handler(int sig)
{
    wait_for_signal=0;
    shutdown(client_sockfd, SHUT_RDWR );
    shutdown(server_sockfd, SHUT_RDWR );
    (void) signal(SIGINT, SIG_DFL);
}

void LsPtpVtpSample_DTPServer(AiUInt32 ulModHandle)
{
    int client_len;
    int size;
    AiUInt16 data;
    int opt = 1;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    AiUInt32 ulBufHeaderIndex;
    AiUInt32 ulBufHeaderAddr;
    AiUInt16 uwOutIndex;
    AiUInt32 ulOutAddr;
    AiUInt32 ulNewRtAddr;

    struct dtp_command cmd;
    
    printf("Starting DiagnosticTestProcedure server\n");

    wait_for_signal = 1;
    (void) signal (SIGINT, signal_handler);

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("create socket failed");
        return;
    }

    // Forcefully attaching socket to the port 1553
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR,  &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        return;
    }

    server_address.sin_family      = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port        = htons(1553);

    if (bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address))<0)
    {
        perror("bind failed");
        return;
    }

    while(wait_for_signal)
    {
        if (listen(server_sockfd, 3) < 0)
        {
            perror("listen failed");
            return;
        }

        printf("Listening for DTP data on port 1553\n");
        printf("Press CTRL-C to stop the server\n");

        client_len = sizeof(client_address);

        if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t*)&client_len))<0)
        {
            perror("accept failed");
            return;
        }
        else
        {
            printf("Client connected. Waiting for commands\n");
        }

        while(wait_for_signal)
        {
            // Read command
            size = read(client_sockfd, &cmd, sizeof(cmd));

            if(size == 0 )
            {
                printf("Client disconnected\n");
                /* Client disconnected */
                break;
            }

            if(size < 0 )
            {
                printf("Read error\n");
                /* Error */
                break;
            }

            if( size >= sizeof(cmd) )
            {
                /*  call DTP Command */
                LsPtpVtpSample_DtpCommand(ulModHandle, cmd);

                if( cmd.command <= DTP_COMMAND_MAX )
                {
                    /* Send acknowledge */
                    cmd.payload = cmd.command;
                    cmd.command = DTP_COMMAND_ACK;
                    size = write(client_sockfd, &cmd, sizeof(cmd));
                    if( size < 0 )
                    {
                        printf("Socket write error\n");
                        break;
                    }
                }
            }
        }

        close(client_sockfd);
    }
    close(server_sockfd);
}

void LsPtpVtpSample_DTPServerCleanup(AiUInt32 ulModHandle)
{
    close(server_sockfd);
}
