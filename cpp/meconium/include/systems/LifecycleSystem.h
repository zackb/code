#pragma once

#include "components/Despawn.h"
#include "entity/Entity.h"

class LifecycleSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const int deltaTime) const {

        std::vector<std::shared_ptr<Entity>> toRemove;

        for (auto& entity : *entities) {
            auto despawn = entity->getComponent<Despawn>();
            if (despawn) {
                despawn->timer -= deltaTime;
                if (despawn->timer <= 0) {
                    toRemove.push_back(entity);
                }
            }
        }

        for (const auto e : toRemove) {
            entities->remove(e);
        }
    }
};
