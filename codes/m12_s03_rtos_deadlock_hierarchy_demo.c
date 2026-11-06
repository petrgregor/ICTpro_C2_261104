/**
 * @file 54b_rtos_deadlock_hierarchy_demo.c
 * @brief Demonstration of Deadlock prevention via strict global lock hierarchy.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// --- Simulated FreeRTOS Primitives for Host PC Execution ---
typedef uint32_t TickType_t;
#define pdTRUE 1
#define pdFALSE 0
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))

typedef struct {
    const char *name;
    bool is_locked;
} MockMutex;

typedef MockMutex* SemaphoreHandle_t;

int xSemaphoreTake(SemaphoreHandle_t mutex, TickType_t timeout_ticks) {
    if (mutex == nullptr || mutex->is_locked) {
        printf("  [RTOS] Timeout (%u ticks) waiting for %s!\n", timeout_ticks, mutex->name);
        return pdFALSE;
    }
    mutex->is_locked = true;
    printf("  [RTOS] Locked %s\n", mutex->name);
    return pdTRUE;
}

void xSemaphoreGive(SemaphoreHandle_t mutex) {
    if (mutex != nullptr && mutex->is_locked) {
        mutex->is_locked = false;
        printf("  [RTOS] Unlocked %s\n", mutex->name);
    }
}

// --- Code from Slide 6: Global Lock Hierarchy ---
static MockMutex flash_mutex_obj = { .name = "flash_mutex (Level 1)", .is_locked = false };
static MockMutex uart_mutex_obj  = { .name = "uart_mutex  (Level 2)", .is_locked = false };

static SemaphoreHandle_t flash_mutex = &flash_mutex_obj;
static SemaphoreHandle_t uart_mutex  = &uart_mutex_obj;

// SAFE ARCHITECTURE: Strict Global Lock Hierarchy
// Rule: Always acquire flash_mutex BEFORE uart_mutex system-wide!
void log_critical_telemetry(const uint8_t *payload, size_t size) {
    if (xSemaphoreTake(flash_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            // Write to both storage and communication channels safely
            printf("  [Telemetry] Safely wrote %zu bytes to Flash and UART.\n", size);
            (void)payload;
            xSemaphoreGive(uart_mutex);
        } else {
            printf("  [Recovery] Could not lock UART, releasing Flash lock to avoid deadlock.\n");
        }
        xSemaphoreGive(flash_mutex);
    }
}

int main(void) {
    printf("--- Deadlock Prevention & Lock Hierarchy ---\n\n");

    const uint8_t telemetry_data[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    printf("1. Executing log_critical_telemetry() with both buses free:\n");
    log_critical_telemetry(telemetry_data, sizeof(telemetry_data));

    printf("\n2. Simulating busy UART bus (verifying safe rollback of flash_mutex):\n");
    uart_mutex_obj.is_locked = true;
    log_critical_telemetry(telemetry_data, sizeof(telemetry_data));
    uart_mutex_obj.is_locked = false;

    return 0;
}
