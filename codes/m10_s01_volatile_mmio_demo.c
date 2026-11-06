/**
 * @file 52_volatile_mmio_demo.c
 * @brief Simulation of Memory-Mapped I/O (MMIO) registers using volatile.
 */
#include <stdio.h>
#include <stdint.h>

// Hardware peripheral layout with volatile registers
typedef struct {
    volatile uint32_t MODE;   // Mode register
    volatile uint32_t OUTPUT; // Output data register
    volatile uint32_t INPUT;  // Input data register
} GpioPeripheral;

// On a real MCU: #define GPIO_PORT_A_BASE 0x40020000UL
// On Host PC: We back the peripheral with static memory to avoid Segmentation Fault
static GpioPeripheral simulated_gpio_a_memory = {0};
#define GPIOA (&simulated_gpio_a_memory)

void hardware_toggle_pin(uint8_t pin_number) {
    // 1. Set pin as output (2 bits per pin in MODE register)
    GPIOA->MODE |= (1UL << (pin_number * 2U));

    // 2. Toggle pin output state. Compiler cannot optimize these writes away!
    GPIOA->OUTPUT ^= (1UL << pin_number);
}

int main(void) {
    printf("--- Section 10: Volatile and Memory-Mapped I/O ---\n\n");

    printf("Initial GPIOA->OUTPUT: 0x%08X\n", GPIOA->OUTPUT);

    hardware_toggle_pin(5);
    printf("After 1st toggle (Pin 5 HIGH): 0x%08X (MODE: 0x%08X)\n",
           GPIOA->OUTPUT, GPIOA->MODE);

    hardware_toggle_pin(5);
    printf("After 2nd toggle (Pin 5 LOW):  0x%08X\n", GPIOA->OUTPUT);

    return 0;
}
