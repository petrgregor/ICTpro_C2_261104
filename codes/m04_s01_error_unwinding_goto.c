/**
 * @file 05_error_unwinding_goto.c
 * @brief Demonstration of the standard C error unwinding pattern using 'goto'.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 05_error_unwinding_goto.c -o unwinding_demo
 */

#include <stdio.h>
#include <stdlib.h>

// Mock structure representing a hardware peripheral
typedef struct {
    int id;
} HardwareResource;

// --- Mock functions to simulate hardware allocation and destruction ---

HardwareResource* resource_allocate_spi(void) {
    printf("[Mock] SPI peripheral allocated.\n");
    HardwareResource *res = (HardwareResource*)malloc(sizeof(HardwareResource));
    if (res) res->id = 1;
    return res;
}

HardwareResource* resource_allocate_dma(void) {
    printf("[Mock] DMA channel allocated.\n");
    HardwareResource *res = (HardwareResource*)malloc(sizeof(HardwareResource));
    if (res) res->id = 2;
    return res;
}

// C23: 'bool', 'true', and 'false' are native keywords. No need for <stdbool.h>.
bool hardware_initialize(HardwareResource *spi, HardwareResource *dma) {
    (void)spi; (void)dma;
    printf("[Mock] Hardware initialization encountered a fatal error!\n");
    return false; // Simulate a failure during initialization
}

void resource_free(HardwareResource *res) {
    if (res != nullptr) {
        printf("[Mock] Resource %d safely freed.\n", res->id);
        free(res);
    }
}

// --- End of Mock functions ---

/**
 * @brief Processes data requiring multiple hardware resources.
 *
 * Demonstrates the single-exit cleanup pattern using goto.
 * This prevents memory leaks and avoids deeply nested 'if' pyramids
 * when an error occurs halfway through resource acquisition.
 */
[[nodiscard]] bool process_sensor_stream(void) {
    bool success_flag = false;
    HardwareResource *spi_handle = nullptr;
    HardwareResource *dma_handle = nullptr;

    // 1. Acquire first resource
    spi_handle = resource_allocate_spi();
    if (spi_handle == nullptr) {
        goto cleanup_none;
    }

    // 2. Acquire second resource
    dma_handle = resource_allocate_dma();
    if (dma_handle == nullptr) {
        goto cleanup_spi;
    }

    // 3. Perform the core operation that might fail
    if (!hardware_initialize(spi_handle, dma_handle)) {
        // Jump directly to the appropriate cleanup block
        goto cleanup_all;
    }

    // If we reach this point, everything succeeded
    printf("Processing successful!\n");
    success_flag = true;

// Reverse-order cleanup blocks (Unwinding)
// Notice how execution falls through the labels naturally
    cleanup_all:
    resource_free(dma_handle);
    dma_handle = nullptr; // Defensive programming: prevent dangling pointers

    cleanup_spi:
    resource_free(spi_handle);
    spi_handle = nullptr;

    cleanup_none:
    return success_flag;
}

int main(void) {
    printf("--- Error Unwinding with 'goto' Demonstration ---\n\n");

    if (!process_sensor_stream()) {
        printf("\nStream processing aborted safely without memory leaks.\n");
    }

    return 0;
}
