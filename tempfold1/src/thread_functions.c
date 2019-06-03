#include "../include/thread_functions.h"
#include <arpa/inet.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/defines.h"
#include "../include/parsing_functions.h"
#include "../include/read_functions.h"
#include "../include/send_functions.h"
#include "../include/session_functions.h"
#include "../include/si
pthread_mutex_unlock(&full_mutex);
if tK_STREAM, 0)) < 0) {
        perror(RED "Error while creating socket" RESET);
        exit(EXIT_FAILURE);
    }
if (connect(newsock, clientptr, sizeof(client)) < 0) {
    perror(RED "Error while connecting" RESET);
    exit(EXIT_FAILURE);
}
// if version is -1 then send GET_FILE_LIST to other client
if (strcmp(item->version, "-1") == 0) {
    send_getfilelist_msg(newsock);
    char msg[FILE_LIST_SIZE];
    read_message_from_socket(newsock, msg, FILE_LIST_SIZE);
    // form backup subdirectory path for this client
    char dirpath[DIRPATH_SIZE];
    sprintf(dirpath, "%s/%s_%d/", ((Arg_struct *)args)->backup_dirname,
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    // create backup subdirectory if it doesn't exist
    struct stat st = {0};
    if (stat(dirpath, &st) == -1) {
        mkdir(dirpath, 0700);
    }
    parse_file_list(msg, cb, dirpath, newsock, client.sin_addr, client.sin_port,
                    &full_mutex, &full_cond);
}
// else send GET_FILE to other client
else {
    send_getfile_msg(newsock, item->pathname, item->version);
    char msg[FILE_BYTES_SIZE];
    if (read(newsock, msg, FILE_BYTES_SIZE) < 0) {
        perror(RED "Error reading from socket" RESET);
        exit(EXIT_FAILURE);
    }
    char file_cont[FILE_BYTES_SIZE];
    // if a file was written in socket, create it
    if (parse_file(msg, file_cont) == 0) {
        // form backup subdirectory path for this client
        char pathname_copy[BUF_SIZE];
        strcpy(pathname_copy, item->pathname);
        char cmd[BUF_SIZE];
        sprintf(cmd, "mkdir -p %s && touch %s", dirname(pathname_copy),
                item->pathname);
        // execute mkdir and touch
        system(cmd);
        // write file contents to file
        FILE *fp = fopen(item->pathname, "w");
        if (fp != NULL) {
            fputs(file_cont, fp);
            fclose(fp);
        }
    }
}
close(newsock);
free(item);
}
pthread_exit(NULL);
}
