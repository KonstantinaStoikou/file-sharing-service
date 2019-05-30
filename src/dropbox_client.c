#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
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

    // get clients ip address
    struct in_addr client_ip = get_client_info();

    client.sin_family = AF_INET;  // internet domain
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(port);

    printf("This Client: Port: %d, Address: %s\n", client.sin_port,
           inet_ntoa(client_ip));

    if ((rem_server = gethostbyname(server_ip)) == NULL) {
        herror(RED "Error in gethostbyname" RESET);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;  // internet domain
    memcpy(&server.sin_addr, rem_server->h_addr, rem_server->h_length);
    server.sin_port = htons(server_port);

    // start new session with server and send LOG_ON message with this client's
    // info
    int sock = start_new_session(serverptr, server, clientptr, client);
    send_logon_msg(sock, port, client_ip, client);

    // initialize list to store other clients' info
    List *client_list = initialize_list();

    // initialize citcular buffer
    Circular_buffer *cb = initialize_circ_buf(bufsize, sizeof(Cb_data));

    char *client_list_msg = send_getclients_msg(sock);
    // parse client list string
    parse_client_list(client_list_msg, client_list, cb);
    free(client_list_msg);
    close(sock);

    printf("List is: \n");
    print_list(client_list);
    printf("Buffer is: \n");
    print_circ_buf(cb);

    // create struct ith shared variables to pass as argument to threads
    Arg_struct *thr_args = malloc(sizeof(Arg_struct));
    thr_args->cb = cb;
    thr_args->client_list = client_list;
    // create worker threads
    create_n_threads(worker_threads_num, thr_args);

    struct sockaddr_in other_client;
    socklen_t other_clientlen;
    struct sockaddr *other_clientptr = (struct sockaddr *)&other_client;

    // start listening for requests from other clients or the server
    int listen_sock = start_listening_port(clientptr, client, port);

    // set signal handler
    struct sigaction act;
    act.sa_handler = exit_action;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while (1) {
        // accept connection
        int newsock = accept(listen_sock, other_clientptr, &other_clientlen);

        // check if SIGINT was received
        if (newsock < 0 && exit_var == 1) {
            close(listen_sock);
            // send message LOG_OFF to server
            int serv_sock =
                start_new_session(serverptr, server, clientptr, client);
            send_logoff_msg(serv_sock, port, client_ip, client);
            close(serv_sock);
            printf("Exit!\n");
            exit(EXIT_SUCCESS);
        }
        // else if ordinary error was caused display error message
        else if (newsock < 0) {
            perror(RED "Error while accepting connection" RESET);
            exit(EXIT_FAILURE);
        }
        printf("Client: Port: %d, Address: %d\n", htons(other_client.sin_port),
               other_client.sin_addr.s_addr);

        handle_client_connection(newsock, client_list, other_client, dirname,
                                 cb);
        // close socket, sock must be closed before it gets re-assigned
        close(newsock);

        printf("List is: \n");
        print_list(client_list);
        printf("Buffer is: \n");
        print_circ_buf(cb);
    }

    return 0;
}
