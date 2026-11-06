/**
 * @file 01_c23_features.c
 * @brief Demonstration of modern C23 standard features.
 *
 * Compile with GCC/Clang: gcc -std=c2x -Wall -Wextra 01_c23_features.c -o c23_demo
 */

#include <stdio.h>
#include <stddef.h>

// 1. constexpr: Enforces strict compile-time evaluation.
// Unlike #define, this has a real type, scope, and can be type-checked by the compiler.
constexpr size_t BUFFER_CAPACITY = 1024;

// 2. Binary literals and digit separators: Greatly improves readability of bitmasks.
constexpr unsigned int HARDWARE_MASK = 0b1010'0000'1111'0000;

// Function demonstrating pointer safety and type deduction
// Note: In C23, 'bool' is a native keyword, <stdbool.h> is no longer strictly required.
bool process_sensor_data(const char *payload, size_t length) {
    // 3. nullptr: The new type-safe null pointer constant (replaces legacy NULL / (void*)0).
    // It is of type nullptr_t and cannot be accidentally converted to an integer.
    if (payload == nullptr) {
        printf("Error: Payload pointer is nullptr.\n");
        return false;
    }

    // 4. typeof: Deduces the type of a variable or expression at compile-time.
    // Extremely useful for macros or when dealing with generic programming.
    typeof(length) processed_bytes = 0;

    // Simulate processing
    while (processed_bytes < length) {
        // Do something with payload[processed_bytes]
        processed_bytes++;
    }

    return true;
}

int main(void) {
    printf("--- C23 Features Demonstration ---\n");
    printf("Buffer Capacity: %zu bytes\n", BUFFER_CAPACITY);
    printf("Hardware Mask: 0x%X\n", HARDWARE_MASK);

    // Test with valid pointer
    const char mock_data[] = "Sensors active";
    printf("\nProcessing valid data...\n");
    if (process_sensor_data(mock_data, sizeof(mock_data))) {
        printf("Data processed successfully.\n");
    }

    // Test with nullptr
    printf("\nProcessing invalid data...\n");
    if (!process_sensor_data(nullptr, 10)) {
        printf("Properly rejected invalid data.\n");
    }

    return 0; // Standard successful exit
}
