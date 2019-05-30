#ifndef READ_FUNCTIONS_HEADER
#define READ_FUNCTIONS_HEADER

#include "pathlist.h"

// Read command line arguments for server program
void read_server_arguments(int argc, char const *argv[], int *port_num);
// Read command line arguments for client program
void read_client_arguments(int argc, char const *argv[], char **dirname,
                           int *port_num, int *worker_threads_num, int *bufsize,
                           int *server_port, char **server_ip);
// Read message from a socket and remove newline character if it exists
void read_message_from_socket(int sockfd, char *msg, int size);
// Add to a list all files (their paths) in a directory and its subdirectories
// and return the list
void list_files(Pathlist *list, char *dirname);
#endif