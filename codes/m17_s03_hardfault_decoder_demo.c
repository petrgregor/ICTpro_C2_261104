/**
 * @file 57b_hardfault_decoder_demo.c
 * @brief Cortex-M HardFault stack frame extractor and post-mortem crash log decoder (Slide 136).
 */
#include <stdio.h>
#include <stdint.h>

#if defined(__ARM_ARCH)
// Real Cortex-M naked assembly stub from Slide 136
__attribute__((naked)) void HardFault_Handler(void) {
    __asm__ volatile(
        "tst lr, #4 \n"       // Check EXC_RETURN bit 2 to determine active stack (MSP vs PSP)
        "ite eq \n"
        "mrseq r0, msp \n"    // Argument 0 = Main Stack Pointer
        "mrsne r0, psp \n"    // Argument 0 = Process Stack Pointer
        "b hardfault_c_handler \n"
    );
}
#endif

// Simulated Cortex-M Configurable Fault Status Register (CFSR at 0xE000ED28)
static volatile uint32_t simulated_scb_cfsr = 0x00008200UL; // PRECISERR + BFARVALID

void hardfault_c_handler(uint32_t *stack_frame) {
    // Extract hardware-saved registers from Cortex-M exception stack frame (Slide 136):
    uint32_t r0  = stack_frame[0];
    uint32_t r1  = stack_frame[1];
    uint32_t r2  = stack_frame[2];
    uint32_t r3  = stack_frame[3];
    uint32_t r12 = stack_frame[4];
    uint32_t lr  = stack_frame[5]; // Link Register (return address)
    uint32_t pc  = stack_frame[6]; // Program Counter: Exact faulting instruction!
    uint32_t psr = stack_frame[7]; // Program Status Register

    // On real MCU: volatile uint32_t cfsr = (*(volatile uint32_t *)0xE000ED28UL);
    volatile uint32_t cfsr = simulated_scb_cfsr;

    printf("  [CRASH DUMP] HardFault Exception Caught!\n");
    printf("    R0  = 0x%08X | R1  = 0x%08X | R2  = 0x%08X | R3  = 0x%08X\n", r0, r1, r2, r3);
    printf("    R12 = 0x%08X | LR  = 0x%08X | PC  = 0x%08X | PSR = 0x%08X\n", r12, lr, pc, psr);
    printf("    SCB->CFSR = 0x%08X\n\n", cfsr);
    printf("  [Action] Run: arm-none-eabi-addr2line -e firmware.elf 0x%08X\n", pc);
    printf("           to locate the exact C source line and write a regression test!\n");
}

int main(void) {
    printf("--- Post-Mortem HardFault Decoder Demo ---\n\n");

    // Simulated hardware stack frame pushed automatically by Cortex-M core upon fault
    uint32_t mocked_psp_stack_frame[8] = {
            0x00000000UL, // R0 (e.g., nullptr dereferenced!)
            0x20001000UL, // R1
            0x00000009UL, // R2
            0x00000000UL, // R3
            0x00000000UL, // R12
            0x08001421UL, // LR (Caller function return address)
            0x080013F4UL, // PC (Exact instruction that triggered HardFault)
            0x61000000UL  // xPSR
    };

    hardfault_c_handler(mocked_psp_stack_frame);
    return 0;
}
