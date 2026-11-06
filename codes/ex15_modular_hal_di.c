/**
 * @file ex15_modular_hal_di.c
 * @brief Reference solution for Section 15 Exercise: HAL Interface, Dependency Injection & static state.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// 1. Abstract HAL Interface (Decouples application logic from physical silicon)
typedef struct {
    int16_t (*read_temp_celsius)(void);
    void (*set_cooling_fan)(bool enable);
} ThermalHalInterface;

// 2. Application service structure holding the injected interface
typedef struct {
    const ThermalHalInterface *hal;
    int16_t threshold_celsius;
} ThermalMonitor;

// 3. Private module state (Internal linkage via 'static')
static uint32_t total_alarms_triggered = 0U;

uint32_t thermal_monitor_get_alarm_count(void) {
    return total_alarms_triggered;
}

bool thermal_monitor_step(ThermalMonitor *mon) {
    if (mon == nullptr || mon->hal == nullptr ||
        mon->hal->read_temp_celsius == nullptr || mon->hal->set_cooling_fan == nullptr) {
        return false;
    }

    int16_t current_temp = mon->hal->read_temp_celsius();
    if (current_temp >= mon->threshold_celsius) {
        mon->hal->set_cooling_fan(true);
        total_alarms_triggered++;
    } else {
        mon->hal->set_cooling_fan(false);
    }
    return true;
}

// --- Simulated Hardware Backend injected into the monitor ---
static int16_t simulated_hw_temp = 45;

static int16_t hw_i2c_sensor_read(void) {
    return simulated_hw_temp;
}

static void hw_gpio_fan_control(bool enable) {
    printf("  [HAL GPIO] Cooling fan turned %s (Temp: %d C)\n",
           enable ? "ON " : "OFF", simulated_hw_temp);
}

int main(void) {
    printf("=== Exercise 15: Modularity, HAL & Dependency Injection ===\n\n");

    const ThermalHalInterface stm32_hal = {
            .read_temp_celsius = hw_i2c_sensor_read,
            .set_cooling_fan   = hw_gpio_fan_control
    };

    ThermalMonitor cpu_monitor = {
            .hal = &stm32_hal,
            .threshold_celsius = 70
    };

    simulated_hw_temp = 55;
    thermal_monitor_step(&cpu_monitor);

    simulated_hw_temp = 75; // Exceeds threshold (70 C)
    thermal_monitor_step(&cpu_monitor);

    simulated_hw_temp = 82; // Exceeds threshold again
    thermal_monitor_step(&cpu_monitor);

    printf("\nTotal thermal alarms recorded internally: %u\n",
           thermal_monitor_get_alarm_count());

    return 0;
}
