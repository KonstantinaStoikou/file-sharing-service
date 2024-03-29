#include "../include/circular_buffer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/defines.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_cond;
pthread_cond_t full_cond;

Circular_buffer *initialize_circ_buf(int capacity, int item_size) {
    // initialize conditional variable
    pthread_cond_init(&empty_cond, NULL);

    Circular_buffer *cb = malloc(sizeof(Circular_buffer));
    cb->buffer = malloc(capacity * item_size);
    cb->buffer_end = (char *)cb->buffer + capacity * item_size;
    cb->capacity = capacity;
    cb->count = 0;
    cb->item_size = item_size;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;

    return cb;
}

void delete_circ_buf(Circular_buffer *cb) {
    free(cb->buffer);
    free(cb);
}

void push_back_circ_buf(Circular_buffer *cb, const void *item) {
    pthread_mutex_lock(&mutex);
    if (cb->count == cb->capacity) {
        pthread_cond_wait(&full_cond, &mutex);
    }
    memcpy(cb->head, item, cb->item_size);
    cb->head = (char *)cb->head + cb->item_size;
    if (cb->head == cb->buffer_end) {
        cb->head = cb->buffer;
    }
    cb->count++;
    pthread_mutex_unlock(&mutex);
}

void pop_front_circ_buf(Circular_buffer *cb, void *item) {
    pthread_mutex_lock(&mutex);
    if (cb->count == 0) {
        pthread_cond_wait(&empty_cond, &mutex);
    }
    memcpy(item, cb->tail, cb->item_size);
    cb->tail = (char *)cb->tail + cb->item_size;
    if (cb->tail == cb->buffer_end) {
        cb->tail = cb->buffer;
    }
    cb->count--;
    pthread_mutex_unlock(&mutex);
}

void print_circ_buf(Circular_buffer *cb) {
    void *current = cb->tail;
    while (current != cb->head) {
        Cb_data *item = malloc(sizeof(Cb_data));
        memcpy(item, current, cb->item_size);
        printf("%d %d %s %s\n", item->ip_address.s_addr, item->port_num,
               item->pathname, item->version);
        free(item);
        // get next item
        current = (char *)current + cb->item_size;
    }
}
