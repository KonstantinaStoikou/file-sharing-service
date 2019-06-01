#include "../include/thread_functions.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/defines.h"

void create_n_threads(int worker_threads_num, Arg_struct *args) {
    // create an array to store thread ids
    pthread_t *t_ids = malloc(worker_threads_num * sizeof(pthread_t));
    // create worker threads
    for (int i = 0; i < worker_threads_num; i++) {
        if (pthread_create(&t_ids[i], NULL, read_from_buffer, (void *)args) !=
            0) {
            perror(RED "Error while creating threads");
            exit(EXIT_FAILURE);
        }
    }
}

void *read_from_buffer(void *args) {
    // read from circular buffer continuously
    while (1) {
    }
    print_circ_buf(((Arg_struct *)args)->cb);
    printf("thread %ld\n", pthread_self());
    pthread_exit(NULL);
}
