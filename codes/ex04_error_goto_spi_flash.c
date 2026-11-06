/**
 * @file ex04_error_goto_spi_flash.c
 * @brief Reference solution for Section 04 Exercise: Safe Unwinding with [[nodiscard]] and goto.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    FLASH_OK = 0,
    FLASH_ERR_ALLOC = -1,
    FLASH_ERR_BUS_LOCK = -2,
    FLASH_ERR_VERIFY = -3
} FlashStatus;

// Simulated low-level hardware resource functions
static bool spi_bus_acquire(void) {
    printf("  [HW] SPI bus locked.\n");
    return true;
}
static void spi_bus_release(void) {
    printf("  [HW] SPI bus unlocked.\n");
}
static void gpio_cs_assert_low(void) {
    printf("  [HW] CS pin asserted LOW (active).\n");
}
static void gpio_cs_deassert_high(void) {
    printf("  [HW] CS pin deasserted HIGH (inactive).\n");
}

// C23 [[nodiscard]] enforces caller to inspect initialization status
[[nodiscard]] FlashStatus flash_storage_mount(bool simulate_verify_fail) {
    FlashStatus status = FLASH_OK;

    // Step 1: Allocate page cache buffer
    uint8_t *page_cache = (uint8_t *)malloc(256U);
    if (page_cache == nullptr) {
        status = FLASH_ERR_ALLOC;
        goto cleanup_none;
    }
    printf("  [Mem] Page cache allocated.\n");

    // Step 2: Acquire SPI bus lock
    if (!spi_bus_acquire()) {
        status = FLASH_ERR_BUS_LOCK;
        goto cleanup_cache;
    }

    // Step 3: Activate Chip Select
    gpio_cs_assert_low();

    // Step 4: Verify JEDEC Flash ID
    if (simulate_verify_fail) {
        printf("  [Error] JEDEC ID mismatch (0xFF != 0xEF)!\n");
        status = FLASH_ERR_VERIFY;
        goto cleanup_all;
    }

    printf("  [Success] Flash mounted and verified!\n");

// Reverse-order resource unwinding
    cleanup_all:
    gpio_cs_deassert_high();
    spi_bus_release();
    cleanup_cache:
    if (status != FLASH_OK) {
        free(page_cache);
        printf("  [Mem] Page cache freed due to failure.\n");
    } else {
        // In a real driver, page_cache would be stored in context; free here for demo cleanliness
        free(page_cache);
    }
    cleanup_none:
    return status;
}

int main(void) {
    printf("=== Exercise 04: SPI Flash Mount Unwinding ===\n\n");

    printf("Test 1: Simulated JEDEC ID verification failure:\n");
    FlashStatus res1 = flash_storage_mount(true);
    printf("Result code: %d\n\n", res1);

    printf("Test 2: Normal successful mount:\n");
    FlashStatus res2 = flash_storage_mount(false);
    printf("Result code: %d\n", res2);

    return 0;
}
