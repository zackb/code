#pragma once

#include "components/Collider.h"
#include "components/TileMap.h"
#include "components/Velocity.h"
#include "entity/Entity.h"

class CollisionSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, TileMap& tileMap);
    void resolveHorizontalCollisions(SDL_Rect& rect,
                                     std::shared_ptr<Velocity>& velocity,
                                     std::shared_ptr<Transform>& transform,
                                     std::shared_ptr<Collider>& collider,
                                     TileMap& tileMap);

private:
    void resolveVerticalCollisions(SDL_Rect& rect,
                                   std::shared_ptr<Velocity>& velocity,
                                   std::shared_ptr<Transform>& transform,
                                   std::shared_ptr<Collider>& collider,
                                   TileMap& tileMap);
    void resolveTileCollisions(SDL_Rect& rect,
                               std::shared_ptr<Velocity>& velocity,
                               std::shared_ptr<Transform>& transform,
                               std::shared_ptr<Collider>& collider,
                               TileMap& tileMap);

    void handleAllRampCollisions(std::shared_ptr<Transform>& transform,
                                 std::shared_ptr<Velocity>& velocity,
                                 std::shared_ptr<Collider>& collider,
                                 SDL_Rect& rect,
                                 int startX,
                                 int endX,
                                 int startY,
                                 int endY,
                                 TileMap& tileMap);
    void handleSolidCollision(SDL_Rect& rect,
                              std::shared_ptr<Velocity>& velocity,
                              std::shared_ptr<Transform>& transform,
                              std::shared_ptr<Collider>& collider,
                              SDL_Rect tileRect,
                              SDL_Rect& intersection);

    float calculateRampY(std::shared_ptr<Transform>& transform,
                         std::shared_ptr<Collider>& collider,
                         TileType rampType,
                         SDL_Rect tileRect,
                         TileMap& tileMap);

    bool isGrounded(const Transform& transform, const Collider& collider, const TileMap& tileMap) const;
};
