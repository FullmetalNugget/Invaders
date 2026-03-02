#include "Boss.h"
#include "raylib.h"
#include <cmath>

void BossBullet::update() {
    if (!active) return;
    rect.x += velocity.x;
    rect.y += velocity.y;

    if (rect.y > GetScreenHeight() || rect.x + rect.width < 0 || rect.x > GetScreenWidth())
        active = false;
}

void BossBullet::draw() {
    if (active)
        DrawRectangleRec(rect, ORANGE);
}

Boss::Boss(float x, float y, float w, float h, Texture2D* tex, int hp, int maxBulletsCount)
    : rect{ x, y, w, h }, texture(tex), health(hp), speed(2.5f), moveDir(1.0f),
    shootTimer(0.0f), maxBullets(maxBulletsCount)
{
    bullets.resize(maxBullets);
}

void Boss::update(float dt) {
    move(dt);
    shoot(dt);

    for (auto& b : bullets)
        b.update();
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
    rect.x += moveDir * speed;

    if (rect.x < 5) { rect.x = 5; moveDir = 1.0f; }
    if (rect.x + rect.width > GetScreenWidth() - 5) { rect.x = GetScreenWidth() - rect.width - 5; moveDir = -1.0f; }
}

void Boss::shoot(float dt) {
    shootTimer += dt;

    if (shootTimer < 3.0f) return;
    shootTimer = 0.0f;

    const int bulletCount = 5;
    const float spreadAngle = 60.0f * DEG2RAD;
    const float startAngle = -spreadAngle / 2.0f;
    const float step = spreadAngle / (bulletCount - 1);

    for (int i = 0; i < bulletCount; i++) {
        for (auto& b : bullets) {
            if (!b.active) {
                float angle = startAngle + i * step;
                b.rect = { rect.x + rect.width / 2.0f - 2, rect.y + rect.height, 4, 10 };
                b.velocity = { sinf(angle) * 5.0f, cosf(angle) * 5.0f };
                b.active = true;
                break;
            }
        }
    }
}