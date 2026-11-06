/**
 * @file 57_regression_test_demo.c
 * @brief Demonstration of TDD Regression Testing (Red -> Green) on a packet parser.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_PAYLOAD_LEN 8U

typedef struct {
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint8_t length;
} Frame;

// 1. BUGGY Implementation (Off-by-one error allowing length == 9)
bool frame_parse_payload_buggy(Frame *frame, const uint8_t *raw_stream, uint8_t len) {
    if (len > (MAX_PAYLOAD_LEN + 1U)) { // BUG: Permits len == 9!
        return false;
    }
    // Note: Clamped in demo to avoid crashing the host process while demonstrating logic fail
    for (uint8_t i = 0; i < len && i < MAX_PAYLOAD_LEN; ++i) {
        frame->payload[i] = raw_stream[i];
    }
    frame->length = len;
    return true;
}

// 2. PATCHED Safe Implementation
bool frame_parse_payload_fixed(Frame *frame, const uint8_t *raw_stream, uint8_t len) {
    if (len > MAX_PAYLOAD_LEN) { // Correct strict boundary check
        return false;
    }
    for (uint8_t i = 0; i < len; ++i) {
        frame->payload[i] = raw_stream[i];
    }
    frame->length = len;
    return true;
}

// Regression test runner for Issue #402
void run_regression_test_issue_402(bool (*parser_func)(Frame *, const uint8_t *, uint8_t),
                                   const char *version_name) {
    Frame frame = {0};
    const uint8_t attack_vector[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    bool accepted = parser_func(&frame, attack_vector, sizeof(attack_vector));

    if (accepted) {
        printf("  [RED / FAIL]   %s: Parser accepted 9-byte payload into 8-byte buffer!\n",
               version_name);
    } else {
        printf("  [GREEN / PASS] %s: Parser safely rejected oversized 9-byte payload.\n",
               version_name);
    }
}

int main(void) {
    printf("--- Section 17: Regression Testing (TDD Red-Green Workflow) ---\n\n");

    run_regression_test_issue_402(frame_parse_payload_buggy, "Buggy Parser (Before Fix)");
    run_regression_test_issue_402(frame_parse_payload_fixed, "Fixed Parser (After Fix) ");

    return 0;
}
