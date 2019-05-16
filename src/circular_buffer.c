#include "../include/circular_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/defines.h"

Circular_buffer *initialize_circ_buf(int capacity, int item_size) {
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
    if (cb->count == cb->capacity) {
        printf(RED "No space in circular buffer." RESET);
    }
    memcpy(cb->head, item, cb->item_size);
    cb->head = (char *)cb->head + cb->item_size;
    if (cb->head == cb->buffer_end) {
        cb->head = cb->buffer;
    }
    cb->count++;
}

void pop_front_circ_buf(Circular_buffer *cb, void *item) {
    if (cb->count == 0) {
        printf(RED "No items in circular buffer." RESET);
    }
    memcpy(item, cb->tail, cb->item_size);
    cb->tail = (char *)cb->tail + cb->item_size;
    if (cb->tail == cb->buffer_end) {
        cb->tail = cb->buffer;
    }
    cb->count--;
}

void print_circ_buf(Circular_buffer *cb) {
    void *current = cb->tail;
    while (current != cb->head) {
        Cb_data *item = malloc(sizeof(Cb_data));
        memcpy(item, current, cb->item_size);
        printf("%s %d\n", item->pathname, item->version);
        free(item);
        // get next item
        current = (char *)current + cb->item_size;
    }
}