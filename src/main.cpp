// ReversiGame.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <functional>
#include <cmath>
#include <map>

#include "../headers/Global.h"
#include "../headers/ResourceManager.h"
#include "../headers/ReversiGame.h"


ResourceManager *ResourceManager::instance = nullptr;

// Entry point
int main() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create and run the game
    ReversiGame game;
    game.run();

    return 0;
}
