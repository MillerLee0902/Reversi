/*****************************************************************//**
* \file   Global.h
 * \brief  Global variables for the Reversi game
 *
 * \author Alan
 * \date   2025/3/14
 *********************************************************************/
#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cmath>

// Board representation
// 's': empty space
// 'b': black piece
// 'w': white piece
// 'a': available move
extern char board[8][8];

// Game state variables
extern short posX, posY;
extern bool player;

// Addition for SFML UI
// Default window size
// const int DEFAULT_WINDOW_WIDTH = 800;
// const int DEFAULT_WINDOW_HEIGHT = 600;

// Player information
struct PlayerInfo {
    std::string name;
    bool isComputer;
};

// Game settings
struct GameSettings {
    bool vsComputer;
    int timerDuration; // in seconds
    bool soundEnabled;
    float volume;
};

// Save game data
struct SavedGame {
    time_t timestamp;
    std::string player1Name;
    std::string player2Name;
    bool vsComputer;
    char gameBoard[8][8];
    bool currentPlayer; // false: black, true: white
    std::vector<std::pair<int, int>> moveHistory;
};

// Setting

// Game Setting
extern const float TRANSITION_DURATION;// seconds
extern unsigned WINDOW_WIDTH;
extern unsigned WINDOW_HEIGHT;
extern const std::string GAME_TITLE;

extern float easeInOut(float t);