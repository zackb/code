#include "corex/systems/DelayedActionSystem.h"

#include "corex/components/DelayedAction.h"

void DelayedActionSystem::update(Entities& entities, const int dt) {

    std::vector<ActionFn> actionQueue;

    for (auto& entity : entities.filtered<DelayedAction>()) {
        auto delayedAction = entity->getComponent<DelayedAction>();
        delayedAction->remainingMs -= dt;
        if (delayedAction->remainingMs <= 0) {
            actionQueue.push_back(delayedAction->action);
            entities.removeComponent<DelayedAction>(entity);
        }
    }

    // execute these delayed actions after to avoid modifying the entity list while iterating
    for (auto action : actionQueue) {
        action();
    }
    entities.flushQueue();
}
