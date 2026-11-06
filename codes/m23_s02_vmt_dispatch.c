/**
 * @file 39_vmt_dispatch.c
 * @brief Virtual Method Table (VMT) implementation for dynamic dispatch.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 39_vmt_dispatch.c -o vmt_dispatch
 */
#include <stdio.h>
#include <stdint.h>

// Forward declaration of the interface
typedef struct AppLogger AppLogger;

// 1. The Virtual Method Table (VMT) defining the interface contract
typedef struct {
    void (*log_info)(AppLogger *self, const char *message);
    void (*log_error)(AppLogger *self, const char *message);
} LoggerVTable;

// 2. The Base Context containing the pointer to the VMT (vptr)
struct AppLogger {
    const LoggerVTable *vptr;
};

// --- Specific Implementation: Console Logger ---
void console_log_info(AppLogger *self, const char *message) {
    (void)self; // Unused parameter explicitly ignored
    printf("[INFO Console] %s\n", message);
}

void console_log_error(AppLogger *self, const char *message) {
    (void)self;
    printf("[ERROR Console] %s\n", message);
}

// The shared static VMT for all Console Logger instances
const LoggerVTable CONSOLE_LOGGER_VMT = {
        .log_info = console_log_info,
        .log_error = console_log_error
};

// --- Generic Dispatch API ---
void logger_info(AppLogger *logger, const char *msg) {
    if (logger != nullptr && logger->vptr != nullptr && logger->vptr->log_info != nullptr) {
        logger->vptr->log_info(logger, msg); // Dynamic dispatch
    }
}

void logger_error(AppLogger *logger, const char *msg) {
    if (logger != nullptr && logger->vptr != nullptr && logger->vptr->log_error != nullptr) {
        logger->vptr->log_error(logger, msg); // Dynamic dispatch
    }
}

int main(void) {
    printf("--- Virtual Method Table (VMT) ---\n\n");

    // Instantiating a logger and binding it to the Console VMT
    AppLogger my_logger = { .vptr = &CONSOLE_LOGGER_VMT };

    // The user code calls the generic API, unaware of the underlying implementation
    logger_info(&my_logger, "System booted successfully.");
    logger_error(&my_logger, "Failed to open configuration file.");

    return 0;
}
