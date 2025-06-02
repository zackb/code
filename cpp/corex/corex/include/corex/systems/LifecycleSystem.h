#pragma once

#include "corex/components/Despawn.h"
#include "corex/entity/Entity.h"

class LifecycleSystem {

public:
    void update(Entities& entities, const int deltaTime) const {

        for (auto& entity : entities.filtered<Despawn>()) {
            auto despawn = entity->getComponent<Despawn>();
            despawn->timer -= deltaTime;
            if (despawn->timer <= 0) {
                // entity should despawn add to removal queue
                entities.queueRemove(entity);
            }
        }

        entities.flushQueue();
    }
};
