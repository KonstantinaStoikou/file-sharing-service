#include "../include/thread_functions.h"
#include <arpa/inet.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/parsing_functions.h"
#include "../include/read_functions.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"
#include "../include/signal_handlers.h"

pthread_mutex_t empty_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t full_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cond;
pthread_cond_t full_cond;

void create_n_threads(int worker_threads_num, Arg_struct *args,
                      pthread_t *t_ids) {
    // create worker threads
    for (int i = 0; i < worker_threads_num; i++) {
        if (pthread_create(&t_ids[i], NULL, read_from_buffer, (void *)args) !=
            0) {
            perror(RED "Error while creating threads");
            exit(EXIT_FAILURE);
        }
    }
}

void stop_threads(int worker_threads_num, pthread_t *t_ids) {
    for (int i = 0; i < worker_threads_num; i++) {
        if (pthread_cancel(t_ids[i]) != 0) {
            perror(RED "Error while canceling threads");
            exit(EXIT_FAILURE);
        }
        printf("Thread %ld killed.\n", t_ids[i]);
        pthread_mutex_unlock(&empty_mutex);
        pthread_mutex_unlock(&full_mutex);
        if (pthread_join(t_ids[i], NULL) != 0) {
            perror(RED "Error while waiting for threads to terminate" RESET);
            exit(EXIT_FAILURE);
        }
        printf("WAITED\n");
    }
    pthread_cond_destroy(&empty_cond);
    pthread_cond_destroy(&full_cond);
    pthread_mutex_destroy(&empty_mutex);
    pthread_mutex_destroy(&full_mutex);
}

void *read_from_buffer(void *args) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    Circular_buffer *cb = ((Arg_struct *)args)->cb;
    // read from circular buffer continuously
    while (1) {
        // wait if buffer is empty
        pthread_cond_wait(&empty_cond, &empty_mutex);
        if (cb->count == 0) {
            pthread_exit(NULL);
        }

        Cb_data *item = malloc(sizeof(Cb_data));
        pop_front_circ_buf(cb, item);
        // connect to client
        struct sockaddr_in client;
        struct sockaddr *clientptr = (struct sockaddr *)&client;
        client.sin_family = AF_INET;  // internet domain
        client.sin_addr = item->ip_address;
        client.sin_port = item->port_num;

        int newsock;
        if ((newsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror(RED "Error while creating socket" RESET);
            exit(EXIT_FAILURE);
        }
        if (connect(newsock, clientptr, sizeof(client)) < 0) {
            perror(RED "Error while connecting" RESET);
            exit(EXIT_FAILURE);
        }
        // if version is -1 then send GET_FILE_LIST to other client
        if (strcmp(item->version, "-1") == 0) {
            send_getfilelist_msg(newsock);
            char msg[FILE_LIST_SIZE];
            read_message_from_socket(newsock, msg, FILE_LIST_SIZE);
            // form backup subdirectory path for this client
            char dirpath[DIRPATH_SIZE];
            sprintf(dirpath, "%s/%s_%d/", ((Arg_struct *)args)->backup_dirname,
                    inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            // create backup subdirectory if it doesn't exist
            struct stat st = {0};
            if (stat(dirpath, &st) == -1) {
                mkdir(dirpath, 0700);
            }
            parse_file_list(msg, cb, dirpath, newsock, client.sin_addr,
                            client.sin_port, &full_mutex, &full_cond);
        }
        // else send GET_FILE to other client
        else {
            send_getfile_msg(newsock, item->pathname, item->version);
            char msg[FILE_BYTES_SIZE];
            if (read(newsock, msg, FILE_BYTES_SIZE) < 0) {
                perror(RED "Error reading from socket" RESET);
                exit(EXIT_FAILURE);
            }
            char file_cont[FILE_BYTES_SIZE];
            // if a file was written in socket, create it
            if (parse_file(msg, file_cont) == 0) {
                // form backup subdirectory path for this client
                char filepath[BUF_SIZE];
                sprintf(filepath, "%s/%s_%d/%s",
                        ((Arg_struct *)args)->backup_dirname,
                        inet_ntoa(client.sin_addr), ntohs(client.sin_port),
                        item->pathname);
                char cmd[BUF_SIZE];
                char filepath_copy[BUF_SIZE];
                strcpy(filepath_copy, filepath);
                sprintf(cmd, "mkdir -p %s && touch %s", dirname(filepath_copy),
                        filepath);
                // execute mkdir and touch
                system(cmd);
                // write file contents to file
                FILE *fp = fopen(filepath, "ab");
                if (fp != NULL) {
                    fputs(file_cont, fp);
                    fclose(fp);
                }
            }
        }
        close(newsock);
        free(item);
    }
    pthread_exit(NULL);
}
