#include "../include/connection_handlers.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/dir_functions.h"
#include "../include/read_functions.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"

void handle_server_connection(int sockfd, List *list,
                              struct sockaddr_in client) {
    char msg[BUF_SIZE];
    // read message from client
    read_message_from_socket(sockfd, msg, BUF_SIZE);

    // break message into words
    char *words[3];  // maximum number of words for a message is 3
    int count = 0;
    char *word = strtok(msg, " ");  // split message by spaces
    while (word) {
        words[count] = word;
        count++;
        word = strtok(NULL, " ");
    }

    if (strcmp(words[0], "LOG_ON") == 0) {
        struct in_addr ip;
        int ip32 = atoi(words[1]);
        ip = *(struct in_addr *)&ip32;
        unsigned short port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }

        // empty message string
        memset(msg, 0, strlen(msg));
        // create child to receive message GET_CLIENTS from client and send the
        // list
        switch (fork()) {
            case -1:
                perror(RED "Error forking child" RESET);
                break;
            case 0:
                // read message from client (client will ask for client list)
                read_message_from_socket(sockfd, msg, BUF_SIZE);
                if (strcmp(msg, "GET_CLIENTS") == 0) {
                    send_client_list(list, tup, sockfd);
                }
                exit(EXIT_SUCCESS);
        }
        // parent will send USER_ON messages to all other clients in the list
        send_useron_msg(list, tup);

    } else if (strcmp(words[0], "LOG_OFF") == 0) {
        Tuple tup;
        tup.ip_address = client.sin_addr;
        tup.port_num = client.sin_port;
        if (delete_list_node(list, tup) == 1) {
            printf(RED "ERROR_IP_PORT_NOT_FOUND_IN_LIST\n" RESET);
        } else {
            // send USER_OFF messages to all other clients in the list
            send_useroff_msg(list, tup);
        }
    }
}

void handle_client_connection(int sockfd, List *list, struct sockaddr_in client,
                              char *dirname, Circular_buffer *cb) {
    char msg[BUF_SIZE];
    // read message from client
    read_message_from_socket(sockfd, msg, BUF_SIZE);

    // break message into words
    char *words[4];  // maximum number of words for a message is 3
    int count = 0;
    char *word = strtok(msg, " ");  // split message by spaces
    while (word) {
        words[count] = word;
        count++;
        word = strtok(NULL, " ");
    }

    if (strcmp(words[0], "USER_ON") == 0) {
        struct in_addr ip;
        int ip32 = atoi(words[1]);
        ip = *(struct in_addr *)&ip32;
        unsigned short port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }
        // add it as item to circular buffer
        else {
            Cb_data *data = malloc(sizeof(Cb_data));
            data->pathname[0] = '\0';
            strcpy(data->version, "-1");
            data->ip_address = ip;
            data->port_num = port;
            if (push_back_circ_buf(cb, data) == 1) {
                printf(RED "Buffer is full, item couldn't be added." RESET);
            }
        }
    } else if (strcmp(words[0], "USER_OFF") == 0) {
        struct in_addr ip;
        int ip32 = atoi(words[1]);
        ip = *(struct in_addr *)&ip32;
        unsigned short port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (delete_list_node(list, tup) == 1) {
            printf(RED "Tuple doesn't exist.\n" RESET);
        }
    } else if (strcmp(words[0], "GET_FILE_LIST") == 0) {
        Pathlist *list = initialize_pathlist();
        list_files(list, dirname);
        send_file_list(list, sockfd);
    } else if (strcmp(words[0], "GET_FILE") == 0) {
        char filepath[PATH_SIZE];
        strcpy(filepath, words[1]);
        char version[MD5_SIZE];
        strcpy(version, words[2]);
        // check if file exists
        if (access(filepath, F_OK) == -1) {
            if (write(sockfd, "FILE_NOT_FOUND", ERROR_MSG_SIZE) < 0) {
                perror(RED "Error writing to socket" RESET);
                exit(EXIT_FAILURE);
            }
        }
        // check if version is the same with already existing file
        char old_md5[MD5_SIZE];
        strcpy(old_md5, get_md5_hash(filepath));
        if (strcmp(old_md5, version) == 0) {
            printf("filepath %s up to date\n", filepath);
            if (write(sockfd, "FILE_UP_TO_DATE", ERROR_MSG_SIZE) < 0) {
                perror(RED "Error writing to socket" RESET);
                exit(EXIT_FAILURE);
            }
        } else {
            send_file_msg(sockfd, filepath, version);
        }
    }
}
