#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/read_functions.h"

int main(int argc, char const *argv[]) {
    char *dirname;
    int port, worker_threads_num, bufsize, server_port, sock;
    char *server_ip;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    read_client_arguments(argc, argv, &dirname, &port, &worker_threads_num,
                          &bufsize, &server_port, &server_ip);

    printf("Dirname: %s\n", dirname);
    printf("Port num: %d\n", port);
    printf("Worker threads: %d\n", worker_threads_num);
    printf("Buffer size: %d\n", bufsize);
    printf("Server port: %d\n", server_port);
    printf("Server ip: %s\n", server_ip);

    // create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }

    if ((rem = gethostbyname(server_ip)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;  // internet domain
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(server_port);

    // initiate connection
    if (connect(sock, serverptr, sizeof(server)) < 0) {
        perror(RED "Error while connecting" RESET);
        exit(EXIT_FAILURE);
    }
    printf("Connecting to %s port %d\n", server_ip, server_port);
    char buf[BUF_SIZE];

    // write response to client
    if (write(sock, "GET_CLIENTS", strlen("GET_CLIENTS") + 1) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    // read message from client
    if (read(sock, buf, BUF_SIZE) <= 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Received string: %s", buf);
    close(sock);

    return 0;
}
