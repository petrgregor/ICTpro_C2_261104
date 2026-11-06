/**
 * @file 28_type_erasure_alignment.c
 * @brief Type erasure using void* and strict memory alignment via alignas.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 28_type_erasure_alignment.c -o alignment
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// In C23, alignas and alignof are standard keywords.
// max_align_t guarantees that this buffer is properly aligned
// for ANY basic data type (including double or uint64_t).
alignas(max_align_t) uint8_t generic_storage[1024];

/**
 * @brief Swaps two memory blocks of arbitrary type safely.
 *
 * @param a Pointer to the first object
 * @param b Pointer to the second object
 * @param element_size The size of the objects in bytes
 */
void generic_swap(void *a, void *b, size_t element_size) {
    if (a == nullptr || b == nullptr || a == b) return;

    // Type erasure allows us to treat any object as a sequence of bytes
    uint8_t *byte_a = (uint8_t *)a;
    uint8_t *byte_b = (uint8_t *)b;

    for (size_t i = 0; i < element_size; ++i) {
        uint8_t temp = byte_a[i];
        byte_a[i] = byte_b[i];
        byte_b[i] = temp;
    }
}

int main(void) {
    printf("--- Type Erasure & Alignment (C23) ---\n\n");

    // alignof evaluates the strict alignment requirement
    printf("Maximum alignment requirement: %zu bytes\n\n", alignof(max_align_t));

    uint32_t val1 = 0x11223344;
    uint32_t val2 = 0xAABBCCDD;

    printf("Before swap: val1 = 0x%08X, val2 = 0x%08X\n", val1, val2);

    // We pass the addresses as void* and explicitly provide the size
    generic_swap(&val1, &val2, sizeof(uint32_t));

    printf("After swap:  val1 = 0x%08X, val2 = 0x%08X\n", val1, val2);

    return 0;
}
