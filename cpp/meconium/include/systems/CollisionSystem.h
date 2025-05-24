#pragma once

#include "components/Collider.h"
#include "components/TileMap.h"
#include "components/Velocity.h"
#include "entity/Entity.h"

class CollisionSystem {
public:
    void update(const std::shared_ptr<Entities>& entities, TileMap& tileMap);

private:
    void resolveHorizontalCollisions(SDL_Rect& rect,
                                     std::shared_ptr<Velocity>& velocity,
                                     std::shared_ptr<Transform>& transform,
                                     std::shared_ptr<Collider>& collider,
                                     TileMap& tileMap);

    void resolveVerticalCollisions(SDL_Rect& rect,
                                   std::shared_ptr<Velocity>& velocity,
                                   std::shared_ptr<Transform>& transform,
                                   std::shared_ptr<Collider>& collider,
                                   TileMap& tileMap);

    void resolvePlayerEnemyBump(Entity& player, Entity& enemy);

    static void forEachNearbySolidTile(
        const SDL_Rect& rect,
        const TileMap& tileMap,
        const std::function<void(const SDL_Rect& tileRect, int tileX, int tileY, TileType type)>& callback);
};
