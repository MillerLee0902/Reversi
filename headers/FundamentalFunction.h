//
// Created by Miller on 2025/3/31.
// Enhanced with AI difficulty levels
//

#ifndef FUNDAMENTALFUNCTION_H
#define FUNDAMENTALFUNCTION_H

#include <vector>
#include <iostream>
#include <fstream>

#define BOARDLENGTH 8

using namespace std;

// AI difficulty levels
enum class AILevel {
    EASY,   // 3 moves ahead
    MEDIUM, // 5 moves ahead
    HARD    // 7 moves ahead
};

class FundamentalFunction {
public:

    FundamentalFunction();

    char board[BOARDLENGTH][BOARDLENGTH]{};

    char** getBoard() {
        // let board -> char** (strictly)
        return reinterpret_cast<char**>(board);
    }

    // Initialize the board
    void initialize();

    // Show all place that user can take
    void showPlayPlace(bool);

    // Consider whether the step is available.
    void dedect(int xPos, int yPos, int moveX, int moveY, bool isWhiteTurn);

    // output the checkerboard in current time.
    void display() const;

    /*get x and y position, chess color and according to rule of black white chess to turn
      over the other color chess in each line.*/
    void turnOver(int, int, bool);

    static bool checkWin(bool);

    std::pair<int, int> AIPlayChess();

    // AI difficulty functions
    void setAIDifficulty(AILevel level);
    AILevel getAIDifficulty() const { return aiDifficulty; }

private:
    int targetX{};
    int targetY{};
    AILevel aiDifficulty;

    // AI helper functions
    std::pair<int, int> getEasyAIMove(const std::vector<std::pair<int, int>>& availableMoves);
    std::pair<int, int> getMediumAIMove(const std::vector<std::pair<int, int>>& availableMoves);
    std::pair<int, int> getHardAIMove(const std::vector<std::pair<int, int>>& availableMoves);

    int minimax(int depth, bool isMaximizing, int alpha, int beta);
    int evaluateBoard();
    void copyBoard(char dest[BOARDLENGTH][BOARDLENGTH], const char src[BOARDLENGTH][BOARDLENGTH]);
    void makeMove(int x, int y, bool isWhite);
    std::vector<std::pair<int, int>> getValidMoves(bool isWhiteTurn);
};

#endif //FUNDAMENTALFUNCTION_H