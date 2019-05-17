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
#include "../include/session_functions.h"
#include "../include/tuple.h"

int main(int argc, char const *argv[]) {
    int port;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    read_server_arguments(argc, argv, &port);

    int sock = start_listening_port(serverptr, &server, port);

    // initialize list to store client info
    List *client_list = initialize_list();

    while (1) {
        // accept connection
        int newsock;
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
            perror(RED "Error while accepting connection" RESET);
            exit(EXIT_FAILURE);
        }
        printf("Client: Port: %d, Address: %s\n", client.sin_port,
               inet_ntoa(client.sin_addr));

        handle_server_connection(newsock, client_list, client);
        // close socket, sock must be closed before it gets re-assigned
        close(newsock);

        print_list(client_list);
    }

    close(sock);
    return 0;
}
