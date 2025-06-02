//
// Created by Miller on 2025/4/9.
//

#include <utility>

#include "../headers/ReversiGame.h"
#include "../headers/SplashScreen.h"

void ReversiGame::initResources() const {
    ResourceManager *resources = ResourceManager::getInstance();

    // Load fonts
    resources->loadFont("main", FONT_PATH + "main_font_.ttf");

    // Load textures
    resources->loadTexture("splash", TEXTURE_PATH + "bg.jpg");
    resources->loadTexture("menu_bg", TEXTURE_PATH + "bg.jpg");
    resources->loadTexture("game_bg", TEXTURE_PATH + "bg.jpg");
    resources->loadTexture("victory_bg", TEXTURE_PATH + "bg.jpg");

    // Load sounds
    resources->loadSoundBuffer("click", SOUND_PATH + "click.wav");
    resources->loadSoundBuffer("place", SOUND_PATH + "place.wav");
    resources->loadSoundBuffer("victory", SOUND_PATH + "victory.wav");

    // Load music
    resources->loadMusic("bgm", MUSIC_PATH + "bgm.ogg");

    // Create saves directory if it doesn't exist
    try {
        std::filesystem::create_directories("saves");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating saves directory: " << e.what() << std::endl;
    }
}

void ReversiGame::setInitialState() {
    // Set up state change callback function
    auto stateChangeCallback = [this](std::shared_ptr<GameState> newState) {
        currentState = std::move(newState);
    };

    // Create splash screen as the initial state
    auto splashScreen = std::make_shared<SplashScreen>(window, stateChangeCallback);

    // Initialize the state
    splashScreen->init();

    // Set as current state
    currentState = splashScreen;
}
// 在ReversiGame.cpp的run()方法中，事件循环需要修改为：
void ReversiGame::run() {
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();

        // SFML 3.0 事件处理
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

            // 传递事件给当前状态处理
            currentState->handleInput(*event);
        }

        // Update
        currentState->update(dt);

        // Render
        window.clear();
        currentState->render();
        window.display();
    }
}
