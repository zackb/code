#include "corex/systems/EnemyAISystem.h"

#include "corex/components/DelayedAction.h"
#include "corex/components/Knockback.h"
#include "corex/components/SoundEffect.h"
#include "corex/components/Tag.h"
#include "corex/entity/EntityFactory.h"

// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities>& entities, const int dt) const {

    auto player = entities->findEntityWithComponent<PlayerTag>();
    if (!player) {
        return;
    }
    auto playerPos = player->getComponent<Transform>();

    // iterate enemies
    for (const auto& entity : entities->filtered<EnemyTag, Transform, Velocity, State, Sprite, EnemyAI>()) {

        if (entity->hasComponent<Knockback>())
            continue;

        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto state = entity->getComponent<State>();
        auto sprite = entity->getComponent<Sprite>();
        auto ai = entity->getComponent<EnemyAI>();

        // reset velocity to 0 until we decide what to do next
        velocity->vx = 0;

        // if the enemy is dying there's nothing to do
        if (state->currentAction == Action::DYING) {
            continue;
        }

        // if we're locked in the previous action nothing to do
        if (state->isActionLocked) {
            continue;
        }

        // if the player is already dead we can chill
        if (auto playerState = player->getComponent<State>(); playerState) {
            if (playerState->currentAction == Action::DYING) {
                // nothing to do
                state->currentAction = Action::IDLE;
                continue;
            }
        }

        auto attack = entity->getComponent<Attack>();

        // always wait the timer for determining if we should attack again
        ai->timeSinceLastAttack += dt;

        // check if we should attack
        if (attack && seesTarget(*playerPos, *position, *attack, state->facingRight)) {

            // check if we can attack
            if (ai->timeSinceLastAttack >= attack->cooldownMs) {
                // we may attack
                if (attack->type == AttackType::RANGE) {
                    // schedule a projectile to fire part way through the animation
                    const auto origin = entity;
                    const auto attackCopy = *attack;
                    entity->addComponent<DelayedAction>(500, // TODO: same here
                                                        [=]() {
                                                            entities->queueAdd(
                                                                EntityFactory::spawnProjectile(*origin, attackCopy));
                                                            entity->addComponent<SoundEffect>(attack->sound, 0);
                                                        });
                }

                ai->timeSinceLastAttack = 0;
                // Lock action for the attack animation duration
                // TODO: these should be defined in the attack fab (animation duration)
                state->lockAction(Action::ATTACKING, 1000);
            } else {
                // we cant attack so cooldown
                state->currentAction = Action::IDLE;
            }
        } else {
            // otherwise determine standard behavior
            performBehavior(*ai, *sprite, *state, *position, *velocity, *playerPos);
        }
    }

    // flush queue because we may have spawned a projectile
    entities->flushQueue();
}

void EnemyAISystem::performBehavior(
    EnemyAI& ai, Sprite& sprite, State& state, Transform& position, Velocity& velocity, Transform& playerPos) const {

    switch (ai.behavior) {
    case EnemyBehavior::IDLE: {
        state.currentAction = Action::IDLE;
        if (playerPos.x > position.x) {
            state.facingRight = true;
            sprite.flipX = false;
        } else {
            state.facingRight = false;
            sprite.flipX = true;
        }
        break;
    }
    case EnemyBehavior::PATROL: {
        auto patrol = ai.patrol;
        state.currentAction = Action::WALKING;
        if (state.facingRight) {
            velocity.vx = patrol.speed;
            if (position.x >= patrol.right) {
                state.facingRight = false;
                sprite.flipX = true;
            }
        } else {
            velocity.vx = -patrol.speed;
            if (position.x <= patrol.left) {
                state.facingRight = true;
                sprite.flipX = false;
            }
        }
        break;
    }
    case EnemyBehavior::CHASE: {
        state.currentAction = Action::WALKING;
        if (playerPos.x < position.x) {
            state.facingRight = false;
            sprite.flipX = true;
            velocity.vx = -ai.chase.speed;
        } else {
            state.facingRight = true;
            sprite.flipX = false;
            velocity.vx = ai.chase.speed;
        }
        break;
    }
    default:
        std::cerr << "unknown enemy action" << std::endl;
        break;
    }
}

bool EnemyAISystem::seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const {
    static int verticalThreshold = 50;
    bool inRange = std::abs(playerPos.x - enemyPos.x) <= attack.attackRange;
    bool onSameY = std::abs(playerPos.y - enemyPos.y) <= verticalThreshold;
    bool inFront = (facingRight && playerPos.x > enemyPos.x) || (!facingRight && playerPos.x < enemyPos.x);

    return inRange && onSameY && inFront;
}
