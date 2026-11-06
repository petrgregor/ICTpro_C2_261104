/**
 * @file 53a_isr_critical_section_demo.c
 * @brief Safe nested critical section saving/restoring PRIMASK for 64-bit variable access (Slide 79).
 */
#include <stdio.h>
#include <stdint.h>

#if defined(__ARM_ARCH)
// Real ARM Cortex-M inline assembly implementation
static inline uint32_t enter_critical_section(void) {
    uint32_t primask;
    __asm__ volatile("mrs %0, primask" : "=r"(primask));
    __asm__ volatile("cpsid i" : : : "memory");
    return primask;
}

static inline void exit_critical_section(uint32_t primask) {
    __asm__ volatile("msr primask, %0" : : "r"(primask) : "memory");
}
#else
// Host PC simulation of Cortex-M PRIMASK register (0 = IRQs enabled, 1 = IRQs masked)
static volatile uint32_t simulated_cpu_primask = 0U;

static inline uint32_t enter_critical_section(void) {
    uint32_t primask = simulated_cpu_primask;
    simulated_cpu_primask = 1U; // Simulate "cpsid i"
    __asm__ volatile("" : : : "memory");
    printf("  [CPU] Enter Critical Section (Saved PRIMASK=%u, New PRIMASK=1)\n", primask);
    return primask;
}

static inline void exit_critical_section(uint32_t primask) {
    __asm__ volatile("" : : : "memory");
    simulated_cpu_primask = primask; // Restore previous interrupt mask state
    printf("  [CPU] Exit Critical Section  (Restored PRIMASK=%u)\n", simulated_cpu_primask);
}
#endif

static volatile uint64_t system_uptime_us = 0x00000001FFFFFFFFULL;

uint64_t get_system_uptime(void) {
    // 64-bit read requires two 32-bit LDR instructions on 32-bit Cortex-M (prevents torn read)
    uint32_t status = enter_critical_section();
    uint64_t current_time = system_uptime_us;
    exit_critical_section(status);
    return current_time;
}

void outer_critical_task(void) {
    printf("1. Entering outer critical section:\n");
    uint32_t outer_state = enter_critical_section();

    printf("2. Calling get_system_uptime() inside outer critical section (nested call):\n");
    uint64_t t = get_system_uptime();
    printf("   -> Safely read 64-bit uptime: 0x%llX us\n", (unsigned long long)t);

    printf("3. Exiting outer critical section:\n");
    exit_critical_section(outer_state);
}

int main(void) {
    printf("--- Nested Critical Section (PRIMASK) Demo ---\n\n");
    outer_critical_task();
    return 0;
}
