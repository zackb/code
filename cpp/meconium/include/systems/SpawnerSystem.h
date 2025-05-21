#pragma once

#include "entity/Entity.h"
#include "level/Enemy.h"
#include <memory>

class SpawnerSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const Enemies& enemies) const;
};
