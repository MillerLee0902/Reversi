/*****************************************************************//**
* \file   Global.cpp
 * \brief  Global variables implementation
 *
 * \author Alan
 * \date   2025/3/31
 *********************************************************************/
#include "../headers/Global.h"

// Define the global variables declared in Global.h
char board[8][8];
short posX = 0, posY = 0;
bool player = false;


// Game Setting
const float TRANSITION_DURATION = 0.5f; // seconds
unsigned WINDOW_WIDTH = 1280;
unsigned WINDOW_HEIGHT = 600;
const std::string GAME_TITLE = "Reversi Game";

// Animation utilities
float easeInOut(float t) {
 return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}