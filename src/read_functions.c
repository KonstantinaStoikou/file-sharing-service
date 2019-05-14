#include "../include/read_functions.h"
#include <stdlib.h>
#include <string.h>

void read_server_arguments(int argc, char const *argv[], int *port_num) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            *port_num = atoi(argv[i + 1]);
        }
    }
}
