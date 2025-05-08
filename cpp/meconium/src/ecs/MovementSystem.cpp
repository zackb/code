#include "ecs/ECS.h"

void MovementSystem::update(const std::vector<std::shared_ptr<Entity>>& entities) const {
    const float GRAVITY = 1.0f;
    const float JUMP_FORCE = -20.0f;
    const float MAX_FALL_SPEED = 30.0f;
    const int GROUND_LEVEL = MovementSystem::groundLevel(Context::windowSize);

    for (auto& entity : entities) {
        auto position = entity->getComponent<Position>();
        auto velocity = entity->getComponent<Velocity>();
        auto input = entity->getComponent<InputControl>();
        auto sprite = entity->getComponent<Sprite>();

        if (!position || !velocity || !input)
            continue;

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

        // Apply gravity
        if (!position->onGround) {
            velocity->vy += GRAVITY;
            if (velocity->vy > MAX_FALL_SPEED)
                velocity->vy = MAX_FALL_SPEED;
        }

        // Update position in CollisionSystem
        // position->x += velocity->vx;
        // position->y += velocity->vy;
    }
}
