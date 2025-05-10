#pragma once
#include "Component.h"
#include <SDL.h>

class Background final : public Component {
public:
    SDL_Texture* texture = nullptr;
    float speed = 0.5f; // Lower = farther back
    int width = 0;
    int height = 0;
    int layer = 0; // Lower number = drawn first

    Background(SDL_Texture* tex, float scrollFactor, int layerOrder)
        : texture(tex), speed(scrollFactor), layer(layerOrder) {
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    }

    ~Background() override = default;
};

class ParallaxBackground final : public Component {
public:
    explicit ParallaxBackground(const std::vector<std::shared_ptr<Background>>& backgrounds) : backgrounds(backgrounds) {}
    std::vector<std::shared_ptr<Background>> getLayers() {
        return backgrounds;
    }

private:
    std::vector<std::shared_ptr<Background>> backgrounds;
};