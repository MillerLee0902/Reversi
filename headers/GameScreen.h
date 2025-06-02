//
// Created by Miller on 2025/4/9.
//

#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>
#include <functional>

#include "../headers/Button.h"
#include "../headers/GameState.h"
#include "../headers/FundamentalFunction.h"
#include "../headers/Global.h"
#include "../headers/Timer.h"
#include "../headers/SaveGame.h"

// 前向宣告
class MainMenu;
class VictoryScreen;
class GameScreen;

// Game Screen State
class GameScreen final : public GameState {
private:
    float aiThinkingTime = 0.0f;
    bool aiThinking = false;

    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text player1Text;
    sf::Text player2Text;
    sf::Text scoreText;
    sf::Text currentPlayerText;

    // Timer display elements
    Timer player1Timer;
    Timer player2Timer;
    sf::Text timerLabel1;
    sf::Text timerLabel2;

    // Game board representation
    sf::RectangleShape boardBackground;
    sf::RectangleShape cellHighlight;
    std::vector<sf::CircleShape> pieces;

    // Buttons
    Button undoButton;
    Button menuButton;
    Button saveButton;

    // Game state
    std::string player1Name;
    std::string player2Name;
    bool vsComputer;
    bool gameOver;

    // Save game manager
    SaveGame saveGameManager;

    // Game logic
    FundamentalFunction gameLogic;
    int player1Score;
    int player2Score;
    bool isWhiteTurn;
    int selectedX;
    int selectedY;

    // Move history for undo
    struct Move {
        int x;
        int y;
        bool wasWhiteTurn;
    };

    std::vector<Move> moveHistory;

    // Constants for board rendering
    const float cellSize = 50.0f;
    const float boardX = (WINDOW_WIDTH - 8 * cellSize) / 2.0f;
    const float boardY = (WINDOW_HEIGHT - 8 * cellSize) / 2.0f + 20.0f;

    sf::Sound placeSound;
    // 添加AI 難度相關成員
    AILevel aiDifficulty;

    // 存檔按鈕狀態管理
    bool saveButtonPressed;
    float saveButtonTimer;
    std::string originalSaveButtonText;

    sf::RectangleShape timerBackground1;
    sf::RectangleShape timerBackground2;

    static void drawRoundedRectangle(sf::RenderWindow &window, const sf::Vector2f &position,
                             const sf::Vector2f &size, const sf::Color &color, float radius) {
        // 與Button.h中相同的圓角矩形繪製函數
        float maxRadius = std::min(size.x, size.y) / 2.0f;
        radius = std::min(radius, maxRadius);

        if (radius <= 1.0f) {
            sf::RectangleShape rect(size);
            rect.setPosition(position);
            rect.setFillColor(color);
            window.draw(rect);
            return;
        }

        sf::RectangleShape mainRect;
        mainRect.setSize({size.x - 2 * radius, size.y});
        mainRect.setPosition({position.x + radius, position.y});
        mainRect.setFillColor(color);
        window.draw(mainRect);

        sf::RectangleShape leftRect;
        leftRect.setSize({radius, size.y - 2 * radius});
        leftRect.setPosition({position.x, position.y + radius});
        leftRect.setFillColor(color);
        window.draw(leftRect);

        sf::RectangleShape rightRect;
        rightRect.setSize({radius, size.y - 2 * radius});
        rightRect.setPosition({position.x + size.x - radius, position.y + radius});
        rightRect.setFillColor(color);
        window.draw(rightRect);

        sf::CircleShape topLeft(radius);
        topLeft.setPosition({position.x, position.y});
        topLeft.setFillColor(color);
        window.draw(topLeft);

        sf::CircleShape topRight(radius);
        topRight.setPosition({position.x + size.x - 2 * radius, position.y});
        topRight.setFillColor(color);
        window.draw(topRight);

        sf::CircleShape bottomLeft(radius);
        bottomLeft.setPosition({position.x, position.y + size.y - 2 * radius});
        bottomLeft.setFillColor(color);
        window.draw(bottomLeft);

        sf::CircleShape bottomRight(radius);
        bottomRight.setPosition({position.x + size.x - 2 * radius, position.y + size.y - 2 * radius});
        bottomRight.setFillColor(color);
        window.draw(bottomRight);
    }


public:
    GameScreen(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback,
               std::string p1Name, std::string p2Name, bool vsAI, AILevel difficulty = AILevel::MEDIUM)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("game_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          player1Text(sf::Text(resources->getFont("main"))),
          player2Text(sf::Text(resources->getFont("main"))),
          scoreText(sf::Text(resources->getFont("main"))),
          currentPlayerText(sf::Text(resources->getFont("main"))),
          player1Timer(resources->getFont("main")),
          player2Timer(resources->getFont("main")),
          timerLabel1(sf::Text(resources->getFont("main"))),
          timerLabel2(sf::Text(resources->getFont("main"))),
          undoButton(
              sf::Vector2f(100.0f, 40.0f),
              sf::Vector2f(WINDOW_WIDTH - 130.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Undo",
              sf::Color(180, 120, 60),
              sf::Color(200, 140, 80),
              sf::Color(160, 100, 40),
              resources->getSoundBuffer("click"),
              15.0f
          ),
          menuButton(
              sf::Vector2f(100.0f, 40.0f),
              sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Menu",
              sf::Color(180, 180, 180),
              sf::Color(200, 200, 200),
              sf::Color(160, 160, 160),
              resources->getSoundBuffer("click"),
              15.0f
          ),
          saveButton(
              sf::Vector2f(120.0f, 40.0f),  // 增加寬度以容納文字
              sf::Vector2f(WINDOW_WIDTH / 2.0f - 60.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Save Game",
              sf::Color(110, 123, 88),     // 使用水墨風格的墨綠色 #6E7B58
              sf::Color(140, 153, 118),    // 較亮的墨綠色
              sf::Color(90, 103, 68),      // 較暗的墨綠色
              resources->getSoundBuffer("click"),
              15.0f
          ),
          player1Name(std::move(p1Name)),
          player2Name(std::move(p2Name)),
          vsComputer(vsAI),
          gameOver(false),
          player1Score(2),
          player2Score(2),
          isWhiteTurn(false),
          selectedX(-1),
          selectedY(-1),
          placeSound(resources->getSoundBuffer("place")),
          aiDifficulty(difficulty),
          saveButtonPressed(false),
          saveButtonTimer(0.0f),
          originalSaveButtonText("Save Game") {
    }

    void init() override;

    void setAIDifficulty(AILevel level);

    void setLoadedGameData(const FundamentalFunction &, bool, int, int);

    void handleInput(sf::Event event) override;

    void checkGameOver();

    void endGame();

    void update(float deltaTime) override;

    void render() override;

    void updateScores();

    void updateBoardPieces();

    bool makeMove(int, int);

    void checkTimers();

    void undoMove();

    void saveCurrentGame();

    void makeAIMove();


};

#endif //GAMESCREEN_H
