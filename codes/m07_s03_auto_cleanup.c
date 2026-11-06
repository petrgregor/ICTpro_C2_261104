/**
 * @file 13_auto_cleanup.c
 * @brief Demonstration of GCC/Clang __attribute__((cleanup)) for RAII in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 13_auto_cleanup.c -o cleanup
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Callback automatically invoked by the compiler when the variable goes out of scope.
 * @param ptr Pointer to the variable (so it's a pointer to a pointer).
 */
void autofree_callback(void *ptr) {
    void **p = (void **)ptr;
    if (*p != nullptr) {
        printf("[AutoCleanup] Freeing dynamically allocated memory at %p...\n", *p);
        free(*p);
        *p = nullptr; // Nullify to prevent double-free
    }
}

// Macro to wrap the compiler extension cleanly
#define AUTO_FREE __attribute__((cleanup(autofree_callback)))

/**
 * @brief Simulates a function with multiple exit points (e.g., error handling).
 */
void process_telemetry(bool simulate_early_error) {
    printf("\n--- Starting telemetry processing ---\n");

    // The compiler registers 'autofree_callback' to be called on 'buffer'
    // immediately before this function returns, no matter HOW it returns.
    AUTO_FREE uint8_t *buffer = (uint8_t *)malloc(1024);

    if (buffer == nullptr) {
        printf("Failed to allocate memory.\n");
        return;
    }
    printf("Buffer allocated successfully at %p.\n", (void*)buffer);

    if (simulate_early_error) {
        printf("Error detected! Returning early...\n");
        // No explicit free() needed here! The compiler injects it.
        return;
    }

    printf("Processing completed successfully.\n");
    // No explicit free() needed here either!
}

int main(void) {
    printf("--- RAII Imitation using GCC/Clang Cleanup Attribute ---\n");

    // Test Case 1: Normal execution
    process_telemetry(false);

    // Test Case 2: Execution interrupted by an early return (error path)
    process_telemetry(true);

    return 0;
}
