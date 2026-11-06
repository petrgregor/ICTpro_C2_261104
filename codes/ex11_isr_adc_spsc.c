/**
 * @file ex11_isr_adc_spsc.c
 * @brief Reference solution for Section 11 Exercise: ISR ADC SPSC Ring Buffer & Atomics.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#define ADC_RING_SIZE 8U // Power of two (7 usable slots in lock-free SPSC)
#define ADC_RING_MASK (ADC_RING_SIZE - 1U)

typedef struct {
    uint16_t samples[ADC_RING_SIZE];
    volatile uint32_t head; // Modified only by ISR (Producer)
    volatile uint32_t tail; // Modified only by Task (Consumer)
} AdcRingBuffer;

static AdcRingBuffer adc_fifo = {0};
static atomic_uint_fast32_t dropped_samples = 0;

bool adc_isr_push(AdcRingBuffer *rb, uint16_t sample) {
    uint32_t current_head = rb->head;
    uint32_t next_head = (current_head + 1U) & ADC_RING_MASK;

    if (next_head == rb->tail) {
        // Buffer full: atomically record dropped sample without disabling interrupts
        atomic_fetch_add_explicit(&dropped_samples, 1U, memory_order_relaxed);
        return false;
    }

    rb->samples[current_head] = sample;
    atomic_signal_fence(memory_order_release); // Ensure sample is written before head updates
    rb->head = next_head;
    return true;
}

bool adc_task_pop(AdcRingBuffer *rb, uint16_t *out_sample) {
    uint32_t current_tail = rb->tail;
    if (current_tail == rb->head) {
        return false; // Buffer empty
    }

    *out_sample = rb->samples[current_tail];
    atomic_signal_fence(memory_order_acquire);
    rb->tail = (current_tail + 1U) & ADC_RING_MASK;
    return true;
}

int main(void) {
    printf("=== Exercise 11: ISR ADC SPSC Ring Buffer & Atomics ===\n\n");

    // Simulate burst of 10 ADC interrupts before worker task wakes up (capacity is 7 usable slots)
    for (uint16_t i = 1; i <= 10; ++i) {
        adc_isr_push(&adc_fifo, i * 100U);
    }

    uint32_t drops = atomic_load_explicit(&dropped_samples, memory_order_relaxed);
    printf("ISR burst complete. Atomically recorded dropped samples: %u\n", drops);

    printf("Worker task draining valid samples: ");
    uint16_t val = 0;
    while (adc_task_pop(&adc_fifo, &val)) {
        printf("%u ", val);
    }
    printf("\n");

    return 0;
}
