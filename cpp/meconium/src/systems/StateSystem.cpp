#include "systems/StateSystem.h"
#include "components/InputControl.h"
#include "components/State.h"
#include "components/Transform.h"
#include "components/Velocity.h"

void StateSystem::update(const std::shared_ptr<Entities>& entities, const int dt) {

    for (auto& entity : *entities) {

        auto state = entity->getComponent<State>();
        auto velocity = entity->getComponent<Velocity>();
        auto transform = entity->getComponent<Transform>();
        auto input = entity->getComponent<InputControl>();

        if (!state || !velocity || !transform || !input) {
            continue;
        }

        // countdown action lock timer
        if (state->isActionLocked) {
            state->actionTimeMs += dt;
            if (state->actionTimeMs >= state->actionDurationMs) {
                state->isActionLocked = false;
            }
        } else {
            // not action locked so we can transition if needed
            if (input->isDown(InputKey::ATTACK)) {
                state->currentAction = Action::ATTACKING;
                // TODO: sword animation duration
                // move this into AttackData component and load from prefab at init time
                state->actionDurationMs = 600;
                state->actionTimeMs = 0;
                state->isActionLocked = true;
            } else if (!transform->onGround) {
                state->currentAction = (velocity->vy < 0) ? Action::JUMPING : Action::FALLING;
            } else if (velocity->vx != 0) {
                state->currentAction = Action::WALKING;
            } else {
                state->currentAction = Action::IDLE;
            }
        }
    }
}
