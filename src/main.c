#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <math.h>

#include "polygon.h"
#include "world.h"
#include "player.h"

const uint32_t TICK_RATE = 120;
const double TICK_TIME = 1.0 / TICK_RATE;

World world;
Player player;
Camera2D camera;

void tick() {
    PlayerUpdate(&player, &world);
}

void draw() {
    WorldDraw(&world);
    PlayerDraw(&player);
    WorldDrawPost(&world);

    draw_inventory(&world, camera);
}

void reset() {
    WorldReset(&world);
    player = CreatePlayer((Vector2) { 300.0, 300.0 });
    clear_inventory();
    give_shape(3, 60, (Color) { 255, 0, 0, 255 });
}

int main(void) {
    const Vector2 level_size = { 960, 540 };
    InitWindow(level_size.x, level_size.y, "Boolean");
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)
    // SetWindowState(FLAG_WINDOW_RESIZABLE);

    world = LoadWorld("assets/world.png");
    camera = (Camera2D) { 0 };
    camera.target = Vector2Scale(level_size, 0.5);
    reset();

    double next_tick = GetTime();
    while (!WindowShouldClose()) {
        double time = GetTime();
        while (next_tick <= time) {
            tick();
            next_tick += TICK_TIME;
        }

        if (IsKeyPressed(KEY_R)) {
            reset();
        }

        const Vector2 screen_size = { GetScreenWidth(), GetScreenHeight() };
        camera.zoom = fmax(screen_size.x / level_size.x, screen_size.y / level_size.y);
        camera.offset = Vector2Scale(screen_size, 0.5);

        BeginDrawing();
        BeginMode2D(camera);
        draw();
        EndMode2D();
        // DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadWorld(world);
    CloseWindow();
    return 0;
}
