/**
 * A linked list to keep paths of files.
 *
 */

#ifndef PATHLIST_HEADER
#define PATHLIST_HEADER

#include "defines.h"

// A node of the list
typedef struct Pathlist_node {
    char path[PATH_SIZE];
    struct Pathlist_node *next;
} Pathlist_node;

// A pointer to the head of the list
typedef struct Pathlist {
    Pathlist_node *head;
} Pathlist;

// Allocate memory for a new list struct and return it
Pathlist *initialize_pathlist(void);
// Insert new node at the beginning of the list and return the newly added node,
// if node already exists return NULL
Pathlist_node *add_pathlist_node(Pathlist *list, char *data);
// Delete a node with the given data, returns 1 if it doesn't exist, else 0
int delete_pathlist_node(Pathlist *list, char *data);
// Search for a node with the given data and return it, return NULL if it
// doesn't exist
Pathlist_node *search_pathlist_node(Pathlist *list, char *data);
// Print all paths in the list
void print_pathlist(Pathlist *list);
// Release memory of list
void delete_Pathlist(Pathlist *list);

#endif