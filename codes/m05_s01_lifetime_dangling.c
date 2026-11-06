/**
 * @file 06_lifetime_dangling.c
 * @brief Demonstration of Object Lifetime and Dangling Pointers in Embedded C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 06_lifetime_dangling.c -o dangling_demo
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Mock DMA registers/state
static const uint8_t *dma_source_address = nullptr;
static bool dma_transfer_pending = false;

// Simulates starting a hardware DMA transfer asynchronously
void hardware_dma_start(const uint8_t *buffer) {
    dma_source_address = buffer;
    dma_transfer_pending = true;
    printf("[Hardware] DMA configured to read from address: %p\n", (const void*)buffer);
}

// --- The Anti-Pattern (Dangerous) ---
void process_sensor_bad(void) {
    // 'payload' has Automatic Storage Duration (allocated on the stack).
    // Its lifetime ends the exact moment this function returns!
    uint8_t payload[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    printf("[App] process_sensor_bad: Starting DMA with stack buffer %p\n", (void*)payload);
    hardware_dma_start(payload);

    // FUNCTION RETURNS: 'payload' is destroyed/invalidated.
    // DMA is still running in the background pointing to dead memory!
}

// --- The Correct Pattern (Safe) ---
// Use Static Storage Duration for buffers used in asynchronous hardware operations
static uint8_t safe_payload[4];

void process_sensor_good(void) {
    safe_payload[0] = 0xC0;
    safe_payload[1] = 0xFF;
    safe_payload[2] = 0xEE;
    safe_payload[3] = 0x00;

    printf("[App] process_sensor_good: Starting DMA with static buffer %p\n", (void*)safe_payload);
    hardware_dma_start(safe_payload);
    // FUNCTION RETURNS: 'safe_payload' stays valid forever.
}

// Simulates standard CPU execution happening while DMA is pending
void simulate_other_function_calls(void) {
    // This function call overwrites the stack memory previously used by process_sensor_bad
    volatile uint8_t garbage[16];
    for (int i = 0; i < 16; i++) {
        garbage[i] = 0xAA;
    }
    printf("[System] Executed other functions, overwriting CPU stack memory...\n");
}

int main(void) {
    printf("--- Lifetime and Dangling Pointer Demo ---\n\n");

    // 1. Trigger the bad pattern
    process_sensor_bad();
    simulate_other_function_calls();

    // Simulate DMA finally executing its read on the background
    if (dma_transfer_pending) {
        printf("[DMA ISR] Reading first byte: 0x%02X (Expected 0xDE, but got garbage!)\n\n",
               dma_source_address[0]);
        dma_transfer_pending = false;
    }

    // 2. Trigger the good pattern
    process_sensor_good();
    simulate_other_function_calls();

    if (dma_transfer_pending) {
        printf("[DMA ISR] Reading first byte: 0x%02X (Safely preserved!)\n",
               dma_source_address[0]);
        dma_transfer_pending = false;
    }

    return 0;
}
