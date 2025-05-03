#include <iostream>
#include "Meconium.h"

int main(int argc, char* argv[]) {

    auto meconium = Meconium();

    if (!meconium.init()) {
        return EXIT_FAILURE;
    }

    const int FPS = 60;
    const int frameDelay = 1000 / 60;
    Uint32 frameStart;
    int frameTime;

    while (meconium.running())
    {
        frameStart = SDL_GetTicks();

        meconium.handleEvent();
        meconium.update();
        meconium.render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    meconium.shutdown();

    return EXIT_SUCCESS;
}

