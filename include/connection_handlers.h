#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "../include/list.h"

// Handle requests in a connection from sever side
void handle_client_connection(int sockfd, List **list,
                              struct sockaddr_in server);
// Send a LOG_ON message to server
void send_logon_msg(int sock, int port);
// Send a GET_CLIENTS message to server and get response
void send_getclients_msg(int sock);

#endif