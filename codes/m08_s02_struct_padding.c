/**
 * @file 15_struct_padding.c
 * @brief Demonstration of struct memory padding and Flexible Array Members.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 15_struct_padding.c -o padding
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

// BAD LAYOUT: Causes memory fragmentation due to padding
struct UnoptimizedSensor {
    uint8_t  id;         // 1 byte + 3 bytes padding
    uint32_t timestamp;  // 4 bytes
    uint16_t status;     // 2 bytes + 2 bytes trailing padding
};

// OPTIMIZED LAYOUT: Ordered from largest to smallest alignment requirements
struct OptimizedSensor {
    uint32_t timestamp;  // 4 bytes
    uint16_t status;     // 2 bytes
    uint8_t  id;         // 1 byte
    uint8_t  reserved;   // 1 byte explicit padding
};

// FLEXIBLE ARRAY MEMBER: A struct ending with an incomplete array type
typedef struct {
    uint16_t packet_id;
    uint16_t payload_length;
    // The zero-sized array acts as a placeholder for dynamically allocated contiguous data
    uint8_t  payload[];
} PacketFrame;

int main(void) {
    printf("--- Struct Padding & Alignment ---\n\n");

    printf("Size of UnoptimizedSensor: %zu bytes\n", sizeof(struct UnoptimizedSensor));
    printf("Size of OptimizedSensor:   %zu bytes\n", sizeof(struct OptimizedSensor));

    printf("\n--- Flexible Array Member Allocation ---\n\n");

    uint16_t dynamic_length = 32;
    // Calculate total required memory: Header size + Payload size
    size_t total_size = sizeof(PacketFrame) + (dynamic_length * sizeof(uint8_t));

    PacketFrame *frame = (PacketFrame *)malloc(total_size);
    if (frame != nullptr) {
        frame->packet_id = 0xABCD;
        frame->payload_length = dynamic_length;

        printf("Allocated PacketFrame. Header: %zu bytes, Payload: %u bytes.\n",
               sizeof(PacketFrame), frame->payload_length);
        printf("Total contiguous memory allocated: %zu bytes.\n", total_size);

        // Accessing the payload safely
        frame->payload[0] = 0xFF;

        free(frame);
        frame = nullptr;
    }

    return 0;
}
