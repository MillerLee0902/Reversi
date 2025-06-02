//
// Created by Miller on 2025/4/10.
//

#include "../headers/Timer.h"
#include <string>

void Timer::update(float deltaTime) {
    if (isActive && playerTurn &&remainingChances > 0) {
        timeRemaining -= deltaTime;

        if (timeRemaining <= 0) {
            timeRemaining = 30.0f; // Reset timer
            remainingChances--;    // Reduce chance
        }

        // Update text contents during update
        timerText.setString(std::to_string(static_cast<int>(timeRemaining)) + "s");
        chancesText.setString("Chances: " + std::to_string(remainingChances));
    }
}

void Timer::reset() {
    timeRemaining = 30.0f;
    timerText.setString("30s");
}

void Timer::switchPlayer() {
    playerTurn = !playerTurn;
    reset();
}

void Timer::setPlayerTurn(bool isWhiteTurn) {

    if (!playerTurn && isWhiteTurn) {
        reset();
    }
    else if (playerTurn && !isWhiteTurn) {
        reset();
    }

    playerTurn = isWhiteTurn;
}

void Timer::setPosition(const sf::Vector2f &timerPos, const sf::Vector2f &chancesPos) {
    timerText.setPosition(timerPos);
    chancesText.setPosition(chancesPos);
}

void Timer::draw(sf::RenderWindow &window) const {
    window.draw(timerText);
    window.draw(chancesText);
}

int Timer::getRemainingChances() const {
    return remainingChances;
}

bool Timer::isPlayerTurn() const {
    return playerTurn;
}

float Timer::getRemainingTime() const {
    return timeRemaining;
}

void Timer::setRemainingTime(const float time) {
    timeRemaining = time;
    timerText.setString(std::to_string(static_cast<int>(timeRemaining)) + "s");
}

void Timer::setRemainingChances(const int chances) {
    remainingChances = chances;
    chancesText.setString("Chances: " + std::to_string(remainingChances));
}