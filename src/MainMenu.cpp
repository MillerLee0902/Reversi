//
// Created by Miller on 2025/4/9.
// Enhanced with ink painting style button colors
//

#include "../headers/MainMenu.h"
#include "../headers/NewGameMenu.h"
#include "../headers/LoadGameMenu.h"
#include "../headers/AboutScreen.h"
#include "../headers/ImprovedNetworkMenu.h"

void MainMenu::init() {
    // Set up background
    const sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
    float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // Set up title text - 使用更符合水墨風格的顏色
    titleText.setString("REVERSI");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color(45, 45, 45));  // 深墨色
    titleText.setOutlineThickness(3.0f);
    titleText.setOutlineColor(sf::Color(200, 200, 200, 150));  // 淺灰輪廓

    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.getCenter());
    titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 6.0f});

    // 水墨風格按鈕配色
    constexpr sf::Vector2f buttonSize(220.0f, 55.0f);
    float buttonY = WINDOW_HEIGHT / 2.0f - 120.0f;
    constexpr float spacing = 70.0f;
    constexpr float cornerRadius = 20.0f;

    // 水墨調色盤
    sf::Color inkGray(88, 88, 88);           // 基礎墨灰
    sf::Color inkGrayHover(108, 108, 108);   // 懸停墨灰
    sf::Color inkGrayPress(68, 68, 68);      // 按下墨灰

    sf::Color cherryPink(180, 120, 130);     // 櫻花粉
    sf::Color cherryPinkHover(200, 140, 150); // 懸停櫻花粉
    sf::Color cherryPinkPress(160, 100, 110); // 按下櫻花粉

    sf::Color mossGreen(110, 123, 88);       // 苔綠
    sf::Color mossGreenHover(130, 143, 108); // 懸停苔綠
    sf::Color mossGreenPress(90, 103, 68);   // 按下苔綠

    sf::Color mistBlue(120, 140, 160);       // 霧藍
    sf::Color mistBlueHover(140, 160, 180);  // 懸停霧藍
    sf::Color mistBluePress(100, 120, 140);  // 按下霧藍

    sf::Color autumnRed(160, 90, 80);        // 秋楓紅
    sf::Color autumnRedHover(180, 110, 100); // 懸停秋楓紅
    sf::Color autumnRedPress(140, 70, 60);   // 按下秋楓紅

    // New Game button - 使用櫻花粉
    buttons.emplace_back(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, buttonY),
        resources->getFont("main"),
        "New Game",
        cherryPink,
        cherryPinkHover,
        cherryPinkPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    );

    buttonY += spacing;

    // Load Game button - 使用苔綠
    buttons.emplace_back(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, buttonY),
        resources->getFont("main"),
        "Load Game",
        mossGreen,
        mossGreenHover,
        mossGreenPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    );

    buttonY += spacing;

    // Online Play button - 使用霧藍
    buttons.emplace_back(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, buttonY),
        resources->getFont("main"),
        "Online Play",
        mistBlue,
        mistBlueHover,
        mistBluePress,
        resources->getSoundBuffer("click"),
        cornerRadius
    );

    buttonY += spacing;

    // About button - 使用墨灰
    buttons.emplace_back(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, buttonY),
        resources->getFont("main"),
        "About",
        inkGray,
        inkGrayHover,
        inkGrayPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    );

    buttonY += spacing;

    // Exit button - 使用秋楓紅
    buttons.emplace_back(
        buttonSize,
        sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, buttonY),
        resources->getFont("main"),
        "Exit",
        autumnRed,
        autumnRedHover,
        autumnRedPress,
        resources->getSoundBuffer("click"),
        cornerRadius
    );

    // Start transition in
    startTransitionIn();
}

void MainMenu::goToNetworkScreen() {
    auto improvedNetworkMenu = std::make_shared<ImprovedNetworkMenu>(window, stateChangeCallback);
    improvedNetworkMenu->init();
    startTransitionTo(improvedNetworkMenu);
}

void MainMenu::goToNewGameScreen() {
    auto newGameMenu = std::make_shared<NewGameMenu>(window, stateChangeCallback);
    newGameMenu->init();
    startTransitionTo(newGameMenu);
}

void MainMenu::goToLoadGameScreen() {
    auto loadGameMenu = std::make_shared<LoadGameMenu>(window, stateChangeCallback);
    loadGameMenu->init();
    startTransitionTo(loadGameMenu);
}

void MainMenu::goToAboutScreen() {
    auto aboutScreen = std::make_shared<AboutScreen>(window, stateChangeCallback);
    aboutScreen->init();
    startTransitionTo(aboutScreen);
}

void MainMenu::handleInput(const sf::Event event) {
    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
            mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

            if (buttons[0].wasClicked()) {
                goToNewGameScreen();
            } else if (buttons[1].wasClicked()) {
                goToLoadGameScreen();
            } else if (buttons[2].wasClicked()) {
                goToNetworkScreen();
            } else if (buttons[3].wasClicked()) {
                goToAboutScreen();
            } else if (buttons[4].wasClicked()) {
                window.close();
            }
        }
    }
}

void MainMenu::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);

    for (const auto &button: buttons) {
        button.draw(window);
    }

    renderTransition();
}

void MainMenu::update(float deltaTime) {
    GameState::update(deltaTime);

    for (auto &button: buttons) {
        button.update(window);
    }
}