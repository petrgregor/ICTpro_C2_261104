/**
 * @file 23_context_pointer.c
 * @brief Demonstration of the Context Pointer idiom vs Global State.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 23_context_pointer.c -o context_ptr
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// --- ANTI-PATTERN: Hardware-centric approach with global state ---
uint32_t g_encoder_ticks = 0;

void process_encoder_global(void) {
    g_encoder_ticks++;
    // Problem: You can only ever have ONE encoder in your entire system.
    // If a new hardware board adds a second encoder, you must duplicate functions.
}

// --- ARCHITECTURAL APPROACH: Context Pointer ---
// The state of the hardware is encapsulated in a structure.
typedef struct {
    uint32_t ticks;
    uint32_t hardware_base_address;
} EncoderContext;

/**
 * @brief Processes an encoder event using a specific instance context.
 *
 * By passing a pointer to the context, this single function can handle
 * an unlimited number of physical encoders.
 */
void encoder_process(EncoderContext *ctx) {
    if (ctx != nullptr) {
        ctx->ticks++;
    }
}

int main(void) {
    printf("--- Context Pointer Demonstration ---\n\n");

    // Instantiating two completely independent physical encoders
    EncoderContext motor_left = { .ticks = 0, .hardware_base_address = 0x40010000 };
    EncoderContext motor_right = { .ticks = 0, .hardware_base_address = 0x40010400 };

    // Simulating interrupts or hardware events
    encoder_process(&motor_left);
    encoder_process(&motor_left);
    encoder_process(&motor_right);

    printf("Left motor ticks:  %u\n", motor_left.ticks);
    printf("Right motor ticks: %u\n", motor_right.ticks);

    return 0;
}
