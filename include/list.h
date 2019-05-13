/**
 * A linked list to keep client info.
 *
 */

#ifndef LIST_HEADER
#define LIST_HEADER

#include "tuple.h"

// A node of the list
typedef struct List_node {
    Tuple tuple;
    struct List_node *next;
} List_node;

// A pointer to the head of the list
typedef struct List {
    List_node *head;
} List;

// Allocate memory for a new list struct and return it
List *initialize_list(void);
// Insert new node at the beginning of the list and return the newly added node
List_node *add_list_node(List **list, Tuple data);
// Delete a node with the given data, returns 1 if there was an error, else 0
int delete_list_node(List **list, Tuple data);
// Search for a node with the given data and return it
List_node *search_list_node(List **list, Tuple data);
// Print all the tuple in the list
void print_list(List *list);
// Release memory of list
void delete_list(List **list);
// Compare the struct members of two tuples and if they are the same return 0,
// else return 1
int compare_tuples(Tuple tuple1, Tuple tuple2);

#endif