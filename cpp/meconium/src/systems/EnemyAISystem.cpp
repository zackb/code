#include "systems/EnemyAISystem.h"

#include "components/Attack.h"
#include "components/EnemyAI.h"
#include "components/Knockback.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include <cstdlib>

// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities>& entities, const int dt) const {
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

        // if the enemy is dying there's nothing to do
        if (state && state->currentAction == Action::DYING) {
            velocity->vx = 0;
            continue;
        }

        // see if we have ai
        auto ai = entity->getComponent<EnemyAI>();
        if (!ai) {
            std::cerr << "enemy has no ai" << std::endl;
            continue;
        }

        // check if we should attack
        auto attack = entity->getComponent<Attack>();
        if (attack && seesTarget(*playerPos, *position, *attack, state->facingRight)) {
            ai->timeSinceLastAttack += dt;

            if (ai->timeSinceLastAttack >= attack->cooldownMs) {
                // spawnProjectile(entities, entity, attack);
                ai->timeSinceLastAttack = 0;
                state->currentAction = Action::ATTACKING;
            }
        }

        // otherwise determine standard behavior
        velocity->vx = 0;
        switch (ai->behavior) {
        case EnemyBehavior::IDLE: {
            state->currentAction = Action::IDLE;
            if (playerPos->x > position->x) {
                state->facingRight = true;
                sprite->flipX = false;
            } else {
                state->facingRight = false;
                sprite->flipX = true;
            }
            break;
        }
        case EnemyBehavior::PATROL: {
            auto patrol = ai->patrol;
            state->currentAction = Action::WALKING;
            if (state->facingRight) {
                velocity->vx = patrol.speed;
                if (position->x >= patrol.right) {
                    state->facingRight = false;
                    sprite->flipX = true;
                }
            } else {
                velocity->vx = -patrol.speed;
                if (position->x <= patrol.left) {
                    state->facingRight = true;
                    sprite->flipX = false;
                }
            }
            break;
        }
        default:
            std::cerr << "unknown enemy action" << std::endl;
            break;
        }
    }
}

bool EnemyAISystem::seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const {
    static int verticalThreshold = 50;
    bool inRange = std::abs(playerPos.x - enemyPos.x) <= attack.attackRange;
    bool onSameY = std::abs(playerPos.y - enemyPos.y) <= verticalThreshold;
    bool inFront = (facingRight && playerPos.x > enemyPos.x) || (!facingRight && playerPos.x < enemyPos.x);

    return inRange && onSameY && inFront;
}
