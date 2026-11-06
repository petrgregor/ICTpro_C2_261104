/**
 * @file 02_c_abi_export.c
 * @brief Implementation of the stable C API.
 *
 * Compile as shared library (Linux): gcc -shared -fPIC 02_c_abi_export.c -o libsensor.so
 */

#include "m01_s02_c_abi_export.h"
#include <stdlib.h>
#include <stdio.h>

// The actual internal definition of the opaque struct (hidden from caller)
struct SensorContext {
    uint32_t internal_hardware_id;
    bool is_active;
    uint64_t simulated_time;
};

// Constructor: Returns opaque handle
SensorContext* sensor_create_instance(void) {
    SensorContext *ctx = (SensorContext*)malloc(sizeof(SensorContext));
    if (ctx) {
        ctx->internal_hardware_id = 0xABCD1234;
        ctx->is_active = true;
        ctx->simulated_time = 10000;
        printf("[C Lib] Sensor instance created.\n");
    }
    return ctx;
}

// Data fetcher: Populates caller-provided memory
bool sensor_fetch_data(SensorContext *ctx, SensorReading *out_reading) {
    if (!ctx || !out_reading || !ctx->is_active) {
        return false;
    }

    // Simulate hardware read
    ctx->simulated_time += 500;

    out_reading->timestamp_us = ctx->simulated_time;
    out_reading->temperature_celsius = 24.5f;
    out_reading->error_flags = 0;

    printf("[C Lib] Fetched data for timestamp %llu.\n", out_reading->timestamp_us);
    return true;
}

// Destructor: Cleans up memory
void sensor_destroy_instance(SensorContext *ctx) {
    if (ctx) {
        ctx->is_active = false;
        free(ctx);
        printf("[C Lib] Sensor instance destroyed.\n");
    }
}

// Embedded Main for testing the ABI locally without external language
int main(void) {
    SensorContext *sensor = sensor_create_instance();

    SensorReading reading = {0};
    if (sensor_fetch_data(sensor, &reading)) {
        printf("Read Temp: %.2f\n", reading.temperature_celsius);
    }

    sensor_destroy_instance(sensor);
    return 0;
}
