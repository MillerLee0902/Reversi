//
// Created by Miller on 2025/4/9.
//

#ifndef REVERSIGAME_H
#define REVERSIGAME_H

#include <SFML/Graphics.hpp>
#include <filesystem>
#include "../headers/ResourceManager.h"
#include "../headers/GameState.h"
#include "../headers/Global.h"

class SplashScreen;

// Main application class
class ReversiGame {
private:
    sf::RenderWindow window;
    std::shared_ptr<GameState> currentState;
    sf::Clock clock;

    bool vsAI;

    // Resource paths
    const std::string FONT_PATH = "./fonts/";
    const std::string TEXTURE_PATH = "./textures/";
    const std::string SOUND_PATH = "./sounds/";
    const std::string MUSIC_PATH = "./music/";

public:
    /**
     * Constructor.
     */
    ReversiGame() : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), GAME_TITLE) {
        // Set 
        window.setFramerateLimit(60);

        // Set window icon
        if (sf::Image icon; icon.loadFromFile(TEXTURE_PATH + "icon.png")) {
            window.setIcon({icon.getSize().x, icon.getSize().y}, icon.getPixelsPtr());
        }

        // Initialize resource manager
        initResources();

        // Set initial state
        setInitialState();
    }

    void initResources() const;

    void setInitialState();

    void run();

    void setVsAI(bool ai) {vsAI = ai; }
    bool isVsAI() const {return vsAI; }

};

#endif //REVERSIGAME_H
