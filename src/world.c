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

Level levels[1];
World world;

void LoadLevels() {
    postprocess_shader = load_shader("assets/world_postprocess.glsl");
    levels[0] = (Level) {
        .texture = LoadTexture("assets/level0.png"),
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
bool ColorSolid(Color color) {
    unsigned char max = color.r;
    if (color.g > max) max = color.g;
    if (color.b > max) max = color.b;
    return max > 220;
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
