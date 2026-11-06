/**
 * @file 26_generic_keyword.c
 * @brief Demonstration of the _Generic keyword for type-based dispatch.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 26_generic_keyword.c -o generic
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Specific backend functions that perform the actual work
void print_int(int x) {
    printf("Integer processed: %d\n", x);
}

void print_float(float x) {
    printf("Float processed: %f\n", x);
}

void print_string(const char *x) {
    printf("String processed: %s\n", x);
}

// Generic macro front-end acting as a compile-time switch
#define print_data(X) _Generic((X), \
    int: print_int,                 \
    float: print_float,             \
    char*: print_string,            \
    const char*: print_string,      \
    default: print_int              \
)(X)

int main(void) {
    printf("--- _Generic Keyword Demonstration ---\n\n");

    // The compiler strictly evaluates the type of the argument
    // and injects the corresponding function call.

    print_data(42);         // Dispatches to print_int
    print_data(3.14159f);   // Dispatches to print_float
    print_data("Hello C!"); // Dispatches to print_string

    return 0;
}
