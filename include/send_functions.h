#ifndef SEND_FUNCTIONS_HEADER
#define SEND_FUNCTIONS_HEADER

#include <netinet/in.h>
#include "../include/list.h"
#include "../include/pathlist.h"

// Send a LOG_ON message to server
void send_logon_msg(int sockfd, int port, struct in_addr client_ip,
                    struct sockaddr_in client);
// Send a LOG_OFF message to server
void send_logoff_msg(int sockfd, int port, struct in_addr client_ip,
                     struct sockaddr_in client);
// Send a GET_CLIENTS message to server and get response and return the response
// string
char* send_getclients_msg(int sockfd);
// Create a string with client tuples and send it to client who made request
void send_client_list(List* list, Tuple tup, int sockfd);
// For each client in the list, except the current one, send a USER_ON message
void send_useron_msg(List* list, Tuple tup);
// For each client in the list, except the current one, send a USER_OFF message
void send_useroff_msg(List* list, Tuple tup);
// Create a string with file-version tuples and send it to client who made
// request
void send_file_list(Pathlist* list, int sockfd);

#endif