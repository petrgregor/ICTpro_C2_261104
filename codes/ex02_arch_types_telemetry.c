/**
 * @file ex02_arch_types_telemetry.c
 * @brief Reference solution for Section 02 Exercise: Portable Telemetry Decoder.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

// 1. Portable structure with guaranteed bit-widths across 8/16/32/64-bit platforms
typedef struct {
    int16_t temp_deci_celsius; // e.g., -125 means -12.5 C
    uint32_t pressure_pa;      // e.g., 101325 Pa
    uint8_t battery_percent;   // 0 to 100 %
} RawAdcFrame;

// 2. Safe arithmetic preventing 16-bit/32-bit intermediate overflow
bool compute_altitude_factor(const RawAdcFrame *frame, int32_t *out_factor) {
    if (frame == nullptr || out_factor == nullptr) {
        return false;
    }

    // Promote operands to 64-bit integer before multiplication to prevent overflow
    int64_t temp_product = (int64_t)frame->temp_deci_celsius * (int64_t)frame->pressure_pa;

    // Scale down safely into 32-bit output range
    *out_factor = (int32_t)(temp_product / 1000LL);
    return true;
}

int main(void) {
    printf("=== Exercise 02: Portable Telemetry Decoder ===\n\n");

    RawAdcFrame sample = {
            .temp_deci_celsius = 235,   // 23.5 C
            .pressure_pa = 101325U,     // Standard pressure (exceeds 16-bit int max 32767!)
            .battery_percent = 88U
    };

    // 3. Safe pointer-to-integer conversion using uintptr_t
    uintptr_t struct_addr = (uintptr_t)&sample;

    printf("Struct address: 0x%" PRIxPTR " (Pointer size: %zu bytes)\n",
           struct_addr, sizeof(void *));
    printf("Field sizes -> temp: %zu B, pressure: %zu B, battery: %zu B (Total struct: %zu B)\n",
           sizeof(sample.temp_deci_celsius),
           sizeof(sample.pressure_pa),
           sizeof(sample.battery_percent),
           sizeof(RawAdcFrame));

    int32_t factor = 0;
    if (compute_altitude_factor(&sample, &factor)) {
        printf("Computed altitude factor: %" PRId32 "\n", factor);
    }

    return 0;
}
