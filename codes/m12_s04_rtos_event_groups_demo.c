/**
 * @file 54c_rtos_event_groups_demo.c
 * @brief Simulation of FreeRTOS Event Groups and atomic multi-event synchronization (Slide 7).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// --- Simulated FreeRTOS Event Group API for Host PC Execution ---
typedef uint32_t EventBits_t;
typedef uint32_t TickType_t;
typedef int BaseType_t;

#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFFUL

typedef struct {
    EventBits_t current_bits;
} MockEventGroup;

typedef MockEventGroup* EventGroupHandle_t;

EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet) {
    if (xEventGroup == nullptr) return 0U;
    xEventGroup->current_bits |= uxBitsToSet;
    printf("  [EventGroup] Set bits 0x%02X -> Current state: 0x%02X\n",
           uxBitsToSet, xEventGroup->current_bits);
    return xEventGroup->current_bits;
}

EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToWaitFor,
                                const BaseType_t xClearOnExit,
                                const BaseType_t xWaitForAllBits,
                                TickType_t xTicksToWait) {
    (void)xTicksToWait;
    if (xEventGroup == nullptr) return 0U;

    EventBits_t captured = xEventGroup->current_bits;
    bool condition_met = false;

    if (xWaitForAllBits == pdTRUE) {
        condition_met = ((captured & uxBitsToWaitFor) == uxBitsToWaitFor);
    } else {
        condition_met = ((captured & uxBitsToWaitFor) != 0U);
    }

    if (condition_met && (xClearOnExit == pdTRUE)) {
        xEventGroup->current_bits &= ~uxBitsToWaitFor;
    }

    return captured;
}

// --- Event Groups (Bit Flags) ---
#define EVENT_BIT_WIFI_CONNECTED    (1UL << 0)
#define EVENT_BIT_TIME_SYNCHRONIZED (1UL << 1)
#define EVENT_BIT_ALL_READY         (EVENT_BIT_WIFI_CONNECTED | EVENT_BIT_TIME_SYNCHRONIZED)

static MockEventGroup network_event_group_storage = { .current_bits = 0U };
static EventGroupHandle_t network_event_group = &network_event_group_storage;

void cloud_sync_task_step(void) {
    // Wait atomically for BOTH conditions before starting upload:
    // xClearOnExit = pdFALSE, xWaitForAllBits = pdTRUE
    EventBits_t status = xEventGroupWaitBits(
            network_event_group,
            EVENT_BIT_ALL_READY,
            pdFALSE,
            pdTRUE,
            portMAX_DELAY
    );

    if ((status & EVENT_BIT_ALL_READY) == EVENT_BIT_ALL_READY) {
        printf("  [CloudSyncTask] All conditions met (0x%02X)! Starting HTTPS payload upload.\n",
               status);
    } else {
        printf("  [CloudSyncTask] Waiting... Current bits: 0x%02X (Required: 0x%02X)\n",
               status, (uint32_t)EVENT_BIT_ALL_READY);
    }
}

int main(void) {
    printf("--- RTOS Event Groups Demo ---\n\n");

    printf("1. Initial check (neither Wi-Fi nor NTP time ready):\n");
    cloud_sync_task_step();

    printf("\n2. Wi-Fi driver connects:\n");
    xEventGroupSetBits(network_event_group, EVENT_BIT_WIFI_CONNECTED);
    cloud_sync_task_step();

    printf("\n3. NTP client synchronizes system clock:\n");
    xEventGroupSetBits(network_event_group, EVENT_BIT_TIME_SYNCHRONIZED);
    cloud_sync_task_step();

    return 0;
}
