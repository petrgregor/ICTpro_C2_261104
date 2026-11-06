/**
 * @file 16_memory_copy_realloc.c
 * @brief memmove for sliding windows and the dangers of realloc in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 16_memory_copy_realloc.c -o memcopy
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STREAM_BUFFER_SIZE 16U

typedef struct {
    uint8_t storage[STREAM_BUFFER_SIZE];
    size_t valid_bytes;
} StreamBuffer;

/**
 * @brief Consumes bytes from the front of the buffer and shifts the rest.
 */
bool stream_buffer_consume(StreamBuffer *stream, size_t consumed_bytes) {
    if (consumed_bytes > stream->valid_bytes) return false;

    size_t remaining_bytes = stream->valid_bytes - consumed_bytes;

    if (remaining_bytes > 0) {
        // OVERLAPPING MEMORY: Source and destination share the same buffer.
        // memmove safely handles this overlap, whereas memcpy would cause Undefined Behavior.
        memmove(&stream->storage[0],
                &stream->storage[consumed_bytes],
                remaining_bytes);
    }

    stream->valid_bytes = remaining_bytes;
    return true;
}

/**
 * @brief Simulates the classic memory leak trap when using realloc.
 */
void dangerous_realloc_pattern(void) {
    printf("\n--- Dangers of realloc ---\n");

    uint8_t *dynamic_buffer = (uint8_t *)malloc(64);
    if (dynamic_buffer) {
        printf("Initial allocation successful at %p.\n", (void*)dynamic_buffer);

        // ANTI-PATTERN: If realloc fails (e.g. out of memory), it returns nullptr,
        // but the original memory block is NOT freed.
        // Assigning the result back to 'dynamic_buffer' instantly leaks the original memory!

        // Simulating a request for a massive amount of memory to force a failure
        uint8_t *temp = (uint8_t *)realloc(dynamic_buffer, 0xFFFFFFFFFFFF);

        if (temp == nullptr) {
            printf("Realloc failed. If we didn't use a 'temp' pointer, the original block at %p would be lost forever!\n", (void*)dynamic_buffer);
        } else {
            dynamic_buffer = temp;
        }

        // Clean up the original buffer safely
        free(dynamic_buffer);
    }
}

int main(void) {
    printf("--- memmove Sliding Window ---\n");

    StreamBuffer my_stream = { .valid_bytes = 6 };
    // Fill with dummy data
    for (uint8_t i = 0; i < 6; i++) my_stream.storage[i] = i + 1; // 1, 2, 3, 4, 5, 6

    printf("Buffer before consume: ");
    for (size_t i = 0; i < my_stream.valid_bytes; i++) printf("%d ", my_stream.storage[i]);
    printf("\n");

    // Consume first 2 bytes
    stream_buffer_consume(&my_stream, 2);

    printf("Buffer after consume:  ");
    for (size_t i = 0; i < my_stream.valid_bytes; i++) printf("%d ", my_stream.storage[i]);
    printf("\n");

    dangerous_realloc_pattern();

    return 0;
}
