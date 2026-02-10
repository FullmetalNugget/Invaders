// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "raylib.h"
#include <iostream>

enum GameState { MENU, PLAYING, OPTIONS, EXIT };

int main()
{
  InitWindow(600, 800, "Not so Space Invaders");

  while(!WindowShouldClose()) {
    BeginDrawing();

    // Blanking screen to black, can change bg color here
    ClearBackground(BLACK);


    EndDrawing();
  }
}

