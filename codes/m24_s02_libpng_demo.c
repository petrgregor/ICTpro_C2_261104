/**
 * @file 48_libpng_demo.c
 * @brief Initialization of libpng structures.
 */
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("--- libpng Demo ---\n");
    printf("Compiled with libpng version: %s\n", PNG_LIBPNG_VER_STRING);

    // 1. Create and initialize the png_struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        printf("Failed to create png_struct.\n");
        return 1;
    }

    // 2. Allocate/initialize the memory for image information
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        printf("Failed to create png_info.\n");
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 1;
    }

    printf("libpng structures successfully initialized in memory.\n");

    // Clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    printf("Memory safely released.\n");

    return 0;
}
