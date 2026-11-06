/**
 * @file ex07_static_memory_pool.c
 * @brief Reference solution for Section 07 Exercise: Deterministic O(1) Memory Pool.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define POOL_BLOCK_SIZE 32U
#define POOL_BLOCK_COUNT 4U

// Union ensures each block is at least 32 bytes while holding 'next' pointer when free
typedef union PoolBlock {
    union PoolBlock *next;
    uint8_t payload[POOL_BLOCK_SIZE];
} PoolBlock;

typedef struct {
    PoolBlock storage[POOL_BLOCK_COUNT];
    PoolBlock *free_list;
    size_t free_count;
} StaticPool;

void pool_init(StaticPool *pool) {
    if (pool == nullptr) return;

    for (size_t i = 0; i < (POOL_BLOCK_COUNT - 1U); ++i) {
        pool->storage[i].next = &pool->storage[i + 1U];
    }
    pool->storage[POOL_BLOCK_COUNT - 1U].next = nullptr;
    pool->free_list = &pool->storage[0];
    pool->free_count = POOL_BLOCK_COUNT;
}

void* pool_alloc(StaticPool *pool) {
    if (pool == nullptr || pool->free_list == nullptr) {
        return nullptr; // Deterministic O(1) out-of-memory handling
    }

    PoolBlock *allocated = pool->free_list;
    pool->free_list = allocated->next;
    pool->free_count--;
    return (void *)allocated->payload;
}

void pool_free(StaticPool *pool, void *ptr) {
    if (pool == nullptr || ptr == nullptr) return;

    PoolBlock *block = (PoolBlock *)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->free_count++;
}

int main(void) {
    printf("=== Exercise 07: Deterministic O(1) Memory Pool ===\n\n");

    StaticPool msg_pool;
    pool_init(&msg_pool);
    printf("Pool initialized. Free blocks: %zu\n", msg_pool.free_count);

    void *blocks[5] = {nullptr};
    for (int i = 0; i < 5; ++i) {
        blocks[i] = pool_alloc(&msg_pool);
        if (blocks[i] != nullptr) {
            printf("  [Alloc #%d] Success -> Address: %p (Remaining free: %zu)\n",
                   i + 1, blocks[i], msg_pool.free_count);
        } else {
            printf("  [Alloc #%d] FAILED  -> Pool exhausted deterministically!\n", i + 1);
        }
    }

    printf("\nFreeing block #2 (%p)...\n", blocks[1]);
    pool_free(&msg_pool, blocks[1]);
    printf("Free blocks available now: %zu\n", msg_pool.free_count);

    void *reused = pool_alloc(&msg_pool);
    printf("Re-allocated block address: %p (Matches freed block #2!)\n", reused);

    return 0;
}
