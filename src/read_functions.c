#include "../include/read_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"

void read_server_arguments(int argc, char const *argv[], int *port_num) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            *port_num = atoi(argv[i + 1]);
        }
    }
}

void read_client_arguments(int argc, char const *argv[], char **dirname,
                           int *port_num, int *worker_threads_num, int *bufsize,
                           int *server_port, char **server_ip) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            *dirname = malloc(strlen(argv[i + 1]) + 1);
            strcpy(*dirname, argv[i + 1]);
        } else if (strcmp(argv[i], "-p") == 0) {
            *port_num = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-w") == 0) {
            *worker_threads_num = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-b") == 0) {
            *bufsize = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-sp") == 0) {
            *server_port = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-sip") == 0) {
            *server_ip = malloc(strlen(argv[i + 1]) + 1);
            strcpy(*server_ip, argv[i + 1]);
        }
    }
}

void read_message_from_socket(int sockfd, char *msg, int size) {
    // read message from socket
    if (read(sockfd, msg, size) < 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    // remove '\n' if it exists inside the message
    char *pos;
    if ((pos = strchr(msg, '\n')) != NULL) {
        *pos = '\0';
    }
}