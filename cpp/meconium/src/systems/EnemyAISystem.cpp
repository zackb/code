#include "systems/EnemyAISystem.h"

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
        // TODO: parameterize patrol bounds
        // "x": 1200,
        // "y": 1200,
        switch (state->currentAction) {
        case Action::IDLE:
            // TODO: direction should be centralized (Facing component?)
            // to support sprites facing different directions
            if (playerPos->x > position->x) {
                state->facingRight = true;
                sprite->flipX = false;
            } else {
                state->facingRight = false;
                sprite->flipX = true;
            }
            break;
        case Action::PATROLLING:
            if (state->facingRight) {
                velocity->vx = 2; // TODO: prefab?
                if (position->x >= 1400) {
                    state->facingRight = false;
                    sprite->flipX = true;
                }
            } else {
                velocity->vx = -2;
                if (position->x <= 800) {
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
