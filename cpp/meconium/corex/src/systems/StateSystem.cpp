#include "corex/systems/StateSystem.h"

#include "corex/components/Attack.h"
#include "corex/components/DelayedAction.h"
#include "corex/components/InputControl.h"
#include "corex/components/Knockback.h"
#include "corex/components/SoundEffect.h"
#include "corex/components/State.h"
#include "corex/components/Transform.h"
#include "corex/components/Velocity.h"
#include "corex/entity/EntityFactory.h"

void StateSystem::update(const std::shared_ptr<Entities>& entities, const int dt) {

    for (auto& entity : entities->filtered<State, Velocity, Transform>()) {

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

        // countdown action lock timer
        if (state->isActionLocked) {
            state->actionTimeMs += dt;
            state->checkActionLock();
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
                        entity->addComponent<DelayedAction>(
                            duration / 2, // TODO: same here
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
