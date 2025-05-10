#pragma once

#include "entity/Entity.h"
#include <memory>

class AnimationSystem {

public:
    void update(const std::shared_ptr<Entities>& entities, const int deltaTime) const;
};
