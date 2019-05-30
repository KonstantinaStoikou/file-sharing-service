#ifndef PARSING_HEADER
#define PARSING_HEADER

#include "circular_buffer.h"
#include "list.h"

// Parse string with client list entries and store them to the list struct and
// add them as items to circular buffer
void parse_client_list(char* str, List* list, Circular_buffer* cb);

#endif