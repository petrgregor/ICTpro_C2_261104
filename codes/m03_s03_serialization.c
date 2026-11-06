/**
 * @file 37_serialization.c
 * @brief Safe byte-by-byte serialization of structs for network/UART transmission.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 37_serialization.c -o serialization
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint16_t sensor_id;
    uint32_t timestamp;
} PayloadData;

/**
 * @brief Safely serializes the payload into a buffer in Big-Endian order.
 *
 * This approach completely ignores compiler padding and host endianness,
 * making it perfectly safe for cross-platform communication.
 */
void serialize_payload(const PayloadData *data, uint8_t *buffer) {
    if (data == nullptr || buffer == nullptr) return;

    // Pack sensor_id (16-bit)
    buffer[0] = (uint8_t)(data->sensor_id >> 8);
    buffer[1] = (uint8_t)(data->sensor_id & 0xFF);

    // Pack timestamp (32-bit)
    buffer[2] = (uint8_t)(data->timestamp >> 24);
    buffer[3] = (uint8_t)(data->timestamp >> 16);
    buffer[4] = (uint8_t)(data->timestamp >> 8);
    buffer[5] = (uint8_t)(data->timestamp & 0xFF);
}

int main(void) {
    printf("--- Struct Serialization ---\n\n");

    PayloadData my_sensor = {
            .sensor_id = 0x1234,
            .timestamp = 0xDEADBEEF
    };

    // The required buffer size is explicitly 6 bytes (2 + 4)
    uint8_t tx_buffer[6] = {0};

    serialize_payload(&my_sensor, tx_buffer);

    printf("Serialized Byte Stream (Network Order):\n");
    for (int i = 0; i < 6; i++) {
        printf("Buffer[%d]: 0x%02X\n", i, tx_buffer[i]);
    }

    return 0;
}
