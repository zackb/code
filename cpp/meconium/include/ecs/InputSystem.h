#pragma once

#include <SDL.h>
#include <vector>
#include <memory>
#include "ECS.h"
#include "InputControl.h"

class InputSystem {
    public:
        void update(std::vector<std::shared_ptr<Entity>>& entities, SDL_Event& e) {
            for (auto& entity : entities) {
                if (auto inputPtr = entity->getComponent<InputControl>()) {
                    auto& input = *inputPtr;
                    input.up = input.down = input.left = input.right = false;
    
                    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                        if (e.key.keysym.sym == SDLK_UP) input.up = true;
                        if (e.key.keysym.sym == SDLK_DOWN) input.down = true;
                        if (e.key.keysym.sym == SDLK_LEFT) input.left = true;
                        if (e.key.keysym.sym == SDLK_RIGHT) input.right = true;
                    }
                }
            }
        }
    };
