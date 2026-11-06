/**
 * @file 55a_cmsis_cache_maintenance_demo.c
 * @brief Simulation of CMSIS L1 D-Cache maintenance (Clean & Invalidate) for DMA.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define BUFFER_SIZE 16U

// Simulated physical SRAM (accessed directly by DMA bus master)
static uint8_t physical_sram_tx[BUFFER_SIZE] = {0};
static uint8_t physical_sram_rx[BUFFER_SIZE] = {0};

// Simulated CPU L1 Data Cache views (Write-Back policy)
static uint8_t cpu_dcache_tx_view[BUFFER_SIZE] = {0};
static uint8_t cpu_dcache_rx_view[BUFFER_SIZE] = {0};

// --- Simulated CMSIS Core Cache Functions (ARM Cortex-M7) ---
void SCB_CleanDCache_by_Addr(uint32_t *addr, int32_t dsize) {
    // Flushes dirty lines from CPU L1 D-Cache down into physical SRAM
    if ((void *)addr == (void *)cpu_dcache_tx_view) {
        memcpy(physical_sram_tx, cpu_dcache_tx_view, (size_t)dsize);
    }
    printf("  [CMSIS] SCB_CleanDCache_by_Addr(%p, %d) -> Flushed dirty cache to physical RAM.\n",
           (void *)addr, dsize);
}

void SCB_InvalidateDCache_by_Addr(uint32_t *addr, int32_t dsize) {
    // Invalidates stale CPU L1 D-Cache lines, forcing a fresh reload from physical SRAM
    if ((void *)addr == (void *)cpu_dcache_rx_view) {
        memcpy(cpu_dcache_rx_view, physical_sram_rx, (size_t)dsize);
    }
    printf("  [CMSIS] SCB_InvalidateDCache_by_Addr(%p, %d) -> Discarded stale cache, fetched fresh RAM.\n",
           (void *)addr, dsize);
}

// --- Simulated Hardware DMA Engine & Application Callback ---
void hardware_dma_start_tx(const uint8_t *tx_buffer, size_t length) {
    (void)tx_buffer;
    // DMA bypasses CPU cache and reads directly from physical_sram_tx!
    printf("  [DMA HW TX] Bus Master transmitting from physical SRAM: \"%.*s\"\n",
           (int)length, (const char *)physical_sram_tx);
}

void process_incoming_packet(const uint8_t *rx_buffer, size_t length) {
    printf("  [CPU App]   Processed received packet from cache view: \"%.*s\"\n",
           (int)length, (const char *)rx_buffer);
}

// --- CMSIS Cache Maintenance Sequence ---
void dma_start_transmit(const uint8_t *tx_buffer, size_t length) {
    // 1. Flush modified CPU data from L1 D-Cache down into physical RAM
    SCB_CleanDCache_by_Addr((uint32_t *)tx_buffer, (int32_t)length);

    // 2. Safely trigger hardware DMA channel
    hardware_dma_start_tx(tx_buffer, length);
}

void dma_rx_complete_isr_callback(uint8_t *rx_buffer, size_t length) {
    // 1. Invalidate stale cache lines so CPU is forced to fetch fresh RAM
    SCB_InvalidateDCache_by_Addr((uint32_t *)rx_buffer, (int32_t)length);

    // 2. CPU safely reads received hardware data
    process_incoming_packet(rx_buffer, length);
}

int main(void) {
    printf("--- CMSIS Cache Maintenance Demo ---\n\n");

    // 1. TX Scenario: CPU prepares packet (lands in L1 D-Cache as dirty)
    printf("1. CPU -> DMA Transmit (TX) Path:\n");
    const char *tx_msg = "PING_STM32";
    memcpy(cpu_dcache_tx_view, tx_msg, strlen(tx_msg) + 1U);
    printf("  [CPU Write] Wrote \"%s\" into L1 D-Cache (Physical RAM still has: \"%s\").\n",
           cpu_dcache_tx_view, physical_sram_tx);

    dma_start_transmit(cpu_dcache_tx_view, strlen(tx_msg) + 1U);

    // 2. RX Scenario: DMA writes new data directly into physical SRAM while CPU cache has old data
    printf("\n2. DMA -> CPU Receive (RX) Path:\n");
    strcpy((char *)cpu_dcache_rx_view, "OLD_STALE_DATA");
    strcpy((char *)physical_sram_rx,   "ACK_FROM_SENSOR");
    printf("  [DMA HW RX] DMA wrote \"%s\" to physical SRAM (CPU Cache still holds: \"%s\").\n",
           physical_sram_rx, cpu_dcache_rx_view);

    dma_rx_complete_isr_callback(cpu_dcache_rx_view, strlen((const char *)physical_sram_rx) + 1U);

    return 0;
}
