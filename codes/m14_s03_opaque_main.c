/**
 * @file 25_opaque_main.c
 * @brief Application code consuming the API.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 25_network_api.c 25_opaque_main.c -o opaque_ptr
 */
#include <stdio.h>
#include "m14_s03_network_api.h"

int main(void) {
    printf("--- Abstract Data Type (Opaque Pointer) ---\n\n");

    // The user ONLY interacts via the pointer and API functions
    NetworkConnection *my_conn = net_create();

    if (my_conn != nullptr) {
        net_send(my_conn, "Hello Server!");

        // COMPILER ERROR: The compiler prevents breaking encapsulation!
        // The user CANNOT access or modify the internal state directly:
        // my_conn->internal_socket_fd = 0; // error: dereferencing pointer to incomplete type

        net_destroy(my_conn);
    }

    return 0;
}
