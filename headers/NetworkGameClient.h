//
// NetworkGameClient.h - 修復版本：解決遊戲結束和斷線問題
//

#ifndef NETWORKGAMECLIENT_H
#define NETWORKGAMECLIENT_H

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/Global.h"
#include "../headers/FundamentalFunction.h"
#include "../headers/VictoryScreen.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <utility>
#include <array>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

class MainMenu;

class NetworkGameClient : public GameState {
private:
    static const int BOARD_SIZE = 8;

    // 網路相關
    sf::TcpSocket socket;
    std::atomic<bool> connected;
    std::atomic<bool> isMyTurn;
    std::atomic<bool> gameOver;
    std::atomic<bool> shouldTransition; // 新增：控制是否需要轉換畫面
    std::shared_ptr<GameState> nextScreen; // 新增：下一個要轉換到的畫面
    std::string playerColor;
    std::string serverAddress;
    unsigned short serverPort;

    // 遊戲狀態和邏輯
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> board{};
    FundamentalFunction gameLogic;
    std::string gameStatus;
    std::string connectionStatus;
    int blackScore, whiteScore;

    // UI元素
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text statusText;
    sf::Text player1Text;
    sf::Text player2Text;
    sf::Text currentPlayerText;
    sf::Text connectionText;
    sf::Text scoreText;

    // 遊戲板渲染
    sf::RectangleShape boardBackground;
    sf::RectangleShape cellHighlight;
    std::vector<sf::CircleShape> pieces;

    Button disconnectButton;

    // 網路線程
    std::thread networkThread;
    std::atomic<bool> networkRunning;

    // 心跳機制
    std::chrono::steady_clock::time_point lastPingTime;
    std::thread pingThread;
    std::atomic<bool> pingRunning;

    // 常量
    const float cellSize = 50.0f;
    const float boardX = (WINDOW_WIDTH - 8 * cellSize) / 2.0f;
    const float boardY = (WINDOW_HEIGHT - 8 * cellSize) / 2.0f + 50.0f;

    int selectedX, selectedY;

public:
    NetworkGameClient(sf::RenderWindow &window,
                     std::function<void(std::shared_ptr<GameState>)> callback,
                     std::string  serverAddr,
                     const unsigned short port)
        : GameState(window, std::move(callback)),
          connected(false),
          isMyTurn(false),
          gameOver(false),
          shouldTransition(false), // 初始化新成員
          serverAddress(std::move(serverAddr)),
          serverPort(port),
          backgroundSprite(resources->getTexture("game_bg")),
          titleText(sf::Text(resources->getFont("main"))),
          statusText(sf::Text(resources->getFont("main"))),
          player1Text(sf::Text(resources->getFont("main"))),
          player2Text(sf::Text(resources->getFont("main"))),
          currentPlayerText(sf::Text(resources->getFont("main"))),
          connectionText(sf::Text(resources->getFont("main"))),
          scoreText(sf::Text(resources->getFont("main"))),
          disconnectButton(
              sf::Vector2f(120.0f, 40.0f),
              sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
              resources->getFont("main"),
              "Disconnect",
              sf::Color(160, 90, 80),
              sf::Color(180, 110, 100),
              sf::Color(140, 70, 60),
              resources->getSoundBuffer("click"),
              15.0f
          ),
          networkRunning(false),
          pingRunning(false),
          selectedX(-1),
          selectedY(-1),
          blackScore(2),
          whiteScore(2) {

        initializeBoard();
    }

    ~NetworkGameClient() override {
        disconnect();
    }

    void init() override {
        // 設置背景
        sf::Vector2u textureSize = resources->getTexture("game_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // 設置標題
        titleText.setString("Online Reversi");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color(45, 45, 45));
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color(200, 200, 200, 150));

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText.setPosition({WINDOW_WIDTH / 2.0f, 40.0f});

        // 設置玩家名稱和狀態文本
        player1Text.setString("Player 1 (Black)");
        player1Text.setCharacterSize(20);
        player1Text.setFillColor(sf::Color(45, 45, 45));
        player1Text.setPosition({50.0f, 100.0f});

        player2Text.setString("Player 2 (White)");
        player2Text.setCharacterSize(20);
        player2Text.setFillColor(sf::Color(45, 45, 45));
        player2Text.setPosition({WINDOW_WIDTH - 250.0f, 100.0f});

        statusText.setString("Connecting to server...");
        statusText.setCharacterSize(20);
        statusText.setFillColor(sf::Color(110, 123, 88));
        statusText.setPosition({50.0f, 80.0f});

        currentPlayerText.setString("Current Turn: Black");
        currentPlayerText.setCharacterSize(20);
        currentPlayerText.setFillColor(sf::Color(45, 45, 45));
        currentPlayerText.setPosition({WINDOW_WIDTH / 2.0f - 80.0f, 80.0f});

        connectionText.setString("Status: Connecting");
        connectionText.setCharacterSize(16);
        connectionText.setFillColor(sf::Color(120, 140, 160));
        connectionText.setPosition({WINDOW_WIDTH - 250.0f, 130.0f});

        scoreText.setString("Score: 2 - 2");
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color(45, 45, 45));
        scoreText.setPosition({WINDOW_WIDTH / 2.0f - 60.0f, 110.0f});

        // 設置遊戲板
        boardBackground.setSize({cellSize * 8, cellSize * 8});
        boardBackground.setPosition({boardX, boardY});
        boardBackground.setFillColor(sf::Color(240, 235, 220));
        boardBackground.setOutlineThickness(2.0f);
        boardBackground.setOutlineColor(sf::Color(76, 76, 76));

        cellHighlight.setSize({cellSize, cellSize});
        cellHighlight.setFillColor(sf::Color(180, 120, 130, 100));

        gameLogic.initialize();
        connectToServer();
        startTransitionIn();
    }

    void handleInput(sf::Event event) override;

    void update(float deltaTime) override {
        GameState::update(deltaTime);

        // 檢查是否需要轉換畫面
        if (shouldTransition.load() && nextScreen) {
            shouldTransition = false;
            startTransitionTo(nextScreen);
            return;
        }

        disconnectButton.update(window);

        if (connected) {
            if (gameOver) {
                statusText.setString(gameStatus);
                statusText.setFillColor(sf::Color(160, 90, 80));
            } else if (isMyTurn) {
                statusText.setString("Your turn!");
                statusText.setFillColor(sf::Color(110, 123, 88));

                if (playerColor == "BLACK") {
                    currentPlayerText.setString("Current Turn: Black (You)");
                } else {
                    currentPlayerText.setString("Current Turn: White (You)");
                }
            } else {
                statusText.setString("Waiting for opponent...");
                statusText.setFillColor(sf::Color(130, 110, 140));

                if (playerColor == "BLACK") {
                    currentPlayerText.setString("Current Turn: White (Opponent)");
                } else {
                    currentPlayerText.setString("Current Turn: Black (Opponent)");
                }
            }

            connectionText.setString("Status: " + connectionStatus);
            scoreText.setString("Score: " + std::to_string(blackScore) + " - " + std::to_string(whiteScore));
        } else {
            statusText.setString("Connection lost");
            statusText.setFillColor(sf::Color(160, 90, 80));
            currentPlayerText.setString("Connection lost");
        }

        updateAvailableMoves();
        updateBoardPieces();

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

    void render() override {
        window.draw(backgroundSprite);
        window.draw(boardBackground);

        for (int i = 0; i <= 8; i++) {
            sf::Vertex hLine[] = {
                sf::Vertex{{boardX, boardY + i * cellSize}, sf::Color(76, 76, 76)},
                sf::Vertex{{boardX + 8 * cellSize, boardY + i * cellSize}, sf::Color(76, 76, 76)}
            };
            window.draw(hLine, 2, sf::PrimitiveType::Lines);

            sf::Vertex vLine[] = {
                sf::Vertex{{boardX + i * cellSize, boardY}, sf::Color(76, 76, 76)},
                sf::Vertex{{boardX + i * cellSize, boardY + 8 * cellSize}, sf::Color(76, 76, 76)}
            };
            window.draw(vLine, 2, sf::PrimitiveType::Lines);
        }

        if (selectedX >= 0 && selectedY >= 0) {
            cellHighlight.setPosition({boardX + selectedX * cellSize, boardY + selectedY * cellSize});
            window.draw(cellHighlight);
        }

        for (const auto &piece : pieces) {
            window.draw(piece);
        }

        window.draw(titleText);
        window.draw(statusText);
        window.draw(player1Text);
        window.draw(player2Text);
        window.draw(currentPlayerText);
        window.draw(connectionText);
        window.draw(scoreText);

        disconnectButton.draw(window);
        renderTransition();
    }

private:
    void initializeBoard() {
        for (auto& row : board) {
            row.fill('s');
        }

        board[3][3] = 'w';
        board[3][4] = 'b';
        board[4][3] = 'b';
        board[4][4] = 'w';

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                gameLogic.board[y][x] = board[y][x];
            }
        }
    }

    void updateAvailableMoves() {
        if (!connected || gameOver) return;

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (board[y][x] == 'B') {
                    gameLogic.board[y][x] = 'b';
                } else if (board[y][x] == 'W') {
                    gameLogic.board[y][x] = 'w';
                } else {
                    gameLogic.board[y][x] = 's';
                }
            }
        }

        if (isMyTurn) {
            bool isWhiteTurn = (playerColor == "WHITE");
            gameLogic.showPlayPlace(isWhiteTurn);
        } else {
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    if (gameLogic.board[y][x] == 'a') {
                        gameLogic.board[y][x] = 's';
                    }
                }
            }
        }
    }

    void connectToServer() {
        std::cout << "Trying to connect to: " << serverAddress << ":" << serverPort << std::endl;

        socket.setBlocking(true);

        auto ipAddress = sf::IpAddress::resolve(serverAddress);
        if (!ipAddress.has_value()) {
            connectionStatus = "Can't resolve server address";
            std::cout << "Can't resolve server address: " << serverAddress << std::endl;
            return;
        }

        std::cout << "Resolved address: " << ipAddress.value().toString() << std::endl;

        connectionStatus = "Connecting...";
        sf::Socket::Status status = socket.connect(ipAddress.value(), serverPort, sf::seconds(10));

        if (status == sf::Socket::Status::Done) {
            connected = true;
            connectionStatus = "Connected to server";
            std::cout << "Successfully connected to server!" << std::endl;
            startNetworkThreads();
        } else {
            connectionStatus = "Failed to connect";
            std::cout << "Failed to connect to server! Status: " << static_cast<int>(status) << std::endl;

            switch(status) {
                case sf::Socket::Status::NotReady:
                    connectionStatus = "Connection timeout";
                    break;
                case sf::Socket::Status::Error:
                    connectionStatus = "Connection error";
                    break;
                default:
                    connectionStatus = "Connection failed";
            }
        }
    }

    void startNetworkThreads() {
        networkRunning = true;
        networkThread = std::thread(&NetworkGameClient::networkLoop, this);

        pingRunning = true;
        pingThread = std::thread(&NetworkGameClient::pingLoop, this);
    }

    void networkLoop() {
        sf::Packet packet;
        socket.setBlocking(true);

        while (networkRunning && connected) {
            sf::Socket::Status status = socket.receive(packet);

            if (status == sf::Socket::Status::Done) {
                std::string message;
                packet >> message;
                handleServerMessage(message);
                packet.clear();
            } else if (status == sf::Socket::Status::Disconnected) {
                connected = false;
                connectionStatus = "Disconnected from server";
                std::cout << "Disconnected from server!" << std::endl;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        networkRunning = false;
    }

    void pingLoop() {
        while (pingRunning && connected) {
            auto now = std::chrono::steady_clock::now();
            auto timeSinceLastPing = std::chrono::duration_cast<std::chrono::seconds>(now - lastPingTime);

            if (timeSinceLastPing.count() >= 30) {
                sf::Packet packet;
                packet << "PING";
                if (socket.send(packet) != sf::Socket::Status::Done) {
                    connected = false;
                    break;
                }
                lastPingTime = now;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        pingRunning = false;
    }

    void handleServerMessage(const std::string& message) {
        std::cout << "Received server message: " << message << std::endl;

        if (message.substr(0, 7) == "WELCOME") {
            playerColor = message.substr(8);
            if (playerColor == "BLACK") {
                isMyTurn = true;
                player1Text.setString("Player 1 (Black) - You");
                player2Text.setString("Player 2 (White) - Opponent");
            } else {
                isMyTurn = false;
                player1Text.setString("Player 1 (Black) - Opponent");
                player2Text.setString("Player 2 (White) - You");
            }
            std::cout << "Assigned role: " << playerColor << std::endl;
        }
        else if (message.substr(0, 5) == "BOARD") {
            updateBoardFromServer(message.substr(6));
        }
        else if (message == "YOUR_TURN") {
            isMyTurn = true;
            std::cout << "It's your turn!" << std::endl;
        }
        else if (message == "WAIT_TURN") {
            isMyTurn = false;
            std::cout << "Waiting for opponent..." << std::endl;
        }
        else if (message.substr(0, 8) == "GAME_END") {
            handleGameEnd(message);
        }
        else if (message == "OPPONENT_DISCONNECTED") {
            gameOver = true;
            createVictoryScreen("You");
        }
        else if (message == "INVALID_MOVE") {
            isMyTurn = true;
            statusText.setString("Invalid move! Please try again.");
            statusText.setFillColor(sf::Color(160, 90, 80));
            std::cout << "Invalid move! Restoring player turn." << std::endl;
        }
        else if (message == "NOT_YOUR_TURN") {
            isMyTurn = false;
            statusText.setString("Not your turn! Please wait.");
            statusText.setFillColor(sf::Color(160, 90, 80));
            std::cout << "Attempted to move out of turn." << std::endl;
        }
        else if (message == "PING") {
            sf::Packet pong;
            pong << "PONG";
            socket.send(pong);
        }
    }

    void updateBoardFromServer(const std::string& boardData) {
        if (boardData.length() != 64) {
            std::cout << "Received invalid board data, length: " << boardData.length() << std::endl;
            return;
        }

        std::cout << "Updating board state from server" << std::endl;

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                board[y][x] = boardData[y * 8 + x];
            }
        }

        blackScore = whiteScore = 0;
        for (const auto& row : board) {
            for (char cell : row) {
                if (cell == 'B') blackScore++;
                else if (cell == 'W') whiteScore++;
            }
        }
    }

    void handleGameEnd(const std::string& message) {
        gameOver = true;
        std::cout << "Game ended: " << message << std::endl;

        if (message.find("OPPONENT_DISCONNECTED") != std::string::npos) {
            createVictoryScreen("You");
            return;
        }

        size_t pos1 = message.find(':', 9);
        if (pos1 != std::string::npos) {
            std::string result = message.substr(9, pos1 - 9);
            std::string winner;

            if (result == "BLACK_WINS") {
                winner = (playerColor == "BLACK") ? "You" : "Opponent";
            } else if (result == "WHITE_WINS") {
                winner = (playerColor == "WHITE") ? "You" : "Opponent";
            } else if (result == "TIE") {
                winner = "It's a tie!";
            }

            createVictoryScreen(winner);
        }

        isMyTurn = false;
    }

    void createVictoryScreen(const std::string& winner) {
        // 先斷開網路連接
        disconnect();

        // 創建勝利畫面
        auto victoryScreen = std::make_shared<VictoryScreen>(
            window, stateChangeCallback, winner, blackScore, whiteScore);
        victoryScreen->init();

        // 播放音效
        sf::Sound victorySound(resources->getSoundBuffer("victory"));
        victorySound.play();

        // 設置要轉換到的畫面
        nextScreen = victoryScreen;
        shouldTransition = true;
    }

    void updateBoardPieces() {
        pieces.clear();

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                if (gameLogic.board[y][x] == 'a' && isMyTurn && connected && !gameOver) {
                    sf::CircleShape availableMove(cellSize / 2.0f - 20.0f);
                    availableMove.setFillColor(sf::Color(110, 140, 110, 150));
                    availableMove.setPosition({boardX + x * cellSize + 20.0f, boardY + y * cellSize + 20.0f});
                    pieces.push_back(availableMove);
                }

                char cellValue = board[y][x];
                if (cellValue == 'B' || cellValue == 'W') {
                    sf::CircleShape piece(cellSize / 2.0f - 5.0f);

                    if (cellValue == 'B') {
                        piece.setFillColor(sf::Color::Black);
                    } else {
                        piece.setFillColor(sf::Color::White);
                    }

                    piece.setPosition({boardX + x * cellSize + 5.0f, boardY + y * cellSize + 5.0f});
                    piece.setOutlineThickness(2.0f);
                    piece.setOutlineColor(sf::Color(76, 76, 76));

                    pieces.push_back(piece);
                }
            }
        }
    }

    void sendMove(int x, int y) {
        if (!connected || !isMyTurn) {
            std::cout << "Cannot send move - not connected or not my turn" << std::endl;
            return;
        }

        std::cout << "Attempting move at: (" << x << "," << y << ")" << std::endl;

        sf::Packet packet;
        std::string moveMessage = "MOVE:" + std::to_string(x) + "," + std::to_string(y);
        packet << moveMessage;

        if (socket.send(packet) == sf::Socket::Status::Done) {
            std::cout << "Successfully sent move: (" << x << "," << y << ")" << std::endl;
            isMyTurn = false;
            statusText.setString("Move sent, waiting for confirmation...");
            statusText.setFillColor(sf::Color(130, 110, 140));
        } else {
            std::cout << "Failed to send move to server" << std::endl;
            statusText.setString("Failed to send move. Please try again.");
            statusText.setFillColor(sf::Color(160, 90, 80));
        }
    }

    void safeDisconnectAndReturn();

    void disconnect() {
        networkRunning = false;
        pingRunning = false;
        connected = false;

        if (networkThread.joinable()) {
            networkThread.join();
        }

        if (pingThread.joinable()) {
            pingThread.join();
        }

        socket.disconnect();
    }
};

#endif //NETWORKGAMECLIENT_H