/**
 * @file 21_recursion_base_case.c
 * @brief Demonstration of the Base Case and memory limits (Stack Overflow risk).
 *
 * Compile: gcc -std=c2x -Wall -Wextra 21_recursion_base_case.c -o base_case
 */
#include <stdio.h>
#include <stdint.h>

/**
 * @brief Calculates the factorial of a number using standard recursion.
 *
 * NOTE ON EMBEDDED SYSTEMS:
 * Each recursive call pushes a new "Stack Frame" onto the CPU stack.
 * This includes the return address, local variables, and saved registers.
 * If 'n' is too large, the MCU stack (often just a few KB) will overflow,
 * causing a HardFault or silent data corruption.
 * This is why MISRA C explicitly forbids recursion.
 */
uint32_t factorial(uint32_t n) {
    // BASE CASE: The mandatory stop condition for the recursion.
    if (n <= 1) {
        return 1;
    }

    // RECURSIVE CASE:
    // The CPU must keep the current state in memory because it has to wait
    // for factorial(n - 1) to finish before it can perform the multiplication.
    return n * factorial(n - 1);
}

/*
// ANTI-PATTERN: Infinite Recursion (No Base Case)
// Uncommenting and running this will crash the program with a Segmentation Fault
// (or HardFault on an MCU) because it completely drains the stack memory.
void dangerous_infinite_recursion(uint32_t n) {
    printf("Consuming stack... %u\n", n);
    dangerous_infinite_recursion(n + 1);
}
*/

int main(void) {
    printf("--- Recursion Base Case & Stack Memory ---\n\n");

    uint32_t num = 5;
    uint32_t result = factorial(num);

    printf("Factorial of %u is %u\n", num, result);
    printf("(This operation consumed 5 stack frames simultaneously.)\n");

    // dangerous_infinite_recursion(1); // Do not run!

    return 0;
}
