#include "../include/list.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/defines.h"
#include "../include/tuple.h"

List *initialize_list(void) {
    List *list = malloc(sizeof(List));
    list->head = NULL;

    return list;
}

List_node *add_list_node(List **list, Tuple data) {
    // check if node already exists
    if (search_list_node(list, data) != NULL) {
        return NULL;
    }
    // allocate memory for node
    List_node *new_node = (List_node *)malloc(sizeof(List_node));
    new_node->tuple = data;
    new_node->next = (*list)->head;
    // change head pointer as new node is added at the beginning
    (*list)->head = new_node;

    return new_node;
}

int delete_list_node(List **list, Tuple data) {
    // store head node
    List_node *current = (*list)->head, *prev;

    // check if node to be deleted is the head
    if (current != NULL && compare_tuples(current->tuple, data) == 0) {
        (*list)->head = current->next;
        free(current);
        return 0;
    }

    // search for the key to be deleted, keep track of the
    // previous node as 'prev->next' needs to change
    while (current != NULL && compare_tuples(current->tuple, data) == 1) {
        prev = current;
        current = current->next;
    }

    // if there is no such node in the list
    if (current == NULL) {
        return 1;
    }

    // unlink the node from linked list
    prev->next = current->next;
    // free memory
    free(current);
    return 0;
}

List_node *search_list_node(List **list, Tuple data) {
    List_node *current = (*list)->head;

    while (current != NULL) {
        if (compare_tuples(current->tuple, data) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void print_list(List *list) {
    List_node *current = list->head;

    while (current != NULL) {
        printf("IP: %s, Port: %d\n", inet_ntoa(current->tuple.ip_address),
               current->tuple.port_num);
        current = current->next;
    }
}

void delete_list(List **list) {
    List_node *current = (*list)->head;
    List_node *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(*list);
}

int compare_tuples(Tuple tuple1, Tuple tuple2) {
    if (tuple1.ip_address.s_addr == tuple2.ip_address.s_addr &&
        tuple1.port_num == tuple2.port_num) {
        return 0;
    } else {
        return 1;
    }
}