/**
 * @file 03_explicit_memory.c
 * @brief Demonstration of explicit resource management in C (vs RAII in C++).
 *
 * Compile: gcc -Wall -Wextra -std=c2x 03_explicit_memory.c -o explicit_mem
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *secure_payload;
    size_t payload_size;
} SecurePacket;

/**
 * @brief Securely processes and destroys the packet.
 *
 * Unlike C++ destructors, this function MUST be called explicitly by the programmer.
 * There is no RAII (Resource Acquisition Is Initialization) to save you if you forget.
 */
void process_and_destroy_packet(SecurePacket *packet) {
    if (!packet || !packet->secure_payload) {
        return;
    }

    printf("Processing packet: %s\n", packet->secure_payload);

    // 1. Explicitly clear sensitive memory to prevent data leaks.
    // In C23, use memset_explicit() (if supported by standard library) to ensure
    // the compiler optimizer doesn't remove this "dead store".
    // For now, we use standard memset to demonstrate the concept.
    memset(packet->secure_payload, 0, packet->payload_size);
    printf("Memory wiped.\n");

    // 2. Explicitly free the heap allocation
    free(packet->secure_payload);

    // 3. Explicitly nullify pointers and lengths to prevent dangling pointer access
    packet->secure_payload = NULL;
    packet->payload_size = 0;

    printf("Packet destroyed safely.\n");
}

int main(void) {
    SecurePacket my_packet;
    my_packet.payload_size = 128;

    // Explicit allocation (no constructors invoked automatically)
    my_packet.secure_payload = (char *)malloc(my_packet.payload_size);

    if (!my_packet.secure_payload) {
        fprintf(stderr, "Allocation failed!\n");
        return 1;
    }

    // Populate the buffer
    strncpy(my_packet.secure_payload, "TOP SECRET: Launch codes 42-X-99", my_packet.payload_size);

    // We must manually pass the pointer. No hidden copy constructors will duplicate this memory.
    process_and_destroy_packet(&my_packet);

    return 0;
}