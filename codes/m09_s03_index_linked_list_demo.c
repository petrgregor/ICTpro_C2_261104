/**
 * @file 60_index_linked_list_demo.c
 * @brief Index-based static linked list without pointers or dynamic heap allocation.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NODES 8
#define LIST_INVALID_INDEX (-1)

typedef struct {
    uint32_t sensor_value;
    int16_t next; // Array index replacing memory pointer
} StaticNode;

typedef struct {
    StaticNode pool[MAX_NODES];
    int16_t head;
    int16_t free_head;
} StaticLinkedList;

void static_list_init(StaticLinkedList *list) {
    list->head = LIST_INVALID_INDEX;
    list->free_head = 0;

    // Link all initial nodes into the free list
    for (int16_t i = 0; i < (MAX_NODES - 1); ++i) {
        list->pool[i].next = (int16_t)(i + 1);
    }
    list->pool[MAX_NODES - 1].next = LIST_INVALID_INDEX;
}

bool static_list_insert_front(StaticLinkedList *list, uint32_t value) {
    if (list->free_head == LIST_INVALID_INDEX) {
        return false; // Static pool exhausted
    }

    int16_t new_idx = list->free_head;
    list->free_head = list->pool[new_idx].next; // Pop from free list

    list->pool[new_idx].sensor_value = value;
    list->pool[new_idx].next = list->head;      // Push to active list
    list->head = new_idx;
    return true;
}

void static_list_print(const StaticLinkedList *list) {
    printf("Active List (head idx = %d): ", list->head);
    int16_t curr = list->head;
    while (curr != LIST_INVALID_INDEX) {
        printf("[idx %d: val=%u] -> ", curr, list->pool[curr].sensor_value);
        curr = list->pool[curr].next;
    }
    printf("END (-1)\n");
}

int main(void) {
    printf("--- Section 09: Index-Based Static Linked List ---\n\n");

    StaticLinkedList sensor_list;
    static_list_init(&sensor_list);

    static_list_insert_front(&sensor_list, 100U);
    static_list_insert_front(&sensor_list, 250U);
    static_list_insert_front(&sensor_list, 400U);

    static_list_print(&sensor_list);
    printf("Next available free node index: %d\n", sensor_list.free_head);

    return 0;
}
