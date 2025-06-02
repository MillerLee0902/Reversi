//
// NetworkMenu.cpp - 修复版本
// Created by Miller on 2025/5/23.
//

#include "../headers/NetworkMenu.h"
#include "../headers/NetworkGameScreen.h"  // 在.cpp文件中包含，避免循環依賴
#include "../headers/MainMenu.h"

void NetworkMenu::startNetworkGame() {
    // 根据 NetworkGameScreen 的构造函数，只需要服务器地址和端口
    // 从 NetworkGameScreen.h 可以看到构造函数签名：
    // NetworkGameScreen(sf::RenderWindow &window,
    //                  std::function<void(std::shared_ptr<GameState>)> callback,
    //                  const std::string& serverAddr,
    //                  const unsigned short port)

    // 创建网络游戏界面 - 使用默认服务器和端口
    const std::shared_ptr<GameState> networkGameScreen = std::make_shared<NetworkGameScreen>(
        window,
        stateChangeCallback,
        "127.0.0.1",  // 默认本地服务器地址
        12345         // 默认端口
    );

    networkGameScreen->init();
    startTransitionTo(networkGameScreen);
}

void NetworkMenu::handleInput(sf::Event event) {
    if (!transitioning) {
        // Handle text input
        for (auto &input : inputs) {
            input.handleInput(event);
        }

        if (event.is<sf::Event::MouseButtonReleased>()) {
            if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
                mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

                // Always check back button
                if (backButton.wasClicked()) {
                    networkManager.disconnect();
                    const std::shared_ptr<GameState> mainMenu = std::make_shared<MainMenu>(
                        window, stateChangeCallback);
                    mainMenu->init();
                    startTransitionTo(mainMenu);
                    return;
                }

                handleButtonClicks();
            }
        }
    }
}

// 如果你想要更完整的实现，可以修改 startNetworkGame() 来使用用户输入的服务器信息
void NetworkMenu::startNetworkGameWithUserInput() {
    std::string serverAddress = "127.0.0.1";  // 默认值
    unsigned short port = 12345;              // 默认值

    // 如果有输入框，从中获取服务器信息
    if (inputs.size() >= 2) {
        std::string serverInput = inputs[0].getValue();
        std::string portInput = inputs[1].getValue();

        if (!serverInput.empty()) {
            serverAddress = serverInput;
        }

        if (!portInput.empty()) {
            try {
                port = static_cast<unsigned short>(std::stoi(portInput));
            } catch (...) {
                port = 12345;  // 如果转换失败，使用默认端口
            }
        }
    }

    // 创建网络游戏界面
    const std::shared_ptr<GameState> networkGameScreen = std::make_shared<NetworkGameScreen>(
        window,
        stateChangeCallback,
        serverAddress,
        port
    );

    networkGameScreen->init();
    startTransitionTo(networkGameScreen);
}

void NetworkMenu::updateNetworkStatus() {
    if (connectionEstablished && currentState == MenuState::WAITING) {
        setupConnectedScreen();
    }

    // Update status text position
    if (!statusText.getString().isEmpty()) {
        statusText.setCharacterSize(20);
        statusText.setFillColor(sf::Color::Yellow);
        sf::FloatRect statusBounds = statusText.getLocalBounds();
        statusText.setOrigin(statusBounds.getCenter());
        statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 150.0f});
    }
}

void NetworkMenu::handleButtonClicks() {
        switch (currentState) {
            case MenuState::MAIN:
                if (buttons[0].wasClicked()) { // Host Game
                    setupHostMenu();
                } else if (buttons[1].wasClicked()) { // Join Game
                    setupJoinMenu();
                }
                break;

            case MenuState::HOST_SETUP:
                if (buttons[0].wasClicked()) { // Start Host
                    std::string playerName = inputs[0].getValue();
                    std::string portStr = inputs[1].getValue();

                    if (playerName.empty()) playerName = "Host";
                    if (portStr.empty()) portStr = "12345";

                    auto port = static_cast<unsigned short>(std::stoi(portStr));

                    if (networkManager.hostGame(port, playerName)) {
                        isHosting = true;
                        setupWaitingScreen();
                    } else {
                        statusText.setString("Failed to start host!");
                        statusText.setFillColor(sf::Color::Red);
                        sf::FloatRect statusBounds = statusText.getLocalBounds();
                        statusText.setOrigin(statusBounds.getCenter());
                        statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 150.0f});
                    }
                }
                break;

            case MenuState::JOIN_SETUP:
                if (buttons[0].wasClicked()) { // Join Game
                    std::string playerName = inputs[0].getValue();
                    std::string hostIP = inputs[1].getValue();
                    std::string portStr = inputs[2].getValue();

                    if (playerName.empty()) playerName = "Player";
                    if (hostIP.empty()) hostIP = "127.0.0.1";
                    if (portStr.empty()) portStr = "12345";

                    unsigned short port = static_cast<unsigned short>(std::stoi(portStr));

                    if (networkManager.joinGame(hostIP, port, playerName)) {
                        isConnecting = true;
                        setupWaitingScreen();
                    } else {
                        statusText.setString("Failed to connect!");
                        statusText.setFillColor(sf::Color::Red);
                        sf::FloatRect statusBounds = statusText.getLocalBounds();
                        statusText.setOrigin(statusBounds.getCenter());
                        statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 150.0f});
                    }
                }
                break;

            case MenuState::CONNECTED:
                if (buttons[0].wasClicked()) { // Start Game
                    startNetworkGame();
                }
                break;

            default:
                break;
        }
    }
