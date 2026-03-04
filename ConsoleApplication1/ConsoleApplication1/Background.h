#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"

class Background
{
public:
    Background(const char* texturePath, float speed);
    ~Background();

    void Update();
    void Draw();

private:
    Texture2D texture;
    float scroll;
    float speed;
    bool loaded; // true if texture loaded correctly
};

#endif