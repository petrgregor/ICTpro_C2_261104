/**
 * @file 27_macro_templates.c
 * @brief Code generation and metaprogramming using C macros (Token Pasting).
 *
 * Compile: gcc -std=c2x -Wall -Wextra 27_macro_templates.c -o macro_templates
 */
#include <stdio.h>
#include <stddef.h>

// Macro to generate a type-safe generic Vector struct and its init function
// The '##' operator merges tokens to create unique type names (e.g., Vector_int)
#define DEFINE_VECTOR_TYPE(TYPE)                   \
    typedef struct {                               \
        TYPE *data;                                \
        size_t size;                               \
        size_t capacity;                           \
    } Vector_##TYPE;                               \
                                                   \
    void vector_init_##TYPE(Vector_##TYPE *vec) {  \
        if (vec != nullptr) {                      \
            vec->data = nullptr;                   \
            vec->size = 0;                         \
            vec->capacity = 0;                     \
            printf("[Vector] Initialized type-safe Vector_" #TYPE ".\n"); \
        }                                          \
    }

// Instantiate the templates for specific types during compilation
DEFINE_VECTOR_TYPE(int)
DEFINE_VECTOR_TYPE(float)
DEFINE_VECTOR_TYPE(double)

int main(void) {
    printf("--- Macro Code Generation (Templates in C) ---\n\n");

    // Using the dynamically generated types
    Vector_int my_ints;
    Vector_float my_floats;
    Vector_double my_doubles;

    // Using the dynamically generated API functions
    vector_init_int(&my_ints);
    vector_init_float(&my_floats);
    vector_init_double(&my_doubles);

    return 0;
}
