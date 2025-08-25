#include "player.h"
#include <raylib.h>
#include <raymath.h>
#include "world.h"

const Vector2 PLAYER_SIZE = { 40.0, 60.0 };

Player CreatePlayer(Vector2 position) {
    return (Player) {
        .position = position,
        .size = PLAYER_SIZE,
        .velocity = Vector2Zero(),
        .squash = 0.0,
        .grounded_time = 0,
        .air_jumps = 0,
    };
}

static bool collides(Player *player, World *world) {
    const Vector2 tl = Vector2Add(player->position, Vector2Multiply(PLAYER_SIZE, (Vector2) { -0.5, -1.0 }));
    for (int x = 0; x <= PLAYER_SIZE.x; x += 2) {
        if (ColorSolid(WorldSample(world, Vector2Add(tl, (Vector2) { x, 0.0 })))) return true;
        if (ColorSolid(WorldSample(world, Vector2Add(tl, (Vector2) { x, PLAYER_SIZE.y })))) return true;
    }
    for (int y = 2; y <= PLAYER_SIZE.y - 2; y += 2) {
        if (ColorSolid(WorldSample(world, Vector2Add(tl, (Vector2) { 0.0, y })))) return true;
        if (ColorSolid(WorldSample(world, Vector2Add(tl, (Vector2) { PLAYER_SIZE.x, y })))) return true;
    }
    return false;
}

static bool move_in_steps(Player *player, World *world, Vector2 v) {
    const int steps = ceil(fabs(v.x + v.y) * 2.0);
    v = Vector2Scale(v, 1.0 / steps);
    for (int i = 1; i <= steps; i++) {
        const Vector2 last_pos = player->position;
        player->position = Vector2Add(player->position, v);
        const Vector2 moved_pos = player->position;
        if (collides(player, world)) {
            Vector2 rv = (Vector2) { -v.y, v.x };
            if (rv.x > 0) rv.x = 1;
            if (rv.x < 0) rv.x = -1;
            if (rv.y > 0) rv.y = 1;
            if (rv.y < 0) rv.y = -1;

            player->position = Vector2Add(moved_pos, rv);
            if (!collides(player, world)) continue;
            player->position = Vector2Subtract(moved_pos, rv);
            if (!collides(player, world)) continue;
            player->position = last_pos;
            return true;
        }
    }
    return false;
}

void PlayerUpdate(Player *player, World *world) {
    bool left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(KEY_H);
    bool right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_L);
    bool jump = IsKeyDown(KEY_SPACE), released = false;
    {
        static bool last_jump = false;
        if (last_jump && jump) jump = false;
        else if (last_jump && !jump) released = true, last_jump = false;
        else last_jump = jump;
    }

    const float target_velocity = (right - left) * (player->grounded_time ? 2.0 : 2.5);
    player->velocity.x += (target_velocity - player->velocity.x) * (player->grounded_time ? 0.1 : 0.06);

    if (jump && (player->grounded_time || player->air_jumps > 0)) {
        player->velocity.y = -10;
        if (!player->grounded_time) player->air_jumps--;
        player->squash = -10.0;
    } else if (released && player->velocity.y < 0) player->velocity.y *= 0.5;

    player->velocity.y += 0.3;
    {
        // Wall slide
        const bool bottom = WorldRaycast(
                world,
                Vector2Add(player->position, (Vector2) { -PLAYER_SIZE.x / 2.0, 0 }),
                (Vector2) { -1.0, 0.0 }, 4.0
            ) < 3.0 || WorldRaycast(
                world,
                Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x / 2.0, 0 }),
                (Vector2) { 1.0, 0.0 }, 4.0
            ) < 3.0;
        const bool center = WorldRaycast(
                world,
                Vector2Add(player->position, (Vector2) { -PLAYER_SIZE.x / 2.0, -PLAYER_SIZE.y / 2.0 }),
                (Vector2) { -1.0, 0.0 }, 4.0
            ) < 3.0 || WorldRaycast(
                world,
                Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x / 2.0, -PLAYER_SIZE.y / 2.0 }),
                (Vector2) { 1.0, 0.0 }, 4.0
            ) < 3.0;
        if ((center || bottom) && player->velocity.y > 1.0) {
            player->velocity.y = 1.0;
            if (center) player->squash = -5.0;
        }
    }
    if (player->grounded_time > 0) player->grounded_time--;

    if (move_in_steps(player, world, Vector2Multiply(player->velocity, (Vector2) { 1, 0 }))) player->velocity.x = 0;
    if (move_in_steps(player, world, Vector2Multiply(player->velocity, (Vector2) { 0, 1 }))) {
        if (player->velocity.y > 0.0) {
            if (!player->grounded_time) {
                player->velocity.x *= 0.2;
                player->squash = 10.0;
            }
            player->grounded_time = 12;
            player->air_jumps = 1;
        }
        player->velocity.y = 0;
    }

    player->size.x += (PLAYER_SIZE.x + player->squash - player->size.x) * 0.3;
    player->size.y = PLAYER_SIZE.x * PLAYER_SIZE.y / player->size.x;
    player->squash *= 0.93;
}


void PlayerDraw(Player *player) {
    DrawRectangleV(
       Vector2Add(player->position, Vector2Multiply(player->size, (Vector2) { -0.5, -1.0 })),
       player->size,
       RAYWHITE
   );
}
