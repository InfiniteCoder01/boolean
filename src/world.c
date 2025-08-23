#include "world.h"
#include <raylib.h>
#include <raymath.h>

World LoadWorld(const char *filepath) {
    Texture2D raw = LoadTexture(filepath);
    RenderTexture2D texture = LoadRenderTexture(raw.width, raw.height);
    UnloadTexture(texture.texture);
    texture.texture = raw;

    return (World) {
        .texture = texture,
        .image = LoadImageFromTexture(texture.texture),
    };
}

void UnloadWorld(World world) {
    UnloadImage(world.image);
    UnloadRenderTexture(world.texture);
}

void WorldDraw(World *world) {
    DrawTexture(world->texture.texture, 0, 0, WHITE);
}

bool ColorSolid(Color color) {
    return (int)color.r + color.g + color.b > 500;
}

Color WorldSample(World *world, Vector2 position) {
    int x = position.x, y = position.y;
    if (x < 0 || y < 0 || x >= world->image.width || y >= world->image.height) {
        return WHITE;
    }
    return GetImageColor(world->image, position.x, position.y);
}

double WorldRaycast(World *world, Vector2 pos, Vector2 step, double max_distance) {
    const double mag = Vector2Length(step);
    for (int it = 0; it * mag < max_distance; it++) {

        Vector2 p = Vector2Add(pos, Vector2Scale(step, it));
        if (ColorSolid(WorldSample(world, p))) return it * mag;
    }
    return max_distance;
}
