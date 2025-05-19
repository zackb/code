#pragma once

#include <SDL.h>
#include <memory>

class GameState {
public:
    virtual ~GameState() = default;

    virtual void handleEvent(SDL_Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual std::unique_ptr<GameState> nextState() { return nullptr; }

};
