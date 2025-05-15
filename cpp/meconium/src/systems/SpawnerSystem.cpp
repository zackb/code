#include "systems/SpawnerSystem.h"
#include "components/Tag.h"

#include <iostream>

void SpawnerSystem::update(const std::shared_ptr<Entities>& entities, Enemies& enemies) const {
    auto player = entities->findEntityWithComponent<PlayerTag>();

    if (!player) {
        std::cerr << "no player found" << std::endl;
        return;
    }

    auto pos = player->getComponent<Transform>();

    for (auto enemy : enemies) {
        if (enemy->hasSpawned)
            continue;
        if (pos->x >= enemy->triggerX) {
            // spawn
            enemy->hasSpawned = true;
        }
    }
}
