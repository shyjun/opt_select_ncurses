/*
 * opt_select_ncurses - Ncurses-based option selection utility
 *
 * Copyright (c) 2025, Shyju N
 * Email: n.shyju@gmail.com
 *
 * Licensed under the BSD 3-Clause License.
 * See the LICENSE file in the project root for full license information.
 */

#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// command to recv dbg data: `nc -l -k -u -p <port>`

#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

int sockfd;
struct sockaddr_in server_addr;
char print_buff[500] = { 0 };

int udp_dbg_enable = 0;
int udp_server_port;

void
set_udp_port(int port)
{
    udp_dbg_enable = 1;
    udp_server_port = port;
}

void
udp_dbg(const char *fmt, ...)
{
    if (udp_dbg_enable == 0)
        return;

    static int inited = 0;
    if (inited == 0) {
        // Create UDP socket
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }

        // Set up the server address structure
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(udp_server_port);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        inited = 1;
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(print_buff, sizeof(print_buff), fmt, args);
    va_end(args);

    // Send message to server
    if (sendto(sockfd, print_buff, strlen(print_buff), 0,
            (struct sockaddr *)&server_addr, sizeof(server_addr))
        < 0) {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}
