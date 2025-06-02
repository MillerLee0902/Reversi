//
// NetworkGameScreen.h - 网络对战游戏界面
// 集成到你现有的SFML项目中
//

#ifndef NETWORKGAMESCREEN_H
#define NETWORKGAMESCREEN_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/Global.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <utility>
#include <array>
#include <string>
#include <thread>
#include <atomic>

class MainMenu;

class NetworkGameScreen : public GameState {
private:
    static const int BOARD_SIZE = 8;

    // 网络相关
    sf::TcpSocket socket;
    std::atomic<bool> connected;
    std::atomic<bool> isMyTurn;
    std::atomic<bool> gameOver;
    std::string playerColor; // "BLACK" 或 "WHITE"
    std::string serverAddress;
    unsigned short serverPort;

    // 游戏状态
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> board;
    std::string gameStatus;
    std::string connectionStatus;

    // UI元素
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text statusText;
    sf::Text playerInfoText;
    sf::Text connectionText;

    // 游戏板渲染
    sf::RectangleShape boardBackground;
    sf::RectangleShape cellHighlight;
    std::vector<sf::CircleShape> pieces;

    Button disconnectButton;

    // 网络线程
    std::thread networkThread;
    std::atomic<bool> networkRunning;

    // 常量
    const float cellSize = 50.0f;
    const float boardX = (WINDOW_WIDTH - 8 * cellSize) / 2.0f;
    const float boardY = (WINDOW_HEIGHT - 8 * cellSize) / 2.0f + 20.0f;

    int selectedX, selectedY;

public:
    NetworkGameScreen(sf::RenderWindow &window,
                     std::function<void(std::shared_ptr<GameState>)> callback,
                     const std::string& serverAddr,
                     const unsigned short port)
        : GameState(window, std::move(callback)),
          connected(false),
          isMyTurn(false),
          gameOver(false),
          serverAddress(serverAddr),
          serverPort(port),
          backgroundSprite(resources->getTexture("game_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          statusText(sf::Text(resources->getFont("main"))),
          playerInfoText(sf::Text(resources->getFont("main"))),
          connectionText(sf::Text(resources->getFont("main"))),
          disconnectButton(
              sf::Vector2f(120.0f, 40.0f),
              sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Disconnect",
              sf::Color(200, 60, 60),
              sf::Color(220, 80, 80),
              sf::Color(180, 40, 40),
              resources->getSoundBuffer("click"),
              15.0f
          ),
          networkRunning(false),
          selectedX(-1),
          selectedY(-1) {

        initializeBoard();
    }

    ~NetworkGameScreen() override {
        disconnect();
    }

    void sendMove(int x, int y) {
        if (!connected || !isMyTurn) return;

        sf::Packet packet;
        std::string moveMessage = "MOVE:" + std::to_string(x) + "," + std::to_string(y);
        packet << moveMessage;

        if (socket.send(packet) == sf::Socket::Status::Done) {
            std::cout << "发送移动: (" << x << "," << y << ")" << std::endl;
            isMyTurn = false; // 等待服务器响应
        }
    }

    void updateBoardPieces() {
        pieces.clear();

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (board[y][x] == 'B' || board[y][x] == 'W') {
                    sf::CircleShape piece(cellSize / 2.0f - 5.0f);

                    if (board[y][x] == 'B') {
                        piece.setFillColor(sf::Color::Black);
                    } else {
                        piece.setFillColor(sf::Color::White);
                    }

                    piece.setPosition({boardX + x * cellSize + 5.0f, boardY + y * cellSize + 5.0f});
                    piece.setOutlineThickness(2.0f);
                    piece.setOutlineColor(sf::Color(100, 100, 100));

                    pieces.push_back(piece);
                }
            }
        }
    }

    void disconnect() {
        networkRunning = false;
        connected = false;

        if (networkThread.joinable()) {
            networkThread.join();
        }

        socket.disconnect();
    }

    void init() override {
        // 设置背景
        sf::Vector2u textureSize = resources->getTexture("game_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // 设置标题
        titleText.setString("Online Reversi");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText.setPosition({WINDOW_WIDTH / 2.0f, 40.0f});

        // 设置状态文本
        statusText.setString("连接到服务器...");
        statusText.setCharacterSize(20);
        statusText.setFillColor(sf::Color::Yellow);
        statusText.setPosition({50.0f, 100.0f});

        playerInfoText.setString("等待分配角色...");
        playerInfoText.setCharacterSize(18);
        playerInfoText.setFillColor(sf::Color::White);
        playerInfoText.setPosition({50.0f, 130.0f});

        connectionText.setString("状态: 连接中");
        connectionText.setCharacterSize(16);
        connectionText.setFillColor(sf::Color::Cyan);
        connectionText.setPosition({WINDOW_WIDTH - 200.0f, 100.0f});

        // 设置游戏板
        boardBackground.setSize({cellSize * 8, cellSize * 8});
        boardBackground.setPosition({boardX, boardY});
        boardBackground.setFillColor(sf::Color(30, 120, 30));
        boardBackground.setOutlineThickness(2.0f);
        boardBackground.setOutlineColor(sf::Color::Black);

        cellHighlight.setSize({cellSize, cellSize});
        cellHighlight.setFillColor(sf::Color(100, 200, 100, 100));

        // 连接到服务器
        connectToServer();

        startTransitionIn();
    }

    void connectToServer();

    void networkLoop();

    void handleServerMessage(const std::string &message);

    void updateBoardFromServer(const std::string &boardData);

    void handleGameEnd(const std::string &message);

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override {
        GameState::update(deltaTime);

        disconnectButton.update(window);

        // 更新状态文本
        if (connected) {
            if (gameOver) {
                statusText.setString(gameStatus);
                statusText.setFillColor(sf::Color::Red);
            } else if (isMyTurn) {
                statusText.setString("轮到你了!");
                statusText.setFillColor(sf::Color::Green);
            } else {
                statusText.setString("等待对手...");
                statusText.setFillColor(sf::Color::Yellow);
            }

            playerInfoText.setString("你是: " + playerColor);
            connectionText.setString("状态: " + connectionStatus);
        } else {
            statusText.setString("连接断开");
            statusText.setFillColor(sf::Color::Red);
        }

        // 更新棋盘显示
        updateBoardPieces();

        // 更新鼠标高亮
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int boardCoordX = static_cast<int>((mousePos.x - boardX) / cellSize);
        int boardCoordY = static_cast<int>((mousePos.y - boardY) / cellSize);

        if (boardCoordX >= 0 && boardCoordX < 8 && boardCoordY >= 0 && boardCoordY < 8) {
            selectedX = boardCoordX;
            selectedY = boardCoordY;
        } else {
            selectedX = -1;
            selectedY = -1;
        }
    }

    void render() override
    {
        window.draw(backgroundSprite);
        window.draw(boardBackground);

        // 画网格线
        for (int i = 0; i <= 8; i++) {
            sf::Vertex hLine[] = {
                sf::Vertex{{boardX, boardY + i * cellSize}, sf::Color::Black},
                sf::Vertex{{boardX + 8 * cellSize, boardY + i * cellSize}, sf::Color::Black}
            };
            window.draw(hLine, 2, sf::PrimitiveType::Lines);

            sf::Vertex vLine[] = {
                sf::Vertex{{boardX + i * cellSize, boardY}, sf::Color::Black},
                sf::Vertex{{boardX + i * cellSize, boardY + 8 * cellSize}, sf::Color::Black}
            };
            window.draw(vLine, 2, sf::PrimitiveType::Lines);
        }

        // 画高亮
        if (selectedX >= 0 && selectedY >= 0) {
            cellHighlight.setPosition({boardX + selectedX * cellSize, boardY + selectedY * cellSize});
            window.draw(cellHighlight);
        }

        // 画棋子
        for (const auto &piece : pieces) {
            window.draw(piece);
        }

        // 画UI文本
        window.draw(titleText);
        window.draw(statusText);
        window.draw(playerInfoText);
        window.draw(connectionText);

        disconnectButton.draw(window);

        renderTransition();
    }

    void initializeBoard();

};

#endif //NETWORKGAMESCREEN_H
