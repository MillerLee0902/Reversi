//
// Created by Miller on 2025/3/31.
// Enhanced with AI difficulty levels
//

#include "../headers/FundamentalFunction.h"
#include <algorithm>
#include <climits>

FundamentalFunction::FundamentalFunction() {
    aiDifficulty = AILevel::MEDIUM; // Default difficulty
}

/**
 * Initialize the board
 * This function haven't any input and return value.
 */
void FundamentalFunction::initialize() {
    for (auto &i: board) {
        for (char &j: i) {
            j = {'s'};
        }
    }

    board[3][3] = 'w';
    board[3][4] = 'b';
    board[4][3] = 'b';
    board[4][4] = 'w';
}

/**
 *  Show all place that user can take
 * @param isWhiteTurn Who should play next (true is white turn, false is black turn)
 */
void FundamentalFunction::showPlayPlace(const bool isWhiteTurn) {
    // a mean available place
    const vector<vector<int> > directions = {
        {-1, -1}, {0, -1}, {1, -1}, {1, 0},
        {1, 1}, {0, 1}, {-1, 1}, {-1, 0}
    };

    // change available point back to 's'
    for (auto &i: board) {
        for (char &j: i) {
            if (j == 'a') {
                j = 's';
            }
        }
    }

    // Loop through all cells on the board
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            // If the current player is white (isWhiteTurn == true), look from white pieces
            // If the current player is black (isWhiteTurn == false), look from black pieces
            char targetPiece = isWhiteTurn ? 'w' : 'b';

            // Only check from pieces of the current player's color
            if (board[y][x] == targetPiece) {
                // Check in all 8 directions
                for (const vector<int> &dir: directions) {
                    dedect(x, y, dir[0], dir[1], isWhiteTurn);
                }
            }
        }
    }
}

/**
 * Consider whether the step is available.
 * \param xPos, yPos, moveX, moveY, isWhiteTurn
 */
void FundamentalFunction::dedect(const int xPos, const int yPos, const int moveX, const int moveY, const bool isWhiteTurn) {
    bool found = false;
    int newX = xPos;
    int newY = yPos;

    if (isWhiteTurn) {
        while (true) {
            newX += moveX;
            newY += moveY;

            if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
                break;
            }
            if (board[newY][newX] == 'b') {
                found = true;
            } else if (board[newY][newX] == 's' && found) {
                board[newY][newX] = 'a';
                break;
            } else {
                break;
            }
        }
    } else {
        while (true) {
            newX += moveX;
            newY += moveY;

            if (newX < 0 || newX > 7 || newY < 0 || newY > 7) {
                break;
            }
            if (board[newY][newX] == 'w') {
                found = true;
            } else if (board[newY][newX] == 's' && found) {
                board[newY][newX] = 'a';
                break;
            } else {
                break;
            }
        }
    }
}

/**
 * display function: output the checkerboard in current time.
 * This function haven't any input and return value.
 */
void FundamentalFunction::display() const {
    cout << "-----------------\n";

    for (auto i: board) {
        for (int j = 0; j < 8; j++) {
            cout << "|";

            if (i[j] == 's') {
                cout << " ";
            } else if (i[j] == 'w') {
                cout << "W";
            } else if (i[j] == 'b') {
                cout << "B";
            } else if (i[j] == 'a') {
                cout << "A";
            }
        }

        cout << "|\n-----------------\n";
    }
}

/**
 *  get x and y position, chess color and according to rule of black white chess to turn
 *  over the other color chess in each line.
 *
 * \param xPos this variable is to get chess x position.
 * \param yPos this variable is to get chess y position.
 * \param isWhiteTurn this variable is to check this chess color.
 */
void FundamentalFunction::turnOver(int xPos, int yPos, bool isWhiteTurn) {
    //cout << "*" << board[yPos][xPos] << endl;
    // searching centered on the white chess.
    if (isWhiteTurn) {
        // find the line of following eligible from the eight direstion of current chess.
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if ((yPos + i) >= 0 && (yPos + i) < 8 && (xPos + j) >= 0 && (xPos + j) < 8) {
                    if (board[yPos + i][xPos + j] == 'b') {
                        int findPointX = j, findPointY = i;
                        int findLine = 0;

                        // search one by one to find the chess is eligibility.
                        while (board[yPos + findPointY][xPos + findPointX] == 'b') {
                            findPointX += j;
                            findPointY += i;
                            if ((yPos + findPointY) < 0 || (yPos + findPointY) >= 8 || (xPos + findPointX) < 0 || (
                                    xPos + findPointX) >= 8) {
                                findLine = 0;
                                break;
                            }
                            if (board[yPos + findPointY][xPos + findPointX] == 's' || board[yPos + findPointY][
                                    xPos + findPointX] == 'a') {
                                findLine = 0;
                                break;
                            }

                            if (board[yPos + findPointY][xPos + findPointX] == 'w') {
                                findLine = 1;
                                break;
                            }
                        }

                        // turn over the black chess between two white chess in one line.
                        if (findLine) {
                            int startPointY = yPos;
                            int startPointX = xPos;
                            while (startPointY != (yPos + findPointY) || startPointX != (xPos + findPointX)) {
                                board[startPointY][startPointX] = 'w';
                                startPointY += i;
                                startPointX += j;
                            }
                            board[yPos + findPointY][xPos + findPointX] = 'w';
                        }
                    }
                }
            }
        }
    }
    // searching centered on the black chess.
    else {
        // find the line of following eligible from the eight direstion of current chess.
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if ((yPos + i) >= 0 && (yPos + i) < 8 && (xPos + j) >= 0 && (xPos + j) < 8) {
                    if (board[yPos + i][xPos + j] == 'w') {
                        int findPointX = j, findPointY = i;
                        int findLine = 0;

                        // search one by one to find the chess is eligibility.
                        while (board[yPos + findPointY][xPos + findPointX] == 'w') {
                            findPointX += j;
                            findPointY += i;
                            if ((yPos + findPointY) < 0 || (yPos + findPointY) >= 8 || (xPos + findPointX) < 0 || (
                                    xPos + findPointX) >= 8) {
                                findLine = 0;
                                break;
                            }

                            if (board[yPos + findPointY][xPos + findPointX] == 's' || board[yPos + findPointY][
                                    xPos + findPointX] == 'a') {
                                findLine = 0;
                                break;
                            }

                            if (board[yPos + findPointY][xPos + findPointX] == 'b') {
                                findLine = 1;
                                break;
                            }
                        }

                        // turn over the black chess between two white chess in one line.
                        if (findLine) {
                            int startPointY = yPos;
                            int startPointX = xPos;
                            //cout << startPointX << " " << startPointY << " " << xPos + findPointX << " " << yPos + findPointY << endl;
                            while (startPointY != (yPos + findPointY) || startPointX != (xPos + findPointX)) {
                                board[startPointY][startPointX] = 'b';
                                startPointY += i;
                                startPointX += j;
                                //cout << startPointX << " " << startPointY << endl;
                            }
                            board[yPos + findPointY][xPos + findPointX] = 'b';
                        }
                    }
                }
            }
        }
    }
}

bool FundamentalFunction::checkWin(bool isWhiteTurn) {
    return true;
}

// Enhanced AI with different difficulty levels
std::pair<int, int> FundamentalFunction::AIPlayChess() {
    // 找到所有標記為 'a' 的可用移動位置
    std::vector<std::pair<int, int>> availableMoves;

    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            if (board[y][x] == 'a') {
                availableMoves.emplace_back(x, y);
            }
        }
    }

    // 如果沒有可用移動，返回 (-1, -1)
    if (availableMoves.empty()) {
        return {-1, -1};
    }

    // Use different AI strategies based on difficulty
    switch (aiDifficulty) {
        case AILevel::EASY:
            return getEasyAIMove(availableMoves);
        case AILevel::MEDIUM:
            return getMediumAIMove(availableMoves);
        case AILevel::HARD:
            return getHardAIMove(availableMoves);
        default:
            return getMediumAIMove(availableMoves);
    }
}

// Easy AI - looks ahead 3 moves
std::pair<int, int> FundamentalFunction::getEasyAIMove(const std::vector<std::pair<int, int>>& availableMoves) {
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = availableMoves[0];

    for (const auto& move : availableMoves) {
        // Make a copy of the board
        char tempBoard[BOARDLENGTH][BOARDLENGTH];
        copyBoard(tempBoard, board);

        // Make the move
        makeMove(move.first, move.second, true);

        // Evaluate using minimax with depth 3
        int score = minimax(3, false, INT_MIN, INT_MAX);

        // Restore board
        copyBoard(board, tempBoard);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

// Medium AI - looks ahead 5 moves
std::pair<int, int> FundamentalFunction::getMediumAIMove(const std::vector<std::pair<int, int>>& availableMoves) {
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = availableMoves[0];

    for (const auto& move : availableMoves) {
        // Make a copy of the board
        char tempBoard[BOARDLENGTH][BOARDLENGTH];
        copyBoard(tempBoard, board);

        // Make the move
        makeMove(move.first, move.second, true);

        // Evaluate using minimax with depth 5
        int score = minimax(5, false, INT_MIN, INT_MAX);

        // Restore board
        copyBoard(board, tempBoard);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

// Hard AI - looks ahead 7 moves
std::pair<int, int> FundamentalFunction::getHardAIMove(const std::vector<std::pair<int, int>>& availableMoves) {
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = availableMoves[0];

    for (const auto& move : availableMoves) {
        // Make a copy of the board
        char tempBoard[BOARDLENGTH][BOARDLENGTH];
        copyBoard(tempBoard, board);

        // Make the move
        makeMove(move.first, move.second, true);

        // Evaluate using minimax with depth 7
        int score = minimax(7, false, INT_MIN, INT_MAX);

        // Restore board
        copyBoard(board, tempBoard);

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

// Minimax algorithm with alpha-beta pruning
int FundamentalFunction::minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (depth == 0) {
        return evaluateBoard();
    }

    std::vector<std::pair<int, int>> moves = getValidMoves(isMaximizing);

    if (moves.empty()) {
        return evaluateBoard();
    }

    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (const auto& move : moves) {
            char tempBoard[BOARDLENGTH][BOARDLENGTH];
            copyBoard(tempBoard, board);

            makeMove(move.first, move.second, true);
            int eval = minimax(depth - 1, false, alpha, beta);

            copyBoard(board, tempBoard);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (const auto& move : moves) {
            char tempBoard[BOARDLENGTH][BOARDLENGTH];
            copyBoard(tempBoard, board);

            makeMove(move.first, move.second, false);
            int eval = minimax(depth - 1, true, alpha, beta);

            copyBoard(board, tempBoard);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break; // Alpha-beta pruning
            }
        }
        return minEval;
    }
}

// Board evaluation function
int FundamentalFunction::evaluateBoard() {
    int whiteCount = 0;
    int blackCount = 0;

    // Count pieces
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            if (board[y][x] == 'w') {
                whiteCount++;
                // Give extra points for corner positions
                if ((x == 0 || x == 7) && (y == 0 || y == 7)) {
                    whiteCount += 10;
                }
                // Give points for edge positions
                else if (x == 0 || x == 7 || y == 0 || y == 7) {
                    whiteCount += 2;
                }
            } else if (board[y][x] == 'b') {
                blackCount++;
                // Give extra points for corner positions
                if ((x == 0 || x == 7) && (y == 0 || y == 7)) {
                    blackCount += 10;
                }
                // Give points for edge positions
                else if (x == 0 || x == 7 || y == 0 || y == 7) {
                    blackCount += 2;
                }
            }
        }
    }

    return whiteCount - blackCount;
}

// Helper functions
void FundamentalFunction::copyBoard(char dest[BOARDLENGTH][BOARDLENGTH],
                                  const char src[BOARDLENGTH][BOARDLENGTH]) {
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            dest[y][x] = src[y][x];
        }
    }
}

void FundamentalFunction::makeMove(int x, int y, bool isWhite) {
    board[y][x] = isWhite ? 'w' : 'b';
    turnOver(x, y, isWhite);
}

std::vector<std::pair<int, int>> FundamentalFunction::getValidMoves(bool isWhiteTurn) {
    // Clear previous available moves
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            if (board[y][x] == 'a') {
                board[y][x] = 's';
            }
        }
    }

    showPlayPlace(isWhiteTurn);

    std::vector<std::pair<int, int>> moves;
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            if (board[y][x] == 'a') {
                moves.emplace_back(x, y);
            }
        }
    }

    return moves;
}

void FundamentalFunction::setAIDifficulty(AILevel level) {
    aiDifficulty = level;
}