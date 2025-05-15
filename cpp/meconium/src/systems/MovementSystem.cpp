#include "ECS.h"

// Handles both physics as well as player input
// TOOD: separate into Physics and InputSystem
void MovementSystem::update(const std::shared_ptr<Entities>& entities) const {
    const float GRAVITY = 1.0f;
    const float JUMP_FORCE = -22.0f;
    const float MAX_FALL_SPEED = 30.0f;

    for (auto& entity : *entities) {
        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto input = entity->getComponent<InputControl>();
        auto sprite = entity->getComponent<Sprite>();

        if (!position || !velocity)
            continue;

        // Apply gravity
        if (!position->onGround) {
            velocity->vy += GRAVITY;
            if (velocity->vy > MAX_FALL_SPEED)
                velocity->vy = MAX_FALL_SPEED;
        }

        if (input) {
            if (input->isDown(InputKey::MOVE_LEFT)) {
                velocity->vx = -5;
                if (sprite)
                    sprite->flipX = true;
            } else if (input->isDown(InputKey::MOVE_RIGHT)) {
                velocity->vx = 5;
                if (sprite)
                    sprite->flipX = false;
            } else {
                velocity->vx = 0;
            }

            // Jump logic
            if (input->isDown(InputKey::JUMP) && position->onGround) {
                velocity->vy = JUMP_FORCE;
                position->onGround = false;
            }
        }

        // Update position in CollisionSystem
        // position->x += velocity->vx;
        // position->y += velocity->vy;
    }
}
