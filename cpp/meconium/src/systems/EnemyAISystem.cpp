#include "systems/EnemyAISystem.h"

#include "assets/AssetLoader.h"
#include "components/Attack.h"
#include "components/Collider.h"
#include "components/EnemyAI.h"
#include "components/Knockback.h"
#include "components/NoGravity.h"
#include "components/Sprite.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"
#include <cstdlib>

// gravity is applied in MovementSystem
void EnemyAISystem::update(const std::shared_ptr<Entities>& entities,
                           const std::shared_ptr<Level>& level,
                           const int dt) const {

    // collect anything we need to add to entities after iteration
    std::vector<std::shared_ptr<Entity>> toAdd;

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

        // see if we have ai
        auto ai = entity->getComponent<EnemyAI>();
        if (!ai) {
            std::cerr << "enemy has no ai" << std::endl;
            continue;
        }

        // always wait the timer for determining if we should attack again
        ai->timeSinceLastAttack += dt;

        velocity->vx = 0;
        // check if we should attack
        if (!state->isActionLocked) {
            auto attack = entity->getComponent<Attack>();
            if (attack && seesTarget(*playerPos, *position, *attack, state->facingRight)) {

                if (ai->timeSinceLastAttack >= attack->cooldownMs) {
                    toAdd.push_back(spawnProjectile(*entities, level, *entity, *attack));
                    ai->timeSinceLastAttack = 0;
                    // Lock action for the attack animation duration
                    state->currentAction = Action::ATTACKING;
                    state->isActionLocked = true;
                    state->actionTimeMs = 0;
                    state->actionDurationMs = 1000; // TODO
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

    // add projectiles after loop
    for (const auto& e : toAdd) {
        entities->add(e);
    }
}

bool EnemyAISystem::seesTarget(Transform& playerPos, Transform& enemyPos, Attack& attack, bool facingRight) const {
    static int verticalThreshold = 50;
    bool inRange = std::abs(playerPos.x - enemyPos.x) <= attack.attackRange;
    bool onSameY = std::abs(playerPos.y - enemyPos.y) <= verticalThreshold;
    bool inFront = (facingRight && playerPos.x > enemyPos.x) || (!facingRight && playerPos.x < enemyPos.x);

    return inRange && onSameY && inFront;
}

std::shared_ptr<Entity> EnemyAISystem::spawnProjectile(Entities& entities,
                                                       const std::shared_ptr<Level>& level,
                                                       Entity& shooter,
                                                       const Attack& attack) const {
    std::cout << "spawning projectile\n";
    auto sprite = level->createSprite(*attack.sprite);
    auto projectile = std::make_shared<Entity>();
    // Set initial position near shooter
    auto shooterPos = shooter.getComponent<Transform>();
    auto shooterSprite = shooter.getComponent<Sprite>();
    float direction = shooter.getComponent<State>()->facingRight ? 1.0f : -1.0f;

    Velocity vel(direction * sprite->speed, 0.0f);

    if (vel.vx < 0) {
        sprite->flipX = true;
    }

    projectile->addComponent<Transform>(
        shooterPos->x, shooterPos->y + shooterSprite->height / 2 + sprite->height, attack.sprite->scale);
    projectile->addComponent<Velocity>(vel);
    projectile->addComponent<NoGravity>();
    projectile->addComponent<Sprite>(sprite);
    projectile->addComponent<Collider>(0, 0, sprite->width, sprite->height);
    Projectile p;
    p.lifetimeMs = sprite->lifetimeMs;
    projectile->addComponent<Projectile>(p);
    return projectile;
}
