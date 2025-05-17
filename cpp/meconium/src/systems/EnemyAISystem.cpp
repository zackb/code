#include "systems/EnemyAISystem.h"

#include "components/EnemyBehavior.h"
#include "components/Knockback.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"

// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities>& entities) const {
    for (const auto& entity : *entities) {
        if (!entity->hasComponent<EnemyTag>())
            continue;

        if (entity->hasComponent<Knockback>())
            continue;

        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto state = entity->getComponent<State>();
        auto sprite = entity->getComponent<Sprite>();

        auto player = entities->findEntityWithComponent<PlayerTag>();
        auto playerPos = player->getComponent<Transform>();

        if (!position || !velocity || !state || !sprite) {
            std::cerr << "missing required components of enemy" << std::endl;
            continue;
        }
        //
        // if the enemy is dying there's nothing to do
        if (state && state->currentAction == Action::DYING) {
            velocity->vx = 0;
            velocity->vy = 0;
            continue;
        }

        velocity->vx = 0;
        auto behavior = entity->getComponent<EnemyBehavior>();
        auto patrol = entity->getComponent<Patrol>();
        if (!behavior) {
            std::cerr << "enemy has no behavior" << std::endl;
            continue;
        }

        switch (*behavior) {
        case EnemyBehavior::IDLE:
            state->currentAction = Action::IDLE;
            if (playerPos->x > position->x) {
                state->facingRight = true;
                sprite->flipX = false;
            } else {
                state->facingRight = false;
                sprite->flipX = true;
            }
            break;
        case EnemyBehavior::PATROL:
            if (!patrol) {
                std::cerr << "patrolling enemy with no patrol" << std::endl;
            }
            state->currentAction = Action::WALKING;
            if (state->facingRight) {
                velocity->vx = patrol->speed;
                if (position->x >= patrol->right) {
                    state->facingRight = false;
                    sprite->flipX = true;
                }
            } else {
                velocity->vx = -patrol->speed;
                if (position->x <= patrol->left) {
                    state->facingRight = true;
                    sprite->flipX = false;
                }
            }
            break;
        default:
            std::cerr << "unknown enemy action" << std::endl;
            break;
        }
    }
}
