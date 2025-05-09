#include "ecs/CollisionSystem.h"

#include "Context.h"
#include "ecs/ECS.h"

void CollisionSystem::update(std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap) {
    for (auto& entity : entities) {
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

        // Grounded check
        transform->onGround = isGrounded(*transform, *collider, tileMap);

        // Respawn if fallen
        if (transform->y > tileMap.mapHeight * tileMap.tileSize * 2) {
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
    int startX = (rect.x - 2) / tileMap.tileSize;
    int endX = (rect.x + rect.w + 2) / tileMap.tileSize;
    int startY = (rect.y - 2) / tileMap.tileSize;
    int endY = (rect.y + rect.h + 2) / tileMap.tileSize;

    transform->onGround = false;

    handleAllRampCollisions(transform, velocity, collider, rect, startX, endX, startY, endY, tileMap);

    if (!transform->onGround) {
        for (int y = startY; y <= endY; ++y) {
            for (int x = startX; x <= endX; ++x) {
                if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                    continue;

                int tileID = tileMap.at(y, x);
                TileType type = tileMap.getTileType(tileID);

                if (type == TileType::Solid) {
                    SDL_Rect tileRect = {
                        x * tileMap.tileSize, y * tileMap.tileSize, tileMap.tileSize, tileMap.tileSize};

                    SDL_Rect intersection;
                    if (SDL_IntersectRect(&rect, &tileRect, &intersection)) {
                        handleSolidCollision(rect, velocity, transform, collider, tileRect, intersection);
                    }
                }
            }
        }
    }
}

void CollisionSystem::handleAllRampCollisions(std::shared_ptr<Transform>& transform,
                                              std::shared_ptr<Velocity>& velocity,
                                              std::shared_ptr<Collider>& collider,
                                              SDL_Rect& rect,
                                              int startX,
                                              int endX,
                                              int startY,
                                              int endY,
                                              TileMap& tileMap) {
    float bestRampY = std::numeric_limits<float>::max();
    bool foundRamp = false;

    startX = std::max(0, startX - 1);
    endX = std::min(tileMap.mapWidth - 1, endX + 1);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                continue;

            int tileID = tileMap.at(y, x);
            TileType type = tileMap.getTileType(tileID);

            if (type == TileType::RampLeft || type == TileType::RampRight) {
                SDL_Rect tileRect = {x * tileMap.tileSize, y * tileMap.tileSize, tileMap.tileSize, tileMap.tileSize};

                float rampY = calculateRampY(transform, collider, type, tileRect, tileMap);

                SDL_Rect bounds = collider->getBounds(transform);
                int playerLeft = bounds.x;
                int playerRight = bounds.x + bounds.w;
                bool horizontalOverlap = (playerRight > tileRect.x && playerLeft < tileRect.x + tileRect.w);

                int playerFeet = bounds.y + bounds.h;
                if (horizontalOverlap && playerFeet > rampY && playerFeet <= tileRect.y + tileRect.h + 5 &&
                    rampY < bestRampY) {
                    bestRampY = rampY;
                    foundRamp = true;
                }
            }
        }
    }

    if (foundRamp && velocity->vy >= 0) {
        SDL_Rect bounds = collider->getBounds(transform);
        transform->y = bestRampY - bounds.h;
        velocity->vy = 0;
        transform->onGround = true;
    }
}

float CollisionSystem::calculateRampY(std::shared_ptr<Transform>& transform,
                                      std::shared_ptr<Collider>& collider,
                                      TileType rampType,
                                      SDL_Rect tileRect,
                                      TileMap& tileMap) {
    SDL_Rect bounds = collider->getBounds(transform);
    int playerMidX = bounds.x + bounds.w / 2;
    int relX = playerMidX - tileRect.x;

    relX = std::max(0, std::min(relX, tileMap.tileSize - 1));

    float rampHeight = (rampType == TileType::RampRight) ? tileMap.tileSize - relX : relX;

    return tileRect.y + tileMap.tileSize - rampHeight;
}

void CollisionSystem::handleSolidCollision(SDL_Rect& rect,
                                           std::shared_ptr<Velocity>& velocity,
                                           std::shared_ptr<Transform>& transform,
                                           std::shared_ptr<Collider>& collider,
                                           SDL_Rect tileRect,
                                           SDL_Rect& intersection) {
    SDL_Rect bounds = collider->getBounds(transform);

    if (intersection.h <= intersection.w) {
        if (rect.y + rect.h - intersection.h <= tileRect.y) {
            transform->y = tileRect.y - bounds.h;
            velocity->vy = 0;
            transform->onGround = true;
        } else if (rect.y >= tileRect.y + tileRect.h - intersection.h) {
            transform->y = tileRect.y + tileRect.h;
            velocity->vy = 0;
        }
    } else if (intersection.w < intersection.h) {
        if (intersection.w >= 4) {
            if (rect.x + rect.w - intersection.w <= tileRect.x) {
                transform->x = tileRect.x - bounds.w;
                velocity->vx = 0;
            } else if (rect.x >= tileRect.x + tileRect.w - intersection.w) {
                transform->x = tileRect.x + tileRect.w;
                velocity->vx = 0;
            }
        }
    }
}

bool CollisionSystem::isGrounded(const Transform& transform, const Collider& collider, const TileMap& tileMap) const {
    auto transformPtr = std::make_shared<Transform>(transform);
    SDL_Rect bounds = collider.getBounds(transformPtr);

    SDL_Rect feetRect = {bounds.x, bounds.y + bounds.h, bounds.w, 2};

    int startX = feetRect.x / tileMap.tileSize;
    int endX = (feetRect.x + feetRect.w - 1) / tileMap.tileSize;
    int startY = feetRect.y / tileMap.tileSize;
    int endY = (feetRect.y + feetRect.h - 1) / tileMap.tileSize;

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (x < 0 || y < 0 || x >= tileMap.mapWidth || y >= tileMap.mapHeight)
                continue;

            int tileID = tileMap.at(y, x);
            TileType type = tileMap.getTileType(tileID);

            if (type == TileType::Solid || type == TileType::RampLeft || type == TileType::RampRight)
                return true;
        }
    }

    return false;
}