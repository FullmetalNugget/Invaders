#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Player.h"
#include "Boss.h"
#include "background.h"
#include "AudioManager.h"
#include "Menu.h"

enum GameState { MENU, PLAYING, OPTIONS, EXIT };

// ----- GLOBAL VARIABLES -----
GameState gameState = MENU;

int lives = 3;
int score = 0;
Rectangle mapBounds = { 0, 0, 600, 800 };

// Player and Boss pointers
Player* player = nullptr;
Boss* boss = nullptr;

// Texture storage
std::map<std::string, Texture2D> textures;

// Current level selection (1..3)
int currentLevel = 1;

// Menu instance
Menu menu;

// ----- FUNCTION PROTOTYPES -----
void UpdatePlaying();
void UpdateOptions();
void DrawPlaying();
void PlayerCollision();
void CheckCollisions();
void loadTextures();
void unload();
void drawFPS();

// ----- MAIN -----
int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(600, 800, "Not so Space Invaders");

    // Initialize audio and load background music via AudioManager
    AudioManager::Get().Init();
    if (AudioManager::Get().LoadMusic("../music/Music.mp3")) {
        AudioManager::Get().SetMusicVolume(0.5f);
        AudioManager::Get().PlayMusic();
        AudioManager::Get().LoadSoundEffect("player_shoot", "../music/player_shoot.mp3");
    }

    Background bg("../img/background.jpg", 100.0f);

    loadTextures();

    player = new Player(100, 500, 34, 34, &textures["player"]);
    boss = new Boss(265, 50, 70, 40, &textures["enemy"]);

    bool musicPlaying = AudioManager::Get().IsMusicPlaying();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Keep the music stream updated each frame
        if (AudioManager::Get().IsMusicLoaded()) {
            AudioManager::Get().UpdateMusic();
        }

        // Mute (M)
        if (IsKeyPressed(KEY_M) && AudioManager::Get().IsMusicLoaded()) {
            if (musicPlaying) {
                AudioManager::Get().PauseMusic();
                musicPlaying = false;
            } else {
                AudioManager::Get().ResumeMusic();
                musicPlaying = true;
            }
        }

        bg.Update();

        BeginDrawing();
        ClearBackground(BLACK);
        bg.Draw();

        switch (gameState) {
        case MENU:
            menu.Update();
            menu.Draw();
            if (menu.IsStartRequested()) {
                // start playing with selected level
                currentLevel = menu.GetSelectedLevel();
                gameState = PLAYING;

                // Reset player
                player->rect.x = 275;
                player->rect.y = 500;
                player->bullet.active = false;

                lives = 3;
                score = 0;

                // Reset and set boss level (use setLevel to enable cumulative attacks)
                boss->rect.x = 265;
                boss->rect.y = 50;
                boss->health = 40;
                boss->moveDir = 1.0f;
                boss->shootTimer = 0.0f;
                boss->setLevel(currentLevel);
                for (auto& b : boss->bullets) b.active = false;

                menu.ResetStart(); // clear the start request
            }
            break;

        case PLAYING:
            player->update();
            boss->update(dt);
            PlayerCollision();
            CheckCollisions();
            DrawPlaying();
            break;

        case OPTIONS:
            UpdateOptions();
            break;

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

// ----- LOAD TEXTURES -----
void loadTextures() {
    std::vector<std::pair<std::string, std::string>> textureFiles = {
        {"player", "../img/player.png"},
        {"enemy", "../img/ship.png"},
    };

    for (auto& entry : textureFiles)
        textures[entry.first] = LoadTexture(entry.second.c_str());
}

// ----- UNLOAD -----
void unload() {
    for (auto& pair : textures)
        UnloadTexture(pair.second);
    textures.clear();

    delete player;
    delete boss;

    // Stop and let AudioManager destructor clean up on shutdown
    if (AudioManager::Get().IsMusicLoaded()) {
        AudioManager::Get().StopMusic();
    }

    CloseWindow();
}

// ----- PLAYING helpers -----
void PlayerCollision() {
    if (player->rect.x < mapBounds.x) player->rect.x = mapBounds.x;
    if (player->rect.x + player->rect.width > mapBounds.width)
        player->rect.x = mapBounds.width - player->rect.width;
    if (player->rect.y < mapBounds.y) player->rect.y = mapBounds.y;
    if (player->rect.y + player->rect.height > mapBounds.height)
        player->rect.y = mapBounds.height - player->rect.height;
}

void CheckCollisions() {
    // Player bullet hits boss
    if (player->bullet.active && CheckCollisionRecs(player->bullet.rect, boss->rect)) {
        player->bullet.active = false;
        boss->health--;
        score += 10;
        if (boss->health <= 0) gameState = MENU;
    }

    // Boss bullets hit player
    for (auto& b : boss->bullets) {
        if (b.active && CheckCollisionRecs(b.rect, player->rect)) {
            b.active = false;
            lives--;
            if (lives <= 0) gameState = MENU;
            break;
        }
    }
}

void DrawPlaying() {
    player->draw();
    boss->draw();

    DrawText(TextFormat("Lives: %d", lives), 10, 40, 20, WHITE);
    DrawText(TextFormat("Score: %d", score), 10, 70, 20, WHITE);
    DrawText(TextFormat("Boss Health: %d", boss->health), 10, 100, 20, WHITE);
}

void UpdateOptions() {
    DrawText("OPTIONS - Press ESC to return", 150, 350, 20, LIGHTGRAY);
    if (IsKeyPressed(KEY_ESCAPE)) gameState = MENU;
}

void drawFPS() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, PURPLE);
}