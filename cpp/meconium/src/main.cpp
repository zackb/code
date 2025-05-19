#include "Context.h"
#include "Meconium.h"
#include "MenuState.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

bool initSDL();
void quitSDL();

int main(int argc, char* argv[]) {
    if (!initSDL()) {
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
            if (event.type == SDL_QUIT) isRunning = false;
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

    // unload all resources
    ResourceManager::cleanup();

    // destroy SDL
    quitSDL();

    return EXIT_SUCCESS;
}

bool initSDL() {

    // init sdl2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    Context::window =
        SDL_CreateWindow("Meconium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, SDL_WINDOW_SHOWN);

    if (!Context::window) {
        std::cerr << "CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    Context::renderer = SDL_CreateRenderer(Context::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!Context::renderer) {
        std::cerr << "CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(Context::window);
        SDL_Quit();
        return false;
    }

    // init sdl2_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        return false;
    }

    // init sdl2_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: %s" << Mix_GetError() << std::endl;
        return false;
    }

    // init sdl2_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }

    // hold window size in Context
    SDL_GetWindowSize(Context::window, &Context::windowSize.width, &Context::windowSize.height);

    return true;
}

void quitSDL() {
    SDL_DestroyRenderer(Context::renderer);
    SDL_DestroyWindow(Context::window);
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}