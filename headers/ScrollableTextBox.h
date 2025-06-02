//
// ScrollableTextBox.h - 可滾動內容的文字方框
//

#ifndef SCROLLABLETEXTBOX_H
#define SCROLLABLETEXTBOX_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class ScrollableTextBox {
private:
    sf::RectangleShape background;
    sf::RectangleShape scrollBar;
    sf::RectangleShape scrollThumb;
    std::vector<sf::Text> textLines;
    sf::Font* font;

    sf::Vector2f position;
    sf::Vector2f size;

    float scrollOffset;
    float maxScrollOffset;
    float lineHeight;
    int visibleLines;

    bool isScrolling;
    bool showScrollBar;
    sf::Vector2f lastMousePos;

    sf::Color backgroundColor;
    sf::Color scrollBarColor;
    sf::Color scrollThumbColor;
    sf::Color textColor;

    float cornerRadius;

    void updateScrollBar();
    void updateVisibleLines();
    void drawRoundedRectangle(sf::RenderWindow& window, const sf::Vector2f& pos,
                             const sf::Vector2f& rectSize, const sf::Color& color,
                             float radius) const;

public:
    ScrollableTextBox(const sf::Vector2f& pos, const sf::Vector2f& boxSize,
                     sf::Font& textFont, float radius = 15.0f);

    void setText(const std::string& text, int characterSize = 16);
    void setBackgroundColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);
    void setPosition(const sf::Vector2f& pos);

    void handleInput(const sf::Event& event, const sf::RenderWindow& window);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    int calculateOptimalFontSize(const std::string &text, int maxSize) const;

    sf::FloatRect getTextArea() const;

    sf::Vector2f getTextAreaPosition() const;

    float getTextAreaWidth() const;

    float getTextAreaHeight() const;

    bool isMouseOver(const sf::RenderWindow& window) const;
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
};

#endif //SCROLLABLETEXTBOX_H