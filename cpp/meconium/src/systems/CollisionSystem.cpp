#include "Context.h"
#include "ECS.h"
#include "systems/DebugSystem.h"

void CollisionSystem::update(const std::shared_ptr<Entities>& entities, TileMap& tileMap) {
    for (auto& entity : *entities) {
        auto transform = entity->getComponent<Transform>();
        auto velocity = entity->getComponent<Velocity>();
        auto collider = entity->getComponent<Collider>();

        if (!transform || !velocity || !collider)
            continue;

        // Save original position
        int originalX = transform->x;
        int originalY = transform->y;

        SDL_Rect currentRect = collider->getBounds(transform);

        // Future collision rect based on velocity
        SDL_Rect futureRect = {
            currentRect.x + velocity->vx, currentRect.y + velocity->vy, currentRect.w, currentRect.h};

        resolveTileCollisions(futureRect, velocity, transform, collider, tileMap);

        // If position hasn't changed, apply velocity directly
        if (transform->x == originalX && transform->y == originalY) {
            transform->x += velocity->vx;
            transform->y += velocity->vy;
        }

        // Respawn if fallen
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

void CollisionSystem::resolveTileCollisions(SDL_Rect& rect,
                                            std::shared_ptr<Velocity>& velocity,
                                            std::shared_ptr<Transform>& transform,
                                            std::shared_ptr<Collider>& collider,
                                            TileMap& tileMap) {

    // not on the ground until we can prove it
    transform->onGround = false;

    // find the rectangle we need to search for tiles

    int startX = (rect.x - 2) / tileMap.tileWidth();
    int endX = (rect.x + rect.w + 2) / tileMap.tileWidth();
    int startY = (rect.y - 2) / tileMap.tileHeight();
    int endY = (rect.y + rect.h + 2) / tileMap.tileHeight();

    DebugSystem::rectToDraw = {
        startX * tileMap.tileWidth(),
        startY * tileMap.tileHeight(),
        (endX - startX + 1) * tileMap.tileWidth(),
        (endY - startY + 1) * tileMap.tileHeight()
    };

    // get all tiles next to the player up and down left and right
    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                continue;

            // get this tile and its type
            int tileID = tileMap.at(y, x);
            TileType type = tileMap.getTileType(tileID);

            // only look at solid tiles
            if (type == TileType::Solid) {
                SDL_Rect tileRect = {
                    x * tileMap.tileWidth(), y * tileMap.tileHeight(), tileMap.tileWidth(), tileMap.tileHeight()};

                bool horizontalOverlap = rect.x < tileRect.x + tileRect.w && rect.x + rect.w > tileRect.x;

                // check for vertical collisinos
                if (rect.y + rect.h >= tileRect.y && rect.y <= tileRect.y && horizontalOverlap) {
                    // collided vertically with this tile going downward
                    velocity->vy = 0;
                    transform->onGround = true;
                    transform->y = tileRect.y - (collider->offsetY + collider->height) * transform->scaleY;
                    return;
                } else if (velocity->vy < 0 && rect.y <= tileRect.y + tileRect.h && rect.y + rect.h >= tileRect.y && horizontalOverlap) {
                    // Collided vertically with this tile going upward
                    velocity->vy = 0;
                    transform->y = tileRect.y + tileRect.h - collider->offsetY * transform->scaleY;
                    return;
                }
            }
        }
    }
}
void CollisionSystem::handleSolidCollision(SDL_Rect& rect,
                                           std::shared_ptr<Velocity>& velocity,
                                           std::shared_ptr<Transform>& transform,
                                           std::shared_ptr<Collider>& collider,
                                           SDL_Rect tileRect,
                                           SDL_Rect& intersection) {
    transform->onGround = true;
    // transform->x = tileRect.x;
    transform->y = tileRect.y - (collider->offsetY + collider->height) * transform->scaleY;
    // velocity->vx = 0;
    velocity->vy = 0;
}
