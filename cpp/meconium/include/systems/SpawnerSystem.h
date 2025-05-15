#pragma once

#include "ECS.h"
#include "Level.h"

#include <memory>

class SpawnerSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, Enemies& enemies) const;
};
