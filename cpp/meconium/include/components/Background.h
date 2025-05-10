#pragma once
#include "Component.h"
#include <SDL.h>

class Background : public Component {
public:
    SDL_Texture* texture = nullptr;
    float speed = 0.5f; // Lower = farther back
    int width = 0;
    int height = 0;
    int layer = 0; // Lower number = drawn first

    Background(SDL_Texture* tex, float factor, int layerOrder)
        : texture(tex), scrollFactor(factor), layer(layerOrder) {
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    }

    ~Background() override = default;
};