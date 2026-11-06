/**
 * @file 54_rtos_sync_demo.c
 * @brief Simulation of RTOS Mutex protection with deterministic timeouts and lock hierarchy.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Mock FreeRTOS types and primitives for Host PC execution
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
    printf("  [RTOS] Acquired %s\n", mutex->name);
    return pdTRUE;
}

void xSemaphoreGive(SemaphoreHandle_t mutex) {
    if (mutex != nullptr) {
        mutex->is_locked = false;
        printf("  [RTOS] Released %s\n", mutex->name);
    }
}

static MockMutex i2c_mutex_storage = { .name = "I2C_Bus_Mutex", .is_locked = false };
static SemaphoreHandle_t i2c_bus_mutex = &i2c_mutex_storage;

bool i2c_bus_read_sensor(uint8_t dev_addr, uint8_t *dest) {
    const TickType_t wait_timeout = pdMS_TO_TICKS(50);

    if (xSemaphoreTake(i2c_bus_mutex, wait_timeout) != pdTRUE) {
        return false; // Graceful recovery on timeout
    }

    // Critical section: Safe access to shared I2C peripheral
    printf("  [I2C] Reading register from device 0x%02X...\n", dev_addr);
    *dest = 42U;

    xSemaphoreGive(i2c_bus_mutex);
    return true;
}

int main(void) {
    printf("--- Section 12: RTOS Tasks and Mutex Synchronization ---\n\n");

    uint8_t sensor_val = 0;

    printf("1. Normal access to free I2C bus:\n");
    i2c_bus_read_sensor(0x68, &sensor_val);

    printf("\n2. Simulating bus locked by another task (Timeout protection):\n");
    i2c_mutex_storage.is_locked = true; // Another task holds the lock
    if (!i2c_bus_read_sensor(0x68, &sensor_val)) {
        printf("  [App] Handled bus timeout safely without system freeze!\n");
    }

    return 0;
}
