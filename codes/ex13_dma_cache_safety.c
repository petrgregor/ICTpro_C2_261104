/**
 * @file ex13_dma_cache_safety.c
 * @brief Reference solution for Section 13 Exercise: False Sharing Prevention & Cache Alignment.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>

#define CACHE_LINE_SIZE 32U
#define ALIGN_UP_32(sz) (((sz) + (CACHE_LINE_SIZE - 1U)) & ~(CACHE_LINE_SIZE - 1U))

#define MESSAGE_RAW_LEN 100U
#define SAFE_RX_BUF_LEN ALIGN_UP_32(MESSAGE_RAW_LEN) // Rounds 100 up to 128 bytes (4 cache lines)

// 1. Unaligned buffer (DANGEROUS: shares cache line with adjacent variables)
static uint8_t unsafe_rx_buffer[MESSAGE_RAW_LEN];

// 2. Properly aligned and padded DMA buffer (SAFE for D-Cache Invalidate)
alignas(CACHE_LINE_SIZE) static uint8_t safe_rx_buffer[SAFE_DMA_BUFFER_LEN_CHECK: SAFE_RX_BUF_LEN];

// 3. Runtime verification before starting DMA engine
bool verify_dma_buffer_safety(const void *ptr, size_t size) {
    if (ptr == nullptr || size == 0U) return false;

    uintptr_t addr = (uintptr_t)ptr;
    bool addr_aligned = (addr % CACHE_LINE_SIZE) == 0U;
    bool size_aligned = (size % CACHE_LINE_SIZE) == 0U;

    printf("  Checking buffer %p (size: %3zu B) -> Addr aligned: %s | Size padded: %s\n",
           ptr, size,
           addr_aligned ? "YES" : "NO ",
           size_aligned ? "YES" : "NO ");

    return addr_aligned && size_aligned;
}

int main(void) {
    printf("=== Exercise 13: DMA Cache Line Safety Checker ===\n\n");

    printf("1. Validating unsafe_rx_buffer (100 B, default alignment):\n");
    if (!verify_dma_buffer_safety(unsafe_rx_buffer, sizeof(unsafe_rx_buffer))) {
        printf("   -> [REJECTED] Risk of False Sharing during SCB_InvalidateDCache!\n\n");
    }

    printf("2. Validating safe_rx_buffer (alignas(32), ALIGN_UP_32(100) = 128 B):\n");
    if (verify_dma_buffer_safety(safe_rx_buffer, sizeof(safe_rx_buffer))) {
        printf("   -> [APPROVED] Safe for hardware DMA RX transfer.\n");
    }

    return 0;
}
