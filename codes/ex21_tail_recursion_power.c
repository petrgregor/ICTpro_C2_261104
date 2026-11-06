/**
 * @file ex21_tail_recursion_power.c
 * @brief Reference solution for Section 21 Exercise: Tail-Call Optimized Power Function.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_RECURSION_DEPTH 32U

// 1. Tail-recursive helper: recursive call is the very last operation (TCO-friendly)
static uint64_t power_tail(uint32_t base, uint32_t exp, uint64_t accumulator) {
    if (exp == 0U) {
        return accumulator; // Base case
    }
    return power_tail(base, exp - 1U, accumulator * (uint64_t)base);
}

// 2. Public wrapper enforcing bounded stack depth
bool safe_power(uint32_t base, uint32_t exp, uint64_t *out_res) {
    if (out_res == nullptr || exp > MAX_RECURSION_DEPTH) {
        return false; // Prevent unbounded stack frame growth
    }
    *out_res = power_tail(base, exp, 1ULL);
    return true;
}

int main(void) {
    printf("=== Exercise 21: Tail-Recursive Power with Depth Guard ===\n\n");

    uint64_t result = 0ULL;
    if (safe_power(2U, 16U, &result)) {
        printf("  2^16 = %llu\n", (unsigned long long)result);
    }

    if (!safe_power(2U, 100U, &result)) {
        printf("  [Stack Guard] Safely rejected exponent 100 (exceeds MAX_RECURSION_DEPTH=%u)!\n",
               MAX_RECURSION_DEPTH);
    }

    return 0;
}
