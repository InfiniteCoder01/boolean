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
        .grounded = false,
    };
}

static bool collides(Player *player, World *world) {
    const Vector2 tl = Vector2Add(player->position, Vector2Multiply(PLAYER_SIZE, (Vector2) { -0.5, -1.0 }));
    const Vector2 tr = Vector2Add(tl, (Vector2) { PLAYER_SIZE.x, 0 });
    const Vector2 bl = Vector2Add(tl, (Vector2) { 0, PLAYER_SIZE.y });
    const Vector2 br = Vector2Add(tl, PLAYER_SIZE);
    return ColorSolid(WorldSample(world, tl))
        || ColorSolid(WorldSample(world, tr))
        || ColorSolid(WorldSample(world, bl))
        || ColorSolid(WorldSample(world, br));
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
    bool jump = IsKeyDown(KEY_SPACE);

    const float target_velocity = (right - left) * 1.0;
    player->velocity.x += (target_velocity - player->velocity.x) * (player->grounded ? 0.1 : 0.2);

    const float last_y_vel = player->velocity.y;
    if (jump && player->grounded) player->velocity.y = -5;
    player->velocity.y += 0.1;

    if (move_in_steps(player, world, Vector2Multiply(player->velocity, (Vector2) { 1, 0 }))) player->velocity.x = 0;
    if (move_in_steps(player, world, Vector2Multiply(player->velocity, (Vector2) { 0, 1 }))) player->velocity.y = 0;

    const float y_acceleration = player->velocity.y - last_y_vel;
    player->size.y += (PLAYER_SIZE.y + y_acceleration * 40.0 - player->size.y) * 0.05;
    player->size.x = PLAYER_SIZE.x * PLAYER_SIZE.y / player->size.y;

    const Vector2 bl = Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x * -0.5, 0 });
    const Vector2 bc = player->position;
    const Vector2 br = Vector2Add(player->position, (Vector2) { PLAYER_SIZE.x * 0.5, 0 });
    const double dl = WorldRaycast(world, bl, (Vector2) { 0.0, 0.2 }, 5.0);
    const double dc = WorldRaycast(world, bc, (Vector2) { 0.0, 0.2 }, 5.0);
    const double dr = WorldRaycast(world, br, (Vector2) { 0.0, 0.2 }, 5.0);
    player->grounded = dl < 1.0 || dc < 1.0 || dr < 1.0;
}


void PlayerDraw(Player *player) {
    DrawRectangleV(
       Vector2Add(player->position, Vector2Multiply(player->size, (Vector2) { -0.5, -1.0 })),
       player->size,
       RAYWHITE
   );
}
