#pragma once

#include "ECS.h"
#include "Level.h"
#include "assets/Definitions.h"

#include <memory>

class SpawnerSystem {
public:
    void update(const std::shared_ptr<Entities>& entities,
                const Enemies& enemies,
                const std::shared_ptr<Level>& level) const;

private:
    void spawnEnemy(const std::shared_ptr<Entities>& entities,
                    const std::shared_ptr<Enemy>& enemy,
                    const std::shared_ptr<Level>& level) const;
};
