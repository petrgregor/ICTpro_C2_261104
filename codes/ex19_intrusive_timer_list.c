/**
 * @file ex19_intrusive_timer_list.c
 * @brief Reference solution for Section 19 Exercise: Intrusive Timer List & container_of.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// 1. Standard container_of macro using compile-time offsetof
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct ListNode {
    struct ListNode *next;
} ListNode;

// 2. Domain structure embedding the intrusive node at a non-zero offset
typedef struct {
    uint32_t period_ms;
    const char *name;
    ListNode node; // Placed at the end (offset > 0)
} SoftTimer;

// 3. Iterate generic nodes and recover enclosing SoftTimer instances
void timers_tick_all(ListNode *head) {
    ListNode *curr = head;
    while (curr != nullptr) {
        SoftTimer *timer = CONTAINER_OF(curr, SoftTimer, node);
        printf("  [Timer Tick] '%s' triggered (Period: %u ms)\n",
               timer->name, timer->period_ms);
        curr = curr->next;
    }
}

int main(void) {
    printf("=== Exercise 19: Intrusive Timer List (container_of) ===\n\n");

    SoftTimer led_blink = { .period_ms = 500U,  .name = "LED_Heartbeat", .node = {nullptr} };
    SoftTimer watchdog  = { .period_ms = 1000U, .name = "WDT_Feeder",    .node = {nullptr} };
    SoftTimer telemetry = { .period_ms = 5000U, .name = "MQTT_Publish",  .node = {nullptr} };

    // Link intrusive nodes manually without any heap allocation
    led_blink.node.next = &watchdog.node;
    watchdog.node.next  = &telemetry.node;

    timers_tick_all(&led_blink.node);

    return 0;
}
