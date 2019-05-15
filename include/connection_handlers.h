#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "../include/list.h"

// Handle requests in a connection from sever side
void handle_client_connection(int sockfd, List** list,
                              struct sockaddr_in server);
// Send a LOG_ON message to server
void send_logon_msg(int sock, int port, struct sockaddr_in client);
// Send a GET_CLIENTS message to server and get response and return the response
// string
char* send_getclients_msg(int sock);
// Parse string with client list entries and store them to the list struct
void parse_client_list(char* str, List** list);

#endif