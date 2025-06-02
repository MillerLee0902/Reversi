//
// Created by Miller on 2025/4/9.
//

#include "../headers/VictoryScreen.h"
#include "../headers/MainMenu.h"

void VictoryScreen::init() {
        // Set up background

        // Scale to fit the window
        sf::Vector2u textureSize = resources->getTexture("victory_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // Set up title text
        titleText.setString("Game Over");
        titleText.setCharacterSize(72);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(3.0f);
        titleText.setOutlineColor(sf::Color::Black);

        // Center title
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.getCenter());
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 4.0f});

        // Winner text
        if (winnerName == "It's a tie!") {
            winnerText.setString("It's a tie!");
        } else {
            winnerText.setString(winnerName + " wins!");
        }
        winnerText.setCharacterSize(48);
        winnerText.setFillColor(sf::Color::Yellow);
        winnerText.setOutlineThickness(2.0f);
        winnerText.setOutlineColor(sf::Color::Black);

        // Center winner text
        sf::FloatRect winnerBounds = winnerText.getLocalBounds();
        winnerText.setOrigin(winnerBounds.getCenter());
        winnerText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.5f});

        // Score text
        scoreText.setString("Final Score: " + std::to_string(player1Score) + " - " + std::to_string(player2Score));
        scoreText.setCharacterSize(36);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineThickness(2.0f);
        scoreText.setOutlineColor(sf::Color::Black);

        // Center score text
        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(scoreBounds.getCenter());
        scoreText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});

        // Start transition in
        startTransitionIn();
    }

void VictoryScreen::handleInput(sf::Event event) {
    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>(); mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            if (newGameButton.wasClicked()) {
                auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                mainMenu->init();
                startTransitionTo(mainMenu);
            } else if (exitButton.wasClicked()) {
                window.close();
            }
        }
    }
}

void VictoryScreen::update(float deltaTime) {
    GameState::update(deltaTime);

    newGameButton.update(window);
    exitButton.update(window);
}

void VictoryScreen::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);
    window.draw(winnerText);
    window.draw(scoreText);

    newGameButton.draw(window);
    exitButton.draw(window);

    renderTransition();
}