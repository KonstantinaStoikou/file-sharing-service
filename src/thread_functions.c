#include "../include/thread_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"

pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cond;

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
        printf("Thread %ld cancelled\n", t_ids[i]);
        // if (pthread_join(t_ids[i], NULL) != 0) {
        //     perror(RED "Error while waiting for threads to terminate" RESET);
        //     exit(EXIT_FAILURE);
        // }
    }
}

void *read_from_buffer(void *args) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    printf("Thread %ld created!\n", pthread_self());
    Circular_buffer *cb = ((Arg_struct *)args)->cb;
    // read from circular buffer continuously
    while (1) {
        // pthread_mutex_lock(&buf_mutex);
        pthread_cond_wait(&empty_cond, &buf_mutex);
        Cb_data *item = malloc(sizeof(Cb_data));
        pop_front_circ_buf(cb, item);
        printf("Item: %d %d %s %d\n", item->ip_address.s_addr, item->port_num,
               item->pathname, item->version);
        // if version is -1 then send GET_FILE_LIST to other client
        if (item->version == -1) {
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
            send_getfilelist_msg(newsock);
            char msg[BUF_SIZE];
            // read message from socket
            if (read(newsock, msg, BUF_SIZE) < 0) {
                perror(RED "Error reading from socket" RESET);
                exit(EXIT_FAILURE);
            }

            printf("Message: %s\n", msg);
            close(newsock);
        }

        // pthread_mutex_unlock(&buf_mutex);
    }
    pthread_exit(NULL);
}
