#pragma once
#include "raylib.h"
#include <vector>

struct BossBullet {
    Rectangle rect;
    Vector2 velocity;
    bool active = false;

    void update();
    void draw();
};

class Boss {
public:
    Rectangle rect;
    Texture2D* texture;
    int health;
    float speed;
    float moveDir;
    float shootTimer;

    std::vector<BossBullet> bullets;
    int maxBullets;

    Boss(float x, float y, float w, float h, Texture2D* tex, int hp = 40, int maxBulletsCount = 20);

    void update(float dt);
    void draw();

private:
    void move(float dt);
    void shoot(float dt);
};