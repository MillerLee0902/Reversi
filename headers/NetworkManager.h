//
// Network Manager for online multiplayer - SFML 3.0 Compatible
// Created for enhanced Reversi game
//

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <cstdint>

enum class NetworkRole {
    HOST,
    CLIENT,
    NONE
};

enum class MessageType {
    MOVE,
    CHAT,
    GAME_STATE,
    PLAYER_INFO,
    DISCONNECT
};

struct NetworkMessage {
    MessageType type;
    std::string data;
    std::int32_t x, y;  // SFML 3.0: Use std::int32_t instead of sf::Int32
    bool isWhiteTurn;
};

class NetworkManager {
private:
    sf::TcpListener listener;
    sf::TcpSocket socket;
    NetworkRole role;
    std::thread networkThread;
    std::atomic<bool> running;
    std::atomic<bool> connected;

    std::queue<NetworkMessage> incomingMessages;
    std::queue<NetworkMessage> outgoingMessages;
    std::mutex messageMutex;

    std::string playerName;
    std::string opponentName;

    std::function<void(const NetworkMessage&)> messageCallback;

public:
    NetworkManager() : role(NetworkRole::NONE), running(false), connected(false) {}

    ~NetworkManager() {
        disconnect();
    }

    // Host a game (server)
    bool hostGame(unsigned short port, const std::string& hostName) {
        if (role != NetworkRole::NONE) return false;

        playerName = hostName;
        role = NetworkRole::HOST;

        if (listener.listen(port) != sf::Socket::Status::Done) {
            role = NetworkRole::NONE;
            return false;
        }

        running = true;
        networkThread = std::thread(&NetworkManager::hostLoop, this);
        return true;
    }

    // Join a game (client)
    bool joinGame(const std::string& serverIP, unsigned short port, const std::string& clientName) {
        if (role != NetworkRole::NONE) return false;

        playerName = clientName;
        role = NetworkRole::CLIENT;

        // SFML 3.0: Create IpAddress from string using resolve
        auto ipAddress = sf::IpAddress::resolve(serverIP);
        if (!ipAddress.has_value()) {
            role = NetworkRole::NONE;
            return false;
        }

        if (socket.connect(ipAddress.value(), port) != sf::Socket::Status::Done) {
            role = NetworkRole::NONE;
            return false;
        }

        connected = true;
        running = true;

        // Send player info
        NetworkMessage msg;
        msg.type = MessageType::PLAYER_INFO;
        msg.data = playerName;
        sendMessage(msg);

        networkThread = std::thread(&NetworkManager::clientLoop, this);
        return true;
    }

    // Send a move
    void sendMove(int x, int y, bool isWhiteTurn) {
        if (!connected) return;

        NetworkMessage msg;
        msg.type = MessageType::MOVE;
        msg.x = static_cast<std::int32_t>(x);
        msg.y = static_cast<std::int32_t>(y);
        msg.isWhiteTurn = isWhiteTurn;

        std::lock_guard<std::mutex> lock(messageMutex);
        outgoingMessages.push(msg);
    }

    // Send chat message
    void sendChat(const std::string& message) {
        if (!connected) return;

        NetworkMessage msg;
        msg.type = MessageType::CHAT;
        msg.data = message;

        std::lock_guard<std::mutex> lock(messageMutex);
        outgoingMessages.push(msg);
    }

    // Get next message
    bool getNextMessage(NetworkMessage& msg) {
        std::lock_guard<std::mutex> lock(messageMutex);
        if (incomingMessages.empty()) return false;

        msg = incomingMessages.front();
        incomingMessages.pop();
        return true;
    }

    // Set message callback
    void setMessageCallback(std::function<void(const NetworkMessage&)> callback) {
        messageCallback = callback;
    }

    // Disconnect
    void disconnect() {
        running = false;
        connected = false;

        if (networkThread.joinable()) {
            networkThread.join();
        }

        socket.disconnect();
        listener.close();
        role = NetworkRole::NONE;
    }

    // Status checks
    bool isConnected() const { return connected; }
    bool isHost() const { return role == NetworkRole::HOST; }
    bool isClient() const { return role == NetworkRole::CLIENT; }

    std::string getPlayerName() const { return playerName; }
    std::string getOpponentName() const { return opponentName; }

private:
    void hostLoop() {
        // Wait for client connection
        if (listener.accept(socket) == sf::Socket::Status::Done) {
            connected = true;

            // Main network loop
            sf::Packet packet;
            while (running && connected) {
                // Check for incoming messages
                sf::Socket::Status receiveStatus = socket.receive(packet);
                if (receiveStatus == sf::Socket::Status::Done) {
                    NetworkMessage msg;
                    if (deserializeMessage(packet, msg)) {
                        if (msg.type == MessageType::PLAYER_INFO) {
                            opponentName = msg.data;
                        }

                        std::lock_guard<std::mutex> lock(messageMutex);
                        incomingMessages.push(msg);

                        if (messageCallback) {
                            messageCallback(msg);
                        }
                    }
                    packet.clear();
                } else if (receiveStatus == sf::Socket::Status::Disconnected) {
                    connected = false;
                    break;
                }

                // Send outgoing messages
                sendPendingMessages();

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        connected = false;
    }

    void clientLoop() {
        sf::Packet packet;
        while (running && connected) {
            // Check for incoming messages
            sf::Socket::Status receiveStatus = socket.receive(packet);
            if (receiveStatus == sf::Socket::Status::Done) {
                NetworkMessage msg;
                if (deserializeMessage(packet, msg)) {
                    if (msg.type == MessageType::PLAYER_INFO) {
                        opponentName = msg.data;
                    }

                    std::lock_guard<std::mutex> lock(messageMutex);
                    incomingMessages.push(msg);

                    if (messageCallback) {
                        messageCallback(msg);
                    }
                }
                packet.clear();
            } else if (receiveStatus == sf::Socket::Status::Disconnected) {
                connected = false;
                break;
            }

            // Send outgoing messages
            sendPendingMessages();

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        connected = false;
    }

    void sendPendingMessages() {
        std::lock_guard<std::mutex> lock(messageMutex);
        while (!outgoingMessages.empty()) {
            NetworkMessage msg = outgoingMessages.front();
            outgoingMessages.pop();

            sf::Packet packet;
            serializeMessage(msg, packet);

            // 修復: 檢查發送狀態
            sf::Socket::Status status = socket.send(packet);
            (void)status; // 避免unused variable警告
        }
    }

    void sendMessage(const NetworkMessage& msg) {
        sf::Packet packet;
        serializeMessage(msg, packet);

        // 修復: 檢查發送狀態
        sf::Socket::Status status = socket.send(packet);
        (void)status; // 避免unused variable警告
    }

    void serializeMessage(const NetworkMessage& msg, sf::Packet& packet) {
        packet << static_cast<std::int32_t>(msg.type);
        packet << msg.data;
        packet << msg.x;
        packet << msg.y;
        packet << msg.isWhiteTurn;
    }

    bool deserializeMessage(sf::Packet& packet, NetworkMessage& msg) {
        std::int32_t type;
        if (!(packet >> type >> msg.data >> msg.x >> msg.y >> msg.isWhiteTurn)) {
            return false;
        }

        msg.type = static_cast<MessageType>(type);
        return true;
    }
};

#endif //NETWORKMANAGER_H