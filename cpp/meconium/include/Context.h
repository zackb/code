#pragma once
#include "Size.h"
#include <SDL.h>

class Context {

public:
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static Size windowSize;
};