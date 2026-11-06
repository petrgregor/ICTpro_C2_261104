/**
 * @file 43b_hal_interface_di_demo.c
 * @brief Hardware Abstraction Layer (HAL) and Dependency Injection in C (Slide 120).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Abstract hardware interface definition
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
    // High-level logic does not know whether byte goes to UART, USB or a Mock!
    bool ok = service->io->send_byte(0xAA);
    if (ok) service->packets_sent++;
    return ok;
}

// --- Backend 1: Simulated STM32 Hardware UART ---
static bool stm32_uart_send(uint8_t byte) {
    printf("  [STM32 UART1->TDR] Transmitted 0x%02X on physical pin PA9.\n", byte);
    return true;
}

// --- Backend 2: Simulated USB Virtual COM Port ---
static bool usb_cdc_send(uint8_t byte) {
    printf("  [USB CDC Endpoint] Queued 0x%02X into USB packet buffer.\n", byte);
    return true;
}

int main(void) {
    printf("--- Dependency Injection & HAL Interface ---\n\n");

    const UartInterface uart_hw = { .send_byte = stm32_uart_send, .receive_byte = nullptr };
    const UartInterface usb_hw  = { .send_byte = usb_cdc_send,    .receive_byte = nullptr };

    TelemetryService srv;

    printf("1. Injecting STM32 UART backend:\n");
    telemetry_init(&srv, &uart_hw);
    telemetry_send_ping(&srv);

    printf("\n2. Injecting USB CDC backend into the EXACT SAME TelemetryService code:\n");
    telemetry_init(&srv, &usb_hw);
    telemetry_send_ping(&srv);

    return 0;
}
