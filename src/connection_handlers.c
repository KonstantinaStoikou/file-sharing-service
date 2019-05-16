#include "../include/connection_handlers.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/tuple.h"

void handle_client_connection(int sockfd, List **list,
                              struct sockaddr_in client) {
    char msg[BUF_SIZE];
    // read message from client
    if (read(sockfd, msg, BUF_SIZE) < 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    // remove '\n' if it exists inside the message
    char *pos;
    if ((pos = strchr(msg, '\n')) != NULL) {
        *pos = '\0';
    }

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
        ip.s_addr = atoi(words[1]);
        unsigned short port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }

        // empty message string
        memset(msg, 0, strlen(msg));
        // read message from client (client will ask for client list immediately
        // after sending LOG_ON)
        if (read(sockfd, msg, BUF_SIZE) < 0) {
            perror(RED "Error reading from socket" RESET);
            exit(EXIT_FAILURE);
        }

        // remove '\n' if it exists inside the message
        char *pos;
        if ((pos = strchr(msg, '\n')) != NULL) {
            *pos = '\0';
        }

        if (strcmp(msg, "GET_CLIENTS") == 0) {
            send_client_list(list, tup, sockfd);
        }
    } else if (strcmp(words[0], "LOG_OFF") == 0) {
        struct in_addr ip;
        ip.s_addr = client.sin_addr.s_addr;
        unsigned short port = client.sin_port;
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (delete_list_node(list, tup) == 1) {
            printf(RED "Tuple doesn't exist.\n" RESET);
        }
    }
}

void send_logon_msg(int sockfd, int port, struct sockaddr_in client) {
    printf("Client: Port: %d, Address: %s\n", client.sin_port,
           inet_ntoa(client.sin_addr));

    // inform server that this new client has arrived
    char msg[BUF_SIZE];
    sprintf(msg, "LOG_ON %d %d", client.sin_addr.s_addr, client.sin_port);
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
    // read message from client
    if (read(sockfd, buf, CLIENT_LIST_SIZE) < 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }

    return buf;
}

void parse_client_list(char *str, List **list) {
    // message form: CLIENT_LIST N ip1,port1 ip2,port2 ip3,port3 ...
    // break message into words
    char *word = strtok(str, " ");  // split first two words of message
    for (int i = 0; i < 2; i++) {
        word = strtok(NULL, " ");
    }
    // split remaining sentence to words which represend tuples
    while (word) {
        // split (by comma) each tuple to ip address and port
        char *token = strtok(word, ",");
        struct in_addr ip;
        unsigned short port;
        for (int i = 0; i < 2; i++) {
            if (i == 0) {
                ip.s_addr = atoi(token);
            } else {
                port = atoi(token);
            }
            token = strtok(NULL, " ");
        }
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }
        word = strtok(NULL, " ");
    }
}

void send_client_list(List **list, Tuple tup, int sockfd) {
    char response[CLIENT_LIST_SIZE];
    sprintf(response, "CLIENT_LIST %d ", (*list)->size);

    // construct message to send to client
    // message format: CLIENT_LIST N ip1,port1 ip2,port2 ip3,port3 ...
    List_node *current = (*list)->head;

    while (current != NULL) {
        // add to message only tuples different that this client's
        if (compare_tuples(current->tuple, tup) == 1) {
            char temp[TUPLE_BUF_SIZE];
            sprintf(temp, "%s,%d ", inet_ntoa(current->tuple.ip_address),
                    current->tuple.port_num);
            strcat(response, temp);
        }
        current = current->next;
    }

    printf("Response %s\n", response);

    // write response to client
    if (write(sockfd, response, CLIENT_LIST_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
}