#include "../include/connection_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"

void handle_client_connection(int sockfd) {
    char msg[BUF_SIZE];
    // read message from client
    if (read(sockfd, msg, BUF_SIZE) <= 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Client: %s, size %ld\n", msg, strlen(msg));

    // break prompt into words
    char *words[3];  // maximum number of words for a message is 3
    int count = 0;
    char *word = strtok(msg, " ");  // split prompt by spaces
    while (word) {
        words[count] = word;
        count++;
        word = strtok(NULL, " ");
    }

    for (int j = 0; j < 3; j++) {
        printf("word %d: %s\n", j, words[j]);
    }
}