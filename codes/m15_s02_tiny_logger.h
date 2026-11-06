/**
 * @file 43_tiny_logger.h
 * @brief A header-only library demonstrating conditional implementation.
 */
#pragma once
#include <stdio.h>

// 1. PUBLIC API (Declarations) - Available everywhere this header is included
void tiny_log(const char *message);

// 2. IMPLEMENTATION - Compiled ONLY in the single translation unit
// that defines TINY_LOGGER_IMPLEMENTATION before including this header.
#ifdef TINY_LOGGER_IMPLEMENTATION

void tiny_log(const char *message) {
    printf("[TINY LOG] %s\n", message);
}

#endif // TINY_LOGGER_IMPLEMENTATION
