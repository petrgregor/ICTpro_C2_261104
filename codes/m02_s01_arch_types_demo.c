/**
 * @file 51_arch_types_demo.c
 * @brief Demonstration of data type sizes across architectures and <stdint.h>.
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void) {
    printf("--- Section 02: Data Types Across Architectures ---\n\n");

    // Inspect sizes on the current host platform (e.g., LLP64 on Windows x64)
    printf("Size of int:       %zu bytes\n", sizeof(int));
    printf("Size of long:      %zu bytes\n", sizeof(long));
    printf("Size of long long: %zu bytes\n", sizeof(long long));
    printf("Size of void*:     %zu bytes\n\n", sizeof(void *));

    // Simulating a 16-bit architecture (AVR / MSP430) where int is 16-bit
    int16_t simulated_16bit_int = 32767; // INT16_MAX
    // Note: Unsigned cast used here only to avoid UB on host while showing wraparound
    int16_t overflowed_val = (int16_t)((uint16_t)simulated_16bit_int + 1U);
    printf("[16-bit MCU Simulation] 32767 + 1 overflows to: %d\n", overflowed_val);

    // SAFE approach using fixed-width integers from <stdint.h>
    uint32_t precise_sensor_reading = 45000U;
    uintptr_t memory_address = (uintptr_t)&precise_sensor_reading;

    printf("[Safe <stdint.h>] Reading: %u stored at address: 0x%zx\n",
           precise_sensor_reading, (size_t)memory_address);

    return 0;
}
