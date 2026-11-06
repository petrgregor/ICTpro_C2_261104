/**
 * @file ex25_final_capstone_project.c
 * @brief Complete Reference Solution for the 2-Hour Capstone Project:
 *        Deterministic Industrial Telemetry Node in C23.
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
#define ALIGN_UP_32(sz) (((sz) + (CACHE_LINE_SIZE - 1U)) & ~(CACHE_LINE_SIZE - 1U))
#define DMA_TX_BUF_SIZE ALIGN_UP_32(SERIALIZED_FRAME_SIZE) // Padded to 32 bytes

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

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
    uint32_t curr_head = fifo->head;
    uint32_t next_head = (curr_head + 1U) & SPSC_FIFO_MASK;

    if (next_head == fifo->tail) {
        atomic_fetch_add_explicit(&fifo->dropped_count, 1U, memory_order_relaxed);
        return false; // Buffer full
    }

    fifo->slots[curr_head] = sample;
    atomic_signal_fence(memory_order_release);
    fifo->head = next_head;
    return true;
}

bool fifo_pop_in_task(IsrSampleFifo *fifo, RawIsrSample *out_sample) {
    uint32_t curr_tail = fifo->tail;
    if (curr_tail == fifo->head) {
        return false; // Buffer empty
    }

    *out_sample = fifo->slots[curr_tail];
    atomic_signal_fence(memory_order_acquire);
    fifo->tail = (curr_tail + 1U) & SPSC_FIFO_MASK;
    return true;
}

// =============================================================================
// 3. PHASE 2: TAGGED UNION, BIT-FIELDS, INTRUSIVE LIST & O(1) MEMORY POOL
// =============================================================================
typedef enum : uint8_t {
    EVT_TEMPERATURE = 1,
    EVT_MOTOR_RPM   = 2,
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

// O(1) Deterministic Memory Pool
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
    for (size_t i = 0; i < (EVENT_POOL_CAPACITY - 1U); ++i) {
        pool->storage[i].next_free = &pool->storage[i + 1U];
    }
    pool->storage[EVENT_POOL_CAPACITY - 1U].next_free = nullptr;
    pool->free_list = &pool->storage[0];
    pool->free_count = EVENT_POOL_CAPACITY;
}

[[nodiscard]] TelemetryEvent* event_pool_alloc(EventMemoryPool *pool) {
    if (pool == nullptr || pool->free_list == nullptr) {
        return nullptr;
    }
    PoolSlot *slot = pool->free_list;
    pool->free_list = slot->next_free;
    pool->free_count--;
    memset(&slot->event, 0, sizeof(TelemetryEvent));
    return &slot->event;
}

void event_pool_free(EventMemoryPool *pool, TelemetryEvent **evt_ptr) {
    if (pool == nullptr || evt_ptr == nullptr || *evt_ptr == nullptr) return;
    PoolSlot *slot = (PoolSlot *)(*evt_ptr);
    slot->next_free = pool->free_list;
    pool->free_list = slot;
    pool->free_count++;
    *evt_ptr = nullptr; // Safe pointer invalidation
}

// =============================================================================
// 4. PHASE 3: BIG-ENDIAN SERIALIZATION, DMA ALIGNMENT & HAL DI
// =============================================================================
// Cache-line aligned DMA transmission buffer (prevents False Sharing)
alignas(CACHE_LINE_SIZE) static uint8_t dma_tx_buffer[DMA_TX_BUF_SIZE];

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

// Padding-immune Big-Endian serializer (8 bytes wire format)
void serialize_event_be(const TelemetryEvent * restrict evt, uint8_t * restrict out_buf) {
    out_buf[0] = (uint8_t)evt->type;
    out_buf[1] = pack_status_bits(evt->flags);

    // 16-bit payload in Big-Endian
    uint16_t raw_payload = (evt->type == EVT_TEMPERATURE)
                           ? (uint16_t)evt->temp_deci_c
                           : evt->motor_rpm;
    out_buf[2] = (uint8_t)((raw_payload >> 8) & 0xFFU);
    out_buf[3] = (uint8_t)(raw_payload & 0xFFU);

    // 32-bit timestamp in Big-Endian
    out_buf[4] = (uint8_t)((evt->timestamp_ms >> 24) & 0xFFU);
    out_buf[5] = (uint8_t)((evt->timestamp_ms >> 16) & 0xFFU);
    out_buf[6] = (uint8_t)((evt->timestamp_ms >> 8)  & 0xFFU);
    out_buf[7] = (uint8_t)(evt->timestamp_ms & 0xFFU);
}

// Single-exit unwinding pattern with [[nodiscard]] and goto
[[nodiscard]] bool process_and_transmit_queue(ListNode **head_ptr,
                                              EventMemoryPool *pool,
                                              const RadioHalInterface *hal) {
    if (head_ptr == nullptr || pool == nullptr || hal == nullptr) return false;

    bool all_ok = false;

    if (!hal->lock_bus()) {
        goto cleanup_none;
    }

    while (*head_ptr != nullptr) {
        ListNode *curr_node = *head_ptr;
        *head_ptr = curr_node->next;

        // Recover parent TelemetryEvent via intrusive CONTAINER_OF macro
        TelemetryEvent *evt = CONTAINER_OF(curr_node, TelemetryEvent, queue_node);

        memset(dma_tx_buffer, 0, sizeof(dma_tx_buffer));
        serialize_event_be(evt, dma_tx_buffer);

        bool tx_ok = hal->dma_transmit(dma_tx_buffer, SERIALIZED_FRAME_SIZE);

        // Always return the event block to the O(1) Memory Pool
        event_pool_free(pool, &evt);

        if (!tx_ok) {
            goto cleanup_bus; // Abort on DMA error while keeping bus unlocked via goto
        }
    }

    all_ok = true;

    cleanup_bus:
    hal->unlock_bus();
    cleanup_none:
    return all_ok;
}

// =============================================================================
// 5. PHASE 4: UNITY-STYLE MOCKS & AUTOMATED TEST SUITE
// =============================================================================
#define TEST_ASSERT_TRUE(cond) \
    if (!(cond)) { printf("  [FAIL] Line %d: %s\n", __LINE__, #cond); return false; }
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

static bool mock_radio_lock(void) {
    mock_bus_locked = true;
    return true;
}
static void mock_radio_unlock(void) {
    mock_bus_locked = false;
}
static bool mock_radio_dma_tx(const uint8_t * restrict aligned_data, size_t len) {
    if (mock_force_dma_fail) return false;
    // Verify 32-byte cache line alignment of incoming DMA pointer
    if (((uintptr_t)aligned_data % CACHE_LINE_SIZE) != 0U) return false;
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

    // 1. Simulate ISR pushing a sensor reading
    RawIsrSample isr_in = { .raw_type = EVT_MOTOR_RPM, .raw_value = 0x1234U, .timestamp_ms = 0xAABBCCDDUL };
    TEST_ASSERT_TRUE(fifo_push_from_isr(&fifo, isr_in));

    // 2. Worker task pops from FIFO and allocates from O(1) Memory Pool
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

    // 3. Process intrusive queue and transmit via Mock HAL
    ListNode *queue_head = &evt->queue_node;
    TEST_ASSERT_TRUE(process_and_transmit_queue(&queue_head, &pool, &mock_radio_hal));

    // 4. Verify Big-Endian bytes, pool recycling, and bus unlock
    TEST_ASSERT_EQUAL_U32(1U, mock_tx_call_count);
    TEST_ASSERT_EQUAL_U32(EVENT_POOL_CAPACITY, pool.free_count); // Recycled!
    TEST_ASSERT_TRUE(!mock_bus_locked); // Cleaned up via goto!

    TEST_ASSERT_EQUAL_HEX8(EVT_MOTOR_RPM, mock_last_tx_frame[0]);
    TEST_ASSERT_EQUAL_HEX8(0x12U, mock_last_tx_frame[2]); // MSB of 0x1234
    TEST_ASSERT_EQUAL_HEX8(0x34U, mock_last_tx_frame[3]); // LSB of 0x1234
    TEST_ASSERT_EQUAL_HEX8(0xAAU, mock_last_tx_frame[4]); // MSB of 0xAABBCCDD
    TEST_ASSERT_EQUAL_HEX8(0xDDU, mock_last_tx_frame[7]); // LSB of 0xAABBCCDD

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

    // 5th allocation must deterministically return nullptr without crashing
    TelemetryEvent *overflow_evt = event_pool_alloc(&pool);
    TEST_ASSERT_TRUE(overflow_evt == nullptr);

    // Test goto cleanup when DMA fails mid-transmission
    mock_force_dma_fail = true;
    ListNode *head = &allocated[0]->queue_node;
    allocated[0]->queue_node.next = nullptr;

    bool tx_res = process_and_transmit_queue(&head, &pool, &mock_radio_hal);
    TEST_ASSERT_TRUE(!tx_res);
    TEST_ASSERT_TRUE(!mock_bus_locked); // Bus MUST be unlocked even on failure!

    // Free remaining 3 blocks
    for (size_t i = 1; i < EVENT_POOL_CAPACITY; ++i) {
        event_pool_free(&pool, &allocated[i]);
    }
    TEST_ASSERT_EQUAL_U32(EVENT_POOL_CAPACITY, pool.free_count);

    printf("  [PASS] test_regression_pool_exhaustion_and_dma_error_unwinding\n");
    return true;
}

int main(void) {
    printf("=== CAPSTONE PROJECT: Industrial Telemetry Node (C23) ===\n\n");
    test_end_to_end_pipeline_and_big_endian_format();
    test_regression_pool_exhaustion_and_dma_error_unwinding();
    return 0;
}
