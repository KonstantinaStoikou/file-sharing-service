#include <stdio.h>
#include "../include/read_functions.h"

int main(int argc, char const *argv[]) {
    char *dirname;
    int port, worker_threads_num, bufsize, server_port, server_ip;

    read_client_arguments(argc, argv, &dirname, &port, &worker_threads_num,
                          &bufsize, &server_port, &server_ip);

    printf("Dirname: %s\n", dirname);
    printf("Port num: %d\n", port);
    printf("Worker threads: %d\n", worker_threads_num);
    printf("Buffer size: %d\n", bufsize);
    printf("Server port: %d\n", server_port);
    printf("Server ip: %d\n", server_ip);
    return 0;
}
