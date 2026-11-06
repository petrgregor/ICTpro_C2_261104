/**
 * @file 32_enum_type_tag.c
 * @brief Using enum as a type tag for runtime identification.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 32_enum_type_tag.c -o enum_tag
 */
#include <stdio.h>
#include <stdint.h>

// In C23, we can specify the exact underlying memory footprint for the enum
typedef enum : uint8_t {
    EVENT_KEYPRESS,
    EVENT_MOUSE_CLICK,
    EVENT_SYSTEM_SHUTDOWN
} EventType;

// Generic structure carrying a type identifier
typedef struct {
    EventType type;
    uint32_t timestamp;
} SystemEvent;

/**
 * @brief Processes the event using the tag to determine the required logic.
 */
void process_event(const SystemEvent *event) {
    if (event == nullptr) return;

    // The compiler will warn us (-Wswitch) if we forget to handle an enum value
    switch (event->type) {
        case EVENT_KEYPRESS:
            printf("[%u] Processing keypress event.\n", event->timestamp);
            break;
        case EVENT_MOUSE_CLICK:
            printf("[%u] Processing mouse click event.\n", event->timestamp);
            break;
        case EVENT_SYSTEM_SHUTDOWN:
            printf("[%u] Shutting down the system!\n", event->timestamp);
            break;
        default:
            printf("[%u] Unknown event received.\n", event->timestamp);
            break;
    }
}

int main(void) {
    printf("--- Enum Type Tag Demonstration ---\n\n");

    SystemEvent e1 = { .type = EVENT_KEYPRESS, .timestamp = 1000 };
    SystemEvent e2 = { .type = EVENT_SYSTEM_SHUTDOWN, .timestamp = 2050 };

    process_event(&e1);
    process_event(&e2);

    return 0;
}
