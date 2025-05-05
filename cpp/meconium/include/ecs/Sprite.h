#pragma once

#include "Component.h"

#include <SDL.h>

class Sprite : public Component
{
public:
    SDL_Texture *texture = nullptr;
    int width = 0;
    int height = 0;
};