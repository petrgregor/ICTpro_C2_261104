/**
 * @file ex17_regression_underflow_fix.c
 * @brief Reference solution for Section 17 Exercise: TDD Regression Test for Unsigned Underflow.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HEADER_SIZE_BYTES 4U
#define MAX_DATA_BYTES    64U

// 1. BUGGY Production Code: Unsigned subtraction underflows if total_length < 4!
bool extract_payload_length_buggy(uint16_t total_length, uint16_t *out_payload_len) {
    // BUG: If total_length == 2, (uint16_t)(2 - 4) wraps around to 65534!
    uint16_t payload_len = (uint16_t)(total_length - HEADER_SIZE_BYTES);

    if (payload_len > MAX_DATA_BYTES) {
        // Note: In a naive implementation without upper bound check, 65534 bytes would be copied!
        // Here we simulate the bug where underflow bypasses a signed check or corrupts state:
    }
    *out_payload_len = payload_len;
    return (total_length <= (MAX_DATA_BYTES + HEADER_SIZE_BYTES)); // Faulty check!
}

// 2. FIXED Production Code: Validate lower bound BEFORE subtraction
bool extract_payload_length_fixed(uint16_t total_length, uint16_t *out_payload_len) {
    if (out_payload_len == nullptr) return false;

    // Strict lower and upper boundary validation before arithmetic
    if (total_length < HEADER_SIZE_BYTES ||
        total_length > (HEADER_SIZE_BYTES + MAX_DATA_BYTES)) {
        return false;
    }

    *out_payload_len = (uint16_t)(total_length - HEADER_SIZE_BYTES);
    return true;
}

// 3. Automated Regression Test for Issue #512
void test_regression_issue_512_header_underflow(
        bool (*sut_func)(uint16_t, uint16_t *),
        const char *label) {

    uint16_t calculated_len = 0U;
    const uint16_t malformed_total_length = 2U; // Smaller than 4-byte header!

    bool ok = sut_func(malformed_total_length, &calculated_len);

    if (ok) {
        printf("  [RED / FAIL]   %s: Accepted malformed frame (total_len=2) -> payload_len underflowed to %u!\n",
               label, calculated_len);
    } else {
        printf("  [GREEN / PASS] %s: Safely rejected malformed frame (total_len=2).\n",
               label);
    }
}

int main(void) {
    printf("=== Exercise 17: TDD Regression Test (Unsigned Underflow) ===\n\n");

    test_regression_issue_512_header_underflow(extract_payload_length_buggy, "Buggy Decoder");
    test_regression_issue_512_header_underflow(extract_payload_length_fixed, "Fixed Decoder");

    return 0;
}
