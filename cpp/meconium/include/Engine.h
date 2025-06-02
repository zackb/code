#pragma once

#include "GameState.h"

class Engine {
public:
    bool init();
    void run(std::unique_ptr<GameState> initialState);
    void shutdown();

private:
    std::unique_ptr<GameState> state;
};
