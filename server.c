#define __USE_W32_SOCKETS
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include "functions.h"


#define BACKLOG 10
#define BUF_SIZE 100


int main(int agrc, char *argv[]) {
    struct sockaddr_in server, client;
    int res;
    int readSize;

    int serversock, clientsock;

    char client_buf[BUF_SIZE];
    char server_reply[BUF_SIZE];

    pid_t cpid;

    // Create TCP socket
    serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock == -1) {
        printf("Creating socket failed\n");
        exit(1);
    }
    printf("Socket successfully created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    // Bind addr socket
    res = bind(serversock, (struct sockaddr *) &server, sizeof(server));
    if (res < 0) {
        printf("Bind failed\n");
        close(serversock);
        exit(1);
    }
    printf("Bind was successfully completed\n");

    // Listen
    res = listen(serversock, BACKLOG);
    if (res != 0) {
        printf("Listen failed\n");
        exit(1);
    }
    printf("Waiting for incoming connections...\n");

    // Game values
    int playerCount = 0, playerTurn = 0;
    int pCount = atoi(argv[3]);
    int clientsocks[pCount];
    int currentPlayerCount = 0;
    // Accept multiple connections from clients
    while (1) {
        // Accept connection from client
        int clientlen = sizeof(client);
        clientsock = accept(serversock, (struct sockaddr *) &client, &clientlen);
        if (clientsock < 0) {
            perror("Accept failed");
            exit(1);
        } else {
            playerCount++;
            printf("Success in connection, player count: %d\n", playerCount);
            clientsocks[currentPlayerCount++] = clientsock;
        }

        if (playerCount == pCount) {
            // Changes game state
            for (int j = 0; j < currentPlayerCount; j++) {
                char str[] = "starting";
                send(clientsocks[j], str, sizeof(str), 0);
            }
            break;
        }
    }

    printf("Game is starting...\n");

    // Now we have all players connected, can run game logic and get input from different users
    int activeInp = 0, gameOver = 0;
    int count = 0;
    const int GOAL = 10;
    int curPlayerSock = clientsocks[playerTurn];
    int badInp = 0, attempts = 0;
    int playerWinID = -1;
    while (1) {
        if (currentPlayerCount == 1) {
            activeInp = 0; gameOver = 1; playerWinID = playerTurn;
        }
        if (!activeInp && !gameOver) {
            char str[] = "TEXT Sum is ";
            char c[10];
            sprintf(c, "%d", count);
            strcat(str, c);
            if (sendMessage(str, curPlayerSock)) {
                kickPlayer(&currentPlayerCount, clientsocks, playerTurn, &curPlayerSock, pCount);
                continue;
            } 
            activeInp = 1;
        } else if (activeInp) {
            if(badInp) {
                if (sendMessage("TEXT Bad Input. Try again:", curPlayerSock)) {
                    kickPlayer(&currentPlayerCount, clientsocks, playerTurn, &curPlayerSock, pCount);
                    continue;
                }
                badInp = 0;
                attempts += 1;
                sleep(1);
                if (attempts == 5) {
                    if (sendMessage("END", curPlayerSock)) {
                        kickPlayer(&currentPlayerCount, clientsocks, playerTurn, &curPlayerSock, pCount);
                        continue;
                    }
                    currentPlayerCount--;
                    clientsocks[playerTurn] = -1;
                    getNextPlayer(&curPlayerSock, pCount, &playerTurn, clientsocks);
                    activeInp = 0;
                    attempts = 0;
                }
            } else {
                if (sendMessage("GO", curPlayerSock)) {
                    kickPlayer(&currentPlayerCount, clientsocks, playerTurn, &curPlayerSock, pCount);
                    continue;
                }

                // get the data
                memset(client_buf, '\0', BUF_SIZE);
                recv(curPlayerSock, client_buf, BUF_SIZE, 0); 
                
                char *token, *rest;
                rest = client_buf;
                token = strtok_r(rest, " ", &rest);
                badInp = 0;
                if (strcmp(token, "MOVE")) {
                    if (!strcmp(token, "QUIT")) {
                        if (sendMessage("END", curPlayerSock)) {
                            kickPlayer(&currentPlayerCount, clientsocks, playerTurn, &curPlayerSock, pCount);
                            continue;
                        }
                        currentPlayerCount--;
                        clientsocks[playerTurn] = -1;
                        getNextPlayer(&curPlayerSock, pCount, &playerTurn, clientsocks);
                        activeInp = 0;
                    } else {
                        badInp = 1;
                    }
                    continue;
                }
                if (strlen(rest) > 0) {
                    int x = atoi(rest);
                    if (x >= 10 || x <= 0) {
                        badInp = 1;
                        continue;
                    } else {
                        attempts = 0;
                        count += x;
                    }
                }
                if (count >= GOAL) {
                    if (count == GOAL) playerWinID = playerTurn+1;
                    gameOver = 1;
                    activeInp = 0;
                } else {
                    activeInp = 0;
                    getNextPlayer(&curPlayerSock, pCount, &playerTurn, clientsocks);
                }
            }
        } else if (gameOver) {
            printf("Gameover\n");
            endGameProcess(pCount, playerWinID, playerTurn, clientsocks);
            break;
        }
    }

    close(serversock);
    return 0;
}
