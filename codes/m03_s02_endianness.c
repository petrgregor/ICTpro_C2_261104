/**
 * @file 36_endianness.c
 * @brief Runtime detection of endianness and byte swapping.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 36_endianness.c -o endianness
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Checks if the current CPU architecture is Little-Endian.
 */
bool is_little_endian(void) {
    uint16_t test_value = 0x0001;
    // Cast the pointer to a byte pointer and read the first byte
    return (*(uint8_t *)&test_value == 1);
}

int main(void) {
    printf("--- Endianness & Conversion ---\n\n");

    if (is_little_endian()) {
        printf("Host Architecture: LITTLE-ENDIAN\n\n");
    } else {
        printf("Host Architecture: BIG-ENDIAN\n\n");
    }

    uint32_t original_value = 0xAABBCCDD;
    printf("Original 32-bit value: 0x%08X\n", original_value);

    // Using GCC/Clang compiler built-in function for extreme efficiency.
    // In production, you might use POSIX htonl()/ntohl() from <arpa/inet.h>.
    uint32_t swapped_value = __builtin_bswap32(original_value);

    printf("Swapped 32-bit value:  0x%08X\n", swapped_value);

    return 0;
}
