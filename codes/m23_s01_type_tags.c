/**
 * @file 38_type_tags.c
 * @brief Demonstration of Runtime Type Information (RTTI) using Type Tags in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 38_type_tags.c -o type_tags
 */
#include <stdio.h>
#include <stdint.h>

// 1. Enum representing all possible derived classes in our system
typedef enum {
    CLASS_TYPE_BASE,
    CLASS_TYPE_CAR,
    CLASS_TYPE_TRUCK
} ClassType;

// 2. Base structure containing the runtime type information
typedef struct {
    ClassType type_id;
} VehicleBase;

// 3. Derived structures embedding the base structure as their FIRST member
typedef struct {
    VehicleBase base;
    uint32_t top_speed_kmh;
} Car;

typedef struct {
    VehicleBase base;
    uint32_t payload_capacity_kg;
} Truck;

/**
 * @brief Generic function that safely downcasts based on the runtime type tag.
 */
void process_vehicle(VehicleBase *v) {
    if (v == nullptr) return;

    // Safe downcasting logic mimicking C++ dynamic_cast
    switch (v->type_id) {
        case CLASS_TYPE_CAR: {
            Car *c = (Car *)v;
            printf("[CAR] Top speed: %u km/h\n", c->top_speed_kmh);
            break;
        }
        case CLASS_TYPE_TRUCK: {
            Truck *t = (Truck *)v;
            printf("[TRUCK] Payload capacity: %u kg\n", t->payload_capacity_kg);
            break;
        }
        default:
            printf("[UNKNOWN] Base vehicle processed.\n");
            break;
    }
}

int main(void) {
    printf("--- Runtime Type Information (Type Tags) ---\n\n");

    Car my_car = { .base = { .type_id = CLASS_TYPE_CAR }, .top_speed_kmh = 220 };
    Truck my_truck = { .base = { .type_id = CLASS_TYPE_TRUCK }, .payload_capacity_kg = 5000 };

    // Passing derived objects to a generic base pointer
    process_vehicle((VehicleBase *)&my_car);
    process_vehicle((VehicleBase *)&my_truck);

    return 0;
}
