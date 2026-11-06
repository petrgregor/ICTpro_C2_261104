/**
 * @file 10_restrict_optimization.c
 * @brief Demonstration of the 'restrict' keyword for memory optimization.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 10_restrict_optimization.c -o restrict_demo
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// WITHOUT 'restrict':
// The compiler assumes that 'output' and 'factor' MIGHT point to the same memory.
// Therefore, in the generated assembly, '*factor' is reloaded from RAM in EVERY loop iteration,
// just in case the previous write to 'output[i]' accidentally changed it.
void apply_gain_standard(int32_t *output, const int32_t *input, const int32_t *factor, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        output[i] = input[i] * (*factor);
    }
}

// WITH 'restrict':
// The programmer makes a strict promise that the memory accessed via 'output'
// does NOT overlap with the memory accessed via 'input' or 'factor'.
// Optimization: The compiler loads '*factor' into a fast CPU register exactly ONCE
// before the loop starts, drastically increasing execution speed.
void apply_gain_optimized(int32_t * restrict output,
                          const int32_t * restrict input,
                          const int32_t * restrict factor,
                          size_t len) {
    for (size_t i = 0; i < len; ++i) {
        output[i] = input[i] * (*factor);
    }
}

int main(void) {
    printf("--- Memory Optimization with 'restrict' ---\n\n");

    int32_t input_signal[5] = {10, -20, 30, -40, 50};
    int32_t output_signal[5] = {0};
    int32_t gain_factor = 3;

    // Valid usage: None of the provided memory blocks overlap.
    apply_gain_optimized(output_signal, input_signal, &gain_factor, 5);

    printf("Applied gain factor of %d:\n", gain_factor);
    for (size_t i = 0; i < 5; ++i) {
        printf("Sample %zu: In = %3d  ->  Out = %3d\n", i, input_signal[i], output_signal[i]);
    }

    // WARNING: If you break the 'restrict' promise by passing overlapping memory,
    // the result is Undefined Behavior!
    apply_gain_optimized(output_signal, input_signal, &gain_factor, 5); // DANGEROUS!
    printf("Applied gain factor of %d:\n", gain_factor);
    for (size_t i = 0; i < 5; ++i) {
        printf("Sample %zu: In = %3d  ->  Out = %3d\n", i, input_signal[i], output_signal[i]);
    }

    return 0;
}
