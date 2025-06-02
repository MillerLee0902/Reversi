//
// Created by Miller on 2025/4/9.
//

#include "../headers/SplashScreen.h"
#include "../headers/MainMenu.h"

void SplashScreen::init() {

    // Set up splash screen

    // Scale to fit the window
    sf::Vector2u textureSize = resources->getTexture("splash").getSize();
    float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // Set up title text
    titleText.setString("REVERSI");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineThickness(3.0f);
    titleText.setOutlineColor(sf::Color::Black);

    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.getCenter());
    titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 3.0f});

    // Set up prompt text
    promptText.setString("Click anywhere to continue");
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setOutlineThickness(2.0f);
    promptText.setOutlineColor(sf::Color::Black);

    // Center prompt
    sf::FloatRect promptBounds = promptText.getLocalBounds(); // get boundary
    promptText.setOrigin(promptBounds.getCenter());
    promptText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT * 0.8f});

    // Start background music
    resources->playMusic("bgm", true);

    // Start transition in
    startTransitionIn();
}

void SplashScreen::handleInput(sf::Event event) {

    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        // Create the main menu state
        auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
        mainMenu->init();

        // Transition to main menu
        startTransitionTo(mainMenu);
    }
}

void SplashScreen::update(float deltaTime) {
    GameState::update(deltaTime);

    // Make the prompt text pulse
    displayTime += deltaTime;
    float alpha = 128.0f + 127.0f * std::sin(displayTime * 2.0f);
    promptText.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
}

void SplashScreen::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);
    window.draw(promptText);

    renderTransition();
}