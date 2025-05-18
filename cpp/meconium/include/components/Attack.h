#pragma once

#include "assets/Definitions.h"
#include "components/Component.h"
#include <memory>

struct Attack : Component {
    std::shared_ptr<SpriteSheetDefinition> sprite;
    int cooldownMs;
    int attackRange;
};

struct Projectile : Component {
    int lifetimeMs;
    int ageMs = 0;
};
