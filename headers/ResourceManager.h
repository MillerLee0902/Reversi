//
// Created by Miller on 2025/4/9.
//

#ifndef RESOURCEMAMAGER_H
#define RESOURCEMAMAGER_H

#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Resource Manager
class ResourceManager {
private:
    static ResourceManager *instance;

    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Music *> music;

    ResourceManager() {
    }

public:

    // no declare -> declare
    static ResourceManager *getInstance() {
        if (!instance) {
            instance = new ResourceManager();
        }
        return instance;
    }

    // Destructor
    ~ResourceManager() {
        for (auto &pair: music) {
            delete pair.second;
        }
    }

    /**
     * To load the texture and append to textures(dict)
     *
     * @param name
     * @param filename
     * @return
     */
    bool loadTexture(const std::string &name, const std::string &filename);

    /**
     * To load the font and append to fonts(dict)
     * @param name
     * @param filename
     * @return
     */
    bool loadFont(const std::string &name, const std::string &filename);

    /**
     * To load the sound and append to sound-buffer(dict)
     * @param name
     * @param filename
     * @return
     */
    bool loadSoundBuffer(const std::string &name, const std::string &filename);

    /**
     * To load music and append to music
     * @param name
     * @param filename
     * @return
     */
    bool loadMusic(const std::string &name, const std::string &filename);

    sf::Texture &getTexture(const std::string &name) {
        return textures.at(name);
    }

    sf::Font &getFont(const std::string &name) {
        return fonts.at(name);
    }

    sf::SoundBuffer &getSoundBuffer(const std::string &name) {
        return soundBuffers.at(name);
    }

    auto *getMusic(const std::string &name) const {
        return music.at(name);
    }

    /**
     * Play music
     * @param name
     * @param loop
     */
    void playMusic(const std::string &name, bool loop = false);

    /**
     * Stop music
     * @param name
     */
    void stopMusic(const std::string &name);
};


#endif //RESOURCEMAMAGER_H
