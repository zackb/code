#pragma once

#include "ECS.h"
#include "level/Level.h"

#include <memory>

class SpawnerSystem {
public:
    void update(const std::shared_ptr<Entities>& entities,
                const Enemies& enemies,
                const std::shared_ptr<Level>& level) const;
};
