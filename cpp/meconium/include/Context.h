#pragma once
#include <SDL.h>
#include "Size.h"

class Context {

public:
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static Size windowSize;
};