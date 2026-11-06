/**
 * @file ex22_generic_clamp_macro.c
 * @brief Reference solution for Section 22 Exercise: Token Pasting & _Generic Clamp API.
 */
#include <stdio.h>
#include <stdint.h>

// 1. Code generator macro using token pasting (##)
#define DEFINE_CLAMP_FUNC(SUFFIX, TYPE) \
    TYPE clamp_##SUFFIX(TYPE val, TYPE min_val, TYPE max_val) { \
        if (val < min_val) return min_val; \
        if (val > max_val) return max_val; \
        return val; \
    }

// Instantiate functions for specific types
DEFINE_CLAMP_FUNC(i32, int32_t)
DEFINE_CLAMP_FUNC(u16, uint16_t)
DEFINE_CLAMP_FUNC(f32, float)

// 2. Compile-time type dispatch using C11/C23 _Generic
#define clamp_val(X, MIN, MAX) _Generic((X), \
    int32_t:  clamp_i32, \
    uint16_t: clamp_u16, \
    float:    clamp_f32  \
)((X), (MIN), (MAX))

int main(void) {
    printf("=== Exercise 22: Generic Programming (_Generic & Macros) ===\n\n");

    int32_t  temp_i32 = -50;
    uint16_t pwm_u16  = 5000U;
    float    volt_f32 = 3.85f;

    printf("Clamped int32_t  (-50 in [-20..100]): %d\n",
           clamp_val(temp_i32, (int32_t)-20, (int32_t)100));
    printf("Clamped uint16_t (5000 in [0..4095]): %u\n",
           clamp_val(pwm_u16, (uint16_t)0U, (uint16_t)4095U));
    printf("Clamped float    (3.85 in [0..3.30]): %.2f\n",
           clamp_val(volt_f32, 0.0f, 3.30f));

    return 0;
}
