/**
 * @file 09_strict_aliasing_punning.c
 * @brief Demonstration of Strict Aliasing rules and safe Type Punning.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 09_strict_aliasing_punning.c -o punning
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 1. DANGEROUS: Strict Aliasing Violation
uint32_t float_to_bits_bad(float value) {
    // UNDEFINED BEHAVIOR: Casting float* to uint32_t* violates strict aliasing.
    // The optimizer assumes uint32_t* cannot alias with a float.
    // At -O2 or -O3, the compiler might ignore this memory read or reorder it incorrectly.
    return *(uint32_t *)&value;
}

// 2. SAFE APPROACH A: Using memcpy
uint32_t float_to_bits_safe_memcpy(float value) {
    uint32_t raw_bits;
    // Standard ISO C guarantees this safely copies the underlying byte representation.
    // Modern compilers completely optimize this out into a single, zero-cost CPU register move.
    memcpy(&raw_bits, &value, sizeof(raw_bits));
    return raw_bits;
}

// 3. SAFE APPROACH B: Using union
uint32_t float_to_bits_safe_union(float value) {
    union {
        float f;
        uint32_t u;
    } converter;

    converter.f = value;
    // C99/C11/C23 explicitly allows reading from a different union member
    // than the one most recently written to (Type Punning).
    return converter.u;
}

int main(void) {
    printf("--- Strict Aliasing and Type Punning ---\n\n");

    float test_val = -3.14159f;

    uint32_t bad_bits    = float_to_bits_bad(test_val);
    uint32_t memcpy_bits = float_to_bits_safe_memcpy(test_val);
    uint32_t union_bits  = float_to_bits_safe_union(test_val);

    printf("Original float value: %f\n\n", test_val);

    printf("Bad punning (Hex):    0x%08X (May break on -O2/-O3)\n", bad_bits);
    printf("Memcpy punning (Hex): 0x%08X (Safe & Optimal)\n", memcpy_bits);
    printf("Union punning (Hex):  0x%08X (Safe)\n", union_bits);

    return 0;
}
