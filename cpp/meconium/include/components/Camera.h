#pragma once

#include "entity/Entity.h"

#include <memory>

struct Camera {
    int viewportWidth;
    int viewportHeight;
};

struct Follow {
    std::weak_ptr<Entity> target; // the entity this camera follows
    float lerpFactor = 1.0f;      // 1.0 = instant, 0.1 = slow follow
};