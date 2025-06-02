//
// SimpleNetworkMenu.h - 简化的网络菜单
//

#ifndef SIMPLENETWORKMENU_H
#define SIMPLENETWORKMENU_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/Global.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include <memory>

// 前向宣告 - 避免循環包含
class MainMenu;

// 简化的网络菜单 - 暂时显示"功能开发中"
class SimpleNetworkMenu : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text messageText;
    Button backButton;

public:
    SimpleNetworkMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          messageText(sf::Text(resources->getFont("main"))),
          backButton(
              sf::Vector2f(100.0f, 40.0f),
              sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Back",
              sf::Color(180, 180, 180),
              sf::Color(200, 200, 200),
              sf::Color(160, 160, 160),
              resources->getSoundBuffer("click"),
              15.0f
          ) {
    }

    void init() override {
        // Set up background
        sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // Set up title text
        titleText.setString("Online Multiplayer");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        // Center title
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        // Set up message text
        messageText.setString(
            "Online Multiplayer Feature\n\n"
            "This feature is currently under development.\n\n"
            "For now, you can enjoy:\n"
            "• Player vs Player (local)\n"
            "• Player vs AI (3 difficulty levels)\n"
            "• Save/Load games\n\n"
            "Online multiplayer will be available in a future update!\n"
            "Thank you for your patience."
        );
        messageText.setCharacterSize(20);
        messageText.setFillColor(sf::Color::White);
        messageText.setLineSpacing(1.5f);

        // Center message text
        sf::FloatRect msgBounds = messageText.getLocalBounds();
        messageText.setOrigin({msgBounds.size.x / 2.0f, msgBounds.size.y / 2.0f});
        messageText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});

        // Start transition in
        startTransitionIn();
    }

    void handleInput(sf::Event event) override {
        if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
            if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>(); mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                if (backButton.wasClicked()) {
                    // 修復：避免循環包含，延遲包含MainMenu
                    goToMainMenu();
                }
            }
        }
    }

    void update(float deltaTime) override {
        GameState::update(deltaTime);
        backButton.update(window);
    }

    void render() override {
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(messageText);
        backButton.draw(window);
        renderTransition();
    }

private:
    void goToMainMenu();  // 實現將在.cpp文件中
};

#endif //SIMPLENETWORKMENU_H