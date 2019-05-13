#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/read_functions.h"

int main(int argc, char const *argv[]) {
    int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    // struct hostent *rem;

    read_server_arguments(argc, argv, &port);

    // reap dead children asynchronously
    // signal(SIGCHLD, sigchld_handler);

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
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
        /* Find client's address */
        // if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,
        // sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
        // herror("gethostbyaddr"); exit(1);}
        // printf("Accepted connection from %s\n", rem->h_name);
        printf("Accepted connection\n");
        // create child for serving client
        pid_t pid = fork();
        // if child process
        if (pid == 0) {
            close(sock);
            char buf[1];
            // receive 1 char
            while (read(newsock, buf, 1) > 0) {
                putchar(buf[0]); /* Print received char */
                /* Capitalize character */
                buf[0] = toupper(buf[0]);
                /* Reply */
                if (write(newsock, buf, 1) < 0) {
                    perror(RED "Error while writing" RESET);
                    exit(EXIT_FAILURE);
                }
                printf("Closing connection.\n");
                // close socket
                close(newsock);
                exit(EXIT_SUCCESS);
            }
        }
        // if error in fork
        else if (pid == -1) {
            perror(RED "Error while forking" RESET);
            exit(EXIT_FAILURE);
        }
        // parent closes socket to client
        // must be closed before it gets re-assigned
        close(newsock);

        return 0;
    }
}
