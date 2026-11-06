/**
 * @file 41_modular_main.c
 * @brief Main application using the external module.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 41_math_utils.c 41_modular_main.c -o modular
 */
#include <stdio.h>
#include "m15_s01_math_utils.h"

int main(void) {
    printf("--- Modularity and #pragma once ---\n\n");

    int32_t result = math_add(15, 27);
    printf("Result from external module: %d\n", result);

    return 0;
}
