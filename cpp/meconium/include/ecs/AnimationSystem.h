#pragma once

#include "Animation.h"
#include "Entity.h"
#include <vector>
#include <memory>

#include "Position.h"
#include "Velocity.h"

class AnimationSystem {
public:
    void update(const std::vector<std::shared_ptr<Entity>>& entities, const int deltaTime) {
        for (auto& entity : entities) {
            const auto animation = entity->getComponent<AnimationComponent>();
            if (!animation) continue;

            // Update animation timing
            animation->update(deltaTime);

            // Update animation state based on entity state
            auto position = entity->getComponent<Position>();
            auto velocity = entity->getComponent<Velocity>();

            if (!position || !velocity) continue;

            // Determine animation state
            if (!position->onGround) {
                // In the air - jumping or falling
                if (velocity->vy < 0) {
                    animation->setState(AnimationState::JUMPING);
                } else {
                    animation->setState(AnimationState::FALLING);
                }
            } else {
                // On the ground - idle or walking
                if (velocity->vx != 0) {
                    animation->setState(AnimationState::WALKING);
                } else {
                    animation->setState(AnimationState::IDLE);
                }
            }
        }
    }
};
