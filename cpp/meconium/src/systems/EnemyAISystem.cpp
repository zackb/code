#include "systems/EnemyAISystem.h"

#include "components/Attack.h"
#include "components/EnemyAI.h"
#include "components/Knockback.h"
#include "components/SoundEffect.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include "entity/EntityFactory.h"

// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities>& entities,
                           const std::shared_ptr<Level>& level,
                           const int dt) const {

    for (const auto& entity : *entities) {
        if (!entity->hasComponent<EnemyTag>())
            continue;

        if (entity->hasComponent<Knockback>())
            continue;

        auto position = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto state = entity->getComponent<State>();
        auto sprite = entity->getComponent<Sprite>();

        auto player = entities->findEntityWithComponent<PlayerTag>();
        auto playerPos = player->getComponent<Transform>();

        if (!position || !velocity || !state || !sprite) {
            std::cerr << "missing required components of enemy" << std::endl;
            continue;
        }

        // if the enemy is dying there's nothing to do
        if (state && state->currentAction == Action::DYING) {
            velocity->vx = 0;
            continue;
        }

        // if the player is already dead we can chill
        bool playerIsDead = false;
        if (auto playerState = player->getComponent<State>(); playerState) {
            if (playerState->currentAction == Action::DYING) {
                playerIsDead = true;
            }
        }

        // see if we have ai
        auto ai = entity->getComponent<EnemyAI>();
        if (!ai) {
            std::cerr << "enemy has no ai" << std::endl;
            continue;
        }

        auto attack = entity->getComponent<Attack>();

        // check for pending projectiles
        if (attack && ai->projectilePending && state->actionTimeMs >= ai->scheduledProjectileTime) {
            entities->queueAdd(EntityFactory::spawnProjectile(level, *entity, *attack));

            // play attack sound
            entity->addComponent<SoundEffect>(attack->sound, 0);

            ai->projectilePending = false;
        }

        // always wait the timer for determining if we should attack again
        ai->timeSinceLastAttack += dt;

        velocity->vx = 0;
        // check if we should attack
        if (!state->isActionLocked) {
            if (attack && !playerIsDead && seesTarget(*playerPos, *position, *attack, state->facingRight)) {

                if (ai->timeSinceLastAttack >= attack->cooldownMs) {
                    // schedule a projectile to fire part way through the animation

                    ai->scheduledProjectileTime = 500; // spawn projectile at 500ms into animation
                    ai->projectilePending = true;

                    ai->timeSinceLastAttack = 0;
                    // Lock action for the attack animation duration
                    state->lockAction(Action::ATTACKING, 1000);
                } else {
                    state->currentAction = Action::IDLE;
                }
            } else {
                // otherwise determine standard behavior
                switch (ai->behavior) {
                case EnemyBehavior::IDLE: {
                    state->currentAction = Action::IDLE;
                    if (playerPos->x > position->x) {
                        state->facingRight = true;
                        sprite->flipX = false;
                    } else {
                        state->facingRight = false;
                        sprite->flipX = true;
                    }
                    break;
                }
                case EnemyBehavior::PATROL: {
                    auto patrol = ai->patrol;
                    state->currentAction = Action::WALKING;
                    if (state->facingRight) {
                        velocity->vx = patrol.speed;
                        if (position->x >= patrol.right) {
                            state->facingRight = false;
                            sprite->flipX = true;
                        }
                    } else {
                        velocity->vx = -patrol.speed;
                        if (position->x <= patrol.left) {
                            state->facingRight = true;
                            sprite->flipX = false;
                        }
                    }
                    break;
                }
                default:
                    std::cerr << "unknown enemy action" << std::endl;
                    break;
                }
            }
        }
    }

    // flush queue
    entities->flushQueue();
}

bool EnemyAISystem::seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const {
    static int verticalThreshold = 50;
    bool inRange = std::abs(playerPos.x - enemyPos.x) <= attack.attackRange;
    bool onSameY = std::abs(playerPos.y - enemyPos.y) <= verticalThreshold;
    bool inFront = (facingRight && playerPos.x > enemyPos.x) || (!facingRight && playerPos.x < enemyPos.x);

    return inRange && onSameY && inFront;
}