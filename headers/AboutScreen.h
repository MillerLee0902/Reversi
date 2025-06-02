//
// AboutScreen.h - 水墨風格按鈕顏色優化版本
//

#ifndef ABOUTSCREEN_H
#define ABOUTSCREEN_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/MainMenu.h"
#include "../headers/Global.h"
#include "../headers/ScrollableTextBox.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include <string>

class AboutScreen : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;

    ScrollableTextBox textBox;
    Button backButton;
    Button docButton;

public:
    AboutScreen(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          textBox(sf::Vector2f(WINDOW_WIDTH / 2.0f - 300.0f, WINDOW_HEIGHT / 2.0f - 150.0f),
                  sf::Vector2f(600.0f, 300.0f),
                  resources->getFont("main"),
                  20.0f),
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
          docButton(
              sf::Vector2f(180.0f, 40.0f),
              sf::Vector2f(WINDOW_WIDTH / 2.0f - 90.0f, WINDOW_HEIGHT - 120.0f),
              resources->getFont("main"),
              "View Documentation",
              sf::Color(150, 100, 80),    // 秋楓茶色
              sf::Color(170, 120, 100),   // 懸停秋楓茶色
              sf::Color(130, 80, 60),     // 按下秋楓茶色
              resources->getSoundBuffer("click"),
              15.0f
          ) {
    }

    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void render() override;

private:
    void openWebPage(const std::string& url);
};

#endif //ABOUTSCREEN_H