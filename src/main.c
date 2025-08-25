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
    give_shape(6, 60, (Color) { 255, 0, 0, 255 });
}

#include <stdio.h>
int main(void) {
    const Vector2 viewport_size = { 960, 540 };
    InitWindow(viewport_size.x, viewport_size.y, "Boolean");
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)
    // SetWindowState(FLAG_WINDOW_RESIZABLE);

    world = LoadWorld("assets/world.png");
    camera = (Camera2D) { 0 };
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
        camera.zoom = fmax(screen_size.x / viewport_size.x, screen_size.y / viewport_size.y);
        camera.offset = Vector2Scale(screen_size, 0.5);

        Vector2 camera_target = Vector2Divide(player.position, viewport_size);
        camera_target.x = floor(camera_target.x) + 0.5;
        camera_target.y = floor(camera_target.y) + 0.5;
        camera_target = Vector2Multiply(camera_target, viewport_size);
        camera.target = Vector2Add(
            camera.target,
            Vector2Scale(Vector2Subtract(
                camera_target,
                camera.target
            ), 0.1));
        printf("%f\n", camera.target.y);

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
