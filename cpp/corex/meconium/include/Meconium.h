#pragma once

#include "corex/GameState.h"
#include <string>

class Meconium : public GameState {

public:
    Meconium(Engine& engine) : GameState(engine) {}

    bool init(std::string character = "blue", std::string level = "level2");
    void update(int deltaTime) override;
    void render() override;
    void handleEvent() override;
    std::unique_ptr<GameState> nextState() override;

private:
    std::unique_ptr<GameState> next;
    std::string playerName;
    std::string levelName;
};
