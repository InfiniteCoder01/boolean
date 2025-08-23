#pragma once
#include <raylib.h>

typedef struct {
    RenderTexture2D texture;
    Image image;
} World;

World LoadWorld(const char *filepath);
void UnloadWorld(World world);
void WorldDraw(World *world);

bool ColorSolid(Color color);
Color WorldSample(World *world, Vector2 position);
double WorldRaycast(World *world, Vector2 pos, Vector2 step, double max_distance);
