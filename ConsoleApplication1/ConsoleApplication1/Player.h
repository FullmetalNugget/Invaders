#pragma once
#include "raylib.h"

struct Bullet {
    Rectangle rect;
    Vector2 velocity;
    bool active = false;
};

class Player {
public:
    Rectangle rect;
    Texture2D* texture;   // reference to texture from map
    Bullet bullet;         // single bullet

    static constexpr float SPEED = 8.0f;
    static constexpr float BULLET_SPEED = 20.0f;

    Player(float x, float y, float w, float h, Texture2D* tex);

    void update();         // handles input + bullet movement
    void draw();           // draws player + bullet

private:
    // auto-fire support: hold button to shoot every fireCooldown seconds
    float fireCooldown = 0.2f;
    float fireTimer = 0.0f;

    void tryShoot(); // internal helper
};
