#include "Player.h"
#include "raylib.h"
#include "AudioManager.h"

Player::Player(float x, float y, float w, float h, Texture2D* tex) {
    rect = { x, y, w, h };
    texture = tex;
    bullet.active = false;
    fireTimer = 0.0f;
}

void Player::tryShoot() {
    if (!bullet.active) {
        bullet.rect = { rect.x + rect.width / 2 - 2, rect.y - 10, 4, 10 };
        bullet.velocity = { 0, -BULLET_SPEED };
        bullet.active = true;

        // Play shoot SFX
        AudioManager::Get().PlaySoundEffect("player_shoot");
    }
}

void Player::update() {
    float dt = GetFrameTime();

    // Handle player input
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        rect.x -= SPEED;
        if (rect.x < 0) rect.x = 0;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        rect.x += SPEED;
        if (rect.x + rect.width > GetScreenWidth())
            rect.x = GetScreenWidth() - rect.width;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        rect.y -= SPEED;
        if (rect.y < 0) rect.y = 0;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        rect.y += SPEED;
        if (rect.y + rect.height > GetScreenHeight())
            rect.y = GetScreenHeight() - rect.height;
    }

    // Auto-fire: hold left mouse button or Space to shoot repeatedly
    fireTimer += dt;
    bool wantShoot = IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsKeyDown(KEY_SPACE);
    if (wantShoot && fireTimer >= fireCooldown) {
        tryShoot();
        fireTimer = 0.0f;
    }

    // single-press fallback (fires immediately on press)
    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) && fireTimer > 0.0f) {
        // If fireTimer wasn't yet reset by hold logic, ensure immediate shot
        tryShoot();
        fireTimer = 0.0f;
    }

    // Update bullet
    if (bullet.active) {
        bullet.rect.x += bullet.velocity.x;
        bullet.rect.y += bullet.velocity.y;

        if (bullet.rect.y + bullet.rect.height < 0)
            bullet.active = false;
    }
}

void Player::draw() {
    // Draw player
    if (texture) {
        DrawTexturePro(
            *texture,
            { 0, 0, (float)texture->width, (float)texture->height },
            rect,
            { 0, 0 },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(rect, WHITE);
    }

    // Draw bullet
    if (bullet.active) {
        DrawRectangleRec(bullet.rect, RED);
    }
}
