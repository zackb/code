#pragma once

#include "entity/Entity.h"
#include <memory>

class AnimationSystem {

public:
    void update(Entities& entities, const int deltaTime) const;
};
