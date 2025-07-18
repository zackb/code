#pragma once
#include "corex/components/Attack.h"
#include "corex/entity/Entity.h"

class ProjectileSystem {

public:
    void update(Entities& entities, const int dt) const {
        for (auto& proj : entities.filtered<Projectile>()) {
            auto p = proj->getComponent<Projectile>();
            p->ageMs += dt;
            if (p->ageMs >= p->lifetimeMs) {
                entities.queueRemove(proj);
            }
        }
        entities.flushQueue();
    }
};
