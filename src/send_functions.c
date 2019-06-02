#include "../include/send_functions.h"
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
#include "../include/session_functions.h"

void send_logon_msg(int sockfd, struct in_addr client_ip,
                    struct sockaddr_in client) {
    // inform server that this new client has arrived
    char msg[BUF_SIZE];
    sprintf(msg, "LOG_ON %d %d", client_ip.s_addr, client.sin_port);
    if (write(sockfd, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
}

void send_logoff_msg(int sockfd) {
    // inform server that this client has exited
    char msg[BUF_SIZE];
    strcpy(msg, "LOG_OFF");
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

void send_file_list(Pathlist *list, int sockfd) {
    char response[FILE_LIST_SIZE];
    sprintf(response, "FILE_LIST %d ", list->size);

    // construct message to send to client
    // message format: FILE_LIST N pathname1,version1 pathname2,version2 ...
    Pathlist_node *current = list->head;

    while (current != NULL) {
        char temp[FILE_BUF_SIZE];
        sprintf(temp, "%s ", current->path);
        strcat(response, temp);

        current = current->next;
    }

    // write response to client
    if (write(sockfd, response, CLIENT_LIST_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Res: %s\n", response);
}

void send_getfilelist_msg(int sock) {
    char msg[BUF_SIZE];
    strcpy(msg, "GET_FILE_LIST");
    if (write(sock, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
}
