/**
 * @file 58_bounded_array_demo.c
 * @brief Bounded Dynamic Array with fixed static capacity (Zero heap usage).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define SENSOR_ARRAY_MAX_CAPACITY 4U // Small capacity to easily demonstrate overflow protection

typedef struct {
    int16_t temperature_raw;
    uint32_t timestamp_ms;
} SensorRecord;

typedef struct {
    SensorRecord items[SENSOR_ARRAY_MAX_CAPACITY];
    size_t count;
} BoundedSensorArray;

bool bounded_array_push(BoundedSensorArray *array, SensorRecord record) {
    if (array == nullptr || array->count >= SENSOR_ARRAY_MAX_CAPACITY) {
        return false; // Deterministic overflow rejection (No silent heap crash)
    }
    array->items[array->count] = record;
    array->count++;
    return true;
}

bool bounded_array_pop(BoundedSensorArray *array, SensorRecord *out_record) {
    if (array == nullptr || out_record == nullptr || array->count == 0) {
        return false; // Underflow protection
    }
    array->count--;
    *out_record = array->items[array->count];
    return true;
}

int main(void) {
    printf("--- Section 09: Bounded Dynamic Array Demo ---\n\n");

    BoundedSensorArray telemetry_log = { .count = 0 };

    // Try pushing 5 records into a 4-element bounded array
    for (uint32_t i = 1; i <= 5; ++i) {
        SensorRecord rec = { .temperature_raw = (int16_t)(200 + i * 5), .timestamp_ms = i * 100U };
        if (bounded_array_push(&telemetry_log, rec)) {
            printf("  [PUSH OK]   Record #%u stored (Temp: %d, Time: %u ms)\n",
                   i, rec.temperature_raw, rec.timestamp_ms);
        } else {
            printf("  [REJECTED]  Record #%u rejected! Array reached max capacity (%u).\n",
                   i, SENSOR_ARRAY_MAX_CAPACITY);
        }
    }

    printf("\nPopping all elements from bounded array:\n");
    SensorRecord out = {0};
    while (bounded_array_pop(&telemetry_log, &out)) {
        printf("  [POP]       Temp: %d, Time: %u ms\n", out.temperature_raw, out.timestamp_ms);
    }

    return 0;
}
