#pragma once
#include "Entity.h"
#include "Level.h"
#include "components/Attack.h"

#include <memory>

class EntityFactory {
public:
    static std::shared_ptr<Entity> spawnEnemy(const std::shared_ptr<Enemy>& enemy, const std::shared_ptr<Level>& level);

    static std::shared_ptr<Entity>
        spawnProjectile(const std::shared_ptr<Level>& level, Entity& shooter, const Attack& attack);
};