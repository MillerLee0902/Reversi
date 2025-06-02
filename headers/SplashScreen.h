//
// Created by Miller on 2025/4/9.
//

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "../headers/GameState.h"

class MainMenu;

// Splash Screen State
class SplashScreen final : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text promptText;
    float displayTime;

public:
    SplashScreen(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("splash")),
          titleText(resources->getFont("main")),
          promptText(resources->getFont("main")),
          displayTime(0.0f) {
    }

    void init() override;

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override;

    void render() override;
};




#endif //SPLASHSCREEN_H
