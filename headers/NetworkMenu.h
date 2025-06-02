//
// Network Game Menu for online multiplayer
// Created for enhanced Reversi game
//

#ifndef NETWORKMENU_H
#define NETWORKMENU_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/TextInput.h"
#include "../headers/NetworkManager.h"
#include "../headers/Global.h"
#include <SFML/Graphics.hpp>
#include <utility>

class NetworkGameScreen;
class MainMenu;

class NetworkMenu : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text statusText;
    sf::Text instructionText;

    std::vector<Button> buttons;
    std::vector<TextInput> inputs;
    Button backButton;

    NetworkManager networkManager;
    bool isHosting;
    bool isConnecting;
    bool connectionEstablished;

    enum class MenuState {
        MAIN,
        HOST_SETUP,
        JOIN_SETUP,
        WAITING,
        CONNECTED
    } currentState;

public:
    NetworkMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          statusText(sf::Text(resources->getFont("main"))),
          instructionText(sf::Text(resources->getFont("main"))),
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
          ),
          isHosting(false),
          isConnecting(false),
          connectionEstablished(false),
          currentState(MenuState::MAIN) {
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
        titleText.setOrigin(titleBounds.getCenter());
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        setupMainMenu();

        // Set up network callback
        networkManager.setMessageCallback([this](const NetworkMessage& msg) {
            if (msg.type == MessageType::PLAYER_INFO) {
                connectionEstablished = true;
                currentState = MenuState::CONNECTED;
            }
        });

        // Start transition in
        startTransitionIn();
    }

    void handleInput(sf::Event event) override;

    void startNetworkGameWithUserInput();

    void update(float deltaTime) override {
        GameState::update(deltaTime);

        // Update buttons
        backButton.update(window);
        for (auto &button : buttons) {
            button.update(window);
        }

        // Update text inputs
        for (auto &input : inputs) {
            input.update(window, deltaTime);
        }

        // Update network status
        updateNetworkStatus();
    }

    void render() override {
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(statusText);
        window.draw(instructionText);

        // Draw buttons
        backButton.draw(window);
        for (const auto &button : buttons) {
            button.draw(window);
        }

        // Draw text inputs
        for (const auto &input : inputs) {
            input.draw(window);
        }

        renderTransition();
    }

private:
    void setupMainMenu() {
        currentState = MenuState::MAIN;
        buttons.clear();
        inputs.clear();

        // Set up instruction text
        instructionText.setString(
            "Choose how to play online:\n\n"
            "Host Game: Create a game room for others to join\n"
            "Join Game: Connect to an existing game room"
        );
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::Black);
        instructionText.setLineSpacing(1.5f);

        // Center instruction text
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.getCenter());
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.5f});

        sf::Vector2f buttonSize(200.0f, 50.0f);
        float startY = WINDOW_HEIGHT / 2.0f + 50.0f;

        // Host Game button
        buttons.emplace_back(
            buttonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY),
            resources->getFont("main"),
            "Host Game",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        );

        // Join Game button
        buttons.emplace_back(
            buttonSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - buttonSize.x / 2.0f, startY + 70.0f),
            resources->getFont("main"),
            "Join Game",
            sf::Color(40, 120, 200),
            sf::Color(60, 140, 220),
            sf::Color(30, 100, 180),
            resources->getSoundBuffer("click"),
            15.0f
        );

        statusText.setString("");
    }

    void setupHostMenu() {
        currentState = MenuState::HOST_SETUP;
        buttons.clear();
        inputs.clear();

        // Set up instruction text
        instructionText.setString(
            "Host a game room:\n\n"
            "Enter your name and the port number to use.\n"
            "Share your IP address with other players."
        );
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setLineSpacing(1.5f);

        // Center instruction text
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.getCenter());
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 3.0f});

        sf::Vector2f inputSize(250.0f, 40.0f);
        float startY = WINDOW_HEIGHT / 2.0f;

        // Player name input
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY),
            resources->getFont("main"),
            "Your Name"
        ));

        // Port input
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY + 50.0f),
            resources->getFont("main"),
            "Port (e.g., 12345)"
        ));

        // Start Host button
        buttons.push_back(Button(
            sf::Vector2f(150.0f, 50.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 120.0f),
            resources->getFont("main"),
            "Start Host",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        statusText.setString("");
    }

    void setupJoinMenu() {
        currentState = MenuState::JOIN_SETUP;
        buttons.clear();
        inputs.clear();

        // Set up instruction text
        instructionText.setString(
            "Join a game room:\n\n"
            "Enter your name, the host's IP address,\n"
            "and the port number they provided."
        );
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setLineSpacing(1.5f);

        // Center instruction text
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.getCenter());
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 3.0f});

        sf::Vector2f inputSize(250.0f, 40.0f);
        float startY = WINDOW_HEIGHT / 2.0f - 20.0f;

        // Player name input
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY),
            resources->getFont("main"),
            "Your Name"
        ));

        // IP address input
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY + 50.0f),
            resources->getFont("main"),
            "Host IP Address"
        ));

        // Port input
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY + 100.0f),
            resources->getFont("main"),
            "Port"
        ));

        // Join button
        buttons.push_back(Button(
            sf::Vector2f(150.0f, 50.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 170.0f),
            resources->getFont("main"),
            "Join Game",
            sf::Color(40, 120, 200),
            sf::Color(60, 140, 220),
            sf::Color(30, 100, 180),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        statusText.setString("");
    }

    void setupWaitingScreen() {
        currentState = MenuState::WAITING;
        buttons.clear();
        inputs.clear();

        if (isHosting) {
            instructionText.setString(
                "Waiting for a player to join...\n\n"
                "Share your IP address and port with other players.\n"
                "They can find your IP at: www.whatismyip.com"
            );
            statusText.setString("Hosting game on port: " + std::to_string(12345));
        } else {
            instructionText.setString(
                "Connecting to host...\n\n"
                "Please wait while we establish the connection."
            );
            statusText.setString("Connecting...");
        }

        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setLineSpacing(1.5f);

        // Center instruction text
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.getCenter());
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});
    }

    void setupConnectedScreen() {
        currentState = MenuState::CONNECTED;
        buttons.clear();
        inputs.clear();

        instructionText.setString(
            "Connection established!\n\n"
            "Connected to: " + networkManager.getOpponentName() + "\n"
            "Ready to start the game?"
        );
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setLineSpacing(1.5f);

        // Center instruction text
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.getCenter());
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});

        // Start Game button
        buttons.push_back(Button(
            sf::Vector2f(200.0f, 50.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 100.0f, WINDOW_HEIGHT / 2.0f + 80.0f),
            resources->getFont("main"),
            "Start Game",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        statusText.setString("Ready to play!");
    }

    void handleButtonClicks();

    void updateNetworkStatus();

    void startNetworkGame();
};

#endif //NETWORKMENU_H