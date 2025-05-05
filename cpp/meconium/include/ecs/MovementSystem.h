#pragma once

#include <vector>
#include <memory>
#include "Entity.h"

class MovementSystem {
    public:
        void update(std::vector<std::shared_ptr<Entity>>& entities) {
            for (auto& entity : entities) {
                auto position = entity->getComponent<Position>();
                auto velocity = entity->getComponent<Velocity>();
                auto input = entity->getComponent<InputControl>();
                if (position && velocity && input) {
    
                    // Update velocity based on input
                    velocity->vx = (input->right - input->left) * 15;
                    velocity->vy = (input->down - input->up) * 15;
    
                    // Update position based on velocity
                    position->x += velocity->vx;
                    position->y += velocity->vy;
                }
            }
        }
    };
