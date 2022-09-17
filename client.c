#define __USE_W32_SOCKETS
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define BUF_SIZE 100

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    int clientsock;

    char server_reply[BUF_SIZE];

    int res, readSize, gameIsRunning = 0;

    // define server address
    if (argc < 3) {
        printf("Missing arguments\n");
        exit(1);
    }

    // server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));

    // create socket
    clientsock = socket(AF_INET, SOCK_STREAM, 0);

    if (clientsock < 0) {
        perror("Could not create socket");
    }

    // Connect (to remote server)
    res = connect(clientsock, (struct sockaddr *)&server, sizeof(server));
    if (res == -1) {
        printf("Connection failed\n");
        exit(1);
    } else {
        printf("Welcome to the game.\n");
        printf("Waiting for more people.\n");
    }

    // Until the game is ready...
    while (1) {
        recv(clientsock, server_reply, BUF_SIZE - 1, 0);
        if (!strcmp(server_reply, "starting")) break;
    }
    
    printf("Starting game now!!\n");
    // Keep communicating with server
    while (1) {
        memset(server_reply, '\0', sizeof(server_reply));
        sleep(0.5);
        send(clientsock, "r", sizeof("r"), 0);
        recv(clientsock, server_reply, BUF_SIZE - 1, 0);
        if (!strcmp(server_reply, "GO")) {
            printf("Enter number: ");
            char client_buf[20];
            fgets(client_buf, BUF_SIZE, stdin);
            client_buf[strcspn(client_buf, "\n")] = 0;
            if (!strcmp(client_buf, "QUIT")) {
                send(clientsock, "QUIT", sizeof("QUIT"), 0);
            } else {
                char newStr[8]="MOVE ";
                strcat(newStr, client_buf);
                // func to send str, but then also check if it was rec
                // client helper function file
                send(clientsock, newStr, sizeof(newStr), 0);
            }    
        } else {
            char *token;
            char *rest = server_reply;
            token = strtok_r(rest, " ", &rest);
            if (!strcmp(token, "TEXT")) {
                printf("%s\n", rest);
            } else if (!strcmp(token, "END")) {
                break;  
            }
        }
    }
    close(clientsock);
    return 0;
}