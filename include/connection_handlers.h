#ifndef CON_HANDLERS_HEADER
#define CON_HANDLERS_HEADER

#include <netinet/in.h>
#include "../include/list.h"

void handle_client_connection(int sockfd, List **list,
                              struct sockaddr_in server);

#endif