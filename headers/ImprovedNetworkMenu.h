//
// ImprovedNetworkMenu.h - 使用可拖曳文字方框的版本
//

#ifndef IMPROVEDNETWORKMENU_H
#define IMPROVEDNETWORKMENU_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/TextInput.h"
#include "../headers/Global.h"
#include "../headers/NetworkGameClient.h"
#include "../headers/ScrollableTextBox.h"
#include <SFML/Graphics.hpp>
#include <utility>

class MainMenu;

class ImprovedNetworkMenu : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text statusText;

    ScrollableTextBox textBox;
    std::vector<Button> buttons;
    std::vector<TextInput> inputs;
    Button backButton;

    std::string defaultServer;
    unsigned short defaultPort;

public:
    ImprovedNetworkMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          statusText(sf::Text(resources->getFont("main"))),
          textBox(sf::Vector2f(WINDOW_WIDTH / 2.0f - 300.0f, WINDOW_HEIGHT / 3.5f),
                  sf::Vector2f(600.0f, 200.0f),
                  resources->getFont("main"),
                  15.0f),
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
          defaultServer("127.0.0.1"),
          defaultPort(53000) {
    }

    void init() override {
        sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        titleText.setString("Online Multiplayer");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        std::string networkInfo =
            "Connect to Online Battle Server\n"
            "\n"
            "How Online Multiplayer Works:\n"
            "1. Server automatically matches two players\n"
            "2. Black moves first, White moves second\n"
            "3. Real-time synchronization between players\n"
            "4. If opponent disconnects, you win automatically\n"
            "5. Stable connection required for best experience\n"
            "\n"
            "Connection Instructions:\n"
            "1. Enter the server address (domain or IP)\n"
            "2. Specify the port number (usually 53000)\n"
            "3. Click Connect to join the matchmaking queue\n"
            "4. Wait for another player to connect\n"
            "5. Game starts automatically when both players ready\n"
            "\n"
            "Server Information:\n"
            "1. Default server: your-domain.com\n"
            "2. Default port: 53000\n"
            "3. Supports TCP connections\n"
            "4. Automatic role assignment (Black/White)\n"
            "5. Built-in ping/pong heartbeat system\n"
            "\n"
            "Network Features:\n"
            "1. Multi-threaded client architecture\n"
            "2. Automatic reconnection attempts\n"
            "3. Connection status monitoring\n"
            "4. Real-time move synchronization\n"
            "5. Game state validation\n"
            "\n"
            "Troubleshooting:\n"
            "1. Ensure stable internet connection\n"
            "2. Check firewall settings\n"
            "3. Verify server address and port\n"
            "4. Try Quick Connect for default settings\n"
            "\n"
            "Enter server details below or use Quick Connect:";

        textBox.setText(networkInfo, 20);  // 從14增加到20
        textBox.setBackgroundColor(sf::Color(255, 255, 255, 128));
        textBox.setTextColor(sf::Color::Black);

        sf::Vector2f inputSize(300.0f, 40.0f);
        float startY = WINDOW_HEIGHT / 2.0f + 120.0f;

        inputs.emplace_back(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY),
            resources->getFont("main"),
            "Server Address (e.g., your-domain.com)"
        );

        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY + 60.0f),
            resources->getFont("main"),
            "Port (e.g., 53000)"
        ));

        inputs[0].setValue(defaultServer);
        inputs[1].setValue(std::to_string(defaultPort));

        buttons.push_back(Button(
            sf::Vector2f(150.0f, 50.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 130.0f),
            resources->getFont("main"),
            "Connect",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        buttons.push_back(Button(
            sf::Vector2f(150.0f, 40.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 190.0f),
            resources->getFont("main"),
            "Quick Connect",
            sf::Color(40, 120, 200),
            sf::Color(60, 140, 220),
            sf::Color(30, 100, 180),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        statusText.setString("");
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::Yellow);
        sf::FloatRect statusBounds = statusText.getLocalBounds();
        statusText.setOrigin({statusBounds.size.x / 2.0f, statusBounds.size.y / 2.0f});
        statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 120.0f});

        startTransitionIn();
    }

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override {
        GameState::update(deltaTime);

        backButton.update(window);
        for (auto &button : buttons) {
            button.update(window);
        }

        for (auto &input : inputs) {
            input.update(window, deltaTime);
        }

        textBox.update(deltaTime);

        if (!statusText.getString().isEmpty()) {
            sf::FloatRect statusBounds = statusText.getLocalBounds();
            statusText.setOrigin({statusBounds.size.x / 2.0f, statusBounds.size.y / 2.0f});
            statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 120.0f});
        }
    }

    void render() override {
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(statusText);

        textBox.draw(window);

        backButton.draw(window);
        for (const auto &button : buttons) {
            button.draw(window);
        }

        for (const auto &input : inputs) {
            input.draw(window);
        }

        renderTransition();
    }

private:
    void connectToServer(bool useDefault) {
        statusText.setString("Connecting to server...");
        statusText.setFillColor(sf::Color::Yellow);

        std::string serverAddress;
        unsigned short port;

        if (useDefault) {
            serverAddress = defaultServer;
            port = defaultPort;
        } else {
            serverAddress = inputs[0].getValue();
            std::string portStr = inputs[1].getValue();

            if (serverAddress.empty()) {
                serverAddress = defaultServer;
            }

            if (portStr.empty()) {
                port = defaultPort;
            } else {
                try {
                    port = static_cast<unsigned short>(std::stoi(portStr));
                } catch (...) {
                    port = defaultPort;
                }
            }
        }

        auto networkGame = std::make_shared<NetworkGameClient>(
            window, stateChangeCallback, serverAddress, port);

        networkGame->init();
        startTransitionTo(networkGame);
    }
};

#endif //IMPROVEDNETWORKMENU_H