#include "systems/EnemyAISystem.h"

#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"


// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities> &entities) const {
    for (const auto& entity: *entities) {
        if (!entity->hasComponent<EnemyTag>())
            continue;

        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto state = entity->getComponent<State>();

        if (!position || !velocity || !state) {
            std::cerr << "missing required components of enemy" << std::endl;
            continue;
        }

        velocity->vx = 0;

        std::cout << entity->id << std::endl;
        std::cout << position->x << " " << position->y << std::endl;
        // TODO: parameterize patrol bounds
        // "x": 1200,
        // "y": 1200,
        switch (state->currentAction) {
            case Action::IDLE:
                break;
            case Action::PATROLLING:
                if (state->facingRight) {
                    velocity->vx = 2; // TODO: prefab?
                    if (position->x >= 1400) {
                        state->facingRight = false;
                    }
                } else {
                    velocity->vx = -2;
                    if (position->x <= 800) {
                        state->facingRight = true;
                        position->x = 800;
                    }
                }
                break;
            default:
                std::cerr << "unknown enemy action" << std::endl;
                break;
        }
    }
}
