#pragma once
#include <algorithm>
#include <memory>
#include <SDL_events.h>

#include "GameState.h"

class StateManager {
public:
    void changeState(std::unique_ptr<GameState> newState) {
        currentState = std::move(newState);
    }

    void handleEvent(SDL_Event& event) const {
        if (currentState) currentState->handleEvent(event);
    }

    void update() const {
        if (currentState) currentState->update();
    }

    void render() const {
        if (currentState) currentState->render();
    }

    GameState* getCurrentState() const {
        return currentState.get();
    }

private:
    std::unique_ptr<GameState> currentState;
};
