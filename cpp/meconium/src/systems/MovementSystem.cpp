#include "systems/MovementSystem.h"

#include "components/InputControl.h"
#include "components/Knockback.h"
#include "components/NoGravity.h"
#include "components/SoundEffect.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "entity/Entity.h"

// Handles both physics as well as player input
void MovementSystem::update(Entities& entities) const {

    const float GRAVITY = 1.0f;
    const float JUMP_FORCE = -22.0f;
    const float MAX_FALL_SPEED = 30.0f;

    for (auto& entity : entities.filtered<Transform, Velocity>()) {

        // skip input control if we're being knocked back
        if (entity->hasComponent<Knockback>()) {
            return;
        }

        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto input = entity->getComponent<InputControl>();
        auto sprite = entity->getComponent<Sprite>();

        // if we're dying dont move
        if (auto state = entity->getComponent<State>()) {
            if (state->currentAction == Action::DYING) {
                velocity->vx = 0;
                velocity->vy = 0;
            }
        }

        // Apply gravity if we should
        if (!position->onGround && !entity->hasComponent<NoGravity>()) {
            velocity->vy += GRAVITY;
            if (velocity->vy > MAX_FALL_SPEED)
                velocity->vy = MAX_FALL_SPEED;
        }

        // this entity has input control and is not dying
        if (input) {
            if (input->isDown(InputKey::MOVE_LEFT)) {
                velocity->vx = -5;
                if (auto state = entity->getComponent<State>(); state) {
                    state->facingRight = false;
                }
                if (sprite)
                    sprite->flipX = true;
            } else if (input->isDown(InputKey::MOVE_RIGHT)) {
                velocity->vx = 5;
                if (auto state = entity->getComponent<State>(); state) {
                    state->facingRight = true;
                }
                if (sprite)
                    sprite->flipX = false;
            } else {
                velocity->vx = 0;
            }

            // Jump logic
            if (input->isDown(InputKey::JUMP) && position->onGround) {
                velocity->vy = JUMP_FORCE;
                position->onGround = false;
                entity->addComponent<SoundEffect>("jump", 0);
            }
        }

        // Update position in CollisionSystem
        // position->x += velocity->vx;
        // position->y += velocity->vy;
    }
}
