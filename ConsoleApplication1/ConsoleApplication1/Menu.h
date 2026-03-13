#pragma once
#include "raylib.h"

class Menu {
public:
    Menu();

    // Call each frame to handle input + update internal state
    void Update();

    // Draw menu UI (call between BeginDrawing/EndDrawing)
    void Draw() const;

    // Returns true when player pressed ENTER to start
    bool IsStartRequested() const { return startRequested; }

    // Selected level (1..3)
    int GetSelectedLevel() const { return selectedLevel; }

    // Returns true when player requested options
    bool IsOptionsRequested() const { return optionsRequested; }

    // Reset start/options flags (useful if you reuse the menu)
    void ResetStart() { startRequested = false; }
    void ResetOptions() { optionsRequested = false; }

private:
    int selectedLevel;
    bool startRequested;
    bool optionsRequested;
};