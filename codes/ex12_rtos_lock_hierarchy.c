/**
 * @file ex12_rtos_lock_hierarchy.c
 * @brief Reference solution for Section 12 Exercise: Lock Hierarchy & Event Group Sync.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Event Group bit definitions
#define EVENT_ADC_CALIBRATED  (1UL << 0)
#define EVENT_STORAGE_MOUNTED (1UL << 1)
#define EVENT_SYSTEM_READY    (EVENT_ADC_CALIBRATED | EVENT_STORAGE_MOUNTED)

typedef struct {
    const char *name;
    uint8_t hierarchy_level;
    bool locked;
} HierarchyMutex;

static HierarchyMutex spi_mutex    = { .name = "SPI_Mutex",    .hierarchy_level = 1, .locked = false };
static HierarchyMutex eeprom_mutex = { .name = "EEPROM_Mutex", .hierarchy_level = 2, .locked = false };
static uint32_t system_event_group = 0U;

bool mutex_take_with_timeout(HierarchyMutex *m, uint32_t timeout_ms) {
    if (m == nullptr || m->locked) {
        printf("  [RTOS] Failed to lock %s within %u ms!\n", m->name, timeout_ms);
        return false;
    }
    m->locked = true;
    printf("  [RTOS] Locked %s (Level %u)\n", m->name, m->hierarchy_level);
    return true;
}

void mutex_give(HierarchyMutex *m) {
    if (m != nullptr && m->locked) {
        m->locked = false;
        printf("  [RTOS] Unlocked %s (Level %u)\n", m->name, m->hierarchy_level);
    }
}

// Deadlock-free dual resource acquisition following strict global lock hierarchy (Level 1 -> Level 2)
bool safe_backup_sensor_to_eeprom(void) {
    // Check Event Group readiness first (WaitForAllBits equivalent)
    if ((system_event_group & EVENT_SYSTEM_READY) != EVENT_SYSTEM_READY) {
        printf("  [Sync] System not ready (Event bits: 0x%02X). Backup postponed.\n",
               system_event_group);
        return false;
    }

    // 1. Always acquire Level 1 (spi_mutex) first
    if (!mutex_take_with_timeout(&spi_mutex, 50U)) {
        return false;
    }

    // 2. Acquire Level 2 (eeprom_mutex) second
    if (!mutex_take_with_timeout(&eeprom_mutex, 50U)) {
        mutex_give(&spi_mutex); // Rollback Level 1 lock on timeout!
        return false;
    }

    printf("  [Action] Sensor data read via SPI and written to EEPROM safely.\n");

    // 3. Release in reverse order (Level 2 -> Level 1)
    mutex_give(&eeprom_mutex);
    mutex_give(&spi_mutex);
    return true;
}

int main(void) {
    printf("=== Exercise 12: RTOS Lock Hierarchy & Event Groups ===\n\n");

    printf("Attempt 1 (Only ADC calibrated, storage not mounted yet):\n");
    system_event_group |= EVENT_ADC_CALIBRATED;
    safe_backup_sensor_to_eeprom();

    printf("\nAttempt 2 (Both Event Group bits set):\n");
    system_event_group |= EVENT_STORAGE_MOUNTED;
    safe_backup_sensor_to_eeprom();

    return 0;
}
