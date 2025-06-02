#pragma once

#include "GameState.h"

class Engine {
public:
    bool init();
    void run(std::shared_ptr<GameState>& initialState);
    void shutdown();

private:
    std::shared_ptr<GameState> state;
};
