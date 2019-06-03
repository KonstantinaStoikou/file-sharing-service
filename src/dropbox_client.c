#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/circular_buffer.h"
#include "../include/connection_handlers.h"
#include "../include/defines.h"
#include "../include/dir_functions.h"
#include "../include/list.h"
#include "../include/parsing_functions.h"
#include "../include/read_functions.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"
#include "../include/signal_handlers.h"
#include "../include/thread_functions.h"

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

    client.sin_family = AF_INET;  // internet domain
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(port);

    if ((rem_server = gethostbyname(server_ip)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;  // internet domain
    memcpy(&server.sin_addr, rem_server->h_addr, rem_server->h_length);
    server.sin_port = htons(server_port);

    // get clients ip address
    struct in_addr client_ip = get_client_info();
    printf("This client has port: %d, address: %s\n", ntohs(client.sin_port),
           inet_ntoa(client_ip));
    // create directory for this client to store backups of other clients
    char backup_dirname[DIRNAME_SIZE];
    make_backup_dir(inet_ntoa(client_ip), port, backup_dirname);

    // start new session with server and send LOG_ON message with this client's
    // info
    int sock = start_new_session(serverptr, server, clientptr, client);
    send_logon_msg(sock, client_ip, client);

    // initialize list to store other clients' info
    List *client_list = initialize_list();
    // initialize citcular buffer
    Circular_buffer *cb = initialize_circ_buf(bufsize, sizeof(Cb_data));

    // start listening for requests from other clients or the server
    int listen_sock = start_listening_port(clientptr, client);

    // create struct with shared variables to pass as argument to threads
    Arg_struct *thr_args = malloc(sizeof(Arg_struct));
    thr_args->cb = cb;
    thr_args->client_list = client_list;
    strcpy(thr_args->backup_dirname, backup_dirname);
    pthread_t *t_ids = malloc(worker_threads_num * sizeof(pthread_t));
    // create worker threads
    create_n_threads(worker_threads_num, thr_args, t_ids);

    // ask server for client list
    char *client_list_msg = send_getclients_msg(sock);
    // parse client list string
    parse_client_list(client_list_msg, client_list, cb);
    free(client_list_msg);
    close(sock);

    struct sockaddr_in other_client;
    socklen_t other_clientlen;
    struct sockaddr *other_clientptr = (struct sockaddr *)&other_client;

    // set signal handler
    struct sigaction act;
    act.sa_handler = exit_action;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    // set of socket descriptors
    fd_set active_fd_set, read_fd_set;
    // initialize the set of active sockets
    FD_ZERO(&active_fd_set);
    FD_SET(listen_sock, &active_fd_set);

    while (1) {
        // block until input arrives on one or more active sockets
        read_fd_set = active_fd_set;
        int err = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        // check if SIGINT was received
        if (err < 0 && exit_var == 1) {
            close(listen_sock);
            // send message LOG_OFF to server
            int serv_sock =
                start_new_session(serverptr, server, clientptr, client);
            send_logoff_msg(serv_sock);
            close(serv_sock);
            // stop all threads
            stop_threads(worker_threads_num, t_ids);
            delete_list(client_list);
            printf(GREEN "Remaining buffer.\n" RESET);
            print_circ_buf(cb);
            delete_circ_buf(cb);
            free(dirname);
            free(server_ip);
            free(t_ids);
            free(thr_args);
            printf(GREEN "Exit.\n" RESET);
            exit(EXIT_SUCCESS);
        } else if (err < 0) {
            perror(RED "Error in select" RED);
            exit(EXIT_FAILURE);
        }

        // service all the sockets with input pending
        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                // connection request on original socket
                if (i == listen_sock) {
                    // accept connection
                    int newsock =
                        accept(listen_sock, other_clientptr, &other_clientlen);
                    if (newsock < 0) {
                        perror(RED "Error while accepting connection" RESET);
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(newsock, &active_fd_set);
                }
                // data arriving on an already connected
                else {
                    handle_client_connection(i, client_list, other_client,
                                             dirname, cb);
                    // close socket and clear it to reuse it
                    close(i);
                    FD_CLR(i, &active_fd_set);
                }
            }
        }
    }

    return 0;
}
