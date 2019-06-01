#ifndef SESSIONS_HEADER
#define SESSIONS_HEADER

#include <netinet/in.h>

// create a socket and bind it to given server
int create_sock_and_bind(struct sockaddr *serverptr, struct sockaddr_in server);
// Start a new session with a server by creating socket and connecting to
// it, and also bind client's port, return socket's file descriptor
int start_new_session(struct sockaddr *serverptr, struct sockaddr_in server,
                      struct sockaddr *clientptr, struct sockaddr_in client);
// Create socket for server (or client working as server) and start listen to
// this port, return listening socket's file descriptor
int start_listening_port(struct sockaddr *serverptr, struct sockaddr_in server);
// Get client's address in network byte order
struct in_addr get_client_info();

#endif