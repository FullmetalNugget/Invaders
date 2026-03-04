#include "background.h"
#include "raylib.h"
#include <cmath>

Background::Background(const char* texturePath, float speed)
{
    texture = LoadTexture(texturePath);
    this->speed = speed;
    scroll = 0.0f;

    loaded = (texture.id != 0);
    if (!loaded) {
        TraceLog(LOG_WARNING, "Background: Failed to load texture '%s' (working dir: %s)", texturePath, GetWorkingDirectory());
        // fallback to full-screen single color by setting a 1x1 placeholder
        texture.width = GetScreenWidth();
        texture.height = GetScreenHeight();
    }
}

Background::~Background()
{
    if (loaded) UnloadTexture(texture);
}

void Background::Update()
{
    scroll += speed * GetFrameTime();

    // Wrap smoothly using fmod so large dt won't jump the background
    if (texture.width > 0)
        scroll = std::fmod(scroll, (float)texture.width);
}

void Background::Draw()
{
    // Guard against zero-sized textures
    int texW = (texture.width > 0) ? texture.width : GetScreenWidth();
    int texH = (texture.height > 0) ? texture.height : GetScreenHeight();

    int startX = -static_cast<int>(scroll);

    if (!loaded) {
        // fallback: draw a simple rectangle background so the scene isn't blank
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), DARKBLUE);
        return;
    }

    for (int y = 0; y < GetScreenHeight(); y += texH) {
        for (int x = startX; x < GetScreenWidth(); x += texW) {
            DrawTexture(texture, x, y, WHITE);
        }
    }
}