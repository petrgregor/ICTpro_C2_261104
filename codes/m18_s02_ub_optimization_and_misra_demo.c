/**
 * @file 44b_ub_optimization_and_misra_demo.c
 * @brief UB signed overflow check elimination and MISRA C rules .
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

// --- Part 1: Code (UB Optimization Hazard) ---

// DANGEROUS: Signed integer overflow is Undefined Behavior!
// At -O2/-O3, GCC/Clang optimizes out the 'if' branch completely!
bool is_addition_safe_bad(int32_t current_val, int32_t delta) {
    if (current_val + delta < current_val) {
        return false; // Dead code under -O2/-O3!
    }
    return true;
}

// CORRECT: Check boundary before performing the operation
bool is_addition_safe_correct(int32_t current_val, int32_t delta) {
    if (delta > 0 && current_val > (INT32_MAX - delta)) {
        return false; // Arithmetic bounds validated safely without triggering UB
    }
    if (delta < 0 && current_val < (INT32_MIN - delta)) {
        return false;
    }
    return true;
}

// --- Part 2: Code from (MISRA C:2012 / 2023 Compliance) ---

// MISRA VIOLATION: Unparenthesized macro expression
#define BUFFER_LIMIT 100 + 20

// MISRA COMPLIANT: Explicit width, explicit const, protected macro expression
#define BUFFER_LIMIT_SAFE ((uint32_t)(100U + 20U))

int32_t process_data_safe(const uint8_t *payload, uint32_t length) {
    if (payload == nullptr || length > BUFFER_LIMIT_SAFE) {
        return -1;
    }
    int32_t sum = 0;
    for (uint32_t i = 0U; i < length; ++i) {
        sum += (int32_t)payload[i];
    }
    return sum;
}

int main(void) {
    printf("--- UB Optimization & MISRA C Demo ---\n\n");

    int32_t base = INT32_MAX - 10;
    int32_t add  = 50; // Will overflow INT32_MAX!

    printf("1. Safe pre-addition boundary check (INT32_MAX - 10 + 50):\n");
    printf("   is_addition_safe_correct() returned: %s\n\n",
           is_addition_safe_correct(base, add) ? "true (SAFE)" : "false (OVERFLOW PREVENTED!)");

    printf("2. Macro precedence trap (MISRA Rule 20.7):\n");
    printf("   2 * BUFFER_LIMIT      (100 + 20)   evaluates to: %d (BUG!)\n", 2 * BUFFER_LIMIT);
    printf("   2 * BUFFER_LIMIT_SAFE ((100U+20U)) evaluates to: %u (CORRECT)\n\n", 2U * BUFFER_LIMIT_SAFE);

    const uint8_t sample_data[3] = {10U, 20U, 30U};
    printf("3. MISRA-compliant function process_data_safe() result: %d\n",
           process_data_safe(sample_data, 3U));

    return 0;
}
