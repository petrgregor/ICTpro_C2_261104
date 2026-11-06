/**
 * @file ex16_unit_test_i2c_mock.c
 * @brief Reference solution for Section 16 Exercise: Unity Unit Tests with I2C Fault Mocking.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Lightweight Unity-style assertion macros
#define TEST_ASSERT_TRUE(cond) \
    if (!(cond)) { printf("  [FAIL] Line %d: Expected TRUE\n", __LINE__); return; }
#define TEST_ASSERT_FALSE(cond) \
    if (cond) { printf("  [FAIL] Line %d: Expected FALSE\n", __LINE__); return; }
#define TEST_ASSERT_EQUAL_HEX8(exp, act) \
    if ((exp) != (act)) { printf("  [FAIL] Line %d: Expected 0x%02X, got 0x%02X\n", __LINE__, (uint8_t)(exp), (uint8_t)(act)); return; }

// --- 1. Production Code Under Test ---
#define BATTERY_I2C_ADDR 0x55U
#define LOW_VOLTAGE_THRESHOLD_MV 3300U

typedef struct {
    bool (*read_u16)(uint8_t dev_addr, uint16_t *out_val);
} I2cInterface;

bool battery_check_low_voltage(const I2cInterface *bus, bool *out_is_low) {
    if (bus == nullptr || bus->read_u16 == nullptr || out_is_low == nullptr) {
        return false;
    }

    uint16_t millivolts = 0U;
    if (!bus->read_u16(BATTERY_I2C_ADDR, &millivolts)) {
        return false; // I2C bus NACK / communication error
    }

    *out_is_low = (millivolts < LOW_VOLTAGE_THRESHOLD_MV);
    return true;
}

// --- 2. Configurable Mock Implementation ---
static uint16_t mock_return_voltage = 0U;
static bool mock_force_nack_error = false;
static uint8_t mock_last_queried_addr = 0U;

static bool mock_i2c_read_u16(uint8_t dev_addr, uint16_t *out_val) {
    mock_last_queried_addr = dev_addr;
    if (mock_force_nack_error) {
        return false; // Simulate hardware NACK
    }
    *out_val = mock_return_voltage;
    return true;
}

static const I2cInterface mock_bus = { .read_u16 = mock_i2c_read_u16 };

void setUp(void) {
    mock_return_voltage = 3700U;
    mock_force_nack_error = false;
    mock_last_queried_addr = 0U;
}

// --- 3. Unit Test Cases ---
void test_battery_nominal_voltage_should_not_trigger_low_warning(void) {
    setUp();
    mock_return_voltage = 3800U;
    bool is_low = true;

    TEST_ASSERT_TRUE(battery_check_low_voltage(&mock_bus, &is_low));
    TEST_ASSERT_FALSE(is_low);
    TEST_ASSERT_EQUAL_HEX8(BATTERY_I2C_ADDR, mock_last_queried_addr);
    printf("  [PASS] test_battery_nominal_voltage_should_not_trigger_low_warning\n");
}

void test_battery_depleted_voltage_should_trigger_low_warning(void) {
    setUp();
    mock_return_voltage = 3100U;
    bool is_low = false;

    TEST_ASSERT_TRUE(battery_check_low_voltage(&mock_bus, &is_low));
    TEST_ASSERT_TRUE(is_low);
    printf("  [PASS] test_battery_depleted_voltage_should_trigger_low_warning\n");
}

void test_battery_i2c_nack_error_should_return_false(void) {
    setUp();
    mock_force_nack_error = true;
    bool is_low = false;

    TEST_ASSERT_FALSE(battery_check_low_voltage(&mock_bus, &is_low));
    printf("  [PASS] test_battery_i2c_nack_error_should_return_false\n");
}

int main(void) {
    printf("=== Exercise 16: Unit Testing & I2C Bus Mocking ===\n\n");
    test_battery_nominal_voltage_should_not_trigger_low_warning();
    test_battery_depleted_voltage_should_trigger_low_warning();
    test_battery_i2c_nack_error_should_return_false();
    return 0;
}
