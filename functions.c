#define __USE_W32_SOCKETS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <unistd.h>
#define BUF_SIZE 100

int sendMessage(char *str, int clientSocket) {
    char client_buf[BUF_SIZE];
    int readSize = recv(clientSocket, client_buf, BUF_SIZE, 0);
    int res = send(clientSocket, str, 50, 0);
    return (readSize < 0 || res < 0);
}

void set(char msg[], char m[]) {
    strcpy(msg, m);
}

void cat(char msg[], char m[]) {
    strcat(msg, m);
}

void getNextPlayer(int *curPlayerSock, int totalPlayerCount, int *playerTurn, int *clientsocks)
{
    while (1) {
        *playerTurn+=1;

        if (*playerTurn >= (totalPlayerCount)) *playerTurn = 0;

        *curPlayerSock = clientsocks[*playerTurn];

        if (*curPlayerSock == -1) continue;
        else break;
    }
}

void kickPlayer(int *currentPlayerCount, int *clientsocks, int playerTurn, int *curPlayerSock, int totalPlayerCount) {
    *currentPlayerCount -= 1;
    clientsocks[playerTurn] = -1;
    getNextPlayer(&(*curPlayerSock), totalPlayerCount, &playerTurn, clientsocks);
}

void endGameProcess(int pCount, int playerWinID, int playerTurn, int *clientsocks) {
    for (int i = 0; i < pCount; i++) {
        char endGameMsg[20];
        if (playerWinID == -1) {
            strcpy(endGameMsg, "TEXT No one has won");
        } else {
            if (i == playerTurn) {
                strcpy(endGameMsg, "TEXT You won!");
            } else {
                strcpy(endGameMsg, "TEXT You lost!");
            }
        }
        sendMessage(endGameMsg, clientsocks[i]);
        sendMessage("END", clientsocks[i]);
    }
}