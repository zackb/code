#pragma once

#include "Component.h"
#include "Transform.h"
#include <SDL.h>
#include <memory>

class Collider : public Component {
public:
    int offsetX = 0, offsetY = 0;
    int width = 0, height = 0;

    Collider(int offsetX, int offsetY, int width, int height)
        : offsetX(offsetX), offsetY(offsetY), width(width), height(height) {}

    SDL_Rect getBounds(const std::shared_ptr<Transform>& transform) const {
        return SDL_Rect{
            static_cast<int>(transform->x + offsetX * transform->scaleX),
            static_cast<int>(transform->y + offsetY * transform->scaleY),
            static_cast<int>(width * transform->scaleX),
            static_cast<int>(height * transform->scaleY)
        };
    }
};