#include "systems/EnemyAISystem.h"

#include "components/EnemyBehavior.h"
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

        velocity->vx = 0;
        auto patrol = entity->getComponent<Patrol>();

        switch (state->currentAction) {
        case Action::IDLE:
            if (playerPos->x > position->x) {
                state->facingRight = true;
                sprite->flipX = false;
            } else {
                state->facingRight = false;
                sprite->flipX = true;
            }
            break;
        case Action::PATROLLING:
            if (!patrol) {
                std::cerr << "patrolling enemy with no patrol" << std::endl;
            }
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
