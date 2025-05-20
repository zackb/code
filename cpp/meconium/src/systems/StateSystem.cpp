#include "systems/StateSystem.h"

#include "components/Attack.h"
#include "components/DelayedAction.h"
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

                    // TODO: need to get this from the attack and animation
                    int duration = attack->type == AttackType::MELEE ? 600 : 1000;
                    state->lockAction(Action::ATTACKING, duration);

                    // check if we should fire a projectile
                    if (attack->type == AttackType::RANGE) {
                        // defer firing the projectile untile we're halfway through the animation
                        const auto origin = entity;
                        const auto attackCopy = *attack;
                        entity->addComponent<DelayedAction>(duration / 2, // TODO: same here
                        [=]() {
                            entities->queueAdd(EntityFactory::spawnProjectile(*origin, attackCopy));
                            entity->addComponent<SoundEffect>(attack->sound, 0);
                        });
                    } else { // melee
                        entity->addComponent<SoundEffect>(attack->sound, 0);
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
