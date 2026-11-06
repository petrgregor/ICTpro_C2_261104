/**
 * @file 35_bitfields.c
 * @brief Demonstration of struct bit-fields for memory optimization.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 35_bitfields.c -o bitfields
 */
#include <stdio.h>
#include <stdint.h>

// Packing multiple variables into exactly 1 byte (8 bits)
typedef struct {
    uint8_t system_ready  : 1; // 1 bit
    uint8_t error_active  : 1; // 1 bit
    uint8_t mode_selector : 3; // 3 bits (values 0 to 7)
    uint8_t sensor_status : 2; // 2 bits (values 0 to 3)
    uint8_t reserved      : 1; // 1 bit padding
} ControllerStatus;

int main(void) {
    printf("--- Bit-fields Demonstration ---\n\n");

    ControllerStatus status = {0}; // Initialize all bits to 0

    status.system_ready = 1;
    status.mode_selector = 5;

    // The compiler handles the complex bit-masking automatically
    printf("Size of ControllerStatus: %zu byte(s)\n", sizeof(ControllerStatus));
    printf("System Ready:  %u\n", status.system_ready);
    printf("Mode Selector: %u\n", status.mode_selector);

    // ERROR: You cannot take the address of a bit-field!
    // uint8_t *ptr = &status.system_ready; // This would cause a compilation error

    return 0;
}
