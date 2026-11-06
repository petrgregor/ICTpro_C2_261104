/**
 * @file 56_unity_mock_demo.c
 * @brief Self-contained demonstration of Dependency Injection, Mocking, and Unity assertions.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// --- Minimal Unity-style Test Macros for standalone compilation ---
#define TEST_ASSERT_TRUE(cond) \
    if (!(cond)) { printf("  [FAIL] Line %d: Expected TRUE\n", __LINE__); return; }
#define TEST_ASSERT_EQUAL_UINT32(exp, act) \
    if ((exp) != (act)) { printf("  [FAIL] Line %d: Expected %u, got %u\n", __LINE__, (uint32_t)(exp), (uint32_t)(act)); return; }
#define TEST_ASSERT_EQUAL_HEX8(exp, act) \
    if ((exp) != (act)) { printf("  [FAIL] Line %d: Expected 0x%02X, got 0x%02X\n", __LINE__, (uint8_t)(exp), (uint8_t)(act)); return; }

// --- 1. Production Code (Hardware Agnostic) ---
typedef struct {
    bool (*send_byte)(uint8_t byte);
    bool (*receive_byte)(uint8_t *out_byte);
} UartInterface;

typedef struct {
    const UartInterface *io;
    uint32_t packets_sent;
} TelemetryService;

void telemetry_init(TelemetryService *service, const UartInterface *io_interface) {
    service->io = io_interface;
    service->packets_sent = 0;
}

bool telemetry_send_ping(TelemetryService *service) {
    if (service->io == nullptr || service->io->send_byte == nullptr) {
        return false;
    }
    bool ok = service->io->send_byte(0xAA);
    if (ok) service->packets_sent++;
    return ok;
}

// --- 2. Unit Test & Mock Implementation ---
static uint8_t sent_payload[8];
static size_t sent_count = 0;

static bool mock_uart_send(uint8_t byte) {
    if (sent_count < sizeof(sent_payload)) {
        sent_payload[sent_count++] = byte;
        return true;
    }
    return false;
}

void setUp(void) {
    sent_count = 0;
}

void test_telemetry_ping_should_send_magic_byte(void) {
    setUp();
    UartInterface mock_io = { .send_byte = mock_uart_send, .receive_byte = nullptr };
    TelemetryService service;

    telemetry_init(&service, &mock_io);
    bool result = telemetry_send_ping(&service);

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT32(1U, service.packets_sent);
    TEST_ASSERT_EQUAL_HEX8(0xAA, sent_payload[0]);
    printf("  [PASS] test_telemetry_ping_should_send_magic_byte\n");
}

int main(void) {
    printf("--- Section 16: Testable Code, Dependency Injection & Mocking ---\n\n");
    test_telemetry_ping_should_send_magic_byte();
    return 0;
}
