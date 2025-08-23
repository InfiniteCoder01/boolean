#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <math.h>

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
}

int main(void) {
    const Vector2 level_size = { 960, 540 };
    InitWindow(level_size.x, level_size.y, "Boolean");
    // SetWindowState(FLAG_WINDOW_RESIZABLE);

    world = LoadWorld("assets/world.png");
    player = CreatePlayer((Vector2) { 100.0, 130.0 });
    camera = (Camera2D) { 0 };
    camera.target = Vector2Scale(level_size, 0.5);
    
    double next_tick = GetTime();
    while (!WindowShouldClose()) {
        double time = GetTime();
        while (next_tick <= time) {
            tick();
            next_tick += TICK_TIME;
        }

        const Vector2 screen_size = { GetScreenWidth(), GetScreenHeight() };
        camera.zoom = fmax(screen_size.x / level_size.x, screen_size.y / level_size.y);
        camera.offset = Vector2Scale(screen_size, 0.5);

        BeginDrawing();
        BeginMode2D(camera);
        draw();
        EndMode2D();
        DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadWorld(world);
    CloseWindow();
    return 0;
}
