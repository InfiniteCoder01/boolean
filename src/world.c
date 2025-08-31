#include "world.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

static Shader load_shader(char *filename) {
    char *code = LoadFileText(filename);
#if defined(PLATFORM_WEB)
    // Override `#version` line for web builds
    code[9] = '1';
    code[10] = '0';
    code[11] = '0';
#endif
    Shader shader = LoadShaderFromMemory(NULL, code);
    UnloadFileText(code);
    return shader;
}

static Shader postprocess_shader;
static bool world_textures_created = false;

Level levels[LEVEL_COUNT];
World world;

void LoadLevels() {
    postprocess_shader = load_shader("assets/world_postprocess.glsl");
    levels[0] = (Level) {
        .texture = LoadTexture("assets/levels/level0.png"),
        .starting_position = (Vector2) { 200.0, 400.0 },
        .shapes = {
            {
                .position = (Vector2) { 450, 350 },
                .sides = 4,
                .radius = 60,
                .color = (Color) { 0, 255, 0, 255 },
            },
            {
                .position = (Vector2) { 1100, 300 },
                .sides = 128,
                .radius = 60,
                .color = (Color) { 255, 0, 0, 255 },
            },
        },
        .nshapes = 2,
    };
    levels[1] = (Level) {
        .texture = LoadTexture("assets/levels/level1.png"),
        .starting_position = (Vector2) { 200.0, 400.0 },
        .shapes = {
            {
                .position = (Vector2) { 1150, 350 },
                .sides = 3,
                .radius = 70,
                .color = (Color) { 255, 100, 0, 255 },
            },
        },
        .nshapes = 1,
    };
    levels[2] = (Level) {
        .texture = LoadTexture("assets/levels/level2.png"),
        .starting_position = (Vector2) { 200.0, 400.0 },
        .shapes = {
            {
                .position = (Vector2) { 880, 350 },
                .sides = 6,
                .radius = 60,
                .color = (Color) { 0, 180, 255, 255 },
            },
        },
        .nshapes = 1,
    };
    levels[3] = (Level) {
        .texture = LoadTexture("assets/levels/level3.png"),
        .starting_position = (Vector2) { 100.0, 400.0 },
        .shapes = {
            {
                .position = (Vector2) { 200, 370 },
                .sides = 4,
                .radius = 60,
                .color = (Color) { 180, 0, 255, 255 },
            },
            {
                .position = (Vector2) { 280, 370 },
                .sides = 4,
                .radius = 60,
                .color = (Color) { 180, 0, 255, 255 },
            },
            {
                .position = (Vector2) { 400, 330 },
                .sides = 5,
                .radius = 60,
                .color = (Color) { 255, 0, 180, 255 },
            },
            {
                .position = (Vector2) { 600, 128 },
                .sides = 5,
                .radius = 60,
                .color = (Color) { 255, 0, 180, 255 },
            },
        },
        .nshapes = 4,
    };
    levels[4] = (Level) {
        .texture = LoadTexture("assets/levels/level4.png"),
        .starting_position = (Vector2) { 200.0, 400.0 },
        .shapes = {
            {
                .position = (Vector2) { 724, 370 },
                .sides = 6,
                .radius = 60,
                .color = (Color) { 255, 30, 200, 255 },
            },
            {
                .position = (Vector2) { 1118, 683 },
                .sides = 3,
                .radius = 60,
                .color = (Color) { 255, 180, 0, 255 },
            },
        },
        .nshapes = 2,
    };
    levels[5] = (Level) {
        .texture = LoadTexture("assets/levels/level5.png"),
        .starting_position = (Vector2) { 200.0, 400.0 },
        .nshapes = 0,
    };
}

void UnloadLevels() {
    UnloadShader(postprocess_shader);
    for (size_t i = 0; i < LEVEL_COUNT; i++) {
        UnloadTexture(levels[i].texture);
    }

    UnloadImage(world.image);
    UnloadRenderTexture(world.texture);
}

void LoadLevel(Level *level) {
    world.level = level;

    if (world_textures_created) UnloadRenderTexture(world.texture);
    world.texture = LoadRenderTexture(level->texture.width, level->texture.height);
    SetTextureWrap(world.texture.texture, TEXTURE_WRAP_CLAMP);

    BeginWorldModification();
    DrawTexture(level->texture, 0, 0, WHITE);
    EndWorldModification();

    for (size_t i = 0; i < level->nshapes; i++) {
        world.shapes[i].present = true;
        world.shapes[i].scale = 1.0;
    }

    world_textures_created = true;
}

// Drawing
void WorldDraw() {
    DrawTextureRec(
        world.texture.texture,
        (Rectangle) { 0, 0, world.texture.texture.width, -world.texture.texture.height },
        Vector2Zero(),
        WHITE
    );
    for (size_t i = 0; i < world.level->nshapes; i++) {
        world.shapes[i].scale += (world.shapes[i].present - world.shapes[i].scale) * 0.1;
        Vector2 position = world.level->shapes[i].position;
        position.y += sin(GetTime() * 2.0 + position.x) * 10.0;
        DrawPoly(
            position,
            world.level->shapes[i].sides,
            world.level->shapes[i].radius * 0.5 * world.shapes[i].scale,
            0.0,
            world.level->shapes[i].color
        );
        DrawPolyLinesEx(
            position,
            world.level->shapes[i].sides,
            world.level->shapes[i].radius * 0.5 * world.shapes[i].scale,
            0.0,
            5.0,
            BLACK
        );
    }
}

void WorldDrawPost() {
    Vector2 size = { world.texture.texture.width, world.texture.texture.height };
    SetShaderValue(
        postprocess_shader,
        GetShaderLocation(postprocess_shader, "worldSize"),
        &size,
        SHADER_UNIFORM_VEC2
    );
    BeginShaderMode(postprocess_shader);
    DrawTextureRec(
        world.texture.texture,
        (Rectangle) { 0, 0, world.texture.texture.width, -world.texture.texture.height },
        Vector2Zero(),
        WHITE
    );
    EndShaderMode();
}

// World modification
void BeginWorldModification() {
    BeginTextureMode(world.texture);
}

void EndWorldModification() {
    EndTextureMode();

    if (world_textures_created) UnloadImage(world.image);
    world.image = LoadImageFromTexture(world.texture.texture);
}

// World sampling
unsigned char ColorMin(Color color) {
    unsigned char min = color.r;
    if (color.g < min) min = color.g;
    if (color.b < min) min = color.b;
    return min;
}

unsigned char ColorMax(Color color) {
    unsigned char max = color.r;
    if (color.g > max) max = color.g;
    if (color.b > max) max = color.b;
    return max;
}

double ColorSat(Color color) {
    const unsigned char max = ColorMax(color);
    const unsigned char min = ColorMin(color);
    return (double)(max - min) / max;
}

bool ColorSolid(Color color) {
    return ColorSat(color) > 0.75 && ColorMax(color) > 220;
}

Color WorldSample(Vector2 position) {
    int x = position.x, y = position.y;
    if (x < 0 || y < 0 || x >= world.image.width || y >= world.image.height) {
        return WHITE;
    }
    return GetImageColor(world.image, position.x, world.texture.texture.height - position.y - 1);
}

double WorldRaycast(Vector2 pos, Vector2 step, double max_distance) {
    const double mag = Vector2Length(step);
    for (int it = 0; it * mag < max_distance; it++) {

        Vector2 p = Vector2Add(pos, Vector2Scale(step, it));
        if (ColorSolid(WorldSample(p))) return it * mag;
    }
    return max_distance;
}
