#include "ECS.h"
#include "components/Attack.h"
#include "components/Despawn.h"
#include "components/Health.h"
#include "components/Knockback.h"
#include "components/SoundEffect.h"
#include "components/State.h"

void CollisionSystem::update(const std::shared_ptr<Entities>& entities, TileMap& tileMap) {
    auto player = entities->findEntityWithComponent<PlayerTag>();

    for (auto& entity : *entities) {
        auto transform = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto collider = entity->getComponent<Collider>();

        if (!transform || !velocity || !collider)
            continue;

        // Horizontal collision pass
        SDL_Rect boundsX = collider->getBounds(transform);
        boundsX.x += velocity->vx;
        resolveHorizontalCollisions(boundsX, velocity, transform, collider, tileMap);

        // Vertical collision pass
        // refresh bounds after possible X change
        SDL_Rect boundsY = collider->getBounds(transform);
        boundsY.y += velocity->vy;
        resolveVerticalCollisions(boundsY, velocity, transform, collider, tileMap);

        // Check player vs enemy collisions
        if (entity->hasComponent<EnemyTag>()) {
            resolvePlayerEnemyCollisions(*player, *entity);
        }

        // Check player vs projectile collisions
        if (entity->hasComponent<Projectile>()) {
            resolvePlayerProjectileCollisions(*player, *entity);
        }

        // fall of map check
        if (transform->y > tileMap.mapHeight * tileMap.tileHeight() * 2) {
            std::cout << "fell off" << std::endl;
            transform->x = 100;
            transform->y = 100;
            velocity->vx = 0;
            velocity->vy = 0;
            transform->onGround = false;
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

void CollisionSystem::resolveHorizontalCollisions(SDL_Rect& rect,
                                                  std::shared_ptr<Velocity>& velocity,
                                                  std::shared_ptr<Transform>& transform,
                                                  std::shared_ptr<Collider>& collider,
                                                  TileMap& tileMap) {

    forEachNearbySolidTile(rect, tileMap, [&](const SDL_Rect& tileRect, int x, int y, TileType type) {
        if (type != TileType::Solid) {
            return;
        }
        // confirm vertical overlap
        if (rect.y + rect.h > tileRect.y && rect.y < tileRect.y + tileRect.h) {
            // now check X direction
            if (velocity->vx > 0 && rect.x + rect.w > tileRect.x && rect.x < tileRect.x) {
                // moving right
                velocity->vx = 0;
                transform->x = tileRect.x - (collider->offsetX + collider->width) * transform->scaleX;
            } else if (velocity->vx < 0 && rect.x < tileRect.x + tileRect.w &&
                       rect.x + rect.w > tileRect.x + tileRect.w) {
                // moving left
                velocity->vx = 0;
                transform->x = tileRect.x + tileRect.w - collider->offsetX * transform->scaleX;
            }
        }
    });
    transform->x += velocity->vx;
}

void CollisionSystem::resolveVerticalCollisions(SDL_Rect& rect,
                                                std::shared_ptr<Velocity>& velocity,
                                                std::shared_ptr<Transform>& transform,
                                                std::shared_ptr<Collider>& collider,
                                                TileMap& tileMap) {
    transform->onGround = false;
    forEachNearbySolidTile(rect, tileMap, [&](const SDL_Rect& tileRect, int x, int y, TileType type) {
        // confirm horizontal overlap confirmed
        if (rect.x + rect.w > tileRect.x && rect.x < tileRect.x + tileRect.w) {
            // first check solids
            if (type == TileType::Solid) {
                // now check Y direction
                if (velocity->vy >= 0 && rect.y + rect.h >= tileRect.y && rect.y < tileRect.y) {
                    // moving down
                    velocity->vy = 0;
                    transform->y = tileRect.y - (collider->offsetY + collider->height) * transform->scaleY;
                    transform->onGround = true;
                } else if (velocity->vy < 0 && rect.y < tileRect.y + tileRect.h &&
                           rect.y + rect.h > tileRect.y + tileRect.h) {
                    // moving up
                    velocity->vy = 0;
                    transform->y = tileRect.y + tileRect.h - collider->offsetY * transform->scaleY;
                }
            } else if (type == TileType::RampLeft || type == TileType::RampRight) {
                // Ramp handling (left and right)

                // Middle of the player
                int localX = rect.x + rect.w / 2 - tileRect.x;

                // Calculate percent across tile
                float percent = static_cast<float>(localX) / tileRect.w;

                int rampY = 0;

                if (type == TileType::RampLeft) {
                    rampY = tileRect.y + tileRect.h - static_cast<int>(tileRect.h * percent);
                } else if (type == TileType::RampRight) {
                    rampY = tileRect.y + static_cast<int>(tileRect.h * percent);
                }
                if (velocity->vy >= 0 && rect.y + rect.h >= rampY && rect.y < rampY) {
                    transform->y = rampY - (collider->offsetY + collider->height) * transform->scaleY;
                    velocity->vy = 0;
                    transform->onGround = true;
                }
            }
        }
    });

    transform->y += velocity->vy;
}

void CollisionSystem::forEachNearbySolidTile(
    const SDL_Rect& rect,
    const TileMap& tileMap,
    const std::function<void(const SDL_Rect& tileRect, int tileX, int tileY, TileType type)>& callback) {
    int startX = (rect.x - 2) / tileMap.tileWidth();
    int endX = (rect.x + rect.w + 2) / tileMap.tileWidth();
    int startY = (rect.y - 2) / tileMap.tileHeight();
    int endY = (rect.y + rect.h + 2) / tileMap.tileHeight();

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                continue;

            int tileID = tileMap.at(y, x);
            TileType type = tileMap.getTileType(tileID);

            // Only consider solid tiles or ramps
            if (type != TileType::Solid && type != TileType::RampLeft && type != TileType::RampRight)
                continue;

            SDL_Rect tileRect = {
                x * tileMap.tileWidth(), y * tileMap.tileHeight(), tileMap.tileWidth(), tileMap.tileHeight()};

            callback(tileRect, x, y, type);
        }
    }
}

// check for entity collisions and apply knockback if needed
void CollisionSystem::resolvePlayerEnemyCollisions(Entity& player, Entity& enemy) {
    if (player.hasComponent<Knockback>()) {
        return;
    }
    auto playerPos = player.getComponent<Transform>();
    auto playerCollider = player.getComponent<Collider>();
    auto enemyPos = enemy.getComponent<Transform>();
    auto enemyCollider = enemy.getComponent<Collider>();

    auto playerRect = playerCollider->getBounds(playerPos);
    auto enemyRect = enemyCollider->getBounds(enemyPos);

    if (aabb(playerRect, enemyRect)) {

        // check for attacks
        if (auto state = player.getComponent<State>()) {
            // player is currently attacking
            if (state->currentAction == Action::ATTACKING) {
                // player has health
                if (const auto health = enemy.getComponent<Health>()) {
                    health->hp -= 20; // TODO: define damage in prefab
                    // player should be despawned for dying
                    if (health->hp <= 0) {
                        enemy.getComponent<State>()->currentAction = Action::DYING;
                        enemy.addComponent<Despawn>(5000);
                    }
                }
            }
        }

        // no attacks so knockback
        // direction: enemy on left => knock right, etc.
        float dx = (playerRect.x + playerRect.w / 2) - (enemyRect.x + enemyRect.w / 2);

        float knockbackX = (dx >= 0) ? 3.0f : -3.0f; // Pixels per second
        float knockbackY = -2.0f;                    // upward knockback
        auto playerVel = player.getComponent<Velocity>();
        playerVel->vx = knockbackX;
        playerVel->vy = knockbackY;

        auto enemyVel = enemy.getComponent<Velocity>();
        enemyVel->vx = -knockbackX;
        enemyVel->vy = knockbackY;

        player.addComponent<Knockback>(200.0);
        enemy.addComponent<Knockback>(200.0);
        player.addComponent<SoundEffect>("bump", 0);
    }
}

void CollisionSystem::resolvePlayerProjectileCollisions(Entity& player, Entity& projectile) {
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
    if (aabb(playerRect, projRect)) {
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

bool CollisionSystem::aabb(SDL_Rect& a, SDL_Rect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y;
}
