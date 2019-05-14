#include "../include/connection_handlers.h"
#include <arpa/inet.h>
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
    if (read(sockfd, msg, BUF_SIZE) <= 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    // remove '\n' if it exists inside the message
    char *pos;
    if ((pos = strchr(msg, '\n')) != NULL) {
        *pos = '\0';
    }

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
        struct in_addr ip;
        ip.s_addr = atoi(words[1]);
        unsigned short port = atoi(words[2]);
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;
        if (add_list_node(list, tup) == NULL) {
            printf(RED "Tuple already exists.\n" RESET);
        }
    } else if (strcmp(words[0], "GET_CLIENTS") == 0) {
        struct in_addr ip;
        ip.s_addr = client.sin_addr.s_addr;
        unsigned short port = client.sin_port;
        Tuple tup;
        tup.ip_address = ip;
        tup.port_num = port;

        char response[CLIENT_LIST_SIZE];
        sprintf(response, "CLIENT_LIST %d ", (*list)->size);

        // construct message to send to client
        // message form: CLIENT_LIST N ip1,port1 ip2,port2 ip3,port3 ...
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