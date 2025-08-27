#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "ui.h"
#include "world.h"
#include "player.h"

Player player;
Camera2D camera;

void tick() {
    PlayerUpdate(&player, &world);
}

void draw() {
    WorldDraw();
    PlayerDraw(&player);
    WorldDrawPost();
}

void load(size_t level) {
    LoadLevel(&levels[level]);
    player = CreatePlayer((Vector2) { 300.0, 300.0 });
    clear_inventory();
    give_shape(6, 60, (Color) { 255, 0, 0, 255 });
    give_shape(4, 40, (Color) { 0, 255, 0, 255 });
}

int main(void) {
    const Vector2 viewport_size = { 960, 540 };
    InitWindow(viewport_size.x, viewport_size.y, "Boolean");
    SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)
    // SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    LoadLevels();
    camera = (Camera2D) { 0 };
    camera.target = Vector2Scale(viewport_size, 0.5);

    size_t level = 0;
    load(level);
    double next_tick = GetTime();
    while (!WindowShouldClose()) {
        double time = GetTime();
        while (next_tick <= time) {
            tick();
            next_tick += 1.0 / 60.0;
        }

        if (IsKeyPressed(KEY_R)) {
            load(level);
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
            ), 0.3));

        BeginDrawing();
        BeginMode2D(camera);
        draw();
        EndMode2D();
        draw_ui(&world, camera);
        // DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadLevels();
    CloseWindow();
    return 0;
}
