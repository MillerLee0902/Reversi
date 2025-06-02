//
// Created by Miller on 2025/4/9.
//

#ifndef VICTORYSCREEN_H
#define VICTORYSCREEN_H

#include <utility>
#include <memory>
#include <functional>

#include "../headers/GameState.h"
#include "../headers/Button.h"

class MainMenu;

// Victory Screen State
class VictoryScreen : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text winnerText;
    sf::Text scoreText;

    Button newGameButton;
    Button exitButton;

    std::string winnerName;
    int player1Score;
    int player2Score;

public:
    VictoryScreen(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback,
                  std::string winner, int score1, int score2)
        : GameState(window, std::move(callback)),
          backgroundSprite(sf::Sprite(resources->getTexture("victory_bg"))),
          titleText(sf::Text(resources->getFont("main"))),
          winnerText(sf::Text(resources->getFont("main"))),
          scoreText(sf::Text(resources->getFont("main"))),
          winnerName(std::move(winner)),
          player1Score(score1),
          player2Score(score2),
          newGameButton(
              sf::Vector2f(200.0f, 50.0f),
              sf::Vector2f(WINDOW_WIDTH / 2.0f - 220.0f, WINDOW_HEIGHT * 0.7f),
              resources->getFont("main"),
              "New Game",
              sf::Color(40, 120, 200),
              sf::Color(60, 140, 220),
              sf::Color(30, 100, 180),
              resources->getSoundBuffer("click")
          ),
          exitButton(
              sf::Vector2f(200.0f, 50.0f),
              sf::Vector2f(WINDOW_WIDTH / 2.0f + 20.0f, WINDOW_HEIGHT * 0.7f),
              resources->getFont("main"),
              "Exit Game",
              sf::Color(200, 60, 60),
              sf::Color(220, 80, 80),
              sf::Color(180, 40, 40),
              resources->getSoundBuffer("click")
          ) {
    }

    void init() override;

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override;

    void render() override;

};




#endif //VICTORYSCREEN_H
