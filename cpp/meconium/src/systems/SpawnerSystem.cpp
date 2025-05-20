#include "systems/SpawnerSystem.h"
#include "assets/AssetLoader.h"
#include "components/Attack.h"
#include "components/EnemyAI.h"
#include "components/Health.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "entity/EntityFactory.h"

#include <iostream>

void SpawnerSystem::update(const std::shared_ptr<Entities>& entities,
                           const Enemies& enemies,
                           const std::shared_ptr<Level>& level) const {

    auto player = entities->findEntityWithComponent<PlayerTag>();

    if (!player) {
        std::cerr << "no player found" << std::endl;
        return;
    }

    auto pos = player->getComponent<Transform>();

    for (auto enemy : enemies) {
        if (enemy->hasSpawned)
            continue;
        if (pos->x >= enemy->def.triggerX) {
            entities->queueAdd(EntityFactory::spawnEnemy(enemy, level));
            enemy->hasSpawned = true;
        }
    }
    entities->flushQueue();
}