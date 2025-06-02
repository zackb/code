#pragma once

#include "corex/entity/Entity.h"

class AnimationSystem {

public:
    void update(Entities& entities, const int deltaTime) const;
};
