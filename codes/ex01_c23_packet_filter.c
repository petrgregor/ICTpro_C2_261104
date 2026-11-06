/**
 * @file ex01_c23_packet_filter.c
 * @brief Reference solution for Section 01 Exercise: C23 Safe Packet Filter.
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// 1. C23 compile-time constants and binary literals with digit separators
constexpr size_t MAX_PAYLOAD_SIZE = 64;
constexpr uint16_t CRITICAL_ERROR_MASK = 0b1000'0000'0000'0001;

// 2. Stable ABI structure with explicit fixed-width types
typedef struct {
    uint32_t sensor_id;
    uint16_t error_flags;
    size_t active_length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} TelemetryPacket;

// 3. Explicit validation, processing, and memory clearing
bool packet_validate_and_process(TelemetryPacket *pkt, uint32_t *out_checksum) {
    // Check pointers using C23 nullptr
    if (pkt == nullptr || out_checksum == nullptr) {
        return false;
    }

    // Reject packet if any critical error bit is active
    if ((pkt->error_flags & CRITICAL_ERROR_MASK) != 0U) {
        printf("[Error] Packet from sensor %u rejected (flags: 0x%04X)\n",
               pkt->sensor_id, pkt->error_flags);
        return false;
    }

    // Bounds check against constexpr limit
    if (pkt->active_length > MAX_PAYLOAD_SIZE) {
        return false;
    }

    // Use C23 typeof to match the exact type of active_length
    typeof(pkt->active_length) i = 0;
    uint32_t checksum = 0;

    while (i < pkt->active_length) {
        checksum += pkt->payload[i];
        i++;
    }

    *out_checksum = checksum;

    // Explicit memory zeroing (no hidden destructors in C)
    memset(pkt->payload, 0, sizeof(pkt->payload));
    pkt->active_length = 0;

    return true;
}

int main(void) {
    printf("=== Exercise 01: C23 Packet Filter Solution ===\n\n");

    TelemetryPacket valid_pkt = {
            .sensor_id = 101U,
            .error_flags = 0b0000'0000'0000'0100, // Non-critical status bit
            .active_length = 4,
            .payload = {10, 20, 30, 40}
    };

    uint32_t checksum = 0;
    if (packet_validate_and_process(&valid_pkt, &checksum)) {
        printf("[Success] Checksum calculated: %u\n", checksum);
        printf("[Security] Payload first byte after memset: %u (active_length: %zu)\n",
               valid_pkt.payload[0], valid_pkt.active_length);
    }

    // Test with critical error flag set
    TelemetryPacket faulty_pkt = {
            .sensor_id = 102U,
            .error_flags = 0b1000'0000'0000'0000, // Matches CRITICAL_ERROR_MASK
            .active_length = 2,
            .payload = {0xFF, 0xFF}
    };

    packet_validate_and_process(&faulty_pkt, &checksum);

    return 0;
}
