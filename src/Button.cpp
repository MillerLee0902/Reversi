/*****************************************************************/ /**
* \file   Button.cpp
 * \brief  Deal with the the button that input address of playing chess in the game screen.
 * \version SFML 3.0 compatible
 *
 * \author Miller
 * \date   2025/4/9
 *********************************************************************/

#include "../headers/Button.h"
#include <cmath>

/**
 * Check whether the mouse is over the button.
 * @param window
 * @return Whether the button is over.
 */
bool Button::isMouseOver(const sf::RenderWindow &window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // SFML 3.0 compatible bounds checking
    sf::FloatRect bounds = shape.getGlobalBounds();
    return (mousePosF.x >= bounds.position.x &&
            mousePosF.x <= bounds.position.x + bounds.size.x &&
            mousePosF.y >= bounds.position.y &&
            mousePosF.y <= bounds.position.y + bounds.size.y);
}

void Button::update(const sf::RenderWindow &window) {
    isHovered = isMouseOver(window);

    // If is hover the button, change color. And detect the mouse is press or not
    if (isHovered) {
        shape.setFillColor(hoverColor);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (!isPressed) {
                clickSound.play();
                isPressed = true;
            }
            shape.setFillColor(activeColor);
        } else {
            isPressed = false;
        }
    } else {
        shape.setFillColor(idleColor);
        isPressed = false;
    }
}

void Button::setText(const std::string &newText) {
    text.setString(newText);
    // 重新調整文字大小和位置
    adjustTextSize();
}

void Button::setPosition(const sf::Vector2f &position) {
    shape.setPosition(position);
    // 重新調整文字位置
    adjustTextSize();
}

bool Button::isClicked(const sf::RenderWindow &window) const {
    return isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}