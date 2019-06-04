/**
 * A circular buffer struct to store data related to files.
 *
 */

#ifndef CIRC_BUF_HEADER
#define CIRC_BUF_HEADER

#include <netinet/in.h>
#include "defines.h"

extern pthread_mutex_t mutex;
extern pthread_cond_t empty_cond;
extern pthread_cond_t full_cond;

typedef struct Circular_buffer {
    void *buffer;      // data buffer
    void *buffer_end;  // end of data buffer
    int capacity;      // maximum number of items in the buffer
    int count;         // number of items in the buffer
    size_t item_size;  // size of each item in the buffer
    void *head;        // pointer to head
    void *tail;        // pointer to tail
} Circular_buffer;

typedef struct Cb_data {
    char pathname[PATH_SIZE];
    char version[MD5_SIZE];
    struct in_addr ip_address;
    unsigned short port_num;
} Cb_data;

// Allocate memory for a new circular buffer struct and return it
Circular_buffer *initialize_circ_buf(int capacity, int item_size);
// Release memory of circular buffer
void delete_circ_buf(Circular_buffer *cb);
// Add an item to the back of the circular buffer
void push_back_circ_buf(Circular_buffer *cb, const void *item);
// Remove an item from the front of the circular buffer
void pop_front_circ_buf(Circular_buffer *cb, void *item);
// Print all items in circular buffer
void print_circ_buf(Circular_buffer *cb);
#endif