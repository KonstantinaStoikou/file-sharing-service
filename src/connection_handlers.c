#include "../include/connection_handlers.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/read_functions.h"
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
        // send USER_OFF messages to all other clients in the list
        send_useroff_msg(list, tup);
    }
}

void handle_client_connection(int sockfd, List *list,
                              struct sockaddr_in client) {
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
    }
}

void send_logon_msg(int sockfd, int port, struct in_addr client_ip,
                    struct sockaddr_in client) {
    printf("This Client: Port: %d, Address: %d\n", client.sin_port,
           client_ip.s_addr);

    // inform server that this new client has arrived
    char msg[BUF_SIZE];
    sprintf(msg, "LOG_ON %d %d", client_ip.s_addr, client.sin_port);
    if (write(sockfd, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
}

void send_logoff_msg(int sockfd, int port, struct in_addr client_ip,
                     struct sockaddr_in client) {
    // inform server that this new client has arrived
    char msg[BUF_SIZE];
    sprintf(msg, "LOG_OFF %d %d", client_ip.s_addr, client.sin_port);
    if (write(sockfd, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
}

char *send_getclients_msg(int sockfd) {
    // empty message string
    char msg[BUF_SIZE];
    strcpy(msg, "GET_CLIENTS");
    if (write(sockfd, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    char *buf = malloc(CLIENT_LIST_SIZE);
    read_message_from_socket(sockfd, buf, CLIENT_LIST_SIZE);

    return buf;
}

void parse_client_list(char *str, List *list) {
    printf("msg: %s\n", str);
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
        }
        count++;
        word = strtok_r(NULL, " ", &saveptr_str);
    }
}

void send_client_list(List *list, Tuple tup, int sockfd) {
    char response[CLIENT_LIST_SIZE];
    // size of passing list will be minus 1 because this client is not included
    int size = list->size - 1;
    sprintf(response, "CLIENT_LIST %d ", size);

    // construct message to send to client
    // message format: CLIENT_LIST N ip1,port1 ip2,port2 ip3,port3 ...
    List_node *current = list->head;

    while (current != NULL) {
        // add to message only tuples different that this client's
        if (compare_tuples(current->tuple, tup) == 1) {
            char temp[TUPLE_BUF_SIZE];
            sprintf(temp, "%d,%d ", current->tuple.ip_address.s_addr,
                    current->tuple.port_num);
            strcat(response, temp);
        }
        current = current->next;
    }

    // write response to client
    if (write(sockfd, response, CLIENT_LIST_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    // printf("Res: %s\n", response);
}

void send_useron_msg(List *list, Tuple tup) {
    char msg[BUF_SIZE];
    List_node *current = list->head;
    while (current != NULL) {
        // send only to other clients, not the same one
        if (compare_tuples(current->tuple, tup) == 1) {
            struct sockaddr_in server;
            struct sockaddr *serverptr = (struct sockaddr *)&server;
            server.sin_family = AF_INET;  // internet domain
            server.sin_addr.s_addr = current->tuple.ip_address.s_addr;
            server.sin_port = current->tuple.port_num;

            int newsock;
            // create socket
            if ((newsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror(RED "Error while creating socket" RESET);
                exit(EXIT_FAILURE);
            }
            // initiate connection
            if (connect(newsock, serverptr, sizeof(server)) < 0) {
                perror(RED "Error while connecting" RESET);
                exit(EXIT_FAILURE);
            }
            sprintf(msg, "USER_ON %d %d", tup.ip_address.s_addr, tup.port_num);
            if (write(newsock, msg, BUF_SIZE) < 0) {
                perror(RED "Error writing to socket" RESET);
                exit(EXIT_FAILURE);
            }
            close(newsock);
        }
        current = current->next;
    }
}

void send_useroff_msg(List *list, Tuple tup) {
    char msg[BUF_SIZE];
    List_node *current = list->head;
    while (current != NULL) {
        // send only to other clients, not the same one
        if (compare_tuples(current->tuple, tup) == 1) {
            struct sockaddr_in server;
            struct sockaddr *serverptr = (struct sockaddr *)&server;
            server.sin_family = AF_INET;  // internet domain
            server.sin_addr.s_addr = current->tuple.ip_address.s_addr;
            server.sin_port = current->tuple.port_num;

            int newsock;
            // create socket
            if ((newsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror(RED "Error while creating socket" RESET);
                exit(EXIT_FAILURE);
            }
            // initiate connection
            if (connect(newsock, serverptr, sizeof(server)) < 0) {
                perror(RED "Error while connecting" RESET);
                exit(EXIT_FAILURE);
            }
            sprintf(msg, "USER_OFF %d %d", tup.ip_address.s_addr, tup.port_num);
            if (write(newsock, msg, BUF_SIZE) < 0) {
                perror(RED "Error writing to socket" RESET);
                exit(EXIT_FAILURE);
            }
            close(newsock);
        }
        current = current->next;
    }
}