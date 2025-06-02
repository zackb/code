#pragma once

#include "corex/Size.h"
#include <SDL.h>

class Context {

public:
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static Size windowSize;
    static double avgFPS;

    static bool init();
    static void destroy();
};
