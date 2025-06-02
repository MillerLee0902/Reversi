//
// UpdatedNetworkMenu.h - 连接到你的服务器的网络菜单
//

#ifndef UPDATEDNETWORKMENU_H
#define UPDATEDNETWORKMENU_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/TextInput.h"
#include "../headers/NetworkGameScreen.h"
#include "../headers/Global.h"
#include <SFML/Graphics.hpp>
#include <utility>

class MainMenu;

class UpdatedNetworkMenu : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text instructionText;
    sf::Text statusText;

    std::vector<Button> buttons;
    std::vector<TextInput> inputs;
    Button backButton;

    bool connecting;
    std::string defaultServer;
    unsigned short defaultPort;

public:
    UpdatedNetworkMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(resources->getTexture("menu_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          instructionText(sf::Text(resources->getFont("main"))),
          statusText(sf::Text(resources->getFont("main"))),
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
          connecting(false),
          defaultServer("127.0.0.1"), // 替换为你的域名
          defaultPort(53000) {
    }

    void init() override {
        // 设置背景
        sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // 设置标题
        titleText.setString("Online Multiplayer");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        // 设置说明文本
        instructionText.setString(
            "连接到在线对战服务器\n\n"
            "• 服务器会自动匹配两个玩家\n"
            "• 黑子先手，白子后手\n"
            "• 如果对手断线，你将自动获胜\n\n"
            "请输入服务器地址和端口："
        );
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setLineSpacing(1.5f);

        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin({instrBounds.size.x / 2.0f, instrBounds.size.y / 2.0f});
        instructionText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.5f});

        // 创建输入框
        sf::Vector2f inputSize(300.0f, 40.0f);
        float startY = WINDOW_HEIGHT / 2.0f + 20.0f;

        // 服务器地址输入框
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY),
            resources->getFont("main"),
            "服务器地址 (例: your-domain.com)"
        ));

        // 端口输入框
        inputs.push_back(TextInput(
            inputSize,
            sf::Vector2f(WINDOW_WIDTH / 2.0f - inputSize.x / 2.0f, startY + 60.0f),
            resources->getFont("main"),
            "端口 (例: 53000)"
        ));

        // 设置默认值
        inputs[0].setValue(defaultServer);
        inputs[1].setValue(std::to_string(defaultPort));

        // 创建按钮
        buttons.push_back(Button(
            sf::Vector2f(150.0f, 50.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 130.0f),
            resources->getFont("main"),
            "连接服务器",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        // 快速连接按钮
        buttons.push_back(Button(
            sf::Vector2f(150.0f, 40.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 75.0f, startY + 190.0f),
            resources->getFont("main"),
            "快速连接",
            sf::Color(40, 120, 200),
            sf::Color(60, 140, 220),
            sf::Color(30, 100, 180),
            resources->getSoundBuffer("click"),
            15.0f
        ));

        // 状态文本
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

        // 更新按钮
        backButton.update(window);
        for (auto &button : buttons) {
            button.update(window);
        }

        // 更新输入框
        for (auto &input : inputs) {
            input.update(window, deltaTime);
        }

        // 更新状态文本位置
        if (!statusText.getString().isEmpty()) {
            sf::FloatRect statusBounds = statusText.getLocalBounds();
            statusText.setOrigin({statusBounds.size.x / 2.0f, statusBounds.size.y / 2.0f});
            statusText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 120.0f});
        }
    }

    void render() override {
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(instructionText);
        window.draw(statusText);

        // 画按钮
        backButton.draw(window);
        for (const auto &button : buttons) {
            button.draw(window);
        }

        // 画输入框
        for (const auto &input : inputs) {
            input.draw(window);
        }

        renderTransition();
    }

private:
    void connectToServer(bool useDefault) {
        connecting = true;
        statusText.setString("正在连接服务器...");
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

        // 创建网络游戏界面
        auto networkGame = std::make_shared<NetworkGameScreen>(
            window, stateChangeCallback, serverAddress, port);

        networkGame->init();
        startTransitionTo(networkGame);
    }
};

#endif //UPDATEDNETWORKMENU_H