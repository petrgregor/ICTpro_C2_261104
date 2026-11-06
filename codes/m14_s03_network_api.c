/**
 * @file 25_network_api.c
 * @brief Private implementation of the NetworkConnection struct.
 */
#include "m14_s03_network_api.h"
#include <stdio.h>
#include <stdlib.h>

// PRIVATE DEFINITION:
// This is hidden entirely from the user who includes network_api.h
struct NetworkConnection {
    int internal_socket_fd;
    bool is_active;
};

NetworkConnection* net_create(void) {
    NetworkConnection *conn = (NetworkConnection *)malloc(sizeof(NetworkConnection));
    if (conn != nullptr) {
        conn->internal_socket_fd = 1001; // Mock socket
        conn->is_active = true;
        printf("[Network API] Connection instantiated securely.\n");
    }
    return conn;
}

bool net_send(NetworkConnection *conn, const char *msg) {
    if (conn == nullptr || !conn->is_active) return false;
    printf("[Network API] Sending via socket %d: %s\n", conn->internal_socket_fd, msg);
    return true;
}

void net_destroy(NetworkConnection *conn) {
    if (conn != nullptr) {
        conn->is_active = false;
        free(conn);
        printf("[Network API] Connection destroyed.\n");
    }
}
