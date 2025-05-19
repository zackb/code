#pragma once
#include "Context.h"
#include "GameState.h"

class MenuState final : public GameState {
public:
    void handleEvent(SDL_Event& event) override {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            // Tell game to transition to PlayState
            requestStartGame = true;
        }
    }

    void update() override {
        // do some animations or something
    }

    void render() override {
        SDL_SetRenderDrawColor(Context::renderer, 0, 0, 50, 255);
        SDL_RenderClear(Context::renderer);

        // Render title screen, "Press Start"

        SDL_RenderPresent(Context::renderer);
    }

    bool shouldStartGame() const { return requestStartGame; }

private:
    bool requestStartGame = false;
};
