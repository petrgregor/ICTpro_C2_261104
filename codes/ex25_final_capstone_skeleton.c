/**
 * @file ex25_final_capstone_skeleton.c
 * @brief Student Skeleton Template for the 2-Hour Capstone Project:
 *        Deterministic Industrial Telemetry Node in C23.
 *
 * INSTRUCTIONS FOR STUDENTS:
 * Implement the missing logic marked with "// TODO:" across Phases 1 to 3.
 * Do NOT use malloc() or free(). Run the target to verify your implementation
 * against the automated Unity-style test suite in Phase 4.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <string.h>

// =============================================================================
// 1. C23 CONSTANTS, CACHE ALIGNMENT & INTRUSIVE LIST MACROS
// =============================================================================
constexpr size_t SPSC_FIFO_SIZE = 8U; // Must be a power of two
constexpr size_t SPSC_FIFO_MASK = SPSC_FIFO_SIZE - 1U;
constexpr size_t EVENT_POOL_CAPACITY = 4U;
constexpr size_t SERIALIZED_FRAME_SIZE = 8U;

#define CACHE_LINE_SIZE 32U

// TODO (Task 1.1): Implement macro ALIGN_UP_32(sz) to round up any size to a multiple of 32 bytes
#define ALIGN_UP_32(sz) (sz) // <-- Fix this macro!

#define DMA_TX_BUF_SIZE ALIGN_UP_32(SERIALIZED_FRAME_SIZE)

// TODO (Task 1.2): Implement CONTAINER_OF(ptr, type, member) using offsetof()
#define CONTAINER_OF(ptr, type, member) ((type *)(ptr)) // <-- Fix this macro!

// =============================================================================
// 2. PHASE 1: LOCK-FREE SPSC RING BUFFER (ISR -> TASK)
// =============================================================================
typedef struct {
    uint8_t raw_type;
    uint16_t raw_value;
    uint32_t timestamp_ms;
} RawIsrSample;

typedef struct {
    RawIsrSample slots[SPSC_FIFO_SIZE];
    volatile uint32_t head; // Written only by ISR
    volatile uint32_t tail; // Written only by Worker Task
    atomic_uint_fast32_t dropped_count;
} IsrSampleFifo;

void fifo_init(IsrSampleFifo *fifo) {
    if (fifo == nullptr) return;
    memset(fifo, 0, sizeof(*fifo));
    atomic_init(&fifo->dropped_count, 0U);
}

bool fifo_push_from_isr(IsrSampleFifo *fifo, RawIsrSample sample) {
    // TODO (Task 2.1):
    // 1. Calculate next_head using SPSC_FIFO_MASK.
    // 2. If buffer is full (next_head == tail), atomically increment dropped_count
    //    (memory_order_relaxed) and return false.
    // 3. Store sample, issue atomic_signal_fence(memory_order_release), update head, return true.
    (void)fifo;
    (void)sample;
    return false;
}

bool fifo_pop_in_task(IsrSampleFifo *fifo, RawIsrSample *out_sample) {
    // TODO (Task 2.2):
    // 1. Check if buffer is empty (tail == head). If so, return false.
    // 2. Read slot into *out_sample, issue atomic_signal_fence(memory_order_acquire).
    // 3. Advance tail using SPSC_FIFO_MASK and return true.
    (void)fifo;
    (void)out_sample;
    return false;
}

// =============================================================================
// 3. PHASE 2: TAGGED UNION, BIT-FIELDS, INTRUSIVE LIST & O(1) MEMORY POOL
// =============================================================================
typedef enum : uint8_t {
    EVT_TEMPERATURE    = 1,
    EVT_MOTOR_RPM      = 2,
    EVT_CRITICAL_FAULT = 3
} EventType;

typedef struct {
    uint8_t sensor_online : 1;
    uint8_t low_battery   : 1;
    uint8_t severity      : 3;
    uint8_t reserved      : 3;
} StatusBitfields;

typedef struct ListNode {
    struct ListNode *next;
} ListNode;

typedef struct {
    EventType type;
    StatusBitfields flags;
    uint32_t timestamp_ms;
    union { // C11/C23 anonymous union
        int16_t temp_deci_c;
        uint16_t motor_rpm;
        uint16_t fault_code;
    };
    ListNode queue_node; // Intrusive list anchor
} TelemetryEvent;

typedef union PoolSlot {
    union PoolSlot *next_free;
    TelemetryEvent event;
} PoolSlot;

typedef struct {
    PoolSlot storage[EVENT_POOL_CAPACITY];
    PoolSlot *free_list;
    size_t free_count;
} EventMemoryPool;

void event_pool_init(EventMemoryPool *pool) {
    // TODO (Task 3.1): Link all slots in pool->storage into a singly-linked free_list
    // and initialize free_count to EVENT_POOL_CAPACITY.
    (void)pool;
}

[[nodiscard]] TelemetryEvent* event_pool_alloc(EventMemoryPool *pool) {
    // TODO (Task 3.2): Pop a slot from pool->free_list in O(1) time, decrement free_count,
    // zero-initialize the TelemetryEvent using memset, and return its pointer.
    // Return nullptr if pool is exhausted.
    (void)pool;
    return nullptr;
}

void event_pool_free(EventMemoryPool *pool, TelemetryEvent **evt_ptr) {
    // TODO (Task 3.3): Return the slot pointed to by *evt_ptr back to the front of free_list,
    // increment free_count, and set *evt_ptr = nullptr to prevent dangling pointers.
    (void)pool;
    (void)evt_ptr;
}

// =============================================================================
// 4. PHASE 3: BIG-ENDIAN SERIALIZATION, DMA ALIGNMENT & HAL DI
// =============================================================================
// TODO (Task 4.1): Enforce 32-byte cache line alignment on dma_tx_buffer using alignas()
static uint8_t dma_tx_buffer[DMA_TX_BUF_SIZE];

typedef struct {
    bool (*lock_bus)(void);
    void (*unlock_bus)(void);
    bool (*dma_transmit)(const uint8_t * restrict aligned_data, size_t len);
} RadioHalInterface;

static uint8_t pack_status_bits(StatusBitfields bf) {
    return (uint8_t)(((bf.sensor_online & 0x01U) << 0) |
                     ((bf.low_battery   & 0x01U) << 1) |
                     ((bf.severity      & 0x07U) << 2));
}

void serialize_event_be(const TelemetryEvent * restrict evt, uint8_t * restrict out_buf) {
    // TODO (Task 4.2): Serialize the event into 8 bytes (Big-Endian wire format):
    // Byte 0: evt->type
    // Byte 1: packed status bits (use pack_status_bits)
    // Bytes 2..3: 16-bit payload (MSB at [2], LSB at [3])
    // Bytes 4..7: 32-bit timestamp_ms (MSB at [4] ... LSB at [7])
    (void)evt;
    (void)out_buf;
    (void)pack_status_bits;
}

[[nodiscard]] bool process_and_transmit_queue(ListNode **head_ptr,
                                              EventMemoryPool *pool,
                                              const RadioHalInterface *hal) {
    if (head_ptr == nullptr || pool == nullptr || hal == nullptr) return false;

    // TODO (Task 4.3):
    // 1. Lock the radio bus via hal->lock_bus().
    // 2. Iterate through the intrusive list (*head_ptr).
    // 3. For each node, recover TelemetryEvent* using CONTAINER_OF.
    // 4. Clear dma_tx_buffer, serialize the event, and call hal->dma_transmit().
    // 5. Always free the event back to the pool using event_pool_free().
    // 6. Use the 'goto' single-exit pattern to ensure hal->unlock_bus() is ALWAYS called
    //    even if hal->dma_transmit() fails mid-queue!
    return false;
}

// =============================================================================
// 5. PHASE 4: AUTOMATED VERIFICATION SUITE (DO NOT MODIFY BELOW THIS LINE)
// =============================================================================
#define TEST_ASSERT_TRUE(cond) \
    if (!(cond)) { printf("  [FAIL] Line %d: Condition failed: %s\n", __LINE__, #cond); return false; }
#define TEST_ASSERT_EQUAL_U32(exp, act) \
    if ((uint32_t)(exp) != (uint32_t)(act)) { \
        printf("  [FAIL] Line %d: Expected %u, got %u\n", __LINE__, (uint32_t)(exp), (uint32_t)(act)); return false; }
#define TEST_ASSERT_EQUAL_HEX8(exp, act) \
    if ((uint8_t)(exp) != (uint8_t)(act)) { \
        printf("  [FAIL] Line %d: Expected 0x%02X, got 0x%02X\n", __LINE__, (uint8_t)(exp), (uint8_t)(act)); return false; }

static uint8_t mock_last_tx_frame[SERIALIZED_FRAME_SIZE];
static uint32_t mock_tx_call_count = 0U;
static bool mock_bus_locked = false;
static bool mock_force_dma_fail = false;

static bool mock_radio_lock(void) { mock_bus_locked = true; return true; }
static void mock_radio_unlock(void) { mock_bus_locked = false; }
static bool mock_radio_dma_tx(const uint8_t * restrict aligned_data, size_t len) {
    if (mock_force_dma_fail) return false;
    if (((uintptr_t)aligned_data % CACHE_LINE_SIZE) != 0U) {
        printf("  [DMA ERROR] Buffer address %p is NOT 32-byte aligned!\n", (const void *)aligned_data);
        return false;
    }
    memcpy(mock_last_tx_frame, aligned_data, len);
    mock_tx_call_count++;
    return true;
}

static const RadioHalInterface mock_radio_hal = {
        .lock_bus     = mock_radio_lock,
        .unlock_bus   = mock_radio_unlock,
        .dma_transmit = mock_radio_dma_tx
};

bool test_end_to_end_pipeline_and_big_endian_format(void) {
    IsrSampleFifo fifo;
    EventMemoryPool pool;
    fifo_init(&fifo);
    event_pool_init(&pool);

    mock_tx_call_count = 0U;
    mock_force_dma_fail = false;

    RawIsrSample isr_in = { .raw_type = EVT_MOTOR_RPM, .raw_value = 0x1234U, .timestamp_ms = 0xAABBCCDDUL };
    TEST_ASSERT_TRUE(fifo_push_from_isr(&fifo, isr_in));

    RawIsrSample task_out = {0};
    TEST_ASSERT_TRUE(fifo_pop_in_task(&fifo, &task_out));

    TelemetryEvent *evt = event_pool_alloc(&pool);
    TEST_ASSERT_TRUE(evt != nullptr);
    TEST_ASSERT_EQUAL_U32(EVENT_POOL_CAPACITY - 1U, pool.free_count);

    evt->type = (EventType)task_out.raw_type;
    evt->flags = (StatusBitfields){ .sensor_online = 1, .low_battery = 0, .severity = 3, .reserved = 0 };
    evt->motor_rpm = task_out.raw_value;
    evt->timestamp_ms = task_out.timestamp_ms;
    evt->queue_node.next = nullptr;

    ListNode *queue_head = &evt->queue_node;
    TEST_ASSERT_TRUE(process_and_transmit_queue(&queue_head, &pool, &mock_radio_hal));

    TEST_ASSERT_EQUAL_U32(1U, mock_tx_call_count);
    TEST_ASSERT_EQUAL_U32(EVENT_POOL_CAPACITY, pool.free_count);
    TEST_ASSERT_TRUE(!mock_bus_locked);

    TEST_ASSERT_EQUAL_HEX8(EVT_MOTOR_RPM, mock_last_tx_frame[0]);
    TEST_ASSERT_EQUAL_HEX8(0x12U, mock_last_tx_frame[2]);
    TEST_ASSERT_EQUAL_HEX8(0x34U, mock_last_tx_frame[3]);
    TEST_ASSERT_EQUAL_HEX8(0xAAU, mock_last_tx_frame[4]);
    TEST_ASSERT_EQUAL_HEX8(0xDDU, mock_last_tx_frame[7]);

    printf("  [PASS] test_end_to_end_pipeline_and_big_endian_format\n");
    return true;
}

bool test_regression_pool_exhaustion_and_dma_error_unwinding(void) {
    EventMemoryPool pool;
    event_pool_init(&pool);

    TelemetryEvent *allocated[EVENT_POOL_CAPACITY];
    for (size_t i = 0; i < EVENT_POOL_CAPACITY; ++i) {
        allocated[i] = event_pool_alloc(&pool);
        TEST_ASSERT_TRUE(allocated[i] != nullptr);
    }

    TelemetryEvent *overflow_evt = event_pool_alloc(&pool);
    TEST_ASSERT_TRUE(overflow_evt == nullptr);

    mock_force_dma_fail = true;
    ListNode *head = &allocated[0]->queue_node;
    allocated[0]->queue_node.next = nullptr;

    bool tx_res = process_and_transmit_queue(&head, &pool, &mock_radio_hal);
    TEST_ASSERT_TRUE(!tx_res);
    TEST_ASSERT_TRUE(!mock_bus_locked);

    for (size_t i = 1; i < EVENT_POOL_CAPACITY; ++i) {
        event_pool_free(&pool, &allocated[i]);
    }
    TEST_ASSERT_EQUAL_U32(EVENT_POOL_CAPACITY, pool.free_count);

    printf("  [PASS] test_regression_pool_exhaustion_and_dma_error_unwinding\n");
    return true;
}

int main(void) {
    printf("=== CAPSTONE PROJECT SKELETON: Test Runner ===\n\n");
    bool t1 = test_end_to_end_pipeline_and_big_endian_format();
    bool t2 = test_regression_pool_exhaustion_and_dma_error_unwinding();

    if (t1 && t2) {
        printf("\nCONGRATULATIONS! All Capstone tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep implementing the TODO sections!\n");
        return 1;
    }
}
