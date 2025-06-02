//
// ScrollableTextBox.cpp - 可滾動內容的文字方框實現
//

#include "../headers/ScrollableTextBox.h"
#include <sstream>
#include <algorithm>
#include <cmath>

ScrollableTextBox::ScrollableTextBox(const sf::Vector2f &pos, const sf::Vector2f &boxSize,
                                     sf::Font &textFont, float radius)
    : position(pos), size(boxSize), font(&textFont), cornerRadius(radius),
      scrollOffset(0.0f), maxScrollOffset(0.0f), lineHeight(20.0f),
      isScrolling(false), showScrollBar(false) {
    background.setSize(size);
    background.setPosition(position);
    backgroundColor = sf::Color(255, 255, 255, 128);
    background.setFillColor(backgroundColor);

    scrollBarColor = sf::Color(200, 200, 200, 180);
    scrollThumbColor = sf::Color(120, 120, 120, 200);
    textColor = sf::Color::Black;

    // 滾動條設置
    scrollBar.setSize({12.0f, size.y - 10.0f});
    scrollBar.setPosition({position.x + size.x - 17.0f, position.y + 5.0f});
    scrollBar.setFillColor(scrollBarColor);

    scrollThumb.setSize({8.0f, 50.0f});
    scrollThumb.setFillColor(scrollThumbColor);

    updateVisibleLines();
}

void ScrollableTextBox::setText(const std::string &text, int characterSize) {
    textLines.clear();

    // 自動計算最適合的字體大小
    int optimalSize = calculateOptimalFontSize(text, characterSize);
    lineHeight = static_cast<float>(optimalSize) * 1.2f;

    std::istringstream iss(text);
    std::string line;

    while (std::getline(iss, line)) {
        if (line.empty()) {
            // 空行
            sf::Text emptyLine(*font);
            emptyLine.setString(" ");
            emptyLine.setCharacterSize(optimalSize);
            emptyLine.setFillColor(textColor);
            textLines.push_back(emptyLine);
        } else {
            sf::Text textLine(*font);
            textLine.setString(line);
            textLine.setCharacterSize(optimalSize);
            textLine.setFillColor(textColor);

            // 檢查是否需要自動換行
            sf::FloatRect textBounds = textLine.getLocalBounds();
            float maxWidth = size.x - 30.0f; // 留出滾動條空間和邊距

            if (textBounds.size.x > maxWidth) {
                // 需要分行
                std::vector<std::string> words;
                std::istringstream wordStream(line);
                std::string word;
                while (wordStream >> word) {
                    words.push_back(word);
                }

                std::string currentLine;
                for (const auto &w: words) {
                    std::string testLine = currentLine.empty() ? w : currentLine + " " + w;
                    textLine.setString(testLine);
                    sf::FloatRect testBounds = textLine.getLocalBounds();

                    if (testBounds.size.x > maxWidth && !currentLine.empty()) {
                        // 當前行已滿，保存並開始新行
                        sf::Text wrappedLine(*font);
                        wrappedLine.setString(currentLine);
                        wrappedLine.setCharacterSize(optimalSize);
                        wrappedLine.setFillColor(textColor);
                        textLines.push_back(wrappedLine);

                        currentLine = w;
                    } else {
                        currentLine = testLine;
                    }
                }

                // 保存最後一行
                if (!currentLine.empty()) {
                    textLine.setString(currentLine);
                    textLines.push_back(textLine);
                }
            } else {
                textLines.push_back(textLine);
            }
        }
    }

    updateVisibleLines();
    updateScrollBar();

    // 重置滾動位置
    scrollOffset = 0.0f;
}

void ScrollableTextBox::updateVisibleLines() {
    visibleLines = static_cast<int>((size.y - 20.0f) / lineHeight);
    maxScrollOffset = std::max(0.0f, static_cast<float>(textLines.size()) * lineHeight - (size.y - 20.0f));
    showScrollBar = maxScrollOffset > 0.0f;
}

void ScrollableTextBox::updateScrollBar() {
    if (!showScrollBar) return;

    float totalContentHeight = static_cast<float>(textLines.size()) * lineHeight;
    float visibleHeight = size.y - 20.0f;

    // 計算滾動條拇指的高度和位置
    float thumbHeight = std::max(20.0f, (visibleHeight / totalContentHeight) * (size.y - 10.0f));
    float scrollBarHeight = size.y - 10.0f;
    float thumbPosition = position.y + 5.0f + (scrollOffset / maxScrollOffset) * (scrollBarHeight - thumbHeight);

    scrollThumb.setSize({8.0f, thumbHeight});
    scrollThumb.setPosition({position.x + size.x - 13.0f, thumbPosition});
}

void ScrollableTextBox::setBackgroundColor(const sf::Color &color) {
    backgroundColor = color;
    background.setFillColor(backgroundColor);
}

void ScrollableTextBox::setTextColor(const sf::Color &color) {
    textColor = color;
    for (auto &textLine: textLines) {
        textLine.setFillColor(textColor);
    }
}

void ScrollableTextBox::setPosition(const sf::Vector2f &pos) {
    position = pos;
    background.setPosition(position);
    scrollBar.setPosition({position.x + size.x - 17.0f, position.y + 5.0f});
    updateScrollBar();
}

bool ScrollableTextBox::isMouseOver(const sf::RenderWindow &window) const {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    return (mousePosF.x >= position.x && mousePosF.x <= position.x + size.x &&
            mousePosF.y >= position.y && mousePosF.y <= position.y + size.y);
}

void ScrollableTextBox::handleInput(const sf::Event &event, const sf::RenderWindow &window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (event.is<sf::Event::MouseButtonPressed>()) {
        const auto &mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            if (isMouseOver(window)) {
                isScrolling = true;
                lastMousePos = mousePosF;
            }
        }
    }

    if (event.is<sf::Event::MouseButtonReleased>()) {
        const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            isScrolling = false;
        }
    }

    if (event.is<sf::Event::MouseMoved>()) {
        if (isScrolling && showScrollBar) {
            // 計算滑鼠移動距離
            float deltaY = mousePosF.y - lastMousePos.y;

            // 將滑鼠移動轉換為滾動偏移
            scrollOffset -= deltaY * 2.0f; // 乘以2來增加滾動靈敏度

            // 限制滾動範圍
            scrollOffset = std::clamp(scrollOffset, 0.0f, maxScrollOffset);

            updateScrollBar();
            lastMousePos = mousePosF;
        } else {
            lastMousePos = mousePosF;
        }
    }

    // 滑鼠滾輪支援
    if (event.is<sf::Event::MouseWheelScrolled>()) {
        const auto &wheelEvent = event.getIf<sf::Event::MouseWheelScrolled>();
        if (wheelEvent && isMouseOver(window) && showScrollBar) {
            // 滾輪向上為正值，向下為負值
            scrollOffset -= wheelEvent->delta * lineHeight * 2.0f;
            scrollOffset = std::clamp(scrollOffset, 0.0f, maxScrollOffset);
            updateScrollBar();
        }
    }
}

void ScrollableTextBox::update(float deltaTime) {
    // 更新文字行的位置
    for (size_t i = 0; i < textLines.size(); ++i) {
        float baseY = position.y + 10.0f + static_cast<float>(i) * lineHeight - scrollOffset;
        textLines[i].setPosition({position.x + 15.0f, baseY});
    }
}

void ScrollableTextBox::drawRoundedRectangle(sf::RenderWindow &window, const sf::Vector2f &pos,
                                             const sf::Vector2f &rectSize, const sf::Color &color,
                                             float radius) const {
    if (radius <= 1.0f) {
        sf::RectangleShape rect(rectSize);
        rect.setPosition(pos);
        rect.setFillColor(color);
        window.draw(rect);
        return;
    }

    float maxRadius = std::min(rectSize.x, rectSize.y) / 2.0f;
    radius = std::min(radius, maxRadius);

    // 主矩形
    sf::RectangleShape mainRect;
    mainRect.setSize({rectSize.x - 2 * radius, rectSize.y});
    mainRect.setPosition({pos.x + radius, pos.y});
    mainRect.setFillColor(color);
    window.draw(mainRect);

    // 左矩形
    sf::RectangleShape leftRect;
    leftRect.setSize({radius, rectSize.y - 2 * radius});
    leftRect.setPosition({pos.x, pos.y + radius});
    leftRect.setFillColor(color);
    window.draw(leftRect);

    // 右矩形
    sf::RectangleShape rightRect;
    rightRect.setSize({radius, rectSize.y - 2 * radius});
    rightRect.setPosition({pos.x + rectSize.x - radius, pos.y + radius});
    rightRect.setFillColor(color);
    window.draw(rightRect);

    // 四個圓角
    sf::CircleShape corners[4];
    for (int i = 0; i < 4; i++) {
        corners[i].setRadius(radius);
        corners[i].setFillColor(color);
    }

    corners[0].setPosition({pos.x, pos.y}); // 左上
    corners[1].setPosition({pos.x + rectSize.x - 2 * radius, pos.y}); // 右上
    corners[2].setPosition({pos.x, pos.y + rectSize.y - 2 * radius}); // 左下
    corners[3].setPosition({pos.x + rectSize.x - 2 * radius, pos.y + rectSize.y - 2 * radius}); // 右下

    for (const auto &corner: corners) {
        window.draw(corner);
    }
}

void ScrollableTextBox::draw(sf::RenderWindow &window) {
    // 繪製圓角背景
    drawRoundedRectangle(window, position, size, backgroundColor, cornerRadius);

    // 直接繪製文字，不使用視窗裁剪
    for (const auto &textLine: textLines) {
        sf::Vector2f linePos = textLine.getPosition();
        // 只繪製在可見區域內的文字行
        if (linePos.y + lineHeight >= position.y + 10.0f &&
            linePos.y <= position.y + size.y - 10.0f) {
            window.draw(textLine);
        }
    }

    // 繪製滾動條
    if (showScrollBar) {
        drawRoundedRectangle(window, scrollBar.getPosition(), scrollBar.getSize(), scrollBarColor, 6.0f);
        drawRoundedRectangle(window, scrollThumb.getPosition(), scrollThumb.getSize(), scrollThumbColor, 4.0f);
    }
}

int ScrollableTextBox::calculateOptimalFontSize(const std::string &text, int maxSize) const {
    // 從最大字體大小開始，逐漸減小直到找到合適的大小
    int currentSize = std::min(maxSize, 28); // 限制最大字體大小為28
    int minSize = 14; // 最小字體大小

    float maxWidth = size.x - 30.0f; // 可用寬度

    // 找出文字中最長的單詞，確保單詞能放入一行
    std::istringstream iss(text);
    std::string line;
    std::string longestWord;

    while (std::getline(iss, line)) {
        std::istringstream wordStream(line);
        std::string word;
        while (wordStream >> word) {
            if (word.length() > longestWord.length()) {
                longestWord = word;
            }
        }
    }

    // 如果有最長單詞，用它來測試
    if (!longestWord.empty()) {
        while (currentSize >= minSize) {
            sf::Text testText(*font);
            testText.setString(longestWord);
            testText.setCharacterSize(currentSize);

            sf::FloatRect bounds = testText.getLocalBounds();
            if (bounds.size.x <= maxWidth) {
                break; // 找到合適的大小
            }
            currentSize--;
        }
    }

    // 進一步測試常見長度的行
    std::string testString = "This is a typical line of text that should fit comfortably within the text box width.";
    while (currentSize >= minSize) {
        sf::Text testText(*font);
        testText.setString(testString);
        testText.setCharacterSize(currentSize);

        sf::FloatRect bounds = testText.getLocalBounds();
        if (bounds.size.x <= maxWidth * 1.3f) {
            // 允許稍微超出，因為會自動換行
            break;
        }
        currentSize--;
    }

    return std::max(currentSize, minSize);
}