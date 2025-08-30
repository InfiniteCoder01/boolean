#pragma once
#include <raylib.h>
#include <stddef.h>

void LoadInventoryAssets();
void UnloadInventoryAssets();
void clear_inventory();
void give_shape(int sides, int radius, Color color);
void draw_ui(Camera2D camera);
