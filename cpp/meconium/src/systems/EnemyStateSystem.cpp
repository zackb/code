//
// Created by Zack Bartel on 5/15/25.
//

#include "systems/EnemyStateSystem.h"

#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"

void EnemyStateSystem::update(const std::shared_ptr<Entities> &entities) {
    auto player = entities->findEntityWithComponent<PlayerTag>();
    auto playerPos = player->getComponent<Transform>();
    for (auto& entity : *entities) {
        if (!entity->hasComponent<EnemyTag>())
            continue;

        auto state = entity->getComponent<State>();
        auto pos = entity->getComponent<Transform>();
        auto sprite = entity->getComponent<Sprite>();
        // assume all sprites face right
        if (playerPos->x > pos->x) {
            state->facingRight = true;
            sprite->flipX = false;
        } else {
            state->facingRight = false;
            sprite->flipX = true;
        }
    }
}
