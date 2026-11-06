/**
 * @file 30_container_of_macro.c
 * @brief Implementing and using the container_of idiom with offsetof.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 30_container_of_macro.c -o container
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Macro to compute the parent structure pointer from a member pointer.
 *
 * @param ptr    Pointer to the member.
 * @param type   Type of the parent container structure.
 * @param member Name of the member within the parent structure.
 */
#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

typedef struct {
    uint64_t timestamp;
    uint32_t hardware_id; // Offset is typically 8 bytes due to alignment
    float    measured_value;
} ComplexSensor;

/**
 * @brief Modifies the parent structure based ONLY on a pointer to hardware_id.
 */
void update_sensor_timestamp(uint32_t *hw_id_ptr, uint64_t new_time) {
    if (hw_id_ptr == nullptr) return;

    // Recover the parent struct pointer by subtracting the known offset
    ComplexSensor *parent = CONTAINER_OF(hw_id_ptr, ComplexSensor, hardware_id);

    parent->timestamp = new_time;
    printf("[Update] Sensor %u updated with timestamp %llu.\n",
           parent->hardware_id, (unsigned long long)parent->timestamp);
}

int main(void) {
    printf("--- container_of Idiom Demonstration ---\n\n");

    ComplexSensor my_sensor = {
            .timestamp = 0,
            .hardware_id = 4096,
            .measured_value = 12.3f
    };

    printf("Original timestamp: %llu\n", (unsigned long long)my_sensor.timestamp);
    printf("Offset of hardware_id: %zu bytes\n", offsetof(ComplexSensor, hardware_id));

    // Passing ONLY the address of the internal member 'hardware_id'
    update_sensor_timestamp(&my_sensor.hardware_id, 1698765432);

    return 0;
}
