#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include "Player.h"
#include "Boss.h"
#include "background.h"
#include "AudioManager.h"
#include "Menu.h"
#include "Options.h"

enum GameState { MENU, PLAYING, OPTIONS, END_SCREEN, EXIT };

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

// Menu and Options instances
Menu menu;
Options options;

// Secret code: "inkrelo"
static const std::string SECRET_CODE = "inkrelo";
std::string typedBuffer;       // keeps last N typed chars
bool secretUnlocked = false;   // set true when code entered
float secretNotifyTimer = 0.0f; // time to show "Secret unlocked" message

// ----- FUNCTION PROTOTYPES -----
void UpdatePlaying();
void UpdateOptions();
void DrawPlaying();
void DrawEndScreen();
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

    // Load persisted settings (if any) before loading/playing music
    AudioManager::Get().LoadSettings("settings.cfg");

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

        // --- Secret code input handling (collect character input each frame) ---
        // Use GetCharPressed to read typed characters (handles repeated keys correctly)
        int keyChar = GetCharPressed();
        while (keyChar > 0) {
            char ch = static_cast<char>(keyChar);
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));

            if (ch >= 'a' && ch <= 'z') {
                typedBuffer.push_back(ch);
                // keep only the most recent SECRET_CODE.length() chars
                if (typedBuffer.size() > SECRET_CODE.size())
                    typedBuffer.erase(0, typedBuffer.size() - SECRET_CODE.size());

                if (typedBuffer == SECRET_CODE) {
                    secretUnlocked = true;
                    secretNotifyTimer = 3.0f; // show confirmation for 3 seconds
                    typedBuffer.clear();
                }
            }

            keyChar = GetCharPressed();
        }

        // decrease secret notification timer
        if (secretNotifyTimer > 0.0f) secretNotifyTimer -= dt;

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
            if (menu.IsOptionsRequested()) {
                menu.ResetOptions();
                gameState = OPTIONS;
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
            options.Update();
            options.Draw();
            if (options.IsBackRequested()) {
                options.ResetBack();
                gameState = MENU;
            }
            break;

        case END_SCREEN:
            DrawEndScreen();
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
        if (boss->health <= 0) {
            // Show END_SCREEN when boss dies if level 3 OR secret unlocked
            if (boss->getLevel() == 3 || secretUnlocked) {
                // stop music to emphasize the end screen (optional)
                if (AudioManager::Get().IsMusicLoaded()) AudioManager::Get().StopMusic();

                // Disable remaining boss bullets
                for (auto& b : boss->bullets) b.active = false;

                gameState = END_SCREEN;
            } else {
                gameState = MENU;
            }
        }
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

    // Show temporary confirmation when secret code is entered
    if (secretNotifyTimer > 0.0f) {
        const char* msg = "Secret unlocked!";
        int size = 18;
        int w = MeasureText(msg, size);
        DrawText(msg, (GetScreenWidth() - w) / 2, 130, size, GREEN);
    }
}

void DrawEndScreen() {
    // Full black background for the end screen
    ClearBackground(BLACK);

    const char* message = "The Earth has successfully been destroyed.";
    const int fontSize = 28;
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int textW = MeasureText(message, fontSize);
    int x = (screenW - textW) / 2;
    int y = screenH / 2 - fontSize / 2;

    DrawText(message, x, y, fontSize, GREEN);

    const char* instr = "Press ENTER or ESC to return to menu";
    int instrSize = 16;
    int instrW = MeasureText(instr, instrSize);
    DrawText(instr, (screenW - instrW) / 2, y + 60, instrSize, LIGHTGRAY);

    // Return to menu on Enter or Escape
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
        // Reset state like returning to menu
        // Reset player position and stats
        player->rect.x = 275;
        player->rect.y = 500;
        player->bullet.active = false;

        lives = 3;
        score = 0;

        // Reset boss for next run
        boss->rect.x = 265;
        boss->rect.y = 50;
        boss->health = 40;
        boss->moveDir = 1.0f;
        boss->shootTimer = 0.0f;
        boss->setLevel(1);
        for (auto& b : boss->bullets) b.active = false;

        // Optionally resume music
        if (AudioManager::Get().IsMusicLoaded()) {
            AudioManager::Get().PlayMusic();
        }

        gameState = MENU;
    }
}

void UpdateOptions() {
    DrawText("OPTIONS - Press ESC to return", 150, 350, 20, LIGHTGRAY);
    if (IsKeyPressed(KEY_ESCAPE)) gameState = MENU;
}

void drawFPS() {
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, PURPLE);
}