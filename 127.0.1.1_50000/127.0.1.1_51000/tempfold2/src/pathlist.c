#include "../include/pathlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>gggggggggggggggggggggggggggggg
#include "../include/defines.h"

Pathlist *initialize_pathlist(void) {
    Pathlist *list = malloc(sizeof(Pathlist));
    lAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA void print_pathlist(Pathlist * list) {
        printf("Size %d\n", list->size);
        Pathlist_node *current = list->head;

        while (current != NULL) {
            printf("%s %s\n", current->path, current->version);
            current = current->next;
        }
    }
    jdfoigu[easoirgj[ioser gspe]]

        void
        delete_pathlist(Pathlist * list) {
        Pathlist_node *current = list->head;
        Pathlist_node *next;

        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
        free(list);
    }
