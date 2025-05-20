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
    auto sprite = createSprite(*sheet);
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

    entity->addComponent<Attack>(createAttack(*enemy->def.attack));

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

std::shared_ptr<Entity> EntityFactory::spawnProjectile(Entity& shooter, const Attack& attack) {
    auto sprite = createSprite(*attack.sprite);
    auto projectile = std::make_shared<Entity>();
    // Set initial position near shooter
    auto shooterPos = shooter.getComponent<Transform>();
    auto shooterSprite = shooter.getComponent<Sprite>();
    auto state = shooter.getComponent<State>();
    float direction = state->facingRight ? 1.0f : -1.0f;

    Velocity vel(direction * sprite->speed, 0.0f);

    // sprites always face right, check if we need to flip
    if (!state->facingRight) {
        sprite->flipX = true;
    }

    // int projectileX = shooterPos->x + (state->facingRight ? shooterSprite->width : -shooterSprite->width) * shooterPos->scaleX;
    projectile->addComponent<Transform>(
        shooterPos->x, shooterPos->y + shooterSprite->height / 2 + sprite->height, attack.sprite->scale);
    projectile->addComponent<Velocity>(vel);
    projectile->addComponent<NoGravity>();
    projectile->addComponent<Sprite>(sprite);
    projectile->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    projectile->addComponent<Projectile>(shooter.id, sprite->lifetimeMs, attack.damage);
    return projectile;
}

std::shared_ptr<Attack> EntityFactory::createAttack(const AttackDefinition& def) {
    auto attack = std::make_shared<Attack>();
    if (!def.sprite.empty()) {
        attack->sprite = AssetLoader::loadSpriteSheet(def.sprite);
    }
    attack->type = def.type == "range" ? AttackType::RANGE : AttackType::MELEE;
    attack->cooldownMs = def.cooldownMs;
    attack->attackRange = def.range;
    attack->sound = def.sound;
    attack->damage = def.damage;
    return attack;
}

std::shared_ptr<Sprite> EntityFactory::createSprite(const std::string& playerPath) {
    return createSprite(*AssetLoader::loadSpriteSheet(playerPath));
}

std::shared_ptr<Sprite> EntityFactory::createSprite(const SpriteSheetDefinition& spriteDef) {
    Sprite sprite;
    sprite.texture = ResourceManager::loadTexture("assets/" + spriteDef.texture);
    sprite.height = spriteDef.tileHeight;
    sprite.width = spriteDef.tileWidth;
    sprite.speed = spriteDef.speed;
    sprite.lifetimeMs = spriteDef.lifetimeMs;
    return std::make_shared<Sprite>(sprite);
}
