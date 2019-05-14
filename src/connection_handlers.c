#include "../include/connection_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"

void handle_client_connection(int sockfd) {
    char buf[BUF_SIZE];
    // read message from client
    if (read(sockfd, buf, BUF_SIZE) <= 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Client: %s, size %ld\n", buf, strlen(buf));
}