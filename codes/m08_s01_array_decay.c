/**
 * @file 14_array_decay.c
 * @brief Demonstration of array decay and pointer arithmetic.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 14_array_decay.c -o decay
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// PITFALL: The compiler silently converts 'uint8_t buffer[64]' to 'uint8_t *buffer'.
// sizeof(buffer) will return the size of the pointer (4 or 8 bytes), NOT 64.
void evaluate_decay(uint8_t buffer[64]) {
    size_t decayed_size = sizeof(buffer);
    printf("[Decay] Size evaluated inside function: %zu bytes (Pointer size!)\n", decayed_size);
}

// SAFE APPROACH: Using a pointer to an array of a strictly defined size.
// The compiler enforces that exactly an array of 64 elements is passed.
void evaluate_fixed(const uint8_t (*buffer_ptr)[64]) {
    size_t exact_size = sizeof(*buffer_ptr);
    printf("[Fixed] Size evaluated via array pointer: %zu bytes (Safe!)\n", exact_size);
}

int main(void) {
    printf("--- Array Decay Demonstration ---\n\n");

    uint8_t actual_array[64] = {0};

    // sizeof in the same scope where the array is defined works correctly
    printf("[Main] Actual size of the array: %zu bytes\n", sizeof(actual_array));

    // Passing to a function triggers Array Decay
    evaluate_decay(actual_array);

    // Passing a pointer to the array retains type safety and size information
    evaluate_fixed(&actual_array);

    // Pointer arithmetic demonstration
    uint32_t word_array[4] = {10, 20, 30, 40};
    uint32_t *ptr = &word_array[0];

    printf("\n--- Pointer Arithmetic ---\n");
    printf("Base address (ptr):     %p\n", (void*)ptr);
    // Adding 1 to the pointer advances it by sizeof(uint32_t), which is 4 bytes
    printf("Next element (ptr + 1): %p\n", (void*)(ptr + 1));

    return 0;
}
