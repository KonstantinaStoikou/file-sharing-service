#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/connection_handlers.h"
#include "../include/defines.h"
#include "../include/read_functions.h"
#include "../include/session_functions.h"

int main(int argc, char const *argv[]) {
    char *dirname;
    int port, worker_threads_num, bufsize, server_port, sock;
    char *server_ip;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    read_client_arguments(argc, argv, &dirname, &port, &worker_threads_num,
                          &bufsize, &server_port, &server_ip);

    printf(BLUE "Dirname: %s\n", dirname);
    printf("Port num: %d\n", port);
    printf("Worker threads: %d\n", worker_threads_num);
    printf("Buffer size: %d\n", bufsize);
    printf("Server port: %d\n", server_port);
    printf("Server ip: %s\n" RESET, server_ip);
    printf("\n");

    if ((rem = gethostbyname(server_ip)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;  // internet domain
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(server_port);

    sock = start_new_session(serverptr, server);
    printf("Connecting to %s port %d\n", server_ip, server_port);
    send_logon_msg(sock, port);
    close(sock);

    sock = start_new_session(serverptr, server);
    send_getclients_msg(sock);
    close(sock);

    return 0;
}
