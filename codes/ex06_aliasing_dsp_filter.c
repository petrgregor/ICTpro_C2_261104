/**
 * @file ex06_aliasing_dsp_filter.c
 * @brief Reference solution for Section 06 Exercise: Strict Aliasing & restrict DSP loop.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// 1. Safe type punning using memcpy (Strict-aliasing compliant)
uint32_t float_extract_exponent(float val) {
    uint32_t raw_bits = 0U;
    memcpy(&raw_bits, &val, sizeof(raw_bits));

    // IEEE 754 single-precision: sign (bit 31), exponent (bits 30..23), mantissa (bits 22..0)
    return (raw_bits >> 23U) & 0xFFU;
}

// 2. Optimized DSP loop using 'restrict' to prevent redundant RAM reloads
void dsp_scale_and_offset(int32_t * restrict out,
                          const int32_t * restrict in,
                          const int32_t * restrict scale,
                          const int32_t * restrict offset,
                          size_t length) {
    // Thanks to 'restrict', compiler caches *scale and *offset in CPU registers once
    for (size_t i = 0; i < length; ++i) {
        out[i] = (in[i] * (*scale)) + (*offset);
    }
}

int main(void) {
    printf("=== Exercise 06: Strict Aliasing & restrict Optimization ===\n\n");

    float sample_voltage = 3.3f;
    uint32_t exp_bits = float_extract_exponent(sample_voltage);
    printf("Float value: %.2f -> IEEE 754 biased exponent: %u (unbiased: %d)\n\n",
           sample_voltage, exp_bits, (int)exp_bits - 127);

    const int32_t raw_adc[4] = {100, 200, 300, 400};
    int32_t calibrated_adc[4] = {0};
    const int32_t gain = 2;
    const int32_t bias = -15;

    dsp_scale_and_offset(calibrated_adc, raw_adc, &gain, &bias, 4U);

    printf("Calibrated DSP output: ");
    for (size_t i = 0; i < 4U; ++i) {
        printf("%d ", calibrated_adc[i]);
    }
    printf("\n");

    return 0;
}
