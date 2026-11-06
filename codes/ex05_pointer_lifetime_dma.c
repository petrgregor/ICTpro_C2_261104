/**
 * @file ex05_pointer_lifetime_dma.c
 * @brief Reference solution for Section 05 Exercise: Ownership, Borrowing & Provenance.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DMA_POOL_SIZE 128U

// Static storage duration: survives function returns for asynchronous DMA access
static uint8_t dma_tx_pool[DMA_POOL_SIZE];
static bool pool_in_use = false;

// 1. Immutable Borrow: Reads buffer to calculate XOR checksum, does not own or mutate
uint8_t compute_crc8(const uint8_t *data, size_t len) {
    if (data == nullptr) return 0U;
    uint8_t crc = 0U;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
    }
    return crc;
}

// 2. Provenance-safe address range validation using uintptr_t
bool is_ptr_in_dma_pool(const uint8_t *ptr) {
    if (ptr == nullptr) return false;

    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t start = (uintptr_t)&dma_tx_pool[0];
    uintptr_t end = (uintptr_t)&dma_tx_pool[DMA_POOL_SIZE];

    return (addr >= start) && (addr < end);
}

uint8_t* acquire_tx_slot(void) {
    if (pool_in_use) return nullptr;
    pool_in_use = true;
    return &dma_tx_pool[0];
}

// 3. Safe invalidation pattern using pointer-to-pointer
void release_tx_slot(uint8_t **slot_ptr) {
    if (slot_ptr != nullptr && *slot_ptr != nullptr) {
        if (is_ptr_in_dma_pool(*slot_ptr)) {
            pool_in_use = false;
            printf("  [Pool] DMA slot released back to static pool.\n");
        }
        // Immediately invalidate caller's pointer to prevent dangling/double-free
        *slot_ptr = nullptr;
    }
}

int main(void) {
    printf("=== Exercise 05: DMA Pointer Lifetime & Ownership ===\n\n");

    uint8_t stack_local_array[8] = {1, 2, 3, 4};
    uint8_t *dma_buf = acquire_tx_slot();

    printf("Is stack_local_array in DMA pool? %s\n",
           is_ptr_in_dma_pool(stack_local_array) ? "YES" : "NO (Rejected for DMA!)");
    printf("Is acquired dma_buf in DMA pool?  %s\n\n",
           is_ptr_in_dma_pool(dma_buf) ? "YES (Safe for async DMA)" : "NO");

    if (dma_buf != nullptr) {
        dma_buf[0] = 0x10;
        dma_buf[1] = 0x20;
        dma_buf[2] = 0x30;

        // Immutable borrow to compute checksum
        uint8_t crc = compute_crc8(dma_buf, 3);
        printf("Computed CRC8 (Immutable borrow): 0x%02X\n", crc);

        // Release ownership and nullify pointer
        release_tx_slot(&dma_buf);
        printf("Caller pointer after release_tx_slot: %p\n", (void *)dma_buf);
    }

    return 0;
}
