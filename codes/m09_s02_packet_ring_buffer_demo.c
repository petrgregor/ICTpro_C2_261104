/**
 * @file 59_packet_ring_buffer_demo.c
 * @brief Packet Ring Buffer (FIFO) for variable-length messages.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PACKET_STORAGE_SIZE 64U // Power of two
#define PACKET_STORAGE_MASK (PACKET_STORAGE_SIZE - 1U)

typedef struct {
    uint8_t buffer[PACKET_STORAGE_SIZE];
    uint32_t write_idx;
    uint32_t read_idx;
} PacketRingBuffer;

bool packet_rb_push(PacketRingBuffer *rb, const uint8_t *payload, uint8_t len) {
    uint32_t free_space = PACKET_STORAGE_SIZE - (rb->write_idx - rb->read_idx);
    if (free_space < (uint32_t)(len + 1U)) {
        return false; // Not enough space for 1-byte length header + payload
    }

    // Write 1-byte length header followed by message payload bytes
    rb->buffer[rb->write_idx & PACKET_STORAGE_MASK] = len;
    rb->write_idx++;

    for (uint8_t i = 0; i < len; ++i) {
        rb->buffer[rb->write_idx & PACKET_STORAGE_MASK] = payload[i];
        rb->write_idx++;
    }
    return true;
}

bool packet_rb_pop(PacketRingBuffer *rb, uint8_t *out_buf, uint8_t max_len, uint8_t *out_len) {
    if (rb->write_idx == rb->read_idx) {
        return false; // Buffer empty
    }

    // Read 1-byte length header
    uint8_t pkt_len = rb->buffer[rb->read_idx & PACKET_STORAGE_MASK];
    rb->read_idx++;

    for (uint8_t i = 0; i < pkt_len; ++i) {
        uint8_t b = rb->buffer[rb->read_idx & PACKET_STORAGE_MASK];
        if (i < max_len) {
            out_buf[i] = b;
        }
        rb->read_idx++;
    }
    *out_len = pkt_len;
    return true;
}

int main(void) {
    printf("--- Section 09: Variable-Length Packet Ring Buffer ---\n\n");

    PacketRingBuffer can_rx_queue = {0};

    const char *msg1 = "PING";
    const char *msg2 = "SENSOR_TEMP:24.5C";
    const char *msg3 = "ACK";

    packet_rb_push(&can_rx_queue, (const uint8_t *)msg1, (uint8_t)strlen(msg1));
    packet_rb_push(&can_rx_queue, (const uint8_t *)msg2, (uint8_t)strlen(msg2));
    packet_rb_push(&can_rx_queue, (const uint8_t *)msg3, (uint8_t)strlen(msg3));

    uint8_t rx_frame[32];
    uint8_t frame_len = 0;

    while (packet_rb_pop(&can_rx_queue, rx_frame, sizeof(rx_frame) - 1U, &frame_len)) {
        rx_frame[frame_len] = '\0'; // Null-terminate for printing
        printf("  [Dequeued Packet] Length: %2u bytes | Payload: \"%s\"\n",
               frame_len, (char *)rx_frame);
    }

    return 0;
}
