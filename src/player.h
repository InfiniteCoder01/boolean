#pragma once
#include <raylib.h>
#include "world.h"

typedef struct {
    Vector2 position, size;
    Vector2 velocity;
    bool grounded;
} Player;

Player CreatePlayer(Vector2 position);
void PlayerUpdate(Player *player, World *world);
void PlayerDraw(Player *player);
