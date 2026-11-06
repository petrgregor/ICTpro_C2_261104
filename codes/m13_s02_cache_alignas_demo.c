/**
 * @file 55_cache_alignas_demo.c
 * @brief DMA buffer alignment to 32-byte Cache Line boundary using alignas.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

#define CACHE_LINE_SIZE 32U

// Macro to round up buffer size to the nearest multiple of cache line size (32B)
#define ALIGN_UP_CACHE(size) \
    (((size) + (CACHE_LINE_SIZE - 1U)) & ~(CACHE_LINE_SIZE - 1U))

#define RAW_PAYLOAD_SIZE 70U
#define SAFE_DMA_BUFFER_SIZE ALIGN_UP_CACHE(RAW_PAYLOAD_SIZE) // Evaluates to 96 bytes

// Strictly aligned to 32-byte boundary and padded to full cache lines
alignas(CACHE_LINE_SIZE) static uint8_t dma_rx_storage[SAFE_DMA_BUFFER_SIZE];

// Simulated CMSIS Cache Maintenance API
void SCB_CleanDCache_by_Addr(uint32_t *addr, int32_t dsize) {
    printf("[CMSIS] Cleaned D-Cache at %p (%d bytes) -> Flushed to physical RAM.\n",
           (void *)addr, dsize);
}

void SCB_InvalidateDCache_by_Addr(uint32_t *addr, int32_t dsize) {
    printf("[CMSIS] Invalidated D-Cache at %p (%d bytes) -> Ready to read fresh DMA data.\n",
           (void *)addr, dsize);
}

int main(void) {
    printf("--- Section 13: Cache, DMA and Data Alignment ---\n\n");

    uintptr_t addr = (uintptr_t)dma_rx_storage;
    printf("Raw payload size:          %u bytes\n", RAW_PAYLOAD_SIZE);
    printf("Padded DMA buffer size:    %u bytes (Multiple of 32B: %s)\n",
           SAFE_DMA_BUFFER_SIZE, (SAFE_DMA_BUFFER_SIZE % 32U == 0) ? "YES" : "NO");
    printf("Buffer memory address:     0x%zx (Aligned to 32B: %s)\n\n",
           (size_t)addr, (addr % 32U == 0) ? "YES" : "NO");

    SCB_CleanDCache_by_Addr((uint32_t *)dma_rx_storage, (int32_t)SAFE_DMA_BUFFER_SIZE);
    SCB_InvalidateDCache_by_Addr((uint32_t *)dma_rx_storage, (int32_t)SAFE_DMA_BUFFER_SIZE);

    return 0;
}
