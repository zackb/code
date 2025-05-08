#pragma once

#include "Entity.h"
#include "Size.h"

#include <memory>
#include <vector>

class MovementSystem {
public:
    void update(const std::vector<std::shared_ptr<Entity>>& entities) const;
};
