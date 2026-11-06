/**
 * @file 54a_rtos_static_task_demo.c
 * @brief Simulation of FreeRTOS static task allocation and Stack High Water Mark diagnostics.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// --- Simulated FreeRTOS Types & Macros for Host PC Execution ---
typedef uint32_t StackType_t;
typedef uint32_t UBaseType_t;
typedef uint32_t TickType_t;
typedef void (*TaskFunction_t)(void *);

#define tskIDLE_PRIORITY 0U
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
#define STACK_CANARY_PATTERN 0xA5A5A5A5UL

typedef struct {
    const char *task_name;
    StackType_t *stack_base;
    uint32_t stack_depth_words;
    UBaseType_t priority;
    TaskFunction_t entry_fn;
} StaticTask_t;

typedef StaticTask_t* TaskHandle_t;

static StaticTask_t *active_tcb = nullptr;

// Simulates FreeRTOS xTaskCreateStatic (Zero heap allocation)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer,
                               StaticTask_t * const pxTaskBuffer) {
    (void)pvParameters;
    if (puxStackBuffer == nullptr || pxTaskBuffer == nullptr) {
        return nullptr;
    }

    // FreeRTOS fills the entire stack with 0xA5 pattern at task creation
    for (uint32_t i = 0; i < ulStackDepth; ++i) {
        puxStackBuffer[i] = STACK_CANARY_PATTERN;
    }

    pxTaskBuffer->task_name = pcName;
    pxTaskBuffer->stack_base = puxStackBuffer;
    pxTaskBuffer->stack_depth_words = ulStackDepth;
    pxTaskBuffer->priority = uxPriority;
    pxTaskBuffer->entry_fn = pxTaskCode;

    active_tcb = pxTaskBuffer;
    printf("  [FreeRTOS] Statically created task '%s' (Stack: %u words at %p, TCB at %p)\n",
           pcName, ulStackDepth, (void *)puxStackBuffer, (void *)pxTaskBuffer);

    return pxTaskBuffer;
}

// Simulates FreeRTOS uxTaskGetStackHighWaterMark (counts untouched 0xA5 words from bottom of stack)
UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask) {
    StaticTask_t *tcb = (xTask == nullptr) ? active_tcb : xTask;
    if (tcb == nullptr) return 0U;

    UBaseType_t free_words = 0U;
    for (uint32_t i = 0; i < tcb->stack_depth_words; ++i) {
        if (tcb->stack_base[i] == STACK_CANARY_PATTERN) {
            free_words++;
        } else {
            break; // Reached highest point of stack growth
        }
    }
    return free_words;
}

void vTaskDelay(const TickType_t xTicksToDelay) {
    printf("  [FreeRTOS] Task delayed for %u ticks.\n", xTicksToDelay);
}

// --- Code from Slide 2: Static Task Allocation & Stack Monitoring ---
#define SENSOR_TASK_STACK_WORDS 256U

// Statically allocated memory for stack and TCB in .bss section (Zero heap usage)
static StackType_t sensor_task_stack[SENSOR_TASK_STACK_WORDS];
static StaticTask_t sensor_task_tcb;

// Helper to simulate stack consumption during nested function calls
static void simulate_stack_usage(uint32_t words_used) {
    // Stack typically grows downwards from top of buffer towards index 0
    for (uint32_t i = 0; i < words_used && i < SENSOR_TASK_STACK_WORDS; ++i) {
        sensor_task_stack[SENSOR_TASK_STACK_WORDS - 1U - i] = 0xDEADBEEFUL;
    }
}

void sensor_task_entry(void *argument) {
    (void)argument;

    // Simulate 2 iterations of the task loop (instead of infinite loop for PC demo)
    for (int step = 1; step <= 2; ++step) {
        printf("\n--- Iteration %d ---\n", step);
        vTaskDelay(pdMS_TO_TICKS(100));

        // Simulate normal stack usage in step 1 (180 words) and heavy usage in step 2 (235 words)
        simulate_stack_usage((step == 1) ? 180U : 235U);

        // Diagnostics: Check minimum unused stack space ever recorded
        UBaseType_t min_stack_headroom = uxTaskGetStackHighWaterMark(nullptr);
        printf("  [Diagnostics] Stack High Water Mark (remaining free words): %u / %u\n",
               min_stack_headroom, SENSOR_TASK_STACK_WORDS);

        if (min_stack_headroom < 32U) {
            printf("  [WARNING] Stack utilization exceeded safe threshold (< 32 words left)!\n");
        }
    }
}

void init_system_tasks(void) {
    // Deterministic creation: Guaranteed to succeed, immune to heap fragmentation
    xTaskCreateStatic(sensor_task_entry, "SensorTask", SENSOR_TASK_STACK_WORDS,
                      nullptr, tskIDLE_PRIORITY + 2U,
                      sensor_task_stack, &sensor_task_tcb);
}

int main(void) {
    printf("--- Static Task Allocation & Stack Watermark ---\n\n");
    init_system_tasks();
    sensor_task_entry(nullptr);
    return 0;
}
