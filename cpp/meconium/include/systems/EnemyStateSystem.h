#pragma once

#include "entity/Entity.h"

class EnemyStateSystem {
public:
    void update(const std::shared_ptr<Entities> &entities);
};
