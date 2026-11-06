/**
 * @file ex18_misra_ub_refactoring.c
 * @brief Reference solution for Section 18 Exercise: MISRA C Compliance & UB Elimination.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

// 1. MISRA-compliant parenthesized macro with explicit unsigned suffix
#define SCALE_OFFSET_SAFE ((int32_t)(10 + 2))

// 2. MISRA Rule 4.6 (fixed-width types), Rule 11.8 (const qualification), zero UB
bool calibrate_sensor_safe(const int32_t *raw_ptr, uint8_t shift, int32_t *out_result) {
    // Guard against null pointer dereference (UB)
    if (raw_ptr == nullptr || out_result == nullptr) {
        return false;
    }

    // Guard against invalid bit-shift width (UB if shift >= bit-width of type)
    if (shift >= 31U) {
        return false;
    }

    int32_t raw = *raw_ptr;

    // Pre-operation overflow check for multiplication (prevents signed integer overflow UB)
    if (raw > (INT32_MAX / SCALE_OFFSET_SAFE) || raw < (INT32_MIN / SCALE_OFFSET_SAFE)) {
        return false;
    }

    int32_t scaled = raw * SCALE_OFFSET_SAFE;

    // Perform left shift safely on non-negative values using unsigned arithmetic
    if (scaled < 0) {
        return false; // Shifting negative signed values left is Undefined Behavior in C
    }

    uint32_t shifted_u = ((uint32_t)scaled) << shift;
    if (shifted_u > (uint32_t)INT32_MAX) {
        return false; // Shift overflowed signed 32-bit range
    }

    *out_result = (int32_t)shifted_u;
    return true;
}

int main(void) {
    printf("=== Exercise 18: MISRA C & Undefined Behavior Refactoring ===\n\n");

    int32_t valid_raw = 100;
    int32_t calibrated = 0;

    if (calibrate_sensor_safe(&valid_raw, 2U, &calibrated)) {
        printf("  [OK] Valid input (100 * 12 << 2) = %d\n", calibrated);
    }

    // Test UB protection: shift >= 32
    if (!calibrate_sensor_safe(&valid_raw, 35U, &calibrated)) {
        printf("  [Blocked UB] Safely rejected invalid shift width (35 bits)!\n");
    }

    // Test UB protection: signed multiplication overflow
    int32_t huge_raw = 300000000;
    if (!calibrate_sensor_safe(&huge_raw, 1U, &calibrated)) {
        printf("  [Blocked UB] Safely rejected signed integer overflow before multiplication!\n");
    }

    return 0;
}
