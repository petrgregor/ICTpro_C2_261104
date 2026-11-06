/**
 * @file 20_recursion_types.c
 * @brief Demonstration of direct and indirect recursion.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 20_recursion_types.c -o recursion_types
 */
#include <stdio.h>
#include <stdint.h>

// --- 1. DIRECT RECURSION ---
// The function calls itself directly within its own body.
void countdown_direct(uint32_t value) {
    if (value == 0) {
        printf("Direct countdown reached zero!\n\n");
        return;
    }
    printf("Direct: %u\n", value);
    countdown_direct(value - 1);
}

// --- 2. INDIRECT RECURSION ---
// Forward declaration is required because process_odd calls process_even and vice versa.
void process_even(uint32_t value);

void process_odd(uint32_t value) {
    if (value == 0) {
        printf("Indirect recursion finished in process_odd.\n\n");
        return;
    }
    printf("Odd function processing: %u\n", value);

    // Calls the counterpart function
    process_even(value - 1);
}

void process_even(uint32_t value) {
    if (value == 0) {
        printf("Indirect recursion finished in process_even.\n\n");
        return;
    }
    printf("Even function processing: %u\n", value);

    // Calls the counterpart function, completing the indirect cycle
    process_odd(value - 1);
}

int main(void) {
    printf("--- Direct vs Indirect Recursion ---\n\n");

    printf("Starting direct countdown (5):\n");
    countdown_direct(5);

    printf("Starting indirect countdown (5):\n");
    process_odd(5);

    return 0;
}
