/**
 * @file 31_intrusive_list.c
 * @brief Implementing an intrusive linked list requiring zero dynamic memory allocation.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 31_intrusive_list.c -o intrusive_list
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// Generic Node. It has absolutely no idea what data it is attached to.
typedef struct ListNode {
    struct ListNode *next;
} ListNode;

// Application-specific payload that embeds the node intrusively
typedef struct {
    float voltage;
    uint32_t id;
    ListNode list_element; // The embedded intrusive node
} PowerMonitor;

/**
 * @brief Iterates through the raw nodes and extracts the payload using CONTAINER_OF.
 */
void process_power_monitors(ListNode *head) {
    ListNode *current = head;

    while (current != nullptr) {
        // Extract the parent structure (payload) from the generic node
        PowerMonitor *monitor = CONTAINER_OF(current, PowerMonitor, list_element);

        printf("Monitor ID: %u | Voltage: %.2f V\n", monitor->id, monitor->voltage);

        current = current->next;
    }
}

int main(void) {
    printf("--- Intrusive Linked List ---\n\n");

    // Static allocation: No malloc needed anywhere!
    PowerMonitor m1 = { .voltage = 3.3f, .id = 1, .list_element = { nullptr } };
    PowerMonitor m2 = { .voltage = 5.0f, .id = 2, .list_element = { nullptr } };
    PowerMonitor m3 = { .voltage = 12.0f, .id = 3, .list_element = { nullptr } };

    // Linking the embedded nodes together
    m1.list_element.next = &m2.list_element;
    m2.list_element.next = &m3.list_element;

    // Pass the head node to the processing function
    process_power_monitors(&m1.list_element);

    return 0;
}
