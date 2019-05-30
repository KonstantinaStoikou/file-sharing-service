#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "circular_buffer.h"
#include "list.h"

// Handle requests in a connection from server side
void handle_server_connection(int sockfd, List* list,
                              struct sockaddr_in server);
// Handle requests from other clients/server in a connection from client side
void handle_client_connection(int sockfd, List* list, struct sockaddr_in client,
                              char* dirname, Circular_buffer* cb);

#endif