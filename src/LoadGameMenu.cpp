//
// Created by Miller on 2025/4/9.
//

#include "../headers/LoadGameMenu.h"
#include "../headers/GameScreen.h"

void LoadGameMenu::loadSavedGames() {
    // Get save files using the SaveGame manager
    savedGames = saveGameManager.getSaveFiles();
}
void LoadGameMenu::createSaveButtons() {
    saveButtons.clear();

    // 增加按鈕寬度以更好地容納文字
    sf::Vector2f saveButtonSize(450.0f, 55.0f);
    float startY = WINDOW_HEIGHT / 3.0f;
    float spacing = 65.0f;

    // 使用水墨風格的顏色，如GPT建議
    sf::Color normalColor(76, 76, 76);          // 柔和墨灰色 #4C4C4C
    sf::Color hoverColor(110, 123, 88);         // 淡墨綠色 #6E7B58
    sf::Color selectedColor(140, 140, 140);     // 稍亮的灰色用於按下狀態

    for (size_t i = 0; i < savedGames.size(); ++i) {
        std::string displayName = saveGameManager.getSaveDisplayName(savedGames[i]);

        // 確保文字不會太長
        if (displayName.length() > 35) {
            displayName = displayName.substr(0, 32) + "...";
        }

        saveButtons.emplace_back(
            saveButtonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - saveButtonSize.x / 2.0f, startY + i * spacing),
            resources->getFont("main"),
            displayName,
            normalColor,
            hoverColor,
            selectedColor,
            resources->getSoundBuffer("click"),
            20.0f  // 圓角半徑
        );
    }
}
void LoadGameMenu::handleInput(sf::Event event) {
    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
            mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

            // Check back button
            if (buttons[0].wasClicked()) {
                auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                mainMenu->init();
                startTransitionTo(mainMenu);
                return;
            }

            // Check save buttons for selection
            for (size_t i = 0; i < saveButtons.size(); ++i) {
                if (saveButtons[i].wasClicked()) {
                    // Handle save selection
                    selectedSaveIndex = static_cast<int>(i);

                    // 更新所有按鈕的顯示狀態
                    for (size_t j = 0; j < saveButtons.size(); ++j) {
                        std::string displayName = saveGameManager.getSaveDisplayName(savedGames[j]);
                        if (displayName.length() > 35) {
                            displayName = displayName.substr(0, 32) + "...";
                        }

                        if (j == selectedSaveIndex) {
                            // 為選中項目添加指示符
                            saveButtons[j].setText("► " + displayName);
                        } else {
                            // 重置其他按鈕
                            saveButtons[j].setText(displayName);
                        }
                    }
                    return;
                }
            }

            // Check action buttons
            if (selectedSaveIndex >= 0 && selectedSaveIndex < static_cast<int>(savedGames.size())) {
                // Open button
                if (actionButtons[0].wasClicked()) {
                    loadGame(savedGames[selectedSaveIndex]);
                    return;
                }

                // Delete button
                if (actionButtons[1].wasClicked()) {
                    deleteGame(savedGames[selectedSaveIndex]);
                    return;
                }
            }
        }
    }
}

void LoadGameMenu::loadGame(const std::string& savePath) {
    // Create a new game screen with loaded game data
    FundamentalFunction gameLogic;
    std::string player1Name, player2Name;
    bool isWhiteTurn, vsComputer;
    int player1Chances = 0, player2Chances = 0;

    // Load the game data
    if (saveGameManager.loadGame(savePath, gameLogic, player1Name, player2Name, isWhiteTurn, vsComputer, player1Chances, player2Chances)) {
        auto gameScreen = std::make_shared<GameScreen>(
            window, stateChangeCallback, player1Name, player2Name, vsComputer);

        // Initialize the base components
        gameScreen->init();

        // We need to pass the loaded game data to the GameScreen
        gameScreen->setLoadedGameData(gameLogic, isWhiteTurn, player1Chances, player2Chances);

        startTransitionTo(gameScreen);
    }
}

void LoadGameMenu::deleteGame(const std::string& savePath) {
    if (saveGameManager.deleteSave(savePath)) {
        // Refresh saved games list
        loadSavedGames();
        createSaveButtons();
        selectedSaveIndex = -1;
    }
}

void LoadGameMenu::update(float deltaTime) {
    GameState::update(deltaTime);

    // Update buttons
    for (auto &button: buttons) {
        button.update(window);
    }

    for (auto &button: saveButtons) {
        button.update(window);
    }

    for (auto &button: actionButtons) {
        button.update(window);
    }
}

void LoadGameMenu::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);

    if (saveButtons.empty()) {
        window.draw(infoText);
    } else {
        // 為選中的存檔項目繪製特殊的圓角背景
        if (selectedSaveIndex >= 0 && selectedSaveIndex < saveButtons.size()) {
            sf::Vector2f buttonPos = saveButtons[selectedSaveIndex].getPosition();
            sf::Vector2f buttonSize = saveButtons[selectedSaveIndex].getSize();

            // 繪製選中項目的高亮背景
            drawRoundedBackground(window,
                sf::Vector2f(buttonPos.x - 5.0f, buttonPos.y - 5.0f),
                sf::Vector2f(buttonSize.x + 10.0f, buttonSize.y + 10.0f),
                sf::Color(216, 169, 169, 100), // 半透明的櫻花粉
                25.0f);
        }
    }

    // Draw save buttons
    for (const auto &button: saveButtons) {
        button.draw(window);
    }

    // Draw control buttons
    for (const auto &button: buttons) {
        button.draw(window);
    }

    // Draw action buttons only if a save is selected
    if (selectedSaveIndex >= 0) {
        for (const auto &button: actionButtons) {
            button.draw(window);
        }
    }

    renderTransition();
}