//
// Created by Miller on 2025/4/9.
// Enhanced with ink painting style button colors
//

#include "../headers/NewGameMenu.h"
#include "../headers/GameScreen.h"
#include "../headers/MainMenu.h"
#include "../headers/AIDifficultySelection.h"

void NewGameMenu::init() {
    // Set up background
    sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
    float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // Set up title text - 水墨風格
    titleText.setFont(resources->getFont("main"));
    titleText.setString("New Game");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(45, 45, 45));  // 深墨色
    titleText.setOutlineThickness(2.0f);
    titleText.setOutlineColor(sf::Color(200, 200, 200, 120));  // 淺灰輪廓

    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.getCenter());
    titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

    // 水墨風格配色方案
    sf::Vector2f buttonSize(250.0f, 50.0f);
    float cornerRadius = 15.0f;

    // 水墨調色盤
    sf::Color bambooGreen(110, 140, 110);      // 竹綠
    sf::Color bambooGreenHover(130, 160, 130); // 懸停竹綠
    sf::Color bambooGreenPress(90, 120, 90);   // 按下竹綠

    sf::Color plumPurple(130, 110, 140);       // 梅紫
    sf::Color plumPurpleHover(150, 130, 160);  // 懸停梅紫
    sf::Color plumPurplePress(110, 90, 120);   // 按下梅紫

    sf::Color jadeGreen(100, 150, 120);        // 玉綠
    sf::Color jadeGreenHover(120, 170, 140);   // 懸停玉綠
    sf::Color jadeGreenPress(80, 130, 100);    // 按下玉綠

    sf::Color inkGray(90, 90, 90);             // 墨灰
    sf::Color inkGrayHover(110, 110, 110);     // 懸停墨灰
    sf::Color inkGrayPress(70, 70, 70);        // 按下墨灰

    // Player vs Player button - 使用竹綠
    buttons.push_back(Button(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, WINDOW_HEIGHT / 3.0f),
        resources->getFont("main"),
        "Player vs Player",
        bambooGreen,
        bambooGreenHover,
        bambooGreenPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    ));

    // Player vs Computer button - 使用梅紫
    buttons.push_back(Button(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, WINDOW_HEIGHT / 3.0f + 60.0f),
        resources->getFont("main"),
        "Player vs Computer",
        plumPurple,
        plumPurpleHover,
        plumPurplePress,
        resources->getSoundBuffer("click"),
        cornerRadius
    ));

    // Text inputs for player names (保持原樣，但文字顏色調整為水墨色調)
    sf::Vector2f inputSize(250.0f, 40.0f);

    // Player 1 name input
    inputs.push_back(TextInput(
        inputSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, WINDOW_HEIGHT / 2.0f + 40.0f),
        resources->getFont("main"),
        "Player 1 Name"
    ));

    // Player 2 name input
    inputs.push_back(TextInput(
        inputSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, WINDOW_HEIGHT / 2.0f + 90.0f),
        resources->getFont("main"),
        "Player 2 Name"
    ));

    // Start Game button - 使用玉綠
    buttons.push_back(Button(
        sf::Vector2f(150.0f, 50.0f),
        sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, WINDOW_HEIGHT / 2.0f + 150.0f),
        resources->getFont("main"),
        "Start Game",
        jadeGreen,
        jadeGreenHover,
        jadeGreenPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    ));

    // Back button - 使用墨灰
    buttons.push_back(Button(
        sf::Vector2f(100.0f, 40.0f),
        sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
        resources->getFont("main"),
        "Back",
        inkGray,
        inkGrayHover,
        inkGrayPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    ));

    // Start transition in
    startTransitionIn();
}

void NewGameMenu::handleInput(sf::Event event) {
    if (!transitioning) {
        // Handle text input
        for (auto &input: inputs) {
            input.handleInput(event);
        }

        if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
            if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
                mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

                if (buttons[0].wasClicked()) {
                    // Player vs Player button
                    vsComputer = false;
                } else if (buttons[1].wasClicked()) {
                    // Player vs Computer button
                    vsComputer = true;
                    inputs[1].setValue("Computer");
                } else if (buttons[2].wasClicked()) {
                    // Start Game button
                    std::string player1Name = inputs[0].getValue();
                    std::string player2Name = inputs[1].getValue();

                    if (player1Name.empty()) player1Name = "Player 1";
                    if (player2Name.empty() && !vsComputer) player2Name = "Player 2";

                    if (vsComputer) {
                        auto difficultyMenu = std::make_shared<AIDifficultySelection>(
                            window, stateChangeCallback, player1Name, "Computer");
                        difficultyMenu->init();
                        startTransitionTo(difficultyMenu);
                    } else {
                        const auto gameScreen = std::make_shared<GameScreen>(
                            window, stateChangeCallback, player1Name, player2Name, false);
                        gameScreen->init();
                        startTransitionTo(gameScreen);
                    }
                } else if (buttons[3].wasClicked()) {
                    // Back button
                    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                    mainMenu->init();
                    startTransitionTo(mainMenu);
                }
            }
        }
    }
}

void NewGameMenu::update(float deltaTime) {
    GameState::update(deltaTime);

    for (auto &button: buttons) {
        button.update(window);
    }

    for (auto &input: inputs) {
        input.update(window, deltaTime);
    }
}

void NewGameMenu::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);

    for (const auto &button: buttons) {
        button.draw(window);
    }

    for (const auto &input: inputs) {
        input.draw(window);
    }

    renderTransition();
}