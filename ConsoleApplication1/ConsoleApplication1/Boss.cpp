#include "Boss.h"
#include "raylib.h"
#include <cmath>

// BossBullet methods
void BossBullet::update() {
    if (!active || isBomb) return; // bombs handled in Boss::update
    rect.x += velocity.x;
    rect.y += velocity.y;

    if (rect.y > GetScreenHeight() || rect.x + rect.width < 0 || rect.x > GetScreenWidth())
        active = false;
}

void BossBullet::draw() {
    if (!active) return;
    if (isBomb) DrawRectangleRec(rect, PURPLE);
    else DrawRectangleRec(rect, ORANGE);
}

// Boss implementation
Boss::Boss(float x, float y, float w, float h, Texture2D* tex, int hp, int maxBulletsCount)
    : rect{ x, y, w, h }, texture(tex), health(hp), speed(2.5f), moveDir(1.0f),
      shootTimer(0.0f), maxBullets(maxBulletsCount),
      spiralAngle(0.0f), spiralStep(10.0f * DEG2RAD),
      attackMask(ATTACK_FAN), level(1),
      fanTimer(0.0f), spiralTimer(0.0f), bombTimer(0.0f),
      fanInterval(2.8f), spiralInterval(0.06f), bombInterval(1.5f)
{
    bullets.resize(maxBullets);

    // TEST: you can FORCE level 3
    // uncomment the following line:
    // setLevel(3);

    // ensure mask matches initial level
    setLevel(level);
}

void Boss::setLevel(int lvl) {
    if (lvl < 1) lvl = 1;
    if (lvl > 3) lvl = 3;
    level = lvl;

    fanTimer = spiralTimer = bombTimer = 0.0f;
    spiralAngle = 0.0f;
    for (auto &b : bullets) {
        b.active = false;
        b.isBomb = false;
        b.bombTimer = 0.0f;
    }

    attackMask = 0;
    if (level >= 1) attackMask |= ATTACK_FAN;
    if (level >= 2) attackMask |= ATTACK_SPIRAL;
    if (level >= 3) attackMask |= ATTACK_BOMB;
}

void Boss::update(float dt) {
    move(dt);
    shoot(dt);

    // Update bullets
    for (auto& b : bullets) {
        if (!b.active) continue;

        if (b.isBomb) {
            b.bombTimer += dt;
            b.rect.x += b.velocity.x;
            b.rect.y += b.velocity.y;

            if (b.bombTimer >= b.bombDelay) {
                explodeBomb(b);
                b.active = false;
            } else {
                if (b.rect.y > GetScreenHeight() || b.rect.x + b.rect.width < 0 || b.rect.x > GetScreenWidth())
                    b.active = false;
            }
        } else {
            b.update();
        }
    }
}

void Boss::draw() {
    if (texture)
        DrawTextureRec(*texture, { 0,0,(float)rect.width,(float)rect.height }, { rect.x, rect.y }, WHITE);
    else
        DrawRectangleRec(rect, RED);

    for (auto& b : bullets)
        b.draw();

    DrawRectangle(rect.x, rect.y - 15, rect.width, 10, DARKGRAY);
    DrawRectangle(rect.x, rect.y - 15, rect.width * ((float)health / 40), 10, RED);
}

void Boss::move(float dt) {
    rect.x += moveDir * speed * dt * 60.0f;

    if (rect.x < 5) { rect.x = 5; moveDir = 1.0f; }
    if (rect.x + rect.width > GetScreenWidth() - 5) { rect.x = GetScreenWidth() - rect.width - 5; moveDir = -1.0f; }
}

bool Boss::spawnBullet(float x, float y, float angleRad, float spd, bool bomb, float bombDelay, int splitCount) {
    for (auto& b : bullets) {
        if (!b.active) {
            b.rect = { x - 2.0f, y, 4, 10 };
            b.velocity = { sinf(angleRad) * spd, cosf(angleRad) * spd };
            b.active = true;
            b.isBomb = bomb;
            b.bombTimer = 0.0f;
            b.bombDelay = bombDelay;
            b.splitCount = splitCount;
            return true;
        }
    }
    return false;
}

void Boss::explodeBomb(BossBullet& bomb) {
    const int count = (bomb.splitCount > 0) ? bomb.splitCount : 12;
    const float angleStep = (2.0f * PI) / (float)count;
    const float bulletSpeed = 4.0f;

    float cx = bomb.rect.x + bomb.rect.width / 2.0f;
    float cy = bomb.rect.y + bomb.rect.height / 2.0f;

    for (int i = 0; i < count; ++i) {
        float ang = i * angleStep;
        spawnBullet(cx, cy, ang, bulletSpeed, false, 0.0f, 0);
    }
}

void Boss::shoot(float dt) {
    // update per-attack timers
    fanTimer += dt;
    spiralTimer += dt;
    bombTimer += dt;

    const float centerX = rect.x + rect.width / 2.0f;
    const float spawnY = rect.y + rect.height;

    // FAN attack
    if ((attackMask & ATTACK_FAN) && fanTimer >= fanInterval) {
        fanTimer = 0.0f;
        const int bulletCount = 5;
        const float spreadAngle = 60.0f * DEG2RAD;
        const float startAngle = -spreadAngle / 2.0f;
        const float step = spreadAngle / (bulletCount - 1);
        const float spd = 5.0f;

        for (int i = 0; i < bulletCount; i++) {
            float angle = startAngle + i * step;
            spawnBullet(centerX, spawnY, angle, spd, false);
        }
    }

    // SPIRAL attack
    if ((attackMask & ATTACK_SPIRAL) && spiralTimer >= spiralInterval) {
        spiralTimer = 0.0f;
        const float spd = 4.5f;
        spawnBullet(centerX, spawnY, spiralAngle, spd, false);
        spiralAngle += spiralStep;
    }

    // BOMB attack
    if ((attackMask & ATTACK_BOMB) && bombTimer >= bombInterval) {
        bombTimer = 0.0f;
        const float bombSpeed = 2.0f;
        const float bombDelay = 1.0f;   // explode after 1 second
        const int splits = 18;
        float angleDown = 0.0f;
        spawnBullet(centerX, spawnY, angleDown, bombSpeed, true, bombDelay, splits);
    }
}