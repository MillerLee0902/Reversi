//
// Created by Miller on 2025/4/10.
//

#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <string>
#include <vector>
#include <filesystem>

class FundamentalFunction;

class SaveGame {
private:
    std::string currentSaveFile;

public:
    SaveGame() : currentSaveFile("") {
    }

    /**
     * Save the current game state to a file
     *
     * @param gameLogic Current game state
     * @param player1Name First player's name
     * @param player2Name Second player's name
     * @param isWhiteTurn Current player turn
     * @param vsComputer Whether playing against computer
     * @return true if save successful, false otherwise
     */
    bool saveGame(const FundamentalFunction &gameLogic,
                  const std::string &player1Name,
                  const std::string &player2Name,
                  bool isWhiteTurn,
                  bool vsComputer,
                  int player1Chance,
                  int player2Chance);

    /**
     * Load a saved game from file
     *
     * @param filename Path to the save file
     * @param gameLogic Game state to load into
     * @param player1Name Output parameter for player 1's name
     * @param player2Name Output parameter for player 2's name
     * @param isWhiteTurn Output parameter for current turn
     * @param vsComputer Output parameter for computer opponent
     * @return true if load successful, false otherwise
     */
    bool loadGame(const std::string &filename,
                  FundamentalFunction &gameLogic,
                  std::string &player1Name,
                  std::string &player2Name,
                  bool &isWhiteTurn,
                  bool &vsComputer,
                  int &player1Chance,
                  int &player2Chance
                  );

    /**
     * Delete a save file
     *
     * @param filename Path to the save file
     * @return true if deleted successfully, false otherwise
     */
    static bool deleteSave(const std::string &filename);

    /**
     * Get list of all save files
     *
     * @return Vector of save file paths
     */
    static std::vector<std::string> getSaveFiles();

    /**
     * Get a formatted display string for a save file
     *
     * @param fileName Path to the save file
     *
     * @return Formatted string with date and time
     */
    static std::string getSaveDisplayName(const std::string &fileName);
};
#endif // SAVEGAME_H
