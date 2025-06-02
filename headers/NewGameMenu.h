//
// Created by Miller on 2025/4/9.
//

#ifndef NEWGAMEMENU_H
#define NEWGAMEMENU_H

#include <SFML/Graphics.hpp>
#include <utility>
#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/TextInput.h"
#include "../headers/MainMenu.h"

class GameScreen;

// New Game Menu State
class NewGameMenu final : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;

    std::vector<Button> buttons;
    std::vector<TextInput> inputs;

    bool vsComputer;

public:
    NewGameMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(resources->getFont("main")),
          vsComputer(false) {
    }

    void init() override;

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override;

    void render() override;

};

#endif //NEWGAMEMENU_H
