#pragma once

#include "Entity.h"
#include <memory>
#include <vector>

class AnimationSystem {

public:
    void update(const std::vector<std::shared_ptr<Entity>>& entities, const int deltaTime);
};
