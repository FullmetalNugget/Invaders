#pragma once
#include "raylib.h"
#include <vector>

struct BossBullet {
    Rectangle rect;
    Vector2 velocity;
    bool active = false;

    // Bomb specific
    bool isBomb = false;
    float bombTimer = 0.0f;
    float bombDelay = 1.0f;
    int splitCount = 12;

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

    // bitflags so attacks can be combined
    enum AttackFlag { ATTACK_FAN = 1 << 0, ATTACK_SPIRAL = 1 << 1, ATTACK_BOMB = 1 << 2 };
    using AttackMask = int;

    Boss(float x, float y, float w, float h, Texture2D* tex, int hp = 40, int maxBulletsCount = 60);

    // level: 1..3 (adds attacks cumulatively)
    void setLevel(int lvl);
    int getLevel() const { return level; }

    // you may still set mask directly if needed
    void setAttackMask(AttackMask m) { attackMask = m; }

    void update(float dt);
    void draw();

private:
    void move(float dt);
    void shoot(float dt);

    // helpers
    bool spawnBullet(float x, float y, float angleRad, float speed, bool bomb = false, float bombDelay = 1.0f, int splitCount = 12);
    void explodeBomb(BossBullet& bomb);

    // spiral state
    float spiralAngle;
    float spiralStep;

    // which attacks are active (bitmask)
    AttackMask attackMask;

    // level (1-3)
    int level;

    // per-attack timers & intervals
    float fanTimer, spiralTimer, bombTimer;
    float fanInterval, spiralInterval, bombInterval;
};