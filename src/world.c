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

World LoadWorld(const char *filepath) {
    Texture2D original = LoadTexture(filepath);
    RenderTexture2D texture = LoadRenderTexture(original.width, original.height);
    SetTextureWrap(texture.texture, TEXTURE_WRAP_CLAMP);

    return (World) {
        .original = original,
        .texture = texture,
        .postprocess_shader = load_shader("assets/world_postprocess.glsl"),
    };
}

void UnloadWorld(World world) {
    UnloadImage(world.image);
    UnloadRenderTexture(world.texture);
}

void WorldReset(World *world) {
    BeginWorldModification(world);
    DrawTexture(world->original, 0, 0, WHITE);
    EndWorldModification(world);
}

void WorldDraw(World *world) {
    DrawTextureRec(world->texture.texture, (Rectangle) { 0, 0, world->texture.texture.width, -world->texture.texture.height }, Vector2Zero(), WHITE);
}

void WorldDrawPost(World *world) {
    Vector2 size = { world->texture.texture.width, world->texture.texture.height };
    SetShaderValue(
        world->postprocess_shader,
        GetShaderLocation(world->postprocess_shader, "worldSize"),
        &size,
        SHADER_UNIFORM_VEC2
    );
    BeginShaderMode(world->postprocess_shader);
    DrawTextureRec(world->texture.texture, (Rectangle) { 0, 0, world->texture.texture.width, -world->texture.texture.height }, Vector2Zero(), WHITE);
    EndShaderMode();
}

// World modification
void BeginWorldModification(World *world) {
    BeginTextureMode(world->texture);
}

void EndWorldModification(World *world) {
    EndTextureMode();

    UnloadImage(world->image);
    world->image = LoadImageFromTexture(world->texture.texture);
}

// World sampling
bool ColorSolid(Color color) {
    unsigned char max = color.r;
    if (color.g > max) max = color.g;
    if (color.b > max) max = color.b;
    return max > 220;
}

Color WorldSample(World *world, Vector2 position) {
    int x = position.x, y = position.y;
    if (x < 0 || y < 0 || x >= world->image.width || y >= world->image.height) {
        return WHITE;
    }
    return GetImageColor(world->image, position.x, world->texture.texture.height - position.y - 1);
}

double WorldRaycast(World *world, Vector2 pos, Vector2 step, double max_distance) {
    const double mag = Vector2Length(step);
    for (int it = 0; it * mag < max_distance; it++) {

        Vector2 p = Vector2Add(pos, Vector2Scale(step, it));
        if (ColorSolid(WorldSample(world, p))) return it * mag;
    }
    return max_distance;
}
