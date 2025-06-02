#pragma once

#include "corex/assets/Definitions.h"
#include "corex/components/Component.h"
#include <memory>

enum class AttackType { MELEE, RANGE };

struct Attack final : Component {
    AttackType type;
    std::shared_ptr<SpriteSheetDefinition> sprite;
    int cooldownMs;
    int damage;
    int attackRange;
    std::string sound;
};

struct Projectile final : Component {
    int ownerId;
    int lifetimeMs;
    int damage;
    int ageMs = 0;

    Projectile(const int ownerId, const int lifetimeMs) : ownerId(ownerId), lifetimeMs(lifetimeMs) {}
    Projectile(const int ownerId, const int lifetimeMs, const int damage)
        : ownerId(ownerId), lifetimeMs(lifetimeMs), damage(damage) {}
};
