#pragma once

#include "Entity.h"
#include "Size.h"

#include <vector>
#include <memory>

class MovementSystem {
public:
    static int groundLevel(const Size &windowSize) {
        return static_cast<int>(windowSize.height * 0.9f);
    }

    void update(const std::vector<std::shared_ptr<Entity> > &entities) const;
};
