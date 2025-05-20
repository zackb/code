#include "entity/EntityFactory.h"

#include "components/Attack.h"
#include "components/Collider.h"
#include "components/Health.h"
#include "components/NoGravity.h"
#include "components/SoundEffect.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"

std::shared_ptr<Entity> EntityFactory::spawnEnemy(const std::shared_ptr<Enemy>& enemy,
                               const std::shared_ptr<Level>& level) {
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
    attack.sprite = AssetLoader::loadSpriteSheet(enemy->def.attack->sprite);
    attack.attackRange = enemy->def.attack->range;
    attack.sound = enemy->def.attack->sound;
    attack.damage = enemy->def.attack->damage;

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

    entity->addComponent<Health>(100);
    entity->addComponent<EnemyTag>();
    return entity;
}
std::shared_ptr<Entity> EntityFactory::spawnProjectile(const std::shared_ptr<Level>& level,
                                                       Entity& shooter,
                                                       const Attack& attack) {
    auto sprite = level->createSprite(*attack.sprite);
    auto projectile = std::make_shared<Entity>();
    // Set initial position near shooter
    auto shooterPos = shooter.getComponent<Transform>();
    auto shooterSprite = shooter.getComponent<Sprite>();
    float direction = shooter.getComponent<State>()->facingRight ? 1.0f : -1.0f;

    Velocity vel(direction * sprite->speed, 0.0f);

    if (vel.vx < 0) {
        sprite->flipX = true;
    }

    projectile->addComponent<Transform>(
        shooterPos->x, shooterPos->y + shooterSprite->height / 2 + sprite->height, attack.sprite->scale);
    projectile->addComponent<Velocity>(vel);
    projectile->addComponent<NoGravity>();
    projectile->addComponent<Sprite>(sprite);
    projectile->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    projectile->addComponent<Projectile>(sprite->lifetimeMs, attack.damage);

    // play arrow firing sound
    shooter.addComponent<SoundEffect>(attack.sound, 0);
    return projectile;
}

