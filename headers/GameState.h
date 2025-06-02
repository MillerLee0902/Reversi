//
// Created by Miller on 2025/4/9.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "../headers/ResourceManager.h"
#include "../headers/Global.h"

#include <utility>

// Base State class
class GameState {
protected:
    std::function<void(std::shared_ptr<GameState>)> stateChangeCallback;
    sf::RenderWindow &window;
    ResourceManager *resources;

    bool transitioning;
    float transitionAlpha;
    float transitionTime;
    bool transitionIn;
    std::shared_ptr<GameState> nextState;

public:
    GameState(sf::RenderWindow &window, std::function<void(std::shared_ptr<GameState>)> callback)
        : stateChangeCallback(std::move(callback)), window(window), resources(ResourceManager::getInstance()) {
        transitioning = false;
        transitionAlpha = 0.0f;
        transitionTime = 0.0f;
        transitionIn = true;
    }

    virtual ~GameState() = default;

    virtual void init() = 0;

    virtual void handleInput(sf::Event event) = 0;

    virtual void update(float deltaTime);

    void renderTransition() const;

    virtual void render() = 0;

    void startTransitionTo(const std::shared_ptr<GameState> &state);

    void startTransitionIn();


};




#endif //GAMESTATE_H
