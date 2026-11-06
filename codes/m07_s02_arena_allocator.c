/**
 * @file 12_arena_allocator.c
 * @brief Demonstration of a linear Arena (Bump) Allocator with aligned allocations.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 12_arena_allocator.c -o arena
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t offset;
} Arena;

void arena_init(Arena *arena, uint8_t *backing_buffer, size_t capacity) {
    arena->buffer = backing_buffer;
    arena->capacity = capacity;
    arena->offset = 0;
    printf("[Arena] Initialized with %zu bytes.\n", capacity);
}

/**
 * @brief Allocates memory sequentially, ensuring proper alignment.
 */
void* arena_alloc(Arena *arena, size_t size, size_t alignment) {
    // Determine the current absolute memory address
    size_t current_addr = (size_t)(arena->buffer + arena->offset);

    // Round up to the nearest multiple of 'alignment'
    size_t aligned_addr = (current_addr + (alignment - 1U)) & ~(alignment - 1U);

    // Calculate the new offset relative to the buffer start
    size_t new_offset = (aligned_addr - (size_t)arena->buffer) + size;

    // Check for arena overflow
    if (new_offset > arena->capacity) {
        printf("[Arena] ERROR: Allocation of %zu bytes failed. Out of capacity!\n", size);
        return nullptr;
    }

    arena->offset = new_offset;
    return (void *)aligned_addr;
}

/**
 * @brief Instantly frees ALL memory allocated in the arena. O(1) operation.
 */
void arena_reset(Arena *arena) {
    arena->offset = 0;
    printf("[Arena] Reset complete. All memory batch-freed.\n");
}

// --- Usage Example ---
static uint8_t workspace_ram[256];

int main(void) {
    printf("--- Arena Allocator Demonstration ---\n\n");

    Arena my_arena;
    arena_init(&my_arena, workspace_ram, sizeof(workspace_ram));

    // Allocate structs and buffers with different alignments
    uint32_t *data1 = (uint32_t *)arena_alloc(&my_arena, 4 * sizeof(uint32_t), 4);
    printf("Allocated data1 at %p (Offset: %zu)\n", (void*)data1, my_arena.offset);

    // Force an 8-byte alignment (e.g., for a 64-bit double or DMA buffer)
    double *data2 = (double *)arena_alloc(&my_arena, 2 * sizeof(double), 8);
    printf("Allocated data2 at %p (Offset: %zu)\n", (void*)data2, my_arena.offset);

    // Batch free everything instantly
    arena_reset(&my_arena);

    // Reallocate in the fresh arena
    uint8_t *data3 = (uint8_t *)arena_alloc(&my_arena, 16, 1);
    printf("Re-allocated data3 at %p (Offset: %zu)\n", (void*)data3, my_arena.offset);

    return 0;
}
