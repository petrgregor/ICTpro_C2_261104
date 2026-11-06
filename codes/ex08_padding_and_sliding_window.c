/**
 * @file ex08_padding_and_sliding_window.c
 * @brief Reference solution for Section 08 Exercise: Struct Padding & memmove Sliding Window.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// 1A. Unoptimized layout: excessive compiler padding (typically 24 bytes on 64-bit / 32-bit aligned)
typedef struct {
    uint8_t flags;      // 1B + 7B padding
    uint64_t timestamp; // 8B
    uint8_t id;         // 1B + 3B padding
    uint32_t crc;       // 4B
    uint16_t len;       // 2B + 6B trailing padding
} BadHeader;

// 1B. Optimized layout: ordered from largest to smallest type (16 bytes total, zero padding)
typedef struct {
    uint64_t timestamp; // 8B (offset 0)
    uint32_t crc;       // 4B (offset 8)
    uint16_t len;       // 2B (offset 12)
    uint8_t flags;      // 1B (offset 14)
    uint8_t id;         // 1B (offset 15)
} GoodHeader;

// 2. Overlapping memory shift using memmove (memcpy would be Undefined Behavior here!)
void sliding_window_append(uint8_t *win, size_t win_size, uint8_t new_sample) {
    if (win == nullptr || win_size == 0U) return;

    if (win_size > 1U) {
        // Shift elements [1 .. win_size-1] left to [0 .. win_size-2]
        memmove(&win[0], &win[1], win_size - 1U);
    }
    win[win_size - 1U] = new_sample;
}

int main(void) {
    printf("=== Exercise 08: Struct Padding & Sliding Window ===\n\n");

    printf("sizeof(BadHeader):  %zu bytes\n", sizeof(BadHeader));
    printf("sizeof(GoodHeader): %zu bytes (Saved %zu bytes per struct!)\n\n",
           sizeof(GoodHeader), sizeof(BadHeader) - sizeof(GoodHeader));

    uint8_t window[5] = {10, 20, 30, 40, 50};
    printf("Initial window: %u %u %u %u %u\n",
           window[0], window[1], window[2], window[3], window[4]);

    sliding_window_append(window, sizeof(window), 60);
    sliding_window_append(window, sizeof(window), 70);

    printf("After 2 shifts: %u %u %u %u %u\n",
           window[0], window[1], window[2], window[3], window[4]);

    return 0;
}
