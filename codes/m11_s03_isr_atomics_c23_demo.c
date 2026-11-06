/**
 * @file 53b_isr_atomics_c23_demo.c
 * @brief Lock-free event counter and atomic_flag synchronization in C23.
 */
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

// Lock-free atomic counter shared between ISR and worker task
static atomic_uint_fast32_t isr_event_counter = 0;
static atomic_flag packet_ready_flag = ATOMIC_FLAG_INIT;

// Executed in hardware interrupt context (e.g. GPIO button interrupt)
void exti_gpio_irq_handler(void) {
    // Atomic increment without disabling global interrupts
    atomic_fetch_add_explicit(&isr_event_counter, 1U, memory_order_relaxed);

    // Clear flag (or test_and_set) indicating a new event requires processing
    atomic_flag_clear_explicit(&packet_ready_flag, memory_order_release);
}

// Executed in background task context
void worker_task_loop(void) {
    // Atomically read and reset counter value
    uint32_t count = atomic_exchange_explicit(&isr_event_counter, 0U, memory_order_acquire);
    if (count > 0U) {
        printf("  [Worker Task] Processed %u hardware interrupt events lock-free!\n", count);
    }
}

int main(void) {
    printf("--- C23 Atomics in ISR Demo ---\n\n");

    // Simulate 3 asynchronous GPIO interrupts firing
    exti_gpio_irq_handler();
    exti_gpio_irq_handler();
    exti_gpio_irq_handler();

    worker_task_loop();
    return 0;
}
