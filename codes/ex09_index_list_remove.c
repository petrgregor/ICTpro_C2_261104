/**
 * @file ex09_index_list_remove.c
 * @brief Reference solution for Section 09 Exercise: Index-based Linked List Pop & Free-list recycling.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NODES 4
#define LIST_INVALID_INDEX (-1)

typedef struct {
    uint32_t sensor_value;
    int16_t next;
} StaticNode;

typedef struct {
    StaticNode pool[MAX_NODES];
    int16_t head;
    int16_t free_head;
} StaticLinkedList;

void static_list_init(StaticLinkedList *list) {
    list->head = LIST_INVALID_INDEX;
    list->free_head = 0;
    for (int16_t i = 0; i < (MAX_NODES - 1); ++i) {
        list->pool[i].next = (int16_t)(i + 1);
    }
    list->pool[MAX_NODES - 1].next = LIST_INVALID_INDEX;
}

bool static_list_insert_front(StaticLinkedList *list, uint32_t value) {
    if (list->free_head == LIST_INVALID_INDEX) return false;

    int16_t new_idx = list->free_head;
    list->free_head = list->pool[new_idx].next;

    list->pool[new_idx].sensor_value = value;
    list->pool[new_idx].next = list->head;
    list->head = new_idx;
    return true;
}

// Exercise solution: O(1) removal and node recycling back to free_head
bool static_list_pop_front(StaticLinkedList *list, uint32_t *out_value) {
    if (list == nullptr || out_value == nullptr || list->head == LIST_INVALID_INDEX) {
        return false; // List is empty
    }

    int16_t removed_idx = list->head;
    *out_value = list->pool[removed_idx].sensor_value;

    // 1. Advance active list head to the next element
    list->head = list->pool[removed_idx].next;

    // 2. Push the removed node index back onto the front of the free list
    list->pool[removed_idx].next = list->free_head;
    list->free_head = removed_idx;

    return true;
}

int main(void) {
    printf("=== Exercise 09: Index-Based Linked List Recycling ===\n\n");

    StaticLinkedList list;
    static_list_init(&list);

    // Fill all 4 nodes
    for (uint32_t i = 1; i <= 4; ++i) {
        static_list_insert_front(&list, i * 10U);
    }
    printf("Pool full. Next free_head: %d\n", list.free_head);

    uint32_t popped_val = 0;
    if (static_list_pop_front(&list, &popped_val)) {
        printf("Popped value: %u -> Recycled node index is now free_head: %d\n",
               popped_val, list.free_head);
    }

    // Verify that we can insert again thanks to recycling
    if (static_list_insert_front(&list, 999U)) {
        printf("Successfully inserted 999 into recycled slot!\n");
    }

    return 0;
}
