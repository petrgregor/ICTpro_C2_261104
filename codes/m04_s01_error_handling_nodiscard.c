/**
 * @file 04_error_handling_nodiscard.c
 * @brief Demonstration of C23 [[nodiscard]] attribute and standard return codes.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 04_error_handling_nodiscard.c -o err_demo
 */

#include <stdio.h>
#include <stdint.h>

// Standard practice: Define descriptive error codes as an enumeration
typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_INVALID_PARAM = -1,
    STATUS_ERR_TIMEOUT = -2,
    STATUS_ERR_HARDWARE = -3
} ErrorStatus;

/**
 * @brief Reads temperature from a specific sensor.
 *
 * C23 Feature: The [[nodiscard]] attribute forces the caller to check the
 * return value. If the caller ignores it, the compiler will emit a warning.
 *
 * @param sensor_id ID of the sensor to read.
 * @param out_temp  Pointer to store the resulting temperature.
 * @return ErrorStatus indicating success or specific failure reason.
 */
[[nodiscard]] ErrorStatus sensor_read_temperature(uint8_t sensor_id, float *out_temp) {
    // C23: nullptr natively replaces the legacy (void*)0 NULL macro
    if (sensor_id > 10 || out_temp == nullptr) {
        return STATUS_ERR_INVALID_PARAM;
    }

    // Simulate a successful hardware read
    *out_temp = 25.4f;
    return STATUS_OK;
}

int main(void) {
    printf("--- C23 [[nodiscard]] Demonstration ---\n");
    float current_temp = 0.0f;

    // CORRECT USAGE: Explicitly checking the return value of a [[nodiscard]] function
    ErrorStatus status = sensor_read_temperature(5, &current_temp);
    if (status == STATUS_OK) {
        printf("Temperature: %.1f C\n", current_temp);
    } else {
        printf("Failed to read sensor. Error code: %d\n", status);
    }

    // INCORRECT USAGE: Ignoring the return value.
    // Uncommenting the line below will cause a compiler warning like:
    // "warning: ignoring return value of function declared with 'nodiscard' attribute"

    sensor_read_temperature(15, &current_temp);

    return 0;
}
