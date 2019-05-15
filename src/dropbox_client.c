#include <arpa/inet.h>
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

    // retrieve this hostname
    char hostbuf[BUF_SIZE];
    if (gethostname(hostbuf, sizeof(hostbuf)) < 0) {
        perror(RED "Error in gethostname" RESET);
        exit(EXIT_FAILURE);
    }

    // retrieve this host's information
    struct hostent *rem_client;
    if ((rem_client = gethostbyname(hostbuf)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in client;
    // memcpy(&client.sin_addr, rem_client->h_addr, rem_client->h_length);
    client.sin_addr.s_addr = htonl(*rem_client->h_addr);
    client.sin_port = htons(port);

    printf("Client: Port: %d, Address: %s\n", client.sin_port,
           inet_ntoa(client.sin_addr));

    // inform server that this new client has arrived
    char msg[BUF_SIZE];
    sprintf(msg, "LOG_ON %d %d", client.sin_addr.s_addr, client.sin_port);
    if (write(sock, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    close(sock);

    sock = start_new_session(serverptr, server);
    // empty message string
    memset(msg, 0, strlen(msg));
    strcpy(msg, "GET_CLIENTS");
    if (write(sock, msg, BUF_SIZE) < 0) {
        perror(RED "Error writing to socket" RESET);
        exit(EXIT_FAILURE);
    }
    char buf[CLIENT_LIST_SIZE];
    // read message from client
    if (read(sock, buf, CLIENT_LIST_SIZE) < 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }

    printf("Received string: %s", buf);
    close(sock);

    return 0;
}
