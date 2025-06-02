//
// AIDifficultySelection.h - 水墨風格按鈕顏色優化版本
//

#ifndef AIDIFFICULTYSELECTION_H
#define AIDIFFICULTYSELECTION_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/Global.h"
#include "../headers/FundamentalFunction.h"
#include "../headers/ScrollableTextBox.h"
#include <SFML/Graphics.hpp>
#include <utility>

#include "GameScreen.h"
#include "NewGameMenu.h"

class GameScreen;
class NewGameMenu;

class AIDifficultySelection : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;

    ScrollableTextBox textBox;
    std::vector<Button> difficultyButtons;
    Button backButton;

    std::string player1Name;
    std::string player2Name;
    AILevel selectedDifficulty;

public:
    AIDifficultySelection(sf::RenderWindow &window,
                         std::function<void(std::shared_ptr<GameState>)> callback,
                         std::string p1Name, std::string p2Name)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          textBox(sf::Vector2f(WINDOW_WIDTH / 2.0f - 250.0f, WINDOW_HEIGHT / 3.0f),
                  sf::Vector2f(500.0f, 200.0f),
                  resources->getFont("main"),
                  15.0f),
          backButton(
              sf::Vector2f(100.0f, 40.0f),
              sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Back",
              sf::Color(90, 90, 90),
              sf::Color(110, 110, 110),
              sf::Color(70, 70, 70),
              resources->getSoundBuffer("click"),
              15.0f
          ),
          player1Name(std::move(p1Name)),
          player2Name(std::move(p2Name)),
          selectedDifficulty(AILevel::MEDIUM) {
    }

    void init() override {
        sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        titleText.setString("Select AI Difficulty");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.getCenter());
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        std::string difficultyInfo =
            "Choose the AI difficulty level that matches your skill:\n"
            "\n"
            "Easy Level:\n"
            "1. AI thinks 3 moves ahead\n"
            "2. Good for beginners learning the game\n"
            "3. Makes some strategic mistakes\n"
            "4. Response time: Very fast\n"
            "\n"
            "Medium Level:\n"
            "1. AI thinks 5 moves ahead\n"
            "2. Provides a balanced challenge\n"
            "3. Good strategic play with occasional oversights\n"
            "4. Response time: Moderate\n"
            "\n"
            "Hard Level:\n"
            "1. AI thinks 7 moves ahead\n"
            "2. Challenging for experienced players\n"
            "3. Advanced strategic planning\n"
            "4. Focuses on corner and edge control\n"
            "5. Response time: Slower but thorough\n"
            "\n"
            "AI Strategy Features:\n"
            "1. Uses minimax algorithm with alpha-beta pruning\n"
            "2. Evaluates board positions based on:\n"
            "  - Total piece count\n"
            "  - Corner positions (high value)\n"
            "  - Edge positions (medium value)\n"
            "  - Center control\n"
            "• Adapts strategy based on game phase\n"
            "\n"
            "Tips for Playing Against AI:\n"
            "1. Control corners early - they cannot be flipped\n"
            "2. Avoid giving opponent access to corners\n"
            "3. Edge pieces are generally safer than center pieces\n"
            "4. Think several moves ahead\n"
            "5. Sometimes sacrificing pieces early leads to better positions\n"
            "\n"
            "Select your preferred difficulty below:";

        textBox.setText(difficultyInfo, 14);
        textBox.setBackgroundColor(sf::Color(255, 255, 255, 128));
        textBox.setTextColor(sf::Color::Black);

        sf::Vector2f buttonSize(200.0f, 50.0f);
        float startY = WINDOW_HEIGHT / 2.0f + 120.0f;
        float spacing = 60.0f;

        // 水墨風格配色
        difficultyButtons.emplace_back(
            buttonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY),
            resources->getFont("main"),
            "Easy",
            sf::Color(120, 160, 140),    // 青竹綠
            sf::Color(140, 180, 160),    // 懸停青竹綠
            sf::Color(100, 140, 120),    // 按下青竹綠
            resources->getSoundBuffer("click"),
            15.0f
        );

        difficultyButtons.emplace_back(
            buttonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + spacing),
            resources->getFont("main"),
            "Medium",
            sf::Color(140, 120, 160),    // 淡紫灰
            sf::Color(160, 140, 180),    // 懸停淡紫灰
            sf::Color(120, 100, 140),    // 按下淡紫灰
            resources->getSoundBuffer("click"),
            15.0f
        );

        difficultyButtons.emplace_back(
            buttonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + spacing * 2),
            resources->getFont("main"),
            "Hard",
            sf::Color(160, 100, 90),     // 赤褐色
            sf::Color(180, 120, 110),    // 懸停赤褐色
            sf::Color(140, 80, 70),      // 按下赤褐色
            resources->getSoundBuffer("click"),
            15.0f
        );

        startTransitionIn();
    }

    void handleInput(sf::Event event) override {
        if (!transitioning) {
            textBox.handleInput(event, window);

            if (event.is<sf::Event::MouseButtonReleased>()) {
                if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
                    mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

                    if (backButton.wasClicked()) {
                        const std::shared_ptr<GameState> newGameMenu = std::make_shared<NewGameMenu>(
                            window, stateChangeCallback);
                        newGameMenu->init();
                        startTransitionTo(newGameMenu);
                        return;
                    }

                    for (size_t i = 0; i < difficultyButtons.size(); ++i) {
                        if (difficultyButtons[i].wasClicked()) {
                            switch (i) {
                                case 0: selectedDifficulty = AILevel::EASY; break;
                                case 1: selectedDifficulty = AILevel::MEDIUM; break;
                                case 2: selectedDifficulty = AILevel::HARD; break;
                                default: ;
                            }
                            startGameWithDifficulty();
                            return;
                        }
                    }
                }
            }
        }
    }

    void update(float deltaTime) override {
        GameState::update(deltaTime);

        backButton.update(window);
        for (auto &button : difficultyButtons) {
            button.update(window);
        }
        textBox.update(deltaTime);
    }

    void render() override {
        window.draw(backgroundSprite);
        window.draw(titleText);

        textBox.draw(window);

        backButton.draw(window);
        for (const auto &button : difficultyButtons) {
            button.draw(window);
        }

        renderTransition();
    }

private:
    void startGameWithDifficulty() {
        auto gameScreen = std::make_shared<GameScreen>(
            window, stateChangeCallback, player1Name, player2Name, true);

        gameScreen->setAIDifficulty(selectedDifficulty);
        gameScreen->init();
        startTransitionTo(gameScreen);
    }
};

#endif //AIDIFFICULTYSELECTION_H