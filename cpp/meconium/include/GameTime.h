#pragma once

#include <SDL.h>

class GameTime {
public:
    static GameTime& getInstance() {
        static GameTime instance;
        return instance;
    }

    void update() {
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
    }

    int getDeltaTime() const { return deltaTime; }

private:
    GameTime() : lastTime(SDL_GetTicks()), deltaTime(0) {}
    Uint32 lastTime;
    int deltaTime;
};