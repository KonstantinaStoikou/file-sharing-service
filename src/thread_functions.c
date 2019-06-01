#include "../include/thread_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/defines.h"

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
        if (pthread_join(t_ids[i], NULL) != 0) {
            perror(RED "Error while waiting for threads to terminate" RESET);
            exit(EXIT_FAILURE);
        }
    }
}

void *read_from_buffer(void *args) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    printf("Thread %ld created!\n", pthread_self());
    // read from circular buffer continuously
    while (1) {
    }
    print_circ_buf(((Arg_struct *)args)->cb);
    printf("Thread %ld exits!\n", pthread_self());
    pthread_exit(NULL);
}
