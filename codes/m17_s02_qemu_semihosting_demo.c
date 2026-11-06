/**
 * @file 57a_qemu_semihosting_demo.c
 * @brief ARM Cortex-M QEMU Semihosting exit code reporting for CI/CD pipelines (Slide 135).
 */
#include <stdio.h>
#include <stdint.h>

#define SEMIHOSTING_SYS_EXIT           0x18U
#define ADP_STOPPED_APPLICATION_EXIT   0x20026U

// Triggers ARM semihosting call via software breakpoint instruction
static inline void semihosting_exit(int32_t exit_code) {
    uint32_t parameters[2] = {
            ADP_STOPPED_APPLICATION_EXIT,
            (uint32_t)exit_code
    };

#if defined(__ARM_ARCH)
    register uint32_t r0 __asm__("r0") = SEMIHOSTING_SYS_EXIT;
    register uint32_t r1 __asm__("r1") = (uint32_t)parameters;

    // ARM Cortex-M semihosting trap instruction intercepted by QEMU
    __asm__ volatile(
        "bkpt 0xAB"
        : : "r"(r0), "r"(r1) : "memory"
    );
#else
    // Host PC simulation of QEMU semihosting trap
    printf("  [QEMU Semihosting Trap 0xAB] r0=0x%02X (SYS_EXIT), Block={0x%X, exit_code=%d}\n",
           SEMIHOSTING_SYS_EXIT, parameters[0], (int)parameters[1]);
    printf("  [CI/CD Pipeline] Emulator terminated with status: %s (%d)\n",
           (exit_code == 0) ? "PIPELINE GREEN" : "PIPELINE RED", exit_code);
#endif
}

void unity_suite_complete_handler(int failed_tests_count) {
    // 0 = Success (Pipeline Green), 1 = Failure (Pipeline Red)
    int32_t exit_code = (failed_tests_count == 0) ? 0 : 1;
    semihosting_exit(exit_code);
}

int main(void) {
    printf("--- QEMU ARM Semihosting Demo ---\n\n");

    printf("Scenario A: All Unity tests passed in QEMU (0 failures):\n");
    unity_suite_complete_handler(0);

    printf("\nScenario B: 2 Unity tests failed in QEMU:\n");
    unity_suite_complete_handler(2);

    return 0;
}
