#include "ui.h"
#include <raylib.h>
#include "world.h"

static Sound place_sound;
void LoadInventoryAssets() {
    place_sound = LoadSound("assets/sounds/place.wav");
}

void UnloadInventoryAssets() {
    UnloadSound(place_sound);
}

#define INVENTORY_SIZE 5
struct {
    int sides, radius, rotation;
    float displayY, displaySize;
    Color color;
    bool present;
} inventory[INVENTORY_SIZE] = { 0 };
int selection = -1;

void clear_inventory() {
    for (size_t i = 0; i < INVENTORY_SIZE; i++) inventory[i].present = 0;
    selection = -1;
}

void give_shape(int sides, int radius, Color color) {
    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        if (!inventory[i].present) {
            inventory[i].sides = sides;
            inventory[i].radius = radius;
            inventory[i].rotation = 0;
            inventory[i].displaySize = 0.0;
            inventory[i].color = color;
            inventory[i].present = true;
            return;
        }
    }
    TraceLog(LOG_WARNING, "Could not insert shape (%d sides), because inventory is full", sides);
}

static bool in_ui = false;
static void draw_inventory(Vector2 mouse) {
    const int size = 60, padding = 10;
    int target_y = padding;
    in_ui = false;

    size_t actual_idx = 0;
    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i].present) {
            // Check if it was just added
            if (inventory[i].displaySize == 0.0) inventory[i].displayY = target_y;
        }
        // Tween size & position
        inventory[i].displaySize += (inventory[i].present - inventory[i].displaySize) * 0.1;
        inventory[i].displayY += (target_y - inventory[i].displayY) * 0.1;

        // Rect
        Rectangle button = (Rectangle) {
            padding + (1.0 - inventory[i].displaySize) * size / 2.0,
            inventory[i].displayY + (1.0 - inventory[i].displaySize) * size / 2.0,
            size * inventory[i].displaySize,
            size * inventory[i].displaySize
        };

        // Draw
        bool hover = CheckCollisionPointRec(GetMousePosition(), button);
        DrawRectangleRounded(button, 0.2, 5, (Color) { 255, 255, 255, 128 + hover * 32 });
        DrawPoly(
            (Vector2) { padding + size / 2.0, inventory[i].displayY + size / 2.0 },
            inventory[i].sides,
            inventory[i].radius * 0.3 * inventory[i].displaySize,
            0,
            inventory[i].color
        );
        DrawPolyLinesEx(
            (Vector2) { padding + size / 2.0, inventory[i].displayY + size / 2.0 },
            inventory[i].sides,
            inventory[i].radius * 0.3 * inventory[i].displaySize,
            0,
            5.0,
            BLACK
        );

        if (inventory[i].present) {
            target_y += size + padding;

            if (hover) in_ui = true;
            if ((hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ONE + actual_idx)) {
                selection = i;
            }
            actual_idx++;
        }
    }
    if (IsKeyPressed(KEY_ZERO)) selection = -1;
}

void draw_ui(Camera2D camera) {
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);
    if (selection >= 0) {
        inventory[selection].rotation += GetMouseWheelMove() * -5;
        BeginMode2D(camera);
        DrawPoly(
            mouse,
            inventory[selection].sides,
            inventory[selection].radius,
            inventory[selection].rotation,
            inventory[selection].color
        );
        EndMode2D();
    }

    draw_inventory(mouse);

    bool add = IsMouseButtonPressed(MOUSE_BUTTON_LEFT), subtract = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    if (selection >= 0 && (add || subtract) && !in_ui) {
        PlaySound(place_sound);
        BeginWorldModification();
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
        EndWorldModification();
        inventory[selection].present = false;
        selection = -1;
    }
}
