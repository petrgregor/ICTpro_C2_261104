/**
 * @file 29_simulated_inheritance.c
 * @brief Demonstration of polymorphism using struct embedding in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 29_simulated_inheritance.c -o inheritance
 */
#include <stdio.h>
#include <stdint.h>

// Base "class" containing shared data and a virtual method pointer
typedef struct {
    uint32_t sensor_id;
    // Virtual method receiving a pointer to itself (the base struct)
    void (*process_data)(void *self);
} SensorBase;

// Derived "class" representing a specific Temperature Sensor
typedef struct {
    // CRITICAL: The base struct MUST be the very first member (offset 0)
    SensorBase base;
    float current_temperature;
} TemperatureSensor;

// Derived "class" representing a Humidity Sensor
typedef struct {
    SensorBase base;
    uint8_t humidity_percent;
} HumiditySensor;

// Implementation of the virtual method for the Temperature Sensor
void temp_sensor_process(void *self) {
    // Safe to cast back to the derived type inside its specific method
    TemperatureSensor *temp_self = (TemperatureSensor *)self;
    printf("[Temperature] Sensor %u reads %.2f C\n",
           temp_self->base.sensor_id, temp_self->current_temperature);
}

// Implementation of the virtual method for the Humidity Sensor
void humidity_sensor_process(void *self) {
    HumiditySensor *hum_self = (HumiditySensor *)self;
    printf("[Humidity] Sensor %u reads %u %%\n",
           hum_self->base.sensor_id, hum_self->humidity_percent);
}

// Polymorphic function accepting ANY sensor derived from SensorBase
void generic_sensor_read(SensorBase *sensor) {
    if (sensor != nullptr && sensor->process_data != nullptr) {
        // Dynamic dispatch: calls the appropriate specific function
        sensor->process_data(sensor);
    }
}

int main(void) {
    printf("--- Simulated Inheritance & Polymorphism ---\n\n");

    TemperatureSensor t_sensor = {
            .base = { .sensor_id = 101, .process_data = temp_sensor_process },
            .current_temperature = 24.5f
    };

    HumiditySensor h_sensor = {
            .base = { .sensor_id = 202, .process_data = humidity_sensor_process },
            .humidity_percent = 60
    };

    // The compiler allows casting to the first member's type safely
    generic_sensor_read((SensorBase *)&t_sensor);
    generic_sensor_read((SensorBase *)&h_sensor);

    return 0;
}
