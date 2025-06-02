//
// Created by Miller on 2025/4/10.
//

#include "../headers/SaveGame.h"
#include "../headers/FundamentalFunction.h"

bool SaveGame::saveGame(const FundamentalFunction &gameLogic,
                        const std::string &player1Name,
                        const std::string &player2Name,
                        bool isWhiteTurn,
                        bool vsComputer,
                        int player1Chance,
                        int player2Chance) {
    // Create filename based on timestamp
    std::time_t currentTime = std::time(nullptr);
    std::string filename = "saves/" + std::to_string(currentTime) + ".txt";
    currentSaveFile = filename;

    // Ensure directory exists
    std::filesystem::create_directories("saves");

    std::ofstream saveFile(filename);
    if (!saveFile.is_open()) {
        return false;
    }

    // Save game metadata
    saveFile << currentTime << '\n';
    saveFile << player1Name << '\n';
    saveFile << player2Name << '\n';
    saveFile << (isWhiteTurn ? "true" : "false") << '\n';
    saveFile << (vsComputer ? "true" : "false") << '\n';
    saveFile << player1Chance << '\n';
    saveFile << player2Chance << '\n';

    // Save board state
    for (auto y: gameLogic.board) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            saveFile << y[x];
        }
        saveFile << '\n';
    }

    saveFile.flush();
    saveFile.close();
    return true;
}

bool SaveGame::loadGame(const std::string &filename,
                        FundamentalFunction &gameLogic,
                        std::string &player1Name,
                        std::string &player2Name,
                        bool &isWhiteTurn,
                        bool &vsComputer,
                        int &player1Chance,
                        int &player2Chance
) {
    std::ifstream loadFile(filename);
    if (!loadFile.is_open()) {
        return false;
    }

    // Load game metadata
    std::string timestamp;
    std::string whiteStr, computerStr;
    std::string tempP1Chance, tempP2Chance;

    std::getline(loadFile, timestamp);
    std::getline(loadFile, player1Name);
    std::getline(loadFile, player2Name);
    std::getline(loadFile, whiteStr);
    std::getline(loadFile, computerStr);
    std::getline(loadFile, tempP1Chance);
    std::getline(loadFile, tempP2Chance);

    player1Chance = stoi(tempP1Chance);
    player2Chance = stoi(tempP2Chance);

    isWhiteTurn = (whiteStr == "true");
    vsComputer = (computerStr == "true");

    // Load board state
    std::string line;
    int y = 0;
    while (y < BOARDLENGTH && std::getline(loadFile, line)) {
        for (int x = 0; x < BOARDLENGTH && x < line.length(); x++) {
            gameLogic.board[y][x] = line[x];
        }
        y++;
    }

    currentSaveFile = filename;
    loadFile.close();

    // Update available moves
    gameLogic.showPlayPlace(isWhiteTurn);

    return true;
}

bool SaveGame::deleteSave(const std::string &filename) {
    try {
        return std::filesystem::remove(filename);
    } catch (const std::filesystem::filesystem_error &) {
        return false;
    }
}

std::vector<std::string> SaveGame::getSaveFiles() {
    std::vector<std::string> saveFiles;

    if (!std::filesystem::exists("saves")) {
        return saveFiles;
    }

    for (const auto &entry: std::filesystem::directory_iterator("saves")) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            saveFiles.push_back(entry.path().string());
        }
    }

    return saveFiles;
}

std::string SaveGame::getSaveDisplayName(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "Unknown save";
    }

    std::string timestampStr;
    std::getline(file, timestampStr);
    file.close();

    try {
        std::time_t timestamp = std::stoll(timestampStr);
        std::string timeStr = std::ctime(&timestamp);

        // Remove trailing newline
        if (!timeStr.empty() && timeStr.back() == '\n') {
            timeStr.pop_back();
        }

        return "Save from " + timeStr;
    } catch (...) {
        return "Save file " + filename;
    }
}
