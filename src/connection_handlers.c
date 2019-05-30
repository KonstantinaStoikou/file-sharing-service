#include "../include/connection_handlers.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/read_functions.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"
#include "../include/tuple.h"

void handle_server_connection(int sockfd, List *list,
                              struct sockaddr_in client) {
    char msg[BUF_SIZE];
    // read message from client
    read_message_from_socket(sockfd, msg, BUF_SIZE);
    printf("MSG: %s\n", msg);

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
    printf("MSG: %s\n", msg);

    // break message into words
    char *words[3];  // maximum number of words for a message is 2
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
            data->version = 0;
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
        print_pathlist(list);
    }
}

void parse_client_list(char *str, List *list, Circular_buffer *cb) {
    printf("Client list: %s\n", str);
    // message form: CLIENT_LIST N ip1,port1 ip2,port2 ip3,port3 ...
    // break message into words
    int count = 0;
    char *saveptr_str;
    char *word = strtok_r(str, " ", &saveptr_str);
    // split remaining sentence to words which represend tuples
    while (word) {
        // if this is not first or second word (CLIENT_LIST or N)
        if (count != 0 && count != 1) {
            // split (by comma) each tuple to ip address and port
            char *saveptr_word;
            char *token = strtok_r(word, ",", &saveptr_word);
            struct in_addr ip;
            unsigned short port;
            for (int i = 0; i < 2; i++) {
                if (i == 0) {
                    int ip32 = atoi(token);
                    ip = *(struct in_addr *)&ip32;
                } else {
                    port = atoi(token);
                }
                token = strtok_r(NULL, ",", &saveptr_word);
            }
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
                data->version = 0;
                data->ip_address = ip;
                data->port_num = port;
                if (push_back_circ_buf(cb, data) == 1) {
                    printf(RED "Buffer is full, item couldn't be added." RESET);
                }
            }
        }
        count++;
        word = strtok_r(NULL, " ", &saveptr_str);
    }
}

void list_files(Pathlist *list, char *dirname) {
    struct dirent *dp;
    DIR *dir = opendir(dirname);

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }
        char path[PATH_SIZE];
        sprintf(path, "%s/%s", dirname, dp->d_name);
        if (dp->d_type == DT_DIR) {
            list_files(list, path);
        } else {
            add_pathlist_node(list, path);
        }
    }
    closedir(dir);
}