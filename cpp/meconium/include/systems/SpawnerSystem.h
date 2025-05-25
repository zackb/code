#pragma once

#include "entity/Entity.h"
#include "level/Enemy.h"

class SpawnerSystem {

public:
    void update(Entities& entities, const Enemies& enemies) const;
};
