#pragma once

#include "assets/Definitions.h"
#include "components/Component.h"
#include <memory>

struct Attack : Component {
    std::shared_ptr<SpriteSheetDefinition> sprite;
    int cooldownMs;
    int damage;
    int attackRange;
};

struct Projectile : Component {
    int lifetimeMs;
    int damage;
    int ageMs = 0;

    Projectile(int lifetimeMs) : lifetimeMs(lifetimeMs) {}
    Projectile(int lifetimeMs, int damage) : lifetimeMs(lifetimeMs), damage(damage) {}
};
