#pragma once

#include <SDL.h>

struct Sprite {
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};