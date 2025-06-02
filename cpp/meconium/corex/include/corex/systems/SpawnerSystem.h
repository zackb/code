#pragma once

#include "corex/entity/Entity.h"
#include "corex/level/Enemy.h"

class SpawnerSystem {

public:
    void update(Entities& entities, const Enemies& enemies) const;
};
