#pragma once

#include "components/InputControl.h"
#include "entity/Entity.h"

#include <SDL.h>

class InputSystem {
public:
    void update(Entities& entities, const Uint8* keyboardState) {
        for (auto& entity : entities) {
            if (auto input = entity->getComponent<InputControl>()) {
                input->update(keyboardState);
            }
        }
    }
};
