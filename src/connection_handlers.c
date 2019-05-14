#include "../include/connection_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/tuple.h"

void handle_client_connection(int sockfd, List **list) {
    char msg[BUF_SIZE];
    // read message from client
    if (read(sockfd, msg, BUF_SIZE) <= 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    // remove '\n' if it exists inside the message
    char *pos;
    if ((pos = strchr(msg, '\n')) != NULL) {
        *pos = '\0';
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

    if (strcmp(words[0], "LOG_ON") == 0) {
        printf("log\n");
        int ip = atoi(words[1]);
        int port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }
    } else if (strcmp(words[0], "GET_CLIENTS") == 0) {
        printf("get clients\n");
    } else if (strcmp(words[0], "LOG_OFF") == 0) {
        printf("log off\n");
    }

    printf("print list\n");
    print_list(*list);
}