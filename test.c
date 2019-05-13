/**
 * A main to make tests on new features
 *
 */

#include <stdio.h>
#include "include/list.h"
#include "include/tuple.h"

int main(int argc, char const *argv[]) {
    List *list = initialize_list();

    Tuple tuple1;
    tuple1.ip_address = 1;
    tuple1.port_num = 10;
    Tuple tuple2;
    tuple2.ip_address = 2;
    tuple2.port_num = 20;
    Tuple tuple3;
    tuple3.ip_address = 3;
    tuple3.port_num = 30;
    Tuple tuple4;
    tuple4.ip_address = 4;
    tuple4.port_num = 40;
    Tuple tuple5;
    tuple5.ip_address = 5;
    tuple5.port_num = 50;

    add_list_node(&list, tuple1);
    add_list_node(&list, tuple2);
    add_list_node(&list, tuple3);
    add_list_node(&list, tuple4);
    add_list_node(&list, tuple5);
    print_list(list);
    printf("\n");
    delete_list_node(&list, tuple4);

    print_list(list);

    delete_list(&list);

    return 0;
}
