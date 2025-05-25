#pragma once

#include "entity/Entity.h"
#include <memory>

class CombatSystem {
public:
    void update(const std::shared_ptr<Entities>& entities);

private:
    void resolvePlayerEnemyCollisions(Entity& player, Entity& enemy);

    void resolvePlayerProjectileCollisions(Entity& player, Entity& projectile);

    void resolvePlayerPickupCollisions(Entity& player, Entity& pickup);
};
