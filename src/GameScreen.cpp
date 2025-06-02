//
// Created by Miller on 2025/4/9.
//

#include "../headers/GameScreen.h"
#include "../headers/MainMenu.h"
#include "../headers/VictoryScreen.h"

void GameScreen::init() {
    // Initialize game logic
    gameLogic.initialize();

    // 设置AI难度
    if (vsComputer) {
        gameLogic.setAIDifficulty(aiDifficulty);
    }

    // Scale background to fit window
    sf::Vector2u textureSize = resources->getTexture("game_bg").getSize();
    float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // Set up title text
    titleText.setString("REVERSI");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineThickness(2.0f);
    titleText.setOutlineColor(sf::Color::Black);

    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.getCenter());
    titleText.setPosition({WINDOW_WIDTH / 2.0f, 40.0f});

    // Set up player text
    player1Text.setString(player1Name + " (Black)");
    player1Text.setCharacterSize(20);
    player1Text.setFillColor(sf::Color::Black);
    player1Text.setPosition({50.0f, 100.0f});

    player2Text.setString(player2Name + " (White)");
    player2Text.setCharacterSize(20);
    player2Text.setFillColor(sf::Color::Black);
    player2Text.setPosition({WINDOW_WIDTH - 250.0f, 100.0f});

    // Set up timer positions
    player1Timer.setPosition({50.0f, 130.0f}, {50.0f, 160.0f});
    player2Timer.setPosition({WINDOW_WIDTH - 250.0f, 130.0f}, {WINDOW_WIDTH - 250.0f, 160.0f});

    // Set initial player turn
    player1Timer.setPlayerTurn(!isWhiteTurn); // Black goes first
    player2Timer.setPlayerTurn(isWhiteTurn);

    // 調整分數文字位置，避免遮擋其他UI元素
    scoreText.setString("Score: " + std::to_string(player1Score) + " - " + std::to_string(player2Score));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);  // 改為白色以提高可見度
    scoreText.setOutlineThickness(1.0f);
    scoreText.setOutlineColor(sf::Color::Black);
    // 將分數移到更安全的位置
    scoreText.setPosition({WINDOW_WIDTH / 2.0f - 60.0f, 80.0f});

    // 調整當前玩家文字位置
    currentPlayerText.setString("Current Turn: Black");
    currentPlayerText.setCharacterSize(20);
    currentPlayerText.setFillColor(sf::Color::White);
    currentPlayerText.setOutlineThickness(1.0f);
    currentPlayerText.setOutlineColor(sf::Color::Black);
    currentPlayerText.setPosition({WINDOW_WIDTH / 2.0f - 80.0f, 110.0f});

    // Set up game board
    boardBackground.setSize({cellSize * 8, cellSize * 8});
    boardBackground.setPosition({boardX, boardY});
    boardBackground.setFillColor(sf::Color(30, 120, 30)); // Dark green
    boardBackground.setOutlineThickness(2.0f);
    boardBackground.setOutlineColor(sf::Color::Black);

    // Set up cell highlight
    cellHighlight.setSize({cellSize, cellSize});
    cellHighlight.setFillColor(sf::Color(100, 200, 100, 100)); // Semi-transparent green

    // 設置計時器半透明背景
    timerBackground1.setSize({180.0f, 90.0f});
    timerBackground1.setPosition({40.0f, 90.0f});
    timerBackground1.setFillColor(sf::Color(255, 255, 255, 100));

    timerBackground2.setSize({180.0f, 90.0f});
    timerBackground2.setPosition({WINDOW_WIDTH - 220.0f, 90.0f});
    timerBackground2.setFillColor(sf::Color(255, 255, 255, 100));

    // 調整UI元素位置 - 將標題和分數移到頂部
    titleText.setPosition({WINDOW_WIDTH / 2.0f, 30.0f});

    scoreText.setPosition({WINDOW_WIDTH / 4.0f, 30.0f});
    currentPlayerText.setPosition({WINDOW_WIDTH * 3.0f / 4.0f, 30.0f});

    // Show available moves
    gameLogic.showPlayPlace(isWhiteTurn);

    // Set up board pieces
    updateBoardPieces();

    // Start transition in
    startTransitionIn();
}

// Modified section from GameScreen::handleInput
void GameScreen::handleInput(sf::Event event) {
    // SFML 3.0: 不再需要检查event类型，直接处理传入的event
    if (!event.is<sf::Event::MouseButtonReleased>() || transitioning) return;

    if (const auto &mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        !mouseEvent || mouseEvent->button != sf::Mouse::Button::Left) {
        return;
        }

    // Check button clicks ( MainMenu )
    if (menuButton.wasClicked()) {
        const auto mainMenu = std::make_shared<MainMenu>(window, stateChangeCallback);
        mainMenu->init();
        startTransitionTo(mainMenu);
        return;
    }
    // Check button clicks ( Save )
    if (saveButton.wasClicked()) {
        saveCurrentGame();
        return;
    }
    // Check button clicks ( Undo )
    if (undoButton.wasClicked() && !moveHistory.empty()) {
        undoMove();
        return;
    }

    // Check board clicks
    if (gameOver) {
        return;
    }

    // Prevent board interaction while AI is thinking
    if (vsComputer && isWhiteTurn && aiThinking) {
        return;
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Calculate board coordinates
    int boardCoordX = static_cast<int>((mousePos.x - boardX) / cellSize);
    int boardCoordY = static_cast<int>((mousePos.y - boardY) / cellSize);

    // Check if click is within board bounds
    if (boardCoordX < 0 || boardCoordX >= 8 || boardCoordY < 0 || boardCoordY >= 8) {
        return;
    }

    // Try to make a move
    if (makeMove(boardCoordX, boardCoordY)) {
        // Move was successful, check if game over conditions
        updateScores();
        checkGameOver();
    }
}
// New method to check if the game should end
void GameScreen::checkGameOver() {
    // Check if current player has valid moves
    bool currentPlayerHasValidMoves = false;
    for (auto &y: gameLogic.board) {
        for (char x: y) {
            if (x == 'a') {
                currentPlayerHasValidMoves = true;
                break;
            }
        }
        if (currentPlayerHasValidMoves)
            break;
    }

    if (!currentPlayerHasValidMoves) {
        // Current player has no valid moves, switch to other player
        isWhiteTurn = !isWhiteTurn;
        gameLogic.showPlayPlace(isWhiteTurn);

        // Check if other player has valid moves
        bool otherPlayerHasValidMoves = false;
        for (auto &y: gameLogic.board) {
            for (char x: y) {
                if (x == 'a') {
                    otherPlayerHasValidMoves = true;
                    break;
                }
            }
            if (otherPlayerHasValidMoves)
                break;
        }

        if (!otherPlayerHasValidMoves) {
            // Both players have no valid moves, game is over
            endGame();
            return; // Add this return to prevent further execution
        } else {
            // Other player can move, update UI
            currentPlayerText.setString("Current Turn: " + std::string(isWhiteTurn ? "White" : "Black"));
            player1Timer.setPlayerTurn(!isWhiteTurn);
            player2Timer.setPlayerTurn(isWhiteTurn);

            // Add this block to trigger AI move when human has no moves
            if (vsComputer && isWhiteTurn) {
                // Reset AI thinking timer to trigger an AI move in the next update
                aiThinking = true;
                aiThinkingTime = 0.0f;
            }
        }
    }
}

// New method to handle end game conditions
void GameScreen::endGame() {
    gameOver = true;

    // Determine winner based on score
    std::string winner;
    if (player1Score > player2Score) {
        winner = player1Name;
    } else if (player2Score > player1Score) {
        winner = player2Name;
    } else {
        winner = "It's a tie!";
    }

    // Create victory screen
    auto victoryScreen =
            std::make_shared<VictoryScreen>(window, stateChangeCallback, winner, player1Score, player2Score);
    victoryScreen->init();

    // Play victory sound
    sf::Sound victorySound(resources->getSoundBuffer("victory"));
    victorySound.play();

    // Transition to victory screen
    startTransitionTo(victoryScreen);
}

// Add this to your GameScreen.h file in the public section:
void checkGameOver();

void GameScreen::update(float deltaTime) {
    GameState::update(deltaTime);

    // 存檔按鈕文字恢復邏輯
    if (saveButtonPressed) {
        saveButtonTimer += deltaTime;
        if (saveButtonTimer >= 2.0f) { // 2秒後恢復原始文字
            saveButton.setText(originalSaveButtonText);
            saveButtonPressed = false;
            saveButtonTimer = 0.0f;
        }
    }

    // Update buttons
    undoButton.update(window);
    menuButton.update(window);
    saveButton.update(window);

    if (vsComputer && isWhiteTurn && !gameOver) {
        if (!aiThinking) {
            aiThinking = true;
            aiThinkingTime = 0.0f;
        } else {
            aiThinkingTime += deltaTime;

            // After 1 second delay, make AI move
            if (aiThinkingTime >= 1.0f) {
                makeAIMove();
                aiThinking = false;

                // After AI move, check if human player has valid moves
                bool humanHasValidMoves = false;
                for (auto &y: gameLogic.board) {
                    for (char x: y) {
                        if (x == 'a') {
                            humanHasValidMoves = true;
                            break;
                        }
                    }
                    if (humanHasValidMoves)
                        break;
                }

                // If human has no valid moves, it's still AI's turn
                if (!humanHasValidMoves && !gameOver) {
                    isWhiteTurn = true;
                    gameLogic.showPlayPlace(true);
                    currentPlayerText.setString("Current Turn: White");
                    player1Timer.setPlayerTurn(false);
                    player2Timer.setPlayerTurn(true);
                    updateBoardPieces();

                    // Reset AI thinking to make another move
                    aiThinking = true;
                    aiThinkingTime = 0.0f;
                }
            }
        }
    }


    // Update timers if game is not over
    if (!gameOver) {
        player1Timer.update(deltaTime);
        player2Timer.update(deltaTime);

        // Check for timer expiration
        checkTimers();
    }

    // Get mouse position for highlighting cells
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int boardCoordX = static_cast<int>((mousePos.x - boardX) / cellSize);
    int boardCoordY = static_cast<int>((mousePos.y - boardY) / cellSize);

    // Check if mouse is over the board
    if (boardCoordX >= 0 && boardCoordX < 8 && boardCoordY >= 0 && boardCoordY < 8) {
        selectedX = boardCoordX;
        selectedY = boardCoordY;
    } else {
        selectedX = -1;
        selectedY = -1;
    }
}

void GameScreen::render() {
    window.draw(backgroundSprite);
    window.draw(boardBackground);

    // 繪製計時器半透明背景
    drawRoundedRectangle(window, timerBackground1.getPosition(),
                        timerBackground1.getSize(),
                        timerBackground1.getFillColor(), 10.0f);
    drawRoundedRectangle(window, timerBackground2.getPosition(),
                        timerBackground2.getSize(),
                        timerBackground2.getFillColor(), 10.0f);

    // Draw board grid
    for (int y = 0; y <= 8; y++) {
        sf::Vertex hLine[] = {
            sf::Vertex{sf::Vector2f(boardX, boardY + y * cellSize), sf::Color::Black},
            sf::Vertex{sf::Vector2f(boardX + 8 * cellSize, boardY + y * cellSize), sf::Color::Black}
        };
        window.draw(hLine, 2, sf::PrimitiveType::Lines);
    }

    for (int x = 0; x <= 8; x++) {
        sf::Vertex vLine[] = {
            sf::Vertex{sf::Vector2f(boardX + x * cellSize, boardY), sf::Color::Black},
            sf::Vertex{sf::Vector2f(boardX + x * cellSize, boardY + 8 * cellSize), sf::Color::Black}
        };
        window.draw(vLine, 2, sf::PrimitiveType::Lines);
    }

    // Draw selected cell highlight
    if (selectedX >= 0 && selectedY >= 0) {
        cellHighlight.setPosition({boardX + selectedX * cellSize, boardY + selectedY * cellSize});
        window.draw(cellHighlight);
    }

    // Draw pieces
    for (const auto &piece: pieces) {
        window.draw(piece);
    }

    // Draw text elements
    window.draw(titleText);
    window.draw(player1Text);
    window.draw(player2Text);
    window.draw(scoreText);
    window.draw(currentPlayerText);

    // Draw timers
    player1Timer.draw(window);
    player2Timer.draw(window);

    // Draw buttons
    undoButton.draw(window);
    menuButton.draw(window);
    saveButton.draw(window);

    // Draw transition overlay
    renderTransition();
}

void GameScreen::updateScores() {
    player1Score = 0;
    player2Score = 0;

    for (auto &y: gameLogic.board) {
        for (char x: y) {
            if (x == 'b') {
                player1Score++;
            } else if (x == 'w') {
                player2Score++;
            }
        }
    }

    scoreText.setString("Score: " + std::to_string(player1Score) + " - " + std::to_string(player2Score));
}

void GameScreen::updateBoardPieces() {
    pieces.clear();

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (gameLogic.board[y][x] == 'b' || gameLogic.board[y][x] == 'w') {
                sf::CircleShape piece(cellSize / 2.0f - 5.0f);

                if (gameLogic.board[y][x] == 'b') {
                    piece.setFillColor(sf::Color::Black);
                } else {
                    piece.setFillColor(sf::Color::White);
                }

                piece.setPosition({boardX + x * cellSize + 5.0f, boardY + y * cellSize + 5.0f});

                piece.setOutlineThickness(2.0f);
                piece.setOutlineColor(sf::Color(100, 100, 100));

                pieces.push_back(piece);
            } else if (gameLogic.board[y][x] == 'a') {
                // Draw available move indicators
                sf::CircleShape availableMove(cellSize / 2.0f - 20.0f);
                availableMove.setFillColor(sf::Color(100, 200, 100, 150));
                availableMove.setPosition({boardX + x * cellSize + 20.0f, boardY + y * cellSize + 20.0f});

                pieces.push_back(availableMove);
            }
        }
    }
}

bool GameScreen::makeMove(int x, int y) {
    // Check if the move is valid
    if (gameLogic.board[y][x] != 'a') {
        return false;
    }

    // Place the piece
    gameLogic.board[y][x] = isWhiteTurn ? 'w' : 'b';

    // Record the move for undo
    moveHistory.push_back({x, y, isWhiteTurn});

    // Play sound
    placeSound.setBuffer(resources->getSoundBuffer("place"));
    placeSound.play();

    // Turn over opponent's pieces
    gameLogic.turnOver(x, y, isWhiteTurn);

    // Switch player
    isWhiteTurn = !isWhiteTurn;

    // Show available move for next player
    gameLogic.showPlayPlace(isWhiteTurn);

    // Update board pieces
    updateBoardPieces();

    currentPlayerText.setString("Current Turn: " + std::string(isWhiteTurn ? "White" : "Black"));

    // Switch timers
    player1Timer.setPlayerTurn(!isWhiteTurn);
    player2Timer.setPlayerTurn(isWhiteTurn);

    // Check if the next player has valid moves
    // This is the key part we need to add
    bool nextPlayerHasValidMoves = false;
    for (auto &y: gameLogic.board) {
        for (char x: y) {
            if (x == 'a') {
                nextPlayerHasValidMoves = true;
                break;
            }
        }
        if (nextPlayerHasValidMoves)
            break;
    }

    // If next player has no valid moves, switch back to the other player
    if (!nextPlayerHasValidMoves) {
        // No valid moves for next player, so switch back
        isWhiteTurn = !isWhiteTurn;

        // Show available moves for original player again
        gameLogic.showPlayPlace(isWhiteTurn);

        // Update UI
        currentPlayerText.setString("Current Turn: " + std::string(isWhiteTurn ? "White" : "Black"));
        player1Timer.setPlayerTurn(!isWhiteTurn);
        player2Timer.setPlayerTurn(isWhiteTurn);

        // Update board to show valid moves
        updateBoardPieces();

        // Check if the original player also has no moves - would be game over
        bool originalPlayerHasValidMoves = false;
        for (auto &y: gameLogic.board) {
            for (char x: y) {
                if (x == 'a') {
                    originalPlayerHasValidMoves = true;
                    break;
                }
            }
            if (originalPlayerHasValidMoves)
                break;
        }

        if (!originalPlayerHasValidMoves) {
            // Both players have no moves, game over
            endGame();
        }
    }

    return true;
}

void GameScreen::checkTimers() {
    if (!isWhiteTurn && player1Timer.getRemainingChances() <= 0) {
        // Black (Player 1) ran out of time - White (Player 2) wins
        gameOver = true;

        auto victoryScreen =
                std::make_shared<VictoryScreen>(window, stateChangeCallback, player2Name, player1Score, player2Score);
        victoryScreen->init();

        startTransitionTo(victoryScreen);
    } else if (isWhiteTurn && player2Timer.getRemainingChances() <= 0) {
        // White (Player 2) ran out of time - Black (Player 1) wins
        gameOver = true;

        auto victoryScreen =
                std::make_shared<VictoryScreen>(window, stateChangeCallback, player1Name, player1Score, player2Score);
        victoryScreen->init();

        startTransitionTo(victoryScreen);
    }
}

void GameScreen::undoMove() {
    if (moveHistory.empty()) {
        return;
    }

    // Get the last move
    Move lastMove = moveHistory.back();
    moveHistory.pop_back();

    // Reset the game board to the previous state
    gameLogic.initialize();

    // Replay all moves except the last one
    for (const auto &move: moveHistory) {
        gameLogic.board[move.y][move.x] = move.wasWhiteTurn ? 'w' : 'b';
        gameLogic.turnOver(move.x, move.y, move.wasWhiteTurn);
    }

    // Set the current player
    isWhiteTurn = lastMove.wasWhiteTurn;
    currentPlayerText.setString("Current Turn: " + std::string(isWhiteTurn ? "White" : "Black"));

    // Show available moves
    gameLogic.showPlayPlace(isWhiteTurn);

    // Update board pieces
    updateBoardPieces();

    // Update scores
    updateScores();

    // Reset timers to correct players
    player1Timer.setPlayerTurn(!isWhiteTurn);
    player2Timer.setPlayerTurn(isWhiteTurn);
}

void GameScreen::saveCurrentGame() {
    const bool success = saveGameManager.saveGame(gameLogic, player1Name, player2Name, isWhiteTurn, vsComputer,
                                            player1Timer.getRemainingChances(), player2Timer.getRemainingChances());

    if (success) {
        sf::Sound saveSound(resources->getSoundBuffer("click"));
        saveSound.play();

        // 顯示成功訊息，並設定定時器
        saveButton.setText("Saved!");
        saveButtonPressed = true;
        saveButtonTimer = 0.0f;
    } else {
        // 顯示失敗訊息
        saveButton.setText("Save Failed");
        saveButtonPressed = true;
        saveButtonTimer = 0.0f;
    }
}

void GameScreen::setLoadedGameData(const FundamentalFunction &loadedLogic, bool currentTurn, int player1Chances = 3,
                                   int player2Chances = 3) {
    // Copy the board state
    for (int y = 0; y < BOARDLENGTH; y++) {
        for (int x = 0; x < BOARDLENGTH; x++) {
            gameLogic.board[y][x] = loadedLogic.board[y][x];
        }
    }

    if (vsComputer) {
        gameLogic.setAIDifficulty(aiDifficulty);
    }

    // Set current turn
    isWhiteTurn = currentTurn;

    // Setup timers for correct player turn
    player1Timer.setPlayerTurn(!isWhiteTurn);
    player2Timer.setPlayerTurn(isWhiteTurn);

    // Set the saved timer values
    player1Timer.setRemainingTime(30);
    player1Timer.setRemainingChances(player1Chances);
    player2Timer.setRemainingTime(30);
    player2Timer.setRemainingChances(player2Chances);

    // Update the current player text
    currentPlayerText.setString("Current Turn: " + std::string(isWhiteTurn ? "White" : "Black"));

    // Update board visuals
    updateBoardPieces();

    // Update scores
    updateScores();

    // Show available moves for the current player
    gameLogic.showPlayPlace(isWhiteTurn);
    updateBoardPieces(); // Update again to show available moves
}

// Add to GameScreen.h (in public or private section, depending on your design)

void GameScreen::makeAIMove() {
    // check ai valid move
    bool hasValidMoves = false;

    for (auto &y: gameLogic.board) {
        for (char x: y) {
            if (x == 'a') {
                hasValidMoves = true;
                break;
            }
        }
        if (hasValidMoves)
            break;
    }
    // If no valid move change to black
    if (!hasValidMoves) {
        isWhiteTurn = false;
        gameLogic.showPlayPlace(false);
        currentPlayerText.setString("Current Turn: Black");
        player1Timer.setPlayerTurn(true);
        player2Timer.setPlayerTurn(false);
        updateBoardPieces();

        // Check if the human player also has no valid moves - would be game over
        bool humanHasValidMoves = false;
        for (auto &y: gameLogic.board) {
            for (char x: y) {
                if (x == 'a') {
                    humanHasValidMoves = true;
                    break;
                }
            }
            if (humanHasValidMoves)
                break;
        }

        if (!humanHasValidMoves) {
            // Both players have no moves, game over
            endGame();
        }

        return;
    }
    try {
        // 獲取 AI 的選擇移動
        auto [moveX, moveY] = gameLogic.AIPlayChess();

        // 確保返回的坐標在有效範圍內
        if (moveX >= 0 && moveX < 8 && moveY >= 0 && moveY < 8 && gameLogic.board[moveY][moveX] == 'a') {
            // 放置棋子
            gameLogic.board[moveY][moveX] = 'w';

            // 記錄移動用於撤銷
            moveHistory.push_back({moveX, moveY, true});

            // 播放聲音
            placeSound.setBuffer(resources->getSoundBuffer("place"));
            placeSound.play();

            // 翻轉對手的棋子
            gameLogic.turnOver(moveX, moveY, true);

            // 切換回人類玩家
            isWhiteTurn = false;

            // 顯示人類玩家的可用移動
            gameLogic.showPlayPlace(isWhiteTurn);

            // 更新棋盤顯示
            updateBoardPieces();

            // 更新 UI 顯示現在是人類的回合
            currentPlayerText.setString("Current Turn: Black");

            // 切換計時器
            player1Timer.setPlayerTurn(true); // 人類玩家 (黑)
            player2Timer.setPlayerTurn(false); // AI 玩家 (白)

            // 更新分數
            updateScores();

            // Check for game over after AI move
            checkGameOver();
        } else {
            // AI 返回的移動無效，切換到人類玩家
            isWhiteTurn = false;
            gameLogic.showPlayPlace(false);
            currentPlayerText.setString("Current Turn: Black");
            player1Timer.setPlayerTurn(true);
            player2Timer.setPlayerTurn(false);
            updateBoardPieces();

            // Check for game over when AI has no valid moves
            checkGameOver();
        }
    } catch (...) {
        // 捕獲任何可能的異常
        isWhiteTurn = false;
        gameLogic.showPlayPlace(false);
        currentPlayerText.setString("Current Turn: Black");
        player1Timer.setPlayerTurn(true);
        player2Timer.setPlayerTurn(false);
        updateBoardPieces();

        // Check for game over on exception
        checkGameOver();
    }
}

void GameScreen::setAIDifficulty(const AILevel level) {
    aiDifficulty = level;
    gameLogic.setAIDifficulty(level);
}
