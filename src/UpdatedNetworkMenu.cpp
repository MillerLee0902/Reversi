//
// Created by Miller on 2025/5/25.
//

#include "../headers/UpdatedNetworkMenu.h"
#include "../headers/MainMenu.h"

void UpdatedNetworkMenu::handleInput(sf::Event event)  {
    if (!transitioning && !connecting) {
        // 处理文本输入
        for (auto &input : inputs) {
            input.handleInput(event);
        }

        if (event.is<sf::Event::MouseButtonReleased>()) {
            if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
                mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {

                // 返回按钮
                if (backButton.wasClicked()) {
                    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
                    mainMenu->init();
                    startTransitionTo(mainMenu);
                    return;
                }

                // 连接服务器按钮
                if (buttons[0].wasClicked()) {
                    connectToServer(false);
                }

                // 快速连接按钮
                if (buttons[1].wasClicked()) {
                    connectToServer(true);
                }
                }
        }
    }
}
