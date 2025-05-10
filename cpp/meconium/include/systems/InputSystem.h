#pragma once

#include "components/InputControl.h"
#include "entity/Entity.h"

#include <SDL.h>
#include <memory>

class InputSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const Uint8* keyboardState) {
        for (auto& entity : *entities) {
            if (auto input = entity->getComponent<InputControl>()) {
                input->update(keyboardState);
            }
        }
    }
};
