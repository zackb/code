#pragma once

#include "assets/Definitions.h"
#include "components/Component.h"
#include <memory>

enum class AttackType {
    MELEE, RANGE
};

struct Attack final : Component {
    AttackType type;
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
