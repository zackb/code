#pragma once

#include "entity/Entity.h"

#include <memory>
#include <vector>

class MovementSystem {
public:
    void update(const std::vector<std::shared_ptr<Entity>>& entities) const;
};
