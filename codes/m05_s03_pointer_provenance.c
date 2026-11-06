/**
 * @file 08_pointer_provenance.c
 * @brief Pointer Provenance and safe pointer comparisons for embedded memory regions.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static uint8_t rx_buffer[64];
static uint16_t tx_buffer[64];

// VALID: Both pointers derive from the same underlying array object
bool is_pointer_within_rx(const uint8_t *ptr) {
    return (ptr >= &rx_buffer[0]) && (ptr < &rx_buffer[64]);
}

// SAFE WAY: Cast to uintptr_t when comparing raw memory hardware addresses.
// Directly comparing independent arrays (p1 < p2) is Undefined Behavior in C.
bool compare_distinct_arrays(const uint8_t *p1, const uint16_t *p2) {
    // return (p1 < p2);  // Cannot apply binary operator< to const unsigned char * and const unsigned short *
    return ((uintptr_t)p1 < (uintptr_t)p2);
}

int main(void) {
    printf("--- Pointer Provenance Demo ---\n\n");

    const uint8_t *test_ptr_rx = &rx_buffer[32];

    printf("RX Buffer base: %p\n", (void*)rx_buffer);
    printf("TX Buffer base: %p\n", (void*)tx_buffer);

    // Safe relational check within the same object
    if (is_pointer_within_rx(test_ptr_rx)) {
        printf("test_ptr_rx is safely within rx_buffer bounds.\n");
    }

    // Comparing distinct objects safely
    if (compare_distinct_arrays(rx_buffer, tx_buffer)) {
        printf("In this compiler/linker layout, rx_buffer resides BEFORE tx_buffer in RAM.\n");
    } else {
        printf("In this compiler/linker layout, rx_buffer resides AFTER tx_buffer in RAM.\n");
    }

    return 0;
}
