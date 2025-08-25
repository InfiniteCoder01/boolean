#pragma once
#include <raylib.h>
#include <stddef.h>
#include "world.h"

typedef struct {
    int sides, radius, rotation;
    Color color;
} Polygon;

#define INVENTORY_SIZE 5
extern Polygon inventory[INVENTORY_SIZE];

void clear_inventory();
void give_shape(int sides, int radius, Color color);
void draw_inventory(World *world, Camera2D camera);
