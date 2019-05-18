#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "../include/list.h"

// Handle requests in a connection from server side
void handle_server_connection(int sockfd, List* list,
                              struct sockaddr_in server);
// Handle requests from other clients/server in a connection from client side
void handle_client_connection(int sockfd, List* list,
                              struct sockaddr_in client);
// Send a LOG_ON message to server
void send_logon_msg(int sockfd, int port, struct in_addr client_ip,
                    struct sockaddr_in client);
// Send a GET_CLIENTS message to server and get response and return the response
// string
char* send_getclients_msg(int sockfd);
// Parse string with client list entries and store them to the list struct
void parse_client_list(char* str, List* list);
// Create a string with client tuples and send it to client who made request
void send_client_list(List* list, Tuple tup, int sockfd);
// For each client in the list, except the current one, send a USER_ON message
void send_useron_msg(List* list, Tuple tup);
// Send a LOG_OFF message to server
void send_logoff_msg(int sockfd, int port, struct in_addr client_ip,
                     struct sockaddr_in client);

#endif