/**
 * @file 25_network_api.h
 * @brief Public interface hiding implementation details via Opaque Pointer.
 */
#ifndef NETWORK_API_H
#define NETWORK_API_H

#include <stdbool.h>

// FORWARD DECLARATION (Opaque Pointer)
// The caller only knows the type name, not its size or internal members.
typedef struct NetworkConnection NetworkConnection;

// Public API Functions
NetworkConnection* net_create(void);
bool net_send(NetworkConnection *conn, const char *msg);
void net_destroy(NetworkConnection *conn);

#endif // NETWORK_API_H
