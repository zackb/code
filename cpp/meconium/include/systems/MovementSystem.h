#pragma once

#include "entity/Entity.h"

#include <memory>
#include <vector>

class MovementSystem {
public:
    void update(const std::shared_ptr<Entities>& entities) const;
};
