#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/list.h"
#include "../include/read_functions.h"
#include "../include/signal_handlers.h"
#include "../include/tuple.h"

int main(int argc, char const *argv[]) {
    int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    read_server_arguments(argc, argv, &port);

    // reap dead children asynchronously
    signal(SIGCHLD, sigchld_handler);
    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }
    // override fails in bind
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) <
        0) {
        perror(RED "setsockopt(SO_REUSEADDR) failed" RESET);
    }
    // internet domain
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    // bind socket to address
    if (bind(sock, serverptr, sizeof(server)) < 0) {
        perror(RED "Error while binding socket to address" RESET);
        exit(EXIT_FAILURE);
    }
    // listen for connections
    if (listen(sock, 5) < 0) {
        perror(RED "Error while listening for connections" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Listening for connections to port %d\n", port);

    while (1) {
        // accept connection
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror(RED "Error while accepting connection" RESET);
            exit(EXIT_FAILURE);
        }
        printf("Accepted connection\n");
        // create child for serving client
        pid_t pid = fork();
        // if child process
        if (pid == 0) {
            close(sock);
            char buf[BUF_SIZE];
            // read message from client
            if (read(newsock, buf, BUF_SIZE) <= 0) {
                perror(RED "Error reading from socket" RESET);
                exit(EXIT_FAILURE);
            }
            printf("Client: %s\n", buf);
            // close socket
            close(newsock);
            exit(EXIT_SUCCESS);
        }
        // if error in fork
        else if (pid == -1) {
            perror(RED "Error while forking" RESET);
            exit(EXIT_FAILURE);
        }
        // parent closes socket to client
        // must be closed before it gets re-assigned
        close(newsock);
    }
    return 0;
}
