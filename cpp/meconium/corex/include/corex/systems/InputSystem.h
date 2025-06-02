#pragma once

#include "corex/components/InputControl.h"
#include "corex/entity/Entity.h"

#include <SDL.h>

class InputSystem {

public:
    void update(Entities& entities, const Uint8* keyboardState) {
        for (auto& entity : entities.filtered<InputControl>()) {
            entity->getComponent<InputControl>()->update(keyboardState);
        }
    }
};
