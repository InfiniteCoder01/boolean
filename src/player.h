#pragma once
#include <raylib.h>
#include "world.h"

typedef struct {
    Vector2 position, size;
    Vector2 velocity;
    float squash;
    int grounded_time;
    int air_jumps;
} Player;

Player CreatePlayer(Vector2 position);
void PlayerUpdate(Player *player, World *world);
void PlayerDraw(Player *player);
