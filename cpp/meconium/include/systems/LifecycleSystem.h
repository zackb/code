#pragma once

#include "components/Despawn.h"
#include "entity/Entity.h"

class LifecycleSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, const int deltaTime) const {

        for (auto& entity : *entities) {
            auto despawn = entity->getComponent<Despawn>();
            if (despawn) {
                despawn->timer -= deltaTime;
                if (despawn->timer <= 0) {
                    // entity should despawn add to removal queue
                    entities->queueRemove(entity);
                }
            }
        }

        entities->flushQueue();
    }
};
