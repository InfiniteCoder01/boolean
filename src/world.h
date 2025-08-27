#pragma once
#include <raylib.h>

typedef struct {
    Texture2D texture;
} Level;

#define LEVEL_COUNT 1
extern Level levels[LEVEL_COUNT];

typedef struct {
    Level *level;
    RenderTexture2D texture;
    Image image;
} World;

extern World world;

void LoadLevels();
void UnloadLevels();

void LoadLevel(Level *level);
void WorldDraw();
void WorldDrawPost();

void BeginWorldModification();
void EndWorldModification();

bool ColorSolid(Color color);
Color WorldSample(Vector2 position);
double WorldRaycast(Vector2 pos, Vector2 step, double max_distance);
