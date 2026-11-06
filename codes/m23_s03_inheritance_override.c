/**
 * @file 40_inheritance_override.c
 * @brief Simulating inheritance and method overriding using VMT in C.
 *
 * Compile: gcc -std=c2x -Wall -Wextra 40_inheritance_override.c -o inheritance
 */
#include <stdio.h>
#include <stdint.h>

// --- 1. Base Class (Shape) ---
typedef struct Shape Shape;

typedef struct {
    void (*draw)(Shape *self);
    float (*get_area)(Shape *self);
} ShapeVTable;

struct Shape {
    const ShapeVTable *vptr;
    uint32_t color_hex;
};

// Generic Base Dispatchers
void shape_draw(Shape *s) {
    if (s && s->vptr && s->vptr->draw) s->vptr->draw(s);
}

float shape_get_area(Shape *s) {
    if (s && s->vptr && s->vptr->get_area) return s->vptr->get_area(s);
    return 0.0f;
}

// --- 2. Derived Class (Circle) ---
typedef struct {
    Shape base; // Inheritance
    float radius;
} Circle;

void circle_draw(Shape *self) {
    Circle *c = (Circle *)self;
    printf("Drawing Circle (Color: #%06X) with radius %.2f\n",
           c->base.color_hex, c->radius);
}

float circle_get_area(Shape *self) {
    Circle *c = (Circle *)self;
    return 3.14159f * c->radius * c->radius;
}

// Specific VMT for Circle (Overrides default behavior)
const ShapeVTable CIRCLE_VMT = {
        .draw = circle_draw,
        .get_area = circle_get_area
};

// --- 3. Derived Class (Rectangle) ---
typedef struct {
    Shape base; // Inheritance
    float width;
    float height;
} Rectangle;

void rect_draw(Shape *self) {
    Rectangle *r = (Rectangle *)self;
    printf("Drawing Rectangle (Color: #%06X) size %.2f x %.2f\n",
           r->base.color_hex, r->width, r->height);
}

float rect_get_area(Shape *self) {
    Rectangle *r = (Rectangle *)self;
    return r->width * r->height;
}

// Specific VMT for Rectangle
const ShapeVTable RECTANGLE_VMT = {
        .draw = rect_draw,
        .get_area = rect_get_area
};

// --- Main Application ---
int main(void) {
    printf("--- Inheritance & Method Overriding ---\n\n");

    Circle my_circle = { .base = { .vptr = &CIRCLE_VMT, .color_hex = 0xFF0000 }, .radius = 5.0f };
    Rectangle my_rect = { .base = { .vptr = &RECTANGLE_VMT, .color_hex = 0x00FF00 }, .width = 4.0f, .height = 3.0f };

    // Polymorphic array of base pointers
    Shape *shapes[] = { (Shape *)&my_circle, (Shape *)&my_rect };

    for (int i = 0; i < 2; i++) {
        shape_draw(shapes[i]);
        printf("  -> Calculated Area: %.2f\n", shape_get_area(shapes[i]));
    }

    return 0;
}
