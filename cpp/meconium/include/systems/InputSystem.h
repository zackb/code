#pragma once

#include "components/InputControl.h"
#include "entity/Entity.h"

#include <SDL.h>
#include <memory>
#include <vector>

class InputSystem {
public:
    void update(std::vector<std::shared_ptr<Entity>>& entities, const Uint8* keyboardState) {
        for (auto& entity : entities) {
            if (auto input = entity->getComponent<InputControl>()) {
                input->update(keyboardState);
            }
        }
    }
};
