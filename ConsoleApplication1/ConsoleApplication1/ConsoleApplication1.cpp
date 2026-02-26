#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include "Player.h"

enum GameState { MENU, PLAYING, OPTIONS, EXIT };

void drawFPS() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, PURPLE);
}

#define BOSS_BULLET_SPEED     5
#define BOSS_WIDTH           70
#define BOSS_HEIGHT          40
#define MAX_BULLETS          20

// ----- GLOBAL VARIABLES -----
GameState gameState = MENU;


Bullet playerBullet = {0};
Bullet bossBullets[MAX_BULLETS] = {0};

struct Boss {
    Rectangle rect;
    int health;
    float speed;
    float moveDir;
    float shootTimer;
};

Boss boss = { {265, 50, BOSS_WIDTH, BOSS_HEIGHT}, 5, 2.5f, 1.0f, 0.0f };

int lives = 3;
int score = 0;

// Map to store loaded textures
std::map<std::string, Texture2D> textures;
Player* player;  // global pointer, initially nullptr


// ----- FUNCTION PROTOTYPES -----
void UpdateMenu();
void UpdatePlaying();
void UpdateOptions();
void DrawPlaying();
void HandlePlayerInput();
void HandlePlayerBullet();
void HandleBossBehavior();
void HandleBossBullets();
void CheckCollisions();
void drawFPS();
void loadTextures();

void unload() {
      // Unload all textures
    for (auto &pair : textures) {
        UnloadTexture(pair.second);
    }
    textures.clear();

    delete player;

    CloseWindow();
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(600, 800, "Not so Space Invaders");

    boss.rect = { 265, 50, BOSS_WIDTH, BOSS_HEIGHT };
    boss.health = 5;
    boss.moveDir = 1.0f;
    boss.speed = 2.5f;
    boss.shootTimer = 0.0f;

    Bullet bossBullets[MAX_BULLETS] = { 0 };
    loadTextures();

    player = new Player(100, 500, 64, 64, &textures["player"]);


    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        switch (gameState) {
            case MENU: UpdateMenu(); break;
            case PLAYING: UpdatePlaying(); break;
            case OPTIONS: UpdateOptions(); break;
            case EXIT:
                CloseWindow();
                return 0;
        }

        drawFPS();
        EndDrawing();
    }
    unload();
    return 0;
}

void loadTextures() {
  // List of texture files and IDs
  std::vector<std::pair<std::string, std::string>> textureFiles = {
      {"player", "../img/player.png"},
      {"enemy", "../img/ship.png"},
  };


  for (auto &entry : textureFiles) {
    textures[entry.first] = LoadTexture(entry.second.c_str());
  }

}

// ----- MENU -----
void UpdateMenu() {
    DrawText("Not So Space Invaders", 150, 200, 40, GREEN);

    if (IsKeyPressed(KEY_ENTER)) {
        gameState = PLAYING;
        player->rect.x = 275;
        lives = 3;
        score = 0;
        player->bullet.active = false;
        boss.rect.x = 265;
        boss.health = 5;
        boss.moveDir = 1.0f;
        boss.shootTimer = 0.0f;
        for (int i = 0; i < MAX_BULLETS; i++) bossBullets[i].active = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) gameState = EXIT;
}

void drawClasses() {
  player->update();
  player->draw();
}

// ----- PLAYING -----
void UpdatePlaying() {
    HandleBossBehavior();
    HandleBossBullets();
    CheckCollisions();
    DrawPlaying();
}


void HandleBossBehavior() {
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
                    bossBullets[j].rect = (Rectangle){
                        boss.rect.x + boss.rect.width/2 - 2,
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
}

void HandleBossBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bossBullets[i].active) continue;

        bossBullets[i].rect.x += bossBullets[i].velocity.x;
        bossBullets[i].rect.y += bossBullets[i].velocity.y;

        if (bossBullets[i].rect.y > GetScreenHeight() ||
            bossBullets[i].rect.x + bossBullets[i].rect.width < 0 ||
            bossBullets[i].rect.x > GetScreenWidth()) {
            bossBullets[i].active = false;
        }
    }
}

void CheckCollisions() {
    // Check if the player's bullet hit the boss
    if (player->bullet.active && CheckCollisionRecs(player->bullet.rect, boss.rect)) {
        player->bullet.active = false;
        boss.health--;
        score += 10;
        if (boss.health <= 0) gameState = MENU;
    }

    // Check if any boss bullets hit the player
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bossBullets[i].active && CheckCollisionRecs(bossBullets[i].rect, player->rect)) {
            bossBullets[i].active = false;
            lives--;
            if (lives <= 0) gameState = MENU;
            break;
        }
    }
}

void DrawPlaying() {
    drawClasses();

    DrawRectangleRec(boss.rect, RED);
    DrawRectangle(boss.rect.x, boss.rect.y - 15, boss.rect.width, 10, DARKGRAY);
    DrawRectangle(boss.rect.x, boss.rect.y - 15,
                  boss.rect.width * (boss.health / 5.0f), 10, RED);

    for (int i = 0; i < MAX_BULLETS; i++)
        if (bossBullets[i].active) DrawRectangleRec(bossBullets[i].rect, ORANGE);

    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, WHITE);
    DrawText(TextFormat("Score: %d", score), 10, 70, 20, WHITE);
    DrawText(TextFormat("Boss Health: %d", boss.health), 10, 100, 20, WHITE);
}
// ----- OPTIONS -----
void UpdateOptions() {
    DrawText("OPTIONS - Press ESC to return", 150, 350, 20, LIGHTGRAY);
    if (IsKeyPressed(KEY_ESCAPE)) gameState = MENU;
}
