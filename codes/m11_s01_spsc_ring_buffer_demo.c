/**
 * @file 53_spsc_ring_buffer_demo.c
 * @brief Lock-free SPSC Ring Buffer and C11/C23 atomics between ISR and Task.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#define SPSC_BUFFER_SIZE 64U // Must be a power of two
#define SPSC_BUFFER_MASK (SPSC_BUFFER_SIZE - 1U)

typedef struct {
    uint8_t storage[SPSC_BUFFER_SIZE];
    volatile uint32_t head; // Written ONLY by Producer (ISR)
    volatile uint32_t tail; // Written ONLY by Consumer (Task)
} SpscRingBuffer;

static SpscRingBuffer uart_rx_fifo = {0};
static atomic_uint_fast32_t isr_event_counter = 0;

bool spsc_push_from_isr(SpscRingBuffer *rb, uint8_t byte) {
    uint32_t current_head = rb->head;
    uint32_t next_head = (current_head + 1U) & SPSC_BUFFER_MASK;

    if (next_head == rb->tail) {
        return false; // Buffer full
    }

    rb->storage[current_head] = byte;
    atomic_signal_fence(memory_order_release); // Compiler/memory barrier
    rb->head = next_head;
    return true;
}

bool spsc_pop_in_task(SpscRingBuffer *rb, uint8_t *out_byte) {
    uint32_t current_tail = rb->tail;
    if (current_tail == rb->head) {
        return false; // Buffer empty
    }

    *out_byte = rb->storage[current_tail];
    atomic_signal_fence(memory_order_acquire); // Compiler/memory barrier
    rb->tail = (current_tail + 1U) & SPSC_BUFFER_MASK;
    return true;
}

// Simulated UART RX Interrupt Service Routine
void simulated_uart_rx_isr(const char *incoming_stream) {
    while (*incoming_stream != '\0') {
        if (spsc_push_from_isr(&uart_rx_fifo, (uint8_t)(*incoming_stream))) {
            atomic_fetch_add_explicit(&isr_event_counter, 1U, memory_order_relaxed);
        }
        incoming_stream++;
    }
}

int main(void) {
    printf("--- Section 11: ISR & Lock-Free SPSC Ring Buffer ---\n\n");

    // 1. Hardware interrupt fires and pushes data asynchronously
    simulated_uart_rx_isr("AT+OK");

    uint32_t total_irqs = atomic_load_explicit(&isr_event_counter, memory_order_relaxed);
    printf("ISR triggered %u times. Draining FIFO in worker task:\nReceived: ", total_irqs);

    // 2. Worker task reads bytes without disabling interrupts
    uint8_t received_byte = 0;
    while (spsc_pop_in_task(&uart_rx_fifo, &received_byte)) {
        printf("%c", (char)received_byte);
    }
    printf("\n");

    return 0;
}
