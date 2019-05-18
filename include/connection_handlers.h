#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "../include/list.h"
#include "../include/pathlist.h"

// Handle requests in a connection from server side
void handle_server_connection(int sockfd, List* list,
                              struct sockaddr_in server);
// Handle requests from other clients/server in a connection from client side
void handle_client_connection(int sockfd, List* list, struct sockaddr_in client,
                              char* dirname);
// Parse string with client list entries and store them to the list struct
void parse_client_list(char* str, List* list);
// Add to a list all files (their paths) in a directory and its subdirectories
// and return the list
void list_files(Pathlist* list, char* dirname);

#endif