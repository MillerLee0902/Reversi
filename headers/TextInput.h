//
// Created by Miller on 2025/4/9.
// SFML 3.0 compatible TextInput
//

#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <SFML/Graphics.hpp>

// TextInput class - SFML 3.0 compatible
class TextInput {
private:
    sf::RectangleShape background;
    sf::Text text;
    sf::Text placeholder;
    sf::Text cursor;
    std::string inputString;
    bool isActive;
    bool showCursor;
    float cursorBlinkTime;
    const float blinkInterval = 0.5f;

public:
    TextInput(const sf::Vector2f &size, const sf::Vector2f &position,
              const sf::Font &font, const std::string &placeholderText)
              : text(font), placeholder(font), cursor(font) {

        background.setSize(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(240, 240, 240));
        background.setOutlineThickness(2.0f);
        background.setOutlineColor(sf::Color(200, 200, 200));

        text.setString("");
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
        text.setPosition({position.x + 10.0f, position.y + size.y / 2.0f - 9.0f});

        placeholder.setString(placeholderText);
        placeholder.setCharacterSize(18);
        placeholder.setFillColor(sf::Color(150, 150, 150));
        placeholder.setPosition({position.x + 10.0f, position.y + size.y / 2.0f - 9.0f});

        cursor.setString("|");
        cursor.setCharacterSize(18);
        cursor.setFillColor(sf::Color::Black);
        cursor.setPosition({position.x + 10.0f, position.y + size.y / 2.0f - 9.0f});

        inputString = "";
        isActive = false;
        showCursor = false;
        cursorBlinkTime = 0.0f;
    }

    void update(const sf::RenderWindow &window, float deltaTime) {
        // Check if clicked
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

            // SFML 3.0 compatible bounds checking
            sf::FloatRect bounds = background.getGlobalBounds();
            isActive = (mousePosF.x >= bounds.position.x &&
                       mousePosF.x <= bounds.position.x + bounds.size.x &&
                       mousePosF.y >= bounds.position.y &&
                       mousePosF.y <= bounds.position.y + bounds.size.y);

            if (isActive) {
                background.setOutlineColor(sf::Color(100, 100, 255));
            }
        }

        // Update cursor blink
        if (isActive) {
            cursorBlinkTime += deltaTime;
            if (cursorBlinkTime >= blinkInterval) {
                cursorBlinkTime = 0.0f;
                showCursor = !showCursor;
            }

            // Position cursor after text - SFML 3.0 compatible
            sf::FloatRect textBounds = text.getGlobalBounds();
            cursor.setPosition({text.getPosition().x + textBounds.size.x, text.getPosition().y});
        } else {
            background.setOutlineColor(sf::Color(200, 200, 200));
            showCursor = false;
        }
    }

    void handleInput(const sf::Event& event) {
        if (!isActive) return;

        // SFML 3.0 compatible event handling
        if (event.is<sf::Event::TextEntered>()) {
            const auto& textEvent = event.getIf<sf::Event::TextEntered>();
            if (textEvent) {
                if (textEvent->unicode == 8) { // Backspace
                    if (!inputString.empty()) {
                        inputString.pop_back();
                    }
                } else if (textEvent->unicode == 13) { // Enter
                    isActive = false;
                } else if (textEvent->unicode >= 32 && textEvent->unicode < 128) {
                    inputString += static_cast<char>(textEvent->unicode);
                }
                text.setString(inputString);
            }
        }
    }

    void draw(sf::RenderWindow &window) const {
        window.draw(background);

        if (inputString.empty()) {
            window.draw(placeholder);
        } else {
            window.draw(text);
        }

        if (isActive && showCursor) {
            window.draw(cursor);
        }
    }

    std::string getValue() const {
        return inputString;
    }

    void setValue(const std::string &value) {
        inputString = value;
        text.setString(inputString);
    }

    bool isActiveInput() const {
        return isActive;
    }
};

#endif //TEXTINPUT_H