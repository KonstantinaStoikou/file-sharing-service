#ifndef SESSIONS_HEADER
#define SESSIONS_HEADER

// Start a new session with a server by creating socket and connecting to it,
// return socket's file descriptor
int start_new_session(struct sockaddr *serverptr, struct sockaddr_in server);

#endif