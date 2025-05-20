#include "systems/StateSystem.h"

#include "components/Attack.h"
#include "components/InputControl.h"
#include "components/Knockback.h"
#include "components/SoundEffect.h"
#include "components/State.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "entity/EntityFactory.h"

void StateSystem::update(const std::shared_ptr<Entities>& entities, const int dt) {

    for (auto& entity : *entities) {

        auto state = entity->getComponent<State>();
        auto velocity = entity->getComponent<Velocity>();
        auto transform = entity->getComponent<Transform>();
        auto input = entity->getComponent<InputControl>();

        // decay knockbacks
        auto knockback = entity->getComponent<Knockback>();
        if (knockback) {
            knockback->duration -= dt;
            if (knockback->duration <= 0) {
                entities->removeComponent<Knockback>(entity);
            }
        }

        if (!state || !velocity || !transform) {
            continue;
        }

        // countdown action lock timer
        if (state->isActionLocked) {
            state->actionTimeMs += dt;
            if (state->actionTimeMs >= state->actionDurationMs) {
                state->isActionLocked = false;
                if (state->onUnlock) {
                    state->onUnlock();
                    state->onUnlock = nullptr; // clear it after use
                }
            }
        } else if (input) {
            // not action locked so we can transition if needed
            if (input->isDown(InputKey::ATTACK)) {
                if (auto attack = entity->getComponent<Attack>()) {
                    // TODO: sword animation duration
                    state->lockAction(Action::ATTACKING, 600);
                    entity->addComponent<SoundEffect>(attack->sound, 0);

                    // check if we should fire a projectile
                    if (attack->type == AttackType::RANGE) {
                        entities->queueAdd(EntityFactory::spawnProjectile(*entity, *attack));
                    }
                }
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
