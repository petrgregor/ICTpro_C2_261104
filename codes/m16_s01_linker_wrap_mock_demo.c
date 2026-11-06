/**
 * @file 56a_linker_wrap_mock_demo.c
 * @brief Live demonstration of GNU Linker substitution (-Wl,--wrap=hardware_i2c_write).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 1. Real production low-level function (would normally access MCU I2C registers)
bool hardware_i2c_write(uint8_t dev_addr, const uint8_t *data, size_t len) {
    (void)dev_addr; (void)data; (void)len;
    printf("  [REAL HW] Accessing physical I2C peripheral registers (Should be intercepted!).\n");
    return true;
}

// 2. Production application logic calling hardware_i2c_write() directly
bool sensor_configure_threshold(uint8_t limit) {
    return hardware_i2c_write(0x48U, &limit, 1U);
}

// 3. Link-time Mock (Compiled with -Wl,--wrap=hardware_i2c_write)
static uint8_t mock_last_byte_written = 0U;
static bool mock_force_bus_error = false;

// Declare original function pointer provided by GNU ld if we ever want to call it
extern bool __real_hardware_i2c_write(uint8_t dev_addr, const uint8_t *data, size_t len);

// Linker intercepts all calls to hardware_i2c_write() and jumps here!
bool __wrap_hardware_i2c_write(uint8_t dev_addr, const uint8_t *data, size_t len) {
    printf("  [LINKER WRAP MOCK] Intercepted call to hardware_i2c_write(addr=0x%02X, len=%zu)!\n",
           dev_addr, len);

    if (mock_force_bus_error) {
        printf("  [LINKER WRAP MOCK] Simulating hardware bus NACK failure!\n");
        return false;
    }
    if (len > 0U && data != nullptr) {
        mock_last_byte_written = data[0];
    }
    return true;
}

int main(void) {
    printf("--- GNU Linker --wrap Mocking Demo ---\n\n");

    printf("1. Calling production function sensor_configure_threshold(0x7F):\n");
    if (sensor_configure_threshold(0x7FU)) {
        printf("   -> Captured byte in mock: 0x%02X\n\n", mock_last_byte_written);
    }

    printf("2. Injecting simulated I2C NACK error via mock_force_bus_error = true:\n");
    mock_force_bus_error = true;
    if (!sensor_configure_threshold(0x7FU)) {
        printf("   -> Production code gracefully detected simulated bus failure!\n");
    }

    return 0;
}
