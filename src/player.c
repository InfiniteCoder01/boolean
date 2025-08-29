#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include "world.h"
#include "ui.h"

const Vector2 PLAYER_SIZE = { 40.0, 60.0 };

Player CreatePlayer(Vector2 position) {
    Player player = {
        .position = position,
        .size = PLAYER_SIZE,
        .velocity = Vector2Zero(),
        .squash = 0.0,
        .grounded_time = 0,
        .air_jumps = 0,
        .trail_ptr = 0,
        .sample = (Color) { 216, 216, 216, 255 },
    };
    for (size_t i = 0; i < TRAIL; i++) player.trail[i] = position;
    return player;
}

static bool collides(Player *player) {
    Vector2 size = PLAYER_SIZE;
    size.y *= 0.8;
    const Vector2 tl = Vector2Add(player->position, Vector2Multiply(size, (Vector2) { -0.5, -1.0 }));
    for (int x = 0; x < size.x; x++) {
        if (ColorSolid(WorldSample(Vector2Add(tl, (Vector2) { x, 0.0 })))) return true;
        if (ColorSolid(WorldSample(Vector2Add(tl, (Vector2) { x, size.y })))) return true;
    }
    for (int y = 1; y < size.y - 1; y++) {
        if (ColorSolid(WorldSample(Vector2Add(tl, (Vector2) { 0.0, y })))) return true;
        if (ColorSolid(WorldSample(Vector2Add(tl, (Vector2) { size.x, y })))) return true;
    }
    return false;
}

static bool move_in_steps(Player *player, Vector2 v) {
    const int steps = ceil(fabs(v.x + v.y) * 2.0);
    v = Vector2Scale(v, 1.0 / steps);
    for (int i = 1; i <= steps; i++) {
        const Vector2 last_pos = player->position;
        player->position = Vector2Add(player->position, v);
        const Vector2 moved_pos = player->position;
        if (collides(player)) {
            Vector2 rv = (Vector2) { -v.y, v.x };
            if (rv.x > 0) rv.x = 1;
            if (rv.x < 0) rv.x = -1;
            if (rv.y > 0) rv.y = 1;
            if (rv.y < 0) rv.y = -1;

            player->position = Vector2Add(moved_pos, rv);
            if (!collides(player)) continue;
            player->position = Vector2Subtract(moved_pos, rv);
            if (!collides(player)) continue;
            player->position = last_pos;
            return true;
        }
    }
    return false;
}

void PlayerUpdate(Player *player) {
    bool left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_H);
    bool right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_L);
    bool jump = IsKeyDown(KEY_SPACE), released = false;
    {
        static bool last_jump = false;
        if (last_jump && jump) jump = false;
        else if (last_jump && !jump) released = true, last_jump = false;
        else last_jump = jump;
    }

    const float target_velocity = (right - left) * (player->grounded_time ? 4.0 : 5.0);
    if (ColorMax(player->sample) < 16) player->velocity.x = player->velocity.x < 0 ? -20.0 : 20.0;
    else player->velocity.x += (target_velocity - player->velocity.x) * (player->grounded_time ? 0.2 : 0.08);

    player->velocity.y += 1.0;

    bool jumped = false;
    {
        // Wall slide
        const bool bl = WorldRaycast(
                Vector2Add(player->position, (Vector2) { -PLAYER_SIZE.x / 2.0, 0 }),
                (Vector2) { -1.0, 0.0 }, 4.0
            ) < 3.0;
        const bool br = WorldRaycast(
                Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x / 2.0, 0 }),
                (Vector2) { 1.0, 0.0 }, 4.0
            ) < 3.0;
        const bool cl = WorldRaycast(
                Vector2Add(player->position, (Vector2) { -PLAYER_SIZE.x / 2.0, -PLAYER_SIZE.y / 2.0 }),
                (Vector2) { -1.0, 0.0 }, 4.0
            ) < 3.0;
        const bool cr = WorldRaycast(
                Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x / 2.0, -PLAYER_SIZE.y / 2.0 }),
                (Vector2) { 1.0, 0.0 }, 4.0
            ) < 3.0;
        if (bl || br || cl || cr) {
            if (player->velocity.y > 2.0) {
                player->velocity.y = 2.0;
                if (cl || cr) player->squash = -5.0;
            }
            if (jump && !player->grounded_time) {
                player->velocity.y = -15;
                if (bl || cl) player->velocity.x = 30;
                else player->velocity.x = -30;
                player->squash = 40.0;
                jumped = true;
            }
        }
    }

    if (jump && !jumped && (player->grounded_time || player->air_jumps > 0)) {
        player->velocity.y = -20;
        if (!player->grounded_time) player->air_jumps--;
        player->squash = -20.0;
    } else if (released && player->velocity.y < 0) player->velocity.y *= 0.5;

    if (player->grounded_time > 0) player->grounded_time--;

    if (move_in_steps(player, Vector2Multiply(player->velocity, (Vector2) { 1, 0 }))) player->velocity.x = 0;
    if (move_in_steps(player, Vector2Multiply(player->velocity, (Vector2) { 0, 1 }))) {
        if (player->velocity.y > 0.0) {
            if (!player->grounded_time) {
                player->velocity.x *= 0.2;
                player->squash = 10.0;
            }
            player->grounded_time = 6;
            player->air_jumps = 1;
        }
        player->velocity.y = 0;
    }

    { // Check what's under the player
        player->sample = WorldSample(player->position);
        if (ColorMax(player->sample) < 16) {
            player->velocity.y *= 0.1;
            player->squash = 30.0;
        }
    }

    for (size_t i = 0; i < world.level->nshapes; i++) {
        if (!world.shapes[i].present) continue;
        const int r = world.level->shapes[i].radius;
        if (Vector2DistanceSqr(world.level->shapes[i].position, player->position) < r * r) {
            world.shapes[i].present = false;
            give_shape(
                world.level->shapes[i].sides,
                world.level->shapes[i].radius,
                world.level->shapes[i].color
            );
        }
    }

    player->size.x += (PLAYER_SIZE.x + player->squash - player->size.x) * 0.5;
    player->size.y = PLAYER_SIZE.x * PLAYER_SIZE.y / player->size.x;
    player->squash *= 0.9;
}

void PlayerDraw(Player *player) {
    { // Draw traill
        for (size_t i = 0; i < TRAIL; i++) {
            const Vector2 p = player->trail[(player->trail_ptr + i) % TRAIL];
            DrawRectangleV(
               Vector2Add(p, Vector2Multiply(PLAYER_SIZE, (Vector2) { -0.5, -1.0 })),
               PLAYER_SIZE,
               (Color) { 255, 255, 255, i * 0.15 }
            );
        }
    }
    DrawRectangleV(
       Vector2Add(player->position, Vector2Multiply(player->size, (Vector2) { -0.5, -1.0 })),
       player->size,
       RAYWHITE
    );
    player->trail[player->trail_ptr] = player->position;
    player->trail_ptr = (player->trail_ptr + 1) % TRAIL;
}
