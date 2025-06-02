//
// Created by Miller on 2025/4/10.
//

#ifndef TIMER_H
#define TIMER_H

#include <SFML/Graphics.hpp>

class Timer {
private:
    float timeRemaining;
    int remainingChances;
    bool isActive;
    sf::Text timerText;
    sf::Text chancesText;
    bool playerTurn; // false for black, true for white

public:
    Timer(const sf::Font &font) :
    timeRemaining(30.0f),
    remainingChances(3),
    isActive(true),
    timerText(font),
    chancesText(font),
    playerTurn(false) {
        // Initialize timer text
        timerText.setFont(font);
        timerText.setCharacterSize(24);
        timerText.setFillColor(sf::Color::Black);
        timerText.setString("30s");

        // Initialize chances text
        chancesText.setFont(font);
        chancesText.setCharacterSize(20);
        chancesText.setFillColor(sf::Color::Black);
        chancesText.setString("Chances: 3");

        // Default position
        timerText.setPosition(sf::Vector2f(0, 0));
        chancesText.setPosition(sf::Vector2f(0, 30));
    }

    void update(float deltaTime);

    void reset();

    void switchPlayer();

    void setPlayerTurn(bool isWhiteTurn);

    void setPosition(const sf::Vector2f &timerPos, const sf::Vector2f &chancesPos);

    void draw(sf::RenderWindow &window) const;

    int getRemainingChances() const;

    bool isPlayerTurn() const;

    float getRemainingTime() const;

    void setRemainingChances(int);

    void setRemainingTime(float);
};

#endif // TIMER_H
