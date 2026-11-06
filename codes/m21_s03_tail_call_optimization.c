/**
 * @file 22_tail_call_optimization.c
 * @brief Demonstration of Tail Recursion and Tail Call Optimization (TCO).
 *
 * Compile with optimization to enable TCO:
 * gcc -std=c2x -Wall -Wextra -O2 22_tail_call_optimization.c -o tail_call
 */
#include <stdio.h>
#include <stdint.h>

// 1. STANDARD RECURSION (No TCO possible)
// The multiplication happens AFTER the recursive call returns.
// The compiler MUST keep the current stack frame alive to perform the multiplication later.
uint32_t factorial_standard(uint32_t n) {
    if (n <= 1) return 1;
    return n * factorial_standard(n - 1);
}

// 2. TAIL RECURSION (TCO capable)
// The 'accumulator' holds the running total.
// The recursive call is the ABSOLUTE LAST operation in the function.
// An optimizing compiler (-O2, -O3) will not create new stack frames.
// It will reuse the current frame and turn this into a simple assembly loop (JMP).
uint32_t factorial_tail(uint32_t n, uint32_t accumulator) {
    if (n <= 1) {
        return accumulator; // BASE CASE
    }

    // TAIL CALL: Nothing else to do after this returns.
    return factorial_tail(n - 1, n * accumulator);
}

// Wrapper to hide the accumulator from the API user
uint32_t calculate_factorial_safe(uint32_t n) {
    return factorial_tail(n, 1);
}

int main(void) {
    printf("--- Tail Call Optimization (TCO) ---\n\n");

    uint32_t val = 6;

    uint32_t result_std = factorial_standard(val);
    uint32_t result_tail = calculate_factorial_safe(val);

    printf("Standard factorial of %u: %u (Consumed %u stack frames)\n", val, result_std, val);
    printf("Tail-rec factorial of %u: %u (Consumed 1 stack frame at -O2)\n", val, result_tail);

    return 0;
}
