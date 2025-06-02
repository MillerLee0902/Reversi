//
// Created by Miller on 2025/4/9.
//

#include "../headers/GameState.h"
#include "../headers/Global.h"
void GameState::update(float deltaTime) {
    if (transitioning) {
        transitionTime += deltaTime;
        float progress = transitionTime / TRANSITION_DURATION;

        if (transitionIn) {
            transitionAlpha = 1.0f - easeInOut(progress);
            if (progress >= 1.0f) {
                transitioning = false;
                transitionAlpha = 0.0f;
            }
        } else {
            transitionAlpha = easeInOut(progress);
            if (progress >= 1.0f) {
                transitioning = false;
                stateChangeCallback(nextState);
            }
        }
    }
}

void GameState::renderTransition() const {
    if (transitioning) {
        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));

        overlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(transitionAlpha * 255)));
        window.draw(overlay);
    }
}

void GameState::startTransitionTo(const std::shared_ptr<GameState> &state) {
    transitioning = true;
    transitionTime = 0.0f;
    transitionIn = false;
    nextState = state;
}

void GameState::startTransitionIn() {
    transitioning = true;
    transitionTime = 0.0f;
    transitionIn = true;
    transitionAlpha = 1.0f;
}