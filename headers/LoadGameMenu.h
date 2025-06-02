//
// Created by Miller on 2025/4/9.
// Modified for better visual appearance with rounded rectangles
//

#ifndef LOADGAMEMENU_H
#define LOADGAMEMENU_H

#include <utility>

#include "../headers/GameState.h"
#include "../headers/Button.h"
#include "../headers/Global.h"
#include "../headers/SaveGame.h"
#include "../headers/MainMenu.h"

// Load Game Menu State
class LoadGameMenu : public GameState {
private:
    sf::Sprite backgroundSprite;
    sf::Text titleText;
    sf::Text infoText;

    std::vector<Button> buttons;          // Control buttons (back)
    std::vector<Button> saveButtons;      // Save file buttons
    std::vector<Button> actionButtons;    // Open and Delete buttons

    std::vector<std::string> savedGames;  // Paths to save files
    SaveGame saveGameManager;

    int selectedSaveIndex;                // Currently selected save

public:
    LoadGameMenu(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : GameState(window, std::move(callback)),
          backgroundSprite(sf::Sprite(resources->getTexture("menu_bg"))),
          titleText(sf::Text(resources->getFont("main"))),
          infoText(sf::Text(resources->getFont("main"))),
          selectedSaveIndex(-1) {
        // Set up backgroundSprite
        sf::Vector2u textureSize = resources->getTexture("menu_bg").getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});

        // Set up title text
        titleText.setString("Load Game");
        titleText.setCharacterSize(48);
        titleText.setFillColor(sf::Color::White);
        titleText.setOutlineThickness(2.0f);
        titleText.setOutlineColor(sf::Color::Black);

        // Center title
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.getCenter());
        titleText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 5.0f});

        // Load saved games
        loadSavedGames();

        // Set up info text
        infoText.setString("No saved games found.");
        infoText.setCharacterSize(24);
        infoText.setFillColor(sf::Color::White);

        // Center info text
        sf::FloatRect infoBounds = infoText.getLocalBounds();
        infoText.setOrigin(infoBounds.getCenter());
        infoText.setPosition({WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f});
    }

    void init() override {
        // Create back button
        buttons.clear();
        buttons.emplace_back(
            sf::Vector2f(100.0f, 40.0f),
            sf::Vector2f(30.0f, WINDOW_HEIGHT - 70.0f),
            resources->getFont("main"),
            "Back",
            sf::Color(180, 180, 180),
            sf::Color(200, 200, 200),
            sf::Color(160, 160, 160),
            resources->getSoundBuffer("click"),
            15.0f
        );

        // Create Open and Delete buttons
        actionButtons.clear();

        // Open button
        actionButtons.emplace_back(
            sf::Vector2f(100.0f, 40.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f - 120.0f, WINDOW_HEIGHT - 70.0f),
            resources->getFont("main"),
            "Open",
            sf::Color(40, 180, 80),
            sf::Color(60, 200, 100),
            sf::Color(30, 160, 60),
            resources->getSoundBuffer("click"),
            15.0f
        );

        // Delete button
        actionButtons.emplace_back(
            sf::Vector2f(100.0f, 40.0f),
            sf::Vector2f(WINDOW_WIDTH / 2.0f + 20.0f, WINDOW_HEIGHT - 70.0f),
            resources->getFont("main"),
            "Delete",
            sf::Color(200, 60, 60),
            sf::Color(220, 80, 80),
            sf::Color(180, 40, 40),
            resources->getSoundBuffer("click"),
            15.0f
        );

        // Create save buttons for each saved game
        createSaveButtons();

        // Reset selection
        selectedSaveIndex = -1;

        // Start transition in
        startTransitionIn();
    }

    void loadSavedGames();

    void createSaveButtons();

    void handleInput(sf::Event event) override;

    void loadGame(const std::string& savePath);

    void deleteGame(const std::string& savePath);

    void update(float deltaTime) override;

    void render() override;



private:
    // Helper function to draw rounded rectangle for save game display
    void drawRoundedBackground(sf::RenderWindow& window, const sf::Vector2f& position,
                             const sf::Vector2f& size, const sf::Color& color, float radius) const {
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
};

#endif //LOADGAMEMENU_H