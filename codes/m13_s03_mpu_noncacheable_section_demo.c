/**
 * @file 55b_mpu_noncacheable_section_demo.c
 * @brief Demonstration of Non-Cacheable linker sections and MPU configuration.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// --- Simulated Hardware Ethernet DMA & MPU Setup ---
static void configure_mpu_noncacheable_region(const void *base_addr, size_t size) {
    printf("  [MPU Config] Configured region at %p (%zu bytes) as Normal, Non-Cacheable.\n",
           base_addr, size);
}

static void hardware_ethernet_dma_init(uint8_t *ring_buffer, size_t size) {
    uintptr_t addr = (uintptr_t)ring_buffer;
    bool is_32b_aligned = ((addr % 32U) == 0U);

    printf("  [ETH DMA]    Initialized ring buffer at 0x%zx (%zu bytes)\n",
           (size_t)addr, size);
    printf("  [ETH DMA]    32-byte alignment check: %s\n",
           is_32b_aligned ? "PASSED" : "FAILED");
}

// --- Non-Cacheable RAM Section & MPU ---
// Place DMA descriptors and circular buffers into dedicated non-cacheable RAM
// Section is defined in the microcontroller linker script (.ld)
#define DMA_BUFFER_SECTION __attribute__((section(".noncacheable_sram"), aligned(32)))

// Zero cache-maintenance overhead: Hardware coherency is guaranteed by MPU
DMA_BUFFER_SECTION static uint8_t network_dma_ring_buffer[1024];

void init_zero_overhead_dma(void) {
    configure_mpu_noncacheable_region(network_dma_ring_buffer, sizeof(network_dma_ring_buffer));

    // No SCB_CleanDCache or SCB_InvalidateDCache needed for this buffer!
    hardware_ethernet_dma_init(network_dma_ring_buffer, sizeof(network_dma_ring_buffer));
}

int main(void) {
    printf("--- Non-Cacheable Section & MPU Demo ---\n\n");

    init_zero_overhead_dma();

    // Direct write by CPU is immediately visible to DMA on the bus
    network_dma_ring_buffer[0] = 0xAAU;
    network_dma_ring_buffer[1] = 0x55U;
    printf("  [Zero Overhead] CPU wrote 0x%02X 0x%02X directly to non-cacheable SRAM.\n",
           network_dma_ring_buffer[0], network_dma_ring_buffer[1]);

    return 0;
}
