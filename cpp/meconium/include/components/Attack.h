#pragma once

#include "assets/Definitions.h"
#include "components/Component.h"
#include <memory>

struct Attack final : Component {
    std::shared_ptr<SpriteSheetDefinition> sprite;
    int cooldownMs;
    int damage;
    int attackRange;
    std::string sound;
};

struct Projectile final : Component {
    int lifetimeMs;
    int damage;
    int ageMs = 0;

    Projectile(const int lifetimeMs) : lifetimeMs(lifetimeMs) {}
    Projectile(const int lifetimeMs, const int damage) : lifetimeMs(lifetimeMs), damage(damage) {}
};
