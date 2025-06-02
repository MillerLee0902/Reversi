//
// Created by Miller on 2025/5/25.
// SimpleNetworkMenu.cpp - 實現文件
//

#include "../headers/SimpleNetworkMenu.h"
#include "../headers/MainMenu.h"  // 在.cpp文件中包含，避免循環依賴

void SimpleNetworkMenu::goToMainMenu() {
    auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
    mainMenu->init();
    startTransitionTo(mainMenu);
}