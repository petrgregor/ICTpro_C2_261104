/**
 * @file 47_sdl3_demo.c
 * @brief Basic SDL3 window creation demonstration.
 */
#include <SDL3/SDL.h>
#include <stdio.h>

int main(void) {
    printf("--- SDL3 Demo ---\n");

    // Initialize the video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create a resizable window
    SDL_Window *window = SDL_CreateWindow("Advanced C: SDL3 Demo",
                                          800, 600,
                                          SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    printf("Window successfully created. Waiting 3 seconds...\n");
    SDL_Delay(3000); // Wait for 3000 milliseconds

    // Clean up resources
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("SDL3 shutdown complete.\n");
    return 0;
}
