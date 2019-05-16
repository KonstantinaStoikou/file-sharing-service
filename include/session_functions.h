#ifndef SESSIONS_HEADER
#define SESSIONS_HEADER

#include <netinet/in.h>

// Start a new session with a server by creating socket and connecting to it,
// return socket's file descriptor
int start_new_session(struct sockaddr *serverptr, struct sockaddr_in server);
// Create socket for client and start listen to this port, return listening
// socket's file descriptor
int start_listening_port(struct sockaddr *clientptr, struct sockaddr_in *client,
                         int port);
// Get client's address in network byte order
struct in_addr get_client_info();

#endif