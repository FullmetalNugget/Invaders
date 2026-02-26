#include "Player.h"
#include "raylib.h"

Player::Player(float x, float y, float w, float h, Texture2D* tex) {
    rect = {x, y, w, h};
    texture = tex;
    bullet.active = false;
}

void Player::update() {
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

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !bullet.active) {
        bullet.rect = { rect.x + rect.width/2 - 2, rect.y - 10, 4, 10 };
        bullet.velocity = {0, -BULLET_SPEED};
        bullet.active = true;
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
    DrawTexturePro(
        *texture,
        {0, 0, (float)texture->width, (float)texture->height},
        rect,
        {0, 0},
        0.0f,
        WHITE
    );

    // Draw bullet
    if (bullet.active) {
        DrawRectangleRec(bullet.rect, RED);
    }
}
