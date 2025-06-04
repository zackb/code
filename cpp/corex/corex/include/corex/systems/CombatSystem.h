#pragma once

#include "corex/entity/Entity.h"
#include <memory>

class CombatSystem {
public:
    void update(const std::shared_ptr<Entities>& entities);

private:
    void resolvePlayerEnemyCollisions(Entity& player, Entity& enemy);

    void resolvePlayerProjectileCollisions(Entity& player, Entity& projectile);
};
