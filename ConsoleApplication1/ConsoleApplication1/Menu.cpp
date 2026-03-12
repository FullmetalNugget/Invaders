#include "Menu.h"
#include "raylib.h"

Menu::Menu()
    : selectedLevel(1), startRequested(false)
{}

void Menu::Update() {
    // Navigation
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        selectedLevel = (selectedLevel < 3) ? selectedLevel + 1 : 1;
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        selectedLevel = (selectedLevel > 1) ? selectedLevel - 1 : 3;
    }

    // Start game
    if (IsKeyPressed(KEY_ENTER)) {
        startRequested = true;
    }
}

void Menu::Draw() const {
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    const char* title = "Not So Space Invaders";
    const int titleSize = 40;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (screenW - titleW) / 2, screenH / 8, titleSize, GREEN);

    const char* prompt = "Select Level:";
    const int promptSize = 20;
    int promptW = MeasureText(prompt, promptSize);
    DrawText(prompt, (screenW - promptW) / 2, screenH / 3, promptSize, LIGHTGRAY);

    const int levelSize = 30;
    const int spacing = 140;
    for (int i = 0; i < 3; ++i) {
        int level = i + 1;
        const char* label = TextFormat("Level %d", level);
        int labelW = MeasureText(label, levelSize);
        int centerX = screenW / 2 + (i - 1) * spacing;
        int x = centerX - labelW / 2;
        int y = screenH / 2 - 10;

        // highlight selected level
        if (level == selectedLevel) {
            DrawRectangle(x - 12, y - 8, labelW + 24, levelSize + 8, Fade(YELLOW, 0.25f));
            DrawText(label, x, y, levelSize, YELLOW);
        } else {
            DrawText(label, x, y, levelSize, WHITE);
        }
    }

    const char* instr2 = "Press ENTER to Start, ESC to Quit";
    const int instrSize = 18;
    int instr2W = MeasureText(instr2, instrSize);
    DrawText(instr2, (screenW - instr2W) / 2, screenH * 3 / 4 + 26, instrSize, LIGHTGRAY);

    const char* musicHint = "Toggle Music: M";
    int musicW = MeasureText(musicHint, 14);
    DrawText(musicHint, screenW - musicW - 10, 10, 14, LIGHTGRAY);
}