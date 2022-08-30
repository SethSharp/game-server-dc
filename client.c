#define __USE_W32_SOCKETS
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "include.h"

#define PORT 80
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    int clientsock;

    char client_buf[BUF_SIZE];
    char server_reply[BUF_SIZE];

    int res;

    // define server address
    if (argc < 2)
    {
        printf("Missing IP address\n");
        exit(1);
    }

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // create socket
    clientsock = socket(AF_INET, SOCK_STREAM, 0);

    if (clientsock < 0)
    {
        perror("Could not create socket");
    }
    printf("Socket created\n");

    // Connect (to remote server)
    res = connect(clientsock, (struct sockaddr *)&server, sizeof(server));
    if (res == -1)
    {
        printf("Connection failed\n");
        exit(1);
    }

    // Keep communicating with server
    while (1)
    {
        memset(client_buf, '\0', sizeof(client_buf));
        memset(server_reply, '\0', sizeof(server_reply));
        printf("> ");
        // read from stdin to client_buf
        scanf("%s", client_buf);

        if (strcmp(client_buf, "quit") == 0)
        {
            close(clientsock);
            exit(0);
        }
        clock_t start, stop;
        start = clock();
        // Send data
        res = send(clientsock, client_buf, BUF_SIZE, 0);
        if (res < 0)
        {
            printf("Sending data to server failed\n");
            exit(1);
        }
        // Recieve data
        res = recv(clientsock, server_reply, BUF_SIZE - 1, 0);
        stop = clock();
        printf("Request: %s\n", client_buf);
        printf("Reply: %s\n", server_reply);
        printf("Time: %6.3fMS\n", (double)(stop - start) / CLOCKS_PER_SEC);
        if (strcmp(client_buf, "bye") == 0) {
            close(clientsock);
            exit(0);
        }
    }

    close(clientsock);
    return 0;
}