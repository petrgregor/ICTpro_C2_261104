/**
 * @file 24_data_invariants.c
 * @brief Grouping related data and protecting structural invariants.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 24_data_invariants.c -o invariants
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_BUFFER_CAPACITY 8

// High Cohesion: The array and its length tracker are bound together.
typedef struct {
    uint8_t data[MAX_BUFFER_CAPACITY];
    size_t  active_length;
} SafeBuffer;

void buffer_init(SafeBuffer *buf) {
    if (buf != nullptr) {
        buf->active_length = 0;
    }
}

/**
 * @brief Appends a byte while strictly protecting the invariant.
 *
 * INVARIANT: active_length must NEVER exceed MAX_BUFFER_CAPACITY.
 */
bool buffer_append(SafeBuffer *buf, uint8_t new_byte) {
    if (buf == nullptr || buf->active_length >= MAX_BUFFER_CAPACITY) {
        return false; // Invariant protected: Rejects the operation safely
    }

    buf->data[buf->active_length] = new_byte;
    buf->active_length++;
    return true;
}

int main(void) {
    printf("--- Data Invariant Protection ---\n\n");

    SafeBuffer tx_buffer;
    buffer_init(&tx_buffer);

    // Attempting to append more bytes than the capacity allows
    for (int i = 1; i <= 10; i++) {
        if (buffer_append(&tx_buffer, (uint8_t)i)) {
            printf("Appended byte %d. Current length: %zu\n", i, tx_buffer.active_length);
        } else {
            printf("Failed to append byte %d! Buffer full (Invariant maintained).\n", i);
        }
    }

    // DANGEROUS ANTI-PATTERN: Directly modifying struct members circumvents safety!
    // tx_buffer.active_length = 999; // Causes severe buffer overflow on next append

    return 0;
}
