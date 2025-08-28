#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "ui.h"
#include "world.h"
#include "player.h"

Player player;
Camera2D camera;

void tick() {
    PlayerUpdate(&player);
}

void draw() {
    WorldDraw();
    PlayerDraw(&player);
    WorldDrawPost();
}

void load(size_t level) {
    LoadLevel(&levels[level]);
    player = CreatePlayer(levels[level].starting_position);
    clear_inventory();
}

void draw_transition(float transition) {
    const double tilt = 200.0;
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();
    const double x1 = Lerp(-width - tilt * 2.0, -tilt, transition);
    const double x2 = x1 + width + tilt;
    const Vector2 points[] = {
        { x1, height },
        { x2, height },
        { x2 + tilt, 0.0 },
        { x1 + tilt, 0.0 },
    };
    DrawTriangleFan(points, sizeof(points) / sizeof(points[0]), BLACK);
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

    size_t level = 0, next_level = LEVEL_COUNT;
    load(level);

    double next_tick = GetTime();
    float transition = 0.0;
    while (!WindowShouldClose()) {
        double time = GetTime();
        while (next_tick <= time) {
            const double delta = 1.0 / 60.0;
            tick();
            next_tick += 1.0 / 60.0;
            if (transition < 2.0) {
                transition += 0.1;
                if (next_level < LEVEL_COUNT && transition > 1.0) transition = 1.0;
            }
        }

        if (IsKeyPressed(KEY_R) || !player.alive) {
            if (next_level == LEVEL_COUNT) {
                next_level = level;
                TraceLog(LOG_ERROR, "%zu, %zu", next_level, level);
                transition = 0.0;
            }
        }
        if (next_level < LEVEL_COUNT) {
            if (transition >= 1.0) {
                load(next_level);
                next_level = LEVEL_COUNT;
            }
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
        draw_transition(transition);
        // DrawFPS(0, 0);
        EndDrawing();
    }

    UnloadLevels();
    CloseWindow();
    return 0;
}
