#pragma once
#include <raylib.h>
#include <stddef.h>
#include "world.h"

#define TRAIL 30

typedef struct {
    Vector2 position, size;
    Vector2 velocity;
    float squash;
    int grounded_time;
    int air_jumps;
    Vector2 trail[30];
    size_t trail_ptr;
} Player;

Player CreatePlayer(Vector2 position);
void PlayerUpdate(Player *player, World *world);
void PlayerDraw(Player *player);
