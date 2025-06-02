//
// Created by Miller on 2025/5/25.
//

#include "../headers/NetworkGameClient.h"
#include "../headers/MainMenu.h"

void NetworkGameClient::handleInput(const sf::Event event) {
    if (event.is<sf::Event::MouseButtonReleased>() && !transitioning) {
        const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

            if (disconnectButton.wasClicked()) {
                // 安全地斷開連接並返回主選單
                safeDisconnectAndReturn();
                return;
            }

            if (connected && isMyTurn && !gameOver) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int boardCoordX = static_cast<int>((mousePos.x - boardX) / cellSize);
                int boardCoordY = static_cast<int>((mousePos.y - boardY) / cellSize);

                if (boardCoordX >= 0 && boardCoordX < 8 &&
                    boardCoordY >= 0 && boardCoordY < 8) {
                    if (gameLogic.board[boardCoordY][boardCoordX] == 'a') {
                        sendMove(boardCoordX, boardCoordY);
                    }
                    }
            }
        }
    }
}

void NetworkGameClient::safeDisconnectAndReturn() {
    // 先設置標誌位，停止網路活動
    networkRunning = false;
    pingRunning = false;
    connected = false;

    // 發送斷開連接訊息（如果還連著的話）
    try {
        sf::Packet packet;
        packet << "DISCONNECT";
        socket.send(packet); // 嘗試發送但不等待回應
    } catch (...) {
        // 忽略任何錯誤，因為我們正在斷開連接
    }

    // 立即關閉socket
    socket.disconnect();

    // 等待線程安全結束，但設置超時
    if (networkThread.joinable()) {
        std::thread killer([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // 如果線程還沒結束，強制結束
        });
        killer.detach();

        networkThread.join();
    }

    if (pingThread.joinable()) {
        pingThread.join();
    }

    // 創建並返回主選單
    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
    mainMenu->init();
    startTransitionTo(mainMenu);
}
