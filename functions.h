#ifndef PREFIXADD_H
#define PREFIXADD_H

int sendMessage(char *str, int clientSocket);
void set(char msg[], char m[]);
void cat(char msg[], char m[]);

void getNextPlayer(int *curPlayerSock, int totalPlayerCount, int *playerTurn, int *clientsocks);
void kickPlayer(int *currentPlayerCount, int *clientsocks, int playerTurn, int *curPlayerSock, int totalPlayerCount);
void endGameProcess(int pCount, int playerWinID, int playerTurn, int *clientsocks);
#endif