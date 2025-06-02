//
// Created by Miller on 2025/4/9.
//

#include "../headers/ResourceManager.h"
#include <iostream>

bool ResourceManager::loadTexture(const std::string &name, const std::string &filename) {
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return false;
    }
    textures[name] = texture;
    return true;
}

bool ResourceManager::loadFont(const std::string &name, const std::string &filename) {
    sf::Font font;
    if (!font.openFromFile(filename)) {
        std::cerr << "Failed to load font: " << filename << std::endl;
        return false;
    }
    fonts[name] = font;
    return true;
}

bool ResourceManager::loadSoundBuffer(const std::string &name, const std::string &filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return false;
    }
    soundBuffers[name] = buffer;
    return true;
}

bool ResourceManager::loadMusic(const std::string &name, const std::string &filename) {
    auto *music = new sf::Music();
    if (!music->openFromFile(filename)) {
        std::cerr << "Failed to load music: " << filename << std::endl;
        delete music;
        return false;
    }
    this->music[name] = music;
    return true;
}

void ResourceManager::playMusic(const std::string &name, bool loop) {
    music.at(name)->setLooping(loop);
    music.at(name)->play();
}

void ResourceManager::stopMusic(const std::string &name) {
    music.at(name)->stop();
}