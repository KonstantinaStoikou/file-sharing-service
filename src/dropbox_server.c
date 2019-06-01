#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/connection_handlers.h"
#include "../include/defines.h"
#include "../include/list.h"
#include "../include/read_functions.h"
#include "../include/session_functions.h"

int main(int argc, char const *argv[]) {
    int port;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;

    read_server_arguments(argc, argv, &port);

    // get clients ip address
    struct in_addr server_ip = get_client_info();

    server.sin_family = AF_INET;  // internet domain
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    printf("This Server: Port: %d, Address: %s\n", server.sin_port,
           inet_ntoa(server_ip));

    int sock = start_listening_port(serverptr, server);

    // set of socket descriptors
    fd_set active_fd_set, read_fd_set;
    // initialize the set of active sockets
    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);

    // initialize list to store client info
    List *client_list = initialize_list();

    while (1) {
        // block until input arrives on one or more active sockets
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror(RED "Error in select" RED);
            exit(EXIT_FAILURE);
        }

        // service all the sockets with input pending
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                // connection request on original socket
                if (i == sock) {
                    // accept connection
                    int newsock;
                    if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {
                        perror(RED "Error while accepting connection" RESET);
                        exit(EXIT_FAILURE);
                    }

                    // if server and client are running on the same device,
                    // convert private ip to public ip
                    if (strcmp(inet_ntoa(client.sin_addr), "127.0.0.1") == 0) {
                        memcpy(&client.sin_addr, &server_ip, sizeof(server_ip));
                    }

                    printf("Client: Port: %d, Address: %s\n", client.sin_port,
                           inet_ntoa(client.sin_addr));
                    FD_SET(newsock, &active_fd_set);
                }
                // data arriving on an already connected
                else {
                    handle_server_connection(i, client_list, client);
                    print_list(client_list);
                    // close socket and clear it to reuse it
                    close(i);
                    FD_CLR(i, &active_fd_set);
                }
            }
        }
    }
    return 0;
}
