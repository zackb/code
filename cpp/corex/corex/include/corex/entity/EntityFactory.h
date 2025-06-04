#pragma once

#include "corex/assets/Definitions.h"
#include "corex/components/Animation.h"
#include "corex/components/Attack.h"
#include "corex/components/Sprite.h"
#include "corex/entity/Entity.h"
#include "corex/level/Enemy.h"
#include <memory>

class EntityFactory {
public:
    static std::shared_ptr<Entity> spawnEnemy(const std::shared_ptr<Enemy>& enemy);

    static std::shared_ptr<Entity> spawnProjectile(Entity& shooter, const Attack& attack);

    static std::shared_ptr<Attack> createAttack(const AttackDefinition& def);

    static std::shared_ptr<Sprite> createSprite(const std::string& spriteSheetPath);

    static std::shared_ptr<Sprite> createSprite(const SpriteSheetDefinition& spriteDef);

    static std::shared_ptr<Entity> createPickupEntity(const PickupDefinition& pickup);
    static std::shared_ptr<Entity> createInteractable(const InteractableDefinition& i);

    static std::shared_ptr<AnimationComponent> createAnimation(const SpriteSheetDefinition& spriteDef);
};
