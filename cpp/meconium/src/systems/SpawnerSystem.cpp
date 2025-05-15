#include "systems/SpawnerSystem.h"
#include "assets/AssetLoader.h"
#include "components/Tag.h"

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
        if (pos->x >= enemy->triggerX) {
            spawnEnemy(entities, enemy, level);
            enemy->hasSpawned = true;
        }
    }
}

void SpawnerSystem::spawnEnemy(const std::shared_ptr<Entities>& entities,
                               const std::shared_ptr<Enemy>& enemy,
                               const std::shared_ptr<Level>& level) const {
    auto entity = std::make_shared<Entity>(99);
    auto sprite = level->createSprite(enemy->sprite);
    auto animation = level->createAnimation(*enemy->sprite);

    entity->addComponent<Sprite>(sprite);
    entity->addComponent<AnimationComponent>(animation);
    entity->addComponent<Transform>(enemy->x, enemy->y);
    entities->add(entity);
}
