#pragma once

#include "Component.h"

#include <SDL.h>

class Sprite : public Component {
public:
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
    bool flipX = false;
    bool flipY = false;

    // projectile related
    int speed = 0;
    int lifetimeMs = 0;

    Sprite() = default;
    Sprite(SDL_Texture* texture, int width, int height) : texture(texture), width(width), height(height) {}

    ~Sprite() override = default;
};
