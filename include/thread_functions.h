#ifndef THREAD_FUNCTIONS_HEADER
#define THREAD_FUNCTIONS_HEADER

#include <pthread.h>
#include "circular_buffer.h"
#include "defines.h"
#include "list.h"

extern pthread_mutex_t buf_mutex;

typedef struct Arg_struct {
    Circular_buffer *cb;
    List *client_list;
    char backup_dirname[DIRNAME_SIZE];
} Arg_struct;

// Create N worker threads
void create_n_threads(int worker_threads_num, Arg_struct *args,
                      pthread_t *t_ids);
// Cancel all worker threads
void stop_threads(int worker_threads_num, pthread_t *t_ids);
// The function that threads will execute, they read continuously from
// circular buffer until a terminating signal is received
void *read_from_buffer(void *args);

#endif