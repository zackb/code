#pragma once

#include "components/Component.h"

struct Attack : Component {
    std::string projectilePrefab;
    int cooldownMs;
    int attackRange;
};

struct Projectile : Component {
    int lifetimeMs;
    int ageMs = 0;
    bool fromEnemy = true; // optional if you want friendly/enemy fire
};
