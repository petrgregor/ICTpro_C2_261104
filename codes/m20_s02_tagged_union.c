/**
 * @file 34_tagged_union.c
 * @brief Tagged Union (Variant) pattern using C11 anonymous unions.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 34_tagged_union.c -o tagged_union
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// The tags defining the possible states of the variant
typedef enum : uint8_t {
    PAYLOAD_INT,
    PAYLOAD_FLOAT,
    PAYLOAD_COORD
} PayloadType;

// Tagged Union (Variant) structure
typedef struct {
    PayloadType type; // The Tag indicating WHICH union member is valid

    // Anonymous union (C11 feature): members can be accessed directly without a sub-name
    union {
        int32_t int_value;
        float   float_value;

        // Anonymous nested struct for coordinates
        struct {
            int16_t x;
            int16_t y;
        } coord;
    };
} VariantMessage;

/**
 * @brief Processes the message dynamically while guaranteeing type safety.
 */
void process_message(const VariantMessage *msg) {
    if (msg == nullptr) return;

    // We MUST check the tag before accessing ANY union member
    switch (msg->type) {
        case PAYLOAD_INT:
            printf("Received Integer: %d\n", msg->int_value);
            break;
        case PAYLOAD_FLOAT:
            printf("Received Float: %.2f\n", msg->float_value);
            break;
        case PAYLOAD_COORD:
            printf("Received Coordinates: X=%d, Y=%d\n", msg->coord.x, msg->coord.y);
            break;
    }
}

int main(void) {
    printf("--- Tagged Union (Variant) Demonstration ---\n\n");

    // Instantiating different variants using designated initializers
    VariantMessage msg1 = { .type = PAYLOAD_INT, .int_value = 42 };
    VariantMessage msg2 = { .type = PAYLOAD_FLOAT, .float_value = 3.14159f };
    VariantMessage msg3 = { .type = PAYLOAD_COORD, .coord = { .x = 100, .y = -200 } };

    process_message(&msg1);
    process_message(&msg2);
    process_message(&msg3);

    msg1.type = PAYLOAD_FLOAT;
    msg1.float_value = 5.14159f;
    process_message(&msg1);

    return 0;
}
