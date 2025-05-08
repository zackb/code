#pragma once
#include "Position.h"
#include "Size.h"
#include "TileMap.h"
#include "Velocity.h"

class CollisionSystem {
public:
    void update(std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap);

private:
    void resolveTileCollisions(SDL_Rect& rect,
                               std::shared_ptr<Velocity>& velocity,
                               std::shared_ptr<Position>& position,
                               std::shared_ptr<Size>& size,
                               TileMap& tileMap);

    void handleAllRampCollisions(std::shared_ptr<Position>& position,
                                 std::shared_ptr<Velocity>& velocity,
                                 std::shared_ptr<Size>& size,
                                 SDL_Rect& rect,
                                 int startX,
                                 int endX,
                                 int startY,
                                 int endY,
                                 TileMap& tileMap);

    float calculateRampY(std::shared_ptr<Position>& position,
                         std::shared_ptr<Velocity>& velocity,
                         std::shared_ptr<Size>& size,
                         TileType rampType,
                         SDL_Rect tileRect,
                         TileMap& tileMap);

    void handleSolidCollision(SDL_Rect& rect,
                              std::shared_ptr<Velocity>& velocity,
                              std::shared_ptr<Position>& position,
                              std::shared_ptr<Size>& size,
                              SDL_Rect tileRect,
                              SDL_Rect& intersection);

    bool isGrounded(const Position& position, const Size& size, const TileMap& tileMap) const;
};
