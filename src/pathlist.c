#include "../include/pathlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/defines.h"

Pathlist *initialize_pathlist(void) {
    Pathlist *list = malloc(sizeof(Pathlist));
    list->head = NULL;
    list->size = 0;

    return list;
}

Pathlist_node *add_pathlist_node(Pathlist *list, char *data) {
    // check if node already exists
    if (search_pathlist_node(list, data) != NULL) {
        return NULL;
    }
    // allocate memory for node
    Pathlist_node *new_node = (Pathlist_node *)malloc(sizeof(Pathlist_node));
    strcpy(new_node->path, data);
    new_node->next = list->head;
    // change head pointer as new node is added at the beginning
    list->head = new_node;
    (list->size)++;

    return new_node;
}

int delete_pathlist_node(Pathlist *list, char *data) {
    // store head node
    Pathlist_node *current = list->head, *prev;

    // check if node to be deleted is the head
    if (current != NULL && strcmp(current->path, data) == 0) {
        list->head = current->next;
        free(current);
        (list->size)--;
        return 0;
    }

    // search for the key to be deleted, keep track of the
    // previous node as 'prev->next' needs to change
    while (current != NULL && strcmp(current->path, data) != 0) {
        prev = current;
        current = current->next;
    }

    // if there is no such node in the list
    if (current == NULL) {
        return 1;
    }
    (list->size)--;
    // unlink the node from linked list
    prev->next = current->next;
    // free memory
    free(current);
    return 0;
}

Pathlist_node *search_pathlist_node(Pathlist *list, char *data) {
    Pathlist_node *current = list->head;

    while (current != NULL) {
        if (strcmp(current->path, data) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void print_pathlist(Pathlist *list) {
    printf("Size %d\n", list->size);
    Pathlist_node *current = list->head;

    while (current != NULL) {
        printf("%s\n", current->path);
        current = current->next;
    }
}

void delete_pathlist(Pathlist *list) {
    Pathlist_node *current = list->head;
    Pathlist_node *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
