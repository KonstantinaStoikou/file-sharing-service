#ifndef PARSING_HEADER
#define PARSING_HEADER

#include "circular_buffer.h"
#include "list.h"

// Parse string with client list entries and store them to the list struct and
// add them as items to circular buffer
void parse_client_list(char* str, List* list, Circular_buffer* cb);
// Parse string with file list entries and add them as items to circular buffer
// (after comparing versions)
void parse_file_list(char* str, Circular_buffer* cb, char* dirname, int sock,
                     struct in_addr ip_address, unsigned short port_num);

#endif