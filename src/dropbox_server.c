#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/connection_handlers.h"
#include "../include/defines.h"
#include "../include/list.h"
#include "../include/read_functions.h"
#include "../include/tuple.h"

int main(int argc, char const *argv[]) {
    int port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    read_server_arguments(argc, argv, &port);

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }
    // override fails in bind
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) <
        0) {
        perror(RED "Error in setsockopt(SO_REUSEADDR)" RESET);
    }

    server.sin_family = AF_INET;  // internet domain
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

    // initialize list to store client info
    List *client_list = initialize_list();

    while (1) {
        // accept connection
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror(RED "Error while accepting connection" RESET);
            exit(EXIT_FAILURE);
        }
        printf("Client: Port: %d, Address: %s\n", client.sin_port,
               inet_ntoa(client.sin_addr));
        printf("Accepted connection\n");

        handle_client_connection(newsock, &client_list, client);

        printf("Closing connection.\n");
        // close socket
        // sock must be closed before it gets re-assigned
        close(newsock);

        print_list(client_list);
    }

    close(sock);
    return 0;
}
