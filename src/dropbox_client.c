#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/circular_buffer.h"
#include "../include/connection_handlers.h"
#include "../include/defines.h"
#include "../include/list.h"
#include "../include/read_functions.h"
#include "../include/session_functions.h"

void *do_nothing(void *arg) {
    printf("thread %ld\n", pthread_self());
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    char *dirname;
    int port, worker_threads_num, bufsize, server_port;
    char *server_ip;
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    struct hostent *rem_server;

    read_client_arguments(argc, argv, &dirname, &port, &worker_threads_num,
                          &bufsize, &server_port, &server_ip);

    // get clients ip address
    struct in_addr client_ip = get_client_info();

    if ((rem_server = gethostbyname(server_ip)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;  // internet domain
    memcpy(&server.sin_addr, rem_server->h_addr, rem_server->h_length);
    server.sin_port = htons(server_port);

    // start new session with server and send LOG_ON message with this client's
    // info
    int sock = start_new_session(serverptr, server);
    send_logon_msg(sock, port, client_ip, client);

    // initialize list to store other clients' info
    List *client_list = initialize_list();

    char *client_list_msg = send_getclients_msg(sock);
    // parse client list string
    parse_client_list(client_list_msg, client_list);
    free(client_list_msg);
    close(sock);

    // initialize citcular buffer
    Circular_buffer *cb = initialize_circ_buf(bufsize, sizeof(Cb_data));

    print_list(client_list);

    // start listening for requests from other clients or the server
    int listen_sock = start_listening_port(clientptr, &client, port);

    // create worker threads
    pthread_t *t_ids = malloc(worker_threads_num * sizeof(pthread_t));

    for (int i = 0; i < worker_threads_num; i++) {
        if (pthread_create(&t_ids[i], NULL, do_nothing, NULL) != 0) {
            perror(RED "Error while creating threads");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
