#include "../include/parsing_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/defines.h"

void parse_client_list(char *str, List *list, Circular_buffer *cb) {
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
                data->version = -1;
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

void parse_file_list(char *str, Circular_buffer *cb, char *dirname) {
    // message form: FILE_LIST N filepath1,version1 filepath2,version2 ...
    // break message into words
    int count = 0;
    char *saveptr_str;
    char *word = strtok_r(str, " ", &saveptr_str);
    // split remaining sentence to words which represend tuples
    while (word) {
        // if this is not first or second word (FILE_LIST or N)
        if (count != 0 && count != 1) {
            // split (by comma) each tuple to pathname and version
            char *saveptr_word;
            char *token = strtok_r(word, ",", &saveptr_word);
            char path[PATH_SIZE];
            char version[MD5_SIZE];
            for (int i = 0; i < 2; i++) {
                if (i == 0) {
                    strcpy(path, token);
                } else {
                    strcpy(version, token);
                }
                token = strtok_r(NULL, ",", &saveptr_word);
            }
            // check if this file already exists in backup
            printf("Checking file: %s %s\n", path, version);
            // check if version is the same with already existing file
            // add it as item to circular buffer
            // else {
            //     Cb_data *data = malloc(sizeof(Cb_data));
            //     data->pathname[0] = '\0';
            //     data->version = -1;
            //     data->ip_address = ip;
            //     data->port_num = port;
            //     if (push_back_circ_buf(cb, data) == 1) {
            //         printf(RED "Buffer is full, item couldn't be added."
            //         RESET);
            //     }
            // }
        }
        count++;
        word = strtok_r(NULL, " ", &saveptr_str);
    }
}
