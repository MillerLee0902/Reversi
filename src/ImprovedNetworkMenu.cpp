//
// ImprovedNetworkMenu.cpp - 使用可拖曳文字方框的版本
//

#include "../headers/ImprovedNetworkMenu.h"
#include "../headers/MainMenu.h"

void ImprovedNetworkMenu::handleInput(sf::Event event) {
    if (!transitioning) {
        textBox.handleInput(event, window);

        for (auto &input: inputs) {
            input.handleInput(event);
        }

        if (event.is<sf::Event::MouseButtonReleased>()) {
            if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
                mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                if (backButton.wasClicked()) {
                    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                    mainMenu->init();
                    startTransitionTo(mainMenu);
                    return;
                }

                if (buttons[0].wasClicked()) {
                    connectToServer(false);
                }

                if (buttons[1].wasClicked()) {
                    connectToServer(true);
                }
            }
        }
    }
}
