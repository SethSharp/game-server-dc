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


#define PORT 80
#define BACKLOG 10
#define BUF_SIZE 1024
#define MAXLINE 30

#pragma comment(lib, "user32.lib")

// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

// Returns host information corresponding to host name
void checkHostEntry(struct hostent *hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}

// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}

// https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getversion
char* getOsType() {
    #if defined(_WIN32) || defined(_WIN64)
        return("Windows");
    #elif defined(__linux)
        return("Linux");
    #elif defined(__APPLE__)
        return("MacOs");
    #else
        return("unkonwn");
    #endif
}

char* getHostData() {
    char *hostbuffer;
    struct hostent *host_entry;
    int hostname;

    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);

    // // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);

    return hostbuffer;
}

int main() {
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
    server.sin_port = htons(PORT);

    // Bind addr socket
    res = bind(serversock, (struct sockaddr *) &server, sizeof(server));
    if (res < 0) {
        printf("Bind failed\n");
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

    // Accept multiple connections from clients
    while (1) {

        // Accept connection from client
        int clientlen = sizeof(client);
        clientsock = accept(serversock, (struct sockaddr *) &client, &clientlen);
        if (clientsock < 0) {
            perror("Accept failed");
            exit(1);
        } else {
            printf("User connected\n");
        }

        // create child process
        cpid = fork();
        if (cpid < 0) {
            perror("Fork failed\n");
            exit(1);
        } else if (cpid == 0) { // chil d process=
            while (1) {
                memset(client_buf, '\0', BUF_SIZE);

                // Recieve data
                readSize = recv(clientsock, client_buf, BUF_SIZE, 0);

                if (readSize < 0) {
                    printf("Recv failed\n");
                    exit(1);
                }

                printf("res: %s\n", client_buf);

                if (strcmp(client_buf, "time") == 0) {
                    char timestr[100];
                    time_t ticks = time(NULL);
                    strcpy(timestr, ctime(&ticks));
                    timestr[strcspn(timestr, "\n")] = 0;
                    send(clientsock, timestr, sizeof(timestr), 0);
                } else if (strcmp(client_buf, "host") == 0) {
                    char *word = getHostData();
                    send(clientsock, word, sizeof(word), 0);
                } else if (strcmp(client_buf, "type") == 0) {
                    char *str = getOsType();
                    send(clientsock, str, sizeof(str), 0);
                } else if (strcmp(client_buf, "bye") == 0) {
                    char str[] = "Bye from server";
                    send(clientsock, str, sizeof(str), 0);
                    close(clientsock);
                    close(serversock);
                    exit(0);
                }
            }
        }
    }

    close(clientsock);
    close(serversock);
    return 0;
}
