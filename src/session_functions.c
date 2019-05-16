#include "../include/session_functions.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/defines.h"

int start_new_session(struct sockaddr *serverptr, struct sockaddr_in server) {
    int sock;
    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }

    // initiate connection
    if (connect(sock, serverptr, sizeof(server)) < 0) {
        perror(RED "Error while connecting" RESET);
        exit(EXIT_FAILURE);
    }
    return sock;
}

int start_listening_port(struct sockaddr *serverptr, struct sockaddr_in *server,
                         int port) {
    int listen_sock;

    // create socket that this server will listen to
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }
    // override fails in bind
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1},
                   sizeof(int)) < 0) {
        perror(RED "Error in setsockopt(SO_REUSEADDR)" RESET);
    }

    server->sin_family = AF_INET;  // internet domain
    server->sin_addr.s_addr = htonl(INADDR_ANY);
    server->sin_port = htons(port);

    // bind socket to address
    if (bind(listen_sock, serverptr, sizeof(*server)) < 0) {
        perror(RED "Error while binding socket to address" RESET);
        exit(EXIT_FAILURE);
    }
    // listen for connections
    if (listen(listen_sock, 5) < 0) {
        perror(RED "Error while listening for connections" RESET);
        exit(EXIT_FAILURE);
    }

    return listen_sock;
}

struct in_addr get_client_info() {
    struct hostent *rem_client;

    // retrieve this client's hostname
    char hostbuf[BUF_SIZE];
    if (gethostname(hostbuf, sizeof(hostbuf)) < 0) {
        perror(RED "Error in gethostname" RESET);
        exit(EXIT_FAILURE);
    }

    // retrieve this client's host information
    if ((rem_client = gethostbyname(hostbuf)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }
    struct in_addr client_addr;
    client_addr.s_addr = htonl(*rem_client->h_addr);
    return client_addr;
}