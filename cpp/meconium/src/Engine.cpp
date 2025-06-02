#include "Engine.h"
#include "Context.h"
#include <SDL.h>

bool Engine::init() { return Context::init(); }

void Engine::run(std::unique_ptr<GameState> initialState) {
    state = std::move(initialState);

    bool isRunning = true;
    SDL_Event event;

    constexpr int FPS = 60;
    constexpr int frameDelay = 1000 / FPS;

    Context::avgFPS = FPS;     // start at target FPS
    const double alpha = 0.05; // smoothing factor for EMA

    while (isRunning) {
        const Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;
            state->handleEvent(event);
        }

        state->update();
        state->render();

        // Transition to the next state if needed
        if (std::unique_ptr<GameState> next = state->nextState()) {
            state = std::move(next);
        }

        Uint32 frameEnd = SDL_GetTicks();
        Uint32 frameTime = frameEnd - frameStart;

        // Update running average FPS
        double currentFPS = (frameTime > 0) ? (1000.0 / frameTime) : 0.0;
        Context::avgFPS = (1.0 - alpha) * Context::avgFPS + alpha * currentFPS;

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Engine::shutdown() { Context::destroy(); }
