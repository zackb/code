#include "systems/SpawnerSystem.h"
#include "components/Attack.h"
#include "components/EnemyAI.h"
#include "components/Sprite.h"
#include "components/State.h"
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
        if (pos->x >= enemy->def.triggerX) {
            spawnEnemy(entities, enemy, level);
            enemy->hasSpawned = true;
        }
    }
}

void SpawnerSystem::spawnEnemy(const std::shared_ptr<Entities>& entities,
                               const std::shared_ptr<Enemy>& enemy,
                               const std::shared_ptr<Level>& level) const {
    auto entity = std::make_shared<Entity>();
    auto sheet = enemy->spriteSheet;
    auto sprite = level->createSprite(*sheet);
    auto animation = level->createAnimation(*sheet);

    EnemyAI ai;
    ai.behavior = enemy->def.behavior;
    if (enemy->def.patrol.has_value()) {
        auto patrol = enemy->def.patrol.value();
        ai.patrol = Patrol(patrol.left, patrol.right, patrol.speed);
    }
    // TODO: chase, idle

    entity->addComponent<AnimationComponent>(animation);
    entity->addComponent<Sprite>(sprite);
    entity->addComponent<State>();
    entity->addComponent<EnemyAI>(ai);

    Attack attack;
    attack.cooldownMs = enemy->def.attack->cooldownMs;
    attack.sprite = level->createSprite(enemy->def.attack->sprite);
    attack.attackRange = 300; // TODO

    entity->addComponent<Attack>(attack);

    entity->addComponent<Transform>(enemy->def.x, enemy->def.y, sheet->scale);
    entity->getComponent<Transform>()->onGround = false;
    entity->addComponent<Velocity>();
    if (sheet->collider.has_value()) {
        auto rect = sheet->collider.value();
        entity->addComponent<Collider>(rect.x, rect.y, rect.width, rect.height);
    } else {
        entity->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    }

    entity->addComponent<EnemyTag>();
    entities->add(entity);
}
