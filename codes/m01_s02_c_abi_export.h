/**
 * @file 02_c_abi_export.h
 * @brief Stable C ABI definition exportable to any other language (Rust, Python FFI, etc.)
 */
#ifndef C_ABI_EXPORT_H
#define C_ABI_EXPORT_H

#include <stdint.h>
#include <stdbool.h>

// Explicitly sized struct guarantees identical memory layout across all languages
// No compiler-dependent sizes like 'int' or 'long' are used in the public interface.
typedef struct {
    uint64_t timestamp_us;
    float temperature_celsius;
    uint32_t error_flags;
} SensorReading;

// Opaque pointer (Handle): Hides internal implementation details from the caller language
typedef struct SensorContext SensorContext;

#ifdef __cplusplus
extern "C" {
#endif

// Public API functions exported with standard C calling conventions
SensorContext* sensor_create_instance(void);
bool sensor_fetch_data(SensorContext *ctx, SensorReading *out_reading);
void sensor_destroy_instance(SensorContext *ctx);

#ifdef __cplusplus
}
#endif

#endif // C_ABI_EXPORT_H
