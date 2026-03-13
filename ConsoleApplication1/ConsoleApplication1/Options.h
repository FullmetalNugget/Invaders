#pragma once
#include "raylib.h"

class Options {
public:
    Options();

    // Update internal state (handle input)
    void Update();

    // Draw options UI (call between BeginDrawing/EndDrawing)
    void Draw();

    // Returned when player requests to go back to menu (via Resume button)
    bool IsBackRequested() const { return backRequested; }
    void ResetBack() { backRequested = false; }

private:
    float masterVolume;       // 0.0 .. 1.0
    bool backRequested;
    bool savedNotification;
    float savedTimer;
};