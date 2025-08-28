#pragma once
#include <raylib.h>
#include <stddef.h>

#define LEVEL_MAX_SHAPES 10

typedef struct {
    Texture2D texture;
    Vector2 starting_position;
    struct {
        Vector2 position;
        int sides, radius;
        Color color;
    } shapes[LEVEL_MAX_SHAPES];
    size_t nshapes;
} Level;

#define LEVEL_COUNT 2
extern Level levels[LEVEL_COUNT];

typedef struct {
    Level *level;
    RenderTexture2D texture;
    Image image;
    struct {
        bool present;
        double scale;
    } shapes[LEVEL_MAX_SHAPES];
} World;

extern World world;

void LoadLevels();
void UnloadLevels();

void LoadLevel(Level *level);
void WorldDraw();
void WorldDrawPost();

void BeginWorldModification();
void EndWorldModification();

unsigned char ColorMin(Color color);
unsigned char ColorMax(Color color);
bool ColorSolid(Color color);
Color WorldSample(Vector2 position);
double WorldRaycast(Vector2 pos, Vector2 step, double max_distance);
