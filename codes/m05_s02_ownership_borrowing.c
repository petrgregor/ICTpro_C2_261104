/**
 * @file 07_ownership_borrowing.c
 * @brief Idioms for Ownership and Borrowing in C without a Garbage Collector.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t id;
    uint16_t data;
} Packet;

// 1. IMMUTABLE BORROW
// Caller retains ownership. Callee gets read-only access.
void telemetry_log_packet(const Packet *packet) {
    if (packet != nullptr) {
        printf("[Log] Packet ID: %u, Data: %u\n", packet->id, packet->data);
    }
}

// 2. MUTABLE BORROW
// Caller retains ownership. Callee modifies the object.
void packet_encrypt_data(Packet *packet) {
    if (packet != nullptr) {
        packet->data ^= 0xAAAA; // Dummy encryption
        printf("[Encrypt] Data obfuscated.\n");
    }
}

// 3. OWNERSHIP TRANSFER (TAKE)
// Callee takes ownership and is now responsible for freeing the memory.
void packet_queue_push(Packet *packet) {
    if (packet != nullptr) {
        printf("[Queue] Packet %u queued. Ownership transferred to queue.\n", packet->id);
        // Simulate queue dispatching and eventually freeing the memory
        free(packet);
    }
}

// 4. SAFE INVALIDATION PATTERN
// Takes a pointer-to-pointer. Frees memory and nullifies the caller's pointer
// to prevent use-after-free (dangling pointer) bugs.
void packet_destroy_safely(Packet **packet_ptr) {
    if (packet_ptr != nullptr && *packet_ptr != nullptr) {
        printf("[Cleanup] Destroying packet %u...\n", (*packet_ptr)->id);
        free(*packet_ptr);
        *packet_ptr = nullptr; // Invalidate caller's handle
    }
}

int main(void) {
    printf("--- Ownership and Borrowing Demo ---\n\n");

    // Main is the initial OWNER of this dynamically allocated packet
    Packet *my_packet = (Packet *)malloc(sizeof(Packet));
    if (my_packet != nullptr) {
        my_packet->id = 42;
        my_packet->data = 1234;

        // Borrowing: Passing access without giving up ownership
        telemetry_log_packet(my_packet); // Immutable borrow
        packet_encrypt_data(my_packet);  // Mutable borrow
        telemetry_log_packet(my_packet); // Immutable borrow

        // Scenario A: Safe destruction (Comment out Scenario B to test)
        /*
        packet_destroy_safely(&my_packet);
        if (my_packet == nullptr) {
            printf("[Main] Pointer successfully nullified. No double-free possible.\n");
        }
        //*/

        // Scenario B: Transferring ownership to another subsystem
        packet_queue_push(my_packet);
        my_packet = nullptr; // MUST manually nullify since queue took ownership by value
        printf("[Main] Ownership relinquished to queue.\n");
    }

    return 0;
}
