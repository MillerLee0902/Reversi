
//
// NetworkGameScreen.cpp - 实现文件
//

#include "../headers/NetworkGameScreen.h"
#include "../headers/MainMenu.h"
#include <iostream>

// 如果你需要完整的.cpp实现文件，这里是主要的方法实现
// 大部分逻辑已经在头文件中实现为内联函数

void NetworkGameScreen::initializeBoard() {
    for (auto& row : board) {
        row.fill('.');
    }

    // 初始化中央四个子
    board[3][3] = 'W';
    board[3][4] = 'B';
    board[4][3] = 'B';
    board[4][4] = 'W';
}

void NetworkGameScreen::connectToServer() {
    std::cout << "尝试连接到: " << serverAddress << ":" << serverPort << std::endl;
    
    // 解析服务器地址
    auto ipAddress = sf::IpAddress::resolve(serverAddress);
    if (!ipAddress.has_value()) {
        connectionStatus = "无法解析服务器地址";
        std::cout << "无法解析服务器地址: " << serverAddress << std::endl;
        return;
    }

    // 连接到服务器
    if (socket.connect(ipAddress.value(), serverPort) == sf::Socket::Status::Done) {
        connected = true;
        connectionStatus = "已连接到服务器";
        
        // 启动网络线程
        networkRunning = true;
        networkThread = std::thread(&NetworkGameScreen::networkLoop, this);
        
        std::cout << "成功连接到服务器!" << std::endl;
    } else {
        connectionStatus = "连接失败";
        std::cout << "连接服务器失败!" << std::endl;
    }
}

void NetworkGameScreen::networkLoop() {
    sf::Packet packet;
    
    while (networkRunning && connected) {
        sf::Socket::Status status = socket.receive(packet);
        
        if (status == sf::Socket::Status::Done) {
            std::string message;
            packet >> message;
            handleServerMessage(message);
            packet.clear();
        } else if (status == sf::Socket::Status::Disconnected) {
            connected = false;
            connectionStatus = "与服务器断开连接";
            std::cout << "与服务器断开连接!" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    networkRunning = false;
}

void NetworkGameScreen::handleServerMessage(const std::string& message) {
    std::cout << "收到服务器消息: " << message << std::endl;
    
    if (message.substr(0, 7) == "WELCOME") {
        // WELCOME:BLACK 或 WELCOME:WHITE
        playerColor = message.substr(8);
        if (playerColor == "BLACK") {
            isMyTurn = true; // 黑子先手
        }
        std::cout << "分配角色: " << playerColor << std::endl;
    }
    else if (message.substr(0, 5) == "BOARD") {
        // BOARD:棋盘数据
        updateBoardFromServer(message.substr(6));
    }
    else if (message == "YOUR_TURN") {
        isMyTurn = true;
        std::cout << "轮到你了!" << std::endl;
    }
    else if (message == "WAIT_TURN") {
        isMyTurn = false;
        std::cout << "等待对手..." << std::endl;
    }
    else if (message.substr(0, 8) == "GAME_END") {
        handleGameEnd(message);
    }
    else if (message == "INVALID_MOVE") {
        // 处理无效移动
        statusText.setString("无效移动!");
        statusText.setFillColor(sf::Color::Red);
        std::cout << "无效移动!" << std::endl;
    }
    else if (message == "PING") {
        // 响应服务器的ping
        sf::Packet pong;
        pong << "PONG";
        socket.send(pong);
    }
}

void NetworkGameScreen::updateBoardFromServer(const std::string& boardData) {
    if (boardData.length() != 64) {
        std::cout << "收到无效的棋盘数据，长度: " << boardData.length() << std::endl;
        return;
    }
    
    std::cout << "更新棋盘状态" << std::endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            board[y][x] = boardData[y * 8 + x];
        }
    }
}

void NetworkGameScreen::handleGameEnd(const std::string& message) {
    gameOver = true;
    
    std::cout << "游戏结束: " << message << std::endl;
    
    // 解析游戏结束消息: GAME_END:结果:分数1:分数2
    size_t pos1 = message.find(':', 9);
    if (pos1 != std::string::npos) {
        std::string result = message.substr(9, pos1 - 9);
        
        if (result == "BLACK_WINS") {
            gameStatus = "黑子获胜!";
        } else if (result == "WHITE_WINS") {
            gameStatus = "白子获胜!";
        } else if (result == "TIE") {
            gameStatus = "平局!";
        }
        
        // 解析分数
        size_t pos2 = message.find(':', pos1 + 1);
        if (pos2 != std::string::npos) {
            std::string score1 = message.substr(pos1 + 1, pos2 - pos1 - 1);
            std::string score2 = message.substr(pos2 + 1);
            gameStatus += " 分数: " + score1 + " vs " + score2;
        }
    }
    
    isMyTurn = false;
}

void NetworkGameScreen::handleInput(sf::Event event) {
    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

            // 检查断开连接按钮
            if (disconnectButton.wasClicked()) {
                disconnect();
                auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                mainMenu->init();
                startTransitionTo(mainMenu);
                return;
            }

            // 检查棋盘点击
            if (connected && isMyTurn && !gameOver) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                int boardCoordX = static_cast<int>((mousePos.x - boardX) / cellSize);
                int boardCoordY = static_cast<int>((mousePos.y - boardY) / cellSize);

                if (boardCoordX >= 0 && boardCoordX < 8 &&
                    boardCoordY >= 0 && boardCoordY < 8) {
                    sendMove(boardCoordX, boardCoordY);
                    }
            }
        }
    }
}
