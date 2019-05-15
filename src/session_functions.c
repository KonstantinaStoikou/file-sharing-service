#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
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
}