//
// AboutScreen.cpp - 使用可拖曳文字方框的版本
//

#include "../headers/AboutScreen.h"
#include <cstdlib>

void AboutScreen::init() {
    sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
    float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
    backgroundSprite.setScale({scaleX, scaleY});

    titleText.setString("About");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineThickness(2.0f);
    titleText.setOutlineColor(sf::Color::Black);

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.getCenter());
    titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 6.0f});

    std::string aboutContent =
        "Reversi Game\n"
        "\n"
        "Created by: Alan, Miller and Jimmy\n"
        "\n"
        "This is a classic Reversi game implementation using SFML for the graphics and user interface.\n"
        "\n"
        "Game Rules:\n"
        "1. Reversi is played on an 8×8 board with black and white discs\n"
        "2. Players take turns placing discs on the board\n"
        "3. A move consists of placing a disc and flipping opponent discs\n"
        "4. To make a valid move, you must flank opponent discs between your disc and an existing disc\n"
        "5. All flanked opponent discs are flipped to your color\n"
        "6. If you cannot make a valid move, your turn is skipped\n"
        "7. The game ends when the board is full or no moves are possible\n"
        "8. The player with the most discs wins\n"
        "\n"
        "Game Features:\n"
        "1. Player vs Player mode (local multiplayer)\n"
        "2. Player vs AI with three difficulty levels:\n"
        "  - Easy: AI thinks 3 moves ahead\n"
        "  - Medium: AI thinks 5 moves ahead  \n"
        "  - Hard: AI thinks 7 moves ahead\n"
        "3. Save and load game functionality\n"
        "4. Online multiplayer support (experimental)\n"
        "5. Timer system with limited chances\n"
        "6. Undo move functionality\n"
        "\n"
        "Controls:\n"
        "1. Use the mouse to place your pieces on valid positions\n"
        "2. Click the Undo button to take back your last move\n"
        "3. Use the Save Game button to save your current progress\n"
        "4. Valid moves are highlighted in light green\n"
        "\n"
        "Development Information:\n"
        "1. Built with SFML 3.0.0 graphics library\n"
        "2. Implements minimax algorithm with alpha-beta pruning for AI\n"
        "3. Features a state-based game architecture\n"
        "4. Includes resource management and audio system\n"
        "5. Supports file-based save/load system\n"
        "\n"
        "Technical Features:\n"
        "1. Object-oriented design with inheritance\n"
        "2. Event-driven input handling\n"
        "3. Smooth transitions between game states\n"
        "4. Dynamic UI scaling and positioning\n"
        "5. Multi-threaded network support\n"
        "\n"
        "2025 All Rights Reserved\n"
        "\n"
        "Thank you for playing our Reversi implementation!\n"
        "We hope you enjoy this classic strategy game.";

    textBox.setText(aboutContent, 16);
    textBox.setBackgroundColor(sf::Color(255, 255, 255, 128));
    textBox.setTextColor(sf::Color::Black);

    startTransitionIn();
}

void AboutScreen::handleInput(sf::Event event) {
    if (!transitioning) {
        textBox.handleInput(event, window);

        if (event.is<sf::Event::MouseButtonReleased>()) {
            const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
            if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                if (backButton.wasClicked()) {
                    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                    mainMenu->init();
                    startTransitionTo(mainMenu);
                } else if (docButton.wasClicked()) {
                    openWebPage("https://hackmd.io/@-plYMV5xSQ2Pm7CxoL7ihg/BJ8nINaAJe");
                }
            }
        }
    }
}

void AboutScreen::update(float deltaTime) {
    GameState::update(deltaTime);

    backButton.update(window);
    docButton.update(window);
    textBox.update(deltaTime);
}

void AboutScreen::render() {
    window.draw(backgroundSprite);
    window.draw(titleText);

    textBox.draw(window);

    backButton.draw(window);
    docButton.draw(window);

    renderTransition();
}

void AboutScreen::openWebPage(const std::string& url) {
    std::string command;

#ifdef _WIN32
    command = "start " + url;
#elif __APPLE__
    command = "open " + url;
#else
    command = "xdg-open " + url;
#endif

    std::system(command.c_str());
}