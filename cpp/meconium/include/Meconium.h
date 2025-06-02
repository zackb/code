#pragma once

#include "GameState.h"

class Meconium : public GameState {

public:
    Meconium(Engine& engine) : GameState(engine) {}

    bool init(std::string character = "blue");
    void update() override;
    void render() override;
    void handleEvent(SDL_Event& event) override;
    std::unique_ptr<GameState> nextState() override;

private:
    std::unique_ptr<GameState> next;
};
