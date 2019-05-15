#ifndef READ_FUNCTIONS_HEADER
#define READ_FUNCTIONS_HEADER

#include "tuple.h"

// Read command line arguments for server program
void read_server_arguments(int argc, char const *argv[], int *port_num);
// Read command line arguments for client program
void read_client_arguments(int argc, char const *argv[], char **dirname,
                           int *port_num, int *worker_threads_num, int *bufsize,
                           int *server_port, char **server_ip);
#endif