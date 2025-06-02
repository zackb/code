#include "corex/systems/SpawnerSystem.h"
#include "corex/components/Tag.h"
#include "corex/components/Transform.h"
#include "corex/entity/EntityFactory.h"
#include <iostream>

void SpawnerSystem::update(Entities& entities, const Enemies& enemies) const {

    auto player = entities.findEntityWithComponent<PlayerTag>();

    if (!player) {
        std::cerr << "no player found" << std::endl;
        return;
    }

    auto pos = player->getComponent<Transform>();

    for (auto enemy : enemies) {
        if (enemy->hasSpawned)
            continue;
        if (pos->x >= enemy->def.triggerX) {
            entities.queueAdd(EntityFactory::spawnEnemy(enemy));
            enemy->hasSpawned = true;
        }
    }
    entities.flushQueue();
}
