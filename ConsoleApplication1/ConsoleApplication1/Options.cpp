// disable MSVC "unsafe" CRT warnings for legacy C functions used by raygui
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Options.h"
#include "AudioManager.h"
#include "raylib.h"

// Provide raygui implementation in this .cpp only to resolve linker errors.
// Remove this define if you instead compile/link raygui.c or another TU provides the implementation.
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string>
#include <fstream>

Options::Options()
    : masterVolume(1.0f), backRequested(false), savedNotification(false), savedTimer(0.0f)
{
    // Initialize from saved settings if available
    // Default settings file name: settings.cfg
    if (AudioManager::Get().LoadSettings("settings.cfg")) {
        std::ifstream ifs("settings.cfg");
        if (ifs.is_open()) {
            std::string line;
            while (std::getline(ifs, line)) {
                if (line.rfind("masterVolume=", 0) == 0) {
                    try {
                        masterVolume = std::stof(line.substr(13));
                    } catch (...) { masterVolume = 1.0f; }
                }
            }
            ifs.close();
        }
    } else {
        // ensure audio manager uses default
        AudioManager::Get().SetMasterVolume(masterVolume);
    }
}

void Options::Update() {
    // Adjust volume with left/right (hold for repeat), or A/D
    const float step = 0.05f;
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
        masterVolume += step;
        if (masterVolume > 1.0f) masterVolume = 1.0f;
        AudioManager::Get().SetMasterVolume(masterVolume);
    }
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
        masterVolume -= step;
        if (masterVolume < 0.0f) masterVolume = 0.0f;
        AudioManager::Get().SetMasterVolume(masterVolume);
    }

    // Quick keys: + / - on keypad (also consider PAGE_UP/DOWN)
    if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) {
        masterVolume += step;
        if (masterVolume > 1.0f) masterVolume = 1.0f;
        AudioManager::Get().SetMasterVolume(masterVolume);
    }
    if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) {
        masterVolume -= step;
        if (masterVolume < 0.0f) masterVolume = 0.0f;
        AudioManager::Get().SetMasterVolume(masterVolume);
    }

    // Save settings (S)
    if (IsKeyPressed(KEY_S)) {
        if (AudioManager::Get().SaveSettings("settings.cfg")) {
            savedNotification = true;
            savedTimer = 2.0f; // show for 2s
        }
    }

    // NOTE: ESC no longer triggers returning to menu to avoid accidental window close.

    // countdown saved notification
    float dt = GetFrameTime();
    if (savedNotification) {
        savedTimer -= dt;
        if (savedTimer <= 0.0f) {
            savedNotification = false;
            savedTimer = 0.0f;
        }
    }
}

void Options::Draw() {
    const int screenW = GetScreenWidth();
    const int screenH = GetScreenHeight();

    const char* title = "OPTIONS";
    const int titleSize = 32;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (screenW - titleW) / 2, 80, titleSize, GREEN);

    // Master volume slider
    const int sliderW = 300;
    const int sliderH = 8;
    int sliderX = (screenW - sliderW) / 2;
    int sliderY = screenH / 2;

    // Background bar
    DrawRectangle(sliderX, sliderY - sliderH / 2, sliderW, sliderH, Fade(LIGHTGRAY, 0.6f));
    // Filled portion
    DrawRectangle(sliderX, sliderY - sliderH / 2, (int)(sliderW * masterVolume), sliderH, GREEN);
    // Knob
    int knobX = sliderX + (int)(sliderW * masterVolume) - 6;
    DrawRectangle(knobX, sliderY - 12, 12, 24, YELLOW);

    const char* volLabel = TextFormat("Master Volume: %.2f (Left/Right or A/D)", masterVolume);
    DrawText(volLabel, (screenW - MeasureText(volLabel, 18)) / 2, sliderY + 24, 18, WHITE);

    const char* saveHint = "Press S to Save settings";
    DrawText(saveHint, (screenW - MeasureText(saveHint, 14)) / 2, sliderY + 56, 14, LIGHTGRAY);

    const char* backHint = "Click Resume to return to Menu";
    DrawText(backHint, (screenW - MeasureText(backHint, 14)) / 2, sliderY + 80, 14, LIGHTGRAY);

    if (savedNotification) {
        const char* savedMsg = "Settings saved to settings.cfg";
        DrawText(savedMsg, (screenW - MeasureText(savedMsg, 14)) / 2, sliderY - 80, 14, GREEN);
    }

    // Resume button (uses raygui). Placed below slider.
    int btnW = 140;
    int btnH = 32;
    int btnX = (screenW - btnW) / 2;
    int btnY = sliderY + 110;
    Rectangle btnRect = { (float)btnX, (float)btnY, (float)btnW, (float)btnH };
    if (GuiButton(btnRect, "Resume")) {
        backRequested = true;
    }
}