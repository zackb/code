#include "ECS.h"
#include "systems/DebugSystem.h"

void CollisionSystem::update(const std::shared_ptr<Entities>& entities, TileMap& tileMap) {
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
}

void CollisionSystem::resolveHorizontalCollisions(SDL_Rect& rect,
                                                  std::shared_ptr<Velocity>& velocity,
                                                  std::shared_ptr<Transform>& transform,
                                                  std::shared_ptr<Collider>& collider,
                                                  TileMap& tileMap) {

    forEachNearbySolidTile(rect, tileMap, [&](const SDL_Rect& tileRect, int x, int y) {
        if (rect.y + rect.h > tileRect.y && rect.y < tileRect.y + tileRect.h) {
            // vertical overlap confirmed, now check X direction
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
    forEachNearbySolidTile(rect, tileMap, [&](const SDL_Rect& tileRect, int x, int y) {
        if (rect.x + rect.w > tileRect.x && rect.x < tileRect.x + tileRect.w) {
            // horizontal overlap confirmed, now check Y direction
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
        }
    });

    transform->y += velocity->vy;
}

void CollisionSystem::forEachNearbySolidTile(
    const SDL_Rect& rect,
    const TileMap& tileMap,
    const std::function<void(const SDL_Rect& tileRect, int tileX, int tileY)>& callback) {
    int startX = (rect.x - 2) / tileMap.tileWidth();
    int endX = (rect.x + rect.w + 2) / tileMap.tileWidth();
    int startY = (rect.y - 2) / tileMap.tileHeight();
    int endY = (rect.y + rect.h + 2) / tileMap.tileHeight();

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                continue;

            int tileID = tileMap.at(y, x);
            if (tileMap.getTileType(tileID) != TileType::Solid)
                continue;

            SDL_Rect tileRect = {
                x * tileMap.tileWidth(), y * tileMap.tileHeight(), tileMap.tileWidth(), tileMap.tileHeight()};

            callback(tileRect, x, y);
        }
    }
}
