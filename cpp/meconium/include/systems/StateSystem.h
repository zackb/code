#pragma once

#include "entity/Entity.h"

class StateSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, int dt);
};
