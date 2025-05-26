
#include "systems/CombatSystem.h"
#include "Utils.h"
#include "components/Attack.h"
#include "components/Collider.h"
#include "components/Despawn.h"
#include "components/Health.h"
#include "components/Knockback.h"
#include "components/State.h"
#include "components/Tag.h"
#include "components/Transform.h"
#include "components/Velocity.h"

void CombatSystem::update(const std::shared_ptr<Entities>& entities) {

    auto player = entities->findEntityWithComponent<PlayerTag>();

    for (auto& entity : entities->filtered<Transform, Collider>()) {
        auto transform = entity->getComponent<Transform>();
        auto collider = entity->getComponent<Collider>();

        // Check player vs enemy collisions
        if (entity->hasComponent<EnemyTag>()) {
            resolvePlayerEnemyCollisions(*player, *entity);
        }

        // Check player vs projectile collisions
        if (entity->hasComponent<Projectile>()) {
            resolvePlayerProjectileCollisions(*player, *entity);
        }
    }

    // check for enemy vs player projectile collisions
    // get all projectiles
    auto projectiles = entities->findByComponents<Projectile>();
    for (auto projectile : projectiles) {
        auto proj = projectile->getComponent<Projectile>();
        // we already resolved enemy projectiles on the player deal with the player's
        if (proj->ownerId == player->id) {
            auto enemies = entities->findByComponents<EnemyTag>();
            for (auto enemy : enemies) {
                resolvePlayerProjectileCollisions(*enemy, *projectile);
            }
        }
    }
}

// check for entity collisions and apply knockback if needed
void CombatSystem::resolvePlayerEnemyCollisions(Entity& player, Entity& enemy) {

    // cant attack when we're getting knocked back
    if (player.hasComponent<Knockback>()) {
        return;
    }

    auto playerPos = player.getComponent<Transform>();

    // determine the player's hitbox
    // first check for a defined Hitbox component falling back to its collider
    SDL_Rect playerRect;
    if (auto hitbox = player.getComponent<Hitbox>()) {
        playerRect = hitbox->collider.getBounds(playerPos);
    } else if (auto collider = player.getComponent<Collider>()) {
        // using the collider as a fallback hitbox
        playerRect = collider->getBounds(playerPos);

        if (auto velocity = player.getComponent<Velocity>()) {
            // if we're falling back to bad collider hitbox, apply a lookahead
            playerRect.x += velocity->vx;
            playerRect.y += velocity->vy;
        }
    } else {
        std::cerr << "no player hitbox or collider\n";
        return;
    }

    auto enemyPos = enemy.getComponent<Transform>();
    auto enemyCollider = enemy.getComponent<Collider>();

    auto enemyRect = enemyCollider->getBounds(enemyPos);

    if (util::aabb(playerRect, enemyRect)) {

        // check for attacks
        if (auto state = player.getComponent<State>()) {
            // player is currently attacking
            if (state->currentAction == Action::ATTACKING && !state->actionApplied) {
                // player has health
                auto attack = player.getComponent<Attack>();
                if (const auto health = enemy.getComponent<Health>()) {
                    health->hp -= attack->damage;

                    // apply knockback
                    // direction: enemy on left => knock right, etc.
                    float dx = (playerRect.x + static_cast<int>(playerRect.w / 2)) -
                               (enemyRect.x + static_cast<int>(enemyRect.w / 2));

                    float knockbackX = (dx >= 0) ? 1.0f : -1.0f; // Pixels per second

                    auto enemyVel = enemy.getComponent<Velocity>();
                    enemyVel->vx = -knockbackX;

                    enemy.addComponent<Knockback>(200.0);

                    // enemy should be despawned for dying
                    if (health->hp <= 0) {
                        enemy.getComponent<State>()->currentAction = Action::DYING;
                        enemy.addComponent<Despawn>(5000);
                    }
                    state->actionApplied = true;
                }
            }
        }
    }
}

void CombatSystem::resolvePlayerProjectileCollisions(Entity& player, Entity& projectile) {

    auto proj = projectile.getComponent<Projectile>();
    if (!proj)
        return;

    // player cant shoot themselves
    if (proj->ownerId == player.id) {
        return;
    }

    auto playerPos = player.getComponent<Transform>();
    auto projPos = projectile.getComponent<Transform>();

    auto playerCollider = player.getComponent<Collider>();
    auto projCollider = projectile.getComponent<Collider>();

    auto playerRect = playerCollider->getBounds(playerPos);
    auto projRect = projCollider->getBounds(projPos);

    // did the projectile hit the player
    if (util::aabb(playerRect, projRect)) {
        if (auto state = player.getComponent<State>()) {
            auto health = player.getComponent<Health>();
            if (health && proj) {
                health->hp -= proj->damage;
                if (health->hp <= 0) {
                    state->lockAction(Action::DYING, 5000);
                    player.addComponent<Despawn>(5000);
                }
            }
        }
        projectile.addComponent<Despawn>(0);
    }
}
