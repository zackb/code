#pragma once

#include "corex/GameState.h"
#include "corex/ui/Font.h"
#include "corex/ui/Text.h"

class LevelSelectState : public GameState {

public:
    LevelSelectState(Engine& engine, const std::string& currentLevel, const std::string& currentPlayer);
    ~LevelSelectState() = default;

    void handleEvent() override;
    void update(int deltaTime) override;
    void render() override;
    std::unique_ptr<GameState> nextState() override;

private:
    std::string currentLevel;
    std::string currentPlayer;
    ui::Font font;
    ui::Text text;
    int totalTime = 0;
    bool startGame = 0;
};
