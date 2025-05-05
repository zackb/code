#pragma once

#include <vector>
#include <memory>
#include "Entity.h"

class MovementSystem {
    public:
        void update(std::vector<std::shared_ptr<Entity>>& entities) {
            for (auto& entity : entities) {
                if (entity->hasPosition() && entity->hasVelocity() && entity->hasInputControl()) {
                    auto& position = *entity->position;
                    auto& velocity = *entity->velocity;
                    auto& input = *entity->inputControl;
    
                    // Update velocity based on input
                    velocity.vx = (input.right - input.left) * 15;
                    velocity.vy = (input.down - input.up) * 15;
    
                    // Update position based on velocity
                    position.x += velocity.vx;
                    position.y += velocity.vy;
                }
            }
        }
    };
