#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <vector>

enum GameState { MENU, PLAYING, OPTIONS, EXIT };

void drawFPS() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, PURPLE);
}

#define PLAYER_SPEED         5
#define PLAYER_BULLET_SPEED   8
#define BOSS_BULLET_SPEED     5
#define BOSS_WIDTH           70
#define BOSS_HEIGHT          40
#define MAX_BULLETS          20



typedef struct {
    Rectangle rect;
    bool active;
    Vector2 velocity;
} Bullet;

typedef struct {
    Rectangle rect;
    int health;
    float moveDir;
    float speed;
    float shootTimer;
} Boss;

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(600, 800, "Not so Space Invaders");

    GameState gameState = MENU;

    Rectangle player = { 275, 750, 50, 30 };
    int lives = 3;
    int score = 0;

    Bullet playerBullet = { 0 };
    playerBullet.active = false;

    Boss boss;
    boss.rect = { 265, 50, BOSS_WIDTH, BOSS_HEIGHT };
    boss.health = 5;
    boss.moveDir = 1.0f;
    boss.speed = 2.5f;
    boss.shootTimer = 0.0f;

    Bullet bossBullets[MAX_BULLETS] = { 0 };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (gameState) {
        case MENU:
        {
            DrawText("Not So Space Invaders", 150, 200, 40, GREEN);

            if (IsKeyPressed(KEY_ENTER)) {
                gameState = PLAYING;
                player.x = 275;
                lives = 3;
                score = 0;
                playerBullet.active = false;
                boss.rect.x = 265;
                boss.health = 5;
                boss.moveDir = 1.0f;
                boss.shootTimer = 0.0f;
                for (int i = 0; i < MAX_BULLETS; i++) bossBullets[i].active = false;
            }
            if (IsKeyPressed(KEY_ESCAPE)) gameState = EXIT;
        } break;

        case PLAYING:
        {
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                player.x -= PLAYER_SPEED;
                if (player.x < 0) player.x = 0;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                player.x += PLAYER_SPEED;
                if (player.x + player.width > GetScreenWidth()) player.x = GetScreenWidth() - player.width;
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !playerBullet.active) {
                playerBullet.rect = { player.x + player.width / 2 - 2, player.y - 10, 4, 10 };
                playerBullet.active = true;
                playerBullet.velocity.x = 0;
                playerBullet.velocity.y = -PLAYER_BULLET_SPEED;
            }

            if (playerBullet.active) {
                playerBullet.rect.x += playerBullet.velocity.x;
                playerBullet.rect.y += playerBullet.velocity.y;
                if (playerBullet.rect.y + playerBullet.rect.height < 0) {
                    playerBullet.active = false;
                }
            }

            boss.rect.x += boss.moveDir * boss.speed;
            if (boss.rect.x < 5) {
                boss.rect.x = 5;
                boss.moveDir = 1.0f;
            }
            if (boss.rect.x + boss.rect.width > GetScreenWidth() - 5) {
                boss.rect.x = GetScreenWidth() - boss.rect.width - 5;
                boss.moveDir = -1.0f;
            }

            boss.shootTimer += GetFrameTime();
            if (boss.shootTimer >= 3.0f) {
                boss.shootTimer = 0.0f;
                int bulletCount = 5;
                float spreadAngle = 60.0f * DEG2RAD;
                float startAngle = -spreadAngle / 2;
                float step = spreadAngle / (bulletCount - 1);
                for (int i = 0; i < bulletCount; i++) {
                    for (int j = 0; j < MAX_BULLETS; j++) {
                        if (!bossBullets[j].active) {
                            float angle = startAngle + i * step;
                            bossBullets[j].rect = {
                                boss.rect.x + boss.rect.width / 2 - 2,
                                boss.rect.y + boss.rect.height,
                                4, 10
                            };
                            bossBullets[j].velocity.x = sinf(angle) * BOSS_BULLET_SPEED;
                            bossBullets[j].velocity.y = cosf(angle) * BOSS_BULLET_SPEED;
                            bossBullets[j].active = true;
                            break;
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bossBullets[i].active) {
                    bossBullets[i].rect.x += bossBullets[i].velocity.x;
                    bossBullets[i].rect.y += bossBullets[i].velocity.y;
                    if (bossBullets[i].rect.y > GetScreenHeight() ||
                        bossBullets[i].rect.x + bossBullets[i].rect.width < 0 ||
                        bossBullets[i].rect.x > GetScreenWidth()) {
                        bossBullets[i].active = false;
                    }
                }
            }

            if (playerBullet.active && CheckCollisionRecs(playerBullet.rect, boss.rect)) {
                playerBullet.active = false;
                boss.health--;
                score += 10;
                if (boss.health <= 0) {
                    gameState = MENU;
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bossBullets[i].active && CheckCollisionRecs(bossBullets[i].rect, player)) {
                    bossBullets[i].active = false;
                    lives--;
                    if (lives <= 0) {
                        gameState = MENU;
                    }
                    break;
                }
            }

            DrawRectangleRec(player, GREEN);
            if (playerBullet.active) DrawRectangleRec(playerBullet.rect, YELLOW);
            DrawRectangleRec(boss.rect, RED);
            DrawRectangle(boss.rect.x, boss.rect.y - 15, boss.rect.width, 10, DARKGRAY);
            DrawRectangle(boss.rect.x, boss.rect.y - 15,
                boss.rect.width * (boss.health / 5.0f), 10, RED);
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bossBullets[i].active) DrawRectangleRec(bossBullets[i].rect, ORANGE);
            }

            DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, WHITE);
            DrawText(TextFormat("Score: %d", score), 10, 70, 20, WHITE);
            DrawText(TextFormat("Boss Health: %d", boss.health), 10, 100, 20, WHITE);
        } break;

        case OPTIONS:
            DrawText("OPTIONS - Press ESC to return", 150, 350, 20, LIGHTGRAY);
            if (IsKeyPressed(KEY_ESCAPE)) gameState = MENU;
            break;

        case EXIT:
            CloseWindow();
            return 0;
        }

        drawFPS();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
