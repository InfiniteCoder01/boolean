#include "polygon.h"
#include <raylib.h>
#include "world.h"

Polygon inventory[INVENTORY_SIZE] = { 0 };
int selection = -1;

void clear_inventory() {
    for (size_t i = 0; i < INVENTORY_SIZE; i++) inventory[i].sides = 0;
}

void give_shape(int sides, int radius, Color color) {
    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i].sides == 0) {
            inventory[i].sides = sides;
            inventory[i].radius = radius;
            inventory[i].rotation = 0;
            inventory[i].color = color;
            return;
        }
    }
    TraceLog(LOG_WARNING, "Could not insert shape (%d sides), because inventory is full");
}

void draw_inventory(World *world, Camera2D camera) {
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
    if (selection >= 0) {
        inventory[selection].rotation += GetMouseWheelMove() * -5;
        DrawPoly(
            mouse,
            inventory[selection].sides,
            inventory[selection].radius,
            inventory[selection].rotation,
            inventory[selection].color
        );
    }

    int x = 10, y = 10;
    const int size = 60;
    bool in_ui = false;
    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i].sides == 0) continue;
        Rectangle button = (Rectangle) { x, y, size, size };
        bool hover = CheckCollisionPointRec(GetMousePosition(), button);
        if (hover) in_ui = true;
        DrawRectangleRounded(button, 0.2, 5, (Color) { 255, 255, 255, 128 + hover * 32 });
        DrawPoly(
            (Vector2) { x + size / 2.0, y + size / 2.0 },
            inventory[i].sides,
            inventory[i].radius * 0.3,
            inventory[i].rotation,
            inventory[i].color
        );
        DrawPolyLinesEx(
            (Vector2) { x + size / 2.0, y + size / 2.0 },
            inventory[i].sides,
            inventory[i].radius * 0.3,
            inventory[i].rotation,
            5.0,
            BLACK
        );
        y += size + 5;
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            selection = i;
        }
    }

    bool add = IsMouseButtonPressed(MOUSE_BUTTON_LEFT), subtract = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    if (selection >= 0 && (add || subtract) && !in_ui) {
        BeginWorldModification(world);
        if (subtract) {
            inventory[selection].color.r *= 0.85;
            inventory[selection].color.g *= 0.85;
            inventory[selection].color.b *= 0.85;
        }
        DrawPoly(
            mouse,
            inventory[selection].sides,
            inventory[selection].radius,
            inventory[selection].rotation,
            inventory[selection].color
        );
        EndWorldModification(world);
        inventory[selection].sides = 0;
        selection = -1;
    }
}
