#pragma once

#include "Entity.h"
#include "Size.h"
#include "Context.h"

#include <vector>
#include <memory>

class MovementSystem
{
public:
    static int groundLevel(const Size &windowSize)
    {
        return static_cast<int>(windowSize.height * 0.9f);
    }

    void update(std::vector<std::shared_ptr<Entity>> &entities)
    {

        const float GRAVITY = 1.0f;
        const float JUMP_FORCE = -20.0f;
        const float MAX_FALL_SPEED = 30.0f;
        const int GROUND_LEVEL = MovementSystem::groundLevel(Context::windowSize);


        for (auto &entity : entities)
        {
            auto position = entity->getComponent<Position>();
            auto velocity = entity->getComponent<Velocity>();
            auto input = entity->getComponent<InputControl>();

            if (!position || !velocity || !input)
                continue;

            if (input->isDown(InputKey::MOVE_LEFT))
            {
                velocity->vx = -5;
            }
            else if (input->isDown(InputKey::MOVE_RIGHT))
            {
                velocity->vx = 5;
            }
            else
            {
                velocity->vx = 0;
            }

            // Jump logic
            if (input->isDown(InputKey::JUMP) && position->onGround)
            {
                velocity->vy = JUMP_FORCE;
                position->onGround = false;
            }

            // Apply gravity
            if (!position->onGround)
            {
                velocity->vy += GRAVITY;
                if (velocity->vy > MAX_FALL_SPEED)
                    velocity->vy = MAX_FALL_SPEED;
            }

            // Update position in CollisionSystem
            // position->x += velocity->vx;
            // position->y += velocity->vy;
        }
    }
};
