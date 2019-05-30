#ifndef THREAD_FUNCTIONS_HEADER
#define THREAD_FUNCTIONS_HEADER

#include "circular_buffer.h"
#include "list.h"

struct arg_struct {
    Circular_buffer *cb;
    List *client_list;
};

// Create N worker threads
void create_n_threads(int worker_threads_num);
// Test function for threads
void *do_nothing(void *args);

#endif