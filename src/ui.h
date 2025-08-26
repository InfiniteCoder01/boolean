#pragma once
#include <raylib.h>
#include <stddef.h>
#include "world.h"

void clear_inventory();
void give_shape(int sides, int radius, Color color);
void draw_ui(World *world, Camera2D camera);
