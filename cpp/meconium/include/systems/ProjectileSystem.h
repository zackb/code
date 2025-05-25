#pragma once
#include "components/Attack.h"
#include "entity/Entity.h"

#include <memory>

class ProjectileSystem {

public:
    void update(Entities& entities, const int dt) const {
        for (auto& proj : entities.findByComponents<Projectile>()) {
            auto p = proj->getComponent<Projectile>();
            p->ageMs += dt;
            if (p->ageMs >= p->lifetimeMs) {
                entities.remove(proj);
            }
        }
    }
};
