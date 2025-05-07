#pragma once

class CollisionSystem {
    public:
        void update(std::vector<std::shared_ptr<Entity>>& entities, TileMap& tileMap) {
            for (auto& entity : entities) {
                auto position = entity->getComponent<Position>();
                auto velocity = entity->getComponent<Velocity>();
                auto size     = entity->getComponent<Size>();
    
                if (!position || !velocity || !size) continue;
    
                SDL_Rect futureRect = {
                    position->x + velocity->vx,
                    position->y + velocity->vy,
                    size->width,
                    size->height
                };
    
                resolveTileCollisions(futureRect, velocity, position, size, tileMap);
            }
        }
    
    private:
        void resolveTileCollisions(SDL_Rect& rect, std::shared_ptr<Velocity>& velocity,
                                   std::shared_ptr<Position>& position, std::shared_ptr<Size>& size,
                                   TileMap& tileMap) {
            // Compute tile indices the player overlaps
            int startX = rect.x / tileMap.tileSize;
            int endX = (rect.x + rect.w - 1) / tileMap.tileSize;
            int startY = rect.y / tileMap.tileSize;
            int endY = (rect.y + rect.h - 1) / tileMap.tileSize;
    
            position->onGround = false;
    
            for (int y = startY; y <= endY; ++y) {
                for (int x = startX; x <= endX; ++x) {
                    if (y < 0 || x < 0 || y >= tileMap.mapHeight || x >= tileMap.mapWidth)
                        continue;
    
                    int tileID = tileMap.at(y, x);
                    if (!tileMap.isSolidTile(tileID)) continue;
    
                    SDL_Rect tileRect = {
                        x * tileMap.tileSize,
                        y * tileMap.tileSize,
                        tileMap.tileSize,
                        tileMap.tileSize
                    };
    
                    SDL_Rect intersection;
                    if (SDL_IntersectRect(&rect, &tileRect, &intersection)) {
                        // Simple resolution: vertical first
                        if (intersection.w >= intersection.h) {
                            // Vertical collision
                            if (velocity->vy > 0) { // Falling
                                position->y = tileRect.y - size->height;
                                velocity->vy = 0;
                                position->onGround = true;
                            } else if (velocity->vy < 0) { // Jumping
                                position->y = tileRect.y + tileMap.tileSize;
                                velocity->vy = 0;
                            }
                        } else {
                            // Horizontal collision
                            if (velocity->vx > 0) { // Moving right
                                position->x = tileRect.x - size->width;
                            } else if (velocity->vx < 0) { // Moving left
                                position->x = tileRect.x + tileMap.tileSize;
                            }
                            velocity->vx = 0;
                        }
                    }
                }
            }
        }
    };
    