/**
 * @file 43_header_only_main.c
 * @brief Application instantiating the header-only library.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 43_header_only_main.c -o header_only
 */
#include <stdio.h>

// Defining this macro generates the actual binary code for the library here
#define TINY_LOGGER_IMPLEMENTATION
#include "m15_s02_tiny_logger.h"

int main(void) {
    printf("--- Header-only Library Pattern ---\n\n");

    tiny_log("Application started successfully.");
    tiny_log("Header-only implementation is working!");

    return 0;
}
