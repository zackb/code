#include "Context.h"
#include "MenuState.h"

int main(int argc, char* argv[]) {

    if (!Context::init()) {
        return EXIT_FAILURE;
    }

    // Start in MenuState
    std::unique_ptr<GameState> state = std::make_unique<MenuState>();

    bool isRunning = true;
    SDL_Event event;

    constexpr int FPS = 60;
    constexpr int frameDelay = 1000 / FPS;

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

        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    // destroy SDL
    Context::destroy();

    return EXIT_SUCCESS;
}
