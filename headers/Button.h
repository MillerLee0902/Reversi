//
// Created by Miller on 2025/4/9.
// SFML 3.0 compatible rounded corners using simple shapes
//

#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    sf::Sound clickSound;
    bool isPressed;
    bool isHovered;
    float cornerRadius;

public:
    // Constructor with corner radius parameter
    Button(const sf::Vector2f &size, const sf::Vector2f &position,
           const sf::Font &font, const std::string &buttonText,
           const sf::Color &idle, const sf::Color &hover, const sf::Color &active,
           const sf::SoundBuffer &soundBuffer, float radius = 10.0f):
           text(font),
           clickSound(sf::Sound(soundBuffer)),
           cornerRadius(radius) {

        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(idle);

        text.setString(buttonText);
        text.setCharacterSize(18);  // 稍微減小初始字體大小
        text.setFillColor(sf::Color::White);

        // 自動調整文字大小和位置
        adjustTextSize();

        idleColor = idle;
        hoverColor = hover;
        activeColor = active;

        isPressed = false;
        isHovered = false;
    }

    /**
    * Check whether mouse is over the block(button)
    * @param window
    * @return isMouseOver the button
    */
    bool isMouseOver(const sf::RenderWindow &window) const;

    /**
     * update the state of mouseHover and press
     * @param window (obj)
     */
    void update(const sf::RenderWindow &window);

    /**
     * Draw the button with rounded corners and text (SFML 3.0 compatible)
     * @param window (object)
     */
    void draw(sf::RenderWindow &window) const {
        if (cornerRadius > 0) {
            drawRoundedRectangle(window, shape.getPosition(), shape.getSize(),
                               shape.getFillColor(), cornerRadius);
        } else {
            window.draw(shape);
        }
        window.draw(text);
    }

    bool isClicked(const sf::RenderWindow &window) const;

    bool wasClicked() const {
        return isHovered && isPressed;
    }

    /**
     * Set button's position
     * @param position (vector)
     */
    void setPosition(const sf::Vector2f &position);


    /**
     * Setting text
     * @param newText
     */
    void setText(const std::string &newText);

    sf::Vector2f getSize() {
        return shape.getSize();
    }

    sf::Vector2f getPosition() {
        return shape.getPosition();
    }

private:
    /**
     * Draw a rounded rectangle using multiple shapes (SFML 3.0 compatible)
     */
    void drawRoundedRectangle(sf::RenderWindow &window, const sf::Vector2f &position,
                            const sf::Vector2f &size, const sf::Color &color,
                            float radius) const {
        // Limit radius to reasonable size
        float maxRadius = std::min(size.x, size.y) / 2.0f;
        radius = std::min(radius, maxRadius);

        if (radius <= 1.0f) {
            // If radius is too small, just draw a regular rectangle
            sf::RectangleShape rect(size);
            rect.setPosition(position);
            rect.setFillColor(color);
            window.draw(rect);
            return;
        }

        // Main rectangle (center)
        sf::RectangleShape mainRect;
        mainRect.setSize({size.x - 2 * radius, size.y});
        mainRect.setPosition({position.x + radius, position.y});
        mainRect.setFillColor(color);
        window.draw(mainRect);

        // Left rectangle
        sf::RectangleShape leftRect;
        leftRect.setSize({radius, size.y - 2 * radius});
        leftRect.setPosition({position.x, position.y + radius});
        leftRect.setFillColor(color);
        window.draw(leftRect);

        // Right rectangle
        sf::RectangleShape rightRect;
        rightRect.setSize({radius, size.y - 2 * radius});
        rightRect.setPosition({position.x + size.x - radius, position.y + radius});
        rightRect.setFillColor(color);
        window.draw(rightRect);

        // Four corner circles
        sf::CircleShape topLeft(radius);
        topLeft.setPosition({position.x, position.y});
        topLeft.setFillColor(color);
        window.draw(topLeft);

        sf::CircleShape topRight(radius);
        topRight.setPosition({position.x + size.x - 2 * radius, position.y});
        topRight.setFillColor(color);
        window.draw(topRight);

        sf::CircleShape bottomLeft(radius);
        bottomLeft.setPosition({position.x, position.y + size.y - 2 * radius});
        bottomLeft.setFillColor(color);
        window.draw(bottomLeft);

        sf::CircleShape bottomRight(radius);
        bottomRight.setPosition({position.x + size.x - 2 * radius, position.y + size.y - 2 * radius});
        bottomRight.setFillColor(color);
        window.draw(bottomRight);
    }

    // 新增：自動調整文字大小以適應按鈕
    void adjustTextSize() {
        sf::FloatRect buttonBounds = shape.getGlobalBounds();
        sf::FloatRect textBounds = text.getGlobalBounds();

        // 確保文字不會超出按鈕邊界
        float maxWidth = buttonBounds.size.x - 20.0f; // 留10px邊距
        float maxHeight = buttonBounds.size.y - 10.0f; // 留5px邊距

        int currentSize = text.getCharacterSize();

        // 如果文字太寬，減小字體大小
        while (textBounds.size.x > maxWidth && currentSize > 12) {
            currentSize--;
            text.setCharacterSize(currentSize);
            textBounds = text.getGlobalBounds();
        }

        // 如果文字太高，也減小字體大小
        while (textBounds.size.y > maxHeight && currentSize > 12) {
            currentSize--;
            text.setCharacterSize(currentSize);
            textBounds = text.getGlobalBounds();
        }

        // 重新居中文字
        text.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
        text.setPosition({
            shape.getPosition().x + shape.getSize().x / 2.0f,
            shape.getPosition().y + shape.getSize().y / 2.0f
        });
    }

};

#endif //BUTTON_H