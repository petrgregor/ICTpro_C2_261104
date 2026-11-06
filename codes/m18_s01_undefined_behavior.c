/**
 * @file 44_undefined_behavior.c
 * @brief Demonstration of common Undefined Behavior (UB) pitfalls in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -pedantic 44_undefined_behavior.c -o ub_demo
 * WARNING: Executing this program invokes Undefined Behavior!
 */
#include <stdio.h>
#include <limits.h>

void trigger_buffer_overflow(void) {
    int data_array[3] = {10, 20, 30};

    // UB: Accessing memory outside the allocated bounds of the array
    printf("[UB] Out of bounds read: %d\n", data_array[5]);
}

void trigger_integer_overflow(void) {
    int max_val = INT_MAX;

    // UB: Signed integer overflow. In C, wrapping around to negative is NOT guaranteed.
    max_val = max_val + 1;
    printf("[UB] Integer overflow result: %d\n", max_val);
}

void trigger_uninitialized_read(void) {
    int uninitialized_var;

    // UB: Reading an uninitialized local variable (indeterminate value)
    printf("[UB] Uninitialized variable: %d\n", uninitialized_var);
}

int main(void) {
    printf("--- Undefined Behavior Demonstration ---\n\n");

    // In a real application, these functions could cause catastrophic failures
    trigger_buffer_overflow();
    trigger_integer_overflow();
    trigger_uninitialized_read();

    return 0;
}
