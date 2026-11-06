/**
 * @file 11_memory_pool.c
 * @brief Demonstration of a Fixed-Size Block Memory Pool for embedded systems.
 *
 * Compile: gcc -std=c2x -Wall -Wextra -O2 11_memory_pool.c -o mempool
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

// A single block in the memory pool.
// When free, it stores a pointer to the next free block.
// When allocated, this space is completely overwritten by user data.
typedef struct MemoryBlock {
    struct MemoryBlock *next;
} MemoryBlock;

// The Memory Pool controller
typedef struct {
    MemoryBlock *free_list;
    size_t block_size;
    size_t total_blocks;
} MemoryPool;

/**
 * @brief Initializes the memory pool by linking all blocks into a free list.
 */
void pool_init(MemoryPool *pool, uint8_t *raw_memory, size_t block_size, size_t block_count) {
    pool->block_size = block_size;
    pool->total_blocks = block_count;
    pool->free_list = nullptr;

    // Link blocks backwards so the first block is at the head of the list
    for (size_t i = block_count; i > 0; --i) {
        MemoryBlock *block = (MemoryBlock *)(raw_memory + ((i - 1) * block_size));
        block->next = pool->free_list;
        pool->free_list = block;
    }
    printf("[Pool] Initialized with %zu blocks of %zu bytes.\n", block_count, block_size);
}

/**
 * @brief Allocates a block from the pool in O(1) time.
 */
void* pool_alloc(MemoryPool *pool) {
    if (pool->free_list == nullptr) {
        printf("[Pool] ERROR: Out of memory! Deterministic failure.\n");
        return nullptr;
    }
    MemoryBlock *allocated_block = pool->free_list;
    pool->free_list = allocated_block->next; // Move head to the next free block
    return (void *)allocated_block;
}

/**
 * @brief Frees a block back into the pool in O(1) time.
 */
void pool_free(MemoryPool *pool, void *ptr) {
    if (ptr == nullptr) return;

    MemoryBlock *block = (MemoryBlock *)ptr;
    block->next = pool->free_list;
    pool->free_list = block; // Push back to the head of the free list
}

// --- Usage Example ---
#define POOL_SIZE 4
#define BLOCK_SIZE 32

static uint8_t pool_storage[POOL_SIZE * BLOCK_SIZE];

int main(void) {
    printf("--- Memory Pool Demonstration ---\n\n");

    MemoryPool my_pool;
    pool_init(&my_pool, pool_storage, BLOCK_SIZE, POOL_SIZE);

    void *allocs[5] = {nullptr};

    // Allocate all available blocks
    for (int i = 0; i < POOL_SIZE; i++) {
        allocs[i] = pool_alloc(&my_pool);
        printf("Allocated block %d at %p\n", i + 1, allocs[i]);
    }

    // Attempt to allocate one more (should fail safely)
    allocs[4] = pool_alloc(&my_pool);

    // Free a block to prevent fragmentation
    printf("\nFreeing block 2...\n");
    pool_free(&my_pool, allocs[1]);

    // Allocate again (should instantly reuse the freed block)
    allocs[4] = pool_alloc(&my_pool);
    printf("Allocated new block at %p (Reused block 2!)\n", allocs[4]);

    return 0;
}
