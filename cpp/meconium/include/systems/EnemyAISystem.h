#pragma once

#include "Level.h"
#include "components/Attack.h"
#include "components/Transform.h"
#include "entity/Entity.h"

class EnemyAISystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const std::shared_ptr<Level>& level, const int dt) const;

private:
    bool seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const;
    std::shared_ptr<Entity> spawnProjectile(Entities& entities,
                                            const std::shared_ptr<Level>& level,
                                            Entity& shooter,
                                            const Attack& attack) const;
};
