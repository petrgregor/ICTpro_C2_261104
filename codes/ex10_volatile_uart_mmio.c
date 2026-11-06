/**
 * @file ex10_volatile_uart_mmio.c
 * @brief Reference solution for Section 10 Exercise: Volatile MMIO UART Driver.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Bit masks for UART hardware registers
#define UART_STATUS_TX_EMPTY (1UL << 7)
#define UART_CTRL_ENABLE     (1UL << 0)

// 1. Memory-Mapped I/O register layout (MUST use volatile)
typedef struct {
    volatile uint32_t STATUS;
    volatile uint32_t TX_DATA;
    volatile uint32_t CONTROL;
} UartRegisters;

// Simulated backing memory for Host PC execution (prevents Segmentation Fault)
static UartRegisters simulated_uart1_hw = { .STATUS = UART_STATUS_TX_EMPTY, .TX_DATA = 0, .CONTROL = 0 };
#define UART1 (&simulated_uart1_hw)

void uart_init(UartRegisters *uart) {
    if (uart == nullptr) return;
    uart->CONTROL |= UART_CTRL_ENABLE;
}

// 2. Bounded polling loop reading volatile STATUS register
bool uart_send_byte_timeout(UartRegisters *uart, uint8_t byte, uint32_t max_polls) {
    if (uart == nullptr) return false;

    uint32_t polls = 0U;
    // Without 'volatile' on STATUS, the compiler (-O2/-O3) would read STATUS only once
    // and turn this into an infinite loop if the bit was initially 0!
    while ((uart->STATUS & UART_STATUS_TX_EMPTY) == 0U) {
        if (++polls >= max_polls) {
            return false; // Hardware timeout protection
        }
    }

    // Write payload byte to hardware transmit register
    uart->TX_DATA = (uint32_t)byte;
    return true;
}

int main(void) {
    printf("=== Exercise 10: Volatile MMIO UART Driver ===\n\n");

    uart_init(UART1);
    const char *msg = "OK";

    for (size_t i = 0; msg[i] != '\0'; ++i) {
        if (uart_send_byte_timeout(UART1, (uint8_t)msg[i], 1000U)) {
            printf("  [UART1 TX] Written '0x%02X' ('%c') to TX_DATA register.\n",
                   (uint8_t)UART1->TX_DATA, msg[i]);
        }
    }

    // Simulate stuck hardware (TX buffer never becomes empty)
    UART1->STATUS &= ~UART_STATUS_TX_EMPTY;
    if (!uart_send_byte_timeout(UART1, '!', 500U)) {
        printf("  [UART1 Timeout] Safely aborted polling after 500 cycles (TX busy)!\n");
    }

    return 0;
}
