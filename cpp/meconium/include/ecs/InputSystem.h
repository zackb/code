#pragma once

#include <SDL.h>
#include <vector>
#include <memory>
#include "ECS.h"
#include "InputControl.h"

class InputSystem
{
public:
    void update(std::vector<std::shared_ptr<Entity>> &entities, const Uint8 *keyboardState)
    {
        for (auto &entity : entities)
        {
            if (auto input = entity->getComponent<InputControl>())
            {
                input->update(keyboardState);
            }
        }
    }
};
