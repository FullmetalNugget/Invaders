// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include "raygui.h"
#include <iostream>

enum GameState { MENU, PLAYING, OPTIONS, EXIT };

void drawFPS() {
  DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, PURPLE);
}

int main()
{
  SetConfigFlags(FLAG_VSYNC_HINT); // enable VSync
  InitWindow(600, 800, "Not so Space Invaders");

  while(!WindowShouldClose()) {
    BeginDrawing();

    // Blanking screen to black, can change bg color here
    ClearBackground(BLACK);

    drawFPS();


    EndDrawing();
  }
}

