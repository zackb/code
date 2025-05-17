#pragma once

#include "components/Component.h"
#include "components/Sprite.h"
#include <memory>

struct Attack : Component {
    std::shared_ptr<Sprite> sprite;
    int cooldownMs;
    int attackRange;
};

struct Projectile : Component {
    int lifetimeMs;
    int ageMs = 0;
};
