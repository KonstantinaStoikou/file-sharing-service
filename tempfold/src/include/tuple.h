/**
 * A struct to hold a tuple with ip address and port_num.
 *
 */

#ifndef TUPLE_HEADER
#define TUPLE_HEADER

#include <netinet/in.h>

typedef struct Tuple {
    struct in_addr ip_address;
    unsigned short port_num;
} Tuple;

#endif