/**
 * @file ex03_binary_can_serializer.c
 * @brief Reference solution for Section 03 Exercise: Big-Endian CAN Frame Serializer.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define CAN_FRAME_PAYLOAD_SIZE 7U

// 1. Bit-field structure for compact status representation (1 byte total)
typedef struct {
    uint8_t enable     : 1;
    uint8_t direction  : 1;
    uint8_t error_code : 4;
    uint8_t reserved   : 2;
} MotorFlags;

// 2. High-level command structure (may contain compiler padding in RAM)
typedef struct {
    uint16_t rpm;
    uint32_t timestamp_ms;
    MotorFlags flags;
} MotorCommand;

// Helper to pack bit-fields deterministically into a single byte
static uint8_t pack_flags(MotorFlags f) {
    return (uint8_t)(((f.enable & 0x01U) << 0) |
                     ((f.direction & 0x01U) << 1) |
                     ((f.error_code & 0x0FU) << 2));
}

static MotorFlags unpack_flags(uint8_t raw) {
    MotorFlags f = {
            .enable     = (raw >> 0) & 0x01U,
            .direction  = (raw >> 1) & 0x01U,
            .error_code = (raw >> 2) & 0x0FU,
            .reserved   = 0U
    };
    return f;
}

// 3. Padding-immune Big-Endian serialization
void serialize_motor_cmd(const MotorCommand *cmd, uint8_t buffer[CAN_FRAME_PAYLOAD_SIZE]) {
    if (cmd == nullptr || buffer == nullptr) return;

    // Pack 16-bit RPM (Big-Endian: MSB first)
    buffer[0] = (uint8_t)((cmd->rpm >> 8) & 0xFFU);
    buffer[1] = (uint8_t)(cmd->rpm & 0xFFU);

    // Pack 32-bit Timestamp (Big-Endian)
    buffer[2] = (uint8_t)((cmd->timestamp_ms >> 24) & 0xFFU);
    buffer[3] = (uint8_t)((cmd->timestamp_ms >> 16) & 0xFFU);
    buffer[4] = (uint8_t)((cmd->timestamp_ms >> 8) & 0xFFU);
    buffer[5] = (uint8_t)(cmd->timestamp_ms & 0xFFU);

    // Pack status flags byte
    buffer[6] = pack_flags(cmd->flags);
}

void deserialize_motor_cmd(const uint8_t buffer[CAN_FRAME_PAYLOAD_SIZE], MotorCommand *out_cmd) {
    if (buffer == nullptr || out_cmd == nullptr) return;

    out_cmd->rpm = (uint16_t)(((uint16_t)buffer[0] << 8) |
                              ((uint16_t)buffer[1]));

    out_cmd->timestamp_ms = ((uint32_t)buffer[2] << 24) |
                            ((uint32_t)buffer[3] << 16) |
                            ((uint32_t)buffer[4] << 8)  |
                            ((uint32_t)buffer[5]);

    out_cmd->flags = unpack_flags(buffer[6]);
}

int main(void) {
    printf("=== Exercise 03: Binary CAN Frame Serializer ===\n\n");

    MotorCommand tx_cmd = {
            .rpm = 3500U,
            .timestamp_ms = 0x12345678UL,
            .flags = { .enable = 1, .direction = 1, .error_code = 5, .reserved = 0 }
    };

    uint8_t wire_buffer[CAN_FRAME_PAYLOAD_SIZE] = {0};
    serialize_motor_cmd(&tx_cmd, wire_buffer);

    printf("Serialized Big-Endian bytes (%u B): ", CAN_FRAME_PAYLOAD_SIZE);
    for (size_t i = 0; i < CAN_FRAME_PAYLOAD_SIZE; ++i) {
        printf("%02X ", wire_buffer[i]);
    }
    printf("\n");

    MotorCommand rx_cmd = {0};
    deserialize_motor_cmd(wire_buffer, &rx_cmd);

    printf("Deserialized -> RPM: %u, Timestamp: 0x%08X, Enable: %u, Dir: %u, Err: %u\n",
           rx_cmd.rpm, rx_cmd.timestamp_ms,
           rx_cmd.flags.enable, rx_cmd.flags.direction, rx_cmd.flags.error_code);

    return 0;
}
