#include "corex/entity/EntityFactory.h"

#include "corex/ResourceManager.h"
#include "corex/assets/AssetLoader.h"
#include "corex/assets/Definitions.h"
#include "corex/components/Attack.h"
#include "corex/components/Collider.h"
#include "corex/components/Health.h"
#include "corex/components/Interactable.h"
#include "corex/components/LevelComponent.h"
#include "corex/components/NoGravity.h"
#include "corex/components/State.h"
#include "corex/components/Tag.h"
#include "corex/components/Transform.h"
#include "corex/components/Velocity.h"
#include <iostream>

std::shared_ptr<Entity> EntityFactory::spawnEnemy(const std::shared_ptr<Enemy>& enemy) {
    auto entity = std::make_shared<Entity>();
    auto sheet = enemy->spriteSheet;
    auto sprite = createSprite(*sheet);
    auto animation = createAnimation(*sheet);

    EnemyAI ai;
    ai.behavior = enemy->def.behavior;
    if (enemy->def.patrol.has_value()) {
        auto patrol = enemy->def.patrol.value();
        ai.patrol = Patrol(patrol.left, patrol.right, patrol.speed);
    } else if (enemy->def.chase.has_value()) {
        auto chase = enemy->def.chase.value();
        ai.chase = Chase(chase.speed);
    }

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

    projectile->addComponent<Velocity>(direction * sprite->speed, 0.0f);

    // sprites always face right, check if we need to flip
    if (!state->facingRight) {
        sprite->flipX = true;
    }

    // int projectileX = shooterPos->x + (state->facingRight ? shooterSprite->width : -shooterSprite->width) *
    // shooterPos->scaleX;
    projectile->addComponent<Transform>(
        shooterPos->x, shooterPos->y + shooterSprite->height / 2 + sprite->height, attack.sprite->scale);
    projectile->addComponent<NoGravity>();
    projectile->addComponent<Sprite>(sprite);
    projectile->addComponent<Collider>(attack.sprite->collider->x,
                                       attack.sprite->collider->y,
                                       attack.sprite->collider->width,
                                       attack.sprite->collider->height);
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

std::shared_ptr<Entity> EntityFactory::createInteractable(const InteractableDefinition& i) {
    Entity entity;
    auto spriteSheet = AssetLoader::loadSpriteSheet(i.sprite);
    auto sprite = createSprite(i.sprite);
    entity.addComponent<Sprite>(sprite);
    entity.addComponent<Transform>(i.position.x, i.position.y, spriteSheet->scale);
    entity.addComponent<Velocity>();
    // TODO: add collider from prefab
    entity.addComponent<Collider>(0, 0, sprite->width, sprite->height);

    // TODO: add Tween from prefab?

    // add the action (GrantHealth, AddToBag, etc)
    Interactable::Type type = Interactable::Type::None;
    std::visit(
        [&](auto&& action) {
            using T = std::decay_t<decltype(action)>;
            if constexpr (std::is_same_v<T, GrantHealth>) {
                type = Interactable::Type::Pickup;
                entity.addComponent<GrantHealth>(action);
            } else if constexpr (std::is_same_v<T, AddToBag>) {
                type = Interactable::Type::Pickup;
                entity.addComponent<AddToBag>(action);
            } else if constexpr (std::is_same_v<T, OpenDoor>) {
                type = Interactable::Type::Door;
                entity.addComponent<OpenDoor>(action);
            }
        },
        i.action);

    entity.addComponent<Interactable>(type);
    entity.addComponent<State>();
    entity.addComponent<AnimationComponent>(createAnimation(*spriteSheet));

    return std::make_shared<Entity>(entity);
}

std::shared_ptr<AnimationComponent> EntityFactory::createAnimation(const SpriteSheetDefinition& spriteDef) {
    auto animComponent = std::make_shared<AnimationComponent>();
    for (auto it : spriteDef.animations) {
        auto anim = std::make_shared<Animation>(it.looping);
        for (int i = 0; i < it.frameCount; i++) {
            anim->addFrame({spriteDef.tileWidth * (i + it.rowOffset),
                            it.row * spriteDef.tileHeight,
                            spriteDef.tileWidth,
                            spriteDef.tileHeight},
                           it.duration);
        }

        AnimationState state = AnimationState::IDLE;
        if (it.name == "idle")
            state = AnimationState::IDLE;
        else if (it.name == "walk")
            state = AnimationState::WALKING;
        else if (it.name == "jump")
            state = AnimationState::JUMPING;
        else if (it.name == "fall")
            state = AnimationState::FALLING;
        else if (it.name == "attack")
            state = AnimationState::ATTACKING;
        else if (it.name == "collect")
            state = AnimationState::COLLECTING;
        else if (it.name == "die")
            state = AnimationState::DYING;
        else if (it.name == "open")
            state = AnimationState::OPENING;
        else
            std::cerr << "Unknown animation state: " << it.name << std::endl;

        animComponent->addAnimation(state, anim);
    }

    animComponent->init();
    return animComponent;
}
